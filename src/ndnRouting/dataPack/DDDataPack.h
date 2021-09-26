#ifndef __DDDATAPACK_H_ 
#define __DDDATAPACK_H_
#include<cstdint>
#include<vector>
#include<unordered_map>
#include<memory>
#include<cstring>
#include <arpa/inet.h>
#include"LinkStateDigest.h"
class DDDataPack{
    public:
    uint32_t neightbor;
    //uint16_t idx;//标识这是第几个分片,其实和名字里的那个是一样的
    uint16_t interfaceMTU;
    uint16_t numberOfDDPackets;
    std::vector<LinkStateDigest>ls;
    public:
    void decode(const char* data, int dataLength);
    std::pair<int,std::unique_ptr<char[]>>encode();
};
#endif