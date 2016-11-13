#include "protocol.h"
#include "client.h"
#include <string>
#include <iostream>

using std::string;
using std::cout;

int main(int argc, char * argv[]) {

	string req;
	Protocol protocol;

	if(argc == 4 && !strcmp(argv[1], "register")) {
		req = protocol.makeRegisterRequest(string(argv[2]));
	} else if (argc == 3 && !strcmp(argv[1], "connect")) {
		req = protocol.makeConnectRequest(string(argv[2]));
	} else {
		printf("usage: %s register|connect <server_id> [portnum]\n", argv[0]);
		return 1;
	}

	Client client;
	if(client.loadConfigFile(".config") < 0) {
		exit(1);
	}

	Protocol::Response res = client.sendRequest(req);

	if(res.isOK()) {
		printf("OK\n");
	} else {
		printf("Error: %s\n", res.getErrorMessage().c_str());
	}

	return 0;

}
