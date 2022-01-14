#include "LinkStateDigest.h"
using namespace std;
using json = nlohmann::json;

void LinkStateDigest::decode(const char* data, int dataLength) {
    const LinkStateDigestPacket* ptr = (const LinkStateDigestPacket*)data;
    routerID = ntoh(ptr->routerID);
    linkStateType = (LinkStateType)(ntoh(ptr->linkStateType));
    sequenceNum = ntoh(ptr->sequenceNum);
    lsAge = ntoh(ptr->lsAge);
}
std::pair<int, std::unique_ptr<char[]>> LinkStateDigest::encode() {
    LinkStateDigestPacket packet;
    packet.routerID = hton(routerID);
    packet.linkStateType = hton(uint16_t(linkStateType));
    packet.sequenceNum = hton(sequenceNum);
    packet.lsAge = hton(lsAge);

    char* buffer = new char[sizeof(LinkStateDigestPacket)];
    memcpy(buffer, &packet, sizeof(LinkStateDigestPacket));
    return {sizeof(LinkStateDigestPacket), unique_ptr<char[]>(buffer)};
}
bool LinkStateDigest::operator<(const LinkStateDigest& o) {
    if (sequenceNum != o.sequenceNum) {
        return sequenceNum < o.sequenceNum;
    }
    // TODO:else if ((abs(leftAge - rightAge) > MAX_AGE_DIFF) && (leftAge > rightAge))??
    return lsAge > o.lsAge;
}

json LinkStateDigest::marshal() const {
    json j;
    j["routerID"] = routerID;
    j["linkStateType"] = getNameForLinkStateType(linkStateType);
    j["sequenceNumber"] = sequenceNum;
    j["lsAge"] = lsAge;
    return j;
}