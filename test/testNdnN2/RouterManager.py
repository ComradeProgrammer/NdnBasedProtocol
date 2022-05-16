
class Router:
    def __init__(self, name: str):
        self.name = name
        self.nics = []


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
        self.counter=1

    def addRouter(self, name: str):
        newRouter = Router(name)
        self.routers[name] = newRouter

    def getRouter(self,name:str):
        return self.routers[name]

    def addLink(self, name1:str, name2:str):
        if not name1 in self.routers:
            raise Exception("router %s not found" % (name1))
        if not name2 in self.routers:
            raise Exception("router %s not found" % (name2))

        router1 = self.routers[name1]
        router2 = self.routers[name2]

        newLink = Link()
        newLink.router1=router1
        newLink.router2=router2
        newLink.ip1="10.%d.%d.1" % (self.counter//256, self.counter % 256)
        newLink.ip2="10.%d.%d.2" % (self.counter//256, self.counter % 256)

        nic1=Nic(router1.name+"eth"+str(self.counter))
        nic2=Nic(router2.name+"eth"+str(self.counter))

        nic1.ip=newLink.ip1
        nic2.ip=newLink.ip2

        newLink.nic1=nic1
        newLink.nic2=nic2

        router1.nics.append(nic1)
        router2.nics.append(nic2)

        self.links.append(newLink)
        
        self.counter+=1
        return newLink
