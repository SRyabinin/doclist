#pragma once

#include <string>

class Repository
{
public:
	std::string GetItems(const std::string& path, long first, long last) const;
};

