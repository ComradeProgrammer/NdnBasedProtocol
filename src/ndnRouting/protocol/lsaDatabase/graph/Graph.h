#ifndef __GRAPH_H_
#define __GRAPH_H_
#include <cstdlib>
#include <queue>
#include <unordered_map>
#include <algorithm>
#include "util/log/Logger.h"
struct Edge {
    uint32_t source;
    uint32_t target;
    int cost;
    uint32_t nextHop = 0;
    bool operator<(const Edge& e) const { return cost < e.cost; }
    bool operator>(const Edge& e) const { return cost > e.cost; }
};

// directed graph
class Graph {
   public:
    Graph(std::shared_ptr<Logger> _logger = nullptr);
    void addVertex(uint32_t rid);
    void addEdge(uint32_t ridSource, uint32_t ridTarget, int cost);


    void removeVertex(uint32_t rid);
    void removeEdge(uint32_t rid1, uint32_t rid2);

    /**
     * @brief calculate the shortest path using dijskra
     */
    void calculateShortestPath(uint32_t source);

    std::unordered_map<uint32_t,std::vector<uint32_t>>calculateMinHopTree(uint32_t source);
    /**
     * @return //rid-><nexthop,mincost>
     */
    std::unordered_map<uint32_t, std::pair<uint32_t, int>> getResult() { return res; }

   private:
    std::shared_ptr<Logger> logger;
    std::unordered_map<uint32_t, std::vector<Edge>> graph;
    // rid-><nexthop,cost>
    std::unordered_map<uint32_t, std::pair<uint32_t, int>> res;
};
#endif