
import mininet.cli
from mininet.topo import Topo
from mininet.net import Mininet
from mininet.net import Host
from mininet.util import dumpNodeConnections
from mininet.log import setLogLevel
from NicManager import NicManager
import time
hostNames=["s1","s2"]

class MyTopo(Topo):
    "Single switch connected to n hosts."

    def build(self, n=2):
        switch1 = self.addHost('s1')
        switch2 = self.addHost("s2")
        self.addLink(switch1, switch2)

        # Python's range(N) generates 0..N-1

def run():
    "Create and test a simple network"
    topo = MyTopo(n=4)
    net = Mininet(topo)
    net.start()
    addressManager=NicManager(net)
    addressManager.assignIP()

    processes=[]
    for i in range(0,len(hostNames)):
        s=net.get(hostNames[i])
        process=s.popen(["../../build/testNdn",hostNames[i]])
        processes.append(process)
        print(s,":",process.pid)
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
    time.sleep(60)


    for i in range(0,len(hostNames)):
        s=net.get(hostNames[i])
        print(i,":",processes[i].poll())
        processes[i].kill()

    # net.stop()
topos = {"mytopo": (lambda: MyTopo())}
# sudo mn --custom FourRouterFork.py --topo mytopo
