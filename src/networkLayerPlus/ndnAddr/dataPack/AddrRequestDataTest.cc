#include <gtest/gtest.h>
// remove property qualifier
#define private public
#define protected public
// start to include here
#include "AddrRequestData.h"
using namespace std;
class AddrRequestDataTest : public ::testing::Test {
   protected:
};

TEST_F(AddrRequestDataTest, testEncodeAndDecode) {
    AddrRequestData data1;
    data1.startAddr=Ipv4Address("192.168.1.1");
    data1.mask=Ipv4Address("255.255.255.0");
    data1.nonce=rand();
    auto tmp=data1.encode();
    AddrRequestData data2;
    data2.decode(tmp.second.get(),tmp.first);
    ASSERT_EQ(data1.startAddr,data2.startAddr);
    ASSERT_EQ(data1.mask,data2.mask);
    ASSERT_EQ(data1.nonce,data2.nonce);

}