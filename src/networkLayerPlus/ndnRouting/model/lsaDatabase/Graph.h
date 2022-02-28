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
    int source;
    int target;
    int cost;

    int nextHop;
    int nextnextHop;
    bool operator<(const Edge& e) const { return cost < e.cost; }
    bool operator>(const Edge& e) const { return cost > e.cost; }
};
class Graph {
   public:
    Graph() = default;
    void addVertex(int rid);
    void addEdge(int ridSource, int ridTarget, int cost);
    void removeVertex(int rid);
    void removeEdge(int rid1, int rid2);

    bool isBidirectionalEdge(int rid1, int rid2);

    std::vector<int>getAllVertices();
    
    /**
     * @brief calculateShortestPath,only bidirectional edge is regarded valid
     *
     * @param source
     * @return std::unordered_map<int, std::vector<int>> target->[nexthop,nextnexthop,cost]
     */
    std::unordered_map<int, std::vector<int>> calculateShortestPath(int source);
    /**
     * @brief calculateMinHopTree,only bidirectional edge is regarded valid
     *
     * @param source
     * @return std::unordered_map<int, int> node->parentnode
     *
     */
    std::unordered_map<int, int> calculateMinHopTree(int source);

   private:
    std::unordered_map<int, std::vector<Edge>> graph;
};
#endif