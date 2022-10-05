#include "lib/networking.h"
#include <iostream>

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
    }
}