#include <gtest/gtest.h>
// remove property qualifier
#define private public
#define protected public
// start to include here
#include "NdnRoutingAcl.h"
using namespace std;

class NdnRoutingAclTest : public ::testing::Test {
   protected:
};
TEST_F(NdnRoutingAclTest, test1) {
    NdnRoutingAcl acl;
    string testString = R"(
        rule 1 deny interface 1  packetName /tmp
        rule 2 permit packetName /tmp
    )";
    acl.parseString(testString);

    NdnRoutingAclData data1;
    data1.packetName = "/tmp";
    data1.interfaceIndex = 1;

    NdnRoutingAclData data2;
    data2.packetName = "/tmp";
    data2.interfaceIndex = 10;

    bool ok1 = acl.match(&data1);
    bool ok2 = acl.match(&data2);

    NdnRoutingAclData data3;
    data3.packetName = "/ttttttt";
    data3.interfaceIndex = 4564;
    bool ok3 = acl.match(&data3);

    ASSERT_EQ(ok1, false);
    ASSERT_EQ(ok2, true);
    ASSERT_EQ(ok3, true);
}
