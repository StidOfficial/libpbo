#include "pbo.hpp"

#include <iostream>
#include <cstring>
#include <cerrno>
#include <experimental/filesystem>

#define PACKING_BUFFER_SIZE 4096
#define SIGNATURE_BUFFER_SIZE 1024
#define HEADER_ENTRY_DEFAULT_SIZE 21

namespace filesystem = std::experimental::filesystem;
namespace pbo
{
	pbo::pbo(std::string file_path) : pbo_file()
	{
		this->pbo_file_path = file_path;
	}

	void pbo::add_entry(entry *entry)
	{
		this->entries.resize(this->size() + 1);
		this->entries[this->size() - 1] = entry;
	}

	void pbo::remove_entry(int index)
	{
		this->entries.erase(this->entries.begin(), this->entries.begin() + index);
	}

	entry* &pbo::get_entry(int index)
	{
		return this->entries[index];
	}

	int pbo::size()
	{
		return this->entries.size();
	}

	void pbo::set_pbo_signature(char* signature_digest)
	{
		this->pbo_checksum.resize(SHA_DIGEST_LENGTH * 2);
		for(int i = 0; i < SHA_DIGEST_LENGTH; i++)
			sprintf(&this->pbo_checksum[i * 2], "%hhx", signature_digest[i]);
	}

	std::string &pbo::pbo_signature()
	{
		return this->pbo_checksum;
	}

	void pbo::set_file_signature(char* signature_digest)
	{
		this->file_checksum.resize(SHA_DIGEST_LENGTH * 2);
		for(int i = 0; i < SHA_DIGEST_LENGTH; i++)
			sprintf(&this->file_checksum[i * 2], "%hhx", signature_digest[i]);
	}

	std::string &pbo::file_signature()
	{
		return this->file_checksum;
	}

