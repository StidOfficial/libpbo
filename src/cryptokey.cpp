#include "cryptokey.hpp"

#include <cstring>
#include <stdexcept>

namespace pbo
{
	cryptokey::cryptokey()
	{
	}

	cryptokey::cryptokey(char type, unsigned int bitlen, unsigned int pubexp)
	{
		cryptokey(type, bitlen, pubexp, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0, NULL, 0);
	}

	cryptokey::cryptokey(char type, unsigned int bitlen, unsigned int pubexp, unsigned char* n, int n_length,
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
	}

	cryptokey::cryptokey(char type, char version, unsigned short reserved, unsigned int alg_id, unsigned int magic, unsigned int bitlen, unsigned int pubexp, unsigned char* n, int n_length,
																				unsigned char* p, int p_length,
																				unsigned char* q, int q_length,
																				unsigned char* dmp1, int dmp1_length,
																				unsigned char* dmq1, int dmq1_length,
																				unsigned char* iqmp, int iqmp_length,
																				unsigned char* d, int d_length)
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
			case PUBLICKEYBLOB:
			{
				if(n != NULL)
					set_n(n, n_length);
				break;
			}
			case PRIVATEKEYBLOB:
			{
				if(n != NULL)
					set_n(n, n_length);
				if(p != NULL)
					set_p(p, p_length);
				if(q != NULL)
					set_q(q, q_length);
				if(dmp1 != NULL)
					set_dmp1(dmp1, dmp1_length);
				if(dmq1 != NULL)
					set_dmq1(dmq1, dmq1_length);
				if(iqmp != NULL)
					set_iqmp(iqmp, iqmp_length);
				if(d != NULL)
					set_d(d, d_length);
				break;
			}
			default:
				throw std::logic_error("Unknown type");
		}
	}

	void cryptokey::set_n(unsigned char* n, int n_length)
	{
		m_n.assign(n, n + n_length);
	}

	void cryptokey::set_p(unsigned char* p, int p_length)
	{
		m_p.assign(p, p + p_length);
	}

	void cryptokey::set_q(unsigned char* q, int q_length)
	{
		m_q.assign(q, q + q_length);
	}

	void cryptokey::set_dmp1(unsigned char* dmp1, int dmp1_length)
	{
		m_dmp1.assign(dmp1, dmp1 + dmp1_length);
	}

	void cryptokey::set_dmq1(unsigned char* dmq1, int dmq1_length)
	{
		m_dmq1.assign(dmq1, dmq1 + dmq1_length);
	}

	void cryptokey::set_iqmp(unsigned char* iqmp, int iqmp_length)
	{
		m_iqmp.assign(iqmp, iqmp + iqmp_length);
	}

	void cryptokey::set_d(unsigned char* d, int d_length)
	{
		m_d.assign(d, d + d_length);
	}

	int cryptokey::size()
	{
		switch(m_blobheader.type)
		{
			case PUBLICKEYBLOB:
				return sizeof(blobheader) + sizeof(rsapubkey) + m_n.size();
			case PRIVATEKEYBLOB:
				return sizeof(blobheader) + sizeof(rsapubkey) + m_n.size() + m_p.size() + m_q.size() + m_dmp1.size() + m_dmq1.size() + m_iqmp.size() + m_d.size();
			default:
				return -1;
		}
	}

	char* cryptokey::data()
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
		std::memcpy(buffer + index, m_n.data(), m_n.size());
		index += m_n.size();

		if(m_blobheader.type == PRIVATEKEYBLOB)
		{
			std::memcpy(buffer + index, m_p.data(), m_p.size());
			index += m_p.size();
			std::memcpy(buffer + index, m_p.data(), m_q.size());
			index += m_q.size();
			std::memcpy(buffer + index, m_dmp1.data(), m_dmp1.size());
			index += m_dmp1.size();
			std::memcpy(buffer + index, m_dmq1.data(), m_dmq1.size());
			index += m_dmq1.size();
			std::memcpy(buffer + index, m_iqmp.data(), m_iqmp.size());
			index += m_iqmp.size();
			std::memcpy(buffer + index, m_d.data(), m_d.size());
		}

		return buffer;
	}
}
