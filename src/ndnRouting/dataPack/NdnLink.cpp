#include "NdnLink.h"
using namespace std;
using json = nlohmann::json;

void NdnLink::decode(const char* data, int dataLength) {
    const NdnLinkPacket* packet = (const NdnLinkPacket*)data;
    linkType = (LinkType)(ntohs(packet->linkType));
    linkID = ntohl(packet->linkID);
    linkData = ntohl(packet->linkData);
    linkDataMask = ntohl(packet->linkDataMask);
    linkCost = ntohl(packet->linkCost);
}
pair<int, std::unique_ptr<char[]>> NdnLink::encode() {
    char* buffer = new char[sizeof(NdnLinkPacket)];
    NdnLinkPacket* packet = (NdnLinkPacket*)buffer;
    packet->linkType = htons((uint16_t)linkType);
    packet->linkID = htonl(linkID);
    packet->linkData = htonl(linkData);
    packet->linkDataMask = htonl(linkDataMask);
    packet->linkCost = htonl(linkCost);

    return {sizeof(NdnLinkPacket), unique_ptr<char[]>(buffer)};
}

json NdnLink::marshal()const {
    json j;
    j["linkType"] = linkType;
    j["linkID"] = linkID;
    Ipv4Address data1;
    data1.addr=linkData;
    Ipv4Address data2;
    data2.addr=linkDataMask;
    j["linkData"] = data1.toString();
    j["linkDataMask"] = data2.toString();
    j["linkCost"] = linkCost;
    return j;
}