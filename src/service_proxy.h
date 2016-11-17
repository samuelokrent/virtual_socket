#ifndef SERVICE_PROXY_H
#define SERVICE_PROXY_H

#include <string>
#include "client.h"

using std::string;

class ServiceProxy : public Client {

	public:

		/**
		 * Creates a new service proxy 
		 *
		 * @param id The id under which to register the server
		 * @param port The port number of the service being proxied
		 */
		ServiceProxy(string id, string port);

		/**
		 * Starts the service proxy
		 */
		virtual void start();


	private:

		// The id under which this server is registered
		string id;

		// The port number of the service being proxied
		string port;

		/**
		 * Facilitates communication between client and server
		 */
		void facilitateConnection(int clientFd, int serverFd);

		/**
		 * Attempts to create a new virtual connection
		 * for the client who sent the request
		 */
		void handleConnectRequest(Protocol::Request * req);
};

#endif
