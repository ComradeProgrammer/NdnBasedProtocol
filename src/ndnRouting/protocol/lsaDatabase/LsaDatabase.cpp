#include "LsaDatabase.h"
#include "ndnRouting/protocol/NdnRoutingProtocol.h"

using namespace std;
using json = nlohmann::json;
shared_ptr<LsaDataPack> LsaDataBase::findLsa(LinkStateType lsaType, uint32_t routerID) {
    switch (lsaType) {
        case LinkStateType::ADJ: {
            for (auto lsa : adjLsa) {
                if (routerID == lsa->routerID) {
                    return lsa;
                }
            }
            return nullptr;
        } break;
        case LinkStateType::RCH: {
            for (auto lsa : rchLsa) {
                if (routerID == lsa->routerID) {
                    return lsa;
                }
            }
            return nullptr;
        } break;
    }
    return nullptr;
}

void LsaDataBase::insertLsa(shared_ptr<LsaDataPack> lsa) {
    switch (lsa->lsType) {
        case LinkStateType::ADJ: {
            for (auto itr = adjLsa.begin(); itr != adjLsa.end(); itr++) {
                if ((*itr)->routerID == lsa->routerID) {
                    adjLsa.erase(itr);
                    break;
                }
            }
            adjLsa.push_back(lsa);
            break;
        }
        case LinkStateType::RCH: {
            for (auto itr = rchLsa.begin(); itr != rchLsa.end(); itr++) {
                if ((*itr)->routerID == lsa->routerID) {
                    rchLsa.erase(itr);
                    break;
                }
            }
            rchLsa.push_back(lsa);
        } break;
    }
    logger->INFOF("LsaDataBase::insertLsa current database %s", toString().c_str());
}

void LsaDataBase::rebuildRoutingTable() {
    // todo: implement
    // first: insert all vertices
    // fix-me: now we assume that there are only p2p link,in future we need to take transit stub and rch lsa into
    // consideraion
    Graph g;
    unordered_set<uint32_t> knownRouters;
    unordered_map<uint32_t, shared_ptr<LsaDataPack>> adjLsaMap;

    uint32_t routerID = NdnRoutingProtocol::getNdnRoutingProtocol()->getRouterID();

    for (auto adj : adjLsa) {
        g.addVertex(adj->routerID);
        knownRouters.insert(adj->routerID);
        adjLsaMap[adj->routerID] = adj;
    }

    // then add edges
    for (auto adj : adjLsa) {
        for (auto link : adj->links) {
            if (link.linkType == LinkType::POINTTOPOINT_LINK) {
                if (knownRouters.find(link.linkID) == knownRouters.end()) {
                    logger->WARNINGF("no lsa found for link %s, ignored", link.toString().c_str());
                    continue;
                }
                g.addEdge(link.linkID, adj->routerID, link.linkCost);
                g.addEdge(adj->routerID, link.linkID, link.linkCost);
            } else {
                // todo: implement
            }
        }
    }

    g.calculate(routerID);
    spfRes = g.getResult();
    // for each result, generate the routing items
    vector<RoutingTableItem>newItems;
    for (auto r : spfRes) {
        uint32_t target = r.first;
        uint32_t nextHopRID=r.second.first;
        int cost = r.second.second;

        if (target == routerID ) {
            continue;
        }

        // determine the nextHop
        if (adjLsaMap.find(routerID) == adjLsaMap.end()) {
            logger->ERROR("no adj lsa related with ourself");
            return;
        }

        auto ourLsa = adjLsaMap[routerID];
        Ipv4Address nextHop;
        bool found = false;
        for (auto link : ourLsa->links) {
            if (link.linkID == nextHopRID) {
                nextHop.addr = link.linkData;
                found = true;
            }
        }
        if (!found) {
            logger->ERROR("no proper next hop found when calculating the routing table");
            return;
        }
        // generate routing table items;
        if (adjLsaMap.find(target) == adjLsaMap.end()) {
            logger->ERROR("no adj lsa related with ourself");
            return;
        }
        auto targetLsa = adjLsaMap[target];
        for (auto targetLink : targetLsa->links) {
            logger->VERBOSEF("target %d link %s",target,targetLink.toString().c_str());
            bool direct = false;
            Ipv4Address targetIp, targetMask;
            // exclude direct-linked network segment
            for (auto sourceLink : ourLsa->links) {
                Ipv4Address  sourceIp, sourceMask;
                targetIp.addr = targetLink.linkData;
                targetMask.addr = targetLink.linkDataMask;
                sourceIp.addr = sourceLink.linkData;
                sourceMask.addr = sourceLink.linkDataMask;

                if (targetIp.andMask(targetMask) == sourceIp.andMask(sourceMask)) {
                    direct = true;
                    break;
                }
            }
            if (direct) {
                continue;
            }
            logger->VERBOSEF("dest %s mask %s nexthop%s",targetIp.toString().c_str(),targetMask.toString().c_str(),nextHop.toString().c_str());
            RoutingTableItem item(targetIp,targetMask,nextHop,logger);
            item.setFromRoutingProtocol(true);
            newItems.push_back(item);
        }
    }

    auto routingTable=RoutingTable::getRoutingTable(logger);
    routingTable->removeAllItem();
    for(auto i : newItems){
        routingTable->addRoutingTableItem(i);
    }
    //todo: remove debug info

    auto routingInfo=runCmd("route -n");
    logger->INFOF("\n%s\n",routingInfo.second.c_str());
}
void LsaDataBase::sendInfoInterestDueToAge(shared_ptr<LsaDataPack> lsa){
    LinkStateDigest digest=lsa->generateLSDigest();
    digest.lsAge=NDN_ROUTING_MAX_AGE;

    auto protocol = NdnRoutingProtocol::getNdnRoutingProtocol();
    InfoInterestPack infoInterest;
    infoInterest.infoType = InfoType::INFO_REFRESH;
    infoInterest.src = protocol->getRouterID();
    infoInterest.ls.push_back(digest);
    infoInterest.neighbors.push_back(protocol->getRouterID());
    auto encodePair = infoInterest.encode();
    auto packet = make_shared<NdnInterest>(logger);

    logger->INFOF("sending out interest info, content %s", infoInterest.toString().c_str());
    string name =
        "/routing/broadcast/INFO/" + getNameForInfoType(InfoType::INFO_REFRESH) + "/" + to_string(protocol->getRouterID()) + "/";
    packet->setName(name);
    packet->setNonce(rand());
    packet->setApplicationParameters(encodePair.first, encodePair.second.get());

    NdnRoutingProtocol::getNdnRoutingProtocol()->unlock();
    //TODO: change to a reasonable source mac address?
    NdnRoutingProtocol::getNdnRoutingProtocol()->sendPacket(MacAddress("00:00:00:00:00:00"), packet);
    // get the lock back because after return the lock needs to be attained
    NdnRoutingProtocol::getNdnRoutingProtocol()->lock();
}

