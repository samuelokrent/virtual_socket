#include <string>
#include <unistd.h>
#include "client_proxy.h"

using std::cout;
using std::endl;
using std::string;

#define CLIENT_PORT "6071"

ClientProxy::ClientProxy(string id) {
    this->id = id;
}

void ClientProxy::start() {

	// Create connection to proxy server
	if(connect() < 0) {
        cout << "Error: Could not connect to proxy server" << endl;
        return;
    }

	string connectReq = protocol.makeConnectRequest(id);
	Protocol::Response connectRes = sendRequest(connectReq);

	if(connectRes.isOK()) {

		// Start listening for the service-client connection
		run(CLIENT_PORT);

	} else {

		cout << "Error: " << connectRes.getErrorMessage() << endl;

		close(connection);
	}
}

int ClientProxy::handleRequest(int requestFd) {
	
	facilitateConnection(requestFd, connection);

	return 0;
}

void ClientProxy::facilitateConnection(int clientFd, int serverFd) {

	close(clientFd);
	close(serverFd);
	exit(0);
}
