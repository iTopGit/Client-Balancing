#ifndef SERVER_H
#define SERVER_H

#include "node_obj.h"
#include "global_values.h"
#include "custom_timer.h"


#include <random>

class ServerOBJ : public NodeOBJ
{
  public:
    ServerOBJ(Ptr<Node> inode)
        : NodeOBJ(inode)
    {
        startServer();
    }

    CustomTimer processingTimer;
    CustomTimer resultTimer;

    void startTrace()
    {
        Ptr<Ipv4> serverIpv4 = this->node->GetObject<Ipv4>();
        serverIpv4->TraceConnectWithoutContext("Rx", MakeCallback(&ServerOBJ::ServerTrace, this));
    }

    void sendMatrix()
    {
        // ____________________ //
        // Check leader Traffic //
        // ____________________ //
        slog("Send Matrix");

        processingTimer.start();
        resultTimer.start();

        string group_leader = checkDistance();

        json allTask = createTask(group_leader);
        clearResult(group_leader);

        if (g_send_type == "balance")
        {
            json data = {{"Tag", "matrix"}};
            data["Body"]["Group"] = group_leader;
            data["Body"]["Task"] = allTask;
            // displayJson("data", data);
            Ptr<Packet> packet = CreateJSONPacket(data);
            Ipv4Address destination = Ipv4Address(group_leader.c_str());
            addressInfo address(destination, 49152);
            SocketSend(this->node, address, packet, "matrix");
        }
        else // Basic
        {
            slog("Send Type : Basic ( " + g_send_type + " )");
            for (auto& [key, value] : allTask.items())
            {
                json data = {{"Tag", "matrix"}};
                json task = {{key, value}};
                data["Body"]["Group"] = group_leader;
                data["Body"]["Task"] = task;
                // displayJson("data", data);
                Ptr<Packet> packet = CreateJSONPacket(data);
                Ipv4Address destination = Ipv4Address(key.c_str());
                addressInfo address(destination, 49152);
                SocketSend(this->node, address, packet, "matrix");
            }
        }

        processingTimer.stop();
        d_computational_time.push_back(processingTimer.getElapsedTime());
    }

  private:
    json hop, group, result_matrix;
    std::chrono::steady_clock::time_point start_time;
    double cpu_time_spent = 0.0; // Tracks the cumulative time spent in operations.

    void startServer()
    {
        Ptr<Socket> serverSocket =
            Socket::CreateSocket(this->node, TypeId::LookupByName("ns3::UdpSocketFactory"));
        InetSocketAddress localAddress = InetSocketAddress(Ipv4Address::GetAny(), 49152);
        serverSocket->Bind(localAddress);
        serverSocket->SetRecvCallback(MakeCallback(&ServerOBJ::serverCallback, this));
    }

    void serverCallback(Ptr<Socket> socket)
    {
        Address senderAddress;
        Ptr<Packet> packet = socket->RecvFrom(senderAddress);
        Ipv4Address sender_ip = getSenderIP(senderAddress);
        json data = PackettoJson(packet);
        if (!data.contains("Tag"))
        {
            return;
        }
        string tag = data["Tag"];
        if (tag == "connection")
        {
            createConnection(sender_ip);
        }
        else if (tag == "leader")
        {
            addMember(sender_ip, data["Body"]);
        }
        else if (tag == "result")
        {
            combineResult(data["Body"]);
        }
    }

    void ServerTrace(Ptr<const Packet> packet, Ptr<Ipv4> ipv4, uint32_t interfaceId)
    {
        Ipv4Header ipv4Header;
        Ptr<Packet> copy = packet->Copy();
        if (!copy->PeekHeader(ipv4Header))
        {
            return;
        }
        // receive IP
        Ipv4Address sender_ip = ipv4Header.GetSource();
        string string_sender_ip = convIPtoString(sender_ip);

        // receive Hop
        uint8_t TTL = ipv4Header.GetTtl();
        int hop_count = 64 - TTL;

        // update hop
        hop[string_sender_ip] = hop_count;
    }

    void createConnection(Ipv4Address sender_ip)
    {
        if (group.empty()) // Create New Group
        {
            string string_sender_ip = convIPtoString(sender_ip);
            group[string_sender_ip] = {{"leader_hop", hop[string_sender_ip]},
                                       {"member", json::array()}};
            displayJson("Group", group);
            slog("Connection Complete");
        }
        else // Ack available Group
        {
            json data = {{"Tag", "leaders"}, {"Body", json::array()}};
            for (auto it = group.begin(); it != group.end(); ++it)
            {
                data["Body"].push_back(it.key());
            }
            Ptr<Packet> packet = CreateJSONPacket(data);
            addressInfo address(sender_ip, 49152);
            SocketSend(node, address, packet, "leaders");
        }
    }

    void addMember(Ipv4Address member, string leader)
    {
        string sMember = convIPtoString(member);
        if (find(group[leader]["member"].begin(), group[leader]["member"].end(), sMember) ==
            group[leader]["member"].end())
        {
            group[leader]["member"].push_back(sMember);
            displayJson("Group", group);
            slog("Connection Complete");
        }
        else
        {
            slog("Member already exists: " + sMember);
        }
    }

