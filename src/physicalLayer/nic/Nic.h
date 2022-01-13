#ifndef __NIC_H_
#define __NIC_H_
#include "linkLayer/MacAddress.h"
#include "networkLayer/ip/Ipv4Address.h"
#include "util/printable/Jsonifiable.h"
class Nic : public Jsonfiable {
   public:
    Nic() = default;
    Nic(std::string _name, int _interfaceID, MacAddress address, Ipv4Address _ipAddr, Ipv4Address _ipMask, bool linkUp);

    // getter of name attribute
    std::string getName() { return name; }
    // get the interfaceID attribute
    int getInterfaceID() { return interfaceID; }
    // get macaddress attribute
    MacAddress getMacAddress() { return macAddress; }
    // getter of linkUp
    bool getLinkUp() { return linkUp; }

    Ipv4Address getIpv4Address() { return ipAddr; }
    Ipv4Address getIpv4Mask() { return ipMask; }

    virtual nlohmann::json marshal() const override;

   private:
    std::string name;
    int interfaceID;
    MacAddress macAddress;
    Ipv4Address ipAddr;
    Ipv4Address ipMask;
    bool linkUp;
};

#endif