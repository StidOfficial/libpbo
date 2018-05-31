#include "signature_generator.hpp"

#include <stdexcept>

#define DEFAULT_BITS 1024

namespace pbo
{
	signature_generator::signature_generator(std::string authorityname)
	{
		BIGNUM* bne = BN_new();
		if(!BN_set_word(bne, RSA_F4))
		{
			throw std::logic_error("BN_set_word() : failed");
			BN_free(bne);
		}

		RSA* keypair = RSA_new();
		if(!RSA_generate_key_ex(keypair, DEFAULT_BITS, bne, NULL))
		{
			throw std::logic_error("RSA_generate_key_ex() : failed");
			BN_free(bne);
			RSA_free(keypair);
		}

		const BIGNUM *n, *e, *d;
		const BIGNUM *p, *q;
		const BIGNUM *dmp1, *dmq1, *iqmp;

		RSA_get0_key(keypair, &n, &e, &d);
		RSA_get0_factors(keypair, &p, &q);
		RSA_get0_crt_params(keypair, &dmp1, &dmq1, &iqmp);

		// Public key : n
		// Private key : n, p, q, dmp1, dmq1, iqmp, d

		BN_free(bne);
		RSA_free(keypair);
	}

	signature signature_generator::private_signature()
	{
		return m_private_signature;
	}

	signature signature_generator::public_signature()
	{
		return m_public_signature;
	}

	signature_generator::~signature_generator()
	{
	}
}
