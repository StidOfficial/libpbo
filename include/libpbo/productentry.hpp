#ifndef PRODUCTENTRY_HPP
#define PRODUCTENTRY_HPP

#include <vector>
#include <string>

#include "pboDll.h"

namespace pbo
{
	class PBODLL_API productentry
	{
	public:
		productentry();
		void set_entry_name(std::string entry_name);
		std::string get_entry_name();
		void set_name(std::string name);
		std::string get_name();
		void set_version(std::string version);
		std::string get_version();
		size_t size();
		void add(std::string value);
		void remove(int index);
		std::string get(int index);
	private:
		std::vector<std::string> product_data;
	};
}

#endif
