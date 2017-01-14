all: TP4_1.pgr

CPP=g++
CC=gcc

FILE=test3.c

MPI_PATH=`dirname \` which mpicc \` `
MPI_FLAGS=-I$(MPI_PATH)/../include/mpi -L$(MPI_PATH)/../lib -lmpi
PLUGIN_FLAGS=-I`gcc -print-file-name=plugin`/include -g -Wall -fno-rtti -shared -fPIC

CFLAGS=-g -O3


libplugin.so: plugin_TP2_1.cpp
	 $(CPP) $(PLUGIN_FLAGS) -o libplugin.so $<

%.so: plugin_%.cpp
	$(CPP) $(PLUGIN_FLAGS) -o $@ $<

%.pgr: %.so $(FILE)
	gcc $(FILE) $(CFLAGS) -o $@ $(MPI_FLAGS) -fplugin=./$< 

%.pdf: %.dot
	dot -Tpdf $< -o $@

clean:
	rm -rf *.pgr *.pdf

clean_all: clean
	rm -rf libplugin.so *.dot
