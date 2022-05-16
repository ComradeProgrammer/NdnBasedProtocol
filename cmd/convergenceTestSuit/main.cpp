#include <unistd.h>

#include <iostream>
#include <fstream>
#include "util/cmd/cmd.h"
#include "util/util.h"

using namespace std;
struct Result {
    long time;
};

Result record(int assertNum) {
    long startTime = getTimeStamp();

    while (1) {
        auto res = runCmd("route -n");
        auto lines = split(res.second, "\n");
        int count = 0;
        // remove the first line and second line
        for (int i = 2; i < lines.size(); i++) {
            if (lines[i] == "") {
                continue;
            }
            count++;
        }
        if (count == assertNum) {
            break;
        }
    }

    long endTime = getTimeStamp();
    Result res;
    res.time=endTime-startTime;
    return res;
}

// commandline argument: assertNum ouputFileName
int main(int argc, char* argv[]) {
    int assertNum = 0;
    if (argc > 1) {
        assertNum = atoi(argv[1]);
    }

    pid_t pid=fork();  
    if(pid!=0){
        exit(0);
    }else{
        fstream out(argv[2],ios::out);
        out<<"start"<<endl;
        Result res=record(assertNum);
        out<<res.time<<endl;
        out.close();
    }
}
