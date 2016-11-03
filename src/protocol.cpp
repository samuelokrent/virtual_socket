#include "protocol.h"
#include <string>
#include <iostream>
#include "json.hpp"
using std::string;
using std::cout;
using nlohmann::json;

/**
 * Protocol: A class defining the communication protocol
 * between server and client
 */

const string Protocol::REGISTER = "register";
const string Protocol::CONNECT = "connect";
const string Protocol::OK = "ok";
const string Protocol::ERROR = "error";

const string TYPE = "type";
const string HOST = "host";
const string STATUS = "status";
const string PORT = "port";
const string ID = "id";
const string ERROR_MESSAGE = "error_message";


string Protocol::makeRegisterRequest(string id, int port) {
	json req;
	req[TYPE] = REGISTER;
	req[PORT] = port;
	req[ID] = id;
	return req.dump() + "\n";
}

string Protocol::makeConnectRequest(string id) {
    json req;
    req[TYPE] = CONNECT;
    req[ID] = id;
    return req.dump() + "\n";
}

string Protocol::makeResponse(bool success, string errMsg, string host, int port) {
    json res;
    res[STATUS] = (success ? OK : ERROR);
	if(success) {
		res[PORT] = std::to_string(port);
    	res[HOST] = host;
	} else {
		res[ERROR_MESSAGE] = errMsg;
	}
    return res.dump() + "\n";
}

/**
 * Request
 */

Protocol::Request::Request(string reqJson) {
	this->j = json::parse(reqJson);
}

bool Protocol::Request::isConnectRequest() {
	return (j[TYPE] == CONNECT);
}

bool Protocol::Request::isRegisterRequest() {
	return (j[TYPE] == REGISTER);
}

string Protocol::Request::getPort() {
	return j[PORT];
}

string Protocol::Request::getID() {
    return j[ID];
}

/**
 * Response
 */

Protocol::Response::Response(string resJson) {
    this->j = json::parse(resJson);
}

bool Protocol::Response::isOK() {
    return (j[STATUS] == OK);
}

bool Protocol::Response::isError() {
    return (j[STATUS] == ERROR);
}

string Protocol::Response::getHost() {
	return j[HOST];
}

string Protocol::Response::getPort() {
    return j[PORT];
}

string Protocol::Response::getErrorMessage() {
    return j[ERROR_MESSAGE];
}
