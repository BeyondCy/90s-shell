all : shell_server clean

shell_server : server.o alive_client.o shell.o 90s\ shell\ server.o
	g++  server.o alive_client.o shell.o 90s\ shell\ server.o -lpthread -lreadline -o shell_server

90s\ shell\ server.o:
	g++ -c 90s\ shell\ server.cpp

shell.o:
	g++ -c shell.cpp

alive_client.o:
	g++ -c alive_client.cpp

server.o:
	g++ -c server.cpp
clean:
	rm -f *.o

