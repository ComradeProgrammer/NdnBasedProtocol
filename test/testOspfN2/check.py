import json
from functools import cmp_to_key
import sys
import datetime

from MyTopo import edgeLenghth

res=[]
total=edgeLenghth*edgeLenghth

maxT=0
for i in range(0,total):
    filename="s"+str(i+1)+"_record.log"
    f=open(filename,"r")
    f.readline()
    s=f.readline()
    if s=="" or s==None:
        print("s"+str(i+1)+"doesn't have enough routing table items")
    convergenceTime=int(s)
    maxT=max(maxT,convergenceTime)

print("convergence Time ",maxT," ms")
now_time = datetime.datetime.now().strftime('%Y-%m-%d-%H:%M:%S')
with open("res/"+now_time+"_convergence.txt","w") as f:
    f.write("convergence Time "+str(maxT)+" ms")