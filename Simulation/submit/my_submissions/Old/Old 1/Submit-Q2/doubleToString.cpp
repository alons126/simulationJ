#include <iostream>
#include <string>
#include <sstream>
#include <iomanip>  // For std::fixed and std::setprecision

std::string doubleToString(double value) {
    std::ostringstream oss;

    // Apply fixed-point and set precision on the ostringstream
    oss << std::fixed << std::setprecision(2) << value;

    std::string result = oss.str();

    // Replace '.' with '_'
    for (auto &ch : result) {
        if (ch == '.') {
            ch = '_';
        }
    }

    return result;
}

std::string doubleToStringWithPercision(double value) {
    std::ostringstream oss;

    // Apply fixed-point and set precision on the ostringstream
    oss << std::fixed << std::setprecision(2) << value;

    std::string result = oss.str();

    return result;
}