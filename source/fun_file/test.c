
#include "file.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <errno.h>
#if defined (__linux__) || defined (__CYGWIN__)
#define __STDC_FORMAT_MACROS
#include <inttypes.h>
#endif


static void foo(void)
{
    int len = 0;
    int i = 0;
    file_backend_type type;
    struct file *fw;
    struct file *f;
    struct iovec *iobuf;
    char buf[128] = {0};
    for (i = 0; i < 2; ++i) {
        if (i == 0)
            type = FILE_BACKEND_IO;
        else if (i == 1)
            type = FILE_BACKEND_FIO;
        file_backend(type);
        printf("backend=%d\n", type);
        fw = file_open("lsusb", F_CREATE);
        file_write(fw, "hello file\n", 11);
        file_sync(fw);
        file_seek(fw, 0, SEEK_SET);
        memset(buf, 0, sizeof(buf));
        len = file_read(fw, buf, sizeof(buf));
        printf("read len = %d, buf = %s", len, buf);
        file_close(fw);

        f = file_open("lsusb", F_RDONLY);
        memset(buf, 0, sizeof(buf));
        len = file_read(f, buf, sizeof(buf));
        printf("read len = %d, buf = %s", len, buf);
        printf("len=%zu\n", file_get_size("lsusb"));

        iobuf = file_dump("lsusb");
        if (iobuf) {
            //printf("len=%zu, buf=%s\n", iobuf->iov_len, (char *)iobuf->iov_base);
        }
    }
}

static void foo2(void)
{
    struct file_systat *stat = file_get_systat("./Makefile");
    if (!stat)
        return;
    printf("total = %" PRIu64 "MB\n", stat->size_total/(1024*1024));
    printf("avail = %" PRIu64 "MB\n", stat->size_avail/(1024*1024));
    printf("free = %" PRIu64 "MB\n", stat->size_free/(1024*1024));
    printf("fs type name = %s\n", stat->fs_type_name);
}

static void foo3(void)
{
    printf("local path=%s\n", file_path_pwd());
    printf("suffix=%s\n", file_path_suffix(file_path_pwd()));
    printf("prefix=%s\n", file_path_prefix(file_path_pwd()));
}

static void foo4(void)
{
    int len = 0;
    char buf[128] = {0};
    struct file *fw = file_open("lsusb", F_APPEND);
    file_write(fw, "hello file\n", 11);
    file_sync(fw);
    file_seek(fw, 0, SEEK_SET);
    memset(buf, 0, sizeof(buf));
    len = file_read(fw, buf, sizeof(buf));
    printf("read len = %d, buf = %s", len, buf);
    file_close(fw);
}

static void foo5(void)
{
    struct file_info info;
    file_get_info("lsusb", &info);
    printf("info->size = %" PRIu64 "\n", info.size);
    printf("info->type = %d\n", info.type);
    printf("info->time_modify = %" PRIu64 "\n", info.modify_sec);
    printf("info->time_access = %" PRIu64 "\n", info.access_sec);
}

#ifdef ENABLE_FILEWATCHER
#define ROOT_DIR		"./"
static struct fw *_fw = NULL;


void on_change(struct fw *fw, enum fw_type type, char *path)
{
    switch (type) {
    case FW_CREATE_DIR:
        printf("[CREATE DIR] %s\n", path);
        break;
    case FW_CREATE_FILE:
        printf("[CREATE FILE] %s\n", path);
        break;
    case FW_DELETE_DIR:
        printf("[DELETE DIR] %s\n", path);
        break;
    case FW_DELETE_FILE:
        printf("[DELETE FILE] %s\n", path);
        break;
    case FW_MOVE_FROM_DIR:
        printf("[MOVE FROM DIR] %s\n", path);
        break;
    case FW_MOVE_TO_DIR:
        printf("[MOVE TO DIR] %s\n", path);
        break;
    case FW_MOVE_FROM_FILE:
        printf("[MOVE FROM FILE] %s\n", path);
        break;
    case FW_MOVE_TO_FILE:
        printf("[MOVE TO FILE] %s\n", path);
        break;
    case FW_MODIFY_FILE:
        printf("[MODIFY FILE] %s\n", path);
        break;
    default:
        break;
    }
}


int file_watcher_foo()
{
    _fw = fw_init(on_change);
    if (!_fw) {
        printf("fw_init failed!\n");
        return -1;
    }
    fw_add_watch_recursive(_fw, ROOT_DIR);
    fw_dispatch(_fw);
    return 0;
}
#endif

#if 0
/**
从FILE中按行读取数据存入buffer中
*/
#define LINE_BUF_SIZE_BYTES 256

static bool buffer_contains_newline(const char *const buffer)
{
    return strchr(buffer, (int)'\n') != NULL;
}

int read_line(FILE *const fhnd, char **const buffer, size_t *const usr_buff_size)
{
    int error_number;
    char *result;
    char *line_buff;
    char *buff_pos;
    size_t buff_size;

    /* The initial buffer size may be different to the expansion size so
     * for first iteration this is probably user defined, and then in latter
     * iterations is just LINE_BUF_SIZE_BYTES */
    size_t block_size;

    if (!buffer || !usr_buff_size) {
        return -EINVAL;
    }

    if (feof(fhnd)) {
        if (*buffer) {
            **buffer = '\0';
        }
        return 0;
    }

    if (!*buffer) {
        line_buff = malloc(LINE_BUF_SIZE_BYTES);
        if (!line_buff) {
            return -ENOMEM;
        }
        line_buff[0] = '\0';

        buff_size = LINE_BUF_SIZE_BYTES;
        block_size = LINE_BUF_SIZE_BYTES;
    }
    else {
        line_buff = *buffer;
        buff_size = *usr_buff_size;
        block_size = buff_size;
    }

    buff_pos = line_buff;

    while (1)
    {
        result = fgets(buff_pos, (int)block_size, fhnd);
        if (!result) {
            /* NULL means error or feof with zero chars read. */
            if (ferror(fhnd)) {
                goto _error;
            }
            /* else: covered by check for feof() at start of function */
        }

        if (feof(fhnd)) {
            break;
        }

        if (!buffer_contains_newline(buff_pos)) {
            /* No newline terminator found and no eof so buffer will be full - must resize so we
             * can continue to read in this line */
            char *tmp_buff_ptr = realloc(line_buff, buff_size + LINE_BUF_SIZE_BYTES);
            if (!tmp_buff_ptr)
                goto _error;

            line_buff  = tmp_buff_ptr;
            buff_pos   = line_buff + buff_size - 1u;
            buff_size += LINE_BUF_SIZE_BYTES;
            block_size = LINE_BUF_SIZE_BYTES + 1u;
        }
        else {
            break;
        }
    }

    *buffer = line_buff;
    *usr_buff_size = buff_size;
    return 1;

_error:
    error_number = errno;

    if (!*buffer) {
        /* We allocated the buffer, not the user, so just free it. */
        free(line_buff);
    }
    else  {
        /* The user allocated the buffer so return a pointer to the last
         * properly realloc()'ed buffer (not updated in the loop to save the
         * extra write */
        *buffer = line_buff;
    }

    return error_number;
}

#endif

int main(int argc, char **argv)
{
    uint64_t size = 1000;
    file_dir_size("./", &size);
    printf("folder_size=%" PRIu64 "\n", size);
    file_dir_tree("./");

    foo5();
    foo4();
    foo();
    foo2();
    foo3();
    if (0 != file_create("jjj.c")) {
        printf("file_create failed!\n");
    }
#ifdef ENABLE_FILEWATCHER
    file_watcher_foo();
#endif
    return 0;
}
