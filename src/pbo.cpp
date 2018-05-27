#include "pbo.hpp"

#include <iostream>
#include <cstring>
#include <cerrno>
#include <experimental/filesystem>

#define PACKING_BUFFER_SIZE 4096
#define SIGNATURE_BUFFER_SIZE 1024
#define HEADER_ENTRY_DEFAULT_SIZE 21

#define ULONG_WRITE(var, value) \
	var[0] = (int)(value & 0xFF); \
	var[1] = (int)((value >> 8) & 0xFF); \
	var[2] = (int)((value >> 16) & 0xFF); \
	var[3] = (int)((value >> 24) & 0xFF);

namespace filesystem = std::experimental::filesystem;
namespace pbo
{
	pbo::pbo(std::string file_path, bool signed_file) : m_file()
	{
		m_path = file_path;
		m_signed = signed_file;
	}

	void pbo::signed_file(bool signed_file)
	{
		m_signed = signed_file;
	}

	bool pbo::is_signed()
	{
		return m_signed;
	}

	void pbo::add_entry(entry *entry)
	{
		m_entries.resize(this->size() + 1);
		m_entries[this->size() - 1] = entry;
	}

	void pbo::remove_entry(int index)
	{
		m_entries.erase(m_entries.begin(), m_entries.begin() + index);
	}

	entry* &pbo::get_entry(int index)
	{
		return m_entries[index];
	}

	size_t pbo::size()
	{
		return m_entries.size();
	}

	void pbo::set_signature(char* digest)
	{
		m_checksum.resize(SHA_DIGEST_LENGTH * 2);
		for(int i = 0; i < SHA_DIGEST_LENGTH; i++)
			sprintf(&m_checksum[i * 2], "%hhx", digest[i]);
	}

	std::string& pbo::signature()
	{
		return m_checksum;
	}

	void pbo::set_file_signature(char* digest)
	{
		m_file_checksum.resize(SHA_DIGEST_LENGTH * 2);
		for(int i = 0; i < SHA_DIGEST_LENGTH; i++)
			sprintf(&m_file_checksum[i * 2], "%hhx", digest[i]);
	}

	std::string& pbo::file_signature()
	{
		return m_file_checksum;
	}

	void pbo::read(char* s, std::streamsize n)
	{
		m_file.read(s, n);
		SHA1_Update(&m_sha_context, s, (size_t)n);
	}

	void pbo::write(const char* s, std::streamsize n)
	{
		m_file.write(s, n);
		SHA1_Update(&m_sha_context, s, (size_t)n);
	}

