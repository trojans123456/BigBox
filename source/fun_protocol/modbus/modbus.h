#ifndef __MODBUS_H
#define __MODBUS_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum modbusError
{
    MODBUS_ERROR_OK = 0, //!< No error
    /**
        \brief Indicates that slave had thrown an exception.

        This exception can be thrown either by master's parsing function
        (indicating incoming exception frame) or by slave's building function
        (indicating that some problem caused the slave to **build an exception frame**).

        \note This error code handles the superset of problems handled by \ref MODBUS_ERROR_PARSE.

        When thrown on slave side, check \ref ModbusSlave.lastException and \ref ModbusSlave.parseError
        for more information.
    */
    MODBUS_ERROR_EXCEPTION = 1,
    /**
        \brief Memory problem

        Either one of memory allocation functions returned NULL or
        fixed-size buffer is not big enough to fit the data (see \ref static-mem).
    */
    MODBUS_ERROR_ALLOC, //!< Memory allocation problem
    MODBUS_ERROR_OTHER, //!< Other reason causing the function to abort (eg. bad function parameter)
    MODBUS_ERROR_NULLPTR, //!< A NULL pointer provided as some crucial parameter
    /**
        Parsing error occurred - check \ref ModbusSlave.parseError

        \note This error code is returned instead of \ref MODBUS_ERROR_EXCEPTION
        when exception should have been thrown, but wasn't (eg. due to broadcasted
        request frame). These two error code should be treated similarly.
    */
    MODBUS_ERROR_PARSE,
    MODBUS_ERROR_BUILD, //!< Frame building error occurred - check \ref ModbusMaster.buildError
    MODBUS_OK = MODBUS_ERROR_OK, //!< No error. Alias of \ref MODBUS_ERROR_OK
} ModbusError_e;

typedef enum
{
    MODBUS_FERROR_OK = MODBUS_OK, //!< Modbus frame OK. No error.
    MODBUS_FERROR_CRC, //!< Invalid CRC
    MODBUS_FERROR_LENGTH, //!< Invalid frame length
    MODBUS_FERROR_COUNT, //!< Invalid declared data item count
    MODBUS_FERROR_VALUE, //!< Illegal data value (eg. when writing a single coil)
    MODBUS_FERROR_RANGE, //!< Invalid register range
    MODBUS_FERROR_NOSRC, //!< There's neither callback function nor value array provided for this data type
    MODBUS_FERROR_NOREAD, //!< No read access to at least one of requested regsiters
    MODBUS_FERROR_NOWRITE, //!< No write access to one of requested regsiters
    MODBUS_FERROR_NOFUN, //!< Function not supported
    MODBUS_FERROR_BADFUN, //!< Requested a parsing function to parse a frame with wrong function code
    MODBUS_FERROR_NULLFUN, //!< Function overriden by user with NULL pointer.
    MODBUS_FERROR_MISM_FUN, //!< Function request-response mismatch
    MODBUS_FERROR_MISM_ADDR, //!< Slave address request-response mismatch
    MODBUS_FERROR_MISM_INDEX, //!< Index value request-response mismatch
    MODBUS_FERROR_MISM_COUNT, //!< Count value request-response mismatch
    MODBUS_FERROR_MISM_VALUE, //!< Data value request-response mismatch
    MODBUS_FERROR_MISM_MASK, //!< Mask value request-response mismatch
    MODBUS_FERROR_BROADCAST //!< Received response for broadcast message
}ModbusFrameError_e;

typedef enum
{
    MODBUS_EXCEP_ILLEGAL_FUNCTION = 1, //!< Illegal function code
    MODBUS_EXCEP_ILLEGAL_ADDRESS = 2, //!< Illegal data address
    MODBUS_EXCEP_ILLEGAL_VALUE = 3, //!< Illegal data value
    MODBUS_EXCEP_SLAVE_FAILURE = 4, //!< Slave could not process the request
    MODBUS_EXCEP_ACK = 5, //!< Acknowledge
    MODBUS_EXCEP_NACK = 7 //!< Negative acknowledge
}ModbusExceptionCode_e;

typedef enum
{
    MODBUS_HOLDING_REGISTER = 1, //!< Holding register
    MODBUS_INPUT_REGISTER = 2, //!< Input register
    MODBUS_COIL = 4, //!< Coil
    MODBUS_DISCRETE_INPUT = 8 //!< Discrete input
}ModbusDataType_e;


