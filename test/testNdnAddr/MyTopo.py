
import mininet.cli
from mininet.topo import Topo
from mininet.net import Mininet
from mininet.net import Host
from mininet.util import dumpNodeConnections
from mininet.log import setLogLevel
from IPy import IP
import time
import os

simulationTime = 60
hostNames = ["s3", "s4","s1", "s2", "s5", "s6", "s7", "s8", "s9"]


class MyTopo(Topo):
    "Single switch connected to n hosts."

    def build(self, n=2):
        switch1 = self.addHost('s1')
        switch2 = self.addHost("s2")
        switch3 = self.addHost("s3")
        switch4 = self.addHost("s4")
        switch5 = self.addHost("s5")
        switch6 = self.addHost("s6")
        switch7 = self.addHost("s7")
        switch8 = self.addHost("s8")
        switch9 = self.addHost("s9")

        self.addLink(switch1, switch2)
        self.addLink(switch2, switch3)
        self.addLink(switch4, switch5)
        self.addLink(switch5, switch6)
        self.addLink(switch7, switch8)
        self.addLink(switch8, switch9)

        self.addLink(switch1, switch4)
        self.addLink(switch4, switch7)
        self.addLink(switch2, switch5)
        self.addLink(switch5, switch8)
        self.addLink(switch3, switch6)
        self.addLink(switch6, switch9)


def run():
    "Create and test a simple network"
    topo = MyTopo()
    net = Mininet(topo)
    net.start()

    # addressManager = NicManager(net)
    # addressManager.assignIP()

    processes = []
    for i in range(0, len(hostNames)):
        s = net.get(hostNames[i])
        arg = ["../../build/ndnaddr", "--name", hostNames[i], ]
        if hostNames[i] == "s3":
            arg.append("--root")
            arg.append("--address")
            arg.append("10.1.0.0")
        if hostNames[i] == "s4":
            arg.append("--root")
            arg.append("--address")
            arg.append("10.2.0.0")
        if hostNames[i] == "s9":
            arg.append("--root")
            arg.append("--address")
            arg.append("10.3.0.0")

        process = s.popen(arg)
        processes.append(process)
        print(s, ":", process.pid)
        #time.sleep(3)
    time.sleep(simulationTime)

    for i in range(0, len(hostNames)):
        s = net.get(hostNames[i])
        print(hostNames[i], ":", processes[i].poll())
        processes[i].kill()

    # check the result
    buffer = ""
    for i in range(0, len(hostNames)):
        s = net.get(hostNames[i])
        buffer += s.cmd("ifconfig")
    res = extractIPFromString(buffer)
    print(res)

    nets = set()
    for link in net.links:
        intf1 = str(link.intf1)
        intf2 = str(link.intf2)
        if res[intf1].startswith("10.0"):
            res[intf1]=""
        if res[intf2].startswith("10.0"):
            res[intf2]=""
        if res[intf1]=="" or res[intf2]=="":
            correct = False
            print(intf1,res[intf1], intf2, res[intf2], correct)
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


topos = {"mytopo": (lambda: MyTopo())}
# sudo mn --custom MyTopo.py --topo mytopo
