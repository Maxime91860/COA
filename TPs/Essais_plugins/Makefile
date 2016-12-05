

all: first_plugin.so

#L'endroit où il y a gcc-plugin.h
PATH_PLUGIN=`gcc -print-file-name=plugin`/include
FLAGS_LD= -shared -fPIC

first_plugin.o : first_plugin.c
	gcc $^ -I$(PATH_PLUGIN) -O3 -c

first_plugin.so : first_plugin.o
	gcc $^ -O3 $(FLAGS_LD) _o $@


clean : 
	rm -rf first_plugin