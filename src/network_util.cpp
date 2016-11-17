#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <string>
#include "protocol.h"
#include "network_util.h"

using std::string;

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

int NetworkUtil::createConnection(string hostname, string port) {
	int sockfd, numbytes;
	char buf[MAXDATASIZE + 1];
	struct addrinfo hints, *servinfo, *p;
	int rv;

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	if ((rv = getaddrinfo(hostname.c_str(), port.c_str(),
			&hints, &servinfo)) != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		return -1;
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
		return -1;
	}

	freeaddrinfo(servinfo); // all done with this structure

	return sockfd;
}

Protocol::Response NetworkUtil::sendRequest(int sockfd, string req) {

	Protocol protocol;
	int res_len;
	char buf[Protocol::PACKET_SIZE + 1];

	send(sockfd, req.c_str(), req.length(), 0);

	if ((res_len = recv(sockfd, buf, MAXDATASIZE, 0)) == -1) {
		perror("recv");
		return Protocol::Response(protocol.makeResponse(
			false, "Could not receive response", "", NULL
		));
	}

	buf[res_len] = '\0';

	return Protocol::Response(buf);
}

Protocol::Request * NetworkUtil::readRequest(int sockfd) {

	Protocol protocol;
	int res_len;
	char buf[Protocol::PACKET_SIZE + 1];

	if ((res_len = recv(sockfd, buf, Protocol::PACKET_SIZE, MSG_WAITALL)) == -1) {
		perror("recv");
		return NULL;
	}

	buf[res_len] = '\0';

	return new Protocol::Request(buf);	
}

Protocol::Response NetworkUtil::forwardResponse(int clientFd, 
									int serverFd, string proxyReq) {

	Protocol::Response serverRes = sendRequest(serverFd, proxyReq);
	string proxyRes = serverRes.toString();
	send(clientFd, proxyRes.c_str(), proxyRes.length(), 0);
	return serverRes;
}
