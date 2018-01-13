#include <experimental/filesystem>
#include <memory>
#include <cstring>
#include <cerrno>
#include <iostream>
#include <libpbo/pbo.hpp>
#include <cstdlib>
#include <fstream>

namespace filesystem = std::experimental::filesystem;

int exitCode = EXIT_SUCCESS;
std::string directoryPath;
std::string pbofilepath;
std::string productName;
std::string productVersion;

namespace filesystem = std::experimental::filesystem;

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
		else if(arg.find("--", 0, 2) == 0 || arg.find("-", 0, 1) == 0)
		{
			std::cerr << "pbounpack: invalid option « " << arg << " »" << std::endl;
		}
		else
		{
            if(pbofilepath.empty())
                pbofilepath = arg;
		}
	}

    if(pbofilepath.empty())
	{
		usage();
		return 0;
	}

    auto pbo = std::make_unique<PBO::PBO>(pbofilepath);

	try
	{
		pbo->unpack();
        int entrycount = pbo->getEntriesSize();
        for(int idx(0); idx<entrycount; idx++) {
            auto entry = pbo->getEntry(idx);
            auto size = entry->getDataSize();
            auto offset = entry->getDataOffset();
            if(size>0) {
                std::vector<char> tempstorage(size);
                std::ifstream input(pbofilepath, std::ios_base::binary);
                auto outfilename = entry->getPath();
                for(auto &character : outfilename) {
                    if(character == '\\') {
                        character = '/';
                    }
                }
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
		exitCode = EXIT_FAILURE;
	}

	return exitCode;
}

void usage()
{
    std::cout << "Usage: pbounpack [FILE]" << std::endl;
}
