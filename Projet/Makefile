CPP=g++
CC=gcc

FILE=test4.c

MPI_PATH=`dirname \` which mpicc \` `
MPI_FLAGS=-I$(MPI_PATH)/../include/mpi -L$(MPI_PATH)/../lib -lmpi
LIB_FLAGS=-g -Wall -shared -fPIC
PLUGIN_FLAGS=-I`gcc -print-file-name=plugin`/include -fno-rtti $(LIB_FLAGS)

CFLAGS=-g -O3

main : test.pgr


libplugin.so: plugin_appel_mpi.cpp
	 $(CPP) $< $(PLUGIN_FLAGS) -o libplugin.so

%.pgr: %.c libplugin.so 
	$(CC) $< $(CFLAGS) $(MPI_FLAGS) -fplugin=./libplugin.so -o EXECUTABLES/$@

gen_pdf: 
	./gen_pdf.sh

clean :
	rm -rf EXECUTABLES/* FICHIERS_GRAPHVIZ/* libplugin.so

clean_plot :
	rm -rf FICHIERS_PLOT_GRAPHVIZ/*