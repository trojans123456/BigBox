#include <stdio.h>
#include "http_buffer.h"
#include "http.h"

int test_main()
{
    http_buffer_t *buf = http_buffer_new(128);
    if(!buf)
        return 0;

    //http_buffer_append(buf,"xxx:%s\n","test",NULL);

    http_buffer_sprintf(buf,"jfsl%s","1");

    printf("%s \n",http_get_data(buf));

    printf("%s \n",random_string(12));
}
