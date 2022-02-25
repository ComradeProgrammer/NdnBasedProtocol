#include "RegisterInterestPack.h"

using namespace std;
using nlohmann::json;
void RegisterInterestPack::decode(const char* data, int dataLength) {
    for (int i = 0; i < 16; i++) {
        databaseHash[i] = data[i];
    }
    int rootNum = (dataLength - 16) / sizeof(RouterID);
    const RouterID* ptr = (const RouterID*)(data + 16);
    for (int i = 0; i < rootNum; i++) {
        roots.push_back(ntoh(*ptr));
        ptr++;
    }
}
pair<int, std::unique_ptr<char[]>> RegisterInterestPack::encode() {
    // first,calculate total memory we need
    int size = 16 + sizeof(RouterID) * (roots.size());
    char* buffer = new char[size];
    for (int i = 0; i < 16; i++) {
        buffer[i] = (char)(databaseHash[i]);
    }
    RouterID* ptr = (RouterID*)(buffer + 16);
    for (int i = 0; i < roots.size(); i++) {
        *ptr = hton(roots[i]);
        ptr++;
    }
    return {size, unique_ptr<char[]>(buffer)};
}
json RegisterInterestPack::marshal() const {
    json res;
    res["databaseHash"] = hexString(databaseHash, 16);
    res["roots"] = roots;
    return res;
}