#ifndef STATIC_ENGINE_NETWORKING_H
#define STATIC_ENGINE_NETWORKING_H

#if defined(_WIN32)
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#include <iphlpapi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>
#include <sstream>
#else
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <string>
#include <vector>
#include <sstream>
#endif

namespace HyperAPI {
    namespace Networking {
        class Socket {
        private:
            struct sockaddr_in address;
            int sock;
            int connection;

        public:
            Socket(
                    int domain,
                    int service,
                    int protocol,
                    int port,
                    u_long interface
            );

            virtual int Connect(int sock, struct sockaddr_in address) = 0;
            void Test(int) const;
            struct sockaddr_in GetAddress() const;
            int GetSocket() const;
            int GetConnection() const;

            void SetConnection(int);
        };

        class BindSocket : public Socket {
        public:
            BindSocket(
                    int domain,
                    int service,
                    int protocol,
                    int port,
                    u_long interface
            );

            int Connect(int sock, struct sockaddr_in address) override;
        };

        class ConnectSocket : public Socket {
        public:
            ConnectSocket(
                    int domain,
                    int service,
                    int protocol,
                    int port,
                    u_long interface
            );

            int Connect(int sock, struct sockaddr_in address) override;
        };
    }
}

#endif
