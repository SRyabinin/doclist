#pragma once
#include "IndexItem.h"

namespace Indexing
{
class Index
{
	const std::string m_name;
	const std::string m_indexName;
	const std::string m_jsonName;
public:
	Index(const std::string& name);
	virtual ~Index(void);
	bool IsExists() const;
	void RebuildIndex() const;
	TItems ReadIndex(long index, long count) const;
};
}

