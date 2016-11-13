#include <sys/socket.h>
#include <string>
#include "protocol.h"
#include "network_util.h"

using std::string;

#define MAXDATASIZE 1024

Protocol::Response NetworkUtil::sendRequest(int sockfd, string req) {

	Protocol protocol;
	int res_len;
	char buf[MAXDATASIZE + 1];

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

Protocol::Response NetworkUtil::forwardResponse(int clientFd, 
									int serverFd, string proxyReq) {

	Protocol::Response serverRes = sendRequest(serverFd, proxyReq);
	string proxyRes = serverRes.toString();
	send(clientFd, proxyRes.c_str(), proxyRes.length(), 0);
	return serverRes;
}
