#ifndef __NICOBSERVER_H_
#define __NICOBSERVER_H_
enum NICEvent{
    UP=0,
    DOWN=1
};
class NICObserver{
    public:
    virtual void onEventHappen(int interfaceID, NICEvent event)=0;
};

#endif