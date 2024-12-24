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
        Ptr<Packet> packet = CreateJSONPacket(data);
        SocketSend(this->node, server_socket, packet, "connection");
    }

  private:
    addressInfo server_socket = addressInfo(Ipv4Address("192.168.1.1"), 49152);

    void startClient()
    {
        Ptr<Socket> clientSocket =
            Socket::CreateSocket(this->node, TypeId::LookupByName("ns3::UdpSocketFactory"));
        InetSocketAddress localAddress = InetSocketAddress(Ipv4Address::GetAny(), 49152);
        clientSocket->Bind(localAddress);
        clientSocket->SetRecvCallback(MakeCallback(&ClientOBJ::clientCallback, this));
    }

    void clientCallback(Ptr<Socket> socket)
    {
        Address senderAddress;
        Ptr<Packet> packet = socket->RecvFrom(senderAddress);
        // Ipv4Address sender_ip = getSenderIP(senderAddress);
        json data = PackettoJson(packet);
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
        else if (tag == "matrix")
        {
            sendMatrix(data["Body"]);
        }
    }

    void findGroup(json leaders)
    {
        string min_hop_ip;
        slog("WIP : Find Group");
        // loop in data
        //     cal min
        min_hop_ip = leaders[0];
        json data = {{"Tag", "leader"}, {"Body", min_hop_ip}};
        Ptr<Packet> packet = CreateJSONPacket(data);
        SocketSend(node, server_socket, packet, "leader");
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
        // displayJson("Result", data);
        Ptr<Packet> packet = CreateJSONPacket(data);
        SocketSend(node, server_socket, packet, "result");
    }

    void forwardMatrix(string group_leader, json task_left)
    {
        for (auto& [key, value] : task_left.items())
        {
            json data = {{"Tag", "matrix"}};
            data["Body"]["Group"] = group_leader;
            data["Body"]["Task"] = {{key, value}};
            // displayJson("data", data);
            Ptr<Packet> packet = CreateJSONPacket(data);
            Ipv4Address destination = Ipv4Address(key.c_str());
            addressInfo address(destination, 49152);
            SocketSend(this->node, address, packet, "matrix");
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