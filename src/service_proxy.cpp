#include <string>
#include <iostream>
#include <unistd.h>
#include <sys/socket.h>
#include "service_proxy.h"
#include "network_util.h"

using std::string;
using std::cout;
using std::endl;

ServiceProxy::ServiceProxy(string id, string port) {
	this->id = id;
	this->port = port;
}

void ServiceProxy::start() {

	if(connect() < 0) {
		cout << "Error: Could not connect to proxy server" << endl;
		return;
	}

	string req = protocol.makeRegisterRequest(id);
	
	Protocol::Response res = sendRequest(req);

	if(!res.isOK()) {
		cout << "Error: " << res.getErrorMessage() << endl;
		return;
	}

	while(1) {
		// read connect request, facil
		Protocol::Request * connectReq = NetworkUtil::readRequest(connection);

		if(!connectReq) {
			cout << "Error reading connect request" << endl;
		} else {
		
			handleConnectRequest(connectReq);
		}
	}

}

void ServiceProxy::handleConnectRequest(Protocol::Request * connectReq) {

	string clientID = connectReq->getClientID();

	// Create new connection to this machine's service
	int serviceConnection = NetworkUtil::createConnection("localhost", port);

	if(serviceConnection < 0) {

		// TODO notify proxy server of failure
		cout << "Error: Could not connect to service on port " << port << endl;

	} else {

		// Create new connection to proxy server
		int newProxyConnection =
				NetworkUtil::createConnection(proxyHost, proxyPort);

		if(newProxyConnection < 0) {
			cout << "Error: " <<
				"Could not create new connection to proxy server" << endl;
		} else {

			if(!fork()) {
				// Inform proxy server of successful connection
				string conCreated =
					protocol.makeConnectionCreatedRequest(clientID);
				send(newProxyConnection,
					conCreated.c_str(), conCreated.length(), 0);

				// And begin facilitating connection
				facilitateConnection(newProxyConnection, serviceConnection);

			} else {
				close(newProxyConnection);
			}

		}

		close(serviceConnection);
	}

	delete connectReq;
}

void ServiceProxy::facilitateConnection(int clientFd, int serverFd) {

	// TODO facilitate connection

	close(clientFd);
	close(serverFd);
	exit(0);
}
