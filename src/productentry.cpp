#include "productentry.hpp"

namespace pbo
{
	productentry::productentry()
	{
	}

	void productentry::set_entry_name(std::string entry_name)
	{
		this->entry_name = entry_name;
	}

	std::string productentry::get_entry_name()
	{
		return this->entry_name;
	}

	void productentry::set_product_name(std::string product_name)
	{
		this->product_name = product_name;
	}

	std::string productentry::get_product_name()
	{
		return this->product_name;
	}

	void productentry::set_product_version(std::string product_version)
	{
		this->product_version = product_version;
	}

	std::string productentry::get_product_version()
	{
		return this->product_version;
	}

	size_t productentry::get_product_data_size()
	{
		return this->product_data.size();
	}

	void productentry::add_product_data(std::string data)
	{
		this->product_data.resize(this->get_product_data_size() + 1);
		this->product_data[this->get_product_data_size() - 1] = data;
	}

	void productentry::remove_product_data(int index)
	{
		this->product_data.erase(this->product_data.begin(), this->product_data.begin() + index);
	}

	std::string productentry::get_product_data(int index)
	{
		return this->product_data[index];
	}
}
