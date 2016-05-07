#ifndef STRING_UTILS_HPP
#define STRING_UTILS_HPP
#include <string>

class StringUtils
{
public:
    static bool EndWith(const std::string& str, const std::string& end)
    {
        return strncmp(str.c_str() + str.length() - end.length(), end.c_str(), end.length()) == 0;
    }

    static std::string Left(const std::string& str, int n)
    {
        return str.substr(0, n);
    }
};

#endif
