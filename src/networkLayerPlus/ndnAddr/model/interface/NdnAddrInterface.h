#ifndef __NDN_ADDR_INTERFACE_H_
#define __NDN_ADDR_INTERFACE_H_
#include <string>

#include "ioc.h"
#include "linkLayer/MacAddress.h"
#include "networkLayer/ip/Ipv4Address.h"
#include "networkLayerPlus/ndnAddr/model/interface/state/NdnAddrInterfaceState.h"
#include "networkLayerPlus/ndnAddr/model/interface/state/NdnAddrInterfaceStateDown.h"
#include "networkLayerPlus/ndnAddr/model/interface/state/NdnAddrInterfaceStateLeader.h"
#include "networkLayerPlus/ndnAddr/model/interface/state/NdnAddrInterfaceStateNormal.h"
#include "networkLayerPlus/ndnAddr/model/interface/state/NdnAddrInterfaceStateWaiting.h"
#include "physicalLayer/nic/NicObserverInterface.h"
class NdnAddrAssignmentProtocol;
class NdnAddrInterface {
   public:
    NdnAddrInterface(NdnAddrAssignmentProtocol* _protocol);
    std::string getName() { return name; }
    void setName(std::string _name) { name = _name; }

    int getInterfaceID() { return interfaceID; }
    void setInterfaceID(int _interfaceID) { interfaceID = _interfaceID; }

    MacAddress getMacAddress() { return macAddress; }
    void setMacAddress(MacAddress address) { macAddress = address; }

    Ipv4Address getIpv4Address() { return ipv4Addr; }
    void setIpv4Address(Ipv4Address addr) { ipv4Addr = addr; }

    Ipv4Address getIpv4Mask() { return ipv4Mask; }
    void setIpv4Mask(Ipv4Address mask) { ipv4Mask = mask; }

    NdnAddrInterfaceStateType getStateType() { return state->getState(); }
    void setState(NdnAddrInterfaceStateType stateType);

   private:
    std::string name;
    int interfaceID;
    MacAddress macAddress;
    Ipv4Address ipv4Addr;
    Ipv4Address ipv4Mask;

    NdnAddrAssignmentProtocol* protocol;

    std::shared_ptr<NdnAddrInterfaceState> state = nullptr;
};
#endif