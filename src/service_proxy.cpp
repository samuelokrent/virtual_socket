#include <string>
#include <iostream>
#include <unistd.h>
#include <sys/socket.h>
#include <csignal>
#include "service_proxy.h"
#include "network_util.h"

using std::string;
using std::cout;
using std::endl;

ServiceProxy * serviceProxy = NULL;

void interrupt_handler(int signum) {
	if(serviceProxy) {
		serviceProxy->shutdown();
	}
}

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

	// Unregister service on shutdown
	serviceProxy = this;
	signal(SIGINT, interrupt_handler);

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

	cout << "Making new connection to proxy server..." << endl;
	// Create new connection to proxy server
	int newProxyConnection =
			NetworkUtil::createConnection(proxyHost, proxyPort);

	if(newProxyConnection < 0) {

		cout << "Error: " <<
			"Could not create new connection to proxy server" << endl;

	} else {

		// Spawn new process to manage connections
		if(!fork()) {

			cout << "Connecting to service..." << endl;
			// Create new connection to this machine's service
			int serviceConnection = NetworkUtil::createConnection("localhost", port);

			if(serviceConnection < 0) {

				cout << "Error: Could not connect to service on port " << port << endl;
				// Inform proxy server of failure to connect
				string conFailed =
					protocol.makeConnectionFailedRequest(clientID);
				send(newProxyConnection,
					conFailed.c_str(), conFailed.length(), 0);
				
				close(newProxyConnection);

			} else {

				cout << "Connected successfully. Notifying proxy server" << endl;
				// Inform proxy server of successful connection
				string conCreated =
					protocol.makeConnectionCreatedRequest(clientID);
				send(newProxyConnection,
					conCreated.c_str(), conCreated.length(), 0);

				cout << "Facilitating connection" << endl;
				// And begin facilitating connection
				NetworkUtil::facilitateConnection(newProxyConnection, serviceConnection);

			}
			
			exit(0);

		} else {

			// Parent process, close socket
			close(newProxyConnection);
		}
	}

	delete connectReq;
}

void ServiceProxy::shutdown() {

	cout << "Shutting down..." << endl;

	int newProxyConnection =
            NetworkUtil::createConnection(proxyHost, proxyPort);

	if(newProxyConnection > 0) {

		// Notify proxy server that this service is no longer available
		string unregister =
				protocol.makeUnregisterRequest(id);
		send(newProxyConnection,
			unregister.c_str(), unregister.length(), 0);

		close(newProxyConnection);
	}

	if(connection > 0) close(connection);

	exit(0);
}
