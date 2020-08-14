#pragma once

#include <string>
#include <ctime>
#include <cstdint>
#include <ios>
#include <filesystem>

#include "pboDll.h"
#include "productentry.hpp"

#define ENTRY_PATH_LEN				128-20

#define PACKINGMETHOD_UNCOMPRESSED	0x0
#define PACKINGMETHOD_COMPRESSED	0x43707273
#define PACKINGMETHOD_VERSION		0x56657273
#define PACKINGMETHOD_ENCRYPTED		0x456e6372
#define PACKINGMETHOD_NULL			0xffffffff

namespace PBO
{
	enum PackingMethod
	{
		Uncompressed = PACKINGMETHOD_UNCOMPRESSED,
		Compressed = PACKINGMETHOD_COMPRESSED,
		Version = PACKINGMETHOD_VERSION,
		Encrypted = PACKINGMETHOD_ENCRYPTED,
		Null = PACKINGMETHOD_NULL
	};

	inline std::string to_string(PackingMethod packing_method)
	{
		switch(packing_method)
		{
			case Uncompressed:
				return "Uncompressed";
			case Compressed:
				return "Compressed";
			case Version:
				return "Version";
			case Encrypted:
				return "Encrypted";
			case Null:
				return "Null";
			default:
				return "Unknown";
		}
	}

	class PBODLL_API Entry
	{
	public:
		Entry();
		~Entry();
		void set_file_path(std::filesystem::path file_path);
		std::filesystem::path get_file_path();
		void set_path(std::filesystem::path path);
		std::filesystem::path get_path();
		void set_packing_method(uint32_t packing_method);
		uint32_t get_packing_method();
		void set_original_size(uint32_t original_size);
		uint32_t get_original_size();
		void set_reserved(uint32_t reserved);
		uint32_t get_reserved();
		void set_timestamp(uint32_t timestamp);
		uint32_t get_timestamp();
		void set_data_size(uint32_t data_size);
		int get_data_size();
		void set_data_offset(std::streampos data_offset);
		std::streampos get_data_offset();
		ProductEntry &get_product_entry();
		bool is_product_entry();
		bool is_file_entry();
		bool is_zero_entry();
	private:
		std::filesystem::path m_file_path;
		std::filesystem::path m_path;
		uint32_t m_packing_method;
		uint32_t m_original_size;
		uint32_t m_reserved;
		uint32_t m_timestamp;
		uint32_t m_data_size;
		std::streampos m_data_offset;
		ProductEntry m_product_entry;
	};
}