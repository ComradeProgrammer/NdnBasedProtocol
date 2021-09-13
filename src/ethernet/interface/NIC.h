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
#include <mutex>
#include <string>
#include <unordered_map>
#include <vector>

#include "ethernet/ethernetPacket/MacAddress.h"
#include "util/log/Logger.h"
class NIC {
   public:
    NIC() = default;
    NIC(std::string _name, int _interfaceID, MacAddress address);

    // getter of name attribute
    std::string getName() { return name; }
    // get the interfaceID attribute
    int getInterfaceID() { return interfaceID; }
    // get macaddress attribute
    MacAddress getMacAddress() { return macAddress; }

   public:
    /**
     * @brief get all the NICsfrom cache. Only those NICs with valid interface
     * index and mac address will be returned. NIC cache will not be flushed
     * under such circumstance
     * @return  vector containing all NICs
     */
    static std::vector<NIC> getAllInterfaces(
        std::shared_ptr<Logger> _logger = nullptr);
    /**
     * @brief get the NICs map from cache.The key of the map is interfaceID.
     * Only those NICs with valid interface index and mac address will be
     * returned. NIC cache will not be flushed under such circumstance.
     * 
     * If the prefix set to a non-empty string, only these NIC with a name starting with the specified prefix will be returned.
     * @return  vector containing all NICs
     */
    static std::unordered_map<int, NIC> getNICMap(
        std::shared_ptr<Logger> _logger = nullptr);
    /**
     * @brief  reload all the NICs information cache via reading file
     * '/proc/net/dev' and launching ioctl query.
     *
     */
    static void reloadCache(std::shared_ptr<Logger> _logger = nullptr);

    static void setPrefix(std::string _prefix);
   private:
    static std::mutex lock;
    static std::vector<NIC> nicVectorCache;
    static std::unordered_map<int, NIC> nicMapCache;
    static std::string prefix;
    /**
     * @brief private function which actually execute the query and reload the
     * cache. lock must be attained before calling this function.
     *
     */
    static void flush(std::shared_ptr<Logger> _logger = nullptr);

   private:
    std::string name;
    int interfaceID;
    MacAddress macAddress;
};
#endif