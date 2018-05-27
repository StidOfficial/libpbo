#include <iostream>
#include <string>
#include <cstdlib>
#include <fstream>

void usage();

int main(int argc, char **argv)
{
	std::string authorityname;

	for(int i = 1;i < argc; i++)
	{
		std::string arg = argv[i];
		if(arg == "-h" || arg == "--help")
		{
			usage();
			return EXIT_SUCCESS;
		}
		else if(arg.find("-an=") == 0)
			authorityname = arg.substr(4);
		else if(arg.find("--authorityname=", 0) == 0)
			authorityname = arg.substr(16);
		else if(arg.find("-", 0, 2) != std::string::npos)
			std::cerr << "pbocreatekey: invalid option « " << arg << " »" << std::endl;
	}

	if(authorityname.empty())
	{
		usage();
		return EXIT_SUCCESS;
	}

	std::string bikey_path = authorityname + ".bikey";
	std::ofstream bikey(bikey_path.c_str(), std::ios_base::binary);

	bikey.write(authorityname.c_str(), authorityname.length() + 1);

	int bikey_size = 148;
	const char* bikey_size_buffer = reinterpret_cast<char*>(&bikey_size);
	bikey.write(bikey_size_buffer, 4); // Size of CryptExportKey = 148

	const char bikey_header[20] = { 0x06, 0x02, 0x00, 0x00, 0x00, 0x24, 0x00, 0x00, 0x52, 0x53, 0x41, 0x31, 0x00, 0x04, 0x00, 0x00, 0x01, 0x00, 0x01, 0x00 };
	bikey.write(bikey_header, 20);

	bikey.close();

	std::string biprivatekey_path = authorityname + ".biprivatekey";
        std::ofstream biprivatekey(biprivatekey_path, std::ios_base::binary);

        biprivatekey.write(authorityname.c_str(), authorityname.length() + 1);

	int biprivatekey_size = 596;
	const char* biprivatekey_size_buffer = reinterpret_cast<char*>(&biprivatekey_size);
        biprivatekey.write(biprivatekey_size_buffer, 4); // Size of CryptExportKey = 596

	const char biprivatekey_header[20] = { 0x07, 0x02, 0x00, 0x00, 0x00, 0x24, 0x00, 0x00, 0x52, 0x53, 0x41, 0x32, 0x00, 0x04, 0x00, 0x00, 0x01, 0x00, 0x01, 0x00 };
	biprivatekey.write(biprivatekey_header, 20);

	biprivatekey.close();

	return EXIT_SUCCESS;
}

void usage()
{
	std::cout << "Usage: pbocreatekey [OPTION]..." << std::endl;
	std::cout << "Options: " << std::endl;
	std::cout << "\t-an=<NAME>, --authorityname=<NAME>\t\tSet authority NAME" << std::endl;
}
