#include "cryptokey.hpp"

#include <cstring>
#include <stdexcept>

namespace pbo
{
	cryptokey::cryptokey()
	{
		cryptokey(0, 0, 0, 0, 0, 0, 0, NULL, 0);
	}

	cryptokey::cryptokey(int type, unsigned int bitlen, unsigned int pubexp)
	{
		switch(type)
		{
			case CRYPTOKEY_PRIVATE:
			{
				m_blobheader = { PRIVATEKEYBLOB, CUR_BLOB_VERSION, 0, CALG_RSA_SIGN };
				m_rsapubkey = { PRIVATEKEY_MAGIC, bitlen, pubexp };
				break;
			}
			case CRYPTOKEY_PUBLIC:
			{
				m_blobheader = { PUBLICKEYBLOB, CUR_BLOB_VERSION, 0, CALG_RSA_SIGN };
				m_rsapubkey = { PUBLICKEY_MAGIC, bitlen, pubexp };
				break;
			}
			default:
				throw std::logic_error("Unknown type");
		}
	}

	cryptokey::cryptokey(char type, char version, unsigned short reserved, unsigned int alg_id, unsigned int magic, unsigned int bitlen, unsigned int pubexp, unsigned char* n, int n_length)
	{
		cryptokey(type, version, reserved, alg_id, magic, bitlen, pubexp, n, n_length, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0);
	}

	cryptokey::cryptokey(char type, char version, unsigned short reserved, unsigned int alg_id, unsigned int magic, unsigned int bitlen, unsigned int pubexp, unsigned char* n, int n_length,
																				unsigned char* p, int p_length,
																				unsigned char* q, int q_length,
																				unsigned char* dmp1, int dmp1_length,
																				unsigned char* dmq1, int dmq1_length,
																				unsigned char* iqmp, int iqmp_length,
																				unsigned char* d, int d_length)
	{
		m_type = type;
		m_blobheader = { type, version, reserved, alg_id };
		m_rsapubkey = { magic, bitlen, pubexp };
		switch(m_type)
		{
			case CRYPTOKEY_PRIVATE:
			{
				m_blobheader = { PRIVATEKEYBLOB, CUR_BLOB_VERSION, 0, CALG_RSA_SIGN };
				m_rsapubkey = { PRIVATEKEY_MAGIC, bitlen, pubexp };

				m_n.assign(n, n + n_length);
				break;
			}
			case CRYPTOKEY_PUBLIC:
			{
				m_blobheader = { PUBLICKEYBLOB, CUR_BLOB_VERSION, 0, CALG_RSA_SIGN };
				m_rsapubkey = { PUBLICKEY_MAGIC, bitlen, pubexp };

				m_n.assign(n, n + n_length);
				m_n.assign(p, p + p_length);
				m_n.assign(q, q + q_length);
				m_n.assign(dmp1, dmp1 + dmp1_length);
				m_n.assign(dmq1, dmp1 + dmq1_length);
				m_n.assign(iqmp, iqmp + iqmp_length);
				m_n.assign(d, d + d_length);
				break;
			}
			default:
				throw std::logic_error("Unknown type");
		}
	}

	unsigned char* cryptokey::key()
	{
		return m_n.data();
	}

	int cryptokey::size()
	{
		switch(m_type)
		{
			case CRYPTOKEY_PRIVATE:
				return sizeof(blobheader) + sizeof(rsapubkey) + m_n.size();
			case CRYPTOKEY_PUBLIC:
			{
				return sizeof(blobheader) + sizeof(rsapubkey) + m_n.size() + m_p.size() + m_q.size() + m_dmp1.size() + m_dmq1.size() + m_iqmp.size() + m_d.size();
			}
			default:
				return -1;
		}
	}

	char* cryptokey::data()
	{
		char* buffer = new char[size()];

		char* blobheader_buffer = reinterpret_cast<char*>(&m_blobheader);
		char* rsapubkey_buffer = reinterpret_cast<char*>(&m_rsapubkey);

		std::memcpy(buffer, blobheader_buffer, sizeof(blobheader));
		std::memcpy(buffer + sizeof(blobheader), rsapubkey_buffer, sizeof(rsapubkey));
		std::memcpy(buffer + sizeof(blobheader) + sizeof(rsapubkey), m_n.data(), m_n.size());

		return buffer;
	}
}
