#include <iostream>
#include <string>
#include <cstdlib>
#include <libpbo/signature.hpp>

void usage();

int main(int argc, char **argv)
{
	std::string privatekey_path;
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
			std::cerr << "pbocreatekey: invalid option « " << arg << " »" << std::endl;
		else
			if(privatekey_path.empty())
				privatekey_path = arg;
			else if(file_path.empty())
				file_path = arg;
	}

	if(privatekey_path.empty() || file_path.empty())
	{
		usage();
		return EXIT_SUCCESS;
	}

	try
	{
		PBO::Signature private_signature(privatekey_path);
	}
	catch(const std::exception& e)
	{
		std::cout << e.what() << std::endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

void usage()
{
	std::cout << "Usage: pbosignkey [PRIVATEKEYFILE] [FILE]" << std::endl;
}
