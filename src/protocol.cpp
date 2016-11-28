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
const string Protocol::CONNECTION_CREATED = "connection_created";
const string Protocol::OK = "ok";
const string Protocol::ERROR = "error";

const string TYPE = "type";
const string STATUS = "status";
const string NODE_ID = "node_id";
const string ERROR_MESSAGE = "error_message";

string Protocol::makeRequest(string type, string node_id) {
	json req;
	req[TYPE] = type;
	req[NODE_ID] = node_id;
	return req.dump() + "\n";
}

string Protocol::makeRegisterRequest(string id) {
	return makeRequest(REGISTER, id);
}

string Protocol::makeConnectRequest(string id) {
	return makeRequest(CONNECT, id);
}

string Protocol::makeProxyConnectRequest(string id) {
	return makeRequest(CONNECT, id);
}

string Protocol::makeConnectionCreatedRequest(string client_id) {
	return makeRequest(CONNECTION_CREATED, client_id);
}

string Protocol::makeResponse(bool success, string errMsg) {
	json res;
	res[STATUS] = (success ? OK : ERROR);
	if(!success) {
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

bool Protocol::Request::isConnectionCreatedRequest() {
	return (j[TYPE] == CONNECTION_CREATED);
}

string Protocol::Request::getServerID() {
	return j[NODE_ID];
}

string Protocol::Request::getClientID() {
	return j[NODE_ID];
}

string Protocol::Request::toString() {
	return j.dump() + "\n";
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

string Protocol::Response::getErrorMessage() {
	return j[ERROR_MESSAGE];
}

string Protocol::Response::toString() {
	return j.dump() + "\n";
}
