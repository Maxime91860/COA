all: TP7_1.pgr

CPP=g++
CC=gcc

FILE=test4.c

MPI_PATH=`dirname \` which mpicc \` `
MPI_FLAGS=-I$(MPI_PATH)/../include/mpi -L$(MPI_PATH)/../lib -lmpi
LIB_FLAGS=-g -Wall -shared -fPIC
PLUGIN_FLAGS=-I`gcc -print-file-name=plugin`/include -fno-rtti $(LIB_FLAGS)

CFLAGS=-g -O3


libplugin.so: plugin_TP2_1.cpp
	 $(CPP) $(PLUGIN_FLAGS) -o libplugin.so $<

libcc.so: checking_collectives.c
	$(CC) $(LIB_FLAGS) $(MPI_FLAGS) -o $@ $<

%.so: plugin_%.cpp
	$(CPP) $(PLUGIN_FLAGS) -o $@ $<

%.pgr: %.so $(FILE) libcc.so 
	$(CC) $(FILE) $(CFLAGS) -o $@ $(MPI_FLAGS) -fplugin=./$< -L. -lcc

%.pdf: %.dot
	dot -Tpdf $< -o $@

clean:
	rm -rf *.pgr *.pdf

clean_all: clean
	rm -rf libplugin.so *.dot
