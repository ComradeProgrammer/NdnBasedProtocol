/*#include <gtest/gtest.h>
// hack complier when test
#define private public
#define protected public
// start to include here
#include <pthread.h>
#include <semaphore.h>

#include "RawSocket.h"
using namespace std;

static sem_t semaphore;
static shared_ptr<EthernetPacket> packet;
class RawSocketTest : public ::testing::Test {
   protected:
    void SetUp() override {
        sem_init(&semaphore, 0, 0);
        pthread_t p1, p2;
    }
    void TearDown() override {}
};

void* TestRawsocketReceiveThread(void*) {
    RawSocket sock;

    auto p = sock.receivePacket(0xffff);
    packet = p.second;
    sem_post(&semaphore);
    return nullptr;
}

void* TestRawsocketSendThread(void*) {
    RawSocket sock;
    char data[] = "Helloworld!";
    shared_ptr<EthernetPacket> packet = make_shared<EthernetPacket>(
        MacAddress("ff:ff:ff:ff:ff:ff"),MacAddress("fa:33:42:5c:9e:23"),
        0xffff, data, 12);
    sock.sendPacket(1, packet);  // maybe this index should be modified depend
                                 // on where this code is running
    return nullptr;
}

TEST_F(RawSocketTest, testSendAndReceive) {
    pthread_t p1, p2;
    int ret = pthread_create(&p1, nullptr, TestRawsocketReceiveThread, nullptr);
    ASSERT_EQ(ret, 0);
    int ret2 = pthread_create(&p2, nullptr, TestRawsocketSendThread, nullptr);
    ASSERT_EQ(ret2, 0);
    sem_wait(&semaphore);
    ASSERT_NE(packet, nullptr);
    ASSERT_EQ(string(packet->getData()), string("Helloworld!"));
}*/