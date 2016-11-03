all: server client

tester:
	g++ -std=c++11 -c src/protocol.cpp src/server.cpp
	g++ -std=c++11 -I/usr/local/Cellar/cppunit/1.13.2/include/cppunit -c test/*.cpp
	g++ -std=c++11 -o tester -L/usr/local/Cellar/cppunit/1.13.2/lib *.o -lCppUnit
	rm *.o

client: protocol.o client.o
	g++ -std=c++11 -c src/client_main.cpp
	g++ -std=c++11 protocol.o client.o client_main.o -o client
	rm *.o

server: protocol.o server.o
	g++ -std=c++11 -c src/server_main.cpp
	g++ -std=c++11 protocol.o server.o server_main.o -o server
	rm *.o

protocol.o:
	g++ -std=c++11 -c src/protocol.cpp

server.o:
	g++ -std=c++11 -c src/server.cpp

client.o:
	g++ -std=c++11 -c src/client.cpp

clean:
	rm *.o src/*.o server client tester
