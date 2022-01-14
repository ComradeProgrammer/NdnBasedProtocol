#include "NdnLink.h"
using namespace std;
using json = nlohmann::json;

void NdnLink::decode(const char* data, int dataLength) {
    const NdnLinkPacket* packet = (const NdnLinkPacket*)data;
    linkType = (LinkType)(ntoh(packet->linkType));
    linkID = ntoh(packet->linkID);
    linkData = ntoh(packet->linkData);
    linkDataMask = ntoh(packet->linkDataMask);
    linkCost = ntoh(packet->linkCost);
}
pair<int, std::unique_ptr<char[]>> NdnLink::encode() {
    char* buffer = new char[sizeof(NdnLinkPacket)];
    NdnLinkPacket* packet = (NdnLinkPacket*)buffer;
    packet->linkType = hton((uint16_t)linkType);
    packet->linkID = hton(linkID);
    packet->linkData = hton(linkData);
    packet->linkDataMask = hton(linkDataMask);
    packet->linkCost = hton(linkCost);

    return {sizeof(NdnLinkPacket), unique_ptr<char[]>(buffer)};
}

json NdnLink::marshal() const {
    json j;
    j["linkType"] = linkType;
    j["linkID"] = linkID;
    Ipv4Address data1;
    data1.addr = linkData;
    Ipv4Address data2;
    data2.addr = linkDataMask;
    j["linkData"] = data1.toString();
    j["linkDataMask"] = data2.toString();
    j["linkCost"] = linkCost;
    return j;
}