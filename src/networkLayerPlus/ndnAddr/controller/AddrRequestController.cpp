#include"AddrRequestController.h"
using namespace std;
void AddrRequestController::onReceiveInterest(int interfaceIndex, MacAddress sourceMac, std::shared_ptr<NdnInterest> packet){
    LOGGER->INFOF(3,"AddrRequestController::onReceiveInterest AddrRequest received %s",packet->getName().c_str());
}
void AddrRequestController::onReceiveData(int interfaceIndex, MacAddress sourceMac, std::shared_ptr<NdnData> packet){

}