#ifndef PBO_HPP
#define PBO_HPP

#include <vector>
#include <fstream>
#include <openssl/sha.h>
#include <openssl/crypto.h>

#include "entry.hpp"

namespace pbo
{
	class pbo
	{
	public:
		pbo(std::string file_path);
		~pbo();

		void add_entry(entry *entry);
		void remove_entry(int index);
		entry* &get_entry(int index);
		int size();
		std::string &pbo_signature();
		std::string &file_signature();

		void pack();
		void unpack();
	private:
		SHA_CTX signature_context;
		std::string pbo_file_path;
		std::fstream pbo_file;
		std::string pbo_checksum;
		std::string file_checksum;
		std::vector<entry*> entries;

		void set_pbo_signature(char* signature_digest);
		void set_file_signature(char* signature_digest);
	};
}

#endif
