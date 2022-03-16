#include "HelloInterestPack.h"
using namespace std;
using json = nlohmann::json;

// Internal class used to resolve or packup hello interest
struct HelloInterestHeader {
    RouterID _routerId;
    Ipv4Address _interfaceIP;
    Ipv4Address _networkMask;
    uint16_t _helloInterval;
    uint16_t _routerDeadInterval;
    unsigned char _databaseHash[16];
} __attribute__((__packed__));

pair<int, unique_ptr<char[]>> HelloInterestPack::encode() {
    // calculate the size
    int size = sizeof(HelloInterestHeader) + 4 * neighbor.size();
    char* buffer = new char[size];
    // populate the header
    HelloInterestHeader header;
    header._routerId = hton(routerId);
    header._interfaceIP = interfaceIP;
    header._networkMask = networkMask;
    header._helloInterval = hton(helloInterval);
    header._routerDeadInterval = hton(routerDeadInterval);
    memcpy(header._databaseHash, databaseHash, 16);
    // insert the header;
    memcpy(buffer, &header, sizeof(HelloInterestHeader));
    // insert the neighbors
    char* ptr = buffer + sizeof(HelloInterestHeader);
    for (auto ip : neighbor) {
        memcpy(ptr, &ip, 4);
        ptr += 4;
    }
    return {size, unique_ptr<char[]>(buffer)};
}

void HelloInterestPack::decode(const char* data, int dataLength) {
    // resolve the header
    const HelloInterestHeader* tmp = (const HelloInterestHeader*)(data);
    routerId = ntoh(tmp->_routerId);
    interfaceIP = tmp->_interfaceIP;
    networkMask = tmp->_networkMask;
    helloInterval = ntoh(tmp->_helloInterval);
    routerDeadInterval = ntoh(tmp->_routerDeadInterval);
    memcpy(databaseHash, tmp->_databaseHash, 16);
    // resolve the neightbors;
    const Ipv4Address* neighbors = (const Ipv4Address*)(data + sizeof(HelloInterestHeader));
    int neighborsize = (dataLength - sizeof(HelloInterestHeader)) / 4;
    for (int i = 0; i < neighborsize; i++) {
        neighbor.push_back(neighbors[i]);
    }
}
json HelloInterestPack::marshal() const {
    json j;
    j["routerId"] = routerId;
    j["interfacIP"] = interfaceIP.toString();
    j["networkMask"] = networkMask.toString();
    j["helloInterval"] = helloInterval;
    j["routerDeadInterval"] = routerDeadInterval;
    j["databaseHash"] = hexString(databaseHash, 16);
    vector<string> res;
    for (auto i : neighbor) {
        res.push_back(i.toString());
    }
    j["neighbor"] = res;
    return j;
}
