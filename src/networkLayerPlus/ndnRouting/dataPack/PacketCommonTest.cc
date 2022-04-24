#include <gtest/gtest.h>
// remove property qualifier
#define private public
#define protected public
// start to include here
#include "PacketCommon.h"
using namespace std;
TEST(PacketCommon, test1) {
    for (int i = 0; i < 10; i++) {
        uint64_t tmp = (uint64_t)rand() * (uint64_t)rand();
        stringstream ss;
        ss << tmp;
        ASSERT_EQ(atoRID(ss.str()), tmp);
    }
}

// 13665665275780622204