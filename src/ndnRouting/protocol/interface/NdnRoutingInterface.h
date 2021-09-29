#ifndef __NDNROUTINGINTERFACE_H_
#define __NDNROUTINGINTERFACE_H_
#include "ethernet/interface/NIC.h"
#include "ndnRouting/protocol/NdnRoutingConstant.h"
#include "ndnRouting/protocol/interface/interfaceState/NdnRoutingInterfaceState.h"
class NdnRoutingInterface {
   public:
    NdnRoutingInterface(NIC nic, std::shared_ptr<Logger> _logger = nullptr);

    // getters and setterrs
    std::string getName() { return name; }
    int getInterfaceID() { return interfaceID; }
    MacAddress getMacAddress() { return macAddress; }
    std::shared_ptr<Logger> getLogger() { return logger; }

   private:
    std::shared_ptr<Logger> logger;
    std::string name;
    int interfaceID;
    MacAddress macAddress;
    std::shared_ptr<NdnRoutingInterfaceState> state;
};
#endif