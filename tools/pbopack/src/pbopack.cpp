#include <experimental/filesystem>
#include <cstring>
#include <cerrno>
#include <iostream>
#include <libpbo/pbo.hpp>
#include <cstdlib>

namespace filesystem = std::experimental::filesystem;

void usage();

int main(int argc, char **argv)
{
	int exit_code = EXIT_SUCCESS;
	std::string directory_path;
	std::string file_path;
	std::string product_name;
	std::string product_version;

	for(int i = 1;i < argc; i++)
	{
		std::string arg = argv[i];
		if(arg == "-h" || arg == "--help")
		{
			usage();
			return EXIT_SUCCESS;
		}
		else if(arg.find("--name=", 0) == 0)
		{
			std::cout << arg.substr(7) << std::endl;
		}
		else if(arg.find("--version=", 0) == 0)
		{
			std::cout << arg.substr(10) << std::endl;
		}
		else if(arg.find("--", 0, 2) == 0 || arg.find("-", 0, 1) == 0)
		{
			std::cerr << "pbopack: invalid option « " << arg << " »" << std::endl;
		}
		else
		{
			if(directory_path.empty())
				directory_path = arg;
			else if(file_path.empty())
				file_path = arg;
		}
	}

	if(directory_path.empty() || file_path.empty())
	{
		usage();
		return EXIT_SUCCESS;
	}

	if(!filesystem::exists(directory_path))
	{
		std::cerr << "pbopack: " << directory_path << ": " << std::strerror(ENOENT) << std::endl;
		return EXIT_FAILURE;
	}

	if(!filesystem::is_directory(directory_path))
	{
		std::cerr << "pbopack: " << directory_path << ": " << std::strerror(ENOTDIR) << std::endl;
		return EXIT_FAILURE;
	}

	if(product_name.empty())
	{
		filesystem::path filesystem_path = file_path;
		product_name = filesystem_path.filename();
	}

	filesystem::path base_dir = directory_path + "/";
	std::string base_dir_path = base_dir.string();

	pbo::pbo* pbo_file = new pbo::pbo(file_path);
	pbo::entry *product_entry = new pbo::entry();
	product_entry->set_packing_method(PACKINGMETHOD_PRODUCTENTRY);
	pbo::productentry *product = product_entry->get_product_entry();
	product->set_entry_name("prefix");
	product->set_product_name(product_name);
	product->set_product_version("");
	pbo_file->add_entry(product_entry);

	for(auto i = filesystem::recursive_directory_iterator(base_dir); i != filesystem::recursive_directory_iterator(); i++)
	{
		if(!filesystem::is_directory(i->status()))
		{
			pbo::entry* file_entry = new pbo::entry();
			file_entry->set_packing_method(PACKINGMETHOD_UNCOMPRESSED);
			std::string entry_file_path = i->path().string();
			file_entry->set_path(entry_file_path.substr(base_dir_path.length()));
			file_entry->set_file_path(i->path().string());
			pbo_file->add_entry(file_entry);
		}
	}

	try
	{
		pbo_file->pack();
	}
	catch(std::exception const &e)
	{
		std::cerr << "pbopack : " << e.what() << std::endl;
		exit_code = EXIT_FAILURE;
	}

	delete pbo_file;
	return exit_code;
}

void usage()
{
	std::cout << "Usage: pbopack [DIRECTORY] [FILE]" << std::endl;
	std::cout << "Arguments :" << std::endl;
	std::cout << "\t--name<NAME>\t\tSet product name" << std::endl;
	std::cout << "\t--version<VERSION>\tSet product version" << std::endl;
}
