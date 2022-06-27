#include "AddrRequestData.h"
using namespace std;
using json = nlohmann::json;

struct AddrRequestDataHeader {
    Ipv4Address startAddr;
    Ipv4Address mask;
} __attribute__((__packed__));

void AddrRequestData::decode(const char* data, int dataLength) {
    const AddrRequestData* header = (const AddrRequestData*)(data);
    startAddr = header->startAddr;
    mask = header->mask;
}
std::pair<int, std::unique_ptr<char[]>> AddrRequestData::encode() {
    int size = sizeof(AddrRequestDataHeader);
    char* buffer = new char[size];
    AddrRequestDataHeader* header = (AddrRequestDataHeader*)(buffer);
    header->startAddr = startAddr;
    header->mask = mask;

    return {size, unique_ptr<char[]>(buffer)};
}
nlohmann::json AddrRequestData::marshal() const {
    json j;
    j["startAddr"] = startAddr.toString();
    j["mask"] = mask.toString();
    return j;
}