void LsaDataBase::ageDataBase(){
    bool shouldRebuild=false;
    auto protocol = NdnRoutingProtocol::getNdnRoutingProtocol();
    uint32_t routerID = protocol->getRouterID();

    vector<shared_ptr<LsaDataPack>>allLsa;
    for(auto lsa:adjLsa){
        allLsa.push_back(lsa);
    }
    for(auto lsa:rchLsa){
        rchLsa.push_back(lsa);
    }

    for(auto lsa: allLsa){
        uint16_t lsaAge=lsa->lsAge;
        bool selfOriginated=((lsa->routerID)==routerID);
        bool unreachable=(spfRes.find(lsa->routerID)==spfRes.end());

        if(
            (selfOriginated  && lsaAge<(NDN_ROUTING_LS_REFRESH-1))||
            (!selfOriginated && lsaAge<(NDN_ROUTING_MAX_AGE -1))){
                lsa->lsAge++;
        }
        // if this lsa was created by this router 
        if(selfOriginated && lsaAge>NDN_ROUTING_LS_REFRESH){
            if(unreachable){
                // ?? in case of unexpected selfOriginated lsa received?
                sendInfoInterestDueToAge(lsa);
            }else{
                int32_t seq=lsa->seqNum;
                if(seq==NDN_ROUTING_MAX_SEQ){
                    //if the sequence number is about to exceed the limit of int
                    sendInfoInterestDueToAge(lsa);
                }else{
                    auto newLsa=protocol->generateLsa();
                    newLsa->seqNum=lsa->seqNum+1;
                    shouldRebuild=true;
                    insertLsa(newLsa);
                    //FIX-ME: INFO type should be up?
                    sendInfoInterestDueToAge(newLsa);
                }
            }
        }
        // if this lsa is not created by this router
        if(!selfOriginated && (lsa->lsAge==NDN_ROUTING_MAX_AGE-1)){
            //some router starts aging earlier while the others may not, so we cannot be certain about
            //which router would reach lsa's maxage first
            sendInfoInterestDueToAge(lsa);
        }
        if(lsaAge>=NDN_ROUTING_MAX_AGE){
            //we cannot just delete a lsa when some neighbors is still exchanging with this router
            //because doing so can jeopardize the digest transmitting list.
            if(
                !protocol->hasNeighborInState(NeighborStateType::EXCHANGE_STATE)
                &&!protocol->hasNeighborInState(NeighborStateType::LOADING_STATE)
            ){
                if(!selfOriginated || unreachable){
                    deleteLsa(lsa);
                }else{
                    auto newLsa=protocol->generateLsa();
                    newLsa->seqNum=lsa->seqNum+1;
                    shouldRebuild=true;
                    insertLsa(newLsa);
                    //FIX-ME: INFO type should be up?
                    sendInfoInterestDueToAge(newLsa);
                }
            }
        }
 
    }
}

void LsaDataBase::deleteLsa(shared_ptr<LsaDataPack> lsa){
    if(lsa->lsType==LinkStateType::ADJ){    
        vector<shared_ptr<LsaDataPack>>::iterator itr;
        for(itr=adjLsa.begin();itr!=adjLsa.end();itr++){
            if((*itr)->routerID==lsa->routerID ){
                break;
            }
        }
        if(itr!=adjLsa.end()){
            adjLsa.erase(itr);
        }
    }else{
        vector<shared_ptr<LsaDataPack>>::iterator itr;
        for(itr=rchLsa.begin();itr!=rchLsa.end();itr++){
            if((*itr)->routerID==lsa->routerID ){
                break;
            }
        }
        if(itr!=rchLsa.end()){
            rchLsa.erase(itr);
        }
    }
}


json LsaDataBase::marshal() const {
    json j;
    vector<json> adjlsastr;
    for (auto lsa : adjLsa) {
        adjlsastr.push_back(lsa->marshal());
    }
    vector<json> rchlsastr;
    for (auto lsa : rchLsa) {
        rchlsastr.push_back(lsa->marshal());
    }
    j["adjLsa"] = adjlsastr;
    j["rchLsa"] = rchlsastr;
    return j;
}