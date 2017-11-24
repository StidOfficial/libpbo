#include <experimental/filesystem>
#include <cstring>
#include <cerrno>
#include <iostream>
#include <libpbo/pbo.hpp>
#include <cstdlib>

namespace filesystem = std::experimental::filesystem;

int exitCode = EXIT_SUCCESS;
std::string directoryPath;
std::string filePath;
std::string productName;
std::string productVersion;

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
			if(directoryPath.empty())
				directoryPath = arg;
			else if(filePath.empty())
				filePath = arg;
		}
	}

	if(directoryPath.empty() || filePath.empty())
	{
		usage();
		return 0;
	}

	if(!filesystem::exists(directoryPath))
	{
		std::cerr << "pbopack: " << directoryPath << ": " << std::strerror(ENOENT) << std::endl;
		return -1;
	}

	if(!filesystem::is_directory(directoryPath))
	{
		std::cerr << "pbopack: " << directoryPath << ": " << std::strerror(ENOTDIR) << std::endl;
		return -1;
	}

	if(productName.empty())
	{
		filesystem::path fPath = filePath;
		productName = fPath.filename();
	}

	filesystem::path baseDir = directoryPath + "/";
	std::string baseDirPath = baseDir.string();

	PBO::PBO *pbo = new PBO::PBO(filePath);
	PBO::Entry *productEntry = new PBO::Entry();
	productEntry->setPackingMethod(PACKINGMETHOD_PRODUCTENTRY);
	PBO::ProductEntry *product = productEntry->getProductEntry();
	product->setEntryName("prefix");
	product->setProductName(productName);
	product->setProductVersion("");
	pbo->addEntry(productEntry);
	for(auto i = filesystem::recursive_directory_iterator(baseDir); i != filesystem::recursive_directory_iterator(); i++)
	{
		if(!filesystem::is_directory(i->status()))
		{
			PBO::Entry *entryFile = new PBO::Entry();
			entryFile->setPackingMethod(PACKINGMETHOD_UNCOMPRESSED);
			std::string entryFilePath = i->path().string();
			entryFile->setPath(entryFilePath.substr(baseDirPath.length()));
			entryFile->setFilePath(i->path().string());
			pbo->addEntry(entryFile);
		}
	}

	try
	{
		pbo->pack();
	}
	catch(std::exception const &e)
	{
		std::cerr << "pbopack : " << e.what() << std::endl;
		exitCode = EXIT_FAILURE;
	}

	delete pbo;
	return exitCode;
}

void usage()
{
	std::cout << "Usage: pbopack [DIRECTORY] [FILE]" << std::endl;
	std::cout << "Arguments :" << std::endl;
	std::cout << "\t--name<NAME>\t\tSet product name" << std::endl;
	std::cout << "\t--version<VERSION>\tSet product version" << std::endl;
}
