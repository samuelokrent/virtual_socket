#include "protocol.h"
#include "client.h"
#include "client_proxy.h"
#include "service_proxy.h"
#include <string>
#include <iostream>

using std::string;
using std::cout;

const string CONFIG_FILE = ".config";

int main(int argc, char * argv[]) {

	Client * client;

	if(argc == 4 && !strcmp(argv[1], "register")) {

		client = new ServiceProxy(argv[2], argv[3]);

	} else if (argc == 3 && !strcmp(argv[1], "connect")) {

		client = new ClientProxy(argv[2]);

	} else {
		printf("usage: %s register|connect <server_id> [portnum]\n", argv[0]);
		return 1;
	}

	if(client->loadConfigFile(CONFIG_FILE) < 0) {
		delete client;
		exit(1);
	}

	client->start();

	delete client;

	return 0;

}
