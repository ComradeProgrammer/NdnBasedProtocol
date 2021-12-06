
import mininet.cli
from mininet.topo import Topo
from mininet.net import Mininet
from mininet.net import Host
from mininet.util import dumpNodeConnections
from mininet.log import setLogLevel
from NicManager import NicManager
import time
hostNames=["s1","s2","s3","s4","s5","s6","s7","s8","s9"]

class MyTopo(Topo):
    "Single switch connected to n hosts."

    def build(self, n=2):
        switch1 = self.addHost('s1')
        switch2 = self.addHost("s2")
        switch3 = self.addHost('s3')
        switch4 = self.addHost('s4')
        switch5 = self.addHost('s5')

        switch6 = self.addHost('s6')
        switch7 = self.addHost("s7")
        switch8 = self.addHost('s8')
        switch9 = self.addHost('s9')


        self.addLink(switch1, switch2)
        self.addLink(switch2, switch3)
        self.addLink(switch3, switch1)

        self.addLink(switch4, switch5)
        self.addLink(switch5, switch6)
        self.addLink(switch6, switch4)

        self.addLink(switch7, switch8)
        self.addLink(switch8, switch9)
        self.addLink(switch9, switch7)

        self.addLink(switch3, switch5)
        self.addLink(switch5, switch7)
        self.addLink(switch7, switch9)





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
        process=s.popen(["../../build/ndnRoutingTest", s])
        processes.append(process)
        print(s,":",process.pid)


    # s1, s2,s3,s4,s5 = net.get("s1", "s2","s3","s4","s5")
    # #s1,s2=net.get("s1","s2")

    # process1 = s1.popen(["../../build/ndnRoutingTest", "s1"])
    # print("s1:",process1.pid)
    # time.sleep(0.01)
    # process2 = s2.popen(["../../build/ndnRoutingTest", "s2"])
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

    time.sleep(20)
    # net.configLinkStatus("s1","s2","up")
    time.sleep(60)
    # print(process1.poll())
    # print(process2.poll())
    # print(process3.poll())
    # print(process4.poll())
    # print(process5.poll())
    # process1.kill()
    # process2.kill()
    # process3.kill()
    # process4.kill()
    # process5.kill()
    #for i in range()


    # net.stop()
topos = {"mytopo": (lambda: MyTopo())}
# sudo mn --custom FourRouterFork.py --topo mytopo
