#include <iostream>
#include <string>
#include <cstdlib>
#include <fstream>
#include <cstring>
#include <ctime>
#include <locale>
#if __has_include(<filesystem>)
#include <filesystem>
#elif __has_include(<experimental/filesystem>)
#include <experimental/filesystem>
namespace std {
	namespace filesystem = std::experimental::filesystem;
}
#else
#error "No filesystem header found !"
#endif
#include <unistd.h>
#include <openssl/md5.h>
#include <libpbo/cryptokey.hpp>
#include <libpbo/signature_generator.hpp>

void usage();
std::string get_owner();

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

	try
	{
		PBO::SignatureGenerator sign_gen(authorityname);

		MD5_CTX md5_biprivatekey, md5_bikey;
		MD5_Init(&md5_biprivatekey);
		MD5_Init(&md5_bikey);

		std::filesystem::path biprivatekey_path = authorityname + ".biprivatekey";
		std::ofstream biprivatekey(std::filesystem::absolute(biprivatekey_path).c_str(), std::ios_base::binary);

		biprivatekey.write(authorityname.c_str(), authorityname.length() + 1);
		int privatecryptokey_length = sign_gen.private_signature().cryptokey().size();
		char* privatecryptokey = sign_gen.private_signature().cryptokey().data();
		biprivatekey.write(reinterpret_cast<char*>(&privatecryptokey_length), sizeof(unsigned int));
		MD5_Update(&md5_bikey, reinterpret_cast<char*>(&privatecryptokey_length), sizeof(unsigned int));
		biprivatekey.write(privatecryptokey, privatecryptokey_length);
		MD5_Update(&md5_bikey, privatecryptokey, privatecryptokey_length);
		delete[] privatecryptokey;

		biprivatekey.close();

		std::filesystem::path bikey_path = authorityname + ".bikey";
		std::ofstream bikey(std::filesystem::absolute(bikey_path).c_str(), std::ios_base::binary);

		bikey.write(authorityname.c_str(), authorityname.length() + 1);
		unsigned int publiccryptokey_length = sign_gen.public_signature().cryptokey().size();
		char* publiccyptokey = sign_gen.public_signature().cryptokey().data();
		bikey.write(reinterpret_cast<char*>(&publiccryptokey_length), sizeof(unsigned int));
		MD5_Update(&md5_bikey, reinterpret_cast<char*>(&publiccryptokey_length), sizeof(unsigned int));
		bikey.write(publiccyptokey, publiccryptokey_length);
		MD5_Update(&md5_bikey, publiccyptokey, publiccryptokey_length);
		delete[] publiccyptokey;

		bikey.close();

		std::locale::global(std::locale("en_US.utf8"));
		std::time_t t = std::time(NULL);

		char mbstr[18];
		std::strftime(mbstr, sizeof(mbstr), "%m/%d/%y %H:%M:%S", std::localtime(&t));

		std::string owner = get_owner();

		std::cout << "Authority:\t" << authorityname << std::endl;
		std::cout << "Creation:\t" << mbstr << " (by " << owner << ")" << std::endl;

		std::cout << std::endl;

		unsigned char digest[MD5_DIGEST_LENGTH];
		char mdString[MD5_DIGEST_LENGTH * 2];

		std::cout << "== PRIVATE KEY ==" << std::endl;
		std::cout << "File: " << std::filesystem::absolute(biprivatekey_path).c_str() << std::endl;
		MD5_Final(digest, &md5_bikey);
		for (int i = 0; i < MD5_DIGEST_LENGTH; i++)
			sprintf(&mdString[i * 2], "%02x", digest[i]);
		std::cout << "MD5: " << mdString << std::endl;

		std::cout << "== PUBLIC KEY ==" << std::endl;
		std::cout << "File: " << std::filesystem::absolute(bikey_path).c_str() << std::endl;
		MD5_Final(digest, &md5_biprivatekey);
		for (int i = 0; i < MD5_DIGEST_LENGTH; i++)
			sprintf(&mdString[i * 2], "%02x", digest[i]);
		std::cout << "MD5: " << mdString << std::endl;
	}
	catch(const std::exception& e)
	{
		std::cout << e.what() << std::endl;
	}

	return EXIT_SUCCESS;
}

void usage()
{
	std::cout << "Usage: pbocreatekey [OPTION]..." << std::endl;
	std::cout << "Options: " << std::endl;
	std::cout << "\t-an=<NAME>, --authorityname=<NAME>\t\tSet authority NAME" << std::endl;
}

std::string get_owner()
{
	char hostname[HOST_NAME_MAX];
	char loginname[LOGIN_NAME_MAX];

	gethostname(hostname, HOST_NAME_MAX);
	getlogin_r(loginname, LOGIN_NAME_MAX);

	std::string owner = std::string(loginname) + "@" + std::string(hostname);

	return owner;
}
