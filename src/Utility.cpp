#include "Utility.h"

#include <algorithm>
#include <iostream>
#include <limits>
#include <string>

std::string toLowercase(const std::string &input)
{
    std::string lowercased = input;
    std::transform(lowercased.begin(), lowercased.end(), lowercased.begin(),
                   [](unsigned char c) { return std::tolower(c); });
    return lowercased;
}

void clearInputBuffer()
{
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}
