#ifndef PBO_HPP
#define PBO_HPP

#include <vector>
#include <fstream>
#include <openssl/sha.h>
#include <openssl/crypto.h>

#include "pboDll.h"
#include "entry.hpp"

namespace pbo
{
	class PBODLL_API pbo
	{
	public:
		pbo(std::string file_path, bool signed_file = true);
		~pbo();

		void signed_file(bool signed_file);
		bool is_signed();
		void add_entry(entry *entry);
		void remove_entry(int index);
		entry* &get_entry(int index);
		size_t size();
		std::string& signature();
		std::string& file_signature();

		void pack();
		void unpack();
	private:
		SHA_CTX m_sha_context;
		std::string m_path;
		bool m_signed;
		std::fstream m_file;
		std::string m_checksum;
		std::string m_file_checksum;
		std::vector<entry*> m_entries;

		void read(char* s, std::streamsize n);
		void write(const char* s, std::streamsize n);
		void set_signature(char* digest);
		void set_file_signature(char* digest);
	};
}

#endif
