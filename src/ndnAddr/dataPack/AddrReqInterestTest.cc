#include <gtest/gtest.h>
// remove property qualifier
#define private public
#define protected public
// start to include here
#include"AddrReqInterest.h"
using namespace std;
class AddrReqInterestTest: public ::testing::Test{
};

TEST_F(AddrReqInterestTest,testEncodeAndDecode){
    for(int i=0;i<5;i++){
        AddrReqInterest oldPacket;
        oldPacket.num=rand();
        auto res=oldPacket.encode();
        AddrReqInterest newPacket;
        newPacket.decode(res.second.get(),res.first);
        ASSERT_EQ(oldPacket.num,newPacket.num);
    }
}