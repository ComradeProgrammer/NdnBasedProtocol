#include"HelloInterestPack.h"
using namespace std;

//Internal class used to resolve or packup hello interest
struct HelloInterestHeader{
    uint32_t _routerId;
    Ipv4Address _interfaceIP;
    Ipv4Address _networkMask;
    uint16_t _helloInterval;
    uint16_t _routerDeadInterval;
}__attribute__((__packed__));

pair<int,unique_ptr<char[]>> HelloInterestPack::encode(){
    //calculate the size
    int size=sizeof(HelloInterestHeader)+4*neighbor.size();
    char* buffer=new char[size];
    //populate the header
    HelloInterestHeader header;
    header._routerId=htonl(routerId);
    header._interfaceIP=interfaceIP;
    header._networkMask=networkMask;
    header._helloInterval=htons(helloInterval);
    header._routerDeadInterval=htons(routerDeadInterval);
    //insert the header;
    memcpy(buffer,&header,sizeof(HelloInterestHeader));
    //insert the neighbors
    char* ptr=buffer+sizeof(HelloInterestHeader);
    for(auto ip: neighbor){
        memcpy(ptr,&ip,4);
        ptr+=4;
    }
    return {size, unique_ptr<char[]>(buffer)};
}


void HelloInterestPack::decode(const char* data, int dataLength){
    // resolve the header
    const HelloInterestHeader* tmp=(const HelloInterestHeader*)(data);
    routerId=ntohl(tmp->_routerId);
    interfaceIP=tmp->_interfaceIP;
    networkMask=tmp->_networkMask;
    helloInterval=ntohs(tmp->_helloInterval);
    routerDeadInterval=ntohs(tmp->_routerDeadInterval);
    //resolve the neightbors;
    const Ipv4Address* neighbors=(const Ipv4Address*)(data+sizeof(HelloInterestHeader));
    int neighborsize=(dataLength-sizeof(HelloInterestHeader))/4;
    for(int i=0;i<neighborsize;i++){
        neighbor.push_back(neighbors[i]);
    }
}