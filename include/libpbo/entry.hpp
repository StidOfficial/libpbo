#ifndef ENTRY_HPP
#define ENTRY_HPP

#include <string>
#include <fstream>
#include <ctime>
#include <cstdint>

#include "pboDll.h"
#include "productentry.hpp"

#define PACKINGMETHOD_UNCOMPRESSED	0x0
#define PACKINGMETHOD_PACKED		0x43707273
#define PACKINGMETHOD_PRODUCTENTRY	0x56657273
#define PACKINGMETHOD_NULL			-0x1

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
		void set_data_offset(int data_offset);
		int get_data_offset();
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
		int data_offset;
		productentry *product_entry;
	};
}

#endif
