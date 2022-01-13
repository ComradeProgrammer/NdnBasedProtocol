from mininet.net import Mininet
from typing import List
class NicManager:
    def __init__(self,net:Mininet):
        self.net=net
        self.hostToInterfaces={}
        self.connections=[]
    def assignIP(self):
        i=0
        #currently assume that only router-to-router links, no broadcast links
        for link in self.net.links:
            link.intf1.setIP("192.168.%d.1"%(i),24)
            link.intf2.setIP("192.168.%d.2"%(i),24)
            i+=1
            #record the information
            self.connections.append((str(link.intf1),str(link.intf2)))
            host1=link.intf1.node.name
            if not host1 in self.hostToInterfaces:
                self.hostToInterfaces[host1]=[(str(link.intf1),link.intf1.IP())]
            else:
                self.hostToInterfaces[host1].append((str(link.intf1),link.intf1.IP()))
            host2=link.intf2.node.name
            if not host2 in self.hostToInterfaces:
                self.hostToInterfaces[host2]=[(str(link.intf2),link.intf2.IP())]
            else:
                self.hostToInterfaces[host2].append((str(link.intf2),link.intf2.IP()))
        f = open("connection.log", "w")
        f.writelines("\n===connections===\n")
        f.write(str(self.connections))
        f.writelines("\n===hosts===\n")
        for k in self.hostToInterfaces:
            v=self.hostToInterfaces[k]
            f.write("%s: %s\n"%(k,str(v)))
        f.close()


