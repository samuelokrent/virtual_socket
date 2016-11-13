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

	private:

		// Hostname of proxy server
		string proxyHost = "";

		// Port number of proxy server (default is 6070)
		string proxyPort = "6070";
		
		Protocol protocol;

};

#endif
