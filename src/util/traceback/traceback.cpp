#include "traceback.h"
using namespace std;

using namespace std;

static string addr2linePath = "/usr/bin/addr2line";
static int depth = 10;
static function<void(std::string)> signalTracebackHandler = nullptr;

/**
 * @brief return the stack information which has NOT been proceeded by addr2line
 * 
 * @return std::vector<string>, each element is information of a layer of stack
 */
std::vector<std::string> rawStacks() {
    void** stackBinary = new void*[depth];
    int stackNum = backtrace(stackBinary, depth);
    char** stackName = backtrace_symbols(stackBinary, stackNum);

    vector<string> res;
    for (int i = 0; i < stackNum; ++i) {
        res.push_back(string(stackName[i]));
    }
    free(stackName);
    delete[] stackBinary;
    return res;
}

/**
 * @brief return the stack information which has been proceeded by addr2line
 * 
 * @return std::vector<string>, each element is information of a layer of stack
 */
std::vector<string> stacks() {
  vector<string> res;
  auto rawStack = rawStacks();
  regex pattern("(.*)\\((.*)\\)\\s*\\[(.*)\\]\\s*");
  smatch matchResult;
  for (string s : rawStack) {
    if (!regex_match(s, matchResult, pattern)) {
      res.push_back(string("unrecognized line ") + s);
    } else {
      auto output = runCmd(addr2linePath + " -e " + matchResult[1].str() + " " +
                           matchResult[2].str().c_str()+" -f -C -p");
      if (output.first != 0) {
        res.push_back(string("unrecognized line ") + s);
      } else {
        string line = output.second;
        res.push_back(line.replace(line.rfind("\n"), 1, ""));
      }
    }
  }
  return res;
}


/**
 * @brief this handler will be called when SIGSEGV or SIGFPE is received
 * 
 * @param sig No. of signal
 */
void signalHandler(int sig) {
    if (sig == SIGSEGV) {
        signalTracebackHandler("SIGSEGV\n" + traceBack());
    } else if (sig == SIGFPE) {
        signalTracebackHandler("SIGFPE\n" + traceBack());
    }
    exit(-1);
}
void setMaxTraceBackDepth(int maxDepth) { depth = maxDepth; }
void setAddr2linePath(string pathToAddr2line) { addr2linePath = pathToAddr2line; }
void initSignalTraceback(function<void(std::string)> handler) {
    signalTracebackHandler = handler;
    signal(SIGSEGV, signalHandler);
    signal(SIGFPE, signalHandler);
}
string traceBack() {
    vector<string>tmp=stacks();
    stringstream ss;
    ss<<"Traceback:"<<endl;
    //starts from 4th line because first 3 lines are functions in this file.
    for(int i=4;i<tmp.size();i++){
        ss<<"\t"<<tmp[i]<<endl;
    }
    return ss.str();
}