#include "../../interface/IRC.hpp"

std::vector<std::string> split(const std::string &str, char delimiter) {
	std::vector<std::string> tokens;
	size_t start = 0;
	size_t end = str.find(delimiter);

	if (end == std::string::npos) {
		tokens.push_back(str);
		return tokens;
	}

	while (end != std::string::npos) {
		tokens.push_back(str.substr(start, end - start));
		start = end + 1;
		end = str.find(delimiter, start);
	}

	tokens.push_back(str.substr(start));

	return tokens;
}

std::string toLowerCase(const std::string& str) {
    std::string lowerStr = str;
    for (size_t i = 0; i < str.length(); i++) {
		lowerStr[i] = tolower(str[i]);
	}
    return lowerStr;
}

std::string intToString(int number) {
    std::ostringstream oss;
    oss << number;
    return oss.str();
}