    string checkDistance()
    {
        if (g_distance == "traffic") {
            return minTrafficIP();
        } else { // hop
            return minHopIP();
        }
    }

    string minHopIP()
    {
        string min_hop_ip;
        int min_hop = numeric_limits<int>::max();
        for (auto& [key, value] : group.items())
        {
            if (value["leader_hop"] < min_hop)
            {
                min_hop = value["leader_hop"];
                min_hop_ip = key;
            }
        }
        return min_hop_ip;
    }

    string minTrafficIP() {
        slog("WIP : Check Distance");
        return group.begin().key();
    }

    int getRandomValue(int min, int max)

    {
        static random_device rd;  // Seed for random number generator
        static mt19937 gen(rd()); // Mersenne Twister generator
        std::uniform_int_distribution<> dis(min, max);
        return dis(gen);
    }

    json createMatrix()
    {
        int matrix_size = g_matrix_size;
        int max = g_value_range;

        json matrix = json::array();
        for (int i = 0; i < matrix_size; ++i)
        {
            json rowArray = json::array();
            for (int j = 0; j < matrix_size; ++j)
            {
                int x = getRandomValue(0, max);
                rowArray.push_back(x); // Use any default value you prefer
            }
            matrix.push_back(rowArray);
        }
        return matrix;
    }

    json divideMatrix(const json& matrix)
    {
        // Divides a matrix into four submatrices and returns them in a json::array.
        size_t n = matrix.size();
        size_t mid = n / 2;

        json A11 = json::array();
        json A12 = json::array();
        json A21 = json::array();
        json A22 = json::array();

        for (size_t i = 0; i < mid; ++i)
        {
            A11.push_back(json::array());
            A12.push_back(json::array());
            for (size_t j = 0; j < mid; ++j)
            {
                A11[i].push_back(matrix[i][j]);
            }
            for (size_t j = mid; j < n; ++j)
            {
                A12[i].push_back(matrix[i][j]);
            }
        }

        for (size_t i = mid; i < n; ++i)
        {
            A21.push_back(json::array());
            A22.push_back(json::array());
            for (size_t j = 0; j < mid; ++j)
            {
                A21[i - mid].push_back(matrix[i][j]);
            }
            for (size_t j = mid; j < n; ++j)
            {
                A22[i - mid].push_back(matrix[i][j]);
            }
        }
        return json::array({A11, A12, A21, A22});
    }

    json createTask(string leader)
    {
        json matrixA = createMatrix();
        json matrixB = createMatrix();
        json devided_matrixA = divideMatrix(matrixA);
        json devided_matrixB = divideMatrix(matrixB);

        json a = devided_matrixA[0];
        json b = devided_matrixA[1];
        json c = devided_matrixA[2];
        json d = devided_matrixA[3];
        json e = devided_matrixB[0];
        json f = devided_matrixB[1];
        json g = devided_matrixB[2];
        json h = devided_matrixB[3];

        json task = {{{"ae", {a, e}}},
                     {{"bg", {b, g}}},
                     {{"ah", {a, h}}},
                     {{"bf", {b, f}}},
                     {{"ce", {c, e}}},
                     {{"dg", {d, g}}},
                     {{"cf", {c, f}}},
                     {{"dh", {d, h}}}};

        json cluster = json::array();
        cluster.push_back(leader);

        json group_member = group[leader]["member"];
        cluster.insert(cluster.end(), group_member.begin(), group_member.end());
        // displayJson("Cluster", cluster);
        json all_task = {{leader, json::array()}};
        int n = cluster.size();
        for (int i = 0; i < n; ++i)
        {
            string member = cluster[i];
            all_task[member] = json::array();
        }
        int task_size = task.size();
        for (int i = 0; i < task_size; ++i)
        {
            int index = i % n;
            string member = cluster[index];
            all_task[member].push_back(task[i]);
        }
        // displayJson("all_task", all_task);
        return all_task;
    }

    void clearResult(string leader)
    {
        // result_matrix = {"leader", [{}, {}, {}, {}]};
        result_matrix[leader] = json::array();
    }

    void combineResult(json body)
    {
        // displayJson("Result", body);
        string leader = body["Group"];
        json result = body["Result"];
        for (auto& item : result)
        {
            result_matrix[leader].push_back(item);
        }
        if (result_matrix[leader].size() == 8)
        {
            calculateResult(leader);
            clearResult(leader);
        }
    }

    void calculateResult(string leader)
    {
        string result = "Task Complete ( " + leader + " )";
        slog(result);
        resultTimer.stop();
        d_result_time.push_back(resultTimer.getElapsedTime());
    }

    // void startProcessing()
    // {
    //     start_time = std::chrono::steady_clock::now();
    // }

    // void endProcessing()
    // {
    //     auto end_time = std::chrono::steady_clock::now();
    //     std::chrono::duration<double, std::milli> elapsed = end_time - start_time;
    //     cpu_time_spent += elapsed.count();

    //     std::ostringstream stream;
    //     stream << std::fixed << std::setprecision(4) << cpu_time_spent;

    //     slog("Computational time : " + stream.str() + " ms.");
    // }
};

#endif // SERVER_H