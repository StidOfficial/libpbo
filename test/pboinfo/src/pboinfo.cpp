#include <iostream>
#include <experimental/filesystem>
#include <ctime>
#include <libpbo/pbo.hpp>
#include <libpbo/productentry.hpp>
#include <cstdlib>

int exitCode = EXIT_SUCCESS;
PBO::PBO *pbo = NULL;
std::string filePath;
void usage();

int main(int argc, char **argv)
{
	for(int i = 1;i < argc; i++)
	{
		std::string arg = argv[i];
		if(arg == "-h" || arg == "--help")
		{
			usage();
			return 0;
		}
		else if(arg.find("-", 0, 2) != std::string::npos)
		{
			std::cerr << "pboinfo: invalid option « " << arg << " »" << std::endl;
		}
		else
		{
			filePath = arg;
		}
	}

	if(filePath.empty())
	{
		usage();
		return 0;
	}

	pbo = new PBO::PBO(filePath);
	try
	{
		pbo->unpack();

		for(int i = 0; i < pbo->getEntriesSize(); i++)
		{
			PBO::Entry *entry = pbo->getEntry(i);
			std::cout << "Packing method: ";
			switch(entry->getPackingMethod())
			{
				case PACKINGMETHOD_NULL:
					std::cout << "Null method" << std::endl;
					break;
				case PACKINGMETHOD_UNCOMPRESSED:
					std::cout << "Uncompressed" << std::endl;
					break;
				case PACKINGMETHOD_PACKED:
					std::cout << "Packed" << std::endl;
					break;
				case PACKINGMETHOD_PRODUCTENTRY:
					std::cout << "Product entry" << std::endl;
					break;
				default:
					std::cout << "Unknown #" << entry->getPackingMethod() << std::endl;
			}

			if(entry->isFileEntry())
			{
				std::cout << "Entry path: " << entry->getPath() << std::endl;
				std::cout << "Original size: " << entry->getOriginalSize() << " bytes" << std::endl;
				std::cout << "Reserved: " << entry->getReserved() << " bytes" << std::endl;
				std::time_t timestamp = entry->getTimestamp();
				std::cout << "Timestamp: " << std::ctime(&timestamp);
				std::cout << "Data size: " << entry->getDataSize() << " bytes" << std::endl;
			}
			else if(entry->isProductEntry())
			{
				PBO::ProductEntry *productEntry = entry->getProductEntry();
				if(productEntry)
				{
					std::cout << "Entry name : " << productEntry->getEntryName() << std::endl;
					std::cout << "Product name : " << productEntry->getProductName() << std::endl;
					std::cout << "Product version : " << productEntry->getProductVersion() << std::endl;

					for(int i = 0; i < productEntry->getProductDataSize(); i++)
					{
						std::cout << "Product data " << i << " : " << productEntry->getProductData(i) << std::endl;
					}
				}
			}

			std::cout << std::endl;
		}

		std::cout << "Signature: " << pbo->getSignature() << std::endl;
	}
	catch(std::exception const &e)
	{
		std::cerr << "pboinfo: " << e.what() << std::endl;
		exitCode = EXIT_FAILURE;
	}

	delete pbo;
	return exitCode;
}

void usage()
{
	std::cout << "Usage: pboinfo [FILE]" << std::endl;
}
