#include "DDInterestPack.h"

using namespace std;
using json = nlohmann::json;

pair<int, unique_ptr<char[]>> DDInterestPack::encode() {
    int size = sizeof(DDInterestPack);
    char* buffer = new char[size];

    RouterID neighborBigEndian = hton(neighbor);
    memcpy(buffer, &neighborBigEndian, size);
    return {size, unique_ptr<char[]>(buffer)};
}

void DDInterestPack::decode(const char* data, int dataLength) {
    RouterID neighborBigEndian = (*((RouterID*)data));
    neighbor = ntoh(neighborBigEndian);
    return;
}
json DDInterestPack::marshal() const {
    json j;
    j["neighbor"] = neighbor;
    return j;
}