#include "productentry.hpp"

namespace PBO
{
	ProductEntry::ProductEntry()
		: std::map<std::string, std::string>()
	{
	}

	void ProductEntry::set_prefix(std::string prefix)
	{
		emplace("prefix", prefix);
	}

	std::string ProductEntry::get_prefix()
	{
		auto it = find("prefix");
		if(it != end())
			return it->second;

		return std::string();
	}

	void ProductEntry::set_product(std::string product)
	{
		emplace("product", product);
	}

	std::string ProductEntry::get_product()
	{
		auto it = find("product");
		if(it != end())
			return it->second;

		return std::string();
	}

	void ProductEntry::set_version(std::string version)
	{
		emplace("version", version);
	}

	std::string ProductEntry::get_version()
	{
		auto it = find("version");
		if(it != end())
			return it->second;

		return std::string();
	}
}
