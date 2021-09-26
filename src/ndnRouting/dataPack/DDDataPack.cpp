#include"DDDataPack.h"
using namespace std;

struct DDDataPacketHeader{
    uint32_t _neightbor;
    //uint16_t _idx;
    uint16_t _interfaceMTU;
    uint16_t _numberOfDDPackets;
}__attribute__((__packed__));

pair<int,unique_ptr<char[]>>DDDataPack::encode(){
    DDDataPacketHeader header;
    header._neightbor=htonl(neightbor);
    //header._idx=
    header._interfaceMTU=htons(interfaceMTU);
    header._numberOfDDPackets=htons(numberOfDDPackets);

    int size=sizeof(DDDataPacketHeader)+ls.size()*sizeof(LinkStateDigest);
    char* buffer=new char[size];
    memcpy(buffer,&header,sizeof(header));

    char* ptr=buffer+sizeof(header);
    for(size_t i=0;i<ls.size();i++){
        auto tmp=ls[i].encode();
        memcpy(ptr,tmp.second.get(),tmp.first);
        ptr+=tmp.first;
    }
    return {size,unique_ptr<char[]>(buffer)};
}
void DDDataPack::decode(const char* data, int dataLength){
    const DDDataPacketHeader* ptr=(const DDDataPacketHeader*)data;
    neightbor=ntohl(ptr->_neightbor);
    interfaceMTU=ntohs(ptr->_interfaceMTU);
    numberOfDDPackets=ntohs(ptr->_numberOfDDPackets);

    const char* ptr2=data+sizeof(DDDataPacketHeader);
    while(ptr2<data+dataLength){
        LinkStateDigest tmp;
        tmp.decode(ptr2,sizeof(LinkStateDigest));
        ls.push_back(tmp);
        ptr+=sizeof(LinkStateDigest);
    }
}