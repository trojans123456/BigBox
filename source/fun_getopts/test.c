#include <stdio.h>
#include <getopt.h>

int main(int argc,char *argv[])
{
    struct option longopts[] =
    {
        {
            "opt-1",required_argument,NULL,'f'
        },
        {
            "help",no_argument,NULL,'h'
        },
        {
            0,0,0,0
        }
    };

    int ch;
    int option_index = 0;
    char file_name[32] = "";

    while((ch = getopt_long(argc,argv,"f:h",longopts,&option_index)) != -1)
    {
        switch(ch)
        {
        case 'f':
            /*操作optarg 全局变量*/
            strncpy(file_name,optarg,32);
            break;
        case 'h':
            break;
        default:
            break;
        }
    }
}

int main(int argc, char *argv[])
{
    char opt;
    while((opt = getopt(argc,argv,"f:s:cdu")) != -1)
    {
        switch(opt)
        {
        case 'c':
        case 'd':
        case 'u':
            printf("%d\n",atoi(optarg));
            break;
        case 's':
            break;
        case '?':
            break;
        default:
            break;
        }
    }

    for(;optind < argc;optind++)
    {
        printf("%s\n",argv[optind]);
    }
    return 0;
}
