#ifndef ENTRY_HPP
#define ENTRY_HPP

#include <string>
#include <fstream>
#include <ctime>

#include "ProductEntry.hpp"

#define PACKINGMETHOD_UNCOMPRESSED	0x0
#define PACKINGMETHOD_PACKED		0x43707273
#define PACKINGMETHOD_PRODUCTENTRY	0x56657273
#define PACKINGMETHOD_NULL		-0x1

namespace PBO
{
	class Entry
	{
	public:
		Entry();
		~Entry();
		void setFilePath(std::string filePath);
		std::string getFilePath();
		void setPath(std::string path);
		std::string getPath();
		void setPackingMethod(unsigned long packingMethod);
		unsigned long getPackingMethod();
		void setOriginalSize(unsigned long originalSize);
		unsigned long getOriginalSize();
		void setReserved(unsigned long reserved);
		unsigned long getReserved();
		void setTimestamp(unsigned long timeStamp);
		unsigned long getTimestamp();
		void setDataSize(unsigned long dataSize);
		int getDataSize();
		void setDataOffset(int dataPos);
		int getDataOffset();
		ProductEntry* &getProductEntry();
		bool isProductEntry();
		bool isFileEntry();
		bool isZeroEntry();
	private:
		std::string filePath;
		std::string path;
		unsigned long packingMethod;
		unsigned long originalSize;
		unsigned long reserved;
		unsigned long timestamp;
		unsigned long dataSize;
		int dataOffset;
		ProductEntry *productEntry;
	};
}

#endif
