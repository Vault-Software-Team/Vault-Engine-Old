#include "lib/networking.h"
#include <iostream>

#ifndef _WIN32
namespace HyperAPI {
    namespace Networking {
        Socket::Socket(int domain, int service, int protocol, int port, u_long interface) {
            address.sin_family = domain;
            address.sin_port = htons(port);
            address.sin_addr.s_addr = htonl(interface);

            sock = socket(domain, service, protocol);
            connection = Connect(sock, address);
            Test(connection);
        }

        void Socket::Test(int a) const {
            if(a < 0) {
                perror("Connection failed");
                exit(EXIT_FAILURE);
            }
        }

        struct sockaddr_in Socket::GetAddress() const {
            return address;
        }

        int Socket::GetSocket() const {
            return sock;
        }

        int Socket::GetConnection() const {
            return connection;
        }

        void Socket::SetConnection(int a) {
            connection = a;
        }

        BindSocket::BindSocket(
                int domain,
                int service,
                int protocol,
                int port,
                u_long interface
        ) : Socket(domain, service, protocol, port, interface) {
            SetConnection(Connect(GetSocket(), GetAddress()));
            Test(GetConnection());
        }

        int BindSocket::Connect(int sock, struct sockaddr_in address) {
            return bind(sock, (struct sockaddr *) &address, sizeof(address));
        }

        ConnectSocket::ConnectSocket(
                int domain,
                int service,
                int protocol,
                int port,
                u_long interface
        ) : Socket(domain, service, protocol, port, interface) {
            SetConnection(Connect(GetSocket(), GetAddress()));
            Test(GetConnection());
        }

        int ConnectSocket::Connect(int sock, struct sockaddr_in address) {
            return connect(sock, (struct sockaddr *) &address, sizeof(address));
        }

        ListenSocket::ListenSocket(
                int domain,
                int service,
                int protocol,
                int port,
                u_long interface,
                int backlog
        ) : BindSocket(domain, service, protocol, port, interface), backlog(backlog) {
            Listen();
            Test(listening);
        }

        void ListenSocket::Listen() {
            listening = listen(GetSocket(), backlog);
        }

        Server::Server(int domain, int service, int protocol, int port, u_long interface, int backlog) {
            socket = new ListenSocket(domain, service, protocol, port, interface, backlog);
        }

        ListenSocket *Server::GetSocket() const {
            return socket;
        }

        TestServer::TestServer() : Server(AF_INET, SOCK_STREAM, 0, 8080, INADDR_ANY, 10) {
            Launch();
        }

        void TestServer::Accepter() {
            auto addr = socket->GetAddress();
            auto addrlen = sizeof(addr);

            newSocket = accept(socket->GetSocket(), (struct sockaddr *) &addr, (socklen_t *) &addrlen);
            read(newSocket, buffer, 30000);


        }

        void TestServer::Handler() {
            std::cout << buffer << std::endl;
        }

        void TestServer::Responder() {
            // send html h1 tag that says hi back to the client please
            const char *hello = "HTTP/1.1 200 OK\nContent-Type:text/html\nContent-Length: 16\n\n<h1>testing</h1>";
            send(newSocket, hello, strlen(hello), 0);
            close(newSocket);
        }

        void TestServer::Launch() {
            while(true) {
                std::cout << "Waiting for connection..." << std::endl;
                Accepter();
                Handler();
                Responder();
                std::cout << "Connection closed" << std::endl;
            }
        }
    }
}
#endif