#include <gtest/gtest.h>
// remove property qualifier
#define private public
#define protected public
// start to include here
#include "Flag.h"

class FlagTest : public ::testing::Test {
   protected:
};
using namespace std;

TEST_F(FlagTest, FlagTest1) {
    int argc = 6;
    char* argv[] = {
        "/test", "-a", "aaa", "-c", "-b", "3",
    };

    Flag flag;
    flag.setFlagForExistence("-c", "operandc");
    flag.setFlagForValue("-a", "description a", "defaulta");
    flag.setFlagForValue("-b", "description b", "55");
    flag.setFlagForValue("-d", "description d", "defaultd");

    string err = flag.parseFlag(argc, argv, false);
    ASSERT_EQ(err, "");
    ASSERT_EQ(flag.getStringFlag("-a"), "aaa");
    ASSERT_EQ(flag.getIntFlag("-b"), 3);
    ASSERT_EQ(flag.flagExists("-c"), true);
    ASSERT_EQ(flag.getStringFlag("-d"), "defaultd");
    ASSERT_EQ(flag.getStringFlag("-e"), "");
    ASSERT_EQ(flag.flagExists("-f"), false);
}
TEST_F(FlagTest, FlagTest2) {
    int argc = 6;
    char* argv[] = {
        "/test", "-a", "-c", "-b", "3",
    };

    Flag flag;
    flag.setFlagForExistence("-c", "operandc");
    flag.setFlagForValue("-a", "description a", "defaulta");
    flag.setFlagForValue("-b", "description b", "55");
    flag.setFlagForValue("-d", "description d", "defaultd");

    string err = flag.parseFlag(argc, argv, false);
    ASSERT_NE(err, "");
}
TEST_F(FlagTest, FlagTest3) {
    int argc = 6;
    char* argv[] = {
        "/test", "-a", "aaa", "aaa", "-c", "-b", "3",
    };

    Flag flag;
    flag.setFlagForExistence("-c", "operandc");
    flag.setFlagForValue("-a", "description a", "defaulta");
    flag.setFlagForValue("-b", "description b", "55");
    flag.setFlagForValue("-d", "description d", "defaultd");

    string err = flag.parseFlag(argc, argv, false);
    ASSERT_NE(err, "");
}
