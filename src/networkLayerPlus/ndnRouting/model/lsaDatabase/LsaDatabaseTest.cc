#include <gtest/gtest.h>

// remove property qualifier
#define private public
#define protected public
// start to include here
#include "LsaDatabase.h"
using namespace std;
class LsaDatabaseTest : public ::testing::Test {
   protected:
    void SetUp() override { Ioc::IOCInit({{LOGGER_TYPE, LOGGER_FILE}, {LOGGER_FILENAME, "/dev/null"}, {PLATFORM, PLATFORM_UNIX}}); }
};
TEST_F(LsaDatabaseTest, testLsaDatabaseHash) {
    int num = rand() % 10;
    vector<shared_ptr<LsaDataPack>> lsas;
    for (int i = 0; i < num; i++) {
        auto lsa = make_shared<LsaDataPack>();
        if (rand() % 2 == 0) {
            lsa->lsType = ADJ;
        } else {
            lsa->lsType = RCH;
        }
        lsa->routerID = rand();
        lsa->seqNum = rand();
        lsa->lsAge = rand();
        lsas.push_back(lsa);
    }

    auto database1 = make_shared<LsaDatabase>();
    auto database2 = make_shared<LsaDatabase>();

    for (int i = 0; i < num; i++) {
        database1->insertLsa(lsas[i]);
        database2->insertLsa(lsas[num - i - 1]);
    }

    auto hash1 = database1->databaseHash();
    auto hash2 = database2->databaseHash();
    for (int i = 0; i < 16; i++) {
        ASSERT_EQ(hash1.first[i], hash2.first[i]);
    }
}