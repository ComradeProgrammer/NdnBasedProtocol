#include "NdnAddrInterface.h"
using namespace std;
NdnAddrInterface::NdnAddrInterface(NdnAddrAssignmentProtocol* _protocol) {
    protocol = _protocol;
    state = make_shared<NdnAddrInterfaceStateDown>(this);
}
void NdnAddrInterface::setState(NdnAddrInterfaceStateType stateType) {
    LOGGER->INFOF(3, "Interface State changed on interface %d , from %s to %s", interfaceID, getNameForAddrInterfaceStateType(state->getState()),
                  getNameForAddrInterfaceStateType(stateType));
    switch (stateType) {
        case NdnAddrInterfaceStateType::DOWN:
            state = make_shared<NdnAddrInterfaceStateDown>(this);
            break;
        case NdnAddrInterfaceStateType::WAITING:
            state = make_shared<NdnAddrInterfaceStateWaiting>(this);
            break;
        case NdnAddrInterfaceStateType::LEADER:
            state = make_shared<NdnAddrInterfaceStateLeader>(this);
            break;
        case NdnAddrInterfaceStateType::NORMAL:
            state = make_shared<NdnAddrInterfaceStateNormal>(this);
            break;
    }
}