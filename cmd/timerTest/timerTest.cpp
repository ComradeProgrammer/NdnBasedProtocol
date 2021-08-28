#include <unistd.h>

#include "util/timer/Timer.h"
using namespace std;
int main() {
    Timer timer;
    timer.startTimer("test1", 1000, [](string name) -> bool {
        cout << name << endl;
        return true;
    });

    sleep(5);
    cout << "cancel" << endl;
    timer.cancelTimer("test1");
    sleep(2);
    cout << "restart" << endl;
    timer.startTimer("test1", 1000, [](string name) -> bool {
        cout << name << endl;
        return true;
    });
    sleep(5);
    return 0;
}