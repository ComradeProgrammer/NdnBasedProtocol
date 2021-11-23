#include "Ipv4Address.h"

using namespace std;

void Ipv4Address::setIp(string s) { inet_pton(AF_INET, s.c_str(), (void*)this); }

string Ipv4Address::toString() const{
    char buffer[20];
    inet_ntop(AF_INET, (void*)this, buffer, 20);
    return string(buffer);
}