#include "StdAfx.h"
#include "Index.h"
#include "Utility.h"
#include <iostream>
#include <fstream>
#include <algorithm>
#include <iterator>

namespace Indexing
{

// Temporary struct for collecting index data.
struct IndexItem : Item, JsonItem
{
	void clear()
	{
		offset = 0;
		length = 0;
		lineSizes.clear();
	}
	inline bool empty() const
	{
		return lineSizes.empty();
	}
};

Index::Index(const std::string& name)
	:m_name(name)
	, m_indexName(utility::replaceExtention(name,"index"))
	, m_jsonName(utility::replaceExtention(name,"json"))
{
}


Index::~Index(void)
{
}

bool Index::IsExists() const
{
	return utility::exists(m_indexName);
}

TItems Index::ReadIndex(long first, long count) const
{
	std::ifstream file (m_indexName, std::ios::in | std::ios::binary);
	if (!file.good())
	{
		throw std::runtime_error("Index file not found or corrupted.");
	}
	
	TItems index(count);
	file.seekg(first*sizeof(Item), file.beg);
	if(file.read((char*)&index[0], sizeof(Item)*count).gcount()!= sizeof(Item)*count)
	{
		throw std::runtime_error("Can not read index record for item. Out of range or index file corrupted. Try to rebuild index.");
	}
	file.close();
	return index;
}

void Index::RebuildIndex() const
{
	std::ifstream file (m_name);

	std::ofstream ofile;
	char line[2048];
	IndexItem item;
	long itemCount = 0;
	if (file.is_open())
	{
		std::ofstream indexFile (m_indexName, std::ios::out | std::ios::binary | std::ios::trunc);
		std::ofstream jsonFile (m_jsonName, std::ios::out | std::ios::trunc);

		std::ifstream::pos_type first = file.tellg();
		int emptyLineCounter = 0;
		jsonFile << "[";
		while ( file.good() )
		{
			if(item.empty())
			{
				item.offset =  file.tellg().seekpos();
			}

			std::streamsize count = file.getline(line, sizeof(line)).gcount();
			if(count <= 1)
			{
				if(++emptyLineCounter == 2)
				{
					// сохраняем описатель документа.
					indexFile << static_cast<const Item&>(item);
					jsonFile << static_cast<const JsonItem&>(item) << ",";
					item.clear();
					emptyLineCounter = 0;
					itemCount++;
				}
			}
			else
			{
				item.length+=static_cast<short>(count);
				item.lineSizes.push_back(static_cast<short>(count-1));
			}
		}
		if(!item.empty())
		{
			indexFile << Item(item);
			jsonFile << JsonItem(item);
		}
		jsonFile << "]";

		file.close();
		indexFile.close();
		jsonFile.close();
	}
	else
	{
		throw std::runtime_error("Repository file not found.");
	}
}
}