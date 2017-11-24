#include "pbo.hpp"

#include <iostream>
#include <cstring>
#include <cerrno>
#include <experimental/filesystem>

#define PACKING_BUFFER_SIZE 4096
#define SIGNATURE_BUFFER_SIZE 1024
#define HEADER_ENTRY_DEFAULT_SIZE 21

namespace filesystem = std::experimental::filesystem;
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

	std::string &PBO::getSignature()
	{
		return this->signature;
	}

	void PBO::pack()
	{
		if(!SHA1_Init(&this->signatureContext))
			throw std::logic_error("Failed to intialize SHA1");

		if(filesystem::is_directory(this->pboFilePath))
			throw std::logic_error(std::strerror(EISDIR));

		this->pboFile.open(this->pboFilePath, std::ofstream::out | std::ofstream::binary);
		if(!this->pboFile.is_open())
			throw std::logic_error(std::strerror(errno));

		char uLong[4];
		for(int i = 0; i < this->getEntriesSize(); i++)
		{
			Entry *entry = this->entries[i];
			this->pboFile.write(entry->getPath().data(), entry->getPath().length() + 1);
			SHA1_Update(&this->signatureContext, entry->getPath().data(), entry->getPath().length() + 1);

			uLong[0] = (int)(entry->getPackingMethod() & 0xFF);
			uLong[1] = (int)((entry->getPackingMethod() >> 8) & 0xFF);
			uLong[2] = (int)((entry->getPackingMethod() >> 16) & 0xFF);
			uLong[3] = (int)((entry->getPackingMethod() >> 24) & 0xFF);
			this->pboFile.write(uLong, sizeof(uLong));
			SHA1_Update(&this->signatureContext, uLong, sizeof(uLong));

			uLong[0] = (int)(entry->getOriginalSize() & 0xFF);
			uLong[1] = (int)((entry->getOriginalSize() >> 8) & 0xFF);
			uLong[2] = (int)((entry->getOriginalSize() >> 16) & 0xFF);
			uLong[3] = (int)((entry->getOriginalSize() >> 24) & 0xFF);
			this->pboFile.write(uLong, sizeof(uLong));
			SHA1_Update(&this->signatureContext, uLong, sizeof(uLong));

			uLong[0] = (int)(entry->getReserved() & 0xFF);
			uLong[1] = (int)((entry->getReserved() >> 8) & 0xFF);
			uLong[2] = (int)((entry->getReserved() >> 16) & 0xFF);
			uLong[3] = (int)((entry->getReserved() >> 24) & 0xFF);
			this->pboFile.write(uLong, sizeof(uLong));
			SHA1_Update(&this->signatureContext, uLong, sizeof(uLong));

			uLong[0] = (int)(entry->getTimestamp() & 0xFF);
			uLong[1] = (int)((entry->getTimestamp() >> 8) & 0xFF);
			uLong[2] = (int)((entry->getTimestamp() >> 16) & 0xFF);
			uLong[3] = (int)((entry->getTimestamp() >> 24) & 0xFF);
			this->pboFile.write(uLong, sizeof(uLong));
			SHA1_Update(&this->signatureContext, uLong, sizeof(uLong));

			uLong[0] = (int)(entry->getDataSize() & 0xFF);
			uLong[1] = (int)((entry->getDataSize() >> 8) & 0xFF);
			uLong[2] = (int)((entry->getDataSize() >> 16) & 0xFF);
			uLong[3] = (int)((entry->getDataSize() >> 24) & 0xFF);
			this->pboFile.write(uLong, sizeof(uLong));
			SHA1_Update(&this->signatureContext, uLong, sizeof(uLong));
			
			switch(entry->getPackingMethod())
			{
				case PACKINGMETHOD_PRODUCTENTRY:
				{
					ProductEntry *productEntry = entry->getProductEntry();
					if(!productEntry->getEntryName().empty())
					{
						this->pboFile.write(productEntry->getEntryName().data(), productEntry->getEntryName().length() + 1);
						SHA1_Update(&this->signatureContext, productEntry->getEntryName().data(), productEntry->getEntryName().length() + 1);
					}

					if(!productEntry->getProductName().empty())
					{
						this->pboFile.write(productEntry->getProductName().data(), productEntry->getProductName().length() + 1);
						SHA1_Update(&this->signatureContext, productEntry->getProductName().data(), productEntry->getProductName().length() + 1);
					}

					if(!productEntry->getProductVersion().empty())
					{
						this->pboFile.write(productEntry->getProductVersion().data(), productEntry->getProductVersion().length() + 1);
						SHA1_Update(&this->signatureContext, productEntry->getProductVersion().data(), productEntry->getProductVersion().length() + 1);
					}

					for(int i = 0; i < productEntry->getProductDataSize(); i++)
					{
						this->pboFile.write(productEntry->getProductData(i).c_str(), productEntry->getProductData(i).length() + 1);
						SHA1_Update(&this->signatureContext, productEntry->getProductData(i).c_str(), productEntry->getProductData(i).length() + 1);
					}
					this->pboFile.write("\0", 1);
					SHA1_Update(&this->signatureContext, "\0", 1);
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

		char zeroEntry[HEADER_ENTRY_DEFAULT_SIZE] = {0};
		this->pboFile.write(zeroEntry, sizeof(zeroEntry));
		SHA1_Update(&this->signatureContext, zeroEntry, sizeof(zeroEntry));

		char entryFileBuffer[PACKING_BUFFER_SIZE];
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
						SHA1_Update(&this->signatureContext, entryFileBuffer, entryFile.gcount());
					}

					entryFile.close();
					break;
				}
				default:
					break;
			}
		}

		char signatureDigest[SHA_DIGEST_LENGTH];
		SHA1_Final((unsigned char*)signatureDigest, &this->signatureContext);
		this->pboFile.write("\0", 1);
		this->pboFile.write(signatureDigest, sizeof(signatureDigest));

		this->signature.resize(SHA_DIGEST_LENGTH * 2);
		for(int i = 0; i < SHA_DIGEST_LENGTH; i++)
			sprintf(&this->signature[i * 2], "%hhx", signatureDigest[i]);
	}

	void PBO::unpack()
	{
		if(!SHA1_Init(&this->signatureContext))
			throw std::logic_error("Failed to intialize SHA1");

		if(filesystem::is_directory(this->pboFilePath))
			throw std::logic_error(std::strerror(EISDIR));

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
			if(((int)this->pboFile.tellg() + HEADER_ENTRY_DEFAULT_SIZE) > fileLength)
				throw std::logic_error("Header entry is too small");

			Entry *entry = new Entry();

			entryPath = "";
			while(this->pboFile.get(byte))
			{
				SHA1_Update(&this->signatureContext, &byte, sizeof(byte));
				if(byte == '\0')
					break;
				else
					entryPath += byte;
			}

			entry->setPath(entryPath);

			this->pboFile.read(uLong, sizeof(uLong));
			SHA1_Update(&this->signatureContext, uLong, sizeof(uLong));
			entry->setPackingMethod(*((unsigned int*)uLong));

			this->pboFile.read(uLong, sizeof(uLong));
			SHA1_Update(&this->signatureContext, uLong, sizeof(uLong));
			entry->setOriginalSize(*((unsigned int*)uLong));

			this->pboFile.read(uLong, sizeof(uLong));
			SHA1_Update(&this->signatureContext, uLong, sizeof(uLong));
			entry->setReserved(*((unsigned int*)uLong));

			this->pboFile.read(uLong, sizeof(uLong));
			SHA1_Update(&this->signatureContext, uLong, sizeof(uLong));
			entry->setTimestamp(*((unsigned int*)uLong));

			this->pboFile.read(uLong, sizeof(uLong));
			SHA1_Update(&this->signatureContext, uLong, sizeof(uLong));
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
							SHA1_Update(&this->signatureContext, &byte, sizeof(byte));
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

		int dataOffset = this->pboFile.tellg();
		int leftDataLength;
		for(int i = 0; i < getEntriesSize(); i++)
		{
			Entry *entry = this->getEntry(i);
			entry->setDataOffset(dataOffset);

			dataOffset = dataOffset + entry->getDataSize();
			leftDataLength = entry->getDataSize();

			char signatureData[SIGNATURE_BUFFER_SIZE];
			while(leftDataLength > 0)
			{
				if(leftDataLength > SIGNATURE_BUFFER_SIZE)
				{
					this->pboFile.read(signatureData, SIGNATURE_BUFFER_SIZE);
					SHA1_Update(&this->signatureContext, &signatureData, SIGNATURE_BUFFER_SIZE);
				}
				else
				{
					this->pboFile.read(signatureData, leftDataLength);
					SHA1_Update(&this->signatureContext, &signatureData, leftDataLength);
				}

				leftDataLength = leftDataLength - SIGNATURE_BUFFER_SIZE;
			}
		}

		if((fileLength - dataOffset) == HEADER_ENTRY_DEFAULT_SIZE)
		{
			this->pboFile.get();

			char originalSignatureDigest[SHA_DIGEST_LENGTH];
			this->pboFile.read(originalSignatureDigest, SHA_DIGEST_LENGTH);

			this->signature.resize(SHA_DIGEST_LENGTH * 2);
			for(int i = 0; i < SHA_DIGEST_LENGTH; i++)
				sprintf(&this->signature[i * 2], "%hhx", originalSignatureDigest[i]);

			char finalSignatureDigest[SHA_DIGEST_LENGTH];
			SHA1_Final((unsigned char*)finalSignatureDigest, &this->signatureContext);

			std::string finalSignature;
			finalSignature.resize(SHA_DIGEST_LENGTH * 2);
			for(int i = 0; i < SHA_DIGEST_LENGTH; i++)
				sprintf(&finalSignature[i * 2], "%hhx", finalSignatureDigest[i]);

			if(this->signature != finalSignature)
				throw std::logic_error("Wrong file signature");
		}
		else
			throw std::logic_error("Signature not found");

		if(dataOffset > fileLength)
			throw std::logic_error("Is too small");
	}

	PBO::~PBO()
	{
		OPENSSL_cleanse(&this->signatureContext, sizeof(this->signatureContext));

		for(int i = 0; i < this->getEntriesSize(); i++)
			delete this->entries[i];

		this->pboFile.close();
	}
}
