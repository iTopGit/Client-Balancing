#ifndef JSON_MANAGER_H
#define JSON_MANAGER_H

// #include "ns3/packet.h"

// #include "node_obj.h"

// #include <nlohmann/json.hpp>
// // using json = nlohmann::json;


// Ptr<Packet>
// NodeOBJ::CreateJSONPacket(json input)
// {
//     string jsonString = input.dump();
//     const uint8_t* dataByte = reinterpret_cast<const uint8_t*>(jsonString.c_str());
//     uint32_t dataSize = jsonString.size();
//     Ptr<Packet> packet = Create<Packet>(dataByte, dataSize);
//     return packet;
// }

// json
// NodeOBJ::PackettoJson(Ptr<Packet> packet)
// {
//     uint8_t* buffer = new uint8_t[packet->GetSize()];
//     packet->CopyData(buffer, packet->GetSize());

//     string receivedJson(reinterpret_cast<char*>(buffer), packet->GetSize());
//     delete[] buffer;

//     json receivedJsonObject = json::parse(receivedJson);
//     return receivedJsonObject;
// }

#endif // JSON_MANAGER_H