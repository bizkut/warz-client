// CkDkim.h: interface for the CkDkim class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _CkDkim_H
#define _CkDkim_H



#include "CkString.h"
class CkByteData;
class CkDkimProgress;
#include "CkMultiByteBase.h"

#ifndef __sun__
#pragma pack (push, 8)
#endif
 

// CLASS: CkDkim
class CkDkim  : public CkMultiByteBase
{
    private:
	CkDkimProgress *m_callback;

	// Don't allow assignment or copying these objects.
	CkDkim(const CkDkim &);
	CkDkim &operator=(const CkDkim &);


    public:
	CkDkim(void *impl);

	CkDkim();
	virtual ~CkDkim();

	// May be called when finished with the object to free/dispose of any
	// internal resources held by the object. 
	void dispose(void);

	// BEGIN PUBLIC INTERFACE

	
	CkDkimProgress *get_EventCallbackObject(void) const;
	void put_EventCallbackObject(CkDkimProgress *progress);

	// UNLOCKCOMPONENT_BEGIN
	bool UnlockComponent(const char *unlockCode);
	// UNLOCKCOMPONENT_END
	// DKIMCANON_BEGIN
	void get_DkimCanon(CkString &str);
	const char *dkimCanon(void);
	void put_DkimCanon(const char *newVal);
	// DKIMCANON_END
	// DOMAINKEYCANON_BEGIN
	void get_DomainKeyCanon(CkString &str);
	const char *domainKeyCanon(void);
	void put_DomainKeyCanon(const char *newVal);
	// DOMAINKEYCANON_END
	// DKIMALG_BEGIN
	void get_DkimAlg(CkString &str);
	const char *dkimAlg(void);
	void put_DkimAlg(const char *newVal);
	// DKIMALG_END
	// DOMAINKEYALG_BEGIN
	void get_DomainKeyAlg(CkString &str);
	const char *domainKeyAlg(void);
	void put_DomainKeyAlg(const char *newVal);
	// DOMAINKEYALG_END
	// DKIMHEADERS_BEGIN
	void get_DkimHeaders(CkString &str);
	const char *dkimHeaders(void);
	void put_DkimHeaders(const char *newVal);
	// DKIMHEADERS_END
	// DOMAINKEYHEADERS_BEGIN
	void get_DomainKeyHeaders(CkString &str);
	const char *domainKeyHeaders(void);
	void put_DomainKeyHeaders(const char *newVal);
	// DOMAINKEYHEADERS_END
	// DKIMDOMAIN_BEGIN
	void get_DkimDomain(CkString &str);
	const char *dkimDomain(void);
	void put_DkimDomain(const char *newVal);
	// DKIMDOMAIN_END
	// DOMAINKEYDOMAIN_BEGIN
	void get_DomainKeyDomain(CkString &str);
	const char *domainKeyDomain(void);
	void put_DomainKeyDomain(const char *newVal);
	// DOMAINKEYDOMAIN_END
	// DKIMSELECTOR_BEGIN
	void get_DkimSelector(CkString &str);
	const char *dkimSelector(void);
	void put_DkimSelector(const char *newVal);
	// DKIMSELECTOR_END
	// DOMAINKEYSELECTOR_BEGIN
	void get_DomainKeySelector(CkString &str);
	const char *domainKeySelector(void);
	void put_DomainKeySelector(const char *newVal);
	// DOMAINKEYSELECTOR_END
	// DKIMBODYLENGTHCOUNT_BEGIN
	int get_DkimBodyLengthCount(void);
	void put_DkimBodyLengthCount(int newVal);
	// DKIMBODYLENGTHCOUNT_END
	// LOADDKIMPK_BEGIN
	bool LoadDkimPk(const char *privateKey, const char *optionalPassword);
	// LOADDKIMPK_END
	// LOADDOMAINKEYPK_BEGIN
	bool LoadDomainKeyPk(const char *privateKey, const char *optionalPassword);
	// LOADDOMAINKEYPK_END
	// LOADDKIMPKFILE_BEGIN
	bool LoadDkimPkFile(const char *privateKeyFilepath, const char *optionalPassword);
	// LOADDKIMPKFILE_END
	// LOADDOMAINKEYPKFILE_BEGIN
	bool LoadDomainKeyPkFile(const char *privateKeyFilepath, const char *optionalPassword);
	// LOADDOMAINKEYPKFILE_END
	// LOADPUBLICKEY_BEGIN
	bool LoadPublicKey(const char *selector, const char *domain, const char *publicKey);
	// LOADPUBLICKEY_END
	// LOADPUBLICKEYFILE_BEGIN
	bool LoadPublicKeyFile(const char *selector, const char *domain, const char *publicKeyFilepath);
	// LOADPUBLICKEYFILE_END
	// PREFETCHPUBLICKEY_BEGIN
	// eventCallbacks
	bool PrefetchPublicKey(const char *selector, const char *domain);
	// PREFETCHPUBLICKEY_END
	// VERIFYDKIMSIGNATURE_BEGIN
	// eventCallbacks
	bool VerifyDkimSignature(int sigIdx, const CkByteData &mimeData);
	// VERIFYDKIMSIGNATURE_END
	// VERIFYDOMAINKEYSIGNATURE_BEGIN
	// eventCallbacks
	bool VerifyDomainKeySignature(int sigIdx, const CkByteData &mimeData);
	// VERIFYDOMAINKEYSIGNATURE_END
	// NUMDKIMSIGNATURES_BEGIN
	int NumDkimSignatures(const CkByteData &mimeData);
	// NUMDKIMSIGNATURES_END
	// NUMDOMAINKEYSIGNATURES_BEGIN
	int NumDomainKeySignatures(const CkByteData &mimeData);
	// NUMDOMAINKEYSIGNATURES_END
	// ADDDKIMSIGNATURE_BEGIN
	bool AddDkimSignature(const CkByteData &mimeIn, CkByteData &outBytes);
	// ADDDKIMSIGNATURE_END
	// ADDDOMAINKEYSIGNATURE_BEGIN
	bool AddDomainKeySignature(const CkByteData &mimeIn, CkByteData &outBytes);
	// ADDDOMAINKEYSIGNATURE_END
	// LOADDKIMPKBYTES_BEGIN
	bool LoadDkimPkBytes(const CkByteData &privateKeyDer, const char *optionalPassword);
	// LOADDKIMPKBYTES_END
	// LOADDOMAINKEYPKBYTES_BEGIN
	bool LoadDomainKeyPkBytes(const CkByteData &privateKeyDer, const char *optionalPassword);
	// LOADDOMAINKEYPKBYTES_END

	// DKIM_INSERT_POINT

	// END PUBLIC INTERFACE



};
#ifndef __sun__
#pragma pack (pop)
#endif



#endif


