#include<iostream>
#include<cstdlib>
#include<regex>
#include"util/cmd/cmd.h"
#include"ip/RoutingTable.h"
using namespace std;
int main(){
    // int ret=system("route add -net 1.1.1.0/24 gw 192.168.50.1");
    // cout<<ret<<endl;
    // cout<<R"(FUCK)"<<endl;
    //  string info="1.1.1.1         192.168.50.1    255.255.255.0         UG    100    0        0 enp0s31f6";
    //  regex pattern("^\\s*([0-9.]+)\\s+([0-9.]+)\\s+([0-9.]+)\\s+\\w+\\s+\\w+\\s+\\w+\\s+\\w+\\s+\\w+\\s*$");
    // smatch result;
    // if(!regex_search(info,result,pattern)){
    //     cout<<"failed"<<endl;
    //     return 0;
    // }
    // cout<<"passed"<<endl;
    //cout<<runCmd("route -n").second<<endl;
    auto table=RoutingTable::getRoutingTable();
    Ipv4Address destination;
    destination.setIp("1.1.1.1");
    Ipv4Address mask;
    mask.setIp("255.255.255.0");
    Ipv4Address nextHop;
    nextHop.setIp("127.0.0.1");

    RoutingTableItem item(destination,mask,nextHop);
    table->addRoutingTableItem(item);

    cout<<runCmd("route -n").second<<endl;
    cout<<"======================"<<endl;
    table->deleteRoutingTableItem(item);
    cout<<runCmd("route -n").second<<endl;


}