#include <gtest/gtest.h>
// remove property qualifier
#define private public
#define protected public
// start to include here
#include "AddrHelloInterest.h"
using namespace std;
class AddrHelloInterestPackTest : public ::testing::Test {
   protected:
};

TEST_F(AddrHelloInterestPackTest, testEncodeAndDecode) {
    AddrHelloInterest helloInterest;
    helloInterest.routerID = rand();
    helloInterest.leader = rand();
    helloInterest.interfaceNum = rand();
    helloInterest.addr.setIp("10.0.0.1");
    helloInterest.mask.setIp("255.255.0.0");
    helloInterest.helloInterval = rand();

    auto encodePair = helloInterest.encode();

    AddrHelloInterest helloInterest2;
    helloInterest2.decode(encodePair.second.get(), encodePair.first);
    ASSERT_EQ(helloInterest2.routerID, helloInterest.routerID);
    ASSERT_EQ(helloInterest2.leader, helloInterest.leader);
    ASSERT_EQ(helloInterest2.interfaceNum, helloInterest.interfaceNum);
    ASSERT_EQ(helloInterest2.addr, helloInterest.addr);
    ASSERT_EQ(helloInterest2.mask, helloInterest.mask);
    ASSERT_EQ(helloInterest2.helloInterval, helloInterest.helloInterval);
}
