#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include <iostream>
#include <map>
#include "server.h"
#include "protocol.h"
#include "network_util.h"

using std::map;
using std::set;
using std::string;
using std::to_string;
using std::cout;
using std::endl;

#define MAXDATASIZE 1024

int Server::registerServer(string id, int server_fd) {
	if(serverMap.count(id)) {
		// ID is already taken
		return -1;
	} else {
		serverMap[id] = server_fd;
		return 0;
	}
}

int Server::handleRequest(int requestFd) {

	cout << "Received new connection" << endl;

	char buf[MAXDATASIZE+1];
	string res;

	// Read in request bytes
	// Requests that follow protocol are less than MAXDATASIZE bytes
	int request_len = recv(requestFd, buf, MAXDATASIZE, 0);

	if(request_len < 0) {

		perror("recv");
		return -1;		

	}

	buf[request_len] = '\0';
	printf("Received %d bytes: %s\n", request_len, buf);

	Protocol::Request req(buf);

	// Type of request determines action taken
	if(req.isRegisterRequest()) {

		// REGISTER REQUEST
		// A new user-server is broadcasting its services
		// Record an association of the provided id and 
		// this new file descriptor
		if(registerServer(req.getServerID(), requestFd)) {

			// Failure to register means ID is taken
			res = protocol.makeResponse(false, "ID already registered");

		} else {

			// Registered!
			res = protocol.makeResponse(true, "");
		}
	
		cout << "Sending register response: " << res << endl;

		send(requestFd, res.c_str(), res.length(), 0);

	} else if(req.isUnregisterRequest()) {

		// UNREGISTER REQUEST
		// A user-server is no longer broadcasting its services
		// Remove from server map
		
		if(serverMap.count(req.getServerID())) {
	
			// Close connection to server
			close(serverMap[req.getServerID()]);

			// Erase ID association
			serverMap.erase(req.getServerID());	
		}

		string res = protocol.makeResponse(true, "");

		cout << "Sending unregister response: " << res << endl;

		send(requestFd, res.c_str(), res.length(), 0);

		close(requestFd);

	} else if(req.isConnectRequest()) {

		// CONNECT REQUEST
		// A new user-client is requesting to connect
		// to a registered server
		// Record this client's file descriptor 
		// and pass the request along to the registered server
		if(serverMap.count(req.getServerID())) {

			string clientID = to_string(requestFd);

			pendingConnections.insert(clientID);	

			int serverFd = serverMap[req.getServerID()];
			string proxyReq = protocol.makeProxyConnectRequest(clientID);

			cout << "Sending proxy connect request: " << proxyReq << endl;
			send(serverFd, proxyReq.c_str(), proxyReq.length(), 0);

		} else {

			// Attempt to connect to nonexistent server
			res = protocol.makeResponse(false, "ID not registered");

			cout << "Sending error connect response: " << res << endl;
			send(requestFd, res.c_str(), res.length(), 0);
			close(requestFd);
		}
		
	} else if(req.isConnectionCreatedRequest()) {

			// CONNECTION_CREATED REQUEST
			// A user-server is responding to a user-client's
			// connect request, via a new socket created for
			// their communication
			// Begin proxying their connection

			updateConnectionStatus(req.getClientID(), requestFd, true);

			close(requestFd);

	} else if(req.isConnectionFailedRequest()) {

			// CONNECTION_FAILED REQUEST
			// A user-server is responding to a user-client's
			// connect request, via a new socket
			// Notify client of failure

			updateConnectionStatus(req.getClientID(), requestFd, false);

			close(requestFd);

	} else {
	
		// UNEXPECTED REQUEST FORMAT
		res = protocol.makeResponse(false, "REGISTER or CONNECT request expected");

		cout << "Sending unexpected response: " << res << endl; 
		send(requestFd, res.c_str(), res.length(), 0);
		close(requestFd);
	}

	// TODO Create CONNECTION_FAILED request to indicate failure of
	// new connection to user-server


	return 0;
}

void Server::updateConnectionStatus(string clientID, int serverFd, bool success) {

	if(pendingConnections.count(clientID)) {

		int clientFd = atoi(clientID.c_str());

		if(!fork()) {

			// In child, inform client of connection status	
			if(success) {	

				string proxyRes = protocol.makeResponse(true, "");

				cout << "Notifying client of successful connection" << endl;
				send(clientFd, proxyRes.c_str(), proxyRes.length(), 0);

				cout << "Facilitating connection" << endl;

				// And facilitate connection
				NetworkUtil::facilitateConnection(clientFd, serverFd);

			} else {

				string proxyRes = 
					protocol.makeResponse(false, "Could not connect to service");

				cout << "Notifying client of failed connection" << endl;
				send(clientFd, proxyRes.c_str(), proxyRes.length(), 0);
				
				close(clientFd);
				close(serverFd);
			}
	
			exit(0);

		} else {
			
			// Connection is no longer pending, so parent can stop
			// keeping track of it
			pendingConnections.erase(clientID);
			close(clientFd);
		}

	} else {

		// Attempt to connect to nonexistent client
		string res = protocol.makeResponse(false, "ID not registered");

		cout << "Sending error response: " << res << endl;
		send(serverFd, res.c_str(), res.length(), 0);
	}
}
