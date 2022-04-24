#include <gtest/gtest.h>

#include "util/traceback/traceback.h"
// remove property qualifier
#define private public
#define protected public
// start to include here
#include "Graph.h"
#define VERTEX 100
#define EDGE 800
using namespace std;
// for standard answer
struct edge {
    int source;
    int target;
    int weight;

    bool operator<(const edge& e) const { return weight >= e.weight; }
};

class GraphTest : public ::testing::Test {
   protected:
    int union_set[VERTEX + 1];
    vector<Edge> edges;
    vector<int> standardRes;

    int findSet(int x) {
        if (union_set[x] == x) {
            return x;
        }
        return union_set[x] = findSet(union_set[x]);
    }

    void SetUp() override {
        struct timeval tm;

        gettimeofday(&tm, NULL);
        int seed = tm.tv_sec * 1000 + tm.tv_usec / 1000;
        srand(seed);
        // generate a graph
        for (int i = 1; i <= VERTEX; i++) {
            union_set[i] = i;
        }
        // genearte edges
        for (int i = 0; i < EDGE; i++) {
            int from = rand() % VERTEX + 1;
            int end = rand() % VERTEX + 1;
            int cost = rand() % 50;
            edges.push_back({from, end, cost, 0, 0});
            int tmp1 = findSet(from);
            int tmp2 = findSet(end);
            union_set[tmp1] = tmp2;
        }
        // make sure the graph is connected
        int tmp = findSet(1);
        for (int i = 2; i <= VERTEX; i++) {
            int tmp2 = findSet(i);
            if (tmp2 != tmp) {
                int cost = rand() % 50;
                edges.push_back({1, i, cost, 0, 0});
                edges.push_back({i, 1, cost, 0, 0});
                union_set[tmp2] = tmp;
            }
        }

        standardTest();
    }

    void standardTest() {
        // standard test passed the tester of luogu
        int n, m, s;
        n = VERTEX;
        s = 1;
        vector<vector<edge>> graph(n + 1);
        for (int i = 0; i < edges.size(); i++) {
            int u, v, w;
            u = edges[i].source;
            v = edges[i].target;
            w = edges[i].cost;
            graph[u].push_back({u, v, w});
            graph[v].push_back({v, u, w});
        }
        standardRes = vector<int>(n + 1, 0x7fffffff);
        priority_queue<edge> q;
        q.push({s, s, 0});
        while (q.size() > 0) {
            edge tmp = q.top();
            q.pop();
            if (standardRes[tmp.target] > tmp.weight) {
                standardRes[tmp.target] = tmp.weight;
                for (edge newedge : graph[tmp.target]) {
                    q.push({s, newedge.target, standardRes[tmp.target] + newedge.weight});
                }
            }
        }
    }
};

TEST_F(GraphTest, randomTestCost) {
    Graph g;
    for (int i = 1; i <= VERTEX; i++) {
        g.addVertex(i);
    }

    for (auto edge : edges) {
        g.addEdge(edge.source, edge.target, edge.cost);
        g.addEdge(edge.target, edge.source, edge.cost);
    }
    auto res = g.calculateShortestPath(1);
    for (int i = 2; i <= VERTEX; i++) {
        ASSERT_EQ(res[i][2], standardRes[i]);
    }
    for (int i = 2; i <= VERTEX; i++) {
        int nextnexthop = res[i][1];
        ASSERT_EQ(res[nextnexthop][0], res[i][0]);
        ASSERT_NE(res[nextnexthop][0], 0);
    }
    // cout<<4<<endl;
}
TEST_F(GraphTest, randomTestMinHopTree) {
    /*
            6 - 7
            |
        1 - 2 - 3
            |
            4 - 5
     */

    Graph g;
    for (int i = 1; i <= 7; i++) {
        g.addVertex(i);
    }
    g.addEdge(1, 2, 1);
    g.addEdge(2, 3, 1);
    g.addEdge(2, 4, 1);
    g.addEdge(2, 6, 1);
    g.addEdge(6, 7, 1);
    g.addEdge(4, 5, 1);

    g.addEdge(2, 1, 1);
    g.addEdge(3, 2, 1);
    g.addEdge(4, 2, 1);
    g.addEdge(6, 2, 1);
    g.addEdge(7, 6, 1);
    g.addEdge(5, 4, 1);

    g.addEdge(4, 3, 1);
    g.addEdge(3, 4, 1);

    auto res = g.calculateMinHopTree(1);
    ASSERT_EQ(res[1], 1);
    ASSERT_EQ(res[2], 1);
    ASSERT_EQ(res[6], 2);
    ASSERT_EQ(res[4], 2);
    ASSERT_EQ(res[7], 6);
    ASSERT_EQ(res[5], 4);
}
