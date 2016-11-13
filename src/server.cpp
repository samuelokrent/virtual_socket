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
		if(registerServer(req.getServerID(), requestFd)) {

			// Failure to register means ID is taken
			res = protocol.makeResponse(false, "ID already registered", req.getServerID(), "");

		} else {

			// Registered!
			res = protocol.makeResponse(true, "", req.getServerID(), "");
		}

		send(requestFd, res.c_str(), res.length(), 0);

	} else if(req.isConnectRequest()) {

		// CONNECT REQUEST
		if(serverMap.count(req.getServerID())) {

			int serverFd = serverMap[req.getServerID()];

			if(!fork()) {
				// In child, facilitate connection
				createVirtualConnection(requestFd, serverFd);
			}			
			// Parent should not send a response

		} else {

			// Attempt to connect to nonexistent server
			res = protocol.makeResponse(false, "ID not registered", "", "");
			send(requestFd, res.c_str(), res.length(), 0);
		}
		
		close(requestFd);

	} else {
	
		// UNEXPECTED REQUEST FORMAT
		res = protocol.makeResponse(false, "REGISTER or CONNECT request expected", 
										"", "");
		send(requestFd, res.c_str(), res.length(), 0);
		close(requestFd);
	}


	return 0;
}

void Server::createVirtualConnection(int clientFd, int serverFd) {

	Protocol::Response serverRes;
	Protocol::Request clientReq;
	string  proxyReq, proxyRes;
	char buf[MAXDATASIZE+1];
	int bytes_read;

	proxyReq = protocol.makeProxyConnectRequest(to_string(clientFd));

	serverRes = NetworkUtil::forwardResponse(clientFd, serverFd, proxyReq);

	while(serverRes.isOK()) {

		bytes_read = recv(clientFd, buf, MAXDATASIZE, 0);
		if(bytes_read == 0)
			break;
		buf[bytes_read] = '\0';
		clientReq = Protocol::Request(buf); 

		if(!clientReq.isDataRequest())
			break;
		proxyReq = protocol.makeProxyDataRequest(to_string(clientFd),
								clientReq.getData());

		serverRes = NetworkUtil::forwardResponse(clientFd, serverFd, proxyReq);

	}
	
	close(clientFd);
	close(serverFd);
	exit(0);	
}
