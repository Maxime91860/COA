

all: first_plugin.so

#L'endroit où il y a gcc-plugin.h
PATH_PLUGIN=-I`gcc -print-file-name=plugin`/include -g -Wall -fno-rtti -shared -fPIC
FLAGS_LD= -shared -fPIC

first_plugin.o : first_plugin.cpp	
	g++ gcc $^ $(PATH_PLUGIN) -O3 -c

first_plugin.so : first_plugin.o
	g++ gcc $^ -O3 $(FLAGS_LD) -o $@

clean : 
	rm -rf first_plugin.so first_plugin.o