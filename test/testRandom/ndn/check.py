import json
from functools import cmp_to_key
import sys

from MyTopo import totalNum
import datetime
res=[]
total=totalNum
for i in range(0,total):
    filename="s"+str(i+1)+"_database.json"
    f=open(filename,"r")
    string=f.read()
    f.close()

    data=json.loads(string)
    data["adjLsa"].sort(key=cmp_to_key(lambda x,y:x["routerID"]-y["routerID"]))
    output=json.dumps(data)
    res.append(output)
    
    f=open(filename,"w")
    f.write(output)
    f.close()

difference=0
for i in range(0,total):
    for j in range(i+1,total):
        if res[i]!=res[j]:
            print("s"+str(i+1)+" "+"s"+str(j+1)+" different")
            difference+=1

if difference==0:
    print("identical")
# sumT=0
# maxT=0
# for i in range(0,total):
#     filename="s"+str(i+1)+"_record.log"
#     f=open(filename,"r")
#     f.readline()
#     s=f.readline()
#     if s=="" or s==None:
#         print("s"+str(i+1)+"doesn't have enough routing table items")
#     convergenceTime=int(s)
#     maxT=max(maxT,convergenceTime)
#     sumT+=convergenceTime

# print("convergence Time ",maxT," ms", "avg",sumT/total)
# now_time = datetime.datetime.now().strftime('%Y-%m-%d-%H:%M:%S')
# with open("res/"+now_time+"_convergence.txt","w") as f:
#     if difference!=0:
#         f.write("not identicatl\n")
#     f.write("convergence Time "+str(maxT)+" ms")