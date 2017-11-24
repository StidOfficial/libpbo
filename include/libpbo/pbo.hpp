#ifndef PBO_HPP
#define PBO_HPP

#include <vector>
#include <fstream>
#include <openssl/sha.h>
#include <openssl/crypto.h>

#include "entry.hpp"

namespace PBO
{
	class PBO
	{
	public:
		PBO(const std::string filePath);
		~PBO();

		void addEntry(Entry *entry);
		void removeEntry(int index);
		Entry* &getEntry(int index);
		int getEntriesSize();
		std::string &getSignature();

		void pack();
		void unpack();
	private:
		SHA_CTX signatureContext;
		std::string pboFilePath;
		std::fstream pboFile;
		std::string signature;
		std::vector<Entry*> entries;
	};
}

#endif
