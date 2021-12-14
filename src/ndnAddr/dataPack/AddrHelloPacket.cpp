#include"AddrHelloPacket.h"
using namespace std;
using nlohmann::json;
struct AddrHelloData{
    uint32_t routerID;
    uint32_t leader;
    uint32_t interfaceNum;
    Ipv4Address addr;
    Ipv4Address mask;
    uint16_t helloInterval;
}__attribute__((__packed__));


void AddrHelloPacket::decode(const char* data, int dataLength){
    const AddrHelloData* ptr=reinterpret_cast<const AddrHelloData*>(data);
    routerID=ntohl(ptr->routerID);
    leader=ntohl(ptr->leader);
    interfaceNum=ntohl(ptr->interfaceNum);
    addr=ptr->addr;
    mask=ptr->mask;
    helloInterval=ntohs(ptr->helloInterval);
}
pair<int, unique_ptr<char[]>> AddrHelloPacket::encode(){
    AddrHelloData* buffer=(AddrHelloData*)(new char[sizeof(AddrHelloData)]);
    buffer->routerID=htonl(routerID);
    buffer->leader=htonl(leader);
    buffer->interfaceNum=htonl(interfaceNum);
    buffer->addr=addr;
    buffer->mask=mask;
    buffer->helloInterval=htons(helloInterval);

    return {sizeof(AddrHelloData),unique_ptr<char[]>((char*)buffer)};
}
nlohmann::json AddrHelloPacket::marshal() const {
    json j;
    j["routerID"]=routerID;
    j["leader"]=leader;
    j["interfaceNum"]=interfaceNum;
    j["addr"]=addr.toString();
    j["mask"]=addr.toString();
    j["helloInterval"]=helloInterval;
    return j;
}