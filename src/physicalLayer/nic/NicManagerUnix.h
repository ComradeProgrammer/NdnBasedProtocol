#ifndef __NIC_MANAGER_UNIX_H_
#define __NIC_MANAGER_UNIX_H_
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
#include"NicManager.h"
// monitor NIC status and notify observers in observer pattern
class NicManagerUnix:public NicManager{
    // for singeleton pattern
    public: 
    NicManagerUnix()=default;
    NicManagerUnix(const NicManagerUnix&)=delete;

    public:
    
    std::unordered_map<int, std::shared_ptr<Nic>> getAllNicsInMap(bool flush=false)override;

    
    void startMonitor()override;
    
    void registerObserver(NicObserverInterface* observer, int interfaceID = -1)override;
    void deleteObserver(NicObserverInterface* observer)override;

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