/**
    \brief Converts number of bits to number of bytes required to store them
    \param n Number of bits
    \returns Number of bytes of required memory
*/
static inline uint16_t modbusBitsToBytes( uint16_t n )
{
    return n != 0 ? ( 1 + ( ( n - 1 ) >> 3 ) ) : 0;
}

/**
    \brief Swaps endianness of provided 16-bit data portion

    \note This function, unlike \ref modbusMatchEndian, works unconditionally

    \param data A 16-bit data portion.
    \returns The same data, but with bytes swapped
    \see modbusMatchEndian
*/
static inline uint16_t modbusSwapEndian( uint16_t data ) { return ( data << 8 ) | ( data >> 8 ); }


#ifdef ARCH_USED_BIG_ENDIAN
    static inline uint16_t modbusMatchEndian( uint16_t data ) { return data; }
#else
    static inline uint16_t modbusMatchEndian( uint16_t data ) { return modbusSwapEndian( data ); }
#endif

uint8_t modbusMaskRead( const uint8_t *mask, uint16_t maskLength, uint16_t bit );

uint8_t modbusMaskWrite( uint8_t *mask, uint16_t maskLength, uint16_t bit, uint8_t value );

uint16_t modbusCRC( const uint8_t *data, uint16_t length );

/** **********************parser*************************************** **/

typedef union modbusParser
{
    uint8_t frame[256];

    struct __attribute__( ( __packed__ ) )
    {
        uint8_t address;
        uint8_t function;
    } base; //Base shared bytes, common for all frames, which always have the same meaning

    struct __attribute__( ( __packed__ ) )
    {
        uint8_t address;
        uint8_t function;
        uint8_t code;
        uint16_t crc;
    } exception;

    struct __attribute__( ( __packed__ ) )
    {
        uint8_t address;
        uint8_t function;
        uint16_t index;
        uint16_t count;
        uint16_t crc;
    } request0102; //Read multiple coils or discrete inputs

    struct __attribute__( ( __packed__ ) )
    {
        uint8_t address;
        uint8_t function;
        uint8_t length;
        uint8_t values[250];
        uint16_t crc;
    } response0102; //Read multiple coils or discrete inputs - response

    struct __attribute__( ( __packed__ ) )
    {
        uint8_t address;
        uint8_t function;
        uint16_t index;
        uint16_t count;
        uint16_t crc;
    } request0304; //Read multiple holding registers or input registers

    struct __attribute__( ( __packed__ ) )
    {
        uint8_t address;
        uint8_t function;
        uint8_t length;
        uint16_t values[125];
        uint16_t crc;
    } response0304; //Read multiple holding registers or input registers - response

    //TODO merge request 05 and 06
    struct __attribute__( ( __packed__ ) )
    {
        uint8_t address;
        uint8_t function;
        uint16_t index;
        uint16_t value;
        uint16_t crc;
    } request05; //Write single coil

    struct __attribute__( ( __packed__ ) )
    {
        uint8_t address;
        uint8_t function;
        uint16_t index;
        uint16_t value;
        uint16_t crc;
    } response05; //Write single coil - response

    struct __attribute__( ( __packed__ ) )
    {
        uint8_t address;
        uint8_t function;
        uint16_t index;
        uint16_t value;
        uint16_t crc;
    } request06; //Write single holding register

    struct __attribute__( ( __packed__ ) )
    {
        uint8_t address;
        uint8_t function;
        uint16_t index;
        uint16_t value;
        uint16_t crc;
    } response06; //Write single holding register

    struct __attribute__( ( __packed__ ) )
    {
        uint8_t address;
        uint8_t function;
        uint16_t index;
        uint16_t count;
        uint8_t length;
        uint8_t values[246];
        uint16_t crc;
    } request15; //Write multiple coils

    struct __attribute__( ( __packed__ ) )
    {
        uint8_t address;
        uint8_t function;
        uint16_t index;
        uint16_t count;
        uint16_t crc;
    } response15; //Write multiple coils - response

    struct __attribute__( ( __packed__ ) )
    {
        uint8_t address;
        uint8_t function;
        uint16_t index;
        uint16_t count;
        uint8_t length;
        uint16_t values[123];
        uint16_t crc;
    } request16; //Write multiple holding registers

    struct __attribute__( ( __packed__ ) )
    {
        uint8_t address;
        uint8_t function;
        uint16_t index;
        uint16_t count;
        uint16_t crc;
    } response16; //Write multiple holding registers

    struct __attribute__( ( __packed__ ) )
    {
        uint8_t address;
        uint8_t function;
        uint16_t index;
        uint16_t andmask;
        uint16_t ormask;
        uint16_t crc;
    } request22; //Mask write single holding register

    struct __attribute__( ( __packed__ ) )
    {
        uint8_t address;
        uint8_t function;
        uint16_t index;
        uint16_t andmask;
        uint16_t ormask;
        uint16_t crc;
    } response22; //Mask write single holding register
} ModbusParser;

