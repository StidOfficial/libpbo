#include "signature_generator.hpp"

#include <stdexcept>

#define DEFAULT_BITS 1024

namespace PBO
{
	SignatureGenerator::SignatureGenerator(std::string authorityname)
	{
		BIGNUM* bne = BN_new();
		if(!BN_set_word(bne, RSA_F4))
		{
			BN_free(bne);
			throw std::logic_error("BN_set_word() : failed");
		}

		m_keypair = RSA_new();
		if(!RSA_generate_key_ex(m_keypair, DEFAULT_BITS, bne, NULL))
		{
			BN_free(bne);
			RSA_free(m_keypair);
			throw std::logic_error("RSA_generate_key_ex() : failed");
		}

		RSA_get0_key(m_keypair, &m_n, &m_e, &m_d);
		RSA_get0_factors(m_keypair, &m_p, &m_q);
		RSA_get0_crt_params(m_keypair, &m_dmp1, &m_dmq1, &m_iqmp);

		CryptoKey private_key(PRIVATEKEYBLOB, CUR_BLOB_VERSION, 0, CALG_RSA_SIGN, PRIVATEKEY_MAGIC, 1024, RSA_F4, m_n, m_p, m_q, m_dmp1, m_dmq1, m_iqmp, m_d);
		CryptoKey public_key(PUBLICKEYBLOB, CUR_BLOB_VERSION, 0, CALG_RSA_SIGN, PUBLICKEY_MAGIC, 1024, RSA_F4, m_n);

		m_private_signature = Signature(authorityname, private_key);
		m_public_signature = Signature(authorityname, public_key);

		BN_free(bne);
	}

	Signature SignatureGenerator::private_signature()
	{
		return m_private_signature;
	}

	Signature SignatureGenerator::public_signature()
	{
		return m_public_signature;
	}

	SignatureGenerator::~SignatureGenerator()
	{
		RSA_free(m_keypair);
	}
}
