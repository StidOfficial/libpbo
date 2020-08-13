#include <experimental/filesystem>
#include <memory>
#include <cstring>
#include <cerrno>
#include <iostream>
#include <libpbo/pbo.hpp>
#include <cstdlib>
#include <fstream>

namespace filesystem = std::experimental::filesystem;

void usage();

int main(int argc, char **argv)
{
	std::string directory_path;
	std::string pbo_file_path;
	std::string product_name;
	std::string product_version;

	for(int i = 1; i < argc; i++)
	{
		std::string arg = argv[i];
		if(arg == "-h" || arg == "--help")
		{
			usage();
			return EXIT_SUCCESS;
		}
		else if(arg.find("--", 0, 2) == 0 || arg.find("-", 0, 1) == 0)
			std::cerr << "pbounpack: invalid option « " << arg << " »" << std::endl;
		else
			if(pbo_file_path.empty())
				pbo_file_path = arg;
	}

	if(pbo_file_path.empty())
	{
		usage();
		return EXIT_SUCCESS;
	}

	PBO::PBO pbo_file(pbo_file_path);
	try
	{
		pbo_file.unpack();

		for(auto entry : pbo_file)
		{
			auto size = entry->get_data_size();
			auto offset = entry->get_data_offset();

			if(size>0) {
				std::vector<char> tempstorage(size);
				std::ifstream input(pbo_file_path, std::ios_base::binary);

				auto outfilename = entry->get_path().string();
				for(auto &character : outfilename)
					if(character == '\\')
						character = '/';

				if(!outfilename.empty()) {
					auto fullpath = filesystem::path(outfilename);
					auto filepath = fullpath.parent_path();

					if(!filepath.empty() &&  !filesystem::exists(filepath)) {
						std::cout << "Creating " << filepath << "\n";
						filesystem::create_directories(filepath);
					}

					std::cout << "Writing file " << outfilename << "\n";

					std::ofstream out(outfilename.c_str(), std::ios_base::binary);
					input.seekg(offset);
					input.read(tempstorage.data(), size);
					out.write(tempstorage.data(), size);
				}
			}
		}
	}
	catch(std::exception const &e)
	{
		std::cerr << "pbounpack : " << e.what() << std::endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

void usage()
{
    std::cout << "Usage: pbounpack [FILE]" << std::endl;
}
