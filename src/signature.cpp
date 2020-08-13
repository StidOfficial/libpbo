#include "signature.hpp"

namespace PBO
{
	Signature::Signature()
	{
	}

	Signature::Signature(std::string path)
	{
		m_cryptokey = cryptokey();
	}

	Signature::Signature(std::string authorityname, CryptoKey cryptokey)
	{
		m_authorityname = authorityname;
		m_cryptokey = cryptokey;
	}

	std::string Signature::authorityname()
	{
		return m_authorityname;
	}

	CryptoKey Signature::cryptokey()
	{
		return m_cryptokey;
	}
}
