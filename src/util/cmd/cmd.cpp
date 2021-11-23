#include"cmd.h"
using namespace std;
pair<int,string> runCmd(string command){
    char buffer[1024];
    stringstream ss;
    FILE* pipe=popen(command.c_str(),"r");
    if(pipe==nullptr){
        //error
        return {-1,""};
    }
    while(fgets(buffer, 1024, pipe)!=NULL){
        ss<<buffer;
    }
    int ret=pclose(pipe);
    return {ret,ss.str()};

}