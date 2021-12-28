#ifndef __NIC_MANAGER_H_
#define __NIC_MANAGER_H_
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
#include<fstream>
#include <cstring>
#include <memory>
#include <mutex>
#include <string>
#include <thread>
#include <unordered_map>
#include <vector>

#include"Nic.h"
#include"NicObserverInterface.h"
#include"util/declaration.h"
// singleton pattern; monitor NIC status and notify observers in observer pattern
class NicManager{
    // for singeleton pattern
    public: 
    static std::shared_ptr<NicManager>getNicManager();
    private:
    static std::shared_ptr<NicManager> nicManager;
    static std::mutex staticLock;
    NicManager()=default;
    NicManager(const NicManager&)=delete;

    public:
    /**
     * @brief Get the All Nics. 
     * 
     * @param flush whether to start the new syscall and get latest results or return the old results. 
     * @return map id->Nic object
     */
    std::unordered_map<int, std::shared_ptr<Nic>> getAllNicsInMap(bool flush=false);

    /**
     * @brief start a new thread to polling the status of all nics and call notifyObservers once differences between new results and old results are found.
     */
    void startMonitor();
    /**
     * @brief register an observer to monitor events on one or all interfaces. Once an event happens, all observers monitoring this interface will be notified by calling its onEventHappen() method.
     * 
     * @param observer The object which want to monitor the interface. Should have implemented NicObserverInterface interface
     * @param interfaceID id of Nic monitored. -1 if you want to monitor all interfaces
     */
    void registerObserver(NicObserverInterface* observer, int interfaceID = -1);
    void deleteObserver(NicObserverInterface* observer);

    private:
    std::mutex lock;
    std::unordered_map<int, std::shared_ptr<Nic>> nicMapCache;
    std::unordered_map<int, std::vector<NicObserverInterface*>> observers;
    /**
     * @brief notify all related observers. (may use multiple threads in the future). Lock should have been acquired
     * 
     */
    void notifyObservers(int interfaceIndex, NICEvent event);

    /**
     * @brief check whether a nic has cable plugged in. need to require lock
     * @param s name of the NIC
     */
    bool checkLinkUpByName(std::string s);
    /**
     * @brief refresh the nicMapCache by execute a syscall at once. call notifyObservers once differences between new results and old results are found. Need to require lock BEFORE calling
     */
    void flush();





};
#endif