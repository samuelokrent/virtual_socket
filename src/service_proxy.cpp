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

	cout << "Starting service proxy..." << endl;

	if(connect() < 0) {
		cout << "Error: Could not connect to proxy server" << endl;
		return;
	}

	string req = protocol.makeRegisterRequest(id);
	
	cout << "Sending register request..." << endl;

	Protocol::Response res = sendRequest(req);

	if(!res.isOK()) {
		cout << "Error: " << res.getErrorMessage() << endl;
		return;
	}

	cout << "Registered. Entering connection loop." << endl;

	while(1) {
		// read connect request, facil
		Protocol::Request * connectReq = NetworkUtil::readRequest(connection);

		if(!connectReq) {
			cout << "Error reading connect request" << endl;
		} else {
			
			cout << "New connection request from " << connectReq->getClientID() << endl;		
			handleConnectRequest(connectReq);
		}
	}

}

void ServiceProxy::handleConnectRequest(Protocol::Request * connectReq) {

	string clientID = connectReq->getClientID();

	cout << "Connecting to service..." << endl;
	// Create new connection to this machine's service
	int serviceConnection = NetworkUtil::createConnection("localhost", port);

	if(serviceConnection < 0) {

		// TODO notify proxy server of failure
		cout << "Error: Could not connect to service on port " << port << endl;

	} else {

		cout << "Making new connection to proxy server..." << endl;
		// Create new connection to proxy server
		int newProxyConnection =
				NetworkUtil::createConnection(proxyHost, proxyPort);

		if(newProxyConnection < 0) {
			cout << "Error: " <<
				"Could not create new connection to proxy server" << endl;
		} else {

			if(!fork()) {

				cout << "Connected successfully. Notifying proxy server" << endl;
				// Inform proxy server of successful connection
				string conCreated =
					protocol.makeConnectionCreatedRequest(clientID);
				send(newProxyConnection,
					conCreated.c_str(), conCreated.length(), 0);

				cout << "Facilitating connection" << endl;
				// And begin facilitating connection
				NetworkUtil::facilitateConnection(newProxyConnection, serviceConnection);

				exit(0);

			} else {
				close(newProxyConnection);
			}

		}

		close(serviceConnection);
	}

	delete connectReq;
}

