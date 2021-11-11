#ifndef __NICMANAGER_H_
#define __NICMANAGER_H_
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
#include <thread>
#include <cstring>
#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>
#include <vector>

#include "ethernet/ethernetPacket/MacAddress.h"
#include "ip/Ipv4Address.h"
#include "util/log/Logger.h"
#include "util/printable/Jsonifiable.h"
#include "NICObserver.h"
#include "NIC.h"
#define POLLING_INTERVAL 1000
//unit: ms

// singleton pattern; monitor NIC status and notify observers in observer pattern
class NICManager {
    // for singeleton pattern
   public:
    static std::shared_ptr<NICManager> getNICManager(std::shared_ptr<Logger> logger = nullptr);

   private:
    static std::shared_ptr<NICManager> nicManager;
    static std::mutex staticLock;
    NICManager(std::shared_ptr<Logger> logger = nullptr);

   public:
    /**
     * @brief get all the NICsfrom cache. Only those NICs with valid interface
     * index and mac address will be returned. NIC cache will not be flushed
     * under such circumstance
     * @return  vector containing all NICs
     */
    std::vector<NIC> getAllInterfaces();

     /**
     * @brief get the NICs map from cache.The key of the map is interfaceID.
     * Only those NICs with valid interface index and mac address will be
     * returned. NIC cache will not be flushed under such circumstance.
     *
     * If the prefix set to a non-empty string, only these NIC with a name
     * starting with the specified prefix will be returned.
     * @return  vector containing all NICs
     */
    std::unordered_map<int, NIC> getNICMap();

    /**
     * @brief Set the Prefix object. default value of prefix is "". if perfix is
     * "", all nics will be given out. if not, only those nics with a name
     * starts with the prefix will be returned when query nics with any methods.
     */
    void setPrefix(std::string _prefix);


    void startMonitor();
    void registerObserver(NICObserver* observer, int interfaceID=-1);
    void deleteObserver(NICObserver* observer);

    private:
    std::mutex lock;
    std::shared_ptr<Logger>logger;
    std::string prefix="";
    std::vector<NIC> nicVectorCache;
    std::unordered_map<int, NIC> nicMapCache;
    std::unordered_map<int,std::vector<NICObserver*>>observers;
    void notifyObservers(int interfaceIndex,NICEvent event);
    

    /**
     * @brief check whether a nic has cable plugged in. need to require lock
     * 
     * @param s name of the NIC
     */
    bool checkLinkUpByName(std::string s);
    /**
     * @brief refresh the nicVectorCache nicMapCache. need to require lock before calling
    */
    void flush();

};
#endif