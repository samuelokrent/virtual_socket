#ifndef SERVER_H
#define SERVER_H

#include <string>
#include "protocol.h"
#include <map>
#include <utility>

using std::map;
using std::string;
using std::pair;

/**
 * Server: The server side of a networking tool
 * that creates virtual sockets between machines
 */
class Server {

	public:

		/**
		 * Starts the server listening on the given port
		 */
		void run(string port);
	
	private:

		// A mapping between unique ID's and registered server hostname/ports
		map<string, pair<string, in_port_t>> serverMap;

		// The protocol defining this server's communications
		Protocol protocol;

		/**
		 * @return The hostname of the given address
		 */
		void *get_in_addr(struct sockaddr *sa);

		/**
		 * @return The port number of the given address
		 */
		in_port_t get_in_port(struct sockaddr *sa);

		/**
		 * Associates the given host/port with the given id
		 */
		int registerServer(string id, string host, in_port_t port);

		/**
		 * Processes and responds to a request, and closes the socket
		 * @param requestFd The file descriptor of the socket 
		 *    issuing the request
		 * @param host The hostname of the client making the request
		 * @param port The source port number of the request
		 */
		int handleRequest(int requestFd, string host, in_port_t port);

		/**
		 * Enters the main connection acceptance loop
		 * @param sockfd The file descriptor of the socket
		 *    on which the server is listening
		 */
		void acceptConnections(int sockfd);

};

#endif
