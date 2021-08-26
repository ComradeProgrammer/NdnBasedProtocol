#include <gtest/gtest.h>
// hack complier when test
#define private public
#define protected public
#include "TlvObject.h"
using namespace std;

class TlvObjectTest : public ::testing::Test {
   protected:
};

TEST_F(TlvObjectTest, testTlvObjectUInt32) {
    uint32_t a = 100;
    TlvObject t(0, a);
    auto resPair = t.encode();
    TlvObject interpretedObject = TlvObject::decode(resPair.second);
    ASSERT_EQ(a, interpretedObject.parseUInt32());

    a = 253;
    t = TlvObject(0, a);
    resPair = t.encode();
    interpretedObject = TlvObject::decode(resPair.second);
    ASSERT_EQ(a, interpretedObject.parseUInt32());
    a = 255;
    t = TlvObject(0, a);
    resPair = t.encode();
    interpretedObject = TlvObject::decode(resPair.second);
    ASSERT_EQ(a, interpretedObject.parseUInt32());
    a = 258;
    t = TlvObject(0, a);
    resPair = t.encode();
    interpretedObject = TlvObject::decode(resPair.second);
    ASSERT_EQ(a, interpretedObject.parseUInt32());
    a = 0xfffc;
    t = TlvObject(0, a);
    resPair = t.encode();
    interpretedObject = TlvObject::decode(resPair.second);
    ASSERT_EQ(a, interpretedObject.parseUInt32());
    a = 0xfffffffc;
    t = TlvObject(0, a);
    resPair = t.encode();
    interpretedObject = TlvObject::decode(resPair.second);
    ASSERT_EQ(a, interpretedObject.parseUInt32());
}

TEST_F(TlvObjectTest, testTlvObjectType) {
    vector<uint64_t> types = {1,   252,    253,    254,        255,
                              256, 0xfffe, 0xffff, 0xfffffffe, 0xffffffff};
    for (int i = 0; i < int(types.size()); i++) {
        auto a = TlvObject(uint64_t(types[i]), uint32_t(0));
        auto resPair = a.encode();
        TlvObject interpretedObject = TlvObject::decode(resPair.second);
        ASSERT_EQ(interpretedObject.getType(), types[i]);
    }
}

TEST_F(TlvObjectTest, testTlvObjectString) {
    vector<string> strings = {"shit1", "shhi2", "", "afgadgfasd"};
    for (string i : strings) {
        auto a = TlvObject(0, i);
        auto resPair = a.encode();
        TlvObject interpretedObject = TlvObject::decode(resPair.second);
        ASSERT_EQ(interpretedObject.parseString(), i);
    }
}
TEST_F(TlvObjectTest, testTlvObjectUint16) {
    vector<uint64_t> types = {1, 252, 253, 254, 255, 256, 0xfffe, 0xffff};
    for (int i = 0; i < int(types.size()); i++) {
        auto a = TlvObject(1, uint16_t(types[i]));
        auto resPair = a.encode();
        TlvObject interpretedObject = TlvObject::decode(resPair.second);
        ASSERT_EQ(interpretedObject.parseUInt16(), types[i]);
    }
}

TEST_F(TlvObjectTest, testTlvObjectUint8) {
    vector<uint64_t> types = {1, 252, 253, 254, 255};
    for (int i = 0; i < int(types.size()); i++) {
        auto a = TlvObject(1, uint8_t(types[i]));
        auto resPair = a.encode();
        TlvObject interpretedObject = TlvObject::decode(resPair.second);
        ASSERT_EQ(interpretedObject.parseUInt8(), types[i]);
    }
}
