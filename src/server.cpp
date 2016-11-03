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
#include <utility>
#include "server.h"
#include "protocol.h"

using std::map;
using std::pair;
using std::make_pair;
using std::string;
using std::cout;
using std::endl;

// Simple server code taken from here: 
// http://beej.us/guide/bgnet/output/html/multipage/clientserver.html#simpleserver

void sigchld_handler(int s) {
	// waitpid() might overwrite errno, so we save and restore it:
	int saved_errno = errno;

	while(waitpid(-1, NULL, WNOHANG) > 0);

	errno = saved_errno;
}

// get sockaddr, IPv4 or IPv6:
void * Server::get_in_addr(struct sockaddr *sa) {
	if (sa->sa_family == AF_INET) {
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}

	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

// Taken from http://stackoverflow.com/questions/2371910/how-to-get-the-ip-address-and-port-number-from-addrinfo-in-unix-c
// get port, IPv4 or IPv6:
in_port_t Server::get_in_port(struct sockaddr *sa) {
	if (sa->sa_family == AF_INET) {
		return (((struct sockaddr_in*)sa)->sin_port);
	}

	return ntohs(((struct sockaddr_in6*)sa)->sin6_port);
}

int Server::registerServer(string id, string host, in_port_t port) {
	if(serverMap.count(id)) {
		// ID is already taken
		return -1;
	} else {
		serverMap[id] = make_pair(host, port);
		return 0;
	}
}

void Server::run(string port) {
	
	int sockfd;  // listen on sock_fd
	struct addrinfo hints, *servinfo, *p;
	struct sigaction sa;
	int yes=1;
	int rv;

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE; // use my IP

	if ((rv = getaddrinfo((char *) NULL, port.c_str(), &hints, &servinfo)) != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		return;
	}

	// loop through all the results and bind to the first we can
	for(p = servinfo; p != NULL; p = p->ai_next) {
		if ((sockfd = socket(p->ai_family, p->ai_socktype,
				p->ai_protocol)) == -1) {
			perror("server: socket");
			continue;
		}

		if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes,
				sizeof(int)) == -1) {
			perror("setsockopt");
			exit(1);
		}

		if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
			close(sockfd);
			perror("server: bind");
			continue;
		}

		break;
	}

	freeaddrinfo(servinfo); // all done with this structure

	if (p == NULL)  {
		fprintf(stderr, "server: failed to bind\n");
		exit(1);
	}

	// Accept up to 10 connections
	if (listen(sockfd, 10) == -1) {
		perror("listen");
		exit(1);
	}

	sa.sa_handler = sigchld_handler; // reap all dead processes
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_RESTART;
	if (sigaction(SIGCHLD, &sa, NULL) == -1) {
		perror("sigaction");
		exit(1);
	}

	printf("server: waiting for connections...\n");

	acceptConnections(sockfd);
}

void Server::acceptConnections(int sockfd) {

	socklen_t sin_size;
	struct sockaddr_storage client_addr; // connector's address information
	int client_fd;
	char client_host[INET6_ADDRSTRLEN];
	in_port_t client_port;

	// Continuously accept connections from clients
	while(1) {

		// Obtain file descriptor for new connection
		sin_size = sizeof client_addr;
		client_fd = accept(sockfd, (struct sockaddr *)&client_addr, &sin_size);
		if (client_fd == -1) {
			perror("accept");
			continue;
		}

		// Obtain client hostname and portnumber
		inet_ntop(client_addr.ss_family,
			get_in_addr((struct sockaddr *) &client_addr),
			client_host, sizeof client_host);
		client_port = get_in_port((struct sockaddr *) &client_addr);

		printf("server: received connection from %s:%d\n", 
			client_host, ntohs(client_port));

		// Process and respond to new request
		if(handleRequest(client_fd, string(client_host), client_port) != 0) {
			fprintf(stderr, "server: error handling request\n");
		}
	}

}

int Server::handleRequest(int requestFd, string host, in_port_t port) {

	char buf[1025];
	string res;

	// Read in request bytes
	// Requests that follow protocol are less than 1024 bytes
	int request_len = recv(requestFd, buf, 1024, 0);

	if(request_len < 0) {

		perror("recv");
		return -1;		

	} else {

		buf[request_len] = '\0';
		printf("Received %d bytes: %s\n", request_len, buf);

		Protocol::Request req(buf);

		// Type of request determines action taken
		if(req.isRegisterRequest()) {

			if(registerServer(req.getID(), host, port)) {

				// Failure to register means ID is taken
				res = protocol.makeResponse(false, "ID already registered", host, ntohs(port));

			} else {

				res = protocol.makeResponse(true, "", host, ntohs(port));
			}

			send(requestFd, res.c_str(), res.length(), 0);

		} else if(req.isConnectRequest()) {

			// If ID has been registered, respond with registered host/port
			// (Full implementation should create a new virtual connection
			//    between client and registered server)
			if(serverMap.count(req.getID())) {

				pair<string, in_port_t> serverInfo = serverMap[req.getID()];
				res = protocol.makeResponse(true, "",
					serverInfo.first, ntohs(serverInfo.second));

			} else {

				res = protocol.makeResponse(false, "ID not registered", "", -1);
			}

			send(requestFd, res.c_str(), res.length(), 0);

		}

	}

	// Finished with this connection
	close(requestFd);
	return 0;
}

