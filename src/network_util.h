#ifndef NETWORK_UTIL_H
#define NETWORK_UTIL_H

#include <string>
#include "protocol.h"


using std::string;

class NetworkUtil {

	public:

		/**
		 * Send the given request to the given socket
		 * @return A protocol response
		 */	
		static Protocol::Response sendRequest(int sockfd, string req);

		/**
		 * Sends the given request string to the server
		 * connected through serverFd, and forwards response
		 * to client file descriptor
		 *
		 * @return The response returned by the server
		 */
		static Protocol::Response forwardResponse(int clientFd, int serverFd, string proxyReq);
};

#endif