	void pbo::pack()
	{
		if(!SHA1_Init(&this->signature_context))
			throw std::logic_error("Failed to intialize SHA1");

		if(filesystem::is_directory(this->pbo_file_path))
			throw std::logic_error(std::strerror(EISDIR));

		this->pbo_file.open(this->pbo_file_path, std::ofstream::out | std::ofstream::binary);
		if(!this->pbo_file.is_open())
			throw std::logic_error(std::strerror(errno));

		char uLong[4];
		for(int i = 0; i < this->size(); i++)
		{
			entry *entry = this->entries[i];
			this->pbo_file.write(entry->get_path().data(), entry->get_path().length() + 1);
			SHA1_Update(&this->signature_context, entry->get_path().data(), entry->get_path().length() + 1);

			uLong[0] = (int)(entry->get_packing_method() & 0xFF);
			uLong[1] = (int)((entry->get_packing_method() >> 8) & 0xFF);
			uLong[2] = (int)((entry->get_packing_method() >> 16) & 0xFF);
			uLong[3] = (int)((entry->get_packing_method() >> 24) & 0xFF);
			this->pbo_file.write(uLong, sizeof(uLong));
			SHA1_Update(&this->signature_context, uLong, sizeof(uLong));

			uLong[0] = (int)(entry->get_original_size() & 0xFF);
			uLong[1] = (int)((entry->get_original_size() >> 8) & 0xFF);
			uLong[2] = (int)((entry->get_original_size() >> 16) & 0xFF);
			uLong[3] = (int)((entry->get_original_size() >> 24) & 0xFF);
			this->pbo_file.write(uLong, sizeof(uLong));
			SHA1_Update(&this->signature_context, uLong, sizeof(uLong));

			uLong[0] = (int)(entry->get_reserved() & 0xFF);
			uLong[1] = (int)((entry->get_reserved() >> 8) & 0xFF);
			uLong[2] = (int)((entry->get_reserved() >> 16) & 0xFF);
			uLong[3] = (int)((entry->get_reserved() >> 24) & 0xFF);
			this->pbo_file.write(uLong, sizeof(uLong));
			SHA1_Update(&this->signature_context, uLong, sizeof(uLong));

			uLong[0] = (int)(entry->get_timestamp() & 0xFF);
			uLong[1] = (int)((entry->get_timestamp() >> 8) & 0xFF);
			uLong[2] = (int)((entry->get_timestamp() >> 16) & 0xFF);
			uLong[3] = (int)((entry->get_timestamp() >> 24) & 0xFF);
			this->pbo_file.write(uLong, sizeof(uLong));
			SHA1_Update(&this->signature_context, uLong, sizeof(uLong));

			uLong[0] = (int)(entry->get_data_size() & 0xFF);
			uLong[1] = (int)((entry->get_data_size() >> 8) & 0xFF);
			uLong[2] = (int)((entry->get_data_size() >> 16) & 0xFF);
			uLong[3] = (int)((entry->get_data_size() >> 24) & 0xFF);
			this->pbo_file.write(uLong, sizeof(uLong));
			SHA1_Update(&this->signature_context, uLong, sizeof(uLong));
			
			switch(entry->get_packing_method())
			{
				case PACKINGMETHOD_PRODUCTENTRY:
				{
					productentry *productEntry = entry->get_product_entry();
					if(!productEntry->get_entry_name().empty())
					{
						this->pbo_file.write(productEntry->get_entry_name().data(), productEntry->get_entry_name().length() + 1);
						SHA1_Update(&this->signature_context, productEntry->get_entry_name().data(), productEntry->get_entry_name().length() + 1);
					}

					if(!productEntry->get_product_name().empty())
					{
						this->pbo_file.write(productEntry->get_product_name().data(), productEntry->get_product_name().length() + 1);
						SHA1_Update(&this->signature_context, productEntry->get_product_name().data(), productEntry->get_product_name().length() + 1);
					}

					if(!productEntry->get_product_version().empty())
					{
						this->pbo_file.write(productEntry->get_product_version().data(), productEntry->get_product_version().length() + 1);
						SHA1_Update(&this->signature_context, productEntry->get_product_version().data(), productEntry->get_product_version().length() + 1);
					}

					for(int i = 0; i < productEntry->get_product_data_size(); i++)
					{
						this->pbo_file.write(productEntry->get_product_data(i).c_str(), productEntry->get_product_data(i).length() + 1);
						SHA1_Update(&this->signature_context, productEntry->get_product_data(i).c_str(), productEntry->get_product_data(i).length() + 1);
					}
					this->pbo_file.write("\0", 1);
					SHA1_Update(&this->signature_context, "\0", 1);
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
		this->pbo_file.write(zeroEntry, sizeof(zeroEntry));
		SHA1_Update(&this->signature_context, zeroEntry, sizeof(zeroEntry));

		char entryFileBuffer[PACKING_BUFFER_SIZE];
		for(int i = 0; i < this->size(); i++)
		{
			entry *entry = this->entries[i];
			switch(entry->get_packing_method())
			{
				case PACKINGMETHOD_UNCOMPRESSED:
				{
					std::ifstream entryFile;
					entryFile.open(entry->get_file_path(), std::ifstream::in | std::ifstream::binary);
					if(!entryFile.is_open())
						throw std::logic_error(std::strerror(errno));

					while(!entryFile.eof())
					{
						entryFile.read(entryFileBuffer, sizeof(entryFileBuffer));
						this->pbo_file.write(entryFileBuffer, entryFile.gcount());
						SHA1_Update(&this->signature_context, entryFileBuffer, (size_t)entryFile.gcount());
					}

					entryFile.close();
					break;
				}
				default:
					break;
			}
		}

		char file_signature_digest[SHA_DIGEST_LENGTH];
		SHA1_Final((unsigned char*)file_signature_digest, &this->signature_context);
		this->set_pbo_signature(file_signature_digest);
		this->set_file_signature(file_signature_digest);

		this->pbo_file.write("\0", 1);
		this->pbo_file.write(file_signature_digest, sizeof(file_signature_digest));
	}

	void pbo::unpack()
	{
		if(!SHA1_Init(&this->signature_context))
			throw std::logic_error("Failed to intialize SHA1");

		if(filesystem::is_directory(this->pbo_file_path))
			throw std::logic_error(std::strerror(EISDIR));

		this->pbo_file.open(this->pbo_file_path, std::fstream::in | std::fstream::binary);
		if(!this->pbo_file.is_open())
			throw std::logic_error(std::strerror(errno));

		this->pbo_file.seekg(0, this->pbo_file.end);
		int fileLength = (int)this->pbo_file.tellg();
		this->pbo_file.seekg(0, this->pbo_file.beg);

		std::string productEntryData;
		std::string entryPath = "";
		char uLong[4];
		char byte;
		while(!this->pbo_file.eof())
		{
			if(((int)this->pbo_file.tellg() + HEADER_ENTRY_DEFAULT_SIZE) > fileLength)
				throw std::logic_error("Header entry is too small");

			entry* pbo_entry = new entry();

			entryPath = "";
			while(this->pbo_file.get(byte))
			{
				SHA1_Update(&this->signature_context, &byte, sizeof(byte));
				if(byte == '\0')
					break;
				else
					entryPath += byte;
			}

			pbo_entry->set_path(entryPath);

			this->pbo_file.read(uLong, sizeof(uLong));
			SHA1_Update(&this->signature_context, uLong, sizeof(uLong));
			pbo_entry->set_packing_method(*((unsigned int*)uLong));

			this->pbo_file.read(uLong, sizeof(uLong));
			SHA1_Update(&this->signature_context, uLong, sizeof(uLong));
			pbo_entry->set_original_size(*((unsigned int*)uLong));

			this->pbo_file.read(uLong, sizeof(uLong));
			SHA1_Update(&this->signature_context, uLong, sizeof(uLong));
			pbo_entry->set_reserved(*((unsigned int*)uLong));

			this->pbo_file.read(uLong, sizeof(uLong));
			SHA1_Update(&this->signature_context, uLong, sizeof(uLong));
			pbo_entry->set_timestamp(*((unsigned int*)uLong));

			this->pbo_file.read(uLong, sizeof(uLong));
			SHA1_Update(&this->signature_context, uLong, sizeof(uLong));
			pbo_entry->set_data_size(*((unsigned int*)uLong));

			if(pbo_entry->is_zero_entry())
			{
				delete pbo_entry;
				break;
			}
			else
			{
				switch(pbo_entry->get_packing_method())
				{
					case PACKINGMETHOD_PRODUCTENTRY:
						productEntryData = "";
						while(this->pbo_file.get(byte))
						{
							SHA1_Update(&this->signature_context, &byte, sizeof(byte));
							if(byte == '\0')
								if(productEntryData.length() == 0)
									break;
								else
								{
									productentry *productEntry = pbo_entry->get_product_entry();
									if(productEntry->get_entry_name().empty())
										productEntry->set_entry_name(productEntryData);
									else if(productEntry->get_product_name().empty())
										productEntry->set_product_name(productEntryData);
									else if(productEntry->get_product_version().empty())
										productEntry->set_product_version(productEntryData);
									else
										productEntry->add_product_data(productEntryData);
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
						std::cout << "WARNING : Unsupported packing method (" << pbo_entry->get_packing_method() << ") !" << std::endl;
						break;
				}
			}

			this->add_entry(pbo_entry);

			if(this->pbo_file.tellg() == (std::streampos)-1)
				throw std::logic_error("No zero entry found");
		}

		int dataOffset = (int)this->pbo_file.tellg();
		int leftDataLength;
		for(int i = 0; i < size(); i++)
		{
			entry* pbo_entry = this->get_entry(i);
			pbo_entry->set_data_offset(dataOffset);

			dataOffset = dataOffset + pbo_entry->get_data_size();
			leftDataLength = pbo_entry->get_data_size();

			char signatureData[SIGNATURE_BUFFER_SIZE];
			while(leftDataLength > 0)
			{
				if(leftDataLength > SIGNATURE_BUFFER_SIZE)
				{
					this->pbo_file.read(signatureData, SIGNATURE_BUFFER_SIZE);
					SHA1_Update(&this->signature_context, &signatureData, SIGNATURE_BUFFER_SIZE);
				}
				else
				{
					this->pbo_file.read(signatureData, leftDataLength);
					SHA1_Update(&this->signature_context, &signatureData, leftDataLength);
				}

				leftDataLength = leftDataLength - SIGNATURE_BUFFER_SIZE;
			}
		}

		if((fileLength - dataOffset) == HEADER_ENTRY_DEFAULT_SIZE)
		{
			this->pbo_file.get();

			char pbo_signature_digest[SHA_DIGEST_LENGTH];
			this->pbo_file.read(pbo_signature_digest, SHA_DIGEST_LENGTH);

			this->set_pbo_signature(pbo_signature_digest);

			char file_signature_digest[SHA_DIGEST_LENGTH];
			SHA1_Final((unsigned char*)file_signature_digest, &this->signature_context);

			this->set_file_signature(file_signature_digest);
		}
		else
			throw std::logic_error("Signature not found");

		if(dataOffset > fileLength)
			throw std::logic_error("Is too small");
	}

	pbo::~pbo()
	{
		OPENSSL_cleanse(&this->signature_context, sizeof(this->signature_context));

		for(int i = 0; i < this->size(); i++)
			delete this->entries[i];

		this->pbo_file.close();
	}
}
