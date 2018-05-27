#include "signature.hpp"

namespace pbo
{
	signature::signature(std::string path)
	{
		m_cryptokey = cryptokey();
	}

	signature::signature(std::string authorityname, ::pbo::cryptokey cryptokey)
	{
		m_authorityname = authorityname;
		m_cryptokey = cryptokey;
	}

	std::string signature::authorityname()
	{
		return m_authorityname;
	}

	::pbo::cryptokey signature::cryptokey()
	{
		return m_cryptokey;
	}
}
