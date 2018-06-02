#include <iostream>
#include <string>
#include <cstdlib>
#include <fstream>
#include <cstring>
#include <libpbo/cryptokey.hpp>
#include <libpbo/signature.hpp>

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

	// RSA
	unsigned int bits = 1024;
	unsigned int exp = RSA_F4;

	BIGNUM* bne = BN_new();
	if(!BN_set_word(bne, exp))
	{
		std::cerr << "BN_set_word() : failed" << std::endl;
		BN_free(bne);

		return EXIT_FAILURE;
	}

	RSA* keypair = RSA_new();
	if(!RSA_generate_key_ex(keypair, bits, bne, NULL))
	{
		std::cerr << "RSA_generate_key_ex() : failed" << std::endl;
		BN_free(bne);
		RSA_free(keypair);

		return EXIT_FAILURE;
	}

	const BIGNUM *n, *e, *d;
	const BIGNUM *p, *q, *dmp1, *dmq1, *iqmp;
	RSA_get0_key(keypair, &n, &e, &d);
	RSA_get0_factors(keypair, &p, &q);
	RSA_get0_crt_params(keypair, &dmp1, &dmq1, &iqmp);

	pbo::cryptokey publickey(PUBLICKEYBLOB, CUR_BLOB_VERSION, 0, CALG_RSA_SIGN, PUBLICKEY_MAGIC, bits, exp, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0);
	pbo::cryptokey privatekey(PRIVATEKEYBLOB, CUR_BLOB_VERSION, 0, CALG_RSA_SIGN, PRIVATEKEY_MAGIC, bits, exp, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0);

	unsigned char key_buffer[1024];
	int key_size;

	key_size = BN_bn2bin(n, key_buffer);
	publickey.set_n(key_buffer, key_size);
	privatekey.set_n(key_buffer, key_size);

	key_size = BN_bn2bin(p, key_buffer);
	privatekey.set_p(key_buffer, key_size);

	key_size = BN_bn2bin(q, key_buffer);
	privatekey.set_q(key_buffer, key_size);

	key_size = BN_bn2bin(dmp1, key_buffer);
	privatekey.set_dmp1(key_buffer, key_size);

	key_size = BN_bn2bin(dmq1, key_buffer);
	privatekey.set_dmq1(key_buffer, key_size);

	key_size = BN_bn2bin(iqmp, key_buffer);
	privatekey.set_iqmp(key_buffer, key_size);

	key_size = BN_bn2bin(d, key_buffer);
	privatekey.set_d(key_buffer, key_size);

	BN_free(bne);
	RSA_free(keypair);

	std::string bikey_path = authorityname + ".bikey";
	std::ofstream bikey(bikey_path.c_str(), std::ios_base::binary);

	bikey.write(authorityname.c_str(), authorityname.length() + 1);
	unsigned int test = publickey.size();
	bikey.write(reinterpret_cast<char*>(&test), sizeof(unsigned int));
	bikey.write(publickey.data(), publickey.size());

	bikey.close();

	std::string biprivatekey_path = authorityname + ".biprivatekey";
	std::ofstream biprivatekey(biprivatekey_path, std::ios_base::binary);

	biprivatekey.write(authorityname.c_str(), authorityname.length() + 1);
	test = privatekey.size();
	biprivatekey.write(reinterpret_cast<char*>(&test), sizeof(unsigned int));
	biprivatekey.write(privatekey.data(), privatekey.size());
	std::cout << privatekey.size() << std::endl;

	biprivatekey.close();

	return EXIT_SUCCESS;
}

void usage()
{
	std::cout << "Usage: pbocreatekey [OPTION]..." << std::endl;
	std::cout << "Options: " << std::endl;
	std::cout << "\t-an=<NAME>, --authorityname=<NAME>\t\tSet authority NAME" << std::endl;
}
