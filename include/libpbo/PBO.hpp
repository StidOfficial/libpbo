#ifndef PBO_HPP_INCLUDED
#define PBO_HPP_INCLUDED

#include <cstring>
#include <vector>
#include <fstream>

#include "Entry.hpp"

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

			void pack();
			void unpack();
		private:
			std::string pboFilePath;
			std::fstream pboFile;
			std::vector<Entry*> entries;
	};
}

#endif
