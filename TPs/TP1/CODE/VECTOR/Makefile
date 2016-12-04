

main : exec

exec : vector.pgr
	./vector.pgr


#Variables personnalisées
CC=gcc
CFLAGS= -O3
LD_FLAGS= -L. -lvector

# $@ : réference à la cible de la ligne
# $^ : référence à l'ensemble des dépendance
# $< : référence à la 1ère dépendance
# %  : % equivalent * du bash

# vector.pgr : compute.o main.o libvector.so
# 	gcc_coa compute.o main.o libvector.so -O3 -o vector.pgr

vector.pgr : compute.o main.o libvector.so
	$(CC) $^ $(CFLAGS) -o $@


# compute.o : compute.c
#	gcc_coa -c compute.c

# main.o : main.c
#	gcc_coa -c main.c

%.o : %.c
	$(CC) $^ $(CFLAGS) -c -o $@

#libvector.so : vector.o
#	gcc_coa vector.c -O3 -shared -fPIC -o libvector.so

libvector.so : vector.o
	$(CC) $^ $(CFLAGS) -shared -fPIC -o $@
	
clean : 
	rm -rf  vector.pgr *.o

clean_all : clean
	rm -rf libvector.so