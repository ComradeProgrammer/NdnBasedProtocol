#include "Graph.h"
using namespace std;
void Graph::addVertex(RouterID rid) {
    if (graph.find(rid) == graph.end()) {
        graph[rid] = vector<Edge>();
    } else {
        LOGGER->WARNINGF("Graph::addVertex: rid %d exists.", rid);
    }
}

void Graph::addEdge(RouterID ridSource, RouterID ridTarget, int cost) {
    if (graph.find(ridSource) == graph.end()) {
        LOGGER->ERRORF("Graph::addVertex: rid %d doesn't exist.", ridSource);
        return;
    }
    if (graph.find(ridTarget) == graph.end()) {
        LOGGER->ERRORF("Graph::addVertex: rid %d doesn't exist.", ridTarget);
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
    graph[ridSource].push_back({ridSource, ridTarget, cost, 0, 0});
}

void Graph::removeVertex(RouterID rid) {
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

void Graph::removeEdge(RouterID rid1, RouterID rid2) {
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

unordered_map<RouterID, vector<RouterID>> Graph::calculateShortestPath(RouterID source) {
    // target->[nexthop,nextnexthop,cost]
    unordered_map<RouterID, vector<RouterID>> res;
    priority_queue<Edge, vector<Edge>, greater<Edge>> queue;
    queue.push({source, source, 0, source, source});
    while (!queue.empty()) {
        Edge e = queue.top();
        queue.pop();
        if (res.find(e.target) != res.end()) {
            continue;
        }

        res[e.target] = vector<RouterID>{e.nextHop, e.nextnextHop, RouterID(e.cost)};
        for (auto i : graph[e.target]) {
            Edge newEdge;
            newEdge.source = source;
            newEdge.target = i.target;
            newEdge.cost = i.cost + e.cost;
            if (e.target == source) {
                newEdge.nextHop = i.target;
                newEdge.nextnextHop = i.target;
            } else if (e.nextHop == e.target) {
                newEdge.nextHop = e.nextHop;
                newEdge.nextnextHop = i.target;
            } else {
                newEdge.nextHop = e.nextHop;
                newEdge.nextnextHop = e.nextnextHop;
            }
            queue.push(newEdge);
        }
    }
    return res;
}

unordered_map<RouterID, RouterID> Graph::calculateMinHopTree(RouterID source) {
    for (auto p : graph) {
        sort(graph[p.first].begin(), graph[p.first].end(), [](Edge e1, Edge e2) -> bool { return e1.target < e2.target; });
    }
    unordered_map<RouterID, RouterID> res;
    res[source] = source;
    vector<RouterID> layer = {source};
    while (layer.size() > 0) {
        vector<RouterID> nextLayer;

        for (auto i : layer) {
            if (graph.find(i) == graph.end()) {
                LOGGER->ERRORF("Graph::addVertex: rid %d doesn't exist in graph.", i);
                continue;
            }
            for (auto e : graph[i]) {
                if (res.find(e.target) != res.end()) {
                    continue;
                }
                nextLayer.push_back(e.target);
                res[e.target] = i;
            }
        }
        layer = nextLayer;
    }
    return res;
}
