#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include "protocol.h"
#include "client.h"
#include "network_util.h"
#include <string>
#include <iostream>
#include <fstream>

using std::string;
using std::cout;
using std::endl;
using std::ifstream;

#define MAXDATASIZE 1024

int Client::connect() {
	this->connection = NetworkUtil::createConnection(proxyHost, proxyPort);
	return connection;
}

Protocol::Response Client::sendRequest(string req) {

	if(connection < 0) {
		return Protocol::Response(protocol.makeResponse(
			false, "Could not connect"));
	}

	return NetworkUtil::sendRequest(connection, req);
}

int Client::loadConfigFile(string filename) {
	
	string line;
	ifstream config(filename);
	
	if(config.is_open()) {
		
		int splitIdx;
		string field;

		while(getline(config, line)) {
			if((line.substr(0, 1) == "#") || (splitIdx = line.find(":")) < 0)
				continue;

			if((field = line.substr(0, splitIdx)) == "ProxyHost") {
				this->proxyHost = line.substr(splitIdx + 2);
			} else if (field == "ProxyPort") {
				this->proxyPort = line.substr(splitIdx + 2);
			}
		}

		config.close();

	} else {
		cout << "Error: could not open config file: " << filename << endl;
		return -1;
	}

	if(proxyHost == "") {
		cout << "Error: hostname not provided";
		return -1;
	}

	return 0;
}
