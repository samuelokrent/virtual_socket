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
const string Protocol::DATA = "data";
const string Protocol::OK = "ok";
const string Protocol::ERROR = "error";

const string TYPE = "type";
const string HOST = "host";
const string STATUS = "status";
const string SERVER_ID = "server_id";
const string CLIENT_ID = "client_id";
const string ERROR_MESSAGE = "error_message";
const string DATA = "data"; 

string Protocol::makeRegisterRequest(string id) {
	json req;
	req[TYPE] = REGISTER;
	req[SERVER_ID] = id;
	return req.dump() + "\n";
}

string Protocol::makeConnectRequest(string id) {
	json req;
	req[TYPE] = CONNECT;
	req[SERVER_ID] = id;
	return req.dump() + "\n";
}

string Protocol::makeProxyConnectRequest(string id) {
	json req;
	req[TYPE] = CONNECT;
	req[CLIENT_ID] = id;
	return req.dump() + "\n";
}

string Protocol::makeDataRequest(string server_id, string data) {
	json req;
	req[TYPE] = DATA;
	req[SERVER_ID] = server_id;
	req[DATA] = string(data);
	return req.dump();
}

string Protocol::makeProxyDataRequest(string client_id, string data) {
	json req;
	req[TYPE] = DATA;
	req[CLIENT_ID] = client_id;
	req[DATA] = string(data);
	return req.dump();
}

string Protocol::makeResponse(bool success, string errMsg, 
								string client_id, string data) {
	json res;
	res[STATUS] = (success ? OK : ERROR);
	res[CLIENT_ID] = client_id;
	if(success && (data != "")) {
		res[DATA] = string(data);
	} else if(!success) {
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

bool Protocol::Request::isDataRequest() {
	return (j[TYPE] == DATA);
}

string Protocol::Request::getServerID() {
	return j[SERVER_ID];
}

string Protocol::Request::getClientID() {
	return j[CLIENT_ID];
}

string Protocol::Request::getData() {
	return j[DATA];
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

string Protocol::Response::getClientID() {
	return j[CLIENT_ID];
}

string Protocol::Response::getData() {
	return j[DATA];
}

string Protocol::Response::getErrorMessage() {
	return j[ERROR_MESSAGE];
}

string Protocol::Response::toString() {
	return j.dump() + "\n";
}
