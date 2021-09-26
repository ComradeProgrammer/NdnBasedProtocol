#include <gtest/gtest.h>
// hack complier when test
#define private public
#define protected public
// start to include here
#include"DDInterestPack.h"
using namespace std;
class DDInterestPackTest : public ::testing::Test {
   protected:
};

TEST_F(DDInterestPackTest,testEncodeAndDecode){
    DDInterestPack old;
    old.neighbor=4556;
    auto resPair=old.encode();
    DDInterestPack newPack;
    newPack.decode(resPair.second.get(),resPair.first);
    ASSERT_EQ(old.neighbor,newPack.neighbor);
}