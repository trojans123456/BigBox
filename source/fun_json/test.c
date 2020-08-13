#include <stdio.h>
#include "json_config.h"

int main(int argc,char *argv[])
{

    const char *json_str = "{   \
                       \"employees\": [ \
                           {\
                               \"firstName\": \"Bill\", \
                               \"lastName\": \"Gates\" \
                           },\
                           { \
                               \"firstName\": \"George\", \
                               \"lastName\": \"Bush\" \
                           },\
                           { \
                               \"firstName\": \"Thomas\",\
                               \"lastName\": \"Carter\" \
                           }\
                       ]\
                   }";


    cJSON *root = cJSON_Parse(json_str);
    if(!root)
    {
        printf("json parser faield\n");
        return 0;
    }
    printf("type = %d\n",root->type);
    if(root->type == cJSON_Object)
    {
        cJSON *json = cJSON_GetObjectItem(root,"employees");
        if(!json)
        {
            printf("....\n");
            return 0;
        }
        cJSON *value,*array;
        int num = cJSON_GetArraySize(json);
        int i;
        for(i = 0;i < num;i++)
        {
            array = cJSON_GetArrayItem(json,i);
            value = cJSON_GetObjectItem(array,"firstName");
            if(value)
            {
                printf("name = %s value = %s\n",value->string,value->valuestring);
            }

            value = cJSON_GetObjectItem(array,"lastName");
            if(value)
            {
                printf("name = %s value = %s\n",value->string,value->valuestring);
            }
        }
    }

    return 0;
}
