#include <iostream>
#include <ctime>
#include <libpbo/pbo.hpp>
#include <libpbo/productentry.hpp>
#include <cstdlib>

void usage();

int main(int argc, char **argv)
{
	std::string file_path;
	bool signed_file = true;

	for(int i = 1;i < argc; i++)
	{
		std::string arg = argv[i];
		if(arg == "-h" || arg == "--help")
		{
			usage();
			return EXIT_SUCCESS;
		}
		else if(arg == "-us" || arg == "--unsigned")
			signed_file = false;
		else if(arg.find("-", 0, 2) != std::string::npos)
			std::cerr << "pboinfo: invalid option « " << arg << " »" << std::endl;
		else
			file_path = arg;
	}

	if(file_path.empty())
	{
		usage();
		return EXIT_SUCCESS;
	}

	PBO::PBO pbo_file(file_path);
	pbo_file.signed_file(signed_file);
	try
	{
		pbo_file.unpack();

		for(auto entry : pbo_file)
		{
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
				if(entry->get_packing_method() == PACKINGMETHOD_VERSION)
				{
					PBO::ProductEntry productentry = entry->get_product_entry();
					std::cout << "Prefix : " << productentry.get_prefix() << std::endl;
					std::cout << "Product name : " << productentry.get_product() << std::endl;
					std::cout << "Product version : " << productentry.get_version() << std::endl;

					std::cout << "Properties :" << std::endl;
					for(auto &property : productentry)
						std::cout << property.first << " : " << property.second << std::endl;
				}
			}

			std::cout << std::endl;
		}

		std::cout << "Type signature: " << ((pbo_file.is_signed()) ? "Signed" : "Unsigned") << std::endl;
		if(pbo_file.is_signed())
		{
			std::cout << "Signature: " << pbo_file.signature() << std::endl;
			std::cout << "File signature: " << pbo_file.file_signature() << std::endl;
			std::cout << "Valid signature: " << ((pbo_file.signature() == pbo_file.file_signature()) ? "true" : "false") << std::endl;
		}
	}
	catch(std::exception const &e)
	{
		std::cerr << "pboinfo: " << e.what() << std::endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

void usage()
{
	std::cout << "Usage: pboinfo [OPTION]... [FILE]" << std::endl;
	std::cout << "Options: " << std::endl;
	std::cout << "\t-us, --unsigned\t\tSet unsigned file" << std::endl;
}
