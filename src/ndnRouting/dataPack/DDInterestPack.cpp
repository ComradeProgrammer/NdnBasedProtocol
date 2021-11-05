#include "DDInterestPack.h"

using namespace std;
using json = nlohmann::json;

pair<int, unique_ptr<char[]>> DDInterestPack::encode() {
    int size = sizeof(DDInterestPack);
    char* buffer = new char[size];
    int neighborBigEndian = htonl(neighbor);
    memcpy(buffer, &neighborBigEndian, 4);
    return {size, unique_ptr<char[]>(buffer)};
}

void DDInterestPack::decode(const char* data, int dataLength) {
    int neighborBigEndian = (*((uint32_t*)data));
    neighbor = ntohl(neighborBigEndian);
    return;
}
json DDInterestPack::marshal(){
    json j;
    j["neighbor"]=neighbor;
    return j;
}