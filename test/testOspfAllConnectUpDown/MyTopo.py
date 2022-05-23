
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


simulationTime = 60
totalNum=14

hostNames = []
routerManager=RouterManager()


class MyTopo(Topo):
    "Single switch connected to n hosts."

    def build(self, n=3):
        switches = {}
        links=[]
        for i in range(0, n):
            hostName = "s"+str(i+1)
            hostNames.append(hostName)
            routerManager.addRouter(hostName)
        for i in range(0, n):
            for j in range(i+1, n):
                link=routerManager.addLink(hostNames[i],hostNames[j])
                links.append(link)



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
        routerManager.generateConfigurationFile()
    

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
    net.configLinkStatus("s1","s2","down")

    processes = []
    packetNums=[]
    dataAmounts=[]
    for i in range(0, len(hostNames)):
        s = net.get(hostNames[i])
        # p,d=getNICStatistic(s,hostNames[i])
        # packetNums.append(p)
        # dataAmounts.append(d)
        

        s.cmd("zebra -d -z /tmp/%szebra.api -i /tmp/%szebra.interface"%(hostNames[i],hostNames[i]))
        s.cmd("ospfd -f %s.conf -d -z /tmp/%szebra.api -i /tmp/%sospfd.interface"%(hostNames[i],hostNames[i],hostNames[i]))
        # s.popen(["../../build/convergenceTestSuit",str((totalNum-1)*totalNum//2),hostNames[i]+"_record.log"])
        time.sleep(0.01)

    time.sleep(30)
    net.configLinkStatus("s1","s2","up")
    for i in range(0, len(hostNames)):
        s = net.get(hostNames[i])
        p,d=getNICStatistic(s,hostNames[i])
        packetNums.append(p)
        dataAmounts.append(d)
        

        # s.cmd("zebra -d -z /tmp/%szebra.api -i /tmp/%szebra.interface"%(hostNames[i],hostNames[i]))
        # s.cmd("ospfd -f %s.conf -d -z /tmp/%szebra.api -i /tmp/%sospfd.interface"%(hostNames[i],hostNames[i],hostNames[i]))
        s.popen(["../../build/convergenceTestSuit",str((totalNum-1)*totalNum//2),hostNames[i]+"_record.log"])
        time.sleep(0.01)
    
    time.sleep(30)
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
