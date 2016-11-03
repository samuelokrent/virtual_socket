#include "protocol.h"
#include <iostream>
using std::cout;

int main() {
	Protocol p;

	Protocol::Request req = Protocol::Request(p.makeConnectRequest("hello"));
	if(req.isConnectRequest()) cout << "is";
	else cout << "not";
	return 0;
}
