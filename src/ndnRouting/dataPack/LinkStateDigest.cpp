#include "LinkStateDigest.h"
using namespace std;
using json = nlohmann::json;

void LinkStateDigest::decode(const char* data, int dataLength) {
    const LinkStateDigestPacket* ptr = (const LinkStateDigestPacket*)data;
    routerID = ntohl(ptr->routerID);
    linkStateType = (LinkStateType)(ntohs(ptr->linkStateType));
    sequenceNum = ntohl(ptr->sequenceNum);
    lsAge = ntohs(ptr->lsAge);
}
std::pair<int, std::unique_ptr<char[]>> LinkStateDigest::encode() {
    LinkStateDigestPacket packet;
    packet.routerID = htonl(routerID);
    packet.linkStateType = htons(linkStateType);
    packet.sequenceNum = htonl(sequenceNum);
    packet.lsAge = htons(lsAge);

    char* buffer = new char[sizeof(LinkStateDigestPacket)];
    memcpy(buffer, &packet, sizeof(LinkStateDigestPacket));
    return {sizeof(LinkStateDigestPacket), unique_ptr<char[]>(buffer)};
}
bool LinkStateDigest::operator<(const LinkStateDigest& o){
    if(sequenceNum!=o.sequenceNum){
        return sequenceNum<o.sequenceNum;
    }
    //TODO:else if ((abs(leftAge - rightAge) > MAX_AGE_DIFF) && (leftAge > rightAge))??
    return lsAge>o.lsAge;
}

string LinkStateDigest::toString(){
    json j;
    j["routerID"]=routerID;
    j["linkStateType"]=getNameForLinkStateType(linkStateType);
    j["sequenceNumber"]=sequenceNum;
    j["lsAge"]=lsAge;
    return j.dump();
}