#include <gtest/gtest.h>
// remove property qualifier
#define private public
#define protected public
// start to include here
#include "DumbAddressPool.h"
using namespace std;
class DumbAddressPoolTest : public ::testing::Test {};
// TEST_F(DumbAddressPoolTest, testIpv4Address) {
//     DumbAddressPool pool;
//     auto res1 = pool.allocateAddress(2);
//     Ipv4Address mask;
//     mask.setIp("255.255.255.252");

//     Ipv4Address expect1;
//     expect1.setIp("192.168.0.0");
//     ASSERT_EQ(res1.first, expect1);
//     ASSERT_EQ(res1.second, mask);


//     auto res2 = pool.allocateAddress(2);
//     Ipv4Address expect2;
//     expect2.setIp("192.168.0.4");
//     ASSERT_EQ(res2.first, expect2);
//     ASSERT_EQ(res2.second, mask);

// }