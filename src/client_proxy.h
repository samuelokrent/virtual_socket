#ifndef CLIENT_PROXY_H
#define CLIENT_PROXY_H

#include <string>
#include "client.h"
#include "simple_server.h"

using std::string;

class ClientProxy : public Client, public SimpleServer {

    public:

        /**
         * Creates a new client proxy
         *
         * @param id The id under which the target user-server is registered
         */
        ClientProxy(string id);

        /**
         * Starts the client proxy
         */
        virtual void start();


    private:

        // The id under which the target user-server is registered
        string id;

		/**
		 * SimpleServer override
		 */
		virtual int handleRequest(int requestFd);
};


#endif
