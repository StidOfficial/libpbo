#include "pbo.hpp"

#include <iostream>
#include <cstring>
#include <cerrno>

#define PACKING_BUFFER_SIZE			4096
#define SIGNATURE_BUFFER_SIZE		1024
#define HEADER_ENTRY_DEFAULT_SIZE	21

namespace PBO
{
	PBO::PBO(std::filesystem::path file_path, bool signed_file)
		: PBO(signed_file)
	{
		open(file_path);
	}

	PBO::PBO(bool signed_file)
		: std::vector<std::shared_ptr<Entry>>(), m_file()
	{
		m_signed = signed_file;
	}

	std::filesystem::path PBO::get_path()
	{
		return m_path;
	}

	void PBO::signed_file(bool signed_file)
	{
		m_signed = signed_file;
	}

	bool PBO::is_signed()
	{
		return m_signed;
	}

	void PBO::set_signature(char *digest)
	{
		m_checksum.resize(SHA_DIGEST_LENGTH * 2);
		for(int i = 0; i < SHA_DIGEST_LENGTH; i++)
			sprintf(&m_checksum[i * 2], "%hhx", digest[i]);
	}

	std::string &PBO::signature()
	{
		return m_checksum;
	}

	void PBO::set_file_signature(char *digest)
	{
		m_file_checksum.resize(SHA_DIGEST_LENGTH * 2);
		for(int i = 0; i < SHA_DIGEST_LENGTH; i++)
			sprintf(&m_file_checksum[i * 2], "%hhx", digest[i]);
	}

	std::string &PBO::file_signature()
	{
		return m_file_checksum;
	}

	void PBO::open(std::filesystem::path file_path)
	{
		m_path = file_path;

		if(m_path.extension() == ".ebo")
			std::cout << "WARNING : .ebo is encrypted file" << std::endl;

		m_file.close();

		clear();

		if(is_signed())
			if(!SHA1_Init(&m_sha_context))
				throw std::logic_error("Failed to intialize SHA1");

		if(std::filesystem::is_directory(m_path))
			throw std::logic_error(std::strerror(EISDIR));
	}

	void PBO::read(Entry* &entry)
	{
		std::string entry_path;
		uint32_t entry_packing_method;
		uint32_t entry_original_size;
		uint32_t entry_reserved;
		uint32_t entry_timestamp;
		uint32_t entry_data_size;

		std::string property_key;
		std::string property_value;

		read(entry_path);
		read(entry_packing_method);
		read(entry_original_size);
		read(entry_reserved);
		read(entry_timestamp);
		read(entry_data_size);

		entry = new Entry();

		entry->set_path(entry_path);
		entry->set_packing_method(entry_packing_method);
		entry->set_original_size(entry_original_size);
		entry->set_reserved(entry_reserved);
		entry->set_timestamp(entry_timestamp);
		entry->set_data_size(entry_data_size);

		if(entry->is_zero_entry())
			return;

		switch(entry->get_packing_method())
		{
			case PackingMethod::Version:
			{
				do
				{
					read(property_key);
					if(property_key.empty())
						break;

					read(property_value);
					if(property_value.empty())
						break;

					entry->get_product_entry().emplace(property_key, property_value);
					
					property_key = "";
					property_value = "";
				}
				while(true);

				if(!property_key.empty())
					entry->get_product_entry().emplace(property_key, property_value);

				break;
			}
			case PackingMethod::Uncompressed:
				break;
			case PackingMethod::Compressed:
				break;
			case PackingMethod::Encrypted:
				std::cout << "WARNING : Encrypted method is unavailable for entry " << entry->get_path() << " !" << std::endl;
				break;
			default:
				std::cout << "WARNING : Unsupported packing method (" << entry->get_packing_method() << ") !" << std::endl;
				break;
		}
	}

	void PBO::read(uint32_t &value)
	{
		read(reinterpret_cast<char*>(&value), sizeof(value));
	}

	void PBO::read(std::string &text)
	{
		std::getline(m_file, text, '\0');

		if(is_signed())
			SHA1_Update(&m_sha_context, text.c_str(), text.size() + 1);
	}

	void PBO::read(char *s, std::streamsize n)
	{
		m_file.read(s, n);

		if(is_signed())
			SHA1_Update(&m_sha_context, s, static_cast<size_t>(n));
	}

