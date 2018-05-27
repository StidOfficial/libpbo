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

	cryptokey::cryptokey(char type, char version, unsigned short reserved, unsigned int alg_id, unsigned int magic, unsigned int bitlen, unsigned int pubexp, unsigned char* key, int key_length)
	{
		m_blobheader = { type, version, reserved, alg_id };
		m_rsapubkey = { magic, bitlen, pubexp };
		m_key.assign(key, key + key_length);
	}

	unsigned char* cryptokey::key()
	{
		return m_key.data();
	}

	int cryptokey::size()
	{
		return sizeof(blobheader) + sizeof(rsapubkey) + m_key.size();
	}

	char* cryptokey::data()
	{
		char* buffer = new char[size()];

		char* blobheader_buffer = reinterpret_cast<char*>(&m_blobheader);
		char* rsapubkey_buffer = reinterpret_cast<char*>(&m_rsapubkey);

		std::memcpy(buffer, blobheader_buffer, sizeof(blobheader));
		std::memcpy(buffer + sizeof(blobheader), rsapubkey_buffer, sizeof(rsapubkey));
		std::memcpy(buffer + sizeof(blobheader) + sizeof(rsapubkey), m_key.data(), m_key.size());

		return buffer;
	}
}
