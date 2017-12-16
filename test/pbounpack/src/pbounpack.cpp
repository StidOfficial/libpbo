#include <experimental/filesystem>
#include <memory>
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
		else if(arg.find("--version=", 0) == 0)
		{
			std::cout << arg.substr(10) << std::endl;
		}
		else if(arg.find("--", 0, 2) == 0 || arg.find("-", 0, 1) == 0)
		{
			std::cerr << "pbounpack: invalid option « " << arg << " »" << std::endl;
		}
		else
		{
			if(filePath.empty())
				filePath = arg;
		}
	}

	if(filePath.empty())
	{
		usage();
		return 0;
	}

	auto pbo = std::make_unique<PBO::PBO>(filePath);

	try
	{
		pbo->unpack();
	}
	catch(std::exception const &e)
	{
		std::cerr << "pbounpack : " << e.what() << std::endl;
		exitCode = EXIT_FAILURE;
	}

	return exitCode;
}

void usage()
{
	std::cout << "Usage: pbounpack [DIRECTORY] [FILE]" << std::endl;
	std::cout << "Arguments :" << std::endl;
	std::cout << "\t--name<NAME>\t\tSet product name" << std::endl;
	std::cout << "\t--version<VERSION>\tSet product version" << std::endl;
}
