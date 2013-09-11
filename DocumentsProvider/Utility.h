#pragma once

#include <string>
#include <vector>
#include <map>
#include <sstream>

namespace utility
{
	// replase extention in file path.
	std::string replaceExtention(const std::string& path, const std::string& extention);

	// Get parent path for spicified path.
	std::string getParent(const std::string& path);

	// Returns a vector of strings that contains the substrings in this instance that are delimited by 
	// specified Unicode character
	std::vector<std::string> split(const std::string &s, char delim);

	template<typename T>
	T convert(const std::string& src)
	{
		std::istringstream stream(src);
		T value;
		stream >> value;  
		return value;
	}

	std::map<std::string, std::string> parseQueryString(const std::string& queryString);

	bool exists(const std::string& path);

	void replace(std::string& str, const std::string& from, const std::string& to);

}