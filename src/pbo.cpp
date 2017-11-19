#include "pbo.hpp"

#include <iostream>
#include <experimental/filesystem>

#define PACKING_BUFFER 4096

namespace PBO
{
	PBO::PBO(const std::string filePath) : pboFile()
	{
		this->pboFilePath = filePath;
	}

	void PBO::addEntry(Entry *entry)
	{
		this->entries.resize(this->getEntriesSize() + 1);
		this->entries[this->getEntriesSize() - 1] = entry;
	}

	void PBO::removeEntry(int index)
	{
		this->entries.erase(this->entries.begin(), this->entries.begin() + index);
	}

	Entry* &PBO::getEntry(int index)
	{
		return this->entries[index];
	}

	int PBO::getEntriesSize()
	{
		return this->entries.size();
	}

	void PBO::pack()
	{
		this->pboFile.open(this->pboFilePath, std::ofstream::out | std::ofstream::binary);
		if(!this->pboFile.is_open())
			throw std::logic_error(std::strerror(errno));

		char uLong[4];
		for(int i = 0; i < this->getEntriesSize(); i++)
		{
			Entry *entry = this->entries[i];
			this->pboFile.write(entry->getPath().data(), entry->getPath().length() + 1);

			uLong[0] = (int)(entry->getPackingMethod() & 0xFF);
			uLong[1] = (int)((entry->getPackingMethod() >> 8) & 0xFF);
			uLong[2] = (int)((entry->getPackingMethod() >> 16) & 0xFF);
			uLong[3] = (int)((entry->getPackingMethod() >> 24) & 0xFF);
			this->pboFile.write(uLong, 4);

			uLong[0] = (int)(entry->getOriginalSize() & 0xFF);
			uLong[1] = (int)((entry->getOriginalSize() >> 8) & 0xFF);
			uLong[2] = (int)((entry->getOriginalSize() >> 16) & 0xFF);
			uLong[3] = (int)((entry->getOriginalSize() >> 24) & 0xFF);
			this->pboFile.write(uLong, 4);

			uLong[0] = (int)(entry->getReserved() & 0xFF);
			uLong[1] = (int)((entry->getReserved() >> 8) & 0xFF);
			uLong[2] = (int)((entry->getReserved() >> 16) & 0xFF);
			uLong[3] = (int)((entry->getReserved() >> 24) & 0xFF);
			this->pboFile.write(uLong, 4);

			uLong[0] = (int)(entry->getTimestamp() & 0xFF);
			uLong[1] = (int)((entry->getTimestamp() >> 8) & 0xFF);
			uLong[2] = (int)((entry->getTimestamp() >> 16) & 0xFF);
			uLong[3] = (int)((entry->getTimestamp() >> 24) & 0xFF);
			this->pboFile.write(uLong, 4);

			uLong[0] = (int)(entry->getDataSize() & 0xFF);
			uLong[1] = (int)((entry->getDataSize() >> 8) & 0xFF);
			uLong[2] = (int)((entry->getDataSize() >> 16) & 0xFF);
			uLong[3] = (int)((entry->getDataSize() >> 24) & 0xFF);
			this->pboFile.write(uLong, 4);
			
			switch(entry->getPackingMethod())
			{
				case PACKINGMETHOD_PRODUCTENTRY:
				{
					ProductEntry *productEntry = entry->getProductEntry();
					if(!productEntry->getEntryName().empty())
						this->pboFile.write(productEntry->getEntryName().data(), productEntry->getEntryName().length() + 1);

					if(!productEntry->getProductName().empty())
						this->pboFile.write(productEntry->getProductName().data(), productEntry->getProductName().length() + 1);

					if(!productEntry->getProductVersion().empty())
						this->pboFile.write(productEntry->getProductVersion().data(), productEntry->getProductVersion().length() + 1);

					for(int i = 0; i < productEntry->getProductDataSize(); i++)
					{
						this->pboFile.write(productEntry->getProductData(i).c_str(), productEntry->getProductData(i).length() + 1);
					}
					this->pboFile.write("\0", 1);
					break;
				}
				case PACKINGMETHOD_UNCOMPRESSED:
					break;
				case PACKINGMETHOD_PACKED:
					throw std::logic_error("Packed method is unavailable !");
					break;
				default:
					throw std::logic_error("Unsupported packing method");
					break;
			}
		}

		char zeroEntry[21] = {0};
		this->pboFile.write(zeroEntry, sizeof(zeroEntry));

		char entryFileBuffer[PACKING_BUFFER];
		for(int i = 0; i < this->getEntriesSize(); i++)
		{
			Entry *entry = this->entries[i];
			switch(entry->getPackingMethod())
			{
				case PACKINGMETHOD_UNCOMPRESSED:
				{
					std::ifstream entryFile;
					entryFile.open(entry->getFilePath(), std::ifstream::in | std::ifstream::binary);
					if(!entryFile.is_open())
						throw std::logic_error(std::strerror(errno));

					while(!entryFile.eof())
					{
						entryFile.read(entryFileBuffer, sizeof(entryFileBuffer));
						this->pboFile.write(entryFileBuffer, entryFile.gcount());
					}

					entryFile.close();
					break;
				}
				default:
					break;
			}
		}
	}

