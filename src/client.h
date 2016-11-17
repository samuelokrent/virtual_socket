#ifndef CLIENT_H
#define CLIENT_H

#include "protocol.h"
#include <string>

using std::string;

class Client {

	public:

		/**
		 * Loads configuration settings from the given file
		 */
		int loadConfigFile(string filename);

		/**
		 * Sends the given request string to the proxy server
		 */	
		Protocol::Response sendRequest(string req);

		/**
		 * Starts the client
		 */
		virtual void start() = 0;
		
	protected:

		// Hostname of proxy server
		string proxyHost = "";

		// Port number of proxy server (default is 6070)
		string proxyPort = "6070";

		// Port number of local server	
		string serverPort;

		// File descriptor of connection to proxy server
		int connection = -1;

		Protocol protocol;

		// Opens a connection to the proxy server
		int connect();

};

#endif
