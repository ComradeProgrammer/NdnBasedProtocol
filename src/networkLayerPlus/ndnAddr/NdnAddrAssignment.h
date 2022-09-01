#ifndef __NDN_ADDR_ASSIGNMENT_PROTOCOL_H_
#define __NDN_ADDR_ASSIGNMENT_PROTOCOL_H_
#include "ioc.h"
#include"util/blockchain/BlockChain.h"
#include "networkLayer/ndn/NdnProtocol.h"
#include "networkLayerPlus/NdnProtocolPlus.h"
#include "networkLayerPlus/ndnAddr/controller/AddrConfirmationController.h"
#include "networkLayerPlus/ndnAddr/controller/AddrCronjobController.h"
#include "networkLayerPlus/ndnAddr/controller/AddrHelloController.h"
#include "networkLayerPlus/ndnAddr/controller/AddrRequestController.h"
#include "networkLayerPlus/ndnAddr/controller/AddrChainController.h"
#include "networkLayerPlus/ndnAddr/dataPack/AddrRequestData.h"
#include "networkLayerPlus/ndnAddr/model/addressPool/AddressPool.h"
#include "networkLayerPlus/ndnAddr/model/addressPool/DumbAddressPool.h"
#include "networkLayerPlus/ndnAddr/model/interface/NdnAddrInterface.h"
#include "networkLayerPlus/ndnAddr/model/addressPool/AddressValidator.h"

class NdnAddrAssignmentProtocol : public NdnProtocolPlus {
   public:
    NdnAddrAssignmentProtocol(RouterID _routerID, std::shared_ptr<NdnProtocol> _ndnProtocol);
    RouterID getRouterID() { return routerID; }
    void setRouterID(RouterID rid) { routerID = rid; }
    bool getIsRoot() { return isRoot; }
    void setIsRoot(bool value) { isRoot = value; }

    virtual void onReceiveNdnPacket(int interfaceIndex, MacAddress sourceMac, std::shared_ptr<NdnPacket> packet);
    void start();
    void sendPacket(MacAddress sourceMac, std::shared_ptr<NdnPacket> packet);
    void lock() { mutexLock->lock(); }
    void unlock() { mutexLock->unlock(); }

    std::shared_ptr<AddrCronjobController> getCronjobHandler() { return cronjobController; }
    std::shared_ptr<AddressPool> getAddressPool() { return addressPool; }
    void setAddressPool(std::shared_ptr<AddressPool> p) { addressPool = p; }

    std::string chainToString();
    void generateBlock();
    //void generateRootInfoBlock();

   private:
    std::shared_ptr<std::mutex> mutexLock;
    bool isRoot = false;

    RouterID routerID;
    std::shared_ptr<NdnProtocol> ndnProtocol;
    std::unordered_map<int, std::shared_ptr<NdnAddrInterface>> interfaces;
    std::shared_ptr<AddrCronjobController> cronjobController;
    std::shared_ptr<AddrHelloController> helloController;
    std::shared_ptr<AddrRequestController> requestController;
    std::shared_ptr<AddrConfirmationController> confirmationController;
    std::shared_ptr<AddrChainController> chainController;

    std::shared_ptr<AddressPool> addressPool;
    std::unordered_map<int, AddrRequestDataWrapper> rootAssignment;  // nonce->assignment

    vector<Ipv4Address> knownAddress;
    vector<Ipv4Address> knownMask;

    BlockChain chain;
    vector<string> blockBuffer;
    vector<string>prevBuffer;
    BlockHash estimatedHash;

    //bool firstBlockGenerated=false;

    AddressValidator validator;

    std::unordered_map<string,std::vector<std::shared_ptr<NdnInterest>>>chainBuffer;

    friend class AddrCronjobController;
    friend class AddrHelloController;
    friend class AddrRequestController;
    friend class AddrConfirmationController;
    friend class AddrChainController;
};
#endif