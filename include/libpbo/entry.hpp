#ifndef ENTRY_HPP
#define ENTRY_HPP

#include <string>
#include <ctime>
#include <cstdint>
#include <ios>

#include "pboDll.h"
#include "productentry.hpp"

#define ENTRY_PATH_LEN	128-20

#define PACKINGMETHOD_UNCOMPRESSED	0x0
#define PACKINGMETHOD_COMPRESSED	0x43707273
#define PACKINGMETHOD_VERSION		0x56657273
#define PACKINGMETHOD_ENCRYPTED	0x456e6372
#define PACKINGMETHOD_NULL		0xffffffff

namespace pbo
{
	class PBODLL_API entry
	{
	public:
		entry();
		~entry();
		void set_file_path(std::string file_path);
		std::string get_file_path();
		void set_path(std::string path);
		std::string get_path();
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
		productentry* &get_product_entry();
		bool is_product_entry();
		bool is_file_entry();
		bool is_zero_entry();
	private:
		std::string file_path;
		std::string path;
		uint32_t packing_method;
		uint32_t original_size;
		uint32_t reserved;
		uint32_t timestamp;
		uint32_t data_size;
		std::streampos data_offset;
		productentry *product_entry;
	};
}

#endif
