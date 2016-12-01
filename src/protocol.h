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

		static const int PACKET_SIZE = 1024;

		// Protocol field values	
		static const string REGISTER; 
		static const string UNREGISTER;
		static const string CONNECT;
		static const string CONNECTION_CREATED;
		static const string CONNECTION_FAILED;
		static const string OK;
		static const string ERROR;

		/**
		 * @return a request string of the given type,
		 * containing the given node id
		 */
		string makeRequest(string type, string node_id);

		/**
		 * @return A request string for registering a user-server
		 * @param server_id The id to associate with this server
		 */	
		string makeRegisterRequest(string server_id);

		/**
		 * @return A request string for unregistering a user-server
		 * @param server_id The id associated with this server
		 */
		string makeUnregisterRequest(string server_id);

		/**
		 * @return A request string intended to be sent
		 * 	from user-client to proxy server, to make a connection
		 * @param server_id The id of server
		 */
		string makeConnectRequest(string server_id);

		/**
		 * @return A request string intended to be sent
		 *	from proxy server to user-server, to make a connection
		 * @param client_id The id of the client who sent the request
		 */
		string makeProxyConnectRequest(string client_id);

		/**
		 * @return A request string intended to be sent
		 * from user-server to proxy server, indicating that a
		 * new connection channel has been created for the given client
		 */
		string makeConnectionCreatedRequest(string client_id);

		/**
		 * @return A request string intended to be sent
		 * from user-server to proxy server, indicating that a
		 * a new connection was not created
		 */
		string makeConnectionFailedRequest(string client_id);

		/**
		 * @return A request string intended to be sent
		 *  from user-client to proxy server, to send data
		 * @param server_id The id of the recipient user-server
		 * @param data The request intended for the end user-server
		 */
		string makeDataRequest(string server_id, string data);

		/**
		 * @return A response string containing the given values
		 * @param success Whether the request was processed successfully
		 * @param errMsg An error message, if an error occured
		 * @param client_id The id of the client this reponse is intended for
		 */
		string makeResponse(bool success, string errMsg);


		/**
		 * Request: A class modeling a request using this protocol
		 */
		class Request {

			public:

				Request() {}
	
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
                 * @return Whether this request is to unregister a user-server
                 */
                bool isUnregisterRequest();

				/**
				 * @return Whether this request is to indicate a new connection
				 * has been created
				 */
				bool isConnectionCreatedRequest();

				/**
				 * @return Whether this request is to indicate a new connection
				 * has failed
				 */
				bool isConnectionFailedRequest();

				/**
				 * @return Whether this request is sending data
				 */
				bool isDataRequest();

				/**
				 * @return The server ID provided in this request
				 */
				string getServerID();

				/**
				 * @return The Client ID provided in this request
				 */
				string getClientID();

				/**
				 * @return The data in the body of this request
				 */
				string getData();

				string toString();

			private:

				// The json object holding the request data
				json j;

		};

		/**
		 * Response: A class modeling a response using this protocol
		 */
		class Response {

			public:

				Response() {}
	
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
				 * @return The id of the client for whom this response was intended
				 */
				string getClientID();
			
				/**
				 * @return The data in the body of this response
				 */
				string getData();

				string toString();
			
			private:

				// The JSON object olding this response's data
				json j;
		};
};

#endif
