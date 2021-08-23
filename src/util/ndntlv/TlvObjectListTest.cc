#include <gtest/gtest.h>
// hack complier when test
#define private public
#define protected public
#include "TlvObject.h"
#include "TlvObjectList.h"
using namespace std;
class TlvObjectListTest : public ::testing::Test {
   protected:
};
TEST_F(TlvObjectListTest, testTlvObjectList1Simple) {
    TlvObject obj1 = TlvObject(1, uint16_t(1));
    TlvObject obj2 = TlvObject(2, uint32_t(1222));
    TlvObject obj3 = TlvObject(3, "1000");

    TlvObjectList list;
    list.insertObject(obj1);
    list.insertObject(obj2);
    list.insertObject(obj3);
    auto res = list.encode();

    TlvObjectList newList(res.second, res.first);
    auto decodeRes = newList.decode();
    ASSERT_EQ(decodeRes.size(), 3);
    ASSERT_EQ(decodeRes[0]->getType(), 1);
    ASSERT_EQ(decodeRes[0]->parseUInt16(), 1);

    ASSERT_EQ(decodeRes[1]->getType(), 2);
    ASSERT_EQ(decodeRes[1]->parseUInt32(), 1222);
    ASSERT_EQ(decodeRes[2]->getType(), 3);
    ASSERT_EQ(decodeRes[2]->parseString(), "1000");
}

TEST_F(TlvObjectListTest, testTlvObjectList1Nested) {
    TlvObject obj1 = TlvObject(1, uint16_t(1));
    TlvObject obj2 = TlvObject(2, uint32_t(1222));
    TlvObject obj3 = TlvObject(3, "1000");

    TlvObjectList innerList;
    innerList.insertObject(obj1);
    innerList.insertObject(obj2);
    innerList.insertObject(obj3);
    auto innerData = innerList.encode();
    TlvObject objInner = TlvObject(TLV_LIST, innerData.first, innerData.second);

    TlvObjectList outerList;
    outerList.insertObject(objInner);
    outerList.insertObject(666);

    auto outerRes = outerList.encode();

    auto newOuterList = TlvObjectList(outerRes.second, outerRes.first);
    auto newOuterRes = newOuterList.decode();

    ASSERT_EQ(newOuterRes[1]->getType(), 666);
    ASSERT_EQ(newOuterRes[1]->getLength(), 0);
    auto newInnerList =
        TlvObjectList(newOuterRes[0]->getData(), newOuterRes[0]->getLength());
    auto decodeRes = newInnerList.decode();

    ASSERT_EQ(decodeRes.size(), 3);
    ASSERT_EQ(decodeRes[0]->getType(), 1);
    ASSERT_EQ(decodeRes[0]->parseUInt16(), 1);

    ASSERT_EQ(decodeRes[1]->getType(), 2);
    ASSERT_EQ(decodeRes[1]->parseUInt32(), 1222);
    ASSERT_EQ(decodeRes[2]->getType(), 3);
    ASSERT_EQ(decodeRes[2]->parseString(), "1000");
}
