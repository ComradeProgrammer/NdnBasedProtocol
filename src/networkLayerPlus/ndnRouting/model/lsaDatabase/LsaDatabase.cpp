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

shared_ptr<LsaDataPack> LsaDatabase::findLsa(LinkStateType lsaType, uint32_t routerID) {
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

void LsaDatabase::insertLsa(shared_ptr<LsaDataPack> lsa) {
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
    LOGGER->INFOF(2, "LsaDataBase::insertLsa current database %s", toString().c_str());
}
unordered_map<RouterID, vector<RouterID>> LsaDatabase::calculateRoutingTable(RouterID source) {
    // construct graph via current databases;
    int index = 1;
    unordered_map<int, shared_ptr<Vertex>> vertices;
    unordered_map<uint32_t, shared_ptr<Vertex>> netVertices;
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
                }else{
                    vertex2=netVertices[network.addr];
                }
                g.addEdge(vertex->index, vertex2->index, link.linkCost);
            }
        }

        //todo: handle rch links
        auto sourceVertex=routerVertices[source];
        auto result=g.calculateShortestPath(sourceVertex->index);
        //todo: generate routing table
    }
}

unordered_map<RouterID, RouterID> LsaDatabase::calculateMinHopTree(RouterID source) {

    int index = 1;
    //index->vertex
    unordered_map<int, shared_ptr<Vertex>> vertices;
    //routerID->vertex
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
            //for minimum-hop tree, we don't need to deal with this network
        }
    }

    //root->parent
    unordered_map<RouterID, RouterID>result;
    int sourceVertexIndex=routerVertices[source]->index;
    for(auto p:vertices){
        auto reverseTree=g.calculateMinHopTree(p.first);
        if(reverseTree.find(sourceVertexIndex)!=reverseTree.end()){
            RouterID root=vertices[p.first]->routerID;
            RouterID parent=vertices[reverseTree[sourceVertexIndex]]->routerID;
            result[root]=parent;
        }
    }

    return result;
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