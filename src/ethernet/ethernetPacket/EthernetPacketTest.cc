#include <gtest/gtest.h>
// hack complier when test
#define private public
#define protected public
// start to include here
#include "EthernetPacket.h"
using namespace std;
class EthernetPacketTest : public ::testing::Test {
   protected:
    EthernetHeader header;
    void SetUp() override {
        header.setDestionationMacAddress(MacAddress("a2:6b:6d:a9:ee:06"));
        header.setSourceMacAddress(MacAddress("30:9c:23:f3:2f:39"));
        header.setType(0x1234);
    }
    void TearDown() override {}
};

TEST_F(EthernetPacketTest, testCopyConstructor) {
    char data[] = "tmp";
    EthernetPacket a(header, data, 4);
    EthernetPacket c(a);

    ASSERT_EQ(0x1234, c.getHeader().getType());
    ASSERT_EQ(string(c.data), "tmp");
    ASSERT_NE(c.data, a.data);
    ASSERT_EQ(c.getPacketSize(), 18);
}

TEST_F(EthernetPacketTest, testAssignConstructor) {
    char data[] = "tmp";
    EthernetPacket a(header, data, 4);
    EthernetPacket c = a;

    ASSERT_EQ(0x1234, c.getHeader().getType());
    ASSERT_EQ(string(c.data), "tmp");
    ASSERT_NE(c.data, a.data);
    ASSERT_EQ(c.getPacketSize(), 18);
}

TEST_F(EthernetPacketTest, testRawDataConstructor) {
    char data[] = "tmp";
    EthernetPacket a(header, data, 4);
    char buffer[18];
    memcpy(buffer, (char*)&header, 14);
    memcpy(buffer + 14, data, 4);
    EthernetPacket c(buffer, 18);
    ASSERT_EQ(0x1234, c.getHeader().getType());
    ASSERT_EQ(string(c.data), "tmp");
    ASSERT_NE(c.data, a.data);

    ASSERT_EQ(c.getPacketSize(), 18);
}
