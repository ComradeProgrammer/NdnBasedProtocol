#include <gtest/gtest.h>
// remove property qualifier
#define private public
#define protected public
#include "Pit.h"
using namespace std;
class PitTest : public ::testing::Test {
   protected:
};

TEST_F(PitTest, testPit1) {
    Pit p;
    auto ptr1 = p.getPitEntry("test");
    ASSERT_EQ(p.entries.size(), 1);
    auto ptr2 = p.getPitEntry("test");
    ASSERT_EQ(p.entries.size(), 1);
    ASSERT_EQ(ptr1, ptr2);
    auto ptr3 = p.getPitEntry("test2");
    ASSERT_EQ(p.entries.size(), 2);

    auto ptr4 = p.findPitEntry("test3");
    ASSERT_EQ(ptr4, nullptr);
}

TEST_F(PitTest, testPit2) {
    Pit p;
    auto ptr1 = p.getPitEntry("test");
    ASSERT_EQ(p.entries.size(), 1);

    p.deletePitEntry("test1");
    ASSERT_EQ(p.entries.size(), 1);
    p.deletePitEntry("test");
    p.deletePitEntry("test");
    p.deletePitEntry("test");
    p.deletePitEntry("test");
    p.deletePitEntry("test");
    ASSERT_EQ(p.entries.size(), 0);
}