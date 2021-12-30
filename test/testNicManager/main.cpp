#include<iostream>
#include"physicalLayer/nic/NicManager.h"
#include"ioc.h"
#include <unistd.h> 
using namespace std;

class FakeNicObserver: public NicObserverInterface{
    virtual void onEventHappen(int interfaceID, NICEvent event){
        LOGGER->INFOF("interface %d event %d",interfaceID,event);
    }
};


int main(int argc, char* argv[]){
    string name(argv[1]);
    Ioc::IOCInit({
        {LOGGER_TYPE,LOGGER_FILE},
        {LOGGER_FILENAME,name+".log"},
        {PLATFORM,PLATFORM_UNIX},
    });
    auto manager=IOC->getNicManager();
    FakeNicObserver observer;
    manager->registerObserver(&observer);

    auto tmp = IOC->getNicManager()->getAllNicsInMap(true);
    for (auto i: tmp) {
        LOGGER->INFO(i.second->toString());
    }

    manager->startMonitor();
    while(1){
        sleep(1);
    }

}