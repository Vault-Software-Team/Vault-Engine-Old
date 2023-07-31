#ifndef STATIC_ENGINE_NETWORKING_H
#define STATIC_ENGINE_NETWORKING_H

#if defined(_WIN32)
#else
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <string.h>
#include <vector>
#include <sstream>

namespace HyperAPI {
    namespace Networking {
        class DLL_API Socket {
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

            virtual int Connect(int sock, struct sockaddr_in address) {
                return 0;
            }
            void Test(int) const;
            struct sockaddr_in GetAddress() const;
            int GetSocket() const;
            int GetConnection() const;

            void SetConnection(int);
        };

        class DLL_API BindSocket : public Socket {
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

        class DLL_API ConnectSocket : public Socket {
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

        class DLL_API ListenSocket : public BindSocket {
        private:
            int backlog;
            int listening;
        public:
            ListenSocket(
                    int domain,
                    int service,
                    int protocol,
                    int port,
                    u_long interface,
                    int backlog
            );

            void Listen();
        };

        class DLL_API Server {
        protected:
            ListenSocket *socket;
            virtual void Accepter(){};
            virtual void Handler(){};
            virtual void Responder(){};
        public:
            Server(
                int domain,
                int service,
                int protocol,
                int port,
                u_long interface,
                int backlog
            );

            virtual void Launch() {};
            ListenSocket *GetSocket() const;
        };

        class DLL_API TestServer : public Server {
        private:
            char buffer[30000] = {0};
            int newSocket;

            void Accepter() override;
            void Handler() override;
            void Responder() override;
        public:
            TestServer();

            void Launch() override;
        };
    }
}
#endif

#endif
