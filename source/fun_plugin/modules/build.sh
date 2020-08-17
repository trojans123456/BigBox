#!/bin/sh

if [ -e plugin_test1.so ];then
	rm -fr plugin_test1.so
fi

if [ -e plugin_test2.so ];then
	rm -fr plugin_test2.so
fi

if [ -e plugin_test3.so ];then
	rm -fr plugin_test3.so
fi

gcc -fPIC -shared plugin_test1.c -o plugin_test1.so

gcc -fPIC -shared plugin_test2.c -o plugin_test2.so

gcc -fPIC -shared plugin_test3.c -o plugin_test3.so
