
from posixpath import split
import mininet.cli
from mininet.topo import Topo
from mininet.net import Mininet
from mininet.net import Host

from mininet.log import setLogLevel
from RouterManager import RouterManager
import time
import os

import re
import subprocess
import datetime
simulationTime =60
totalNum = 30
hostNames = []
routerManager=RouterManager()
edgenum=0

class MyTopo(Topo):
    "Single switch connected to n hosts."

    def build(self, n=3):
        global edgenum
        switches = {}
        links=[]
        for i in range(0, n):
            hostName = "s"+str(i+1)
            hostNames.append(hostName)
            routerManager.addRouter(hostName)
        with open("../graph.txt")as f:
            while True:
                line=f.readline().strip()
                if not line or line=="":
                    break
                splits=line.split(",")
                src=int(splits[0])
                dst=int(splits[1])
                link=routerManager.addLink(hostNames[src-1],hostNames[dst-1])
                links.append(link)
                edgenum+=1
        for i in range(0, n):

            hostName = "s"+str(i+1)
            router=routerManager.getRouter(hostName)
            switch = self.addHost(hostName,ip=router.nics[0].ip+"/24")
            switches[hostName]=switch
                # switches.append(switch)
        for i in range(0,len(links)):
            link=links[i]
            switcha=switches[link.router1.name]
            switchb=switches[link.router2.name]
            self.addLink(switcha, switchb,
                intfName1=link.nic1.name,
                intfName2=link.nic2.name,
                params1={ 'ip' : link.ip1+"/24" },
                params2={ 'ip' : link.ip2+"/24" }
            )

def getNICStatistic(s,name):
    router=routerManager.getRouter(name)
    totalPackets=0
    totalData=0
    for j in range(0,len(router.nics)):
        nicName=router.nics[j].name
        p=s.popen(["ifconfig "+nicName],stdout=subprocess.PIPE,stderr=subprocess.PIPE, shell=True)

        out = p.stdout.read().decode('utf-8')
        
        res = re.search(r'TX packets (\d*)  bytes (\d*)', out)
        packets=int(res.group(1))
        amount=int(res.group(2))
        totalData+=amount
        totalPackets+=packets
    return (totalPackets,totalData)


def run():
    "Create and test a simple network"
    topo = MyTopo(totalNum)
    net = Mininet(topo)
    net.start()

    processes = []
    packetNums=[]
    dataAmounts=[]
    for i in range(0, len(hostNames)):
        s = net.get(hostNames[i])
        p,d=getNICStatistic(s,hostNames[i])
        packetNums.append(p)
        dataAmounts.append(d)
        process = s.popen(["../../../build/routing", "--name",
                          hostNames[i], "--simulationTime", str(simulationTime-5),"--password","aaaaa", ])
        s.popen(["../../../build/convergenceTestSuit",str(edgenum),hostNames[i]+"_record.log"])
        processes.append(process)
        print(s, ":", process.pid)
        time.sleep(0.01)

    # s1, s2,s3,s4,s5 = net.get("s1", "s2","s3","s4","s5")
    # s1,s2=net.get("s1","s2")

    # process1 = s1.popen(["../../build/testNdn", "s1"])
    # print("s1:",process1.pid)
    # time.sleep(0.01)
    # process2 = s2.popen(["../../build/testNdn", "s2"])
    # print("s2:",process2.pid)

    # process3 = s3.popen(["../../build/ndnRoutingTest", "s3"])
    # print("s3:",process3.pid)
    # process4 = s4.popen(["../../build/ndnRoutingTest", "s4"])
    # print("s4:",process4.pid)
    # process5 = s5.popen(["../../build/ndnRoutingTest", "s5"])
    # print("s5:",process5.pid)
    # time.sleep(20)

    # l=net.linksBetween(s1,s2)
    # net.configLinkStatus("s1","s2","down")
    # time.sleep(10)
    # net.configLinkStatus("s1","s2","up")
    # time.sleep(10)
    # net.configLinkStatus("s1","s2","down")
    # time.sleep(10)
    # net.configLinkStatus("s1","s2","up")
    time.sleep(simulationTime)
    
    for i in range(0, len(hostNames)):
        s = net.get(hostNames[i])
        print(hostNames[i], ":", processes[i].poll())
        processes[i].kill()
    totalPacket=0
    totalData=0
    for i in range(0, len(hostNames)):
        s = net.get(hostNames[i])
        p,d=getNICStatistic(s,hostNames[i])

        packetNums[i]=p-packetNums[i]
        dataAmounts[i]=d-dataAmounts[i]
        totalData+=dataAmounts[i]
        totalPacket+=packetNums[i]
    print("totalPacket: ",totalPacket, "totalData: ",totalData, "bytes")
    now_time = datetime.datetime.now().strftime('%Y-%m-%d-%H:%M:%S')
    with open("res/"+now_time+"_datastatitic.txt","w") as f:
        f.write("totalPacket: "+str(totalPacket)+ " totalData: "+str(totalData)+ " bytes")
    

    # net.stop()
topos = {"mytopo": (lambda: MyTopo())}
# sudo mn --custom MyTopo.py --topo mytopo
