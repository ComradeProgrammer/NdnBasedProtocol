#ifndef __NDNLINK_H_
#define __NDNLINK_H_
#include<cstdint>
#include<vector>
#include<unordered_map>
#include<memory>
#include<cstring>
#include <arpa/inet.h>
enum LinkType{
    POINTTOPOINT_LINK = 1,
    TRANSIT_LINK = 2,
    STUB_LINK = 3
};


class NdnLink{
    public:
    LinkType linkType;

    //用于标识路由器的链路。其取值与链路的类型有关：如果是到另一台路由器的点对点的连接，其取值为邻居路由器的router id. 如果连接到stub网络，其取值为相应子网的网络地址
    uint32_t linkID;

    //如果是到另一台路由器的点对点的连接或连接到Transit网络（TransNet），取值为与对端直接相连的接口IP地址；
    //如果是连接到Stub网络，取值为相应子网的子网掩码
    uint32_t linkData ;
    uint32_t linkDataMask;
    uint32_t linkCost;
    public:
    void decode(const char* data, int dataLength);
    std::pair<int,std::unique_ptr<char[]>>encode();
};
#endif