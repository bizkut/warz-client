// CkSshKey.h: interface for the CkSshKey class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _CkSshKey_H
#define _CkSshKey_H



#include "CkString.h"
class CkByteData;
#include "CkMultiByteBase.h"

#ifndef __sun__
#pragma pack (push, 8)
#endif
 

// CLASS: CkSshKey
class CkSshKey  : public CkMultiByteBase
{
    private:
	// Don't allow assignment or copying these objects.
	CkSshKey(const CkSshKey &);
	CkSshKey &operator=(const CkSshKey &);


    public:
	CkSshKey(void *impl);

	CkSshKey();
	virtual ~CkSshKey();

	// May be called when finished with the object to free/dispose of any
	// internal resources held by the object. 
	void dispose(void);

	// BEGIN PUBLIC INTERFACE


	// ISRSAKEY_BEGIN
	bool get_IsRsaKey(void);
	// ISRSAKEY_END
	// ISDSAKEY_BEGIN
	bool get_IsDsaKey(void);
	// ISDSAKEY_END
	// ISPRIVATEKEY_BEGIN
	bool get_IsPrivateKey(void);
	// ISPRIVATEKEY_END
	// PASSWORD_BEGIN
	void get_Password(CkString &str);
	const char *password(void);
	void put_Password(const char *newVal);
	// PASSWORD_END
	// COMMENT_BEGIN
	void get_Comment(CkString &str);
	const char *comment(void);
	void put_Comment(const char *newVal);
	// COMMENT_END
	// GENFINGERPRINT_BEGIN
	bool GenFingerprint(CkString &outStr);
	const char *genFingerprint(void);
	// GENFINGERPRINT_END
	// TOXML_BEGIN
	bool ToXml(CkString &outStr);
	const char *toXml(void);
	// TOXML_END
	// TORFC4716PUBLICKEY_BEGIN
	bool ToRfc4716PublicKey(CkString &outStr);
	const char *toRfc4716PublicKey(void);
	// TORFC4716PUBLICKEY_END
	// TOOPENSSHPUBLICKEY_BEGIN
	bool ToOpenSshPublicKey(CkString &outStr);
	const char *toOpenSshPublicKey(void);
	// TOOPENSSHPUBLICKEY_END
	// FROMXML_BEGIN
	bool FromXml(const char *xmlKey);
	// FROMXML_END
	// FROMRFC4716PUBLICKEY_BEGIN
	bool FromRfc4716PublicKey(const char *keyStr);
	// FROMRFC4716PUBLICKEY_END
	// FROMOPENSSHPUBLICKEY_BEGIN
	bool FromOpenSshPublicKey(const char *keyStr);
	// FROMOPENSSHPUBLICKEY_END
	// FROMPUTTYPRIVATEKEY_BEGIN
	bool FromPuttyPrivateKey(const char *keyStr);
	// FROMPUTTYPRIVATEKEY_END
	// FROMOPENSSHPRIVATEKEY_BEGIN
	bool FromOpenSshPrivateKey(const char *keyStr);
	// FROMOPENSSHPRIVATEKEY_END
	// GENERATERSAKEY_BEGIN
	bool GenerateRsaKey(int numBits, int exponent);
	// GENERATERSAKEY_END
	// GENERATEDSAKEY_BEGIN
	bool GenerateDsaKey(int numBits);
	// GENERATEDSAKEY_END
	// SAVETEXT_BEGIN
	bool SaveText(const char *strToSave, const char *path);
	// SAVETEXT_END
	// LOADTEXT_BEGIN
	bool LoadText(const char *path, CkString &outStr);
	const char *loadText(const char *path);
	// LOADTEXT_END
	// TOOPENSSHPRIVATEKEY_BEGIN
	bool ToOpenSshPrivateKey(bool bEncrypt, CkString &outStr);
	const char *toOpenSshPrivateKey(bool bEncrypt);
	// TOOPENSSHPRIVATEKEY_END
	// TOPUTTYPRIVATEKEY_BEGIN
	bool ToPuttyPrivateKey(bool bEncrypt, CkString &outStr);
	const char *toPuttyPrivateKey(bool bEncrypt);
	// TOPUTTYPRIVATEKEY_END

	// SSHKEY_INSERT_POINT

	// END PUBLIC INTERFACE


};
#ifndef __sun__
#pragma pack (pop)
#endif



#endif


