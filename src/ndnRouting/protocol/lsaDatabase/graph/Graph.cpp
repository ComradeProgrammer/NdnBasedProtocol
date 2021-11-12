#include "Graph.h"
using namespace std;
Graph::Graph(shared_ptr<Logger> _logger) { logger = Logger::getDefaultLoggerIfNull(_logger); }

void Graph::addVertex(uint32_t rid) {
    if (graph.find(rid) == graph.end()) {
        graph[rid] = vector<Edge>();
    } else {
        logger->WARNINGF("Graph::addVertex: rid %d exists.", rid);
    }
}

void Graph::addEdge(uint32_t ridSource, uint32_t ridTarget, int cost) {
    if (graph.find(ridSource) == graph.end()) {
        logger->ERRORF("Graph::addVertex: rid %d doesn't exist.", ridSource);
        return;
    }
    if (graph.find(ridTarget) == graph.end()) {
        logger->ERRORF("Graph::addVertex: rid %d doesn't exist.", ridTarget);
        return;
    }
    graph[ridSource].push_back({ridSource, ridTarget, cost, 0});
    // graph[ridTarget].push_back({ridTarget,ridSource,cost,0});
}

void Graph::calculate(uint32_t source) {
    res.clear();
    priority_queue<Edge, vector<Edge>, greater<Edge>> queue;
    queue.push({source, source, 0, source});
    while (!queue.empty()) {
        Edge e = queue.top();
        queue.pop();

        if (res.find(e.target) != res.end()) {
            continue;
        }
        res[e.target] = {e.nextHop, e.cost};
        for (auto i : graph[e.target]) {
            Edge newEdge;
            newEdge.source = source;
            newEdge.target = i.target;
            newEdge.cost = i.cost + e.cost;
            if (i.target == 1) {
                cout << "";
            }
            if (e.target == source) {
                newEdge.nextHop = i.target;
            } else {
                newEdge.nextHop = e.nextHop;
            }
            queue.push(newEdge);
        }
    }
}