#include"HelloController.h"
void HelloController::onReceiveInterest(int interfaceIndex, MacAddress sourceMac, std::shared_ptr<NdnInterest> interest){
    auto helloInfoData = interest->getApplicationParameters();
    HelloInterestPack helloInfo;
    helloInfo.decode(helloInfoData.second.get(), helloInfoData.first);
    LOGGER->INFOF("HelloController::onReceiveInterest at interface %d, source %s, content %s",interfaceIndex,sourceMac.toString().c_str(),helloInfo.toString().c_str());
}
void HelloController::onReceiveData(int interfaceIndex, MacAddress sourceMac, std::shared_ptr<NdnData> packet){

}
