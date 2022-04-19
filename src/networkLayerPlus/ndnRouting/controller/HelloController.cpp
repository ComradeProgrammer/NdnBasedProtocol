#include "HelloController.h"

#include "networkLayerPlus/ndnRouting/NdnRoutingProtocol.h"
using namespace std;
void HelloController::onReceiveInterest(int interfaceIndex, MacAddress sourceMac, std::shared_ptr<NdnInterest> interest) {
    try {
        lock_guard<mutex> lockFunction(*(protocol->mutexLock));

        // resolve the content of hello packet
        auto helloInfoDataEncrypted = interest->getApplicationParameters();

        HelloInterestPack helloInfo;
        shared_ptr<SymmetricCipher>decryptor =make_shared<Aes>();
        string key=protocol->getPassword();
        decryptor->setKey(key.c_str(),key.size());
        auto helloInfoData=decryptor->decrypt(helloInfoDataEncrypted.second.get(),helloInfoDataEncrypted.first);
        helloInfo.decode((const char *)helloInfoData.first.get(), helloInfoData.second);

        LOGGER->INFOF(2, "HelloController::onReceiveInterest at interface %d, source %s, content %s", interfaceIndex, sourceMac.toString().c_str(),
                      helloInfo.toString().c_str());

        auto interfaceObj = protocol->interfaces[interfaceIndex];
        // check the mask, interval and routerDeadInterval
        if (!(helloInfo.networkMask == interfaceObj->getIpv4Mask())) {
            LOGGER->WARNING("NdnRoutingInterface::onReceiveHelloInterest packet is dropped due to incompatible mask");
            return;
        }
        if (helloInfo.routerDeadInterval != NDNROUTING_ROUTERDEADINTERVAL) {
            LOGGER->WARNING("NdnRoutingInterface::onReceiveHelloInterest packet is dropped due to incompatible router dead interval");
            return;
        }

        // if related neighbor is not recorded, create a new one
        auto neighborObj = interfaceObj->getNeighborByRouterID(helloInfo.routerId);
        if (neighborObj == nullptr) {
            bool ok = helloInfo.verifyRouterID();
            if (!ok) {
                LOGGER->ERROR("router ID and public key in hello packet don't match");
                return;
            }
            neighborObj = make_shared<NdnRoutingNeighbor>(interfaceObj.get());
            neighborObj->setRouterID(helloInfo.routerId);
            neighborObj->setMacAddress(sourceMac);
            neighborObj->setIpv4Address(helloInfo.interfaceIP);
            neighborObj->setIpv4Mask(helloInfo.networkMask);
            neighborObj->setPublicKey(string(helloInfo.publicKey));
            interfaceObj->addNeighbor(neighborObj);
        }
        if (neighborObj->getState() < NeighborStateType::EXCHANGE) {
            bool ok = helloInfo.verifySignature();
            if (!ok) {
                LOGGER->ERRORF("invalid signature %s", interest->getName().c_str());
                return;
            }
        } else {
            bool ok = helloInfo.verifySignature(neighborObj->getPublicKey());
            if (!ok) {
                LOGGER->ERRORF("invalid signature %s", interest->getName().c_str());
                return;
            }
        }

        neighborObj->processEvent(NeighborEventType::HELLO_RECEIVED);

        // now resolve the neighbor info incorporated in the hello packet
        // if we find that current instance shows up in hellopacket, then just go directly into 2-way
        bool found = false;
        for (auto i : helloInfo.neighbor) {
            if (i == interfaceObj->getIpv4Address()) {
                // found
                neighborObj->processEvent(NeighborEventType::TWOWAY_RECEIVED);
                found = true;
                break;
            }
        }
        if (!found) {
            neighborObj->processEvent(NeighborEventType::ONEWAY_RECEIVED);
        }

        // if the state of all neighbors are full but hash is incorrect, return to exchanging state
        if (protocol->allNeighboursFull() && protocol->broadcastLsaPendingRequestList.size() == 0) {
            auto ourHash = protocol->database->databaseHash();
            bool identical = true;
            for (int i = 0; i < 16; i++) {
                if (ourHash.first[i] != helloInfo.databaseHash[i]) {
                    identical = false;
                    break;
                }
            }
            if (!identical) {
                LOGGER->WARNINGF("checking hash failed, neighbor %d, fallback", neighborObj->getRouterID());
                neighborObj->processEvent(NeighborEventType::INVALID_HASH);
            }
        }
    } catch (exception e) {
        LOGGER->ERRORF("standard exception captured, %s", e.what());
        exit(-1);
    } catch (...) {
        LOGGER->ERROR("non-standard exception captured");
        exit(-1);
    }
}
void HelloController::onReceiveData(int interfaceIndex, MacAddress sourceMac, std::shared_ptr<NdnData> packet) {
    // should not reach here. Hello has no data.
}
