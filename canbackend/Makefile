all: server canclient

canclient: canclient.o
	gcc canclient.o -o canclient

server: server.o
	gcc server.o -o server

canclient.o: canclient.c
	gcc -c canclient.c

server.o: server.c
	gcc -c server.c

clean:
	rm -rf *.o canclient server
