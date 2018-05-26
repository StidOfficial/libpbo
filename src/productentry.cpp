#include "productentry.hpp"

#define ENTRY_NAME_INDEX	0
#define PRODUCT_NAME_INDEX	1
#define PRODUCT_VERSION_INDEX	2

namespace pbo
{
	productentry::productentry()
	{
	}

	void productentry::set_entry_name(std::string entry_name)
	{
		if(size() > ENTRY_NAME_INDEX)
			product_data[ENTRY_NAME_INDEX] = entry_name;
		else
			add(entry_name);
	}

	std::string productentry::get_entry_name()
	{
		if(size() > ENTRY_NAME_INDEX)
			return product_data[ENTRY_NAME_INDEX];
		else
			return std::string();
	}

	void productentry::set_name(std::string name)
	{
		if(size() >= PRODUCT_NAME_INDEX)
			product_data[PRODUCT_NAME_INDEX] = name;
		else
			add(name);
	}

	std::string productentry::get_name()
	{
		if(size() > PRODUCT_NAME_INDEX)
			return product_data[PRODUCT_NAME_INDEX];
		else
			return std::string();
	}

	void productentry::set_version(std::string version)
	{
		if(size() > PRODUCT_VERSION_INDEX)
			product_data[PRODUCT_VERSION_INDEX] = version;
		else
			add(version);
	}

	std::string productentry::get_version()
	{
		if(size() > PRODUCT_VERSION_INDEX)
			return product_data[PRODUCT_VERSION_INDEX];
		else
			return std::string();
	}

	size_t productentry::size()
	{
		return product_data.size();
	}

	void productentry::add(std::string value)
	{
		product_data.resize(product_data.size() + 1);
		product_data[size() - 1] = value;
	}

	void productentry::remove(int index)
	{
		product_data.erase(product_data.begin(), product_data.begin() + index);
	}

	std::string productentry::get(int index)
	{
		if(size() > index)
			return product_data[index];
		else
			return std::string();
	}
}
