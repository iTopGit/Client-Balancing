#ifndef CREATE_NODE_H
#define CREATE_NODE_H

#include "ns3/node-container.h"
#include "global_values.h"

void
AddNodeName(const string& base_name, NodeContainer& nodes)
{
    for (uint32_t i = 0; i < nodes.GetN(); ++i)
    {
        string name = base_name + " " + to_string(i + 1);
        Names::Add(name, nodes.Get(i));
    }
}

void
CreateNode(NodeContainer& server_nodes,
           NodeContainer& switch_nodes,
           NodeContainer& router_nodes,
           NodeContainer& client_nodes)
{
    server_nodes.Create(1);

    int n_switch = 2;
    if (g_subnet != "same")
    {
        n_switch = 5;
    }

    switch_nodes.Create(n_switch);
    router_nodes.Create(1);

    client_nodes.Create(4);

    if (server_nodes.GetN() == 0 || client_nodes.GetN() == 0 || 
        switch_nodes.GetN() == 0 || router_nodes.GetN() == 0) {
        std::cerr << "Error: Nodes failed to initialize in CreateNode()!" << std::endl;
    }

    Names::Add("Server", server_nodes.Get(0));
    AddNodeName("Switch", switch_nodes);
    Names::Add("Router", router_nodes.Get(0));
    AddNodeName("Client", client_nodes);
}

#endif // CREATE_NODE_H
