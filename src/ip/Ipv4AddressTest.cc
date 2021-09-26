#include <gtest/gtest.h>
// hack complier when test
#define private public
#define protected public
// start to include here
#include"Ipv4Address.h"
using namespace std;
class Ipv4AddressTest : public ::testing::Test{

};

TEST_F(Ipv4AddressTest , testIpv4Address) {
    Ipv4Address addr1;
    addr1.setIp("192.168.0.1");
    ASSERT_EQ(addr1.toString(),"192.168.0.1");
}
TEST_F(Ipv4AddressTest , testContent) {
    Ipv4Address addr1;
    addr1.setIp("192.168.0.1");
    //this is big endian
    unsigned char tmp[4]={192,168,0,1};
    ASSERT_EQ(*((uint32_t*)(tmp)),addr1.addr);
}