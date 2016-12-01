# Socket Proxy

This is a set of server and client software meant to facilitate the creation of virtual sockets between arbitrary network-enabled machines. It relies on the existence of a server that can be accessed by both machines and is running the socket proxy daemon. A machine that wishes to make one of its services available does so by registering with this central proxy server vi the socket proxy client, under a unique ID. After registering, another machine on a separate network may establish a connection to the service by providing this unique ID to the socket proxy client.

