
from turtle import delay
import mininet.cli
from mininet.topo import Topo
from mininet.net import Mininet
from mininet.net import Host
from mininet.util import dumpNodeConnections
from mininet.log import setLogLevel
from IPy import IP
import time
import os
import re
from mininet.link import TCLink
import subprocess
import datetime
from RouterManager import RouterManager
routerManager=RouterManager()
simulationTime = 60
hostNames = []
edgenum=0
nodenum=50


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
        with open("graph.txt")as f:
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
                params2={ 'ip' : link.ip2+"/24" },
                delay='0ms', loss=0, use_htb=True
            )



def run():
    "Create and test a simple network"
    topo = MyTopo(nodenum)
    net = Mininet(topo, link=TCLink)
    net.start()

    # addressManager = NicManager(net)
    # addressManager.assignIP()
    packetNums=[]
    dataAmounts=[]
    processes = []
    for i in range(0, len(hostNames)):
        s = net.get(hostNames[i])
        p,d=getNICStatistic(s,hostNames[i])
        packetNums.append(p)
        dataAmounts.append(d)
        arg = ["../../build/ndnaddr", "--name", hostNames[i], ]
        s.cmd("./clear.sh")
        if hostNames[i] == "s1":
            arg.append("--root")
            arg.append("--address")
            arg.append("10.1.0.0")
        if hostNames[i] == "s2":
            arg.append("--root")
            arg.append("--address")
            arg.append("10.2.0.0")
        if hostNames[i] == "s3":
            arg.append("--root")
            arg.append("--address")
            arg.append("10.3.0.0")
        if hostNames[i] == "s4":
            arg.append("--root")
            arg.append("--address")
            arg.append("10.4.0.0")
        if hostNames[i] == "s5":
            arg.append("--root")
            arg.append("--address")
            arg.append("10.5.0.0")
        if hostNames[i] == "s6":
            arg.append("--root")
            arg.append("--address")
            arg.append("10.6.0.0")
        if hostNames[i] == "s7":
            arg.append("--root")
            arg.append("--address")
            arg.append("10.7.0.0")
        # if hostNames[i] == "s8":
        #     arg.append("--root")
        #     arg.append("--address")
        #     arg.append("10.8.0.0")
        # if hostNames[i] == "s9":
        #     arg.append("--root")
        #     arg.append("--address")
        #     arg.append("10.9.0.0")
        
        
        process = s.popen(arg)
        processes.append(process)
        print(s, ":", process.pid)
        time.sleep(0.01)
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

    # check the result
    buffer = ""
    for i in range(0, len(hostNames)):
        s = net.get(hostNames[i])
        buffer += s.cmd("ifconfig")
    res = extractIPFromString(buffer)
    print(res)

    truenum=0
    falsenum=0
    nets = set()
    for link in net.links:
        intf1 = str(link.intf1)
        intf2 = str(link.intf2)
        if res[intf1]=="" or res[intf2]=="":
            print(intf1, intf2,  "Invalid")
            falsenum+=1
            continue
        ip1 = IP(res[intf1])
        ip2 = IP(res[intf2])
        correct = (ip1.make_net("255.255.255.252") ==
                   ip2.make_net("255.255.255.252"))
        if correct:
            net = ip1.make_net("255.255.255.252")
            if net in nets:
                correct = False
            nets.add(net)
        print(intf1, ip1, intf2, ip2, correct)
        if correct:
            truenum+=1
        else:
            falsenum+=1
    
    timeuse=0
    print("totalPacket: ",totalPacket, "totalData: ",totalData, "bytes")
    print(truenum,falsenum,truenum/(truenum+falsenum))
    for i in range(1,len(hostNames)+1):
        try:
            with open(str(i)+".txt") as f:
                line=f.readline()
                t=int(line)
                timeuse=max(timeuse,t)
        except:
            pass
    print("totalTime/ms: ",t)
def extractIPFromString(s: str):
    res = {}
    lines = s.splitlines()
    for i in range(0, len(lines)):
        line = lines[i]
        arr = line.split(" ")
        if len(arr) > 1 and arr[1].startswith("flags"):
            intfName = arr[0][0:len(arr[0])-1]
            if intfName == "lo":
                continue
            i += 1
            line = lines[i].strip()
            arr = line.split(" ")
            if len(arr) > 1 and arr[0] == "inet":
                ip = arr[1]
                res[intfName] = ip
            else:
                res[intfName] = ""
    return res
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

topos = {"mytopo": (lambda: MyTopo())}
# sudo mn --custom MyTopo.py --topo mytopo
