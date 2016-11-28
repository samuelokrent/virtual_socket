#ifndef NETWORK_UTIL_H
#define NETWORK_UTIL_H

#include <string>
#include "protocol.h"


using std::string;

class NetworkUtil {

	public:

		/**
		 * Opens a tcp connection to the given host and port
		 * 
		 * @return A socket file descriptor, or -1 on error
		 */
		static int createConnection(string hostname, string port);

		/**
		 * Send the given request to the given socket
		 * @return A protocol response
		 */	
		static Protocol::Response sendRequest(int sockfd, string req);

		/**
		 * Reads and returns a request packet from the given socket
		 */
		static Protocol::Request * readRequest(int sockfd);

		/**
		 * Sends the given request string to the server
		 * connected through serverFd, and forwards response
		 * to client file descriptor
		 *
		 * @return The response returned by the server
		 */
		static Protocol::Response forwardResponse(int clientFd, int serverFd, string proxyReq);

		/**
		 * Exchanges data between the given sockets
		 */
		static void facilitateConnection(int clientFd, int serverFd);

};

#endif
