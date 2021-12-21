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
    // take the edge with least cost when met the duplicated edge
    for (int i = 0; i < graph[ridSource].size(); i++) {
        if (graph[ridSource][i].target == ridTarget) {
            graph[ridSource][i].cost = min(graph[ridSource][i].cost, cost);
            return;
        }
    }
    // no existing edge found
    graph[ridSource].push_back({ridSource, ridTarget, cost, 0});
    // graph[ridTarget].push_back({ridTarget,ridSource,cost,0});
}

void Graph::removeVertex(uint32_t rid) {
    if (graph.find(rid) == graph.end()) {
        return;
    }
    for (auto p : graph) {
        for (auto itr = p.second.begin(); itr != p.second.end(); itr++) {
            if (itr->target == rid) {
                p.second.erase(itr);
                // no duplictate edge thus it's ok to break here
                break;
            }
        }
    }
    graph.erase(graph.find(rid));
}

void Graph::removeEdge(uint32_t rid1, uint32_t rid2) {
    if (graph.find(rid1) == graph.end()) {
        return;
    }
    for (auto itr = graph[rid1].begin(); itr != graph[rid1].end(); itr++) {
        if (itr->target == rid2) {
            graph[rid1].erase(itr);
            // no duplictate edge thus it's ok to break here
            break;
        }
    }
}

void Graph::calculateShortestPath(uint32_t source) {
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

unordered_map<uint32_t,vector<uint32_t>>Graph::calculateMinHopTree(uint32_t source){
    //to make sure every node can generate the same minimum hop trees, we need to iterate edges in a certain sequence
    for(auto p:graph){
        sort(graph[p.first].begin(),graph[p.first].end(),[](Edge e1,Edge e2)->bool{
            return e1.target<e2.target;
        });
    }

    unordered_map<uint32_t,vector<uint32_t>>res;
    res[source]=vector<uint32_t>();
    vector<uint32_t>layer={source};

    while(layer.size()>0){
        vector<uint32_t>nextLayer;
        for(auto i: layer){
            if(graph.find(i)==graph.end()){
                logger->ERRORF("Graph::addVertex: rid %d doesn't exist in graph.", i);
                continue;
            }
           
            for(auto e:graph[i]){
                if(res.find(e.target)!=res.end()){
                    continue;
                }
                nextLayer.push_back(e.target);
                res[i].push_back(e.target);
                res[e.target]=vector<uint32_t>();
            }
        }
        layer=nextLayer;
    }
    return res;
}