	void PBO::write(Entry *entry)
	{
		write(entry->get_path());
		write(entry->get_packing_method());
		write(entry->get_original_size());
		write(entry->get_reserved());
		write(entry->get_timestamp());
		write(entry->get_data_size());

		switch(entry->get_packing_method())
		{
			case PackingMethod::Version:
			{
				for(auto &product_entry : entry->get_product_entry())
				{
					if(product_entry.first.empty())
						break;

					write(product_entry.first);

					if(product_entry.second.empty())
						break;

					write(product_entry.second);
				}

				write(NULL_PRODUCT_ENTRY);
				break;
			}
			case PackingMethod::Uncompressed:
				break;
			case PackingMethod::Compressed:
			case PackingMethod::Encrypted:
				throw std::logic_error("Packed method is unavailable !");
				break;
			default:
				throw std::logic_error("Unsupported packing method");
				break;
		}
	}

	void PBO::write(uint32_t value)
	{
		write(reinterpret_cast<const char*>(&value), sizeof(uint32_t));
	}

	void PBO::write(std::string text)
	{
		write(text.c_str(), text.length() + 1);
	}

	void PBO::write(const char *s, std::streamsize n)
	{
		m_file.write(s, n);

		if(is_signed())
			SHA1_Update(&m_sha_context, s, static_cast<size_t>(n));
	}

	void PBO::pack()
	{
		m_file.open(m_path, std::ofstream::out | std::ofstream::binary);
		if(!m_file.is_open())
			throw std::logic_error(std::strerror(errno));

		// Write all entries
		for(auto &entry : *this)
			write(entry.get());

		// Write zero entry to end the headers
		char zeroEntry[HEADER_ENTRY_DEFAULT_SIZE] = {0};
		write(zeroEntry, sizeof(zeroEntry));

		// Write all data entries
		char entryFileBuffer[PACKING_BUFFER_SIZE];
		for(auto &entry : *this)
		{
			switch(entry->get_packing_method())
			{
				case PackingMethod::Uncompressed:
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

		// Write data signature
		char file_digest[SHA_DIGEST_LENGTH];
		SHA1_Final(reinterpret_cast<unsigned char*>(&file_digest), &m_sha_context);

		m_file.write("\0", 1);
		m_file.write(file_digest, sizeof(file_digest));

		set_signature(file_digest);
		set_file_signature(file_digest);
	}

	void PBO::unpack()
	{
		m_file.open(m_path, std::fstream::in | std::fstream::binary);
		if(!m_file.is_open())
			throw std::logic_error(std::strerror(errno));

		m_file.seekg(0, m_file.end);
		std::streampos file_size = m_file.tellg();
		m_file.seekg(0, m_file.beg);

		Entry *entry = nullptr;
		while(!m_file.eof())
		{
			if((m_file.tellg() + std::streampos(HEADER_ENTRY_DEFAULT_SIZE)) > file_size)
				throw std::logic_error("Header entry is too small");

			read(entry);
			if(entry->is_zero_entry())
			{
				delete entry;
				break;
			}

			emplace_back(entry);

			if(m_file.eof())
				throw std::logic_error("No zero entry found");
		}

		// Check entry data integrity
		std::streampos data_offset = m_file.tellg();
		int leftDataLength;

		for(auto &entry : *this)
		{
			entry->set_data_offset(data_offset);

			data_offset = data_offset + std::streamsize(entry->get_data_size());
			leftDataLength = entry->get_data_size();

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

		// Get and check signatures
		if((file_size - data_offset) == HEADER_ENTRY_DEFAULT_SIZE)
		{
			m_file.get(); // entry path always empty, skip

			char pbo_digest[SHA_DIGEST_LENGTH];
			m_file.read(pbo_digest, SHA_DIGEST_LENGTH);

			set_signature(pbo_digest);

			char file_digest[SHA_DIGEST_LENGTH];
			SHA1_Final(reinterpret_cast<unsigned char*>(file_digest), &m_sha_context);

			set_file_signature(file_digest);
		}
		else if(is_signed())
				throw std::logic_error("Signature not found");

		if(data_offset > file_size)
			throw std::logic_error("Is too small");
	}

	PBO::~PBO()
	{
		if(is_signed())
			OPENSSL_cleanse(&m_sha_context, sizeof(m_sha_context));

		m_file.close();
	}
}
