#pragma once

#include <vector>
#include <memory>
#include <fstream>
#include <openssl/sha.h>
#include <openssl/evp.h>
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

#include "pboDll.h"
#include "entry.hpp"

namespace PBO
{
	class PBODLL_API PBO : public std::vector<std::shared_ptr<Entry>>
	{
	public:
		PBO(std::filesystem::path file_path, bool signed_file = true);
		PBO(bool signed_file = true);
		~PBO();

		std::filesystem::path get_path();
		void signed_file(bool signed_file);
		bool is_signed();
		std::string &signature();
		std::string &file_signature();
		void open(std::filesystem::path file_path);

		void pack();
		void unpack();
	private:
		EVP_MD_CTX *m_evp_md_context;
		std::filesystem::path m_path;
		bool m_signed;
		std::fstream m_file;
		std::string m_checksum;
		std::string m_file_checksum;

		void read(Entry* &entry);
		void read(uint32_t &value);
		void read(std::string &text);
		void read(char *s, std::streamsize n);
		void write(Entry *entry);
		void write(uint32_t value);
		void write(std::string text);
		void write(const char *s, std::streamsize n);
		void set_signature(char *digest);
		void set_file_signature(char *digest);
	};
}