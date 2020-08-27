
#include "modbus.h"


uint8_t modbusMaskRead( const uint8_t *mask, uint16_t maskLength, uint16_t bit )
{
    //Return nth bit from uint8_t array

    if ( mask == NULL ) return 255;
    if ( ( bit >> 3 ) >= maskLength ) return 255;
    return ( mask[bit >> 3] & ( 1 << ( bit % 8 ) ) ) >> ( bit % 8 );
}

uint8_t modbusMaskWrite( uint8_t *mask, uint16_t maskLength, uint16_t bit, uint8_t value )
{
    //Write nth bit in uint8_t array

    if ( mask == NULL ) return 255;
    if ( ( bit >> 3 ) >= maskLength ) return 255;
    if ( value )
        mask[bit >> 3] |= ( 1 << ( bit % 8 ) );
    else
        mask[bit >> 3] &= ~( 1 << ( bit % 8 ) );
    return 0;
}

uint16_t modbusCRC( const uint8_t *data, uint16_t length )
{
    //Calculate CRC16 checksum using given data and length

    uint16_t crc = 0xFFFF;
    uint16_t i;
    uint8_t j;

    if ( data == NULL ) return 0;

    for ( i = 0; i < length; i++ )
    {
        crc ^= (uint16_t) data[i]; //XOR current data byte with crc value

        for ( j = 8; j != 0; j-- )
        {
            //For each bit
            //Is least-significant-bit set?
            if ( ( crc & 0x0001 ) != 0 )
            {
                crc >>= 1; //Shift to right and xor
                crc ^= 0xA001;
            }
            else
                crc >>= 1;
        }
    }

    return modbusSwapEndian( crc );
}

/************************** master **************************/
ModbusError_e modbusParseException( ModbusMaster *status, ModbusParser *parser )
{
    //Parse exception frame and write data to MODBUSMaster structure

    //Check if given pointers are valid
    if ( status == NULL || parser == NULL ) return MODBUS_ERROR_NULLPTR;

    //Copy data (modbusParseResponse checked if length is 5 so it should be safe)
    status->exception.address = parser->exception.address;
    status->exception.function = parser->exception.function;
    status->exception.code = parser->exception.code;

    return MODBUS_ERROR_EXCEPTION;
}

