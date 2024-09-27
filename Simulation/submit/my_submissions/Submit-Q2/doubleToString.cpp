//
// Created by Alon Sportes on 01/09/2024.
//
#include <iostream>
#include <string>
#include <sstream>

std::string doubleToString(double value) {
    std::ostringstream oss;
    oss << value;
    std::string result = oss.str();

    // Replace '.' with '_'
    for (auto& ch : result) {
        if (ch == '.') {
            ch = '_';
        }
    }

    return result;
}