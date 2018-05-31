#ifndef SIGNATURE_HPP
#define SIGNATURE_HPP

#include <string>
#include <openssl/rsa.h>

#include "pboDll.h"
#include "cryptokey.hpp"

namespace pbo
{
	class PBODLL_API signature
	{
	public:
		signature();
		signature(std::string path);
		signature(std::string authorityname, ::pbo::cryptokey cryptokey);
		std::string authorityname();
		pbo::cryptokey cryptokey();
	private:
		std::string m_authorityname;
		pbo::cryptokey m_cryptokey;
	};
}

#endif
