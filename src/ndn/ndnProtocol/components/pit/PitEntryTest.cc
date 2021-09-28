#include <gtest/gtest.h>
// hack complier when test
#define private public
#define protected public
#include "PitEntry.h"
using namespace std;
class PitEntryTest : public ::testing::Test {
   protected:
};

TEST_F(PitEntryTest, testPitEntry) {
    auto ptr = make_shared<PitEntry>("test");
    ASSERT_EQ(ptr->getName(), "test");
    ASSERT_EQ(ptr->isPending(), false);

    ptr->addInputRecord(1, 100);
    ptr->addInputRecord(1, 101);
    ptr->addInputRecord(2, 200);
    ptr->addInputRecord(2, 201);

    ASSERT_EQ(ptr->isPending(), true);
    ASSERT_EQ(ptr->inRecords.size(), 2);

    ASSERT_EQ(ptr->isLoopingInterest(1, 100), false);
    ASSERT_EQ(ptr->isLoopingInterest(2, 101), true);
    ASSERT_EQ(ptr->isLoopingInterest(1, 103), false);
    ASSERT_EQ(ptr->isLoopingInterest(3, 101), true);
    ASSERT_EQ(ptr->isLoopingInterest(4, 401), false);
}