	void PBO::unpack()
	{
		this->pboFile.open(this->pboFilePath, std::fstream::in | std::fstream::binary);
		if(!this->pboFile.is_open())
			throw std::logic_error(std::strerror(errno));

		this->pboFile.seekg(0, this->pboFile.end);
		int fileLength = this->pboFile.tellg();
		this->pboFile.seekg(0, this->pboFile.beg);

		std::string productEntryData;
		std::string entryPath = "";
		char uLong[4];
		char byte;

		while(!this->pboFile.eof())
		{
			if(((int)this->pboFile.tellg() + 21) > fileLength)
				throw std::logic_error("Header entry is too small");

			Entry *entry = new Entry();

			entryPath = "";
			while(this->pboFile.get(byte))
			{
				if(byte == '\0')
					break;
				else
					entryPath += byte;
			}

			entry->setPath(entryPath);

			this->pboFile.read(uLong, 4);
			entry->setPackingMethod(*((unsigned int*)uLong));

			this->pboFile.read(uLong, 4);
			entry->setOriginalSize(*((unsigned int*)uLong));

			this->pboFile.read(uLong, 4);
			entry->setReserved(*((unsigned int*)uLong));

			this->pboFile.read(uLong, 4);
			entry->setTimestamp(*((unsigned int*)uLong));

			this->pboFile.read(uLong, 4);
			entry->setDataSize(*((unsigned int*)uLong));

			if(entry->isZeroEntry())
			{
				delete entry;
				break;
			}
			else
			{
				switch(entry->getPackingMethod())
				{
					case PACKINGMETHOD_PRODUCTENTRY:
						productEntryData = "";
						while(this->pboFile.get(byte))
						{
							if(byte == '\0')
								if(productEntryData.length() == 0)
									break;
								else
								{
									ProductEntry *productEntry = entry->getProductEntry();
									if(productEntry->getEntryName().empty())
										productEntry->setEntryName(productEntryData);
									else if(productEntry->getProductName().empty())
										productEntry->setProductName(productEntryData);
									else if(productEntry->getProductVersion().empty())
										productEntry->setProductVersion(productEntryData);
									else
										productEntry->addProductData(productEntryData);
									productEntryData = "";
								}
							else
								productEntryData.push_back(byte);
						}
						break;
					case PACKINGMETHOD_UNCOMPRESSED:
						break;
					case PACKINGMETHOD_PACKED:
						std::cout << "WARNING : Packed method is unavailable !" << std::endl;
						break;
					default:
						std::cout << "WARNING : Unsupported packing method (" << entry->getPackingMethod() << ") !" << std::endl;
						break;
				}
			}

			this->addEntry(entry);

			if(this->pboFile.tellg() == -1)
				throw std::logic_error("No zero entry found");
		}

		int sDataOffset = this->pboFile.tellg();
		for(int i = 0; i < getEntriesSize(); i++)
		{
			Entry *entry = this->getEntry(i);
			entry->setDataOffset(sDataOffset);

			sDataOffset = sDataOffset + entry->getDataSize();
		}

		if(sDataOffset > fileLength)
			throw std::logic_error("Is too small");
	}

	PBO::~PBO()
	{
		for(int i = 0; i < this->getEntriesSize(); i++)
			delete this->entries[i];

		this->pboFile.close();
	}
}
