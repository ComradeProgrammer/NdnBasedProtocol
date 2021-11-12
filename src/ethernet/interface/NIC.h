#ifndef __NIC_H_
#define __NIC_H_
#include <arpa/inet.h>
#include <linux/ethtool.h>
#include <linux/if_packet.h>
#include <linux/sockios.h>
#include <net/ethernet.h>
#include <net/if.h>
#include <netinet/in.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cstring>
#include <mutex>
#include <string>
#include <unordered_map>
#include <vector>

#include "ethernet/ethernetPacket/MacAddress.h"
#include "ip/Ipv4Address.h"
#include "util/log/Logger.h"
#include "util/printable/Jsonifiable.h"
class NIC : public Jsonfiable {
   public:
    NIC() = default;
    NIC(std::string _name, int _interfaceID, MacAddress address, Ipv4Address _ipAddr, Ipv4Address _ipMask, bool linkUp);

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
    virtual nlohmann::json marshal()const override;

   private:
    std::string name;
    int interfaceID;
    MacAddress macAddress;
    Ipv4Address ipAddr;
    Ipv4Address ipMask;
    bool linkUp;
};
#endif