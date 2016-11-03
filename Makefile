all:
	g++ -std=c++11 src/protocol.cpp src/server.cpp src/server_main.cpp -o server

clean:
	rm src/*.o server
