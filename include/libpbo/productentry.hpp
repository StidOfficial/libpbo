#pragma once

#include <map>
#include <string>

#define NULL_PRODUCT_ENTRY	""

#include "pboDll.h"

namespace PBO
{
	class PBODLL_API ProductEntry : public std::map<std::string, std::string>
	{
	public:
		ProductEntry();
		void set_prefix(std::string entry_name);
		std::string get_prefix();
		void set_product(std::string product);
		std::string get_product();
		void set_version(std::string version);
		std::string get_version();
	};
}