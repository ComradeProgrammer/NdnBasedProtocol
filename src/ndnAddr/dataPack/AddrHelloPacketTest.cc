#include <gtest/gtest.h>
// remove property qualifier
#define private public
#define protected public
// start to include here
#include"AddrHelloPacket.h"
using namespace std;
class AddrHelloPackettest: public ::testing::Test {
};

TEST_F(AddrHelloPackettest, testEncodeAndDecode){
    AddrHelloPacket oldPacket;
    oldPacket.routerID=rand();
    oldPacket.leader=rand();
    oldPacket.interfaceNum=rand();
    oldPacket.addr.setIp("192.168.1.1");
    oldPacket.mask.setIp("255.255.255.0");
    oldPacket.helloInterval=456;

    auto res=oldPacket.encode();

    AddrHelloPacket newPacket;
    newPacket.decode(res.second.get(),res.first);
    ASSERT_EQ(oldPacket.routerID,newPacket.routerID);
    ASSERT_EQ(oldPacket.leader,newPacket.leader);
    ASSERT_EQ(oldPacket.interfaceNum,newPacket.interfaceNum);
    ASSERT_EQ(oldPacket.addr,newPacket.addr);
    ASSERT_EQ(oldPacket.mask,newPacket.mask);
    ASSERT_EQ(oldPacket.helloInterval,newPacket.helloInterval);
}

