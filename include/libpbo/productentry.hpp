#ifndef PRODUCTENTRY_HPP
#define PRODUCTENTRY_HPP

#include <vector>
#include <string>

namespace pbo
{
	class productentry
	{
	public:
		productentry();
		void set_entry_name(std::string entry_name);
		std::string get_entry_name();
		void set_product_name(std::string product_name);
		std::string get_product_name();
		void set_product_version(std::string product_version);
		std::string get_product_version();
		int get_product_data_size();
		void add_product_data(std::string data);
		void remove_product_data(int index);
		std::string get_product_data(int index);
	private:
		std::string entry_name;
		std::string product_name;
		std::string product_version;
		std::vector<std::string> product_data;
	};
}

#endif
