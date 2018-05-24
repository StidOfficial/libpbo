#include <iostream>
#include <experimental/filesystem>
#include <ctime>
#include <libpbo/pbo.hpp>
#include <libpbo/productentry.hpp>
#include <cstdlib>

void usage();

int main(int argc, char **argv)
{
	int exit_code = EXIT_SUCCESS;
	std::string file_path;

	for(int i = 1;i < argc; i++)
	{
		std::string arg = argv[i];
		if(arg == "-h" || arg == "--help")
		{
			usage();
			return EXIT_SUCCESS;
		}
		else if(arg.find("-", 0, 2) != std::string::npos)
		{
			std::cerr << "pboinfo: invalid option « " << arg << " »" << std::endl;
		}
		else
		{
			file_path = arg;
		}
	}

	if(file_path.empty())
	{
		usage();
		return EXIT_SUCCESS;
	}

	pbo::pbo* pbo_file = new pbo::pbo(file_path);
	try
	{
		pbo_file->unpack();

		for(int i = 0; i < (int)pbo_file->size(); i++)
		{
			pbo::entry *entry = pbo_file->get_entry(i);
			std::cout << "Packing method: ";
			switch(entry->get_packing_method())
			{
				case PACKINGMETHOD_NULL:
					std::cout << "Null method" << std::endl;
					break;
				case PACKINGMETHOD_UNCOMPRESSED:
					std::cout << "Uncompressed" << std::endl;
					break;
				case PACKINGMETHOD_COMPRESSED:
					std::cout << "Compressed" << std::endl;
					break;
				case PACKINGMETHOD_VERSION:
					std::cout << "Version" << std::endl;
					break;
				default:
					std::cout << "Unknown #" << entry->get_packing_method() << std::endl;
			}

			if(entry->is_file_entry())
			{
				std::cout << "Entry path: " << entry->get_path() << std::endl;
				std::cout << "Original size: " << entry->get_original_size() << " bytes" << std::endl;
				std::cout << "Reserved: " << entry->get_reserved() << " bytes" << std::endl;
				std::time_t timestamp = entry->get_timestamp();
				std::cout << "Timestamp: " << std::ctime(&timestamp);
				std::cout << "Data size: " << entry->get_data_size() << " bytes" << std::endl;
			}
			else if(entry->is_product_entry())
			{
				pbo::productentry* product_entry = entry->get_product_entry();
				if(product_entry)
				{
					std::cout << "Entry name : " << product_entry->get_entry_name() << std::endl;
					std::cout << "Product name : " << product_entry->get_product_name() << std::endl;
					std::cout << "Product version : " << product_entry->get_product_version() << std::endl;

					for(int i = 0; i < (int)product_entry->get_product_data_size(); i++)
					{
						std::cout << "Product data " << i << " : " << product_entry->get_product_data(i) << std::endl;
					}
				}
			}

			std::cout << std::endl;
		}

		std::cout << "PBO signature: " << pbo_file->pbo_signature() << std::endl;
		std::cout << "File signature: " << pbo_file->file_signature() << std::endl;
		std::cout << "Valid signature: " << ((pbo_file->pbo_signature() == pbo_file->file_signature()) ? "true" : "false") << std::endl;
	}
	catch(std::exception const &e)
	{
		std::cerr << "pboinfo: " << e.what() << std::endl;
		exit_code = EXIT_FAILURE;
	}

	delete pbo_file;
	return exit_code;
}

void usage()
{
	std::cout << "Usage: pboinfo [FILE]" << std::endl;
}
