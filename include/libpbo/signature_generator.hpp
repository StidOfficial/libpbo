#ifndef SIGNATURE_GENERATOR_HPP
#define SIGNATURE_GENERATOR_HPP

#include <string>
#include <openssl/rsa.h>

#include "pboDll.h"
#include "cryptokey.hpp"
#include "signature.hpp"

namespace pbo
{
	class PBODLL_API signature_generator
	{
	public:
		signature_generator(std::string authorityname);
		~signature_generator();
		signature private_signature();
		signature public_signature();
	private:
		RSA* m_keypair;
		const BIGNUM *m_n, *m_e, *m_d, *m_p, *m_q, *m_dmp1, *m_dmq1, *m_iqmp;
		signature m_private_signature;
		signature m_public_signature;
	};
}

#endif
