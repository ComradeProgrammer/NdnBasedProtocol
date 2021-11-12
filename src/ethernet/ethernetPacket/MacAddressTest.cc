#include <gtest/gtest.h>
// hack complier when test
#define private public
#define protected public
// start to include here
#include "MacAddress.h"
using namespace std;

class MacAddressTest : public ::testing::Test {
   protected:
    void SetUp() override {
        string macStr = "d2:d4:12:9e:00:54";
        address = new MacAddress(macStr);
    }
    void TearDown() override { delete address; }

    MacAddress* address = nullptr;
};

TEST_F(MacAddressTest, StringToBinary) {
    ASSERT_EQ(address->addr[0], 0xd2);
    ASSERT_EQ(address->addr[1], 0xd4);
    ASSERT_EQ(address->addr[2], 0x12);
    ASSERT_EQ(address->addr[3], 0x9e);
    ASSERT_EQ(address->addr[4], 0x00);
    ASSERT_EQ(address->addr[5], 0x54);
}

TEST_F(MacAddressTest, BinaryToString) { ASSERT_EQ(address->toString(), "d2:d4:12:9e:00:54"); }
TEST_F(MacAddressTest, equalTest) {
    string macStr = "d2:d4:12:ff:00:54";
    MacAddress addr1(macStr);
    MacAddress addr2(macStr);
    ASSERT_EQ(addr1, addr2);
}