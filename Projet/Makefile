CPP=g++
CC=gcc


MPI_PATH=`dirname \` which mpicc \` `
MPI_FLAGS=-I$(MPI_PATH)/../include/mpi -L$(MPI_PATH)/../lib -lmpi
LIB_FLAGS=-g -Wall -shared -fPIC
PLUGIN_FLAGS=-I`gcc -print-file-name=plugin`/include -fno-rtti $(LIB_FLAGS)

CFLAGS=-g -O3

main : test.pgr test2.pgr test3.pgr test4.pgr


libplugin.so: plugin_appel_mpi.cpp
	 $(CPP) $< $(PLUGIN_FLAGS) -o libplugin.so

libcc.so: checking_collectives.c
	$(CC) $(LIB_FLAGS) $(MPI_FLAGS) -o $@ $<

%.pgr: %.c libplugin.so libcc.so
	$(CC) $< $(CFLAGS) $(MPI_FLAGS) -fplugin=./libplugin.so -L. -lcc -o EXECUTABLES/$@

gen_pdf: 
	./gen_pdf.sh

clean :
	rm -rf EXECUTABLES/* FICHIERS_GRAPHVIZ/* libplugin.so libcc.so

clean_plot :
	rm -rf FICHIERS_PLOT_GRAPHVIZ/*