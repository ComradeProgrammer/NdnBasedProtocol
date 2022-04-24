#ifndef __CRONJOB_HANDLER_H_
#define __CRONJOB_HANDLER_H_
#include "linkLayer/MacAddress.h"
#include "networkLayer/ip/Ipv4Address.h"
#include "networkLayer/ndn/ndnPacket/NdnData.h"
#include "networkLayer/ndn/ndnPacket/NdnInterest.h"
#include "networkLayerPlus/ndnRouting/model/neighbor/NdnRoutingNeighbor.h"
#include "util/cipher/SymmetricCipher.h"
#include "util/cipher/aes/Aes.h"
#include "util/declaration.h"

class NdnRoutingProtocol;

class CronJobHandler {
   public:
    CronJobHandler(NdnRoutingProtocol* _protocol) : protocol(_protocol) {}

    // called in interface down state object
    void sendingHelloMessageCronJob(int interfaceIndex);

    void neighborInactivityCronJob(NdnRoutingNeighbor* neighbor);

    bool ddInterestExpireCronJob(std::shared_ptr<int> retransmissionTime, std::shared_ptr<NdnInterest> packet, MacAddress sourceMac, std::string timerName);
    bool localLsaExpireCronJob(std::shared_ptr<int> retransmissionTime, std::shared_ptr<NdnInterest> packet, MacAddress sourceMac, std::string timerName);
    bool infoLsaExpireCronJob(std::shared_ptr<int> retransmissionTime, std::shared_ptr<NdnInterest> packet, MacAddress sourceMac, std::string timerName);
    bool registerExpireCronJob(std::shared_ptr<int> retransmissionTime, std::shared_ptr<NdnInterest> packet, MacAddress sourceMac, std::string timerName);
    bool deRegisterExpireCronJob(std::shared_ptr<int> retransmissionTime, std::shared_ptr<NdnInterest> packet, MacAddress sourceMac, std::string timerName);

   protected:
    NdnRoutingProtocol* protocol;
};
#endif