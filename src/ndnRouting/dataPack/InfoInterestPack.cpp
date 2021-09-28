#include "InfoInterestPack.h"
using namespace std;

struct InfoInterestPackHeader {
    uint16_t infoType;
    uint16_t ptrToNeighbors;
    uint32_t src;
} __attribute__((__packed__));

void InfoInterestPack::decode(const char* data, int dataLength) {
    const InfoInterestPackHeader* header = (const InfoInterestPackHeader*)data;
    infoType = (InfoType)(ntohs(header->infoType));
    src = ntohl(header->src);
    int ptrToNeighbors = ntohs(header->ptrToNeighbors);

    // resolve the link state digest array
    LinkStateDigestPacket* ptr1 =
        (LinkStateDigestPacket*)(data + sizeof(LinkStateDigestPacket));
    int lsSize = (ptrToNeighbors - sizeof(InfoInterestPackHeader)) /
                 (sizeof(LinkStateDigestPacket));
    for (int i = 0; i < lsSize; i++) {
        LinkStateDigest digest;
        digest.decode((const char*)(ptr1 + i), sizeof(LinkStateDigestPacket));
        ls.push_back(digest);
    }
    // decode the neighbors array
    uint32_t* ptr2 = (uint32_t*)(data + ptrToNeighbors);
    int neighborSize = (dataLength - ptrToNeighbors) / 4;
    for (int i = 0; i < neighborSize; i++) {
        neighbors.push_back(ntohl(ptr2[i]));
    }
}

pair<int, unique_ptr<char[]>> InfoInterestPack::encode() {
    // calculate the packet size;
    int size = sizeof(InfoInterestPackHeader) +
               sizeof(LinkStateDigestPacket) * ls.size() + 4 * neighbors.size();
    char* buffer = new char[size];
    InfoInterestPackHeader* header = (InfoInterestPackHeader*)buffer;
    // encode the header part
    header->infoType = htons((uint16_t)infoType);
    header->src = htonl(src);
    header->ptrToNeighbors = htons(sizeof(InfoInterestPackHeader) +
                                   sizeof(LinkStateDigestPacket) * ls.size());

    // encode the link state digest array
    char* ptr = buffer + sizeof(InfoInterestPackHeader);
    for (int i = 0; i < ls.size(); i++) {
        auto tmp = ls[i].encode();
        memcpy(ptr, tmp.second.get(), tmp.first);
        ptr += tmp.first;
    }

    // encode the neighbors array
    uint32_t* ptr2 = (uint32_t*)ptr;
    for (int i = 0; i < neighbors.size(); i++) {
        *ptr2 = htonl(neighbors[i]);
        ptr2++;
    }
    return {size, unique_ptr<char[]>(buffer)};
}