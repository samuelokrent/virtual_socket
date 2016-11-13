#ifndef SIMPLE_SERVER_H
#define SIMPLE_SERVER_H

#include <string>

using std::string;

class SimpleServer {

	public:

		/**
         * Starts the server listening on the given port
         */
        void run(string port);

	protected:

		// The file descriptor of the socket on which
		// 	this server listens
		int sock_fd;

		/**
         * Creates a simple server instance listening on
         *  the given port
         * @return the file desciptor of the server socket,
         *  or -1 on error
         */
        int startListening(string port);

		/**
		 * Enters the main connection acceptance loop
		 * @param sockfd The file descriptor of the socket
		 *	on which the server is listening
		 */
		void acceptConnections(int sockfd);

		/**
		 * Processes and responds to a request, and closes the socket
		 * @param requestFd The file descriptor of the socket
		 *	issuing the request
		 * @param host The hostname of the client making the request
		 * @param port The source port number of the request
		 */
		virtual int handleRequest(int requestFd) = 0;
	
		/**
		 * @return The hostname of the given address
		 */
		void *get_in_addr(struct sockaddr *sa);

		/**
		 * @return The port number of the given address
		 */
		in_port_t get_in_port(struct sockaddr *sa);

};

#endif
