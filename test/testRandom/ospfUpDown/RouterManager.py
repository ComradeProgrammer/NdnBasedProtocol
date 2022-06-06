
class Router:
    def __init__(self, name: str):
        self.name = name
        self.nics = []

    def generateConfigurationFile(self):
        res = "hostname %s_ospfd\n" % (self.name)
        res += "router ospf\n"
        for i in range(0, len(self.nics)):
            res += "  network %s/24 area 0\n" % (self.nics[i].ip)
        for i in range(0, len(self.nics)):
            res += "interface %s\n" % (self.nics[i].name)
            res += "  ip ospf network point-to-point\n"
        res+="debug ospf event\n"
        res+="log file %s.log"%(self.name)

        with open(self.name+".conf","w") as f:
            f.write(res)
        

class Nic:
    def __init__(self, name: str):
        self.name = name
        self.ip = ""


class Link:
    def __init__(self):
        self.router1 = None
        self.router2 = None
        self.nic1 = None
        self.nic2 = None
        self.ip1 = ""
        self.ip2 = ""


class RouterManager:
    def __init__(self):
        self.routers = {}
        self.links = []
        self.counter = 1

    def addRouter(self, name: str):
        newRouter = Router(name)
        self.routers[name] = newRouter

    def getRouter(self, name: str):
        return self.routers[name]

    def addLink(self, name1: str, name2: str):
        if not name1 in self.routers:
            raise Exception("router %s not found" % (name1))
        if not name2 in self.routers:
            raise Exception("router %s not found" % (name2))

        router1 = self.routers[name1]
        router2 = self.routers[name2]

        newLink = Link()
        newLink.router1 = router1
        newLink.router2 = router2
        newLink.ip1 = "10.%d.%d.1" % (self.counter//256, self.counter % 256)
        newLink.ip2 = "10.%d.%d.2" % (self.counter//256, self.counter % 256)

        nic1 = Nic(router1.name+"eth"+str(self.counter))
        nic2 = Nic(router2.name+"eth"+str(self.counter))

        nic1.ip = newLink.ip1
        nic2.ip = newLink.ip2

        newLink.nic1 = nic1
        newLink.nic2 = nic2

        router1.nics.append(nic1)
        router2.nics.append(nic2)

        self.links.append(newLink)

        self.counter += 1
        return newLink
    def generateConfigurationFile(self):
        for i in self.routers.keys():
            self.routers[i].generateConfigurationFile()
