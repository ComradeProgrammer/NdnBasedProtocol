#ifndef __FLAG_H_
#define __FLAG_H_
#include <iostream>
#include <string>
#include <unordered_map>
#include <unordered_set>
class Flag {
   public:
    void setFlagForValue(std::string operand, std::string description, std::string defaultValue = "");
    void setFlagForExistence(std::string operand, std::string description);
    std::string parseFlag(int argc, char* argv[], bool allowUnrecognized = false);

    int getIntFlag(std::string operand);
    std::string getStringFlag(std::string operand);
    bool flagExists(std::string operand);

   private:
    std::unordered_map<std::string, std::string> flagDescription;
    std::unordered_map<std::string, std::string> flagValues;
    std::unordered_map<std::string, bool> flagForExistence;

    std::string generateDescription();
};
#endif