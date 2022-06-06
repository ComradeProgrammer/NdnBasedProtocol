from random import randint


import random
nodeNum=75
edgeNum=nodeNum*nodeNum//8
print(nodeNum," ",edgeNum)
graph=[]
edges=[]

disjointSet=[]
def findset(x):
    global disjointSet
    if disjointSet[x]!=x:
        disjointSet[x]=findset(disjointSet[x])
    return disjointSet[x]

#构造点
for i in range(0,nodeNum):
    tmp=[]
    for j in range(0,nodeNum):
        tmp.append(0)
    graph.append(tmp)
    disjointSet.append(i)
#构造边
while len(edges)<edgeNum:
    src=random.randint(0,nodeNum-1)
    dst=random.randint(0,nodeNum-1)
    if dst<=src:
        continue
    if  graph[src][dst]!=0:
        continue
    edges.append((src,dst))
    graph[src][dst]=1
    if findset(src)!=findset(dst):
        disjointSet[findset(src)]=disjointSet[findset(dst)]
if graph[0][1]==0:
    graph[0][1]=1
    edges.append((0,1))
    if findset(0)!=findset(1):
        disjointSet[findset(0)]=disjointSet[findset(1)]


#确保连通性

align=findset(0)
for i in range(1,nodeNum):
    tmp=findset(i)
    if findset(i)!=align:
        edges.append((align,findset(i)))
        disjointSet[i]=align
with open("graph.txt","w") as f:
    for i in range(0,len(edges)):

        f.writelines("%d,%d\n"%(edges[i][0]+1,edges[i][1]+1))   
