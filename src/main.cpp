#include <fstream>
#include <iostream>
#include<pthread.h>
#include<semaphore.h>
#include "ethernet/RawSocket.h"
#include "util/log/FileLogger.h"
using namespace std;

sem_t semaphore;
shared_ptr<EthernetPacket> packet;
void* receiveThread(void*){
    RawSocket sock;


    auto p=sock.receivePacket(0xffff);
    packet=p.second;
    sem_post(&semaphore);
    return nullptr;
}


void* sendThread(void*){
    RawSocket sock;
    EthernetHeader header;
    header.setSourceMacAddress(MacAddress("fa:33:42:5c:9e:23"));
    header.setDestionationMacAddress(MacAddress("ff:ff:ff:ff:ff:ff"));
    header.setType(0xffff);
    char data[]="shit";
    shared_ptr<EthernetPacket> packet=make_shared<EthernetPacket>(header,data,5);
    sock.sendPacket(2,packet);
    return nullptr;
}


int main() {
    sem_init(&semaphore,0,0);
    pthread_t p1,p2;
    int ret=pthread_create(&p1,nullptr, receiveThread,nullptr);
    if(ret!=0){
        cout<<"error";
    }
     int ret2=pthread_create(&p2,nullptr,sendThread,nullptr);
     if(ret2!=0){
         cout<<"error";
     }
    sem_wait(&semaphore);
    cout<<packet->getData();
    

}