ModbusError_e modbusParseResponse( ModbusMaster *status )
{

    uint8_t err = 0;

    //Check if given pointer is valid
    if ( status == NULL ) return MODBUS_ERROR_NULLPTR;

    //Reset output registers before parsing frame
    status->exception.address = 0;
    status->exception.function = 0;
    status->exception.code = 0;

    #if !defined(ARCH_USED_STATIC_MEM)
        free( status->data.coils );
        status->data.coils = NULL;
        status->data.regs = NULL;
    #endif
    status->data.length = 0;
    status->data.index = 0;
    status->data.count = 0;
    status->data.type = 0;
    status->data.address = 0;
    status->data.function = 0;

    //Check if frames are not too short and return error (to avoid problems with memory allocation)
    //That enables us to omit the check in each parsing function
    if ( status->response.length < 4u || status->response.frame == NULL || \
        status->request.length < 4u || status->request.frame == NULL )
        {
            status->parseError = MODBUS_FERROR_LENGTH;
            return MODBUS_ERROR_PARSE;
        }

    //Check both response and request frames CRC
    //The CRC of the frames are copied to a variable in order to avoid an unaligned memory access,
    //which can cause runtime errors in some platforms like AVR and ARM.
    uint16_t crcresp;
    uint16_t crcreq;

    memcpy(&crcresp, status->response.frame + status->response.length - 2, 2);
    memcpy(&crcreq,	 status->request.frame	+ status->request.length  - 2, 2);

    if ( crcresp != modbusCRC( status->response.frame, status->response.length - 2 ) ||
         crcreq	 != modbusCRC( status->request.frame,  status->request.length  - 2 ) )
    {
        status->parseError = MODBUS_FERROR_CRC;
        return MODBUS_ERROR_PARSE;
    }

    ModbusParser *parser = (ModbusParser*) status->response.frame;
    ModbusParser *requestParser = (ModbusParser*) status->request.frame;

    uint8_t functionMatch = 0, functionExec = 0;
    status->parseError = MODBUS_OK;

    //Check user defined functions
    #ifdef ARCH_USED_USER_FUNC
        if ( status->userFunctions != NULL )
        {
            uint16_t i;
            for ( i = 0; i < status->userFunctionCount; i++ )
            {
                if ( status->userFunctions[i].function == parser->base.function )
                {
                    functionMatch = 1;

                    //If the function is overriden and handler pointer is valid, user the callback
                    if ( status->userFunctions[i].handler != NULL )
                    {
                        err = status->userFunctions[i].handler( status, parser, requestParser );
                        functionExec = 1;
                    }
                    else
                        functionExec = 0; //Function overriden, but pointer is invalid

                    //Search till first match
                    break;
                }
            }
        }
    #endif

    if ( !functionMatch )
    {
        functionExec = 1;

        //Catching exceptions can be overriden by user functions
        if ( ( parser->base.function & 128 ) && status->response.length == 5 )
        {
            err = modbusParseException( status, parser );
        }
        else
        {
            switch ( parser->base.function )
            {

                    case 1: //Read multiple coils
                    case 2: //Read multiple discrete inputs
                        err = modbusParseResponse0102( status, parser, requestParser );
                        break;

                    case 3: //Read multiple holding registers
                    case 4: //Read multiple input registers
                        err = modbusParseResponse0304( status, parser, requestParser );
                        break;



                    case 5: //Write single coil
                        err = modbusParseResponse05( status, parser, requestParser );
                        break;



                    case 6: //Write single holding reg
                        err = modbusParseResponse06( status, parser, requestParser );
                        break;



                    case 15: //Write multiple coils
                        err = modbusParseResponse15( status, parser, requestParser );
                        break;



                    case 16: //Write multiple holding registers
                        err = modbusParseResponse16( status, parser, requestParser );
                        break;



                    case 22: //Mask write holding register
                        err = modbusParseResponse22( status, parser, requestParser );
                        break;


                default: //Function code not known by master
                    functionExec = 0;
                    break;
            }
        }
    }

    //Function not executed
    if ( !functionExec )
    {
        if ( functionMatch )
            status->parseError = MODBUS_FERROR_NULLFUN; //User override
        else
            status->parseError = MODBUS_FERROR_NOFUN; //Unsupported function

        return MODBUS_ERROR_PARSE;
    }


    return err;
}

ModbusError modbusMasterInit( ModbusMaster *status )
{
    //Check if given pointer is valid
    if ( status == NULL ) return MODBUS_ERROR_NULLPTR;

    //Very basic init of master side
    #ifndef ARCH_USED_STATIC_MEM
        status->request.frame = NULL;
    #else
        memset( status->request.frame, 0, REQUEST_STATIC_MEM_MAX_LEN );
    #endif
    status->request.length = 0;

    #ifndef ARCH_USED_STATIC_MEM
        status->response.frame = NULL;
    #else
        memset( status->response.frame, 0, RESPONSE_STATIC_MEM_MAX_LEN );
    #endif
    status->response.length = 0;

    #ifndef ARCH_USED_STATIC_MEM
        status->data.coils = NULL;
        status->data.regs = NULL;
    #else
        memset( status->data.coils, 0, DATA_STATIC_MEM_MAX_LEN );
    #endif
    status->data.length = 0;

    status->data.count = 0;
    status->data.index = 0;
    status->data.type = 0;
    status->data.address = 0;

    status->exception.address = 0;
    status->exception.function = 0;
    status->exception.code = 0;

    return MODBUS_ERROR_OK;
}



ModbusError_e modbusMasterEnd( ModbusMaster *status )
{
    //Check if given pointer is valid
    if ( status == NULL ) return MODBUS_ERROR_NULLPTR;

    //Free memory
    #ifndef ARCH_USED_STATIC_MEM
        free( status->request.frame );
        status->request.frame = NULL;

        free( status->data.coils );
        status->data.coils = NULL;
        status->data.regs = NULL;
    #endif

    return MODBUS_ERROR_OK;
}


ModbusError_e modbusBuildException( ModbusSlave *status, uint8_t function, ModbusExceptionCode code )
{
    //Generates modbus exception frame in allocated memory frame
    //Returns generated frame length

    //Check if given pointer is valid
    if ( status == NULL ) return MODBUS_ERROR_NULLPTR;

    //Setup 'last exception' in slave struct
    status->lastException = code;

    //If request is broadcasted, do not form exception frame
    ModbusParser *requestParser = (ModbusParser*) status->request.frame;
    if ( requestParser != NULL && requestParser->base.address == 0 )
    {
        status->response.length = 0;
        return MODBUS_OK;
    }

    #ifndef ARCH_USED_STATIC_MEM
        //Reallocate frame memory
        status->response.frame = (uint8_t *) calloc( 5, sizeof( uint8_t ) );
        if ( status->response.frame == NULL ) return MODBUS_ERROR_ALLOC;
    #else
        if ( 5 * sizeof( uint8_t ) > LIGHTMODBUS_STATIC_MEM_SLAVE_RESPONSE ) return MODBUS_ERROR_ALLOC;
    #endif

    ModbusParser *exception = (ModbusParser *) status->response.frame;

    //Setup exception frame
    exception->exception.address = status->address;
    exception->exception.function = ( 1 << 7 ) | function;
    exception->exception.code = code;
    exception->exception.crc = modbusCRC( exception->frame, 3 );

    //Set frame length - frame is ready
    status->response.length = 5;

    //So, user should rather know, that master slave had to throw exception, right?
    //That's the reason exception should be thrown - just like that, an information
    return MODBUS_ERROR_EXCEPTION;
}


#ifdef LIGHTMODBUS_SLAVE_BASE
ModbusError modbusParseRequest( ModbusSlave *status )
{
    //Parse and interpret given modbus frame on slave-side
    uint8_t err = 0;

    //Check if given pointer is valid
    if ( status == NULL ) return MODBUS_ERROR_NULLPTR;

    //Reset response frame status
    status->response.length = 0;

    //If there is memory allocated for response frame - free it
    #ifndef LIGHTMODBUS_STATIC_MEM_SLAVE_RESPONSE
        free( status->response.frame );
        status->response.frame = NULL;
    #endif

    status->parseError = MODBUS_OK;

    //If user tries to parse an empty frame return error
    //That enables us to omit the check in each parsing function
    if ( status->request.length < 4u || status->request.frame == NULL )
    {
        status->parseError = MODBUS_FERROR_LENGTH;
        return MODBUS_ERROR_PARSE;
    }

    //Check CRC
    //The CRC of the frame is copied to a variable in order to avoid an unaligned memory access,
    //which can cause runtime errors in some platforms like AVR and ARM.
    uint16_t crc;

    memcpy(&crc, status->request.frame + status->request.length - 2, 2);

    if ( crc != modbusCRC( status->request.frame, status->request.length - 2 ) )
    {
        status->parseError = MODBUS_FERROR_CRC;
        return MODBUS_ERROR_PARSE;
    }


    ModbusParser *parser = (ModbusParser *) status->request.frame;

    //If frame is not broadcasted and address doesn't match skip parsing
    if ( parser->base.address != status->address && parser->base.address != 0 )
        return MODBUS_ERROR_OK;


    uint8_t functionMatch = 0, functionExec = 0;

    //Firstly, check user function array
    #ifdef LIGHTMODBUS_SLAVE_USER_FUNCTIONS
    if ( status->userFunctions != NULL )
    {
        uint16_t i;
        for ( i = 0; i < status->userFunctionCount; i++ )
        {
            if ( status->userFunctions[i].function == parser->base.function )
            {
                functionMatch = 1;

                //If the function is overriden and handler pointer is valid, user the callback
                if ( status->userFunctions[i].handler != NULL )
                {
                    err = status->userFunctions[i].handler( status, parser );
                    functionExec = 1;
                }
                else
                    functionExec = 0;

                //Search till first match
                break;
            }
        }
    }
    #endif

    if ( !functionMatch )
    {
        functionExec = 1;
        switch ( parser->base.function )
        {
            #if defined(LIGHTMODBUS_F01S) || defined(LIGHTMODBUS_F02S)
                case 1: //Read multiple coils
                case 2: //Read multiple discrete inputs
                    err = modbusParseRequest0102( status, parser );
                    break;
            #endif

            #if defined(LIGHTMODBUS_F03S) || defined(LIGHTMODBUS_F04S)
                case 3: //Read multiple holding registers
                case 4: //Read multiple input registers
                    err = modbusParseRequest0304( status, parser );
                    break;
            #endif

            #ifdef LIGHTMODBUS_F05S
                case 5: //Write single coil
                    err = modbusParseRequest05( status, parser );
                    break;
            #endif

            #ifdef LIGHTMODBUS_F06S
                case 6: //Write single holding reg
                    err = modbusParseRequest06( status, parser );
                    break;
            #endif

            #ifdef LIGHTMODBUS_F15S
                case 15: //Write multiple coils
                    err = modbusParseRequest15( status, parser );
                    break;
            #endif

            #ifdef LIGHTMODBUS_F16S
                case 16: //Write multiple holding registers
                    err = modbusParseRequest16( status, parser );
                    break;
            #endif

            #ifdef LIGHTMODBUS_F22S
                case 22: //Mask write single register
                    err = modbusParseRequest22( status, parser );
                    break;
            #endif

            default:
                err = MODBUS_OK;
                functionExec = 0;
                break;
        }
    }

    //Function did not execute
    if ( !functionExec )
    {
        if ( functionMatch ) //Matched but not executed
            err = modbusBuildExceptionErr( status, parser->base.function, MODBUS_EXCEP_ILLEGAL_FUNCTION, MODBUS_FERROR_NULLFUN ); //User override
        else
            err = modbusBuildExceptionErr( status, parser->base.function, MODBUS_EXCEP_ILLEGAL_FUNCTION, MODBUS_FERROR_NOFUN ); //No override, no support
    }

    return err;
}
#endif