	void pbo::pack()
	{
		if(is_signed())
			if(!SHA1_Init(&m_sha_context))
				throw std::logic_error("Failed to intialize SHA1");

		if(filesystem::is_directory(m_path))
			throw std::logic_error(std::strerror(EISDIR));

		m_file.open(m_path, std::ofstream::out | std::ofstream::binary);
		if(!m_file.is_open())
			throw std::logic_error(std::strerror(errno));

		char ulong_buffer[4];
		for(size_t i = 0; i < this->size(); i++)
		{
			entry *entry = m_entries[i];
			write(entry->get_path().c_str(), entry->get_path().length() + 1);

			ULONG_WRITE(ulong_buffer, entry->get_packing_method());
			write(ulong_buffer, sizeof(uint32_t));

			ULONG_WRITE(ulong_buffer, entry->get_original_size());
			write(ulong_buffer, sizeof(uint32_t));

			ULONG_WRITE(ulong_buffer, entry->get_reserved());
			write(ulong_buffer, sizeof(uint32_t));

			ULONG_WRITE(ulong_buffer, entry->get_timestamp());
			write(ulong_buffer, sizeof(uint32_t));

			ULONG_WRITE(ulong_buffer, entry->get_data_size());
			write(ulong_buffer, sizeof(uint32_t));

			switch(entry->get_packing_method())
			{
				case PACKINGMETHOD_VERSION:
				{
					productentry* productEntry = entry->get_product_entry();
					for(size_t i = 0; i < productEntry->size(); i++)
					{
						if(productEntry->get(i).empty())
							break;

						write(productEntry->get(i).c_str(), productEntry->get(i).length() + 1);
					}

					write("\0", 1);
					break;
				}
				case PACKINGMETHOD_UNCOMPRESSED:
					break;
				case PACKINGMETHOD_COMPRESSED:
				case PACKINGMETHOD_ENCRYPTED:
					throw std::logic_error("Packed method is unavailable !");
					break;
				default:
					throw std::logic_error("Unsupported packing method");
					break;
			}
		}

		char zeroEntry[HEADER_ENTRY_DEFAULT_SIZE] = {0};
		write(zeroEntry, sizeof(zeroEntry));

		char entryFileBuffer[PACKING_BUFFER_SIZE];
		for(size_t i = 0; i < this->size(); i++)
		{
			entry *entry = m_entries[i];
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
						write(entryFileBuffer, entryFile.gcount());
					}

					entryFile.close();
					break;
				}
				default:
					break;
			}
		}

		char file_digest[SHA_DIGEST_LENGTH];
		SHA1_Final(reinterpret_cast<unsigned char*>(&file_digest), &m_sha_context);
		this->set_signature(file_digest);
		this->set_file_signature(file_digest);

		m_file.write("\0", 1);
		m_file.write(file_digest, sizeof(file_digest));
	}

	void pbo::unpack()
	{
		if(is_signed())
			if(!SHA1_Init(&m_sha_context))
				throw std::logic_error("Failed to intialize SHA1");

		if(filesystem::is_directory(m_path))
			throw std::logic_error(std::strerror(EISDIR));

		m_file.open(m_path, std::fstream::in | std::fstream::binary);
		if(!m_file.is_open())
			throw std::logic_error(std::strerror(errno));

		m_file.seekg(0, m_file.end);
		std::streampos file_size = m_file.tellg();
		m_file.seekg(0, m_file.beg);

		std::string productEntryData;
		std::string entryPath;
		char uLong[4];
		while(!m_file.eof())
		{
			if((m_file.tellg() + std::streampos(HEADER_ENTRY_DEFAULT_SIZE)) > file_size)
				throw std::logic_error("Header entry is too small");

			entry* pbo_entry = new entry();

			std::getline(m_file, entryPath, '\0');
			if(is_signed())
				SHA1_Update(&m_sha_context, entryPath.c_str(), entryPath.size() + 1);
			pbo_entry->set_path(entryPath);

			read(uLong, sizeof(uLong));
			pbo_entry->set_packing_method(*((uint32_t*)uLong));

			read(uLong, sizeof(uLong));
			pbo_entry->set_original_size(*((uint32_t*)uLong));

			read(uLong, sizeof(uLong));
			pbo_entry->set_reserved(*((uint32_t*)uLong));

			read(uLong, sizeof(uLong));
			pbo_entry->set_timestamp(*((uint32_t*)uLong));

			read(uLong, sizeof(uLong));
			pbo_entry->set_data_size(*((uint32_t*)uLong));

			if(pbo_entry->is_zero_entry())
			{
				delete pbo_entry;
				break;
			}
			else
			{
				switch(pbo_entry->get_packing_method())
				{
					case PACKINGMETHOD_VERSION:
					{
						productentry* product_entry = pbo_entry->get_product_entry();

						do
						{
							std::getline(m_file, productEntryData, '\0');
							if(is_signed())
								SHA1_Update(&m_sha_context, productEntryData.c_str(), productEntryData.size() + 1);

							if(!productEntryData.empty())
								product_entry->add(productEntryData);
						}
						while(!product_entry->get(product_entry->size() - 1).empty() && !productEntryData.empty());

						break;
					}
					case PACKINGMETHOD_UNCOMPRESSED:
						break;
					case PACKINGMETHOD_COMPRESSED:
					case PACKINGMETHOD_ENCRYPTED:
						std::cout << "WARNING : Compressed and encrypted method is unavailable for entry " << pbo_entry->get_path() << " !" << std::endl;
						break;
					default:
						std::cout << "WARNING : Unsupported packing method (" << pbo_entry->get_packing_method() << ") !" << std::endl;
						break;
				}
			}

			this->add_entry(pbo_entry);

			if(m_file.eof())
				throw std::logic_error("No zero entry found");
		}

		std::streampos data_offset = m_file.tellg();
		int leftDataLength;
		for(size_t i = 0; i < size(); i++)
		{
			entry* pbo_entry = this->get_entry(i);
			pbo_entry->set_data_offset(data_offset);

			data_offset = data_offset + std::streamsize(pbo_entry->get_data_size());
			leftDataLength = pbo_entry->get_data_size();

			char signatureData[SIGNATURE_BUFFER_SIZE];
			while(leftDataLength > 0)
			{
				if(leftDataLength > SIGNATURE_BUFFER_SIZE)
					read(signatureData, SIGNATURE_BUFFER_SIZE);
				else
					read(signatureData, leftDataLength);

				leftDataLength = leftDataLength - SIGNATURE_BUFFER_SIZE;
			}
		}

		if((file_size - data_offset) == HEADER_ENTRY_DEFAULT_SIZE)
		{
			m_file.get();

			char pbo_digest[SHA_DIGEST_LENGTH];
			m_file.read(pbo_digest, SHA_DIGEST_LENGTH);

			this->set_signature(pbo_digest);

			char file_digest[SHA_DIGEST_LENGTH];
			SHA1_Final(reinterpret_cast<unsigned char*>(file_digest), &m_sha_context);

			this->set_file_signature(file_digest);
		}
		else
			if(is_signed())
				throw std::logic_error("Signature not found");

		if(data_offset > file_size)
			throw std::logic_error("Is too small");
	}

	pbo::~pbo()
	{
		if(is_signed())
			OPENSSL_cleanse(&m_sha_context, sizeof(m_sha_context));

		for(size_t i = 0; i < this->size(); i++)
			delete m_entries[i];

		m_file.close();
	}
}
