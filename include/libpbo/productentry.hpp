#ifndef PRODUCTENTRY_HPP
#define PRODUCTENTRY_HPP

#include <vector>
#include <string>

namespace PBO
{
	class ProductEntry
	{
	public:
		ProductEntry();
		void setEntryName(const std::string entryName);
		const std::string getEntryName();
		void setProductName(const std::string productName);
		const std::string getProductName();
		void setProductVersion(const std::string productVersion);
		const std::string getProductVersion();
		int getProductDataSize();
		void addProductData(std::string data);
		void removeProductData(int index);
		const std::string getProductData(int index);
	private:
		std::string entryName;
		std::string productName;
		std::string productVersion;
		std::vector<std::string> productData;
	};
}

#endif
