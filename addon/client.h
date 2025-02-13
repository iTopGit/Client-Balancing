#ifndef CLIENT_H
#define CLIENT_H

#include "calcuator.h"
#include "node_obj.h"

class ClientOBJ : public NodeOBJ
{
  public:
    ClientOBJ(Ptr<Node> inode)
        : NodeOBJ(inode)
    {
        startClient();
    }

    void connect()
    {
        json data = {{"Tag", "connection"}};
        InetSocketAddress serverAddress = InetSocketAddress(server_ip, 49152);

        if (g_protocol == "TCP")
        {
            Ptr<Socket> serverSocket =
                Socket::CreateSocket(this->node, TypeId::LookupByName("ns3::TcpSocketFactory"));
            serverSocket->Connect(serverAddress);

            // add the socket to the map
            tcpClientSockets[server_ip] = serverSocket;
            clientSocketSend(data, server_ip, "connection");

            // Set the callback for receiving responses from the server
            serverSocket->SetRecvCallback(MakeCallback(&ClientOBJ::tcpReceiveCallback, this));
        }
        else if (g_protocol == "UDP")
        {
            // add the address to the map
            udpClientAddresses.emplace(server_ip, serverAddress);
            clientSocketSend(data, server_ip, "connection");
        }
    }

  private:
    Ipv4Address server_ip = Ipv4Address("192.168.1.1");
    Ptr<Socket> clientSocket;
    map<Ipv4Address, Ptr<Socket>> tcpClientSockets;
    map<Ipv4Address, InetSocketAddress> udpClientAddresses;

    void startClient()
    {
        if (g_protocol == "TCP")
        {
            // Create a TCP socket for listening to incoming connections
            clientSocket =
                Socket::CreateSocket(this->node, TypeId::LookupByName("ns3::TcpSocketFactory"));
            InetSocketAddress localAddress = InetSocketAddress(Ipv4Address::GetAny(), 49152);
            clientSocket->Bind(localAddress);
            clientSocket->Listen();

            // Set accept callback for new TCP connections
            clientSocket->SetAcceptCallback(MakeNullCallback<bool, Ptr<Socket>, const Address&>(),
                                            MakeCallback(&ClientOBJ::handleAccept, this));
        }
        else if (g_protocol == "UDP")
        {
            // Create the UDP socket (same logic as before)
            clientSocket =
                Socket::CreateSocket(this->node, TypeId::LookupByName("ns3::UdpSocketFactory"));
            InetSocketAddress localAddress = InetSocketAddress(Ipv4Address::GetAny(), 49152);
            clientSocket->Bind(localAddress);

            // Set callback to handle incoming UDP data
            clientSocket->SetRecvCallback(MakeCallback(&ClientOBJ::udpReceiveCallback, this));
        }
    }

    void handleAccept(Ptr<Socket> socket, const Address& from) // TCP
    {
        Ipv4Address clientIp = InetSocketAddress::ConvertFrom(from).GetIpv4();
        // Store the accepted socket in a vector
        tcpClientSockets[clientIp] = socket;

        // Set callback for receiving data on this client socket
        socket->SetRecvCallback(MakeCallback(&ClientOBJ::tcpReceiveCallback, this));
    }

    void clientSocketSend(json data, Ipv4Address dest_ip, const string& comment)
    {
        Ptr<Packet> packet = CreateJSONPacket(data);

        if (g_protocol == "TCP")
        {
            Ptr<Socket> clientSocket = tcpClientSockets[dest_ip];
            if (clientSocket == nullptr)
            {
                slog("E: Socket is null " + comment);
                return;
            }
            else if (packet == nullptr)
            {
                slog("E: Packet is null.");
                return;
            }

            SocketSend(clientSocket, emptyAddress, packet, comment);
        }
        else if (g_protocol == "UDP")
        {
            // Check if the IP exists in the map
            auto it = udpClientAddresses.find(dest_ip);
            if (it != udpClientAddresses.end())
            {
                InetSocketAddress clientAddress = it->second;
                SocketSend(clientSocket, clientAddress, packet, comment);
            }
            else
            {
                InetSocketAddress fallbackAddress(dest_ip, 49152);
                SocketSend(clientSocket, fallbackAddress, packet, comment);
            }
        }
    }

    map<Ipv4Address, string> tcpBuffer;
    void tcpReceiveCallback(Ptr<Socket> socket)
    {
        Address senderAddress;
        socket->GetPeerName(senderAddress);
        Ipv4Address senderIp = InetSocketAddress::ConvertFrom(senderAddress).GetIpv4();
        Ptr<Packet> packet = socket->Recv();

        uint8_t* buffer = new uint8_t[packet->GetSize()];
        packet->CopyData(buffer, packet->GetSize());

        string receivedPart(reinterpret_cast<char*>(buffer), packet->GetSize());
        delete[] buffer;

        // Append new data to the TCP buffer
        tcpBuffer[senderIp] += receivedPart;

        // Process each complete JSON object in the buffer
        size_t pos;
        while ((pos = tcpBuffer[senderIp].find("\n")) != string::npos)
        {
            string jsonStr = tcpBuffer[senderIp].substr(0, pos); // Extract complete JSON
            tcpBuffer[senderIp].erase(0, pos + 1);               // Remove processed part

            try
            {
                json data = json::parse(jsonStr);
                filterTag(data, senderIp);
            }
            catch (json::parse_error& e)
            {
                std::cerr << "JSON Parse Error: " << e.what() << std::endl;
                std::cerr << "Raw JSON String: " << jsonStr << std::endl;
            }
        }
    }

