#pragma once

#include <string>
#include <vector>
#include <ostream>
#include <iterator>

namespace Indexing
{
// Описывает положение и размер конкретного документа в общем тектовом файле.
#pragma pack(push, 1)
struct Item
{
	Item():offset(0), length(0)
	{
	}
	// Смещение текста от начала файла;
	fpos_t offset;

	// Длинна текста в символах.
	unsigned short length;
};
#pragma pack(pop)

typedef std::vector<Item> TItems;

template<typename T>
std::ostream& operator <<(std::basic_ostream<T>& stream, const Item& item) 
{
	stream.write((char*)&item, sizeof(Item));
	return stream; 
}

// Описывает размер текста в каждм элементе репозитория.
struct JsonItem
{
	// Длины всех строк во фрагменте текста.
	std::vector<short> lineSizes;
};

typedef std::vector<JsonItem> TJsonItems;

template<typename T>
std::ostream& operator <<(std::basic_ostream<T>& stream, const JsonItem& item) 
{
	stream << "[";
	copy(item.lineSizes.begin(),item.lineSizes.end()-1, 
		std::ostream_iterator<short>(stream,","));
	stream << *item.lineSizes.rbegin();
	stream << "]";
   return stream; 
}

//template<typename T>
//void writeJson(std::basic_ostream<T>& stream, const TJsonItems& items)
//{
//	stream << "[";
//	std::for_each(items.begin(), items.end()-1, [&stream](const JsonItem& item) 
//	{
//		writeJson(stream, item);
//		stream << ",";
//	});
//	writeJson(stream, *items.rbegin());
//	stream << "]";
//};


}

