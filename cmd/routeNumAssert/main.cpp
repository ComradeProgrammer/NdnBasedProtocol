#include <iostream>

#include "util/cmd/cmd.h"
#include "util/util.h"
using namespace std;
int main(int argc, char* argv[]) {
    long startTime = getTimeStamp();
    int assertNum = 0;
    if (argc > 1) {
        assertNum = atoi(argv[1]);
    }

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
        if(count==assertNum){
            break;
        }
    }

    long endTime = getTimeStamp();
    cout << endTime - startTime << "ms" << endl;

}