    void udpReceiveCallback(Ptr<Socket> socket)
    {
        Address senderAddress;
        Ptr<Packet> packet = socket->RecvFrom(senderAddress);

        Ipv4Address senderIp = InetSocketAddress::ConvertFrom(senderAddress).GetIpv4();
        uint16_t senderPort = InetSocketAddress::ConvertFrom(senderAddress).GetPort();
        InetSocketAddress completeSenderAddress(senderIp, senderPort);

        // add the address to the map
        udpClientAddresses.emplace(senderIp, completeSenderAddress);

        json data = PackettoJson(packet);

        filterTag(data, senderIp);
    }

    void filterTag(json data, Ipv4Address sender_ip)
    {
        if (!data.contains("Tag"))
        {
            slog("Error : Tag not found.");
            return;
        }
        string tag = data["Tag"];

        if (tag == "leaders")
        {
            findGroup(data["Body"]);
        }
        else if (tag == "register")
        {
            registerMember(data["Body"]);
        }
        else if (tag == "matrix")
        {
            sendMatrix(data["Body"]);
        }
    }

    void findGroup(json leaders)
    {
        string min_hop_ip;
        for (const auto& leader : leaders)
        {
            // std::string leader_str = leader.get<std::string>();
            // Ipv4Address leader_ip = Ipv4Address(leader_str.c_str());
        }
        slog("WTP : Find Group (Client.h)");
        min_hop_ip = leaders[0];
        json data = {{"Tag", "leader"}, {"Body", min_hop_ip}};
        clientSocketSend(data, server_ip, "leader");

        Ipv4Address leader_ip = Ipv4Address(min_hop_ip.c_str());

        if (g_protocol == "TCP")
        {
            // Create and connect to the leader socket
            Ptr<Socket> leaderSocket =
                Socket::CreateSocket(this->node, TypeId::LookupByName("ns3::TcpSocketFactory"));
            InetSocketAddress leaderAddress = InetSocketAddress(leader_ip, 49152);
            leaderSocket->Connect(leaderAddress);

            // Store the socket in the TCP map
            tcpClientSockets[leader_ip] = leaderSocket;

            // Set callback for receiving data from the leader
            leaderSocket->SetRecvCallback(MakeCallback(&ClientOBJ::tcpReceiveCallback, this));

            // slog("TCP Connection established with leader: " + min_hop_ip);

            // Send registration message to leader
            json register_data = {{"Tag", "register"}, {"Body", name}};
            clientSocketSend(register_data, leader_ip, "register");
        }
        else if (g_protocol == "UDP")
        {
            json register_data = {{"Tag", "register"}, {"Body", name}};
            clientSocketSend(register_data, leader_ip, "register");
        }
    }

    void registerMember(string body)
    {
        string text = name + " register : " + body;
        slog(text);
    }

    void sendMatrix(json body)
    {
        // data = {{"Body", {{"Group", group_leader}, {"Task", allTask}}}};
        // Extract own task
        json matrix = body["Task"][this->sip];
        // displayJson("Matrix", matrix);
        if (body.size() > 1)
        {
            json task_left = body["Task"];
            task_left.erase(this->sip);
            forwardMatrix(body["Group"], task_left);
        }
        json result = calculateMatrix(matrix);
        json data;
        data["Tag"] = "result";
        data["Body"]["Group"] = body["Group"];
        data["Body"]["Result"] = result;
        // slog(name);
        clientSocketSend(data, server_ip, "result");
    }

    void forwardMatrix(string group_leader, json task_left)
    {
        for (auto& [key, value] : task_left.items())
        {
            json data = {{"Tag", "matrix"}};
            data["Body"]["Group"] = group_leader;
            data["Body"]["Task"] = {{key, value}};
            // displayJson("data", data);
            Ipv4Address destination = Ipv4Address(key.c_str());
            clientSocketSend(data, destination, "forward");
        }
    }

    json calculateMatrix(json all_matrix)
    {
        json all_result = json::array();
        for (const auto& item : all_matrix)
        {
            auto it = item.begin();
            string matrix_name = it.key();
            json json_matrix = it.value();
            vector<vector<int>> matrixA = convJsonToVector(json_matrix[0]);
            vector<vector<int>> matrixB = convJsonToVector(json_matrix[1]);
            vector<vector<int>> result = multiply_matrix(matrixA, matrixB);
            json json_result = json::object();
            json_result[matrix_name] = convVectorToJson(result);
            all_result.push_back(json_result);
        }
        return all_result;
    }
};

#endif // CLIENT_H