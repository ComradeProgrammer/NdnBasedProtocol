#include <gtest/gtest.h>
// hack complier when test
#define private public
#define protected public
// start to include here
#include <net/ethernet.h>

#include "EthernetHeader.h"
using namespace std;

class EthernetHeaderTest : public ::testing::Test {
   protected:
    EthernetHeader header;
    void SetUp() override {
        header.setDestionationMacAddress(MacAddress("a2:6b:6d:a9:ee:06"));
        header.setSourceMacAddress(MacAddress("30:9c:23:f3:2f:39"));
        header.setType(0x1234);
    }
};

TEST_F(EthernetHeaderTest, testByteOrder) {
    ASSERT_EQ(header.type, htons(0x1234));
}

TEST_F(EthernetHeaderTest, testReinterpret) {
    ASSERT_EQ(sizeof(header), 14);
    ether_header* tmp = reinterpret_cast<ether_header*>(&header);

    int destExpect[6] = {0xa2, 0x6b, 0x6d, 0xa9, 0xee, 0x06};
    int srcExpect[6] = {0x30, 0x9c, 0x23, 0xf3, 0x2f, 0x39};
    for (int i = 0; i < 6; i++) {
        ASSERT_EQ(tmp->ether_dhost[i], destExpect[i]);
    }
    for (int i = 0; i < 6; i++) {
        ASSERT_EQ(tmp->ether_shost[i], srcExpect[i]);
    }
}

TEST_F(EthernetHeaderTest, testRawDataConstructor) {
    EthernetHeader newHeader((char*)&header);
    ASSERT_EQ(newHeader.destination.toString(), "a2:6b:6d:a9:ee:06");
    ASSERT_EQ(newHeader.source.toString(), "30:9c:23:f3:2f:39");
    ASSERT_EQ(newHeader.getType(), 0x1234);
}