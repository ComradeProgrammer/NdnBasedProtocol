#include <gtest/gtest.h>
// hack complier when test
#define private public
#define protected public
// start to include here
#include "Ipv4Address.h"
using namespace std;
class Ipv4AddressTest : public ::testing::Test {};

TEST_F(Ipv4AddressTest, testIpv4Address) {
    Ipv4Address addr1;
    addr1.setIp("192.168.0.1");
    ASSERT_EQ(addr1.toString(), "192.168.0.1");
}
TEST_F(Ipv4AddressTest, testContent) {
    Ipv4Address addr1;
    addr1.setIp("192.168.0.1");
    // this is big endian
    unsigned char tmp[4] = {192, 168, 0, 1};
    ASSERT_EQ(*((uint32_t*)(tmp)), addr1.addr);
}
TEST_F(Ipv4AddressTest,testAddMask){
    Ipv4Address addr1;
    addr1.setIp("192.168.111.1");
    Ipv4Address mask1;
    mask1.setIp("255.255.255.0");
    ASSERT_EQ(addr1.andMask(mask1).toString(),"192.168.111.0");

    Ipv4Address mask2;
    mask2.setIp("255.255.0.0");
    ASSERT_EQ(addr1.andMask(mask2).toString(),"192.168.0.0");

    Ipv4Address mask3;
    mask3.setIp("255.0.0.0");
    ASSERT_EQ(addr1.andMask(mask3).toString(),"192.0.0.0");


    Ipv4Address mask4;
    mask4.setIp("255.255.255.255");
    ASSERT_EQ(addr1.andMask(mask4).toString(),"192.168.111.1");

}
TEST_F(Ipv4AddressTest, testPrefixLength) {
    Ipv4Address mask1;
    mask1.setIp("255.255.255.0");
    ASSERT_EQ(mask1.getPrefixLength(),24);

    Ipv4Address mask2;
    mask2.setIp("255.255.0.0");
    ASSERT_EQ(mask2.getPrefixLength(),16);

    Ipv4Address mask3;
    mask3.setIp("255.0.0.0");
    ASSERT_EQ(mask3.getPrefixLength(),8);

    Ipv4Address mask4;
    mask4.setIp("255.255.255.255");
    ASSERT_EQ(mask4.getPrefixLength(),32);

}