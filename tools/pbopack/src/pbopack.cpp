#include <filesystem>
#include <cstring>
#include <cerrno>
#include <iostream>
#include <libpbo/pbo.hpp>
#include <cstdlib>

void usage();

int main(int argc, char **argv)
{
	std::string directory_path;
	std::string file_path;
	std::string product_name = "";
	std::string product_version = "";

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
			product_name = arg.substr(7);
		}
		else if(arg.find("--version=", 0) == 0)
		{
			product_version = arg.substr(10);
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

	if(!std::filesystem::exists(directory_path))
	{
		std::cerr << "pbopack: " << directory_path << ": " << std::strerror(ENOENT) << std::endl;
		return EXIT_FAILURE;
	}

	if(!std::filesystem::is_directory(directory_path))
	{
		std::cerr << "pbopack: " << directory_path << ": " << std::strerror(ENOTDIR) << std::endl;
		return EXIT_FAILURE;
	}

	if(product_name.empty())
	{
		std::filesystem::path filesystem_path = file_path;
		product_name = filesystem_path.filename().generic_string();
	}

	std::filesystem::path base_dir = directory_path + "/";
	std::string base_dir_path = base_dir.string();

	PBO::PBO pbo_file(file_path);
	PBO::Entry *product_entry = new PBO::Entry();
	product_entry->set_packing_method(PACKINGMETHOD_VERSION);
	product_entry->get_product_entry().set_prefix("a3");
	product_entry->get_product_entry().set_product(product_name);
	product_entry->get_product_entry().set_version(product_version);

	pbo_file.emplace_back(product_entry);

	for(auto i = std::filesystem::recursive_directory_iterator(base_dir); i != std::filesystem::recursive_directory_iterator(); i++)
	{
		if(!std::filesystem::is_directory(i->status()))
		{
			PBO::Entry* file_entry = new PBO::Entry();
			file_entry->set_packing_method(PACKINGMETHOD_UNCOMPRESSED);
			std::string entry_file_path = i->path().string();
			file_entry->set_path(entry_file_path.substr(base_dir_path.length()));
			file_entry->set_file_path(i->path().string());

			pbo_file.emplace_back(file_entry);
		}
	}

	try
	{
		pbo_file.pack();
	}
	catch(std::exception const &e)
	{
		std::cerr << "pbopack : " << e.what() << std::endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

void usage()
{
	std::cout << "Usage: pbopack [OPTION]... [DIRECTORY] [FILE]" << std::endl;
	std::cout << "Options :" << std::endl;
	std::cout << "\t--name=<NAME>\t\tSet product name" << std::endl;
	std::cout << "\t--version=<VERSION>\tSet product version" << std::endl;
}
