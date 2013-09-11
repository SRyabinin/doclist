#include "StdAfx.h"
#include "Utility.h"
#include <algorithm>


namespace utility
{
	std::string replaceExtention(const std::string& path, const std::string& extention)
	{
		std::string result;
		size_t pos = path.find_last_of(".");
		if(pos != std::string::npos)
			result.assign(path.begin(), path.begin() + pos);
		else
			result = path;
		result += "." + extention;
		return result;
	}

	std::string getParent(const std::string& path)
	{
		std::string result;
		size_t pos = path.find_last_of("\\");
		if(pos != std::string::npos)
			result.assign(path.begin(), path.begin() + pos);
		else
			result = path;
		return result;
	}

	std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elements)
	{
		std::stringstream ss(s);
		std::string item;
		while (std::getline(ss, item, delim))
		{
			elements.push_back(item);
		}
		return elements;
	}


	std::vector<std::string> split(const std::string &s, char delim)
	{
		std::vector<std::string> elems;
		split(s, delim, elems);
		return elems;
	}

	std::map<std::string, std::string> parseQueryString(const std::string& queryString)
	{
		std::map<std::string, std::string> result;
		std::vector<std::string> params = utility::split(queryString, '&');
		std::for_each(params.begin(), params.end(), [&result](const std::string param)
		{
			size_t pos = param.find('=');
			if(pos != std::string::npos)
			{
				result[param.substr(0, pos)] = param.substr(pos+1);
			}
			else
			{
				result[param] = std::string();
			}
		});
		return result;
	}

	bool exists(const std::string& path)
	{
		DWORD dwAttrib = ::GetFileAttributesA(path.c_str());

		  return (dwAttrib != INVALID_FILE_ATTRIBUTES && 
				 !(dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
	}

	void replace(std::string& str, const std::string& from, const std::string& to)
	{
		if(from.empty())
			return;
		size_t start_pos = 0;
		while((start_pos = str.find(from, start_pos)) != std::string::npos)
		{
			str.replace(start_pos, from.length(), to);
			start_pos += to.length(); // In case 'to' contains 'from', like replacing 'x' with 'yx'
		}
	}
}
