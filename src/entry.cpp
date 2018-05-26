#include "entry.hpp"

#include <experimental/filesystem>

namespace filesystem = std::experimental::filesystem;
namespace pbo
{
	entry::entry()
	{
		this->packing_method = PACKINGMETHOD_NULL;
		this->original_size = 0;
		this->reserved = 0;
		this->timestamp = 0;
		this->data_size = 0;
		this->data_offset = -1;
		this->product_entry = NULL;
	}

	void entry::set_file_path(std::string file_path)
	{
		this->file_path = file_path;

		filesystem::path filesystem_path = file_path;
		this->set_data_size((uint32_t)filesystem::file_size(filesystem_path));
		filesystem::file_time_type last_write_time = filesystem::last_write_time(filesystem_path);
		std::time_t entry_time = decltype(last_write_time)::clock::to_time_t(last_write_time);
		this->set_timestamp((uint32_t)entry_time);
	}

	std::string entry::get_file_path()
	{
		return this->file_path;
	}

	void entry::set_path(std::string path)
	{
		if(path.length() > ENTRY_PATH_LEN)
			throw std::length_error("Invalid path length");

		this->path = path;
	}

	std::string entry::get_path()
	{
		return path;
	}

	void entry::set_packing_method(uint32_t packing_method)
	{
		this->packing_method = packing_method;
		if(!this->product_entry && packing_method == PACKINGMETHOD_VERSION)
			this->product_entry = new productentry();
	}

	uint32_t entry::get_packing_method()
	{
		return this->packing_method;
	}

	void entry::set_original_size(uint32_t original_size)
	{
		this->original_size = original_size;
	}

	uint32_t entry::get_original_size()
	{
		return this->original_size;
	}

	void entry::set_reserved(uint32_t reserved)
	{
		this->reserved = reserved;
	}

	uint32_t entry::get_reserved()
	{
		return this->reserved;
	}

	void entry::set_timestamp(uint32_t timestamp)
	{
		this->timestamp = timestamp;
	}

	uint32_t entry::get_timestamp()
	{
		return this->timestamp;
	}

	void entry::set_data_size(uint32_t data_size)
	{
		this->data_size = data_size;
	}

	int entry::get_data_size()
	{
		return this->data_size;
	}

	void entry::set_data_offset(int data_offset)
	{
		this->data_offset = data_offset;
	}

	int entry::get_data_offset()
	{
		return this->data_offset;
	}

	productentry* &entry::get_product_entry()
	{
		return this->product_entry;
	}

	bool entry::is_product_entry()
	{
		return get_packing_method() == PACKINGMETHOD_VERSION;
	}

	bool entry::is_file_entry()
	{
		return get_packing_method() == PACKINGMETHOD_UNCOMPRESSED ||
			get_packing_method() == PACKINGMETHOD_COMPRESSED;
	}

	bool entry::is_zero_entry()
	{
		return this->path.length() == 0 &&
			(this->packing_method == 0 || this->packing_method == PACKINGMETHOD_COMPRESSED) &&
			this->original_size == 0 &&
			this->reserved == 0 &&
			this->timestamp == 0 &&
			this->data_size == 0;
	}

	entry::~entry()
	{
		if(this->product_entry)
			delete this->product_entry;
	}
}
