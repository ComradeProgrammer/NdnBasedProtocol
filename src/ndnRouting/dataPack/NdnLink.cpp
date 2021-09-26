#include"NdnLink.h"
using namespace std;

struct NdnLinkPacket{
    uint16_t linkType;
    uint32_t linkID;
    uint32_t linkData ;
    uint32_t linkDataMask;
    uint32_t linkCost;
}__attribute__((__packed__));

void NdnLink::decode(const char* data, int dataLength){
    const NdnLinkPacket* packet=(const NdnLinkPacket *)data;
    linkType=(LinkType)(ntohs(packet->linkType));
    linkID=ntohl(packet->linkID);
    linkData=ntohl(packet->linkData);
    linkDataMask=ntohl(packet->linkDataMask);
    linkCost=ntohl(packet->linkCost);
}
pair<int,std::unique_ptr<char[]>>NdnLink::encode(){
    char* buffer=new char[sizeof(NdnLinkPacket)];
    NdnLinkPacket* packet=(NdnLinkPacket *)buffer;
    packet->linkType=htons((uint16_t)linkType);
    packet->linkID=htonl(linkID);
    packet->linkData=htonl(linkData);
    packet->linkDataMask=htonl(linkDataMask);
    packet->linkCost=htonl(linkCost);

    return {sizeof(NdnLinkPacket),unique_ptr<char[]>(buffer)};
}