#ifndef IMPLEMENT_APP_H
#define IMPLEMENT_APP_H

#include "client.h"
#include "server.h"

ServerOBJ* s;
ClientOBJ *c1, *c2, *c3, *c4;

void
slog(string message)
{
    message = "s " + message;
    cout << fixed << setprecision(4) << Simulator::Now().GetSeconds() << "s " << message << endl;
}

void
startServer(NodeContainer server)
{
    s = new ServerOBJ(server.Get(0));
    s->startTrace();
}

void
startClient(NodeContainer clients)
{
    if (g_n_client == 1)
    {
        c1 = new ClientOBJ(clients.Get(0));
    }
    else if (g_n_client == 2)
    {
        c1 = new ClientOBJ(clients.Get(0));
        c2 = new ClientOBJ(clients.Get(1));
    }
    else if (g_n_client == 3)
    {
        c1 = new ClientOBJ(clients.Get(0));
        c2 = new ClientOBJ(clients.Get(1));
        c3 = new ClientOBJ(clients.Get(2));
    }
    else if (g_n_client == 4)
    {
        c1 = new ClientOBJ(clients.Get(0));
        c2 = new ClientOBJ(clients.Get(1));
        c3 = new ClientOBJ(clients.Get(2));
        c4 = new ClientOBJ(clients.Get(3));
    }
}

void
ImplementApp(vector<NodeContainer> nodes)
{
    NodeContainer server_nodes = nodes[0];
    NodeContainer client_nodes = nodes[1];

    Simulator::Schedule(MilliSeconds(0), &startServer, server_nodes);
    Simulator::Schedule(MilliSeconds(0), &startClient, client_nodes);

    int n_client = g_n_client;
    for (int i = 1; i <= n_client; ++i)
    {
        Simulator::Schedule(MilliSeconds(10), [i]() {
            if (i == 1)
            {
                c1->connect();
            }
            else if (i == 2)
            {
                c2->connect();
            }
            else if (i == 3)
            {
                c3->connect();
            }
            else if (i == 4)
            {
                c4->connect();
            }
        });
    }

    Simulator::Schedule(MilliSeconds(100), []() { s->sendMatrix(); });
}

#endif // IMPLEMENT_APP_H