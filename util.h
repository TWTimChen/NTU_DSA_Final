#ifndef UTIL_H
#define UTIL_H

#include <sstream>
#include <string>
#include <vector>
#include <algorithm>
#include <map>

inline 
void split(const std::string& str, std::vector<std::string>& cont, char delim = ' ')
{
    std::stringstream ss(str);
    std::string token;
    cont.clear();
    while (getline(ss, token, delim)) {
        cont.push_back(token);
    }
}

inline
std::string getMonthIndex(std::string name)
{
    std::map<std::string, std::string> months 
    {
        { "January", "01" },
        { "February", "02" },
        { "March", "03" },
        { "April", "04" },
        { "May", "05" },
        { "June", "06" },
        { "July", "07" },
        { "August", "08" },
        { "September", "09" },
        { "October", "10" },
        { "November", "11" },
        { "December", "12" }
    };

    std::map<std::string, std::string>::iterator iter;
    iter = months.find(name);

    if( months.find(name) != months.cend() )
        return iter->second;
    return "00";
}

inline
void cleanStr(std::string& rawStr)
{
    std::transform(
        rawStr.begin(),
        rawStr.end(),
        rawStr.begin(),
        ::tolower
    );

    typedef bool(*DecisionFn)(char);
    DecisionFn isClear { [](char c) { return !std::isalnum(c); } };
    std::replace_if(rawStr.begin(), rawStr.end(), isClear, ' '); 
}

#endif