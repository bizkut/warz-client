// CkRsa.h: interface for the CkRsa class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _CkRsa_H
#define _CkRsa_H



#include "CkString.h"
class CkByteData;
#include "CkMultiByteBase.h"

#ifndef __sun__
#pragma pack (push, 8)
#endif
 

// CLASS: CkRsa
class CkRsa  : public CkMultiByteBase
{
    private:
	// Don't allow assignment or copying these objects.
	CkRsa(const CkRsa &);
	CkRsa &operator=(const CkRsa &);


    public:
	CkRsa(void *impl);

	CkRsa();
	virtual ~CkRsa();

	// May be called when finished with the object to free/dispose of any
	// internal resources held by the object. 
	void dispose(void);

	// BEGIN PUBLIC INTERFACE

	bool UnlockComponent(const char *unlockCode);


	bool DecryptStringENC(const char *str, bool bUsePrivateKey, CkString &outStr);
	bool DecryptString(const CkByteData &bData, bool bUsePrivateKey, CkString &outStr);
	bool DecryptBytesENC(const char *str, bool bUsePrivateKey, CkByteData &outData);
	bool DecryptBytes(const CkByteData &bData, bool bUsePrivateKey, CkByteData &outData);

	bool EncryptStringENC(const char *str, bool bUsePrivateKey, CkString &outStr);
	bool EncryptBytesENC(const CkByteData &bData, bool bUsePrivateKey, CkString &outStr);
	bool EncryptString(const char *str, bool bUsePrivateKey, CkByteData &outData);
	bool EncryptBytes(const CkByteData &bData, bool bUsePrivateKey, CkByteData &outData);

	void get_EncodingMode(CkString &outStr);
	void put_EncodingMode(const char *str);
	void get_Charset(CkString &outStr);
	void put_Charset(const char *str);

	// numBits can range from 384 to 16384
	bool GenerateKey(int numBits);

	bool ExportPublicKey(CkString &outStr);
	bool ExportPrivateKey(CkString &outStr);

	bool ImportPublicKey(const char *strXml);
	bool ImportPrivateKey(const char *strXml);

	// x1 is a .snk filename.  Returns XML
	bool SnkToXml(const char *snkPath, CkString &outStr);

	// Number of bits in the RSA key (i.e. 1024, 2048, etc.)
	long get_NumBits(void);

	bool get_OaepPadding(void);
	void put_OaepPadding(bool newVal);

	// If true, use Crypto API's byte ordering (little endian) when creating signatures.
	// The default value of this property is true.
	bool get_LittleEndian(void);
	void put_LittleEndian(bool newVal);

	// New in Version 3.0
	bool VerifyStringENC(const char *str, const char *hashAlg, const char *sig);
	bool VerifyString(const char *str, const char *hashAlg, const CkByteData &sigData);
	bool VerifyBytesENC(const CkByteData &bData, const char *hashAlg, const char *encodedSig);
	bool VerifyBytes(const CkByteData &bData, const char *hashAlg, const CkByteData &sigData);

	const char *signStringENC(const char *str, const char *hashAlg);
	const char *signBytesENC(const CkByteData &bData, const char *hashAlg);

	bool SignStringENC(const char *str, const char *hashAlg, CkString &outStr);
	bool SignBytesENC(const CkByteData &bData, const char *hashAlg, CkString &outStr);
	bool SignString(const char *str, const char *hashAlg, CkByteData &outData);
	bool SignBytes(const CkByteData &bData, const char *hashAlg, CkByteData &outData);


	const char *decryptStringENC(const char *str, bool bUsePrivateKey);
	const char *decryptString(const CkByteData &bData, bool bUsePrivateKey);
	const char *encryptStringENC(const char *str, bool bUsePrivateKey);
	const char *encryptBytesENC(const CkByteData &bData, bool bUsePrivateKey);
	const char *encodingMode(void);
	const char *charset(void);
	const char *exportPublicKey(void);
	const char *exportPrivateKey(void);
	const char *snkToXml(const char *snkPath);

	// OPENSSLVERIFYBYTES_BEGIN
	bool OpenSslVerifyBytes(const CkByteData &signature, CkByteData &outBytes);
	// OPENSSLVERIFYBYTES_END
	// OPENSSLSIGNBYTES_BEGIN
	bool OpenSslSignBytes(const CkByteData &data, CkByteData &outBytes);
	// OPENSSLSIGNBYTES_END
	// OPENSSLSIGNBYTESENC_BEGIN
	bool OpenSslSignBytesENC(const CkByteData &data, CkString &outStr);
	const char *openSslSignBytesENC(const CkByteData &data);
	// OPENSSLSIGNBYTESENC_END
	// OPENSSLSIGNSTRING_BEGIN
	bool OpenSslSignString(const char *str, CkByteData &outBytes);
	// OPENSSLSIGNSTRING_END
	// OPENSSLSIGNSTRINGENC_BEGIN
	bool OpenSslSignStringENC(const char *str, CkString &outStr);
	const char *openSslSignStringENC(const char *str);
	// OPENSSLSIGNSTRINGENC_END
	// OPENSSLVERIFYBYTESENC_BEGIN
	bool OpenSslVerifyBytesENC(const char *str, CkByteData &outBytes);
	// OPENSSLVERIFYBYTESENC_END
	// OPENSSLVERIFYSTRING_BEGIN
	bool OpenSslVerifyString(const CkByteData &data, CkString &outStr);
	const char *openSslVerifyString(const CkByteData &data);
	// OPENSSLVERIFYSTRING_END
	// OPENSSLVERIFYSTRINGENC_BEGIN
	bool OpenSslVerifyStringENC(const char *str, CkString &outStr);
	const char *openSslVerifyStringENC(const char *str);
	// OPENSSLVERIFYSTRINGENC_END
	// VERIFYPRIVATEKEY_BEGIN
	bool VerifyPrivateKey(const char *xml);
	// VERIFYPRIVATEKEY_END
	// VERIFYHASH_BEGIN
	bool VerifyHash(const CkByteData &hashBytes, const char *hashAlg, const CkByteData &sigBytes);
	// VERIFYHASH_END
	// VERIFYHASHENC_BEGIN
	bool VerifyHashENC(const char *encodedHash, const char *hashAlg, const char *encodedSig);
	// VERIFYHASHENC_END
	// SIGNHASH_BEGIN
	bool SignHash(const CkByteData &hashBytes, const char *hashAlg, CkByteData &outBytes);
	// SIGNHASH_END
	// SIGNHASHENC_BEGIN
	bool SignHashENC(const char *encodedHash, const char *hashAlg, CkString &outStr);
	const char *signHashENC(const char *encodedHash, const char *hashAlg);
	// SIGNHASHENC_END
	// VERBOSELOGGING_BEGIN
	bool get_VerboseLogging(void);
	void put_VerboseLogging(bool newVal);
	// VERBOSELOGGING_END
	// NOUNPAD_BEGIN
	bool get_NoUnpad(void);
	void put_NoUnpad(bool newVal);
	// NOUNPAD_END

	// RSA_INSERT_POINT

	// END PUBLIC INTERFACE


};
#ifndef __sun__
#pragma pack (pop)
#endif



#endif


