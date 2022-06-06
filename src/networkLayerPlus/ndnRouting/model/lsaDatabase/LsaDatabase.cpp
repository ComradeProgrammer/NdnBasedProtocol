#include "LsaDatabase.h"
using namespace std;
using json = nlohmann::json;
enum VertexType { ROUTER, NET };
struct Vertex {
    int index;
    VertexType vertexType;
    RouterID routerID;
    Ipv4Address addr;
    Ipv4Address mask;
};

struct RoutingItem {
    Ipv4Address targetIP;
    Ipv4Address targetMask;
    Ipv4Address nextHop;
    int cost;
};

shared_ptr<LsaDataPack> LsaDatabase::findLsa(LinkStateType lsaType, RouterID routerID) {
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

void LsaDatabase::deleteLsa(std::shared_ptr<LsaDataPack> lsa) {
    switch (lsa->lsType) {
        case LinkStateType::ADJ: {
            for (auto itr = adjLsa.begin(); itr != adjLsa.end(); itr++) {
                if ((*itr)->routerID == lsa->routerID) {
                    adjLsa.erase(itr);
                    break;
                }
            }
            break;
        }
        case LinkStateType::RCH: {
            for (auto itr = rchLsa.begin(); itr != rchLsa.end(); itr++) {
                if ((*itr)->routerID == lsa->routerID) {
                    rchLsa.erase(itr);
                    break;
                }
            }
        } break;
    }
}

void LsaDatabase::insertLsa(shared_ptr<LsaDataPack> lsa) {
    deleteLsa(lsa);
    switch (lsa->lsType) {
        case LinkStateType::ADJ: {
            adjLsa.push_back(lsa);
            break;
        }
        case LinkStateType::RCH: {
            rchLsa.push_back(lsa);
        } break;
    }
    LOGGER->INFOF(2, "LsaDataBase::insertLsa %s current database(%d) %s",lsa->toString().c_str(), adjLsa.size() + rchLsa.size(), toString().c_str());
}

void LsaDatabase::calculateRoutingTable(RouterID source) {
    unordered_map<uint32_t, shared_ptr<LsaDataPack>> adjLsaMap;
    for (auto adj : adjLsa) {
        adjLsaMap[adj->routerID] = adj;
    }
    // construct graph via current databases;
    int index = 1;
    // index->vertex
    unordered_map<int, shared_ptr<Vertex>> vertices;
    // routerID->vertex
    unordered_map<RouterID, shared_ptr<Vertex>> routerVertices;
    // net addr->vertex
    unordered_map<uint32_t, shared_ptr<Vertex>> netVertices;

    Graph g;
    //构建拓扑图
    for (int i = 0; i < adjLsa.size(); i++) {
        shared_ptr<Vertex> vertex = nullptr;
        if (routerVertices.find(adjLsa[i]->routerID) == routerVertices.end()) {
            vertex = make_shared<Vertex>();
            vertex->index = index++;
            vertex->routerID = adjLsa[i]->routerID;
            vertex->vertexType = VertexType::ROUTER;
            routerVertices[vertex->routerID] = vertex;
            vertices[vertex->index] = vertex;
        } else {
            vertex = routerVertices[adjLsa[i]->routerID];
        }
        g.addVertex(vertex->index);

        // links
        for (int j = 0; j < adjLsa[i]->links.size(); j++) {
            NdnLink link = adjLsa[i]->links[j];
            if (link.linkType == LinkType::POINTTOPOINT_LINK) {
                // p2p , counterpart is also a router
                // check whether we need to create a new one
                RouterID rid2 = link.linkID;
                shared_ptr<Vertex> vertex2 = nullptr;
                if (routerVertices.find(rid2) == routerVertices.end()) {
                    vertex2 = make_shared<Vertex>();
                    vertex2->index = index++;
                    vertex2->routerID = rid2;
                    vertex2->vertexType = VertexType::ROUTER;
                    routerVertices[vertex2->routerID] = vertex2;
                    vertices[vertex2->index] = vertex2;
                } else {
                    vertex2 = routerVertices[rid2];
                }
                // add link
                g.addEdge(vertex->index, vertex2->index, link.linkCost);
                g.addEdge(vertex2->index, vertex->index, link.linkCost);
            } else if (link.linkType == LinkType::TRANSIT_LINK) {
                // network
                // find whether we have recorded this net
                Ipv4Address addr, mask, network;
                addr.addr = link.linkData;
                mask.addr = link.linkDataMask;
                network = addr.andMask(mask);

                shared_ptr<Vertex> vertex2 = nullptr;
                if (netVertices.find(network.addr) == netVertices.end()) {
                    vertex2 = make_shared<Vertex>();
                    vertex2->index = index++;
                    vertex2->addr = network;
                    vertex2->mask = mask;
                    netVertices[network.addr] = vertex2;
                    vertices[vertex2->index] = vertex2;
                } else {
                    vertex2 = netVertices[network.addr];
                }
                g.addEdge(vertex->index, vertex2->index, link.linkCost);
            }
        }
    }


    // todo: handle rch links
    auto sourceVertex = routerVertices[source];
    if (sourceVertex == nullptr) {
        LOGGER->WARNING("no vertex about ourself recorded");
        return;
    }

    auto result = g.calculateShortestPath(sourceVertex->index);

    auto ourLsa = adjLsaMap[source];
    if (ourLsa == nullptr) {
        LOGGER->WARNING("no lsa about ourself found");
        return;
    }


    // generate routing table
    //准备加入路由表的表项,网段->路由表项
    unordered_map<int, RoutingItem> insertList;
    for (auto r : result) {
        int targetIndex = r.first;
        if (vertices.find(targetIndex) == vertices.end()) {
            LOGGER->ERROR("LsaDatabase::calculateRoutingTable, not recorded vertex found");
            continue;
        }

        if (vertices[targetIndex]->vertexType != ROUTER) {
            continue;
        }

        // router link,use next hop
        int nextHopIndex = r.second[0];
        if (vertices.find(nextHopIndex) == vertices.end()) {
            LOGGER->ERROR("LsaDatabase::calculateRoutingTable, not recorded vertex found");
            continue;
        }
        //下一跳不是路由器，用下一跳的下一跳
        if (vertices[nextHopIndex]->vertexType != ROUTER) {
            nextHopIndex = r.second[1];
            if (vertices.find(nextHopIndex) == vertices.end()) {
                LOGGER->ERROR("LsaDatabase::calculateRoutingTable, not recorded vertex found");
                continue;
            }
        }

        RouterID targetRouterID = vertices[targetIndex]->routerID;
        RouterID nextHopRouterID = vertices[nextHopIndex]->routerID;
        // find out the lsa of nextHop and target so that we can detetmine the ip of target
        if (adjLsaMap.find(targetRouterID) == adjLsaMap.end()) {
            LOGGER->INFOF(2, "no adj lsa related with target %d", targetRouterID);
            continue;
        }
        if (adjLsaMap.find(nextHopRouterID) == adjLsaMap.end()) {
            LOGGER->INFOF(2, "no adj lsa related with nextHop %d", nextHopRouterID);
            continue;
        }


        //确定下一跳地址
        Ipv4Address nextHopAddr;
        bool found = false;
        int currentCost = 0x7fffffff;
        for (auto link : ourLsa->links) {
            if (link.linkID == nextHopRouterID) {
                found = true;
                if (link.linkCost < currentCost) {
                    // choose the route with least cost when facing duplicated edge
                    currentCost = link.linkCost;
                    nextHopAddr.addr = link.linkData;
                }
            }
        }
        if (!found) {
            LOGGER->ERRORF("no proper next hop found when calculating the routing table,nextHopRouterID %d ", nextHopRouterID);
            continue;
        }

        auto targetLsa = adjLsaMap[targetRouterID];
        //把每一个和目标路由器相连的网段加入路由表
        for (auto targetLink : targetLsa->links) {
            // LOGGER->VERBOSEF("target %d link %s", targetRouterID, targetLink.toString().c_str());
            Ipv4Address targetIp, targetMask;
            bool direct = false;

            //排除直连网段
            for (auto sourceLink : ourLsa->links) {
                Ipv4Address sourceIp, sourceMask;
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

            Ipv4Address networkLocation = targetIp.andMask(targetMask);
            if (insertList.find(networkLocation.addr) != insertList.end()) {
                if (insertList[networkLocation.addr].cost < r.second[2]) {
                    continue;
                }
            }
            insertList[networkLocation.addr] = {targetIp, targetMask, nextHopAddr, r.second[2]};
            // LOGGER->VERBOSEF("target %s, target %s, nextHopAddr
            // %s",targetIp.toString().c_str(),targetMask.toString().c_str(),nextHopAddr.toString().c_str());
        }
    }


    //插入路由表项
    auto routingTable = IOC->getRoutingTable();
    routingTable->removeAllItem();
    for (auto newItem : insertList) {
        RoutingTableItem item(newItem.second.targetIP, newItem.second.targetMask, newItem.second.nextHop);
        item.setFromRoutingProtocol(true);
        routingTable->addRoutingTableItem(item);
    }

    // todo: remove debug info

    // auto routingInfo = runCmd("route -n");
    // LOGGER->INFOF(2, "here \n%s\n", routingInfo.second.c_str());
}

unordered_map<RouterID, RouterID> LsaDatabase::calculateMinHopTree(RouterID source) {
    int index = 1;
    // index->vertex
    unordered_map<int, shared_ptr<Vertex>> vertices;
    // routerID->vertex
    unordered_map<RouterID, shared_ptr<Vertex>> routerVertices;

    Graph g;

    for (int i = 0; i < adjLsa.size(); i++) {
        shared_ptr<Vertex> vertex = nullptr;
        if (routerVertices.find(adjLsa[i]->routerID) == routerVertices.end()) {
            vertex = make_shared<Vertex>();
            vertex->index = index++;
            vertex->routerID = adjLsa[i]->routerID;
            vertex->vertexType = VertexType::ROUTER;
            routerVertices[vertex->routerID] = vertex;
            vertices[vertex->index] = vertex;
        } else {
            vertex = routerVertices[adjLsa[i]->routerID];
        }
        g.addVertex(vertex->index);

        // links
        for (int j = 0; j < adjLsa[i]->links.size(); j++) {
            NdnLink link = adjLsa[i]->links[j];
            if (link.linkType == LinkType::POINTTOPOINT_LINK) {
                // p2p , counterpart is also a router
                // check whether we need to create a new one
                RouterID rid2 = link.linkID;
                shared_ptr<Vertex> vertex2 = nullptr;
                if (routerVertices.find(rid2) == routerVertices.end()) {
                    vertex2 = make_shared<Vertex>();
                    vertex2->index = index++;
                    vertex2->routerID = rid2;
                    vertex2->vertexType = VertexType::ROUTER;
                    routerVertices[vertex2->routerID] = vertex2;
                    vertices[vertex2->index] = vertex2;
                } else {
                    vertex2 = routerVertices[rid2];
                }
                // add link
                g.addEdge(vertex->index, vertex2->index, link.linkCost);
                g.addEdge(vertex2->index, vertex->index, link.linkCost);
            }
            // todo: need to deal with this network
        }
    }

    // root->parent
    unordered_map<RouterID, RouterID> result;
    // 因为我们可能在和其他路由器形成邻居关系之前，通过local lsa交换收到其他路由器的LSA，使得source 节点的lsa未必被加入过DATABASE时候就被触发计算
    //所以要确保source也被加入过，不然下面这句就炸了
    if (routerVertices.find(source) == routerVertices.end()) {
        return result;
    }
    int sourceVertexIndex = routerVertices[source]->index;
    for (auto p : vertices) {
        auto reverseTree = g.calculateMinHopTree(p.first);
        if (reverseTree.find(sourceVertexIndex) != reverseTree.end()) {
            RouterID root = vertices[p.first]->routerID;
            RouterID parent = vertices[reverseTree[sourceVertexIndex]]->routerID;
            result[root] = parent;
        }
    }

    stringstream ss;
    for (auto i : result) {
        ss << "root " << i.first << ", parent " << i.second << endl;
    }
    LOGGER->INFO(2,"minimum hop tree:\n" + ss.str());

    return result;
}

std::pair<std::unique_ptr<unsigned char[]>, int> LsaDatabase::databaseHash() {
    vector<string> names;
    for (auto lsa : adjLsa) {
        auto digest = lsa->generateLSDigest();
        string name = "LS/" + getNameForLinkStateType(digest.linkStateType) + "/" + to_string((unsigned long long)(digest.routerID)) + "/" +
                      to_string(digest.sequenceNum);
        names.push_back(name);
    }

    for (auto lsa : rchLsa) {
        auto digest = lsa->generateLSDigest();
        string name = "LS/" + getNameForLinkStateType(digest.linkStateType) + "/" + to_string((unsigned long long)(digest.routerID)) + "/" +
                      to_string(digest.sequenceNum);
        names.push_back(name);
    }
    sort(names.begin(), names.end());
    shared_ptr<Hasher> hasher = make_shared<Md5Hasher>();
    for (auto n : names) {
        hasher->input(n.c_str(), n.size());
    }
    // the hasher we choose here must return an array of 16 bytes or there will be bugs because the packet format require 16 bytes
    return hasher->getResult();
}

json LsaDatabase::marshal() const {
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

std::string LsaDatabase::printContent() {
    json j = marshal();
    //auto hash = databaseHash();
   // j["databaseHash"] = hexString(hash.first.get(), 16);
    return j.dump();
}