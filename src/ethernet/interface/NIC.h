#ifndef __NIC_H_
#define __NIC_H_
#include <arpa/inet.h>
#include <linux/if_packet.h>
#include <net/ethernet.h>
#include <net/if.h>
#include <netinet/in.h>
#include <sys/ioctl.h>
#include <sys/socket.h>

#include <cstring>
#include <string>
#include <vector>

#include "ethernet/ethernetPacket/MacAddress.h"
#include "util/log/Logger.h"
class NIC {
   public:
    NIC()=default;
    NIC(std::string _name, int _interfaceID, MacAddress address);

    // getter of name attribute
    std::string getName() { return name; }
    // get the interfaceID attribute
    int getInterfaceID() { return interfaceID; }
    // get macaddress attribute
    MacAddress getMacAddress() { return macAddress; }
    /**
     * @brief get all the NICs via reading file '/proc/net/dev' and launch ioctl
     * query. Only those NICs with valid interface index and mac address will be
     * returned.
     * @return  vector containing all NICs
     *
     */
    static std::vector<NIC> getAllInterfaces(
        std::shared_ptr<Logger> _logger = nullptr);

   private:
    std::string name;
    int interfaceID;

    MacAddress macAddress;
};
#endif