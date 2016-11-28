#include <string>
#include <unistd.h>
#include "client_proxy.h"
#include "network_util.h"

using std::cout;
using std::endl;
using std::string;

#define CLIENT_PORT "6071"

ClientProxy::ClientProxy(string id) {
    this->id = id;
}

void ClientProxy::start() {

	cout << "Starting client proxy" << endl;

	// Create connection to proxy server
	if(connect() < 0) {
        cout << "Error: Could not connect to proxy server" << endl;
        return;
    }

	cout << "Sending connect request..." << endl;

	string connectReq = protocol.makeConnectRequest(id);
	Protocol::Response connectRes = sendRequest(connectReq);

	if(connectRes.isOK()) {

		cout << "Connected. Start listening for connections on port " << CLIENT_PORT << endl;		

		// Start listening for the service-client connection
		run(CLIENT_PORT);

	} else {

		cout << "Error: " << connectRes.getErrorMessage() << endl;

		close(connection);
	}
}

int ClientProxy::handleRequest(int requestFd) {

	cout << "Received connection. Begin facilitating" << endl;
	
	NetworkUtil::facilitateConnection(requestFd, connection);

	exit(0);

	return 0;
}
