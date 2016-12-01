# Socket Proxy

This is a set of server and client software meant to facilitate the creation of virtual sockets between arbitrary network-enabled machines. It relies on the existence of a server that can be accessed by both machines and is running the socket proxy daemon. A machine that wishes to make one of its services available does so by registering with this central proxy server via the socket proxy client, under a unique ID. After registering, another machine on a separate network may establish a connection to the service by providing this unique ID to the socket proxy client.

## Installing

In order to use this software, first clone this repository

`git clone https://github.com/samuelokrent/virtual_socket.git`

Then run `make` inside the new directory to build the executables. Two binaries are created; `spd` runs the central proxy server, `sp` runs the client.

## Settings

The file `.spconfig` in the root directory of the project contains configuration settings for the socket proxy client.

- ProxyHost: The host name of the central proxy server being used
- ProxyPort: The port number on which the central proxy server is listening (default port is 6070)

## Usage

### spd

To start the central proxy server, simply run `./spd` within the root directory of the project.

### sp

Assuming a central proxy server has been properly configured, in order to register a service on your local machine, run

`./sp register <ID> <PORT>`

where ID is the unique ID to associate with your service, and PORT is the port number on which your server runs. For example, `./sp register my_web_app 80`.

To connect with a registered service, first run

`./sp connect <ID>`

where ID is the ID of the service. Output should indicate whether or not a connection is successfully established. If it is, then the socket proxy client is now listening on localhost:6071. Connections to this port are forwarded to the registered service. So, if you want to make an http request to a registered web server under the id 'my_web_app', run

```
./sp connect my_web_app
curl http://localhost:6071
```

Or, to ssh as a user 'user1' to an ssh daemon registered under 'my_sshd', run

```
./sp connect my_sshd
ssh user1@localhost -p 6071
```
