#include"src/util/FileLogger.h"
#include<iostream>
#include<fstream>
using namespace std;
int main(){
    FileLogger l("test.txt");
    l.error("test",74,"");
}