#include "cryptokey.hpp"

#include <cstring>
#include <stdexcept>

namespace PBO
{
	CryptoKey::CryptoKey()
	{
	}

	/*cryptokey::cryptokey(char type, unsigned int bitlen, unsigned int pubexp, unsigned char* n, int n_length,
										unsigned char* p, int p_length,
										unsigned char* q, int q_length,
										unsigned char* dmp1, int dmp1_length,
										unsigned char* dmq1, int dmq1_length,
										unsigned char* iqmp, int iqmp_length,
										unsigned char* d, int d_length)
	{
		switch(type)
		{
			case PUBLICKEYBLOB:
			{
				cryptokey(PUBLICKEYBLOB, CUR_BLOB_VERSION, 0, CALG_RSA_SIGN, PUBLICKEY_MAGIC, bitlen, pubexp, n, n_length, p, p_length, q, q_length, dmp1, dmp1_length, dmq1, dmq1_length, iqmp, iqmp_length, d, d_length);
				break;
			}
			case PRIVATEKEYBLOB:
			{
				cryptokey(PRIVATEKEYBLOB, CUR_BLOB_VERSION, 0, CALG_RSA_SIGN, PRIVATEKEY_MAGIC, bitlen, pubexp, n, n_length, p, p_length, q, q_length, dmp1, dmp1_length, dmq1, dmq1_length, iqmp, iqmp_length, d, d_length);
				break;
			}
			default:
				throw std::logic_error("Unknown type");
		}
	}*/

	CryptoKey::CryptoKey(char type, char version, unsigned short reserved, unsigned int alg_id, unsigned int magic, unsigned int bitlen, unsigned int pubexp, const BIGNUM *n,
																				const BIGNUM *p,
																				const BIGNUM *q,
																				const BIGNUM *dmp1,
																				const BIGNUM *dmq1,
																				const BIGNUM *iqmp,
																				const BIGNUM *d)
	{
		m_blobheader.type = type;
		m_blobheader.version = version;
		m_blobheader.reserved = reserved;
		m_blobheader.alg_id = alg_id;

		m_rsapubkey.magic = magic;
		m_rsapubkey.bitlen = bitlen;
		m_rsapubkey.pubexp = pubexp;

		switch(m_blobheader.type)
		{
			case PRIVATEKEYBLOB:
			{
				set_n(n);
				set_p(p);
				set_q(q);
				set_dmp1(dmp1);
				set_dmq1(dmq1);
				set_iqmp(iqmp);
				set_d(d);
				break;
			}
			case PUBLICKEYBLOB:
			{
				set_n(n);
				break;
			}
			default:
				throw std::logic_error("Unknown type");
		}
	}

	void CryptoKey::set_n(const BIGNUM *n)
	{
		m_n = n;
	}

	void CryptoKey::set_p(const BIGNUM *p)
	{
		m_p = p;
	}

	void CryptoKey::set_q(const BIGNUM *q)
	{
		m_q = q;
	}

	void CryptoKey::set_dmp1(const BIGNUM *dmp1)
	{
		m_dmp1 = dmp1;
	}

	void CryptoKey::set_dmq1(const BIGNUM *dmq1)
	{
		m_dmq1 = dmq1;
	}

	void CryptoKey::set_iqmp(const BIGNUM *iqmp)
	{
		m_iqmp = iqmp;
	}

	void CryptoKey::set_d(const BIGNUM *d)
	{
		m_d = d;
	}

	int CryptoKey::size()
	{
		switch(m_blobheader.type)
		{
			case PRIVATEKEYBLOB:
				return sizeof(blobheader) + sizeof(rsapubkey) + BN_num_bytes(m_n) \
										+ BN_num_bytes(m_p) \
										+ BN_num_bytes(m_q) \
										+ BN_num_bytes(m_dmp1) \
										+ BN_num_bytes(m_dmq1) \
										+ BN_num_bytes(m_iqmp) \
										+ BN_num_bytes(m_d);
			case PUBLICKEYBLOB:
				return sizeof(blobheader) + sizeof(rsapubkey) + BN_num_bytes(m_n);
			default:
				return -1;
		}
	}

	char* CryptoKey::data()
	{
		if(size() < 0)
			return NULL;

		char* buffer = new char[size()];

		char* blobheader_buffer = reinterpret_cast<char*>(&m_blobheader);
		char* rsapubkey_buffer = reinterpret_cast<char*>(&m_rsapubkey);

		int index = 0;
		std::memcpy(buffer + index, blobheader_buffer, sizeof(blobheader));
		index += sizeof(blobheader);
		std::memcpy(buffer + index, rsapubkey_buffer, sizeof(rsapubkey));
		index += sizeof(rsapubkey);

		unsigned char key_buffer[1024];

		BN_bn2bin(m_n, key_buffer);
		std::memcpy(buffer + index, key_buffer, BN_num_bytes(m_n));
		index += BN_num_bytes(m_n);

		if(m_blobheader.type == PRIVATEKEYBLOB)
		{
			BN_bn2bin(m_n, key_buffer);
			std::memcpy(buffer + index, key_buffer, BN_num_bytes(m_p));
			index += BN_num_bytes(m_p);

			BN_bn2bin(m_q, key_buffer);
			std::memcpy(buffer + index, key_buffer, BN_num_bytes(m_q));
			index += BN_num_bytes(m_q);

			BN_bn2bin(m_dmp1, key_buffer);
			std::memcpy(buffer + index, key_buffer, BN_num_bytes(m_dmp1));
			index += BN_num_bytes(m_dmp1);

			BN_bn2bin(m_dmq1, key_buffer);
			std::memcpy(buffer + index, key_buffer, BN_num_bytes(m_dmq1));
			index += BN_num_bytes(m_dmq1);

			BN_bn2bin(m_iqmp, key_buffer);
			std::memcpy(buffer + index, key_buffer, BN_num_bytes(m_iqmp));
			index += BN_num_bytes(m_iqmp);

			BN_bn2bin(m_d, key_buffer);
			std::memcpy(buffer + index, key_buffer, BN_num_bytes(m_d));
		}

		return buffer;
	}

	CryptoKey::~CryptoKey()
	{
		
	}
}
