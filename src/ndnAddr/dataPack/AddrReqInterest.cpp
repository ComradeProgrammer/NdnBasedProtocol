#include "AddrReqInterest.h"
using namespace std;
using nlohmann::json;
struct AddrReqInterestRaw{
    uint32_t num;
}__attribute__((__packed__));

void AddrReqInterest::decode(const char* data, int dataLength) {
    const AddrReqInterestRaw* ptr=reinterpret_cast<const AddrReqInterestRaw*>(data);
    num=ntohl(ptr->num);
}
pair<int, unique_ptr<char[]>> AddrReqInterest::encode() {
    AddrReqInterestRaw* buffer=(AddrReqInterestRaw*)(new char[sizeof(AddrReqInterestRaw)]);
    buffer->num=htonl(num);

    return {sizeof(AddrReqInterestRaw),unique_ptr<char[]>((char*)buffer)};
}
json AddrReqInterest::marshal() const {
    json j;
    j["num"]=num;
    return j;
}