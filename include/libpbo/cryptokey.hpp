#pragma once

#include <vector>
#include <openssl/bn.h>

#if _WIN32 || _WIN64
#include <Windows.h>
#else
#define KEYSTATEBLOB				0xC
#define OPAQUEKEYBLOB				0x9
#define PLAINTEXTKEYBLOB			0x8
#define PRIVATEKEYBLOB				0x7
#define PUBLICKEYBLOB				0x6
#define PUBLICKEYBLOBEX				0xA
#define SIMPLEBLOB					0x1
#define SYMMETRICWRAPKEYBLOB		0xB

#define CUR_BLOB_VERSION			0x2

#define CALG_3DES					0x00006603
#define CALG_3DES_112				0x00006609
#define CALG_AES					0x00006611
#define CALG_AES_128				0x0000660e
#define CALG_AES_192				0x0000660f
#define CALG_AES_256				0x00006610
#define CALG_AGREEDKEY_ANY			0x0000aa03
#define CALG_CYLINK_MEK				0x0000660c
#define CALG_DES					0x00006601
#define CALG_DESX					0x00006604
#define CALG_DH_EPHEM				0x0000aa02
#define CALG_DH_SF					0x0000aa01
#define CALG_DSS_SIGN				0x00002200
#define CALG_ECDH					0x0000aa05
#define CALG_ECDH_EPHEM				0x0000ae06
#define CALG_ECDSA					0x00002203
#define CALG_ECMQV					0x0000a001
#define CALG_HASH_REPLACE_OWF		0x0000800b
#define CALG_HUGHES_MD5				0x0000a003
#define CALG_HMAC					0x00008009
#define CALG_KEA_KEYX				0x0000aa04
#define CALG_MAC					0x00008005
#define CALG_MD2					0x00008001
#define CALG_MD4					0x00008002
#define CALG_MD5					0x00008003
#define CALG_NO_SIGN				0x00002000
#define CALG_OID_INFO_CNG_ONLY		0xffffffff
#define CALG_OID_INFO_PARAMETERS	0xfffffffe
#define CALG_PCT1_MASTER			0x00004c04
#define CALG_RC2					0x00006602
#define CALG_RC4					0x00006801
#define CALG_RC5					0x0000660d
#define CALG_RSA_KEYX				0x0000a400
#define CALG_RSA_SIGN				0x00002400
#define CALG_SCHANNEL_ENC_KEY		0x00004c07
#define CALG_SCHANNEL_MAC_KEY		0x00004c03
#define CALG_SCHANNEL_MASTER_HASH	0x00004c02
#define CALG_SEAL					0x00006802
#define CALG_SHA					0x00008004
#define CALG_SHA1					0x00008004
#define CALG_SHA_256				0x0000800c
#define CALG_SHA_384				0x0000800d
#define CALG_SHA_512				0x0000800e
#define CALG_SKIPJACK				0x0000660a
#define CALG_SSL2_MASTER			0x00004c05
#define CALG_SSL3_MASTER			0x00004c01
#define CALG_SSL3_SHAMD5			0x00008008
#define CALG_TEK					0x0000660b
#define CALG_TLS1_MASTER			0x00004c06
#define CALG_TLS1PRF				0x0000800a
#endif

#define PUBLICKEY_MAGIC				0x31415352
#define PRIVATEKEY_MAGIC			0x32415352

// Ref : https://msdn.microsoft.com/en-us/library/windows/desktop/aa387453(v=vs.85).aspx
// Ref : https://msdn.microsoft.com/en-us/library/windows/desktop/aa387685(v=vs.85).aspx

typedef struct _publickeystruc
{
	char type;
	char version;
	unsigned short reserved;
	unsigned int alg_id;
} blobheader, publickeystruc;

typedef struct _rsapubkey
{
	unsigned int magic;
	unsigned int bitlen;
	unsigned int pubexp;
} rsapubkey;

#include "pboDll.h"

namespace PBO
{
	class PBODLL_API CryptoKey
	{
	public:
		CryptoKey();
		CryptoKey(char type, unsigned int bitlen, unsigned int pubexp, const BIGNUM *n = NULL,
										const BIGNUM *p = NULL,
										const BIGNUM *q = NULL,
										const BIGNUM *dmp1 = NULL,
										const BIGNUM *dmq1 = NULL,
										const BIGNUM *iqmp = NULL,
										const BIGNUM *d = NULL);
		CryptoKey(char type, char version, unsigned short reserved, unsigned int alg_id, unsigned int magic, unsigned int bitlen, unsigned int pubexp, const BIGNUM *n = NULL,
																				const BIGNUM *p = NULL,
																				const BIGNUM *q = NULL,
																				const BIGNUM *dmp1 = NULL,
																				const BIGNUM *dmq1 = NULL,
																				const BIGNUM *iqmp = NULL,
																				const BIGNUM *d = NULL);
		~CryptoKey();
		void set_n(const BIGNUM *n);
		void set_p(const BIGNUM *p);
		void set_q(const BIGNUM *q);
		void set_dmp1(const BIGNUM *dmp1);
		void set_dmq1(const BIGNUM *dmq1);
		void set_iqmp(const BIGNUM *iqmp);
		void set_d(const BIGNUM *d);
		int size();
		char* data();
	private:
		blobheader m_blobheader;
		rsapubkey m_rsapubkey;
		const BIGNUM *m_n, *m_p, *m_q, *m_dmp1, *m_dmq1, *m_iqmp, *m_d;
	};
}