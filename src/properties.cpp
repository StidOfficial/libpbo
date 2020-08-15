#include "properties.hpp"

namespace PBO
{
	Properties::Properties()
		: std::map<std::string, std::string>()
	{
	}

	void Properties::set_prefix(std::string prefix)
	{
		emplace("prefix", prefix);
	}

	std::string Properties::get_prefix()
	{
		auto it = find("prefix");
		if(it != end())
			return it->second;

		return std::string();
	}

	void Properties::set_product(std::string product)
	{
		emplace("product", product);
	}

	std::string Properties::get_product()
	{
		auto it = find("product");
		if(it != end())
			return it->second;

		return std::string();
	}

	void Properties::set_version(std::string version)
	{
		emplace("version", version);
	}

	std::string Properties::get_version()
	{
		auto it = find("version");
		if(it != end())
			return it->second;

		return std::string();
	}
}
