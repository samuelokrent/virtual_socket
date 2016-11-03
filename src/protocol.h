#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <string>
#include "json.hpp"
using std::string;
using nlohmann::json;

/**
 * Protocol: A class defining the communication protocol
 * between server and client
 *
 * Requests and responses are implemented as json strings
 */
class Protocol {

	public:

		// Protocol field values	
		static const string REGISTER; 
		static const string CONNECT;
		static const string OK;
		static const string ERROR;

		/**
		 * @return A request string for registering a server
		 * @param id The id to associate with this server
		 * @param port The port on which the server listens
		 */	
		string makeRegisterRequest(string id, int port);

        /**
         * @return A request string for connecting to a server
         * @param id The id of server
         */
		string makeConnectRequest(string id);

        /**
		 * @return A response string containing the given values
         */
		string makeResponse(bool success, string errMsg, string host, int port);

		/**
		 * Request: A class modeling a request using this protocol
		 */
		class Request {

			public:
	
				/**
				 * Creates a new request object from the given request json string
				 */
				Request(string reqJson);

				/**
				 * @return Whether this request is for a connection
				 */
				bool isConnectRequest();

				/**
				 * @return Whether this request is to register a new server
				 */
				bool isRegisterRequest();

				/**
				 * @return the port number provided in this request
				 */
				string getPort();

				/**
				 * @return The server ID provided in this request
				 */
				string getID();

			private:

				// The json object holding the request data
				json j;

		};

        /**
         * Response: A class modeling a response using this protocol
         */
		class Response {

			public:
	
				/**
                 * Creates a new response object from the given response json string
                 */	
				Response(string resJson);

				/**
				 * @return Whether the response's status is ok
				 */
				bool isOK();

				/**
				 * @return Whether the response indicates an error occured
				 */
				bool isError();

				/**
				 * @return The response's error message, if it exists
				 */
				string getErrorMessage();
	
				/**
				 * @return The hostname returned in this response
				 */
				string getHost();
			
				/**
				 * @return The port number returned in this response
				 */
				string getPort();
			
			private:

				// The JSON object olding this response's data
				json j;
		};
};

#endif