/********************************** master ********************************/
struct modbusMaster;
/** 用户定义数据 */
typedef struct modbusMasterUserFunction
{
    uint8_t function; //!< The function code
    ModbusError_e ( *handler )( struct modbusMaster *status, ModbusParser *parser, ModbusParser *requestParser ); //!< Pointer to the user defined parsing function
} ModbusMasterUserFunction;

typedef struct modbusMaster
{
    uint8_t predictedResponseLength; /*预计收到的字节数*/
    struct
    {
#ifdef ARCH_USED_STATIC_MEM
#define REQUEST_STATIC_MEM_MAX_LEN  256
        uint8_t frame[REQUEST_STATIC_MEM_MAX_LEN];
#else
        uint8_t *frame;
#endif
        uint8_t length;
    }request;
    struct
    {
#ifdef ARCH_USED_STATIC_MEM
#define RESPONSE_STATIC_MEM_MAX_LEN 256
        uint8_t frame[RESPONSE_STATIC_MEM_MAX_LEN];
#else
        uint8_t *frame;
#endif
    }response;

    struct
    {
        uint8_t address; //!< Addres of slave that sent in the data
        uint16_t index; //!< Modbus address of the first register/coil
        uint16_t count; //!< Number of data units (coils, registers, etc.)
        uint8_t length; //!< Length of data in bytes
        ModbusDataType_e type; //!< Type of data
        uint8_t function; //!< Function that accessed the data
#ifdef ARCH_USED_STATIC_MEM
#define DATA_STATIC_MEM_MAX_LEN     256
        union
        {
            uint8_t coils[DATA_STATIC_MEM_MAX_LEN];
            uint8_t regs[DATA_STATIC_MEM_MAX_LEN];
        };
#else
        uint8_t *coils;
        uint8_t *regs;
#endif
    }data;

    /*从salve接收到的异常数据 */
    struct
    {
        uint8_t address; //!< Slave device address
        uint8_t function; //!< Function that has thrown the exception
        ModbusExceptionCode code; //!< Exception code
    } exception;

    ModbusFrameError_e parseError;
    ModbusFrameError_e buildError;
#ifdef ARCH_USED_USER_FUNC
    ModbusMasterUserFunction *userFunctions;
    uint16_t userFunctionsCount;
#endif
}ModbusMaster;


ModbusError_e modbusParseResponse( ModbusMaster *status );


ModbusError_e modbusMasterInit( ModbusMaster *status );


ModbusError_e modbusMasterEnd( ModbusMaster *status );

/*****************************************slave ***********************************/
struct modbusSlave;
typedef struct modbusSlaveUserFunction
{
    uint8_t function; //!< The function code
    /**
        \brief Pointer to the user defined function
        \param status Slave structure to work with
        \param parser The parser structure containing frame data
        \returns A \ref ModbusError error code
    */
    ModbusError_e ( *handler )( struct modbusSlave *status, ModbusParser *parser );
} ModbusSlaveUserFunction;

typedef enum
{
    MODBUS_REGQ_R, //!< Requests callback function to return register value
    MODBUS_REGQ_W, //!< Requests callback function to write the register
    MODBUS_REGQ_R_CHECK, //!< Asks callback function if register can be read
    MODBUS_REGQ_W_CHECK //!< Asks callback function if register can be written
}ModbusRegisterQuery_e;

typedef uint16_t ( *ModbusRegisterCallbackFunction )( ModbusRegisterQuery_e query, ModbusDataType_e datatype, uint16_t index, uint16_t value, void *ctx );

struct modbusSlave
{
    uint8_t address; //!< The slave's address

    //Universal register/coil callback function
    #if ARCH_USED_USER_REGISTER_COIL_FUNC
        /**
            \brief The pointer to the user-defined register callback function

            \note Requires `LIGHTMODBUS_COIL_CALLBACK` or `LIGHTMODBUS_REGISTER_CALLBACK` to be defined
        */
        ModbusRegisterCallbackFunction registerCallback;

        /**
            \brief The user data pointer passed to the callback function each time it's used
            \warning This pointer is not managed nor controlled by library.
            So, what you set is what you get.
        */
        void *registerCallbackContext;
    #endif

