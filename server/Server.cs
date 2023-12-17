using System;
using System.Collections.Generic;
using System.Net;
using System.Net.Sockets;

namespace GameNetwork
{
    class Server
    {
        public static int max_players { get; private set; }
        public static int port { get; private set; }

        public static Dictionary<int, Client> clients = new Dictionary<int, Client>();

        private static TcpListener tcp_listener;

        public static void start(int max_p, int _port)
        {
            max_players = max_p;
            port = _port;

            Console.WriteLine("Starting the server");
            init_server_data();

            tcp_listener = new TcpListener(IPAddress.Any, port);
            tcp_listener.Start();
            tcp_listener.BeginAcceptTcpClient(new AsyncCallback(connect_callback), null);
            Console.WriteLine("Started on port " + port);
        }

        private static void connect_callback(IAsyncResult result)
        {
            TcpClient client = tcp_listener.EndAcceptTcpClient(result);
            tcp_listener.BeginAcceptTcpClient(new AsyncCallback(connect_callback), null);
            Console.Write($"Incoming connection from: {client.Client.RemoteEndPoint}");

            for (int i = 1; i <= max_players; i++)
            {
                if (clients[i].tcp.socket == null)
                {
                    clients[i].tcp.Connect(client);
                    return;
                }
            }

            Console.Write($"{client.Client.RemoteEndPoint} Failed to connect: Server is full");
        }

        private static void init_server_data()
        {
            for (int i = 1; i <= max_players; i++)
            {
                clients.Add(i, new Client(i));
            }
        }
    }
}