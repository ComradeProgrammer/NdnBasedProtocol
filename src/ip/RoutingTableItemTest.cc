#include <gtest/gtest.h>
// remove property qualifier
#define private public
#define protected public
// start to include here
#include "RoutingTableItem.h"
using namespace std;
class RoutingTableItemTest : public ::testing::Test {};
TEST_F(RoutingTableItemTest, RoutingTableItemTest1) {
    string test = "1.1.1.1         192.168.50.1    255.255.255.0         UG    100    0        0 enp0s31f6";
    RoutingTableItem item(test);
    Ipv4Address realDest, realNextHop, realMask;
    realDest.setIp("1.1.1.1");
    realNextHop.setIp("192.168.50.1");
    realMask.setIp("255.255.255.0");
    ASSERT_EQ(realDest, item.getDestination());
    ASSERT_EQ(realNextHop, item.getNextHop());
    ASSERT_EQ(realMask, item.getMask());
}