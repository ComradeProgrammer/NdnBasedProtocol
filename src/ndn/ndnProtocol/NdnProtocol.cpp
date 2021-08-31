#include"NdnProtocol.h"
using namespace std;
NdnProtocol::NdnProtocol(shared_ptr<Logger> log = nullptr){
    logger = Logger::getDefaultLoggerIfNull(log);
}
void NdnProtocol::onReceiveInterest(int interfaceIndex, MacAddress sourceMac,std::shared_ptr<NdnInterest>interest){
    logger->INFOF("Entering NdnProtocol::onReceiveInterest, from interface %d, macaddress %s",interfaceIndex, sourceMac.toString().c_str());
}