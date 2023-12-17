using System;
using GameNetwork;

class Program
{
    static void Main(string[] args)
    {
        Console.Title = "Server";
        Server.start(50, 26950);
        Console.ReadKey();
    }
}
