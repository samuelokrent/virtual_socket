all: protocol.o server.o client.o
	g++ -std=c++11 -c src/server_main.cpp
	g++ -std=c++11 -c src/client_main.cpp
	g++ -std=c++11 protocol.o client.o client_main.o -o client
	g++ -std=c++11 protocol.o server.o server_main.o -o server
	rm *.o

tester:
	g++ -std=c++11 -c src/protocol.cpp
	g++ -std=c++11 -I/usr/local/Cellar/cppunit/1.13.2/include/cppunit -c test/*.cpp
	g++ -std=c++11 -o tester -L/usr/local/Cellar/cppunit/1.13.2/lib *.o -lCppUnit
	rm *.o

client: protocol.o client.o
	g++ -std=c++11 -c src/client_main.cpp
	g++ -std=c++11 protocol.o network_util.o client.o client_main.o -o client
	rm *.o

server: protocol.o server.o
	g++ -std=c++11 -c src/server_main.cpp
	g++ -std=c++11 protocol.o network_util.o server.o simple_server.o server_main.o -o server
	rm *.o

network_util.o:
	g++ -std=c++11 -c src/network_util.cpp

protocol.o:
	g++ -std=c++11 -c src/protocol.cpp

server.o: simple_server.o network_util.o
	g++ -std=c++11 -c src/server.cpp

simple_server.o:
	g++ -std=c++11 -c src/simple_server.cpp

client.o: network_util.o
	g++ -std=c++11 -c src/client.cpp

clean:
	rm *.o src/*.o server client tester
