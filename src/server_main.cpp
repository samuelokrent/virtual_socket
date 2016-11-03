#include <iostream>
#include <string>
#include "server.h"

int main(int argc, char * argv[]) {

	std::string port;

	if(argc == 1) {
		port = "6070";
	} else if (argc == 2) {
		port = string(argv[1]);
	} else {
		printf("usage: %s [PORTNUM]\n", argv[0]);
		return -1;
	}

	Server s;
	s.run(port);

	return 0;
}
