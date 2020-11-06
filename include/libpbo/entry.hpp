#pragma once

#include <string>
#include <ctime>
#include <cstdint>
#include <ios>
#if __has_include(<filesystem>)
#include <filesystem>
#elif __has_include(<experimental/filesystem>)
#include <experimental/filesystem>
namespace std {
	namespace filesystem = std::experimental::filesystem;
}
#else
#error "No filesystem header found !"
#endif

#include "pboDll.h"
#include "properties.hpp"

#define ENTRY_PATH_LEN	128-20

namespace PBO
{
	enum PackingMethod
	{
		Uncompressed = 0x0,
		Compressed = 0x43707273,
		Version = 0x56657273,
		Encrypted = 0x456e6372,
		Null = 0xffffffff
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
		Properties &get_properties();
		bool is_version_entry();
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
		Properties m_properties;
	};
}