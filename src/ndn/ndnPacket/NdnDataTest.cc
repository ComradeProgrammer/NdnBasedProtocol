#include <gtest/gtest.h>
// hack complier when test
#define private public
#define protected public
#include "NdnData.h"
using namespace std;

class NdnDataTest : public ::testing::Test {
   protected:
};
TEST_F(NdnDataTest, testNdnData) {
    NdnData data;
    data.setName("test");
    data.setContent(11, "Helloworld");
    auto tmp = data.encode();

    auto newData = NdnData::decode(tmp.second.get());
    ASSERT_NE(newData, nullptr);
    ASSERT_EQ(newData->getName(), "test");
    ASSERT_EQ(string(newData->getContent().second.get()), "Helloworld");
}