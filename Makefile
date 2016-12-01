all: sp spd
	rm *.o

tester:
	g++ -std=c++11 -c src/protocol.cpp
	g++ -std=c++11 -I/usr/local/Cellar/cppunit/1.13.2/include/cppunit -c test/*.cpp
	g++ -std=c++11 -o tester -L/usr/local/Cellar/cppunit/1.13.2/lib *.o -lCppUnit

sp: protocol.o client.o service_proxy.o client_proxy.o simple_server.o
	g++ -std=c++11 -c src/client_main.cpp
	g++ -std=c++11 -pthread protocol.o network_util.o client.o service_proxy.o client_proxy.o client_main.o simple_server.o -o sp

spd: protocol.o server.o
	g++ -std=c++11 -c src/server_main.cpp
	g++ -std=c++11 -pthread protocol.o network_util.o server.o simple_server.o server_main.o -o spd

network_util.o:
	g++ -std=c++11 -pthread -c src/network_util.cpp

protocol.o:
	g++ -std=c++11 -c src/protocol.cpp

server.o: simple_server.o network_util.o
	g++ -std=c++11 -c src/server.cpp

simple_server.o:
	g++ -std=c++11 -c src/simple_server.cpp

client_proxy.o:
	g++ -std=c++11 -c src/client_proxy.cpp

service_proxy.o:
	g++ -std=c++11 -c src/service_proxy.cpp

client.o: network_util.o
	g++ -std=c++11 -c src/client.cpp

clean:
	rm *.o src/*.o server client tester
