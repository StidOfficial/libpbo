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

	for(int i = 1;i < argc; i++)
	{
		std::string arg = argv[i];
		if(arg == "-h" || arg == "--help")
		{
			usage();
			return EXIT_SUCCESS;
		}
		else if(arg.find("--", 0, 2) == 0 || arg.find("-", 0, 1) == 0)
		{
			std::cerr << "pbocheck: invalid option « " << arg << " »" << std::endl;
		}
		else
		{
			if(directory_path.empty())
				directory_path = arg;
		}
	}

	if(directory_path.empty())
	{
		usage();
		return EXIT_SUCCESS;
	}

	if(!std::filesystem::exists(directory_path))
	{
		std::cerr << "pbocheck: " << directory_path << ": " << std::strerror(ENOENT) << std::endl;
		return EXIT_FAILURE;
	}

	if(!std::filesystem::is_directory(directory_path))
	{
		std::cerr << "pbocheck: " << directory_path << ": " << std::strerror(ENOTDIR) << std::endl;
		return EXIT_FAILURE;
	}

	for(auto i = std::filesystem::recursive_directory_iterator(directory_path); i != std::filesystem::recursive_directory_iterator(); i++)
	{
		if(!std::filesystem::is_directory(i->status()) && i->path().extension() == ".pbo")
		{
			PBO::PBO pbo_file(i->path());

            std::cout << "Check " << i->path().string();
            try
            {
                pbo_file.unpack();

                std::cout << " : OK (" << pbo_file.size() << ")" << std::endl;
            }
            catch(std::exception const &e)
            {
                std::cout << " : KO (" << e.what() << ")" << std::endl;
            }
		}
	}

	return EXIT_SUCCESS;
}

void usage()
{
	std::cout << "Usage: pbocheck [DIRECTORY]" << std::endl;
	std::cout << "Options :" << std::endl;
}