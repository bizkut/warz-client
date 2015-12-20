// CkPublicKey.h: interface for the CkPublicKey class.
//
//////////////////////////////////////////////////////////////////////
#ifndef _CKPUBLICKEY_H
#define _CKPUBLICKEY_H



class CkString;

#include "CkMultiByteBase.h"
#include "CkString.h"
#include "CkByteData.h"

#ifndef __sun__
#pragma pack (push, 8)
#endif
 

// CLASS: CkPublicKey
class CkPublicKey : public CkMultiByteBase 
{
    private:
	// Don't allow assignment or copying these objects.
	CkPublicKey &operator=(const CkPublicKey &) { return *this; }

    public:
	CkPublicKey(const CkPublicKey &pubKey);

	CkPublicKey();
	CkPublicKey(void *impl);
	virtual ~CkPublicKey();

	// May be called when finished with the object to free/dispose of any
	// internal resources held by the object. 
	void dispose(void);

	// BEGIN PUBLIC INTERFACE

	// Load from file:
	bool LoadRsaDerFile(const char *path);
	bool LoadOpenSslDerFile(const char *path);
	bool LoadOpenSslPemFile(const char *path);
	bool LoadXmlFile(const char *path);

	// Load from memory.
	bool LoadOpenSslPem(const char *str);
	bool LoadOpenSslDer(const CkByteData &data);
	bool LoadRsaDer(const CkByteData &data);
	bool LoadXml(const char *xml);

	// Save to file:
	bool SaveRsaDerFile(const char *path);
	bool SaveOpenSslDerFile(const char *path);
	bool SaveOpenSslPemFile(const char *path);
	bool SaveXmlFile(const char *path);

	// Get to memory
	bool GetRsaDer(CkByteData &outData) const;
	bool GetOpenSslDer(CkByteData &outData) const;
	bool GetOpenSslPem(CkString &outStr) const;
	bool GetXml(CkString &outStr) const;


	const char *getOpenSslPem(void);
	const char *getXml(void);
	// LOADPKCS1PEM_BEGIN
	bool LoadPkcs1Pem(const char *str);
	// LOADPKCS1PEM_END

	// PUBLICKEY_INSERT_POINT

	// END PUBLIC INTERFACE
	

};
#ifndef __sun__
#pragma pack (pop)
#endif


#endif
