#include <gtest/gtest.h>
// hack complier when test
#define private public
#define protected public
// start to include here
#include"Graph.h"
#define VERTEX 100
#define EDGE 800
using namespace std;
//for standard answer
struct edge{
    int source;
    int target;
    int weight;


    bool operator<(const edge& e)const{
        return weight>=e.weight;
    }
};

class GraphTest : public ::testing::Test{
    protected:
    int union_set[VERTEX];
    vector<Edge>edges;
    vector<int>standardRes;
    
    int findSet(int x){
        if(union_set[x]==x){
            return x;
        }
        return union_set[x]=findSet(union_set[x]);
    }

    void SetUp() override {
        struct timeval tm;

        gettimeofday(&tm, NULL);
        int seed = tm.tv_sec * 1000 + tm.tv_usec / 1000;
        srand(seed);
        //generate a graph
        for(int i=0;i<VERTEX;i++){
            union_set[i]=i;
        }
        //genearte edges
        for(int i=0;i<EDGE;i++){
            uint32_t from=rand()%VERTEX;
            uint32_t end=rand()%VERTEX;
            int cost=rand()%50;
            edges.push_back({from,end,cost,0});
            int tmp1=findSet(from);
            int tmp2=findSet(end);
            union_set[tmp1]=tmp2;
        }
        //make sure the graph is connected
        int tmp=findSet(0);
        for(int i=1;i<VERTEX;i++){
            int tmp2=findSet(i);
            if(tmp2!=tmp){
                int cost=rand()%50;
                edges.push_back({0,(uint32_t)i,cost,0});
                edges.push_back({(uint32_t)i,0,cost,0});
                union_set[tmp2]=tmp;
            }
        }
        standardTest();
    }

    void standardTest(){
        //standard test passed the tester of luogu
        int n,m,s;
        n=VERTEX;
        s=0;
        vector<vector<edge>>graph(n+1);
        for(int i=0;i<edges.size();i++){
            int u,v,w;
            u=edges[i].source;
            v=edges[i].target;
            w=edges[i].cost;
            graph[u].push_back({u,v,w});
            graph[v].push_back({v,u,w});
        }
        standardRes=vector<int>(n+1,0x7fffffff);
        priority_queue<edge> q;
        q.push({s,s,0});
        while(q.size()>0){
            edge tmp=q.top();
            q.pop();
            if(standardRes[tmp.target]>tmp.weight){
                standardRes[tmp.target]=tmp.weight;
                for(edge newedge:graph[tmp.target]){
                    q.push({s,newedge.target,standardRes[tmp.target]+newedge.weight});
                }
            }

        }
    }

};

TEST_F(GraphTest, randomTestCost){
    Graph g;
    for(int i=0;i<VERTEX;i++){
        g.addVertex(i);
    }
    for(auto edge:edges){
        g.addEdge(edge.source,edge.target,edge.cost);
        g.addEdge(edge.target,edge.source,edge.cost);
    }
    g.calculate(0);
    for(int i=0;i<VERTEX;i++){
        ASSERT_EQ(g.res[i].second,standardRes[i]);
    }
}
