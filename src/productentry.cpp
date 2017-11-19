#include "productentry.hpp"

#include <iostream>

namespace PBO
{
	ProductEntry::ProductEntry()
	{
	}

	void ProductEntry::setEntryName(const std::string entryName)
	{
		this->entryName = entryName;
	}

	const std::string ProductEntry::getEntryName()
	{
		return this->entryName;
	}

	void ProductEntry::setProductName(const std::string productName)
	{
		this->productName = productName;
	}

	const std::string ProductEntry::getProductName()
	{
		return this->productName;
	}

	void ProductEntry::setProductVersion(const std::string productVersion)
	{
		this->productVersion = productVersion;
	}

	const std::string ProductEntry::getProductVersion()
	{
		return this->productVersion;
	}

	int ProductEntry::getProductDataSize()
	{
		return this->productData.size();
	}

	void ProductEntry::addProductData(std::string data)
	{
		this->productData.resize(this->getProductDataSize() + 1);
		this->productData[this->getProductDataSize() - 1] = data;
	}

	void ProductEntry::removeProductData(int index)
	{
		this->productData.erase(this->productData.begin(), this->productData.begin() + index);
	}

	const std::string ProductEntry::getProductData(int index)
	{
		return this->productData[index];
	}
}
