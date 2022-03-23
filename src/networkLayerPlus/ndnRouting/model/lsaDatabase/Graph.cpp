#include "Graph.h"
using namespace std;
void Graph::addVertex(int rid) {
    if (graph.find(rid) == graph.end()) {
        graph[rid] = vector<Edge>();
    }
    // } else {
    //     LOGGER->WARNINGF("Graph::addVertex: rid %d exists.", rid);
    // }
}

void Graph::addEdge(int ridSource, int ridTarget, int cost) {
    if (graph.find(ridSource) == graph.end()) {
        addVertex(ridSource);
    }
    if (graph.find(ridTarget) == graph.end()) {
        addVertex(ridTarget);
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

void Graph::removeVertex(int rid) {
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

void Graph::removeEdge(int rid1, int rid2) {
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

bool Graph::isBidirectionalEdge(int rid1, int rid2) {
    if (graph.find(rid1) == graph.end()) {
        return false;
    }
    if (graph.find(rid2) == graph.end()) {
        return false;
    }

    bool find1 = false;
    for (int i = 0; i < graph[rid1].size(); i++) {
        if (graph[rid1][i].target == rid2) {
            find1 = true;
            break;
        }
    }
    bool find2 = false;

    for (int i = 0; i < graph[rid2].size(); i++) {
        if (graph[rid2][i].target == rid1) {
            find2 = true;
            break;
        }
    }
    return find1 && find2;
}

vector<int> Graph::getAllVertices() {
    vector<int> res;
    for (auto p : graph) {
        res.push_back(p.first);
    }
    return res;
}

unordered_map<int, vector<int>> Graph::calculateShortestPath(int source) {
    // target->[nexthop,nextnexthop,cost]
    unordered_map<int, vector<int>> res;
    priority_queue<Edge, vector<Edge>, greater<Edge>> queue;
    queue.push({source, source, 0, source, source});
    while (!queue.empty()) {
        cout<<queue.size()<<endl;
       // cout<<"here"<<endl;
        Edge e = queue.top();
        queue.pop();
        if (res.find(e.target) != res.end()) {
            continue;
        }
        res[e.target] = vector<int>{e.nextHop, e.nextnextHop, int(e.cost)};
        for (auto i : graph[e.target]) {
            // if (!isBidirectionalEdge(i.source, i.target)) {
            //     continue;
            // }

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
    if(res.find(source)!=res.end()){
        res.erase(res.find(source));
    }
    return res;
}

unordered_map<int, int> Graph::calculateMinHopTree(int source) {
    for (auto p : graph) {
        sort(graph[p.first].begin(), graph[p.first].end(), [](Edge e1, Edge e2) -> bool { return e1.target < e2.target; });
    }
    unordered_map<int, int> res;
    res[source] = source;
    vector<int> layer = {source};
    while (layer.size() > 0) {
        vector<int> nextLayer;

        for (auto i : layer) {
            if (graph.find(i) == graph.end()) {
                LOGGER->ERRORF("Graph::addVertex: rid %d doesn't exist in graph.", i);
                continue;
            }
            for (auto e : graph[i]) {
                // if (!isBidirectionalEdge(e.source, e.target)) {
                //     continue;
                // }
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
