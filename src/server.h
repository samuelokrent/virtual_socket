#ifndef SERVER_H
#define SERVER_H

#include "protocol.h"
#include "simple_server.h"
#include <map>
#include <set>
#include <string>

using std::map;
using std::set;
using std::string;

/**
 * Server: The server side of a networking tool
 * that creates virtual sockets between machines
 */
class Server : public SimpleServer {

	private:

		// A mapping between unique ID's and registered server file descriptors
		map<string, int> serverMap;

		// A set of file descriptors of clients who are attempting connections
		set<string> pendingConnections;

		// The protocol defining this server's communications
		Protocol protocol;

		/**
		 * Associates the given file descriptor with the given id
		 */
		int registerServer(string id, int server_fd);

		/**
		 * Processes and responds to a request, and closes the socket
		 * @param requestFd The file descriptor of the socket 
		 *    issuing the request
		 * @param host The hostname of the client making the request
		 * @param port The source port number of the request
		 */
		virtual int handleRequest(int requestFd);

		/**
		 * Creates and facilitates a virtual connection between
		 * user-client and user-server
		 * @param clientFd The file descriptor of the client connection
		 * @param serverFd the file descriptor of the server connection
		 */
		void facilitateConnection(int clientFd, int serverFd);

};

#endif
