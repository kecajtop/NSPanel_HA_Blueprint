// text.cpp

#include "text.h"
#include <algorithm>
#include <cctype>
#include <cstring>

namespace nspanel_ha_blueprint {

    template <unsigned int N>
    void copyStringToCharArray(char (&dest)[N], const std::string& src) {
        size_t length = std::min(src.size(), static_cast<size_t>(N - 1));
        std::strncpy(dest, src.c_str(), length);
        dest[length] = '\0';
    }

    // Explicit template instantiation might be needed depending on usage
    // template void copyStringToCharArray<YOUR_SIZE_HERE>(char (&)[YOUR_SIZE_HERE], const std::string&);

    bool isNumberChar(char c) {
        return std::isdigit(static_cast<unsigned char>(c)) || c == '.' || c == '-' || c == ',';
    }

    std::string adjustDecimalSeparator(const std::string& input, char decimalSeparator) {
        if (decimalSeparator == '.') {
            return input;
        }

        size_t numericEnd = 0;
        for (; numericEnd < input.size() && isNumberChar(input[numericEnd]); ++numericEnd);

        std::string numericPart = input.substr(0, numericEnd);
        std::string suffix = input.substr(numericEnd);

        char* end;
        double val = strtod(numericPart.c_str(), &end);

        if (end != numericPart.c_str() && *end == '\0') {
            size_t decimalPointPos = numericPart.find('.');
            if (decimalPointPos != std::string::npos) {
                numericPart[decimalPointPos] = decimalSeparator;
            }
            return numericPart + suffix;
        } else {
            return input;
        }
    }

    std::string process_time_format(const std::string &time_format, int current_hour,
                                    const std::string &meridiem_am, const std::string &meridiem_pm) {
        std::string processed_time_format = time_format;

        if (processed_time_format.find("%-H") != std::string::npos) {
            processed_time_format = processed_time_format.replace(
                processed_time_format.find("%-H"), sizeof("%-H") - 1, std::to_string(current_hour)
            );
        }

        if (processed_time_format.find("%-I") != std::string::npos) {
            if (current_hour > 12) {
                processed_time_format = processed_time_format.replace(
                    processed_time_format.find("%-I"), sizeof("%-I") - 1, std::to_string(current_hour - 12)
                );
            } else if (current_hour == 0) {
                processed_time_format = processed_time_format.replace(
                    processed_time_format.find("%-I"), sizeof("%-I") - 1, "12"
                );
            } else {
                processed_time_format = processed_time_format.replace(
                    processed_time_format.find("%-I"), sizeof("%-I") - 1, std::to_string(current_hour)
                );
            }
        }

        return processed_time_format;
    }


}  // namespace nspanel_ha_blueprint
