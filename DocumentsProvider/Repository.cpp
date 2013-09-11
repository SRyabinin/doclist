// DocumentsProvider.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "Repository.h"
#include "Index.h"
#include "Utility.h"
#include <string>
#include <fstream>
#include <algorithm>
#include <sstream>

void read(std::istream& stream, long id, const Indexing::Item& item, std::ostream& result)
{
	std::string text;
	text.resize(item.length);
	stream.seekg(item.offset, stream.beg);
	if(stream.read((char*)&text[0], item.length).gcount() != item.length)
	{
		throw std::runtime_error("Not read enough data from repository.");
	}
	utility::replace(text, "\'", "\'\'");
	utility::replace(text, "\"", "\\\"");
	utility::replace(text, "\n", "\\\\n");
	result << "{\"id\":" << id << ", \"text\":\"" << text << "\"}";
}

std::string Repository::GetItems(const std::string& path, long first, long last) const
{
	if(last < first)
		throw std::runtime_error("Invalid arguments. Arguments last can not be less than first.");
	Indexing::Index index(path);
	Indexing::TItems items = index.ReadIndex(first, last-first+1);
	std::ostringstream result;
	std::ifstream file (path);
	if (file.good())
	{
		long id = first;
		result << '[';
		std::for_each(items.begin(), items.end()-1, [&result, &file, &id](const Indexing::Item& item)
		{
			read(file, id++, item, result);
			result << ",";
		});
		read(file, id++, *items.rbegin(), result);
		result << ']';
	}
	file.close();
	return result.str();
}
