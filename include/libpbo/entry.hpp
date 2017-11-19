#ifndef ENTRY_HPP
#define ENTRY_HPP

#include <string>
#include <fstream>
#include <ctime>
#include <cstdint>

#include "productentry.hpp"

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
		void setPackingMethod(uint32_t packingMethod);
		uint32_t getPackingMethod();
		void setOriginalSize(uint32_t originalSize);
		uint32_t getOriginalSize();
		void setReserved(uint32_t reserved);
		uint32_t getReserved();
		void setTimestamp(uint32_t timeStamp);
		uint32_t getTimestamp();
		void setDataSize(uint32_t dataSize);
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
		uint32_t packingMethod;
		uint32_t originalSize;
		uint32_t reserved;
		uint32_t timestamp;
		uint32_t dataSize;
		int dataOffset;
		ProductEntry *productEntry;
	};
}

#endif
