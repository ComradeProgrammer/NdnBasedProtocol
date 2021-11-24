#include "Ipv4Address.h"

using namespace std;

void Ipv4Address::setIp(string s) { inet_pton(AF_INET, s.c_str(), (void*)this); }

string Ipv4Address::toString() const{
    char buffer[20];
    inet_ntop(AF_INET, (void*)this, buffer, 20);
    return string(buffer);
}

Ipv4Address Ipv4Address::andMask(Ipv4Address mask){
    uint32_t addrInHostSequence=ntohl(addr);
    uint32_t maskInHostSequence=ntohl(mask.addr);
    Ipv4Address res;
    res.addr=htonl(addrInHostSequence&maskInHostSequence);
    return res;
}

int Ipv4Address::getPrefixLength(){
    uint32_t mask=ntohl(addr);
    for(int i=0;i<32;i++){
        if(mask&(1<<i)){
            return 32-i;
        }
    }
    return -1;
}

