#ifndef __TRACEBACK_H_
#define __TRACEBACK_H_
#include <execinfo.h>
#include <signal.h>

#include <exception>
#include <functional>
#include <iostream>
#include <regex>
#include <sstream>
#include <string>
#include <vector>

#include "util/cmd/cmd.h"

/**
 * @brief start to receive SIGSEGV or SIGFPE, abd set the handler to deal with the traceback message when the program received a SIGSEGV or SIGFPE. THIS
 * FUNCTION SHOULD BE CALLED IN MAIN BEFORE EVERYTHING STARTED.
 *
 * @param handler  std::function<void(std::string)>, the parameter of this function is the traceback message, ususally you need to print the traceback in your
 * way.
 */
void initSignalTraceback(std::function<void(std::string)> handler);
/**
 * @brief Set the Max Trace Back Depth, default value is 10
 */
void setMaxTraceBackDepth(int maxDepth);
/**
 * @brief Set the Addr2line Path , default is ""/usr/bin/addr2line""
 */
void setAddr2linePath(std::string pathToAddr2line);

/**
 * @brief immediately return the inforamtion of traceback of current calling stack
 */
std::string traceBack();

#endif