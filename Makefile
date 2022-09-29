all: ftps ftpc ftp.zip

ftp.zip: Makefile ftpc.c ftps.c
	zip lab1.zip Makefile ftps.c ftpc.c

ftps: ftps.o 
	gcc -o ftps ftps.o 

ftpc: ftpc.o
	gcc -o ftpc ftpc.o

ftps.o: ftps.c
	gcc -c ftps.c

ftpc.o: ftpc.c
	gcc -c ftpc.c

clean:
	rm -rf *.o ftpc ftps lab1.zip

