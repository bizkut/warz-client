// CkDsa.h: interface for the CkDsa class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _CkDsa_H
#define _CkDsa_H



#include "CkString.h"
class CkByteData;

/*
    IMPORTANT: Objects returned by methods as non-const pointers must be deleted
    by the calling application. 

  */

#include "CkMultiByteBase.h"

class CkDsaProgress;

#ifndef __sun__
#pragma pack (push, 8)
#endif
 

// CLASS: CkDsa
class CkDsa  : public CkMultiByteBase
{
    private:
	CkDsaProgress *m_callback;

	// Don't allow assignment or copying these objects.
	CkDsa(const CkDsa &) { } 
	CkDsa &operator=(const CkDsa &) { return *this; }
	CkDsa(void *impl);

    public:
	CkDsa();
	virtual ~CkDsa();


	// May be called when finished with the object to free/dispose of any
	// internal resources held by the object. 
	void dispose(void);

	// BEGIN PUBLIC INTERFACE
	
	CkDsaProgress *get_EventCallbackObject(void) const;
	void put_EventCallbackObject(CkDsaProgress *progress);

    	// GROUPSIZE_BEGIN
	int get_GroupSize(void);
	void put_GroupSize(int newVal);
	// GROUPSIZE_END
	// GENKEY_BEGIN
	bool GenKey(int numBits);
	// GENKEY_END
	// GENKEYFROMPARAMSDERFILE_BEGIN
	bool GenKeyFromParamsDerFile(const char *path);
	// GENKEYFROMPARAMSDERFILE_END
	// GENKEYFROMPARAMSPEMFILE_BEGIN
	bool GenKeyFromParamsPemFile(const char *path);
	// GENKEYFROMPARAMSPEMFILE_END
	// GENKEYFROMPARAMSDER_BEGIN
	bool GenKeyFromParamsDer(const CkByteData &derBytes);
	// GENKEYFROMPARAMSDER_END
	// GENKEYFROMPARAMSPEM_BEGIN
	bool GenKeyFromParamsPem(const char *pem);
	// GENKEYFROMPARAMSPEM_END
	// UNLOCKCOMPONENT_BEGIN
	bool UnlockComponent(const char *unlockCode);
	// UNLOCKCOMPONENT_END
	// VERIFYKEY_BEGIN
	bool VerifyKey(void);
	// VERIFYKEY_END
	// TOXML_BEGIN
	bool ToXml(bool bPublicOnly, CkString &outStr);
	const char *toXml(bool bPublicOnly);
	// TOXML_END
	// FROMXML_BEGIN
	bool FromXml(const char *xmlKey);
	// FROMXML_END
	// FROMENCRYPTEDPEM_BEGIN
	bool FromEncryptedPem(const char *password, const char *pemData);
	// FROMENCRYPTEDPEM_END
	// FROMPEM_BEGIN
	bool FromPem(const char *pemData);
	// FROMPEM_END
	// SAVETEXT_BEGIN
	bool SaveText(const char *strToSave, const char *path);
	// SAVETEXT_END
	// LOADTEXT_BEGIN
	bool LoadText(const char *path, CkString &outStr);
	const char *loadText(const char *path);
	// LOADTEXT_END
	// TOENCRYPTEDPEM_BEGIN
	bool ToEncryptedPem(const char *password, CkString &outStr);
	const char *toEncryptedPem(const char *password);
	// TOENCRYPTEDPEM_END
	// TOPEM_BEGIN
	bool ToPem(CkString &outStr);
	const char *toPem(void);
	// TOPEM_END
	// TODER_BEGIN
	bool ToDer(CkByteData &outBytes);
	// TODER_END
	// TODERFILE_BEGIN
	bool ToDerFile(const char *path);
	// TODERFILE_END
	// FROMDER_BEGIN
	bool FromDer(const CkByteData &derData);
	// FROMDER_END
	// FROMDERFILE_BEGIN
	bool FromDerFile(const char *path);
	// FROMDERFILE_END
	// FROMPUBLICPEM_BEGIN
	bool FromPublicPem(const char *pemData);
	// FROMPUBLICPEM_END
	// TOPUBLICPEM_BEGIN
	bool ToPublicPem(CkString &outStr);
	const char *toPublicPem(void);
	// TOPUBLICPEM_END

	
		// HASH_BEGIN
	void get_Hash(CkByteData &data);
	void put_Hash(const CkByteData &data);
	// HASH_END
	// SIGNATURE_BEGIN
	void get_Signature(CkByteData &data);
	void put_Signature(const CkByteData &data);
	// SIGNATURE_END
	// SETENCODEDHASH_BEGIN
	bool SetEncodedHash(const char *encoding, const char *encodedHash);
	// SETENCODEDHASH_END
	// SETENCODEDSIGNATURE_BEGIN
	bool SetEncodedSignature(const char *encoding, const char *encodedSig);
	// SETENCODEDSIGNATURE_END
	// GETENCODEDSIGNATURE_BEGIN
	bool GetEncodedSignature(const char *encoding, CkString &outStr);
	const char *getEncodedSignature(const char *encoding);
	// GETENCODEDSIGNATURE_END
	// GETENCODEDHASH_BEGIN
	bool GetEncodedHash(const char *encoding, CkString &outStr);
	const char *getEncodedHash(const char *encoding);
	// GETENCODEDHASH_END
	// SIGNHASH_BEGIN
	bool SignHash(void);
	// SIGNHASH_END
	// VERIFY_BEGIN
	bool Verify(void);
	// VERIFY_END
	// SETKEYEXPLICIT_BEGIN
	bool SetKeyExplicit(int groupSizeInBytes, const char *pHex, const char *qHex, const char *gHex, const char *xHex);
	// SETKEYEXPLICIT_END
	// TOPUBLICDERFILE_BEGIN
	bool ToPublicDerFile(const char *path);
	// TOPUBLICDERFILE_END
	// TOPUBLICDER_BEGIN
	bool ToPublicDer(CkByteData &outBytes);
	// TOPUBLICDER_END
	// FROMPUBLICDER_BEGIN
	bool FromPublicDer(const CkByteData &derData);
	// FROMPUBLICDER_END
	// FROMPUBLICDERFILE_BEGIN
	bool FromPublicDerFile(const char *path);
	// FROMPUBLICDERFILE_END
	// HEXP_BEGIN
	void get_HexP(CkString &str);
	const char *hexP(void);
	// HEXP_END
	// HEXQ_BEGIN
	void get_HexQ(CkString &str);
	const char *hexQ(void);
	// HEXQ_END
	// HEXG_BEGIN
	void get_HexG(CkString &str);
	const char *hexG(void);
	// HEXG_END
	// HEXX_BEGIN
	void get_HexX(CkString &str);
	const char *hexX(void);
	// HEXX_END
	// HEXY_BEGIN
	void get_HexY(CkString &str);
	const char *hexY(void);
	// HEXY_END
	// SETPUBKEYEXPLICIT_BEGIN
	bool SetPubKeyExplicit(int groupSizeInBytes, const char *pHex, const char *qHex, const char *gHex, const char *yHex);
	// SETPUBKEYEXPLICIT_END

	// DSA_INSERT_POINT

	// END PUBLIC INTERFACE


};
#ifndef __sun__
#pragma pack (pop)
#endif



#endif


