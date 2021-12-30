#ifndef __NIC_MANAGER_H_
#define __NIC_MANAGER_H_
#include"Nic.h"
#include"NicObserverInterface.h"
class NicManager{
    public:
    /**
     * @brief Get the All Nics. 
     * 
     * @param flush whether to start the new syscall and get latest results or return the old results. 
     * @return map id->Nic object
     */
    virtual std::unordered_map<int, std::shared_ptr<Nic>> getAllNicsInMap(bool flush=false)=0;
    /**
     * @brief start a new thread to polling the status of all nics and call notifyObservers once differences between new results and old results are found.
     */
    virtual void startMonitor()=0;
    /**
     * @brief register an observer to monitor events on one or all interfaces. Once an event happens, all observers monitoring this interface will be notified by calling its onEventHappen() method.
     * 
     * @param observer The object which want to monitor the interface. Should have implemented NicObserverInterface interface
     * @param interfaceID id of Nic monitored. -1 if you want to monitor all interfaces
     */
    virtual void registerObserver(NicObserverInterface* observer, int interfaceID = -1)=0;
    virtual void deleteObserver(NicObserverInterface* observer)=0;
};
#endif
