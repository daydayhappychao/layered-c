#ifndef STRING_UTIL_HPP
#define STRING_UTIL_HPP

#include <vector>
namespace GuiBridge {
std::vector<std::string> split(const std::string &str, const std::string &delimiter_regex);
std::string trim(const std::string &str);
}  // namespace GuiBridge

#endif