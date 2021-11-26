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
    logger->VERBOSE("here1");
    g.calculate(routerID);
    auto res = g.getResult();
    // for each result, generate the routing items
    vector<RoutingTableItem>newItems;
    logger->VERBOSE("here2");

    for (auto r : res) {
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