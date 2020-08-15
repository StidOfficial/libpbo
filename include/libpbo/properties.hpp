#pragma once

#include <map>
#include <string>

#define NULL_PROPERTY	""

#include "pboDll.h"

namespace PBO
{
	class PBODLL_API Properties : public std::map<std::string, std::string>
	{
	public:
		Properties();
		void set_prefix(std::string entry_name);
		std::string get_prefix();
		void set_product(std::string product);
		std::string get_product();
		void set_version(std::string version);
		std::string get_version();
	};
}