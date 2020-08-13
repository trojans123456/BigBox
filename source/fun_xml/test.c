#include <stdio.h>
#include <string.h>

#include "xml_config.h"

int main(int argc,char *argv[])
{
    char buffer[16384] = "";
    char line[1024 + 1] = "";

    FILE *file = fopen("test_xml.xml","r");
    if(!file)
    {
        printf("open failed\n");
        return 0;
    }

    int pos = 0;
    while(fgets(buffer + pos,1024,file) != NULL)
    {
        pos += strlen(buffer + pos);
    }

    fclose(file);

    printf("buffer = %s pos = %d\n",buffer,pos);

    struct xml_node *root = xml_parse(buffer,pos);
    if(!root)
    {
        printf("xml parser failed\n");
        return 0;
    }

    printf("%s \n",xml_dump_tree(root));

    struct xml_node_set *node = xml_find_element(root,"topic");
    if(node)
    {
        printf("name = %s\n",xml_get_attribute(*node->node,"name"));
        printf("count = %d\n",node->count);
    }




}
