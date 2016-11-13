#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include "protocol.h"
#include "client.h"
#include "network_util.h"
#include <string>
#include <iostream>
#include <fstream>

using std::string;
using std::cout;
using std::endl;
using std::ifstream;

#define MAXDATASIZE 1024

// Simple client code taken from here:
// http://beej.us/guide/bgnet/output/html/multipage/clientserver.html#simpleserver

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
	if (sa->sa_family == AF_INET) {
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}

	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

Protocol::Response Client::sendRequest(string req) {

	int sockfd, numbytes;  
	char buf[MAXDATASIZE + 1];
	struct addrinfo hints, *servinfo, *p;
	int rv;
	char s[INET6_ADDRSTRLEN];

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	if ((rv = getaddrinfo(proxyHost.c_str(), proxyPort.c_str(), 
			&hints, &servinfo)) != 0) {

		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));

		return Protocol::Response(
			protocol.makeResponse(false, "Could not resolve host", "", NULL)
		);
	}

	// loop through all the results and connect to the first we can
	for(p = servinfo; p != NULL; p = p->ai_next) {
		if ((sockfd = socket(p->ai_family, p->ai_socktype,
				p->ai_protocol)) == -1) {
			perror("client: socket");
			continue;
		}

		if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
			close(sockfd);
			continue;
		}

		break;
	}

	if (p == NULL) {
		fprintf(stderr, "client: failed to connect\n");
		return Protocol::Response(
			protocol.makeResponse(false, "Could not connect", "", NULL)
		);
	}

	inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr),
			s, sizeof s);

	freeaddrinfo(servinfo); // all done with this structure

	Protocol::Response res = NetworkUtil::sendRequest(sockfd, req);

	close(sockfd);

	return res;
}

int Client::loadConfigFile(string filename) {
	
	string line;
	ifstream config(filename);
	
	if(config.is_open()) {
		
		int splitIdx;
		string field;

		while(getline(config, line)) {
			if((line.substr(0, 1) == "#") || (splitIdx = line.find(":")) < 0)
				continue;

			if((field = line.substr(0, splitIdx)) == "Host") {
				this->proxyHost = line.substr(splitIdx + 2);
			} else if (field == "Port") {
				this->proxyPort = line.substr(splitIdx + 2);
			}
		}

		config.close();

	} else {
		cout << "Error: could not open config file: " << filename << endl;
		return -1;
	}

	if(proxyHost == "") {
		cout << "Error: hostname not provided";
		return -1;
	}

	return 0;
}
