#ifndef __NIC_OBSERVER_INTERFACE_H_
#define __NIC_OBSERVER_INTERFACE_H_
enum NICEvent { NIC_UP = 0, NIC_DOWN = 1 };
class NicObserverInterface{
public:
    virtual void onEventHappen(int interfaceID, NICEvent event) = 0;
};
#endif