
#include <regex>
#include <vector>
namespace GuiBridge {
std::vector<std::string> split(const std::string &str, const std::string &delimiter_regex) {
    std::vector<std::string> tokens;
    std::regex re(delimiter_regex);
    std::sregex_token_iterator begin(str.begin(), str.end(), re, -1);
    std::sregex_token_iterator end;

    for (auto it = begin; it != end; ++it) {
        tokens.push_back(it->str());
    }

    return tokens;
}
std::string trim(const std::string &str) {
    // 使用find_if_not查找第一个非空白字符
    auto start = std::find_if_not(str.begin(), str.end(), [](unsigned char ch) { return std::isspace(ch); });

    // 使用find_if_not查找最后一个非空白字符
    auto end = std::find_if_not(str.rbegin(), str.rend(), [](unsigned char ch) { return std::isspace(ch); }).base();

    // 构造一个新的字符串
    return (start < end ? std::string(start, end) : std::string());
}
}  // namespace GuiBridge
