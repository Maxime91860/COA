all:  plugin_max.so plugin_max.pgr


#TP2_1.pgr
CPP=g++
CC=gcc

MPI_PATH=`dirname \` which mpicc \` `
MPI_FLAGS=-I$(MPI_PATH)/../include/mpi -L$(MPI_PATH)/../lib -lmpi
PLUGIN_FLAGS=-I`gcc -print-file-name=plugin`/include -g -Wall -fno-rtti -shared -fPIC

CFLAGS=-g


libplugin.so: plugin_TP2_1.cpp
	 $(CPP) $(PLUGIN_FLAGS) -o libplugin.so $<

%.so: plugin_%.cpp
	$(CPP) $(PLUGIN_FLAGS) -o $@ $<

plugin_max.so: plugin_max.cpp
	$(CPP) $(PLUGIN_FLAGS) -o $@ $<


%.pgr: %.so test.c
	gcc test.c $(CFLAGS) -o $@ $(MPI_FLAGS) -fplugin=./$< 

clean:
	rm -rf *.pgr

clean_all: clean
	rm -rf *.so *.dot
