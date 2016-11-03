#ifndef CLIENT_H
#define CLIENT_H

#include "protocol.h"
#include <string>

using std::string;

class Client {

	public:
	
		Protocol::Response sendRequest(string req);

	private:
		
		Protocol protocol;

};

#endif
