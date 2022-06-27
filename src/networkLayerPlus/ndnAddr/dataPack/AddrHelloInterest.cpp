#include "AddrHelloInterest.h"
using namespace std;
using json = nlohmann::json;

struct AddrHelloInterestHeader {
    RouterID routerID;
    RouterID leader;
    int32_t interfaceNum;
    Ipv4Address addr;
    Ipv4Address mask;
    int helloInterval;
} __attribute__((__packed__));

void AddrHelloInterest::decode(const char* data, int dataLength) {
    const AddrHelloInterestHeader* header = (const AddrHelloInterestHeader*)data;
    routerID = ntoh(header->routerID);
    leader = ntoh(header->leader);
    interfaceNum = ntoh(header->interfaceNum);
    addr = header->addr;
    mask = header->mask;
    helloInterval = ntoh(header->helloInterval);
}

std::pair<int, std::unique_ptr<char[]>> AddrHelloInterest::encode() {
    int size = sizeof(AddrHelloInterestHeader);
    char* buffer = new char[size];
    AddrHelloInterestHeader* header = (AddrHelloInterestHeader*)buffer;
    header->routerID = hton(routerID);
    header->leader = hton(leader);
    header->interfaceNum = hton(interfaceNum);
    header->addr = addr;
    header->mask = mask;
    header->helloInterval = hton(helloInterval);
    return {size, unique_ptr<char[]>(buffer)};
}

nlohmann::json AddrHelloInterest::marshal() const {
    json j;
    j["routerID"] = routerID;
    j["leader"] = leader;
    j["interfaceNum"] = interfaceNum;
    j["addr"] = addr.toString();
    j["mask"] = mask.toString();
    j["helloInterval"] = helloInterval;
    return j;
}