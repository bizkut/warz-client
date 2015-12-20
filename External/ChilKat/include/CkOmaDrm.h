// CkOmaDrm.h: interface for the CkOmaDrm class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _CKOMADRM_H
#define _CKOMADRM_H



#include "CkString.h"
class CkByteData;
#include "CkMultiByteBase.h"

#ifndef __sun__
#pragma pack (push, 8)
#endif
 

// CLASS: CkOmaDrm
class CkOmaDrm  : public CkMultiByteBase
{
    private:
	// Don't allow assignment or copying these objects.
	CkOmaDrm(const CkOmaDrm &);
	CkOmaDrm &operator=(const CkOmaDrm &);
	CkOmaDrm(void *impl);

    public:
	CkOmaDrm();
	virtual ~CkOmaDrm();

	// May be called when finished with the object to free/dispose of any
	// internal resources held by the object. 
	void dispose(void);

	// BEGIN PUBLIC INTERFACE

	bool UnlockComponent(const char *unlockCode);


	bool LoadDcfFile(const char *path);
	bool LoadDcfData(const CkByteData &data);
	
	void get_Base64Key(CkString &str);
	void put_Base64Key(const char *key);
	
	int get_DrmContentVersion(void);
	
	void get_ContentType(CkString &str);
	void put_ContentType(const char *contentType);
	
	void get_ContentUri(CkString &str);
	void put_ContentUri(const char *contentUri);
	
	void get_Headers(CkString &str);
	void put_Headers(const char *headers);
	
	void get_IV(CkByteData &data);
	void put_IV(const CkByteData &data);
	
	void get_EncryptedData(CkByteData &data);
	void get_DecryptedData(CkByteData &data);
	
	bool SaveDecrypted(const char *path);
	
	bool GetHeaderField(const char *fieldName, CkString &outVal);
	
	void LoadUnencryptedData(const CkByteData &data);
	bool LoadUnencryptedFile(const char *path);
	
	void SetEncodedIV(const char *encodedIv, const char *encoding);
	
	bool CreateDcfFile(const char *path);

	const char *base64Key(void);
	const char *contentType(void);
	const char *contentUri(void);
	const char *headers(void);
	const char *getHeaderField(const char *fieldName);

	// CK_INSERT_POINT

	// END PUBLIC INTERFACE


};
#ifndef __sun__
#pragma pack (pop)
#endif



#endif


