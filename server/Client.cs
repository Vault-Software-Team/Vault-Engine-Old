using System;
using System.Collections.Generic;
using System.Net;
using System.Net.Sockets;

namespace GameNetwork
{
    class Client
    {
        public static int data_buffer_size = 4096;
        public int id;
        public TCP tcp;

        public Client(int client_id)
        {
            id = client_id;
            tcp = new TCP(id);
        }

        public class TCP
        {
            public TcpClient socket;
            public NetworkStream stream;
            private byte[] receive_buffer;
            private readonly int id;

            public TCP(int ID)
            {
                id = ID;
            }

            public void Connect(TcpClient socket)
            {
                this.socket = socket;
                socket.ReceiveBufferSize = data_buffer_size;
                socket.SendBufferSize = data_buffer_size;

                stream = socket.GetStream();
                receive_buffer = new byte[data_buffer_size];
                stream.BeginRead(receive_buffer, 0, data_buffer_size, receive_callback, null);
            }

            private void receive_callback(IAsyncResult result)
            {
                try
                {
                    int byte_length = stream.EndRead(result);

                    if (byte_length <= 0)
                    {
                        return;
                    }

                    byte[] data = new byte[byte_length];
                    Array.Copy(receive_buffer, data, byte_length);
                }
                catch (Exception ex)
                {
                    Console.WriteLine(ex);
                }
            }
        }


    }
}