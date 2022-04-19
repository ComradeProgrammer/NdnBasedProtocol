#include "Flag.h"
using namespace std;
void Flag::setFlagForValue(string operand, string description, std::string defaultValue) {
    if (operand == "-h" || operand == "--help") {
        return;
    }
    flagValues[operand] = defaultValue;
    flagDescription[operand] = description;
}

void Flag::setFlagForExistence(std::string operand, std::string description) {
    if (operand == "-h" || operand == "--help") {
        return;
    }
    flagForExistence[operand] = false;
    flagDescription[operand] = description;
}

string Flag::parseFlag(int argc, const char* const argv[], bool allowUnrecognized) {
    for (int i = 1; i < argc; i++) {
        std::string operand = string(argv[i]);
        if (operand == "-h" || operand == "--help") {
            cout << generateDescription();
            return "help page";
        }
        // operand must start with -
        if (operand.size() == 0 || operand[0] != '-') {
            return "invalid operand " + operand;
        }

        if (flagValues.find(operand) != flagValues.end()) {
            // flag for existence
            i++;
            if (i >= argc) {
                return "missing content for operand " + operand;
            }
            string value = string(argv[i]);
            if (value.size() == 0 || value[0] == '-') {
                return "missing content for operand " + operand;
            }
            flagValues[operand] = value;
        } else if (flagForExistence.find(operand) != flagForExistence.end()) {
            flagForExistence[operand] = true;
        } else if (!allowUnrecognized) {
            return "unrecognized operand " + operand;
        }
    }
    return "";
}

int Flag::getIntFlag(string operand) { return atoi(flagValues[operand].c_str()); }

std::string Flag::getStringFlag(string operand) { return flagValues[operand]; }

bool Flag::flagExists(std::string operand) { return flagForExistence[operand]; }

std::string Flag::generateDescription() { return ""; }