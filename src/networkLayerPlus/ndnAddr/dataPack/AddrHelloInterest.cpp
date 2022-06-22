#include "AddrHelloInterest.h"
using namespace std;
using json = nlohmann::json;

struct AddrHelloInterestHeader {
    RouterID routerID;
    RouterID leader;
    int32_t requiredSize;
    Ipv4Address addr;
    Ipv4Address mask;
} __attribute__((__packed__));

void AddrHelloInterest::decode(const char* data, int dataLength) {
    const AddrHelloInterestHeader* header = (const AddrHelloInterestHeader*)data;
    routerID = ntoh(header->routerID);
    leader = ntoh(header->leader);
    requiredSize = ntoh(header->requiredSize);
    addr = header->addr;
    mask = header->mask;
}

std::pair<int, std::unique_ptr<char[]>> AddrHelloInterest::encode() {
    int size = sizeof(AddrHelloInterestHeader);
    char* buffer = new char[size];
    AddrHelloInterestHeader* header = (AddrHelloInterestHeader*)buffer;
    header->routerID = hton(routerID);
    header->leader = hton(leader);
    header->requiredSize = hton(requiredSize);
    header->addr = addr;
    header->mask = mask;
    return {size, unique_ptr<char[]>(buffer)};
}

nlohmann::json AddrHelloInterest::marshal() const {
    json j;
    j["routerID"] = routerID;
    j["leader"] = leader;
    j["requiredSize"] = requiredSize;
    j["addr"] = addr.toString();
    j["mask"] = mask.toString();
}