#include <gtest/gtest.h>
// remove property qualifier
#define private public
#define protected public
// start to include here
#include "util.h"
using namespace std;

class UtilTest : public ::testing::Test {
   protected:
};

TEST_F(UtilTest, testntoh) {
    uint8_t tmp = (uint8_t)(rand() % 0xff);
    ASSERT_EQ(tmp, ntoh(tmp));
    ASSERT_EQ(tmp, hton(tmp));

    uint16_t tmp2 = (uint16_t)(rand() % 0xffff);
    ASSERT_EQ(ntohs(tmp2), ntoh(tmp2));
    ASSERT_EQ(htons(tmp2), hton(tmp2));

    uint32_t tmp3 = (uint32_t)(rand());
    ASSERT_EQ(ntohl(tmp3), ntoh(tmp3));
    ASSERT_EQ(htonl(tmp3), hton(tmp3));

    uint64_t tmp4 = (uint64_t)((((uint64_t)rand()) << 32) | rand());
    ASSERT_EQ(ntohll(tmp4), ntoh(tmp4));
    ASSERT_EQ(htonll(tmp4), hton(tmp4));

    __uint128_t tmp5 = (__uint128_t)((((__uint128_t)rand()) << 96) | (((__uint128_t)rand()) << 64) | (((__uint128_t)rand()) << 32) | rand());
    ASSERT_EQ(ntohu128(tmp5), ntoh(tmp5));
    ASSERT_EQ(htonu128(tmp5), hton(tmp5));
}
