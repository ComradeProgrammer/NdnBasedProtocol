#ifndef __NDNADDRINTERFACE_H_
#define __NDNADDRINTERFACE_H_
#include "ethernet/interface/NICManager.h"
#include "ndn/ndnProtocol/NdnProtocol.h"
class NdnAddrInterface : public NICObserver{
    public: 
    NdnAddrInterface(NIC nic, std::shared_ptr<Logger> _logger = nullptr);
    virtual void onEventHappen(int interfaceID, NICEvent event) override;
    private:
    std::shared_ptr<Logger> logger;
};
#endif