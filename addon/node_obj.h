#ifndef NODE_OBJ_H
#define NODE_OBJ_H

#include <iomanip>
#include <iostream>

#include "global_values.h"

using json = nlohmann::json;

class NodeOBJ
{
  public:
    Ptr<Node> node;
    string name;
    Ipv4Address ip;
    string sip;
    vector<u_int16_t> port;

    NodeOBJ(Ptr<Node> inode)
    {
        node = inode;
        name = Names::FindName(node);
        ip = node->GetObject<Ipv4>()->GetAddress(1, 0).GetLocal();
        sip = convIPtoString(ip);
        slog(name + " is created.");
    }

    ~NodeOBJ()
    {
        cout << name << " is destroyed." << endl;
    }

    Ptr<Packet> CreateJSONPacket(json input)
    {
        string jsonString = input.dump();
        const uint8_t* dataByte = reinterpret_cast<const uint8_t*>(jsonString.c_str());
        uint32_t dataSize = jsonString.size();
        Ptr<Packet> packet = Create<Packet>(dataByte, dataSize);
        return packet;
    }

    json PackettoJson(Ptr<Packet> packet)
    {
        uint8_t* buffer = new uint8_t[packet->GetSize()];
        packet->CopyData(buffer, packet->GetSize());

        string receivedJson(reinterpret_cast<char*>(buffer), packet->GetSize());
        delete[] buffer;

        json receivedJsonObject = json::parse(receivedJson);
        return receivedJsonObject;
    }

  protected:
    struct addressInfo
    {
        Ipv4Address ip;
        uint16_t port;

        addressInfo(Ipv4Address address, uint16_t p)
            : ip(address),
              port(p)
        {
        }
    };

    string convIPtoString(Ipv4Address conv_ip)
    {
        ostringstream oss;
        conv_ip.Print(oss);
        return oss.str();
    }

    Ipv4Address getSenderIP(Address sender_address)
    {
        InetSocketAddress inetSenderAddress = InetSocketAddress::ConvertFrom(sender_address);
        Ipv4Address senderIp = inetSenderAddress.GetIpv4();
        // uint16_t senderPort = inetSenderAddress.GetPort();
        return senderIp;
    }

    void SocketSend(Ptr<Node> sourceNode,
                    addressInfo destAddress,
                    Ptr<Packet> packet,
                    string comment = "")
    {
        Ptr<Socket> senderSocket =
            Socket::CreateSocket(sourceNode, TypeId::LookupByName("ns3::UdpSocketFactory"));
        InetSocketAddress remoteAddress = InetSocketAddress(destAddress.ip, destAddress.port);
        senderSocket->Connect(remoteAddress);
        senderSocket->Send(packet);
        // SendLog(sourceNode, destAddress, comment);
    }

    void displayJson(const string& name, const json& data)
    {
        string base = name + " : ";
        if (data.empty())
        {
            slog(base + "is empty");
            return;
        }
        string jsonString = data.dump(4);
        slog(base + jsonString);
    }

    void slog(string message)
    {
        if (debug)
        {
            cout << fixed << setprecision(4) << Simulator::Now().GetSeconds() * 1000 << "ms "
                 << message << endl;
        }
    }

    vector<vector<int>> convJsonToVector(const json& jsonArray)
    {
        vector<vector<int>> result;

        // Iterate through the outer JSON array
        for (const auto& innerArray : jsonArray)
        {
            vector<int> innerVector;

            // Convert each inner JSON array to vector<int>
            for (const auto& element : innerArray)
            {
                innerVector.push_back(element);
            }

            // Add the inner vector to the result
            result.push_back(innerVector);
        }

        return result;
    }

    json convVectorToJson(const std::vector<std::vector<int>>& vec)
    {
        json jsonArray = json::array();

        // Iterate through the outer vector
        for (const auto& innerVector : vec)
        {
            json innerArray = json::array();

            // Add each inner vector to the JSON array
            for (const auto& element : innerVector)
            {
                innerArray.push_back(element);
            }

            jsonArray.push_back(innerArray);
        }

        return jsonArray;
    }

  private:
    map<Ipv4Address, string> ip_name = {{Ipv4Address("192.168.1.1"), "Server"},
                                        {Ipv4Address("192.168.2.2"), "Client 1"},
                                        {Ipv4Address("192.168.2.3"), "Client 2"},
                                        {Ipv4Address("192.168.2.4"), "Client 3"},
                                        {Ipv4Address("192.168.2.5"), "Client 4"},
                                        {Ipv4Address("192.168.3.2"), "Client 2"},
                                        {Ipv4Address("192.168.4.2"), "Client 3"},
                                        {Ipv4Address("192.168.5.2"), "Client 4"}};

    void SendLog(Ptr<Node> src_node, addressInfo dest_sock, string comment)
    {
        string source = Names::FindName(src_node);
        if (source == "Server")
        {
            source += "  ";
        }
        string destination = ip_name[dest_sock.ip];
        string text = "Send : " + source + " >> " + destination;
        if (comment != "")
        {
            text += " ( " + comment + " )";
        }
        slog(text);
    }
};

#endif // NODE_OBJ_H
