#ifndef __GRAPH_H_
#define __GRAPH_H_
#include <algorithm>
#include <cstdlib>
#include <queue>
#include <unordered_map>
#include <vector>

#include "ioc.h"
#include "networkLayerPlus/ndnRouting/dataPack/PacketCommon.h"
struct Edge {
    RouterID source;
    RouterID target;
    int cost;

    RouterID nextHop;
    RouterID nextnextHop;
    bool operator<(const Edge& e) const { return cost < e.cost; }
    bool operator>(const Edge& e) const { return cost > e.cost; }
};
class Graph {
   public:
    Graph() = default;
    void addVertex(RouterID rid);
    void addEdge(RouterID ridSource, RouterID ridTarget, int cost);
    void removeVertex(RouterID rid);
    void removeEdge(RouterID rid1, RouterID rid2);
    /**
     * @brief 
     * 
     * @param source 
     * @return std::unordered_map<RouterID, std::vector<RouterID>> target->[nexthop,nextnexthop,cost]
     */
    std::unordered_map<RouterID, std::vector<RouterID>> calculateShortestPath(RouterID source);
    /**
     * @brief 
     * 
     * @param source 
     * @return std::unordered_map<RouterID, RouterID> node->parentnode
     * 
     */
    std::unordered_map<RouterID, RouterID> calculateMinHopTree(RouterID source);

   private:
    std::unordered_map<RouterID, std::vector<Edge>> graph;
};
#endif