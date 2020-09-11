#include <stdio.h>

#include "inirw.h"

int main()
{
	const char *file = "config.ini";
	
	char *sect;
	char *key;
	char value[256];
	int intval;

	printf("load file %s\n\n", file);
    ini_t *a_ini = parser_ini_load(file);
    if(!a_ini)
        return 0;

	sect = 0;
	key = "_str001";
    iniGetString(a_ini,sect, key, value, sizeof(value), "notfound!");
	printf("[%s] %s = %s\n", sect, key, value);

	sect = "sect1";
	key = "str001";
    iniGetString(a_ini,sect, key, value, sizeof(value), "notfound!");
	printf("[%s] %s = %s\n", sect, key, value);

	sect = "sect1";
	key = "str005";
    iniGetString(a_ini,sect, key, value, sizeof(value), "notfound!");
	printf("[%s] %s = %s\n", sect, key, value);

	sect = "sect2";
	key = "str002";
    iniGetString(a_ini,sect, key, value, sizeof(value), "notfound!");
	printf("[%s] %s = %s\n", sect, key, value);

	sect = "sect2";
	key = "int002";
    intval = iniGetInt(a_ini,sect, key, 1000);
	printf("[%s] %s = %d\n", sect, key, intval);

    iniSetString(a_ini,"sect2", "str002", "\"v'a;l;u#e'002\"");
    iniSetString(a_ini,"sect2", "str003", "value003");

    iniSetInt(a_ini,"sect2", "int001", 100, 0);
    iniSetInt(a_ini,"sect2", "int002", 200, 16);
    iniSetInt(a_ini,"sect2", "int003", 300, 8);

	getchar();

    parser_ini_free(a_ini);
}
