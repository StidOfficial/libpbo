#pragma once

#include <string>
#include <openssl/rsa.h>

#include "pboDll.h"
#include "cryptokey.hpp"
#include "signature.hpp"

namespace PBO
{
	class PBODLL_API SignatureGenerator
	{
	public:
		SignatureGenerator(std::string authorityname);
		~SignatureGenerator();
		Signature private_signature();
		Signature public_signature();
	private:
		RSA* m_keypair;
		const BIGNUM *m_n, *m_e, *m_d, *m_p, *m_q, *m_dmp1, *m_dmq1, *m_iqmp;
		Signature m_private_signature;
		Signature m_public_signature;
	};
}