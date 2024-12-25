#include "ns3/applications-module.h"
#include "ns3/bridge-module.h"
#include "ns3/core-module.h"
#include "ns3/csma-module.h"
#include "ns3/internet-module.h"
#include "ns3/log.h"
#include "ns3/netanim-module.h"
#include "ns3/network-module.h"
#include "ns3/node-container.h"
#include "ns3/point-to-point-module.h"
#include "ns3/socket.h"

#include <map>
#include <nlohmann/json.hpp>
#include <string>

NS_LOG_COMPONENT_DEFINE("ProjectLog");
using namespace ns3;
using namespace std;
using json = nlohmann::json;

#include "addon/client.h"
#include "addon/create_node.h"
#include "addon/global_values.h"
#include "addon/implement_app.h"
#include "addon/json_manager.h"
#include "addon/node_obj.h"
#include "addon/server.h"
#include "uncleane.h"

// ___________________________ //
// ========== ERROR ========== //
// client to client check hop  //
// ‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾ //

// _____________________________ //
// ========== Missing ========== //
// TCP                           //
// through put                   //
// Traffic distance              //
// energy                        //
// disconnect                    //
// ‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾ //

void
setupLogging()
{
    Time::SetResolution(Time::NS);

    LogComponentEnable("ProjectLog", LOG_LEVEL_INFO);
    LogComponentEnable("UdpEchoClientApplication", LOG_LEVEL_INFO);
    LogComponentEnable("UdpEchoServerApplication", LOG_LEVEL_INFO);
    LogComponentEnable("PacketSink", LOG_LEVEL_INFO);
}

vector<NodeContainer>
configSimulator()
{
    NodeContainer server_nodes, switch_nodes, router_nodes, client_nodes;
    CreateNode(server_nodes, switch_nodes, router_nodes, client_nodes);

    UCConfigSimulation(server_nodes, switch_nodes, router_nodes, client_nodes);

    Ipv4GlobalRoutingHelper::PopulateRoutingTables();

    return {server_nodes, client_nodes, switch_nodes, router_nodes};
}

void
displayResult()
{
    string parameter = g_send_type + ", " + g_subnet + ", " + g_protocol + ", " + g_distance +
                       ", " + to_string(g_n_client);
    cout << "Simulation : \n" << parameter << endl;

    vector<double> vec;

    vec = d_computational_time;
    if (debug) // show data
    {
        cout << "Computational Time : \n";
        copy(vec.begin(), vec.end(), ostream_iterator<double>(cout, " "));
        cout << endl;
    }
    r_computational_time = accumulate(vec.begin(), vec.end(), 0.0) / vec.size();

    vec = d_result_time;
    if (debug) // show data
    {
        cout << "Computational Time : \n";
        copy(vec.begin(), vec.end(), ostream_iterator<double>(cout, " "));
        cout << endl;
    }
    r_result_time = accumulate(vec.begin(), vec.end(), 0.0) / vec.size();

    cout << "Average Computational Time : " << r_computational_time << endl;
    cout << "Average Result Time : " << r_result_time << endl;
}

void
resetResult()
{
    d_computational_time.clear();
    d_result_time.clear();
}

void
runSimulation()
{
    Names::Clear();

    vector<NodeContainer> nodes = configSimulator();
    ImplementApp(nodes);

    Simulator::Run();
    Simulator::Destroy();

    if (debug)
    {
        displayResult();
    }
}

void
fullSimulation(int round = 1)
{
    vector<string> send_type_values = {"basic", "balance"};
    vector<string> subnet_values = {"same", "diff"};
    vector<string> protocol_values = {"UDP", "TCP"};
    vector<string> distance_values = {"hop", "traffic"};
    vector<int> n_client_values = {1, 2, 3, 4};

    for (auto send_type : send_type_values)
    {
        for (auto subnet : subnet_values)
        {
            for (auto protocol : protocol_values)
            {
                if (protocol == "TCP")
                {
                    continue;
                }
                for (auto distance : distance_values)
                {
                    if (distance == "traffic")
                {
                    continue;
                }
                    for (auto n_client : n_client_values)
                    {
                        for (int i = 0; i < round; i++)
                        {
                            g_send_type = send_type;
                            g_subnet = subnet;
                            g_protocol = protocol;
                            g_distance = distance;
                            g_n_client = n_client;

                            runSimulation();
                        }
                        displayResult();
                        resetResult();
                    }
                }
            }
        }
    }
}

int
main(int argc, char* argv[])
{
    CommandLine cmd(__FILE__);
    cmd.Parse(argc, argv);

    setupLogging();

    debug = true;

    if (debug)
    {
        g_send_type = "balance";
        g_subnet = true;
        g_protocol = "UDP";
        g_n_client = 4;

        runSimulation();
    }
    else
    {
        int round = 10;
        fullSimulation(round);
    }
    return 0;
}

void
animInt(string path, vector<NodeContainer> nodes)
{
    AnimationInterface anim(path + "output.xml");
    NodeContainer server_nodes = nodes[0];
    NodeContainer client_nodes = nodes[1];

    anim.UpdateNodeDescription(server_nodes.Get(0), "Server");
    anim.UpdateNodeColor(server_nodes.Get(0), 255, 0, 0);      // Red
    anim.SetConstantPosition(server_nodes.Get(0), 10.0, 10.0); // Position for server node

    for (uint32_t i = 0; i < client_nodes.GetN(); ++i)
    {
        string name = "Client " + to_string(i + 1);
        anim.UpdateNodeDescription(client_nodes.Get(i), name);
        anim.UpdateNodeColor(client_nodes.Get(i), 0, 255, 0); // Green
        anim.SetConstantPosition(client_nodes.Get(i), 20.0 + i * 5.0, 20.0);
    }
}

// animInt("/home/ntk/workspace/ns-allinone-3.42/ns-3.42/scratch/project/", nodes);