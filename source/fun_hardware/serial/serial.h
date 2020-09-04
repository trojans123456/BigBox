#ifndef __SERIAL_H
#define __SERIAL_H

#include <stddef.h>

#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <stdarg.h>
#include <limits.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <limits.h>
#include <poll.h>

#include <dirent.h>


#ifdef __cplusplus
extern "C" {
#endif



/** Return values. */
enum serial_return {
    /** Operation completed successfully. */
    SERIAL_OK = 0,
    /** Invalid arguments were passed to the function. */
    SERIAL_ERR_ARG = -1,
    /** A system error occurred while executing the operation. */
    SERIAL_ERR_FAIL = -2,
    /** A memory allocation failed while executing the operation. */
    SERIAL_ERR_MEM = -3,
    /** The requested operation is not supported by this system or device. */
    SERIAL_ERR_SUPP = -4
};

/** Port access modes. */
enum serial_mode {
    /** Open port for read access. */
    SERIAL_MODE_READ = 1,
    /** Open port for write access. */
    SERIAL_MODE_WRITE = 2,
    /** Open port for read and write access. @since 0.1.1 */
    SERIAL_MODE_READ_WRITE = 3
};

/** Port events. */
enum serial_event {
    /** Data received and ready to read. */
    SERIAL_EVENT_RX_READY = 1,
    /** Ready to transmit new data. */
    SERIAL_EVENT_TX_READY = 2,
    /** Error occurred. */
    SERIAL_EVENT_ERROR = 4
};

/** Buffer selection. */
enum serial_buffer {
    /** Input buffer. */
    SERIAL_BUF_INPUT = 1,
    /** Output buffer. */
    SERIAL_BUF_OUTPUT = 2,
    /** Both buffers. */
    SERIAL_BUF_BOTH = 3
};

/** Parity settings. */
enum serial_parity {
    /** Special value to indicate setting should be left alone. */
    SERIAL_PARITY_INVALID = -1,
    /** No parity. */
    SERIAL_PARITY_NONE = 0,
    /** Odd parity. */
    SERIAL_PARITY_ODD = 1,
    /** Even parity. */
    SERIAL_PARITY_EVEN = 2,
    /** Mark parity. */
    SERIAL_PARITY_MARK = 3,
    /** Space parity. */
    SERIAL_PARITY_SPACE = 4
};

/** RTS pin behaviour. */
enum serial_rts {
    /** Special value to indicate setting should be left alone. */
    SERIAL_RTS_INVALID = -1,
    /** RTS off. */
    SERIAL_RTS_OFF = 0,
    /** RTS on. */
    SERIAL_RTS_ON = 1,
    /** RTS used for flow control. */
    SERIAL_RTS_FLOW_CONTROL = 2
};

/** CTS pin behaviour. */
enum serial_cts {
    /** Special value to indicate setting should be left alone. */
    SERIAL_CTS_INVALID = -1,
    /** CTS ignored. */
    SERIAL_CTS_IGNORE = 0,
    /** CTS used for flow control. */
    SERIAL_CTS_FLOW_CONTROL = 1
};

/** DTR pin behaviour. */
enum serial_dtr {
    /** Special value to indicate setting should be left alone. */
    SERIAL_DTR_INVALID = -1,
    /** DTR off. */
    SERIAL_DTR_OFF = 0,
    /** DTR on. */
    SERIAL_DTR_ON = 1,
    /** DTR used for flow control. */
    SERIAL_DTR_FLOW_CONTROL = 2
};

/** DSR pin behaviour. */
enum serial_dsr {
    /** Special value to indicate setting should be left alone. */
    SERIAL_DSR_INVALID = -1,
    /** DSR ignored. */
    SERIAL_DSR_IGNORE = 0,
    /** DSR used for flow control. */
    SERIAL_DSR_FLOW_CONTROL = 1
};

/** XON/XOFF flow control behaviour. */
enum serial_xonxoff {
    /** Special value to indicate setting should be left alone. */
    SERIAL_XONXOFF_INVALID = -1,
    /** XON/XOFF disabled. */
    SERIAL_XONXOFF_DISABLED = 0,
    /** XON/XOFF enabled for input only. */
    SERIAL_XONXOFF_IN = 1,
    /** XON/XOFF enabled for output only. */
    SERIAL_XONXOFF_OUT = 2,
    /** XON/XOFF enabled for input and output. */
    SERIAL_XONXOFF_INOUT = 3
};

/** Standard flow control combinations. */
enum serial_flowcontrol {
    /** No flow control. */
    SERIAL_FLOWCONTROL_NONE = 0,
    /** Software flow control using XON/XOFF characters. */
    SERIAL_FLOWCONTROL_XONXOFF = 1,
    /** Hardware flow control using RTS/CTS signals. */
    SERIAL_FLOWCONTROL_RTSCTS = 2,
    /** Hardware flow control using DTR/DSR signals. */
    SERIAL_FLOWCONTROL_DTRDSR = 3
};

/** Input signals. */
enum serial_signal {
    /** Clear to send. */
    SERIAL_SIG_CTS = 1,
    /** Data set ready. */
    SERIAL_SIG_DSR = 2,
    /** Data carrier detect. */
    SERIAL_SIG_DCD = 4,
    /** Ring indicator. */
    SERIAL_SIG_RI = 8
};



struct serial_port {
    char *name;
    int fd;
};

struct serial_port_config {
    int baudrate;
    int bits;
    enum serial_parity parity;
    int stopbits;
    enum serial_rts rts;
    enum serial_cts cts;
    enum serial_dtr dtr;
    enum serial_dsr dsr;
    enum serial_xonxoff xon_xoff;
};

struct port_data {
    struct termios term;
    int controlbits;
    int termiox_supported;
    int rts_flow;
    int cts_flow;
    int dtr_flow;
    int dsr_flow;

};

typedef int event_handle;


/* Standard baud rates. */

#define BAUD_TYPE speed_t
#define BAUD(n) {B##n, n}


struct std_baudrate {
    BAUD_TYPE index;
    int value;
};

#define ARRAY_SIZE(x) (sizeof(x) / sizeof(x[0]))


unsigned long get_termios_get_ioctl(void);
unsigned long get_termios_set_ioctl(void);
size_t get_termios_size(void);
int get_termios_speed(void *data);
void set_termios_speed(void *data, int speed);
size_t get_termiox_size(void);
int get_termiox_flow(void *data, int *rts, int *cts, int *dtr, int *dsr);
void set_termiox_flow(void *data, int rts, int cts, int dtr, int dsr);



/**
 * @struct serial_event_set
 * A set of handles to wait on for events.
 */
struct serial_event_set {
    /** Array of OS-specific handles. */
    void *handles;
    /** Array of bitmasks indicating which events apply for each handle. */
    enum serial_event *masks;
    /** Number of handles. */
    unsigned int count;
};


/**
 * 打开一个指定的串口
 *
 * @param[in] port Pointer to a port structure. Must not be NULL.
 * @param[in] flags Flags to use when opening the serial port.
 *
 * @return serial_OK upon success, a negative error code otherwise.
 *
 * struct serial_port port = {"/dev/ttyxxx",-1};
 *
 */
enum serial_return serial_open(struct serial_port *port, enum serial_mode flags);

/**
 * 关闭指定的串口
 *
 * @param[in] port Pointer to a port structure. Must not be NULL.
 *
 * @return serial_OK upon success, a negative error code otherwise.
 *
 *
 */
enum serial_return serial_close(struct serial_port *port);


/**
 * 获取指定串口的配置
 * @param[in] port Pointer to a port structure. Must not be NULL.
 * @param[out] config Pointer to a configuration structure that will hold
 *                    the result. Upon errors the contents of the config
 *                    struct will not be changed. Must not be NULL.
 *
 * @return serial_OK upon success, a negative error code otherwise.
 *
 * @since 0.1.0
 */
enum serial_return serial_get_config(struct serial_port *port, struct serial_port_config *config);

/**
 * 设置指定串口的配置
 *
 * @param[in] port Pointer to a port structure. Must not be NULL.
 * @param[in] config Pointer to a configuration structure. Must not be NULL.
 *
 * @return serial_OK upon success, a negative error code otherwise.
 *
 */
enum serial_return serial_set_config(struct serial_port *port, const struct serial_port_config *config);

/**
 * 设置波特率
 *
 * @param[in] port Pointer to a port structure. Must not be NULL.
 * @param[in] baudrate Baud rate in bits per second.
 *
 * @return serial_OK upon success, a negative error code otherwise.
 *
 */
enum serial_return serial_set_baudrate(struct serial_port *port, int baudrate);

/**
 * 获取指定串口的波特率
 *
 * @param[in] config Pointer to a configuration structure. Must not be NULL.
 * @param[out] baudrate_ptr Pointer to a variable to store the result. Must not be NULL.
 *
 * @return serial_OK upon success, a negative error code otherwise.
 *
 */
enum serial_return serial_get_config_baudrate(const struct serial_port_config *config, int *baudrate_ptr);

/**
 * Set the baud rate in a port configuration.
 *
 * @param[in] config Pointer to a configuration structure. Must not be NULL.
 * @param[in] baudrate Baud rate in bits per second, or -1 to retain the current setting.
 *
 * @return serial_OK upon success, a negative error code otherwise.
 *
 * @since 0.1.0
 */
enum serial_return serial_set_config_baudrate(struct serial_port_config *config, int baudrate);

/**
 * Set the data bits for the specified serial port.
 *
 * @param[in] port Pointer to a port structure. Must not be NULL.
 * @param[in] bits Number of data bits.
 *
 * @return serial_OK upon success, a negative error code otherwise.
 *
 * @since 0.1.0
 */
enum serial_return serial_set_bits(struct serial_port *port, int bits);

/**
 * Get the data bits from a port configuration.
 *
 * The user should allocate a variable of type int and
 * pass a pointer to this to receive the result.
 *
 * @param[in] config Pointer to a configuration structure. Must not be NULL.
 * @param[out] bits_ptr Pointer to a variable to store the result. Must not be NULL.
 *
 * @return serial_OK upon success, a negative error code otherwise.
 *
 * @since 0.1.0
 */
enum serial_return serial_get_config_bits(const struct serial_port_config *config, int *bits_ptr);

/**
 * Set the data bits in a port configuration.
 *
 * @param[in] config Pointer to a configuration structure. Must not be NULL.
 * @param[in] bits Number of data bits, or -1 to retain the current setting.
 *
 * @return serial_OK upon success, a negative error code otherwise.
 *
 * @since 0.1.0
 */
enum serial_return serial_set_config_bits(struct serial_port_config *config, int bits);

/**
 * Set the parity setting for the specified serial port.
 *
 * @param[in] port Pointer to a port structure. Must not be NULL.
 * @param[in] parity Parity setting.
 *
 * @return serial_OK upon success, a negative error code otherwise.
 *
 * @since 0.1.0
 */
enum serial_return serial_set_parity(struct serial_port *port, enum serial_parity parity);

/**
 * Get the parity setting from a port configuration.
 *
 * The user should allocate a variable of type enum serial_parity and
 * pass a pointer to this to receive the result.
 *
 * @param[in] config Pointer to a configuration structure. Must not be NULL.
 * @param[out] parity_ptr Pointer to a variable to store the result. Must not be NULL.
 *
 * @return serial_OK upon success, a negative error code otherwise.
 *
 * @since 0.1.0
 */
enum serial_return serial_get_config_parity(const struct serial_port_config *config, enum serial_parity *parity_ptr);

/**
 * Set the parity setting in a port configuration.
 *
 * @param[in] config Pointer to a configuration structure. Must not be NULL.
 * @param[in] parity Parity setting, or -1 to retain the current setting.
 *
 * @return serial_OK upon success, a negative error code otherwise.
 *
 * @since 0.1.0
 */
enum serial_return serial_set_config_parity(struct serial_port_config *config, enum serial_parity parity);

/**
 * Set the stop bits for the specified serial port.
 *
 * @param[in] port Pointer to a port structure. Must not be NULL.
 * @param[in] stopbits Number of stop bits.
 *
 * @return serial_OK upon success, a negative error code otherwise.
 *
 * @since 0.1.0
 */
enum serial_return serial_set_stopbits(struct serial_port *port, int stopbits);

/**
 * Get the stop bits from a port configuration.
 *
 * The user should allocate a variable of type int and
 * pass a pointer to this to receive the result.
 *
 * @param[in] config Pointer to a configuration structure. Must not be NULL.
 * @param[out] stopbits_ptr Pointer to a variable to store the result. Must not be NULL.
 *
 * @return serial_OK upon success, a negative error code otherwise.
 *
 * @since 0.1.0
 */
enum serial_return serial_get_config_stopbits(const struct serial_port_config *config, int *stopbits_ptr);

/**
 * Set the stop bits in a port configuration.
 *
 * @param[in] config Pointer to a configuration structure. Must not be NULL.
 * @param[in] stopbits Number of stop bits, or -1 to retain the current setting.
 *
 * @return serial_OK upon success, a negative error code otherwise.
 *
 * @since 0.1.0
 */
enum serial_return serial_set_config_stopbits(struct serial_port_config *config, int stopbits);

/**
 * Set the RTS pin behaviour for the specified serial port.
 *
 * @param[in] port Pointer to a port structure. Must not be NULL.
 * @param[in] rts RTS pin mode.
 *
 * @return serial_OK upon success, a negative error code otherwise.
 *
 * @since 0.1.0
 */
enum serial_return serial_set_rts(struct serial_port *port, enum serial_rts rts);

/**
 * Get the RTS pin behaviour from a port configuration.
 *
 * The user should allocate a variable of type enum serial_rts and
 * pass a pointer to this to receive the result.
 *
 * @param[in] config Pointer to a configuration structure. Must not be NULL.
 * @param[out] rts_ptr Pointer to a variable to store the result. Must not be NULL.
 *
 * @return serial_OK upon success, a negative error code otherwise.
 *
 * @since 0.1.0
 */
enum serial_return serial_get_config_rts(const struct serial_port_config *config, enum serial_rts *rts_ptr);

/**
 * Set the RTS pin behaviour in a port configuration.
 *
 * @param[in] config Pointer to a configuration structure. Must not be NULL.
 * @param[in] rts RTS pin mode, or -1 to retain the current setting.
 *
 * @return serial_OK upon success, a negative error code otherwise.
 *
 * @since 0.1.0
 */
enum serial_return serial_set_config_rts(struct serial_port_config *config, enum serial_rts rts);

/**
 * Set the CTS pin behaviour for the specified serial port.
 *
 * @param[in] port Pointer to a port structure. Must not be NULL.
 * @param[in] cts CTS pin mode.
 *
 * @return serial_OK upon success, a negative error code otherwise.
 *
 * @since 0.1.0
 */
enum serial_return serial_set_cts(struct serial_port *port, enum serial_cts cts);

/**
 * Get the CTS pin behaviour from a port configuration.
 *
 * The user should allocate a variable of type enum serial_cts and
 * pass a pointer to this to receive the result.
 *
 * @param[in] config Pointer to a configuration structure. Must not be NULL.
 * @param[out] cts_ptr Pointer to a variable to store the result. Must not be NULL.
 *
 * @return serial_OK upon success, a negative error code otherwise.
 *
 * @since 0.1.0
 */
enum serial_return serial_get_config_cts(const struct serial_port_config *config, enum serial_cts *cts_ptr);

/**
 * Set the CTS pin behaviour in a port configuration.
 *
 * @param[in] config Pointer to a configuration structure. Must not be NULL.
 * @param[in] cts CTS pin mode, or -1 to retain the current setting.
 *
 * @return serial_OK upon success, a negative error code otherwise.
 *
 * @since 0.1.0
 */
enum serial_return serial_set_config_cts(struct serial_port_config *config, enum serial_cts cts);

/**
 * Set the DTR pin behaviour for the specified serial port.
 *
 * @param[in] port Pointer to a port structure. Must not be NULL.
 * @param[in] dtr DTR pin mode.
 *
 * @return serial_OK upon success, a negative error code otherwise.
 *
 * @since 0.1.0
 */
enum serial_return serial_set_dtr(struct serial_port *port, enum serial_dtr dtr);

/**
 * Get the DTR pin behaviour from a port configuration.
 *
 * The user should allocate a variable of type enum serial_dtr and
 * pass a pointer to this to receive the result.
 *
 * @param[in] config Pointer to a configuration structure. Must not be NULL.
 * @param[out] dtr_ptr Pointer to a variable to store the result. Must not be NULL.
 *
 * @return serial_OK upon success, a negative error code otherwise.
 *
 * @since 0.1.0
 */
enum serial_return serial_get_config_dtr(const struct serial_port_config *config, enum serial_dtr *dtr_ptr);

/**
 * Set the DTR pin behaviour in a port configuration.
 *
 * @param[in] config Pointer to a configuration structure. Must not be NULL.
 * @param[in] dtr DTR pin mode, or -1 to retain the current setting.
 *
 * @return serial_OK upon success, a negative error code otherwise.
 *
 * @since 0.1.0
 */
enum serial_return serial_set_config_dtr(struct serial_port_config *config, enum serial_dtr dtr);

/**
 * Set the DSR pin behaviour for the specified serial port.
 *
 * @param[in] port Pointer to a port structure. Must not be NULL.
 * @param[in] dsr DSR pin mode.
 *
 * @return serial_OK upon success, a negative error code otherwise.
 *
 * @since 0.1.0
 */
enum serial_return serial_set_dsr(struct serial_port *port, enum serial_dsr dsr);

/**
 * Get the DSR pin behaviour from a port configuration.
 *
 * The user should allocate a variable of type enum serial_dsr and
 * pass a pointer to this to receive the result.
 *
 * @param[in] config Pointer to a configuration structure. Must not be NULL.
 * @param[out] dsr_ptr Pointer to a variable to store the result. Must not be NULL.
 *
 * @return serial_OK upon success, a negative error code otherwise.
 *
 * @since 0.1.0
 */
enum serial_return serial_get_config_dsr(const struct serial_port_config *config, enum serial_dsr *dsr_ptr);

/**
 * Set the DSR pin behaviour in a port configuration.
 *
 * @param[in] config Pointer to a configuration structure. Must not be NULL.
 * @param[in] dsr DSR pin mode, or -1 to retain the current setting.
 *
 * @return serial_OK upon success, a negative error code otherwise.
 *
 * @since 0.1.0
 */
enum serial_return serial_set_config_dsr(struct serial_port_config *config, enum serial_dsr dsr);

/**
 * Set the XON/XOFF configuration for the specified serial port.
 *
 * @param[in] port Pointer to a port structure. Must not be NULL.
 * @param[in] xon_xoff XON/XOFF mode.
 *
 * @return serial_OK upon success, a negative error code otherwise.
 *
 * @since 0.1.0
 */
enum serial_return serial_set_xon_xoff(struct serial_port *port, enum serial_xonxoff xon_xoff);

/**
 * Get the XON/XOFF configuration from a port configuration.
 *
 * The user should allocate a variable of type enum serial_xonxoff and
 * pass a pointer to this to receive the result.
 *
 * @param[in] config Pointer to a configuration structure. Must not be NULL.
 * @param[out] xon_xoff_ptr Pointer to a variable to store the result. Must not be NULL.
 *
 * @return serial_OK upon success, a negative error code otherwise.
 *
 * @since 0.1.0
 */
enum serial_return serial_get_config_xon_xoff(const struct serial_port_config *config, enum serial_xonxoff *xon_xoff_ptr);

/**
 * Set the XON/XOFF configuration in a port configuration.
 *
 * @param[in] config Pointer to a configuration structure. Must not be NULL.
 * @param[in] xon_xoff XON/XOFF mode, or -1 to retain the current setting.
 *
 * @return serial_OK upon success, a negative error code otherwise.
 *
 * @since 0.1.0
 */
enum serial_return serial_set_config_xon_xoff(struct serial_port_config *config, enum serial_xonxoff xon_xoff);

/**
 * Set the flow control type in a port configuration.
 *
 * This function is a wrapper that sets the RTS, CTS, DTR, DSR and
 * XON/XOFF settings as necessary for the specified flow control
 * type. For more fine-grained control of these settings, use their
 * individual configuration functions.
 *
 * @param[in] config Pointer to a configuration structure. Must not be NULL.
 * @param[in] flowcontrol Flow control setting to use.
 *
 * @return serial_OK upon success, a negative error code otherwise.
 *
 * @since 0.1.0
 */
enum serial_return serial_set_config_flowcontrol(struct serial_port_config *config, enum serial_flowcontrol flowcontrol);

/**
 * Set the flow control type for the specified serial port.
 *
 * This function is a wrapper that sets the RTS, CTS, DTR, DSR and
 * XON/XOFF settings as necessary for the specified flow control
 * type. For more fine-grained control of these settings, use their
 * individual configuration functions.
 *
 * @param[in] port Pointer to a port structure. Must not be NULL.
 * @param[in] flowcontrol Flow control setting to use.
 *
 * @return serial_OK upon success, a negative error code otherwise.
 *
 * @since 0.1.0
 */
enum serial_return serial_set_flowcontrol(struct serial_port *port, enum serial_flowcontrol flowcontrol);

/**
 * @}
 *
 * @defgroup Data Data handling
 *
 * Reading, writing, and flushing data.
 *
 * @{
 */

/**
 * Read bytes from the specified serial port, blocking until complete.
 *
 * @warning If your program runs on Unix, defines its own signal handlers, and
 *          needs to abort blocking reads when these are called, then you
 *          should not use this function. It repeats system calls that return
 *          with EINTR. To be able to abort a read from a signal handler, you
 *          should implement your own blocking read using serial_nonblocking_read()
 *          together with a blocking method that makes sense for your program.
 *          E.g. you can obtain the file descriptor for an open port using
 *          serial_get_port_handle() and use this to call select() or pselect(),
 *          with appropriate arrangements to return if a signal is received.
 *
 * @param[in] port Pointer to a port structure. Must not be NULL.
 * @param[out] buf Buffer in which to store the bytes read. Must not be NULL.
 * @param[in] count Requested number of bytes to read.
 * @param[in] timeout_ms Timeout in milliseconds, or zero to wait indefinitely.
 *
 * @return The number of bytes read on success, or a negative error code. If
 *         the number of bytes returned is less than that requested, the
 *         timeout was reached before the requested number of bytes was
 *         available. If timeout is zero, the function will always return
 *         either the requested number of bytes or a negative error code.
 *
 * @since 0.1.0
 */
enum serial_return serial_blocking_read(struct serial_port *port, void *buf, size_t count, unsigned int timeout_ms);

/**
 * Read bytes from the specified serial port, returning as soon as any data is
 * available.
 *
 * @warning If your program runs on Unix, defines its own signal handlers, and
 *          needs to abort blocking reads when these are called, then you
 *          should not use this function. It repeats system calls that return
 *          with EINTR. To be able to abort a read from a signal handler, you
 *          should implement your own blocking read using serial_nonblocking_read()
 *          together with a blocking method that makes sense for your program.
 *          E.g. you can obtain the file descriptor for an open port using
 *          serial_get_port_handle() and use this to call select() or pselect(),
 *          with appropriate arrangements to return if a signal is received.
 *
 * @param[in] port Pointer to a port structure. Must not be NULL.
 * @param[out] buf Buffer in which to store the bytes read. Must not be NULL.
 * @param[in] count Maximum number of bytes to read. Must not be zero.
 * @param[in] timeout_ms Timeout in milliseconds, or zero to wait indefinitely.
 *
 * @return The number of bytes read on success, or a negative error code. If
 *         the result is zero, the timeout was reached before any bytes were
 *         available. If timeout_ms is zero, the function will always return
 *         either at least one byte, or a negative error code.
 *
 * @since 0.1.1
 */
enum serial_return serial_blocking_read_next(struct serial_port *port, void *buf, size_t count, unsigned int timeout_ms);

/**
 * Read bytes from the specified serial port, without blocking.
 *
 * @param[in] port Pointer to a port structure. Must not be NULL.
 * @param[out] buf Buffer in which to store the bytes read. Must not be NULL.
 * @param[in] count Maximum number of bytes to read.
 *
 * @return The number of bytes read on success, or a negative error code. The
 *         number of bytes returned may be any number from zero to the maximum
 *         that was requested.
 *
 * @since 0.1.0
 */
enum serial_return serial_nonblocking_read(struct serial_port *port, void *buf, size_t count);

/**
 * Write bytes to the specified serial port, blocking until complete.
 *
 * Note that this function only ensures that the accepted bytes have been
 * written to the OS; they may be held in driver or hardware buffers and not
 * yet physically transmitted. To check whether all written bytes have actually
 * been transmitted, use the serial_output_waiting() function. To wait until all
 * written bytes have actually been transmitted, use the serial_drain() function.
 *
 * @warning If your program runs on Unix, defines its own signal handlers, and
 *          needs to abort blocking writes when these are called, then you
 *          should not use this function. It repeats system calls that return
 *          with EINTR. To be able to abort a write from a signal handler, you
 *          should implement your own blocking write using serial_nonblocking_write()
 *          together with a blocking method that makes sense for your program.
 *          E.g. you can obtain the file descriptor for an open port using
 *          serial_get_port_handle() and use this to call select() or pselect(),
 *          with appropriate arrangements to return if a signal is received.
 *
 * @param[in] port Pointer to a port structure. Must not be NULL.
 * @param[in] buf Buffer containing the bytes to write. Must not be NULL.
 * @param[in] count Requested number of bytes to write.
 * @param[in] timeout_ms Timeout in milliseconds, or zero to wait indefinitely.
 *
 * @return The number of bytes written on success, or a negative error code.
 *         If the number of bytes returned is less than that requested, the
 *         timeout was reached before the requested number of bytes was
 *         written. If timeout is zero, the function will always return
 *         either the requested number of bytes or a negative error code. In
 *         the event of an error there is no way to determine how many bytes
 *         were sent before the error occurred.
 *
 * @since 0.1.0
 */
enum serial_return serial_blocking_write(struct serial_port *port, const void *buf, size_t count, unsigned int timeout_ms);

/**
 * Write bytes to the specified serial port, without blocking.
 *
 * Note that this function only ensures that the accepted bytes have been
 * written to the OS; they may be held in driver or hardware buffers and not
 * yet physically transmitted. To check whether all written bytes have actually
 * been transmitted, use the serial_output_waiting() function. To wait until all
 * written bytes have actually been transmitted, use the serial_drain() function.
 *
 * @param[in] port Pointer to a port structure. Must not be NULL.
 * @param[in] buf Buffer containing the bytes to write. Must not be NULL.
 * @param[in] count Maximum number of bytes to write.
 *
 * @return The number of bytes written on success, or a negative error code.
 *         The number of bytes returned may be any number from zero to the
 *         maximum that was requested.
 *
 * @since 0.1.0
 */
enum serial_return serial_nonblocking_write(struct serial_port *port, const void *buf, size_t count);

/**
 * Gets the number of bytes waiting in the input buffer.
 *
 * @param[in] port Pointer to a port structure. Must not be NULL.
 *
 * @return Number of bytes waiting on success, a negative error code otherwise.
 *
 * @since 0.1.0
 */
enum serial_return serial_input_waiting(struct serial_port *port);

/**
 * Gets the number of bytes waiting in the output buffer.
 *
 * @param[in] port Pointer to a port structure. Must not be NULL.
 *
 * @return Number of bytes waiting on success, a negative error code otherwise.
 *
 * @since 0.1.0
 */
enum serial_return serial_output_waiting(struct serial_port *port);

/**
 * Flush serial port buffers. Data in the selected buffer(s) is discarded.
 *
 * @param[in] port Pointer to a port structure. Must not be NULL.
 * @param[in] buffers Which buffer(s) to flush.
 *
 * @return serial_OK upon success, a negative error code otherwise.
 *
 * @since 0.1.0
 */
enum serial_return serial_flush(struct serial_port *port, enum serial_buffer buffers);

/**
 * Wait for buffered data to be transmitted.
 *
 * @warning If your program runs on Unix, defines its own signal handlers, and
 *          needs to abort draining the output buffer when when these are
 *          called, then you should not use this function. It repeats system
 *          calls that return with EINTR. To be able to abort a drain from a
 *          signal handler, you would need to implement your own blocking
 *          drain by polling the result of serial_output_waiting().
 *
 * @param[in] port Pointer to a port structure. Must not be NULL.
 *
 * @return serial_OK upon success, a negative error code otherwise.
 *
 * @since 0.1.0
 */
enum serial_return serial_drain(struct serial_port *port);

/**
 * @}
 *
 * @defgroup Waiting Waiting
 *
 * Waiting for events and timeout handling.
 *
 * @{
 */

/**
 * Allocate storage for a set of events.
 *
 * The user should allocate a variable of type struct serial_event_set *,
 * then pass a pointer to this variable to receive the result.
 *
 * The result should be freed after use by calling serial_free_event_set().
 *
 * @param[out] result_ptr If any error is returned, the variable pointed to by
 *                        result_ptr will be set to NULL. Otherwise, it will
 *                        be set to point to the event set. Must not be NULL.
 *
 * @return serial_OK upon success, a negative error code otherwise.
 *
 * @since 0.1.0
 */
enum serial_return serial_new_event_set(struct serial_event_set **result_ptr);

/**
 * Add events to a struct serial_event_set for a given port.
 *
 * The port must first be opened by calling serial_open() using the same port
 * structure.
 *
 * After the port is closed or the port structure freed, the results may
 * no longer be valid.
 *
 * @param[in,out] event_set Event set to update. Must not be NULL.
 * @param[in] port Pointer to a port structure. Must not be NULL.
 * @param[in] mask Bitmask of events to be waited for.
 *
 * @return serial_OK upon success, a negative error code otherwise.
 *
 * @since 0.1.0
 */
enum serial_return serial_add_port_events(struct serial_event_set *event_set,
    const struct serial_port *port, enum serial_event mask);

/**
 * Wait for any of a set of events to occur.
 *
 * @param[in] event_set Event set to wait on. Must not be NULL.
 * @param[in] timeout_ms Timeout in milliseconds, or zero to wait indefinitely.
 *
 * @return serial_OK upon success, a negative error code otherwise.
 *
 * @since 0.1.0
 */
enum serial_return serial_wait(struct serial_event_set *event_set, unsigned int timeout_ms);

/**
 * Free a structure allocated by serial_new_event_set().
 *
 * @param[in] event_set Event set to free. Must not be NULL.
 *
 * @since 0.1.0
 */
void serial_free_event_set(struct serial_event_set *event_set);


/**
 * Gets the status of the control signals for the specified port.
 *
 * The user should allocate a variable of type "enum serial_signal" and pass a
 * pointer to this variable to receive the result. The result is a bitmask
 * in which individual signals can be checked by bitwise OR with values of
 * the serial_signal enum.
 *
 * @param[in] port Pointer to a port structure. Must not be NULL.
 * @param[out] signal_mask Pointer to a variable to receive the result.
 *                         Must not be NULL.
 *
 * @return serial_OK upon success, a negative error code otherwise.
 *
 *
 */
enum serial_return serial_get_signals(struct serial_port *port, enum serial_signal *signal_mask);

/**
 * Put the port transmit line into the break state.
 *
 * @param[in] port Pointer to a port structure. Must not be NULL.
 *
 * @return serial_OK upon success, a negative error code otherwise.
 *
 * @since 0.1.0
 */
enum serial_return serial_start_break(struct serial_port *port);

/**
 * Take the port transmit line out of the break state.
 *
 * @param[in] port Pointer to a port structure. Must not be NULL.
 *
 * @return serial_OK upon success, a negative error code otherwise.
 *
 * @since 0.1.0
 */
enum serial_return serial_end_break(struct serial_port *port);


#ifdef __cplusplus
}
#endif

#endif
