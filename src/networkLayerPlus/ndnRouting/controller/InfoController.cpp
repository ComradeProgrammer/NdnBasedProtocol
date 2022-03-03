#include "InfoController.h"
using namespace std;
void InfoController::onReceiveInterest(int interfaceIndex, MacAddress sourceMac, std::shared_ptr<NdnInterest> packet) {
    // decide whether we need to send lsa interest.

    // 1.resolve the lsa type, source and sequence number
    //  /routing/broadcast/INFO/TYPE/routerID/seqNum
    auto splits = split(packet->getName(), "/");
    if (splits.size() != 6) {
        LOGGER->ERRORF("invalid info interest name %s", packet->getName().c_str());
        return;
    }
    LinkStateType lsType;
    if (splits[4] == "ADJ") {
        lsType = LinkStateType::ADJ;
    } else {
        lsType = LinkStateType::RCH;
    }


    
}

void InfoController::onReceiveData(int interfaceIndex, MacAddress sourceMac, std::shared_ptr<NdnData> packet) {}