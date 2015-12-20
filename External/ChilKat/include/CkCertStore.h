// CkCertStore.h: interface for the CkCertStore class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _CKCERTSTORE_H
#define _CKCERTSTORE_H



class CkCert;
class CkString;
#include "CkMultiByteBase.h"
#include "CkString.h"

class CkByteData;


/*
    IMPORTANT: Objects returned by methods as non-const pointers must be deleted
    by the calling application. 

  */

#ifndef __sun__
#pragma pack (push, 8)
#endif
 


// CLASS: CkCertStore
class CkCertStore  : public CkMultiByteBase
{
    private:
	// Don't allow assignment or copying these objects.
	CkCertStore(const CkCertStore &) { } 
	CkCertStore &operator=(const CkCertStore &) { return *this; }
	CkCertStore(void *impl);

    public:
	CkCertStore();
	virtual ~CkCertStore();

	// May be called when finished with the object to free/dispose of any
	// internal resources held by the object. 
	void dispose(void);

	// BEGIN PUBLIC INTERFACE


	bool LoadPfxFile(const char *path, const char *password);
	bool LoadPfxData(const CkByteData &pfxData, const char *password);
#if !defined(CHILKAT_MONO)
	bool LoadPfxData2(const unsigned char *pByteData, unsigned long szByteData, const char *password);
#endif

	CkCert *FindCertBySerial(const char *serialNumber);
	CkCert *FindCertBySubjectE(const char *emailAddress);
	CkCert *FindCertBySubjectO(const char *organization);
	CkCert *FindCertBySubjectCN(const char *commonName);
	CkCert *FindCertBySubject(const char *subject);
	CkCert *GetCertificate(long index);

	long get_NumCertificates(void);

	CkCert *FindCertByRfc822Name(const char *name);
	CkCert *FindCertBySha1Thumbprint(const char *str);
	bool get_VerboseLogging(void);
	void put_VerboseLogging(bool newVal);
	// AVOIDWINDOWSPKACCESS_BEGIN
	bool get_AvoidWindowsPkAccess(void);
	void put_AvoidWindowsPkAccess(bool newVal);
	// AVOIDWINDOWSPKACCESS_END

#if defined(WIN32) && !defined(__MINGW32__)
	CkCert *GetEmailCert(long index);
	CkCert *FindCertForEmail(const char *emailAddress);
	long get_NumEmailCerts(void);

	bool RemoveCertificate(const CkCert &cert) { return this->RemoveCertificate(&cert); }
	bool AddCertificate(const CkCert &cert) { return this->AddCertificate(&cert); }

	bool CreateFileStore(const char *path);
	bool CreateMemoryStore(void);
	bool CreateRegistryStore(const char *regRoot, const char *regPath);

	bool OpenCurrentUserStore(bool readOnly);
	bool OpenFileStore(const char *path, bool readOnly);
	bool OpenRegistryStore(const char *regRoot, const char *regPath, bool readOnly);
	bool OpenLocalSystemStore(bool readOnly);

#endif


	// CERTSTORE_INSERT_POINT

	// END PUBLIC INTERFACE


#if defined(WIN32) && !defined(__MINGW32__)
	bool RemoveCertificate(const CkCert *cert);
	bool AddCertificate(const CkCert *cert);

	bool OpenChilkatStore(bool readOnly);
	bool OpenOutlookStore(bool readOnly);

        bool GetCertStoreSpec(CkString &outCss);
#endif


	// The following methods should not be called by an application.
	// They for internal use only.
	void inject(void *impl);


};
#ifndef __sun__
#pragma pack (pop)
#endif


#endif
