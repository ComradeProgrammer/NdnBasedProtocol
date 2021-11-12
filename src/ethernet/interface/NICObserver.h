#ifndef __NICOBSERVER_H_
#define __NICOBSERVER_H_
enum NICEvent { NIC_UP = 0, NIC_DOWN = 1 };
class NICObserver {
   public:
    virtual void onEventHappen(int interfaceID, NICEvent event) = 0;
};

#endif