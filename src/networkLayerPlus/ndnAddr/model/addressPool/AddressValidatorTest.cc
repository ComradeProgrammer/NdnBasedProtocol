#include <gtest/gtest.h>
// remove property qualifier
#define private public
#define protected public
// start to include here
#include "AddressValidator.h"

class AddressValidatorTest : public ::testing::Test {};
TEST_F(AddressValidatorTest, testBuildAndCheck) {
    AddressValidator v;
    string data = "10.2.0.4 255.255.255.252;10.2.0.12 255.255.255.252;10.2.0.24 255.255.255.252;10.2.0.28 255.255.255.252;10.2.0.32 255.255.255.252";
    v.establishFromChainBlock(data);
    ASSERT_EQ(v.addresses.size(), 5);
    ASSERT_EQ(v.masks.size(), 5);

    bool ok1 = v.checkAddress(Ipv4Address("10.2.0.29"), Ipv4Address("255.255.255.252"));
    ASSERT_EQ(ok1, false);

    bool ok2 = v.checkAddress(Ipv4Address("192.168.0.1"), Ipv4Address("255.255.255.252"));
    ASSERT_EQ(ok2, true);
}