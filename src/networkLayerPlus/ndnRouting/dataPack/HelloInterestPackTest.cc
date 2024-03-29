#include <gtest/gtest.h>
// remove property qualifier
#define private public
#define protected public
// start to include here
#include "HelloInterestPack.h"
#include "util/signature/Md5RsaSignatureFactory.h"
#include "util/traceback/traceback.h"

using namespace std;
#include "util/declaration.h"

class HelloInterestPackTest : public ::testing::Test {
   protected:
};

TEST_F(HelloInterestPackTest, testEncodeAndDecode) {
    HelloInterestPack packet;
    packet.routerId = 123;
    packet.interfaceIP.setIp("192.168.0.1");
    packet.networkMask.setIp("255.255.255.0");
    packet.helloInterval = 10;
    packet.routerDeadInterval = 20;
    for (int i = 0; i < 5; i++) {
        Ipv4Address tmp;
        tmp.setIp("192.168.0." + to_string(i));
        packet.neighbor.push_back(tmp);
    }
    for (int i = 0; i < 16; i++) {
        packet.databaseHash[i] = rand() % 0xff;
    }

    auto resPair = packet.encode();

    HelloInterestPack packet2;
    packet2.decode(resPair.second.get(), resPair.first);
    ASSERT_EQ(packet2.routerId, packet.routerId);
    ASSERT_EQ(packet2.interfaceIP, packet.interfaceIP);
    ASSERT_EQ(packet2.networkMask, packet.networkMask);
    ASSERT_EQ(packet2.helloInterval, packet.helloInterval);
    ASSERT_EQ(packet2.routerDeadInterval, packet.routerDeadInterval);
    ASSERT_EQ(packet2.neighbor.size(), packet.neighbor.size());
    for (int i = 0; i < 5; i++) {
        ASSERT_EQ(packet.neighbor[i], packet2.neighbor[i]);
    }
    for (int i = 0; i < 16; i++) {
        ASSERT_EQ(packet.databaseHash[i], packet2.databaseHash[i]);
    }
}
TEST_F(HelloInterestPackTest, testEncodeAndDecode2) {
    auto keyPair = RsaCipher::generateRsaKeyPair(1024);

    HelloInterestPack packet;
    packet.routerId = 123;
    packet.interfaceIP.setIp("192.168.0.1");
    packet.networkMask.setIp("255.255.255.0");
    packet.helloInterval = 10;
    packet.routerDeadInterval = 20;
    for (int i = 0; i < 5; i++) {
        Ipv4Address tmp;
        tmp.setIp("192.168.0." + to_string(i));
        packet.neighbor.push_back(tmp);
    }
    for (int i = 0; i < 16; i++) {
        packet.databaseHash[i] = rand() % 0xff;
    }
    packet.publicKey = new char[PUBLIC_KEY_LENGTH];
    memcpy(packet.publicKey, keyPair.first.c_str(), PUBLIC_KEY_LENGTH);
    packet.signSignature(keyPair.second);
    auto resPair = packet.encode();

    HelloInterestPack packet2;
    packet2.decode(resPair.second.get(), resPair.first);
    ASSERT_EQ(packet2.routerId, packet.routerId);
    ASSERT_EQ(packet2.interfaceIP, packet.interfaceIP);
    ASSERT_EQ(packet2.networkMask, packet.networkMask);
    ASSERT_EQ(packet2.helloInterval, packet.helloInterval);
    ASSERT_EQ(packet2.routerDeadInterval, packet.routerDeadInterval);
    for (int i = 0; i < 5; i++) {
        ASSERT_EQ(packet.neighbor[i], packet2.neighbor[i]);
    }
    for (int i = 0; i < 16; i++) {
        ASSERT_EQ(packet.databaseHash[i], packet2.databaseHash[i]);
    }
    bool ok = packet2.verifySignature();
    ASSERT_TRUE(ok);
}

TEST_F(HelloInterestPackTest, testEncodeAndDecode3) {
    initSignalTraceback([](string traceback) { cout << traceback; });
    auto keyPair = RsaCipher::generateRsaKeyPair(1024);

    HelloInterestPack packet;
    packet.routerId = 123;
    packet.interfaceIP.setIp("192.168.0.1");
    packet.networkMask.setIp("255.255.255.0");
    packet.helloInterval = 10;
    packet.routerDeadInterval = 20;
    for (int i = 0; i < 5; i++) {
        Ipv4Address tmp;
        tmp.setIp("192.168.0." + to_string(i));
        packet.neighbor.push_back(tmp);
    }
    for (int i = 0; i < 16; i++) {
        packet.databaseHash[i] = rand() % 0xff;
    }
    packet.publicKey = new char[PUBLIC_KEY_LENGTH];
    memcpy(packet.publicKey, keyPair.first.c_str(), PUBLIC_KEY_LENGTH);
    packet.signSignature(keyPair.second);
    auto resPair = packet.encode();

    HelloInterestPack packet2;
    packet2.decode(resPair.second.get(), resPair.first);
    ASSERT_EQ(packet2.routerId, packet.routerId);
    ASSERT_EQ(packet2.interfaceIP, packet.interfaceIP);
    ASSERT_EQ(packet2.networkMask, packet.networkMask);
    ASSERT_EQ(packet2.helloInterval, packet.helloInterval);
    ASSERT_EQ(packet2.routerDeadInterval, packet.routerDeadInterval);
    for (int i = 0; i < 5; i++) {
        ASSERT_EQ(packet.neighbor[i], packet2.neighbor[i]);
    }
    for (int i = 0; i < 16; i++) {
        ASSERT_EQ(packet.databaseHash[i], packet2.databaseHash[i]);
    }
    bool ok = packet2.verifySignature(keyPair.first);
    ASSERT_TRUE(ok);
}