    //Slave registers arrays
    #ifndef ARCH_USED_USER_REGISTER_COIL_FUNC
        uint16_t *registers; //!< Pointer to registers data
        uint16_t *inputRegisters; //!< Pointer to input registers data
        uint8_t *registerMask; //!< Mask for register write protection (each bit corresponds to one register)
        uint16_t registerMaskLength; //!< Write protection mask (\ref registerMask) length in bytes (each byte covers 8 registers)
    #endif
    uint16_t registerCount; //!< Slave's register count
    uint16_t inputRegisterCount; //!< Slave's input register count

    //Slave coils array
    #ifndef ARCH_USED_USER_REGISTER_COIL_FUNC
        uint8_t *coils; //!< Pointer to coils data
        uint8_t *discreteInputs; //!< Pointer to discrete inputs data
        uint8_t *coilMask; //!< Masks for coil write protection (each bit corresponds to one coil)
        uint16_t coilMaskLength; //!< Write protection mask (\ref coilMask) length in bytes (each byte covers 8 coils)
    #endif
    uint16_t coilCount; //!< Slave's coil count
    uint16_t discreteInputCount; //!< Slave's discrete input count

    /**
        \brief Exception code of the last exception generated by \ref modbusBuildException
        \see modbusBuildException
    */
    ModbusExceptionCode_e lastException;

    /**
        \brief More specific error code of problem encountered during frame parsing

        This variable is set up by \ref modbusBuildExceptionErr function
        \see modbusBuildExceptionErr
    */
    ModbusFrameError_e parseError;

    //Array of user defined functions - these can override default Modbus functions
    #ifdef ARCH_USED_USER_FUNC
        /**
            \brief A pointer to user defined Modbus functions array

            \note Requires `SLAVE_USER_FUNCTIONS` module (see \ref building)
            \see user-functions
        */
        ModbusSlaveUserFunction *userFunctions;
        uint16_t userFunctionCount; //!< Number of user-defined Modbus functions /see userFunctions
    #endif

    /**
        \brief Struct containing slave's response to the master's request

        \note Declaration of the `frame` member depends on the library configuration.
        It can be either a statically allocated array or a pointer to dynamically allocated memory.
        The behavior is dependant on definition of the `LIGHTMODBUS_STATIC_MEM_SLAVE_RESPONSE` macro

        \see \ref static-mem
    */
    struct
    {
        #ifdef ARCH_USED_STATIC_MEM
        #define SLAVE_RESPONSE_STATIC_MEM_MAX_LEN   256
            //! Statically allocated memory for the response frame
            uint8_t frame[SLAVE_RESPONSE_STATIC_MEM_MAX_LEN];
        #else
            //! A pointer to dynamically allocated memory for the response frame
            uint8_t *frame;
        #endif

        //! Frame length in bytes
        uint8_t length;
    } response;

    /**
        \brief Struct containing master's request frame

        \note Declaration of the `frame` member depends on the library configuration.
        It can be either a statically allocated array or a pointer to dynamically allocated memory.
        The behavior is dependant on definition of the `LIGHTMODBUS_STATIC_MEM_SLAVE_REQUEST` macro

        \see \ref static-mem
    */
    struct
    {
        #ifdef ARCH_USED_STATIC_MEM
#define SLAVE_REQUEST_STATIC_MEM_MAX_LEN
            //! Statically allocated memory for the request frame
            uint8_t frame[SLAVE_REQUEST_STATIC_MEM_MAX_LEN];
        #else
            //! A pointer to dynamically allocated memory for the request frame
            const uint8_t *frame;
        #endif

        //! Frame length in bytes
        uint8_t length;
    } request;
}ModbusSlave;

ModbusError_e modbusBuildException( ModbusSlave *status, uint8_t function, ModbusExceptionCode_e code );

ModbusError_e modbusParseRequest( ModbusSlave *status );

ModbusError_e modbusSlaveInit( ModbusSlave *status );

ModbusError_e modbusSlaveEnd( ModbusSlave *status );


static inline ModbusError_e modbusBuildExceptionErr( ModbusSlave *status, uint8_t function, ModbusExceptionCode_e code, ModbusFrameError_e parseError ) //Build an exception and write error to status->parseError
{
    if ( status == NULL ) return MODBUS_ERROR_NULLPTR;
    status->parseError = parseError;
    ModbusError_e err = modbusBuildException( status, function, code );
    if ( err == MODBUS_ERROR_OK ) return MODBUS_ERROR_PARSE;
    else return err;
}





#ifdef __cplusplus
}
#endif

#endif