#ifdef LIGHTMODBUS_SLAVE_BASE
ModbusError modbusSlaveInit( ModbusSlave *status )
{
    //Very basic init of slave side
    //User has to modify pointers etc. himself

    //Check if given pointer is valid
    if ( status == NULL ) return MODBUS_ERROR_NULLPTR;

    //Reset response frame status
    #ifndef LIGHTMODBUS_STATIC_MEM_SLAVE_REQUEST
        status->request.frame = NULL;
    #else
        memset( status->request.frame, 0, LIGHTMODBUS_STATIC_MEM_SLAVE_REQUEST );
    #endif
    status->request.length = 0;

    #ifndef LIGHTMODBUS_STATIC_MEM_SLAVE_RESPONSE
        status->response.frame = NULL;
    #else
        memset( status->response.frame, 0, LIGHTMODBUS_STATIC_MEM_SLAVE_RESPONSE );
    #endif
    status->response.length = 0;

    //Slave cannot have broadcast address
    if ( status->address == 0 )
        return MODBUS_ERROR_OTHER;

    //Some safety checks
    #ifdef LIGHTMODBUS_REGISTER_CALLBACK
        if ( status->registerCallback == NULL ) status->registerCount = status->inputRegisterCount = 0;
    #else
        if ( status->registerCount == 0 || status->registers == NULL )
        {
            status->registerCount = 0;
            status->registers = NULL;
        }

        if ( status->inputRegisterCount == 0 || status->inputRegisters == NULL )
        {
            status->inputRegisterCount = 0;
            status->inputRegisters = NULL;
        }
    #endif

    #ifdef LIGHTMODBUS_COIL_CALLBACK
        if ( status->registerCallback == NULL ) status->coilCount = status->discreteInputCount = 0;
    #else
        if ( status->coilCount == 0 || status->coils == NULL )
        {
            status->coilCount = 0;
            status->coils = NULL;
        }

        if ( status->discreteInputCount == 0 || status->discreteInputs == NULL )
        {
            status->discreteInputCount = 0;
            status->discreteInputs = NULL;
        }
    #endif

    return MODBUS_ERROR_OK;
}
#endif

#ifdef LIGHTMODBUS_SLAVE_BASE
ModbusError modbusSlaveEnd( ModbusSlave *status )
{
    //Check if given pointer is valid
    if ( status == NULL ) return MODBUS_ERROR_NULLPTR;

    //Free memory
    #ifndef LIGHTMODBUS_STATIC_MEM_SLAVE_RESPONSE
        free( status->response.frame );
        status->response.frame = NULL;
    #endif

    return MODBUS_ERROR_OK;
}
#endif
