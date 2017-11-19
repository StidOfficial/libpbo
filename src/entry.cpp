#include "entry.hpp"

#include <experimental/filesystem>

namespace PBO
{
	Entry::Entry()
	{
		this->packingMethod = PACKINGMETHOD_NULL;
		this->originalSize = 0;
		this->reserved = 0;
		this->timestamp = 0;
		this->dataSize = 0;
		this->dataOffset = -1;
		this->productEntry = NULL;
	}

	void Entry::setFilePath(std::string filePath)
	{
		this->filePath = filePath;

		std::experimental::filesystem::path fsPath = filePath;
		this->setDataSize(std::experimental::filesystem::file_size(fsPath));
		std::experimental::filesystem::file_time_type lastWriteTime = std::experimental::filesystem::last_write_time(fsPath);
		std::time_t entryTime = decltype(lastWriteTime)::clock::to_time_t(lastWriteTime);
		this->setTimestamp((unsigned long)entryTime);
	}

	std::string Entry::getFilePath()
	{
		return this->filePath;
	}

	void Entry::setPath(std::string path)
	{
		this->path = path;
	}

	std::string Entry::getPath()
	{
		return path;
	}

	void Entry::setPackingMethod(unsigned long packingMethod)
	{
		this->packingMethod = packingMethod;
		if(!this->productEntry && packingMethod == PACKINGMETHOD_PRODUCTENTRY)
			this->productEntry = new ProductEntry();
	}

	unsigned long Entry::getPackingMethod()
	{
		return packingMethod;
	}

	void Entry::setOriginalSize(unsigned long originalSize)
	{
		this->originalSize = originalSize;
	}

	unsigned long Entry::getOriginalSize()
	{
		return originalSize;
	}

	void Entry::setReserved(unsigned long reserved)
	{
		this->reserved = reserved;
	}

	unsigned long Entry::getReserved()
	{
		return reserved;
	}

	void Entry::setTimestamp(unsigned long timestamp)
	{
		this->timestamp = timestamp;
	}

	unsigned long Entry::getTimestamp()
	{
		return this->timestamp;
	}

	void Entry::setDataSize(unsigned long dataSize)
	{
		this->dataSize = dataSize;
	}

	int Entry::getDataSize()
	{
		return dataSize;
	}

	void Entry::setDataOffset(int dataOffset)
	{
		this->dataOffset = dataOffset;
	}

	int Entry::getDataOffset()
	{
		return this->dataOffset;
	}

	ProductEntry* &Entry::getProductEntry()
	{
		return this->productEntry;
	}

	bool Entry::isProductEntry()
	{
		return getPackingMethod() == PACKINGMETHOD_PRODUCTENTRY;
	}

	bool Entry::isFileEntry()
	{
		return getPackingMethod() == PACKINGMETHOD_UNCOMPRESSED || getPackingMethod() == PACKINGMETHOD_PACKED;
	}

	bool Entry::isZeroEntry()
	{
		return (path.length() == 0 && packingMethod == 0 && originalSize == 0 && reserved == 0 && timestamp == 0 && dataSize == 0);
	}

	Entry::~Entry()
	{
		if(this->productEntry)
			delete this->productEntry;
	}
}
