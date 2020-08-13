#pragma once

#include <string>
#include <openssl/rsa.h>

#include "pboDll.h"
#include "cryptokey.hpp"

namespace PBO
{
	class PBODLL_API Signature
	{
	public:
		Signature();
		Signature(std::string path);
		Signature(std::string authorityname, CryptoKey cryptokey);
		std::string authorityname();
		CryptoKey cryptokey();
	private:
		std::string m_authorityname;
		CryptoKey m_cryptokey;
	};
}