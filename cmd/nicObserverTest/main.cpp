#include <iostream>
#include<fstream>
#include "ndn/ndnPacket/NdnInterest.h"
#include "ndn/ndnProtocol/NdnProtocol.h"
#include "ndn/ndnProtocol/NdnTransmitter.h"
#include "ndnRouting/protocol/NdnRoutingProtocol.h"
#include "util/log/FileLogger.h"
#include "ethernet/interface/NICManager.h"
using namespace std;
shared_ptr<Logger>logger;
class FakeObserver: public NICObserver{
    virtual void onEventHappen(int interfaceID, NICEvent event)override{
       logger->INFOF("interface %d, event %d",interfaceID,event);
    }
};
int main(){
    logger = make_shared<FileLogger>("log.log");
    auto manager=NICManager::getNICManager(logger);
    auto tmp =  manager->getAllInterfaces();
    for (auto i = 0; i < tmp.size(); i++) {
        logger->INFO(tmp[i].toString());
    }

   
    FakeObserver* observer=new FakeObserver;
    logger->INFOF("address %x",observer);
    manager->registerObserver(observer,2);
    manager->startMonitor();
    while(1){
        sleep(1);
    }
}