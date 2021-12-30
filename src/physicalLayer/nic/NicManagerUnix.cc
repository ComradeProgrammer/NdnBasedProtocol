#include <gtest/gtest.h>
// remove property qualifier
#define private public
#define protected public
// start to include here
#include "NicManagerUnix.h"

class TestObserver : public NicObserverInterface {
   public:
    int countUP = 0;
    int countDOWN = 0;
    virtual void onEventHappen(int interfaceID, NICEvent event) override {
        if (event == NICEvent::NIC_UP) {
            countUP++;
        } else if (event == NICEvent::NIC_DOWN) {
            countDOWN++;
        }
    }
};

class NICManagerTest : public ::testing::Test {};

TEST_F(NICManagerTest, testObserve) {
    auto manager = NicManagerUnix::getNicManager();
    TestObserver observer1;
    TestObserver observer2;
    TestObserver observer3;
    TestObserver observer4;
    manager->registerObserver(&observer1, 1);
    manager->registerObserver(&observer2, 1);
    manager->registerObserver(&observer3, 2);
    manager->registerObserver(&observer4, -1);

    manager->lock.lock();
    manager->notifyObservers(1, NIC_UP);
    manager->lock.unlock();
    ASSERT_EQ(observer1.countUP, 1);
    ASSERT_EQ(observer2.countUP, 1);
    ASSERT_EQ(observer1.countDOWN, 0);
    ASSERT_EQ(observer2.countDOWN, 0);
    ASSERT_EQ(observer4.countUP, 1);
    ASSERT_EQ(observer4.countDOWN, 0);
    ASSERT_EQ(observer3.countUP, 0);
    ASSERT_EQ(observer3.countDOWN, 0);

    manager->lock.lock();
    manager->notifyObservers(2, NIC_UP);
    manager->lock.unlock();
    ASSERT_EQ(observer1.countUP, 1);
    ASSERT_EQ(observer2.countUP, 1);
    ASSERT_EQ(observer1.countDOWN, 0);
    ASSERT_EQ(observer2.countDOWN, 0);
    ASSERT_EQ(observer4.countUP, 2);
    ASSERT_EQ(observer4.countDOWN, 0);
    ASSERT_EQ(observer3.countUP, 1);
    ASSERT_EQ(observer3.countDOWN, 0);

    manager->deleteObserver(&observer2);

    manager->lock.lock();
    manager->notifyObservers(1, NIC_DOWN);
    manager->lock.unlock();

    ASSERT_EQ(observer1.countUP, 1);
    ASSERT_EQ(observer2.countUP, 1);
    ASSERT_EQ(observer1.countDOWN, 1);
    ASSERT_EQ(observer2.countDOWN, 0);
    ASSERT_EQ(observer4.countUP, 2);
    ASSERT_EQ(observer4.countDOWN, 1);
    ASSERT_EQ(observer3.countUP, 1);
    ASSERT_EQ(observer3.countDOWN, 0);
}