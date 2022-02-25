#include "RegisterDataPack.h"
using namespace std;
using nlohmann::json;

void RegisterDataPack::decode(const char* data, int dataLength) {
    const bool* ptr = (const bool*)data;
    ok = *ptr;
}
std::pair<int, std::unique_ptr<char[]>> RegisterDataPack::encode() {
    char* buffer = new char[1];
    bool* ptr = (bool*)buffer;
    *ptr = ok;
    return {1, unique_ptr<char[]>(buffer)};
}
nlohmann::json RegisterDataPack::marshal() const {
    json res;
    res["ok"] = ok;
    return res;
}