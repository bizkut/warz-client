#ifndef _CkCertW_H
#define _CkCertW_H
#pragma once

#include "CkWideCharBase.h"
#include "CkString.h"
#include "CkCert.h"

class CkPrivateKeyW;
class CkPublicKeyW;
class CkDateTimeW;



#ifndef __sun__
#pragma pack (push, 8) 
#endif


class CkCertW : public CkWideCharBase
{
    private:
	CkCert *m_impl;

        // Disallow assignment or copying this object.
	CkCertW(const CkCertW &) { }
	CkCertW &operator=(const CkCertW &) { return *this; }


    public:
	CkCertW()
	    {
	    m_impl = new CkCert;
	    m_impl->put_Utf8(true);
	    }
	CkCertW(CkCert *p, bool bForMono) : m_impl(p)
	    {
	    if (!m_impl) m_impl = new CkCert;
	    m_impl->put_Utf8(true);
	    m_forMono = bForMono;
	    }
	CkCertW(bool bForMono)
	    {
	    m_impl = new CkCert;
	    m_impl->put_Utf8(true);
	    m_forMono = bForMono;
	    }
	CkCert *getMbPtr(void) { return m_impl; }
	CkCert &getMbRef(void) { return *m_impl; }
	const CkCert *getMbConstPtr(void) const { return m_impl; }
	const CkCert &getMbConstRef(void) const { return *m_impl; }
	void dispose(void) { m_impl->dispose();  }
	virtual ~CkCertW() { delete m_impl; m_impl = 0; }

bool get_AvoidWindowsPkAccess(void);
void put_AvoidWindowsPkAccess(bool propVal);
int get_CertVersion(void);
void get_CspName(CkString &propVal);
void get_DebugLogFilePath(CkString &propVal);
void put_DebugLogFilePath(const wchar_t *w1);
bool get_Expired(void);
bool get_ForClientAuthentication(void);
bool get_ForCodeSigning(void);
bool get_ForSecureEmail(void);
bool get_ForServerAuthentication(void);
bool get_ForTimeStamping(void);
bool get_HasKeyContainer(void);
unsigned long get_IntendedKeyUsage(void);
bool get_IsRoot(void);
void get_IssuerC(CkString &propVal);
void get_IssuerCN(CkString &propVal);
void get_IssuerDN(CkString &propVal);
void get_IssuerE(CkString &propVal);
void get_IssuerL(CkString &propVal);
void get_IssuerO(CkString &propVal);
void get_IssuerOU(CkString &propVal);
void get_IssuerS(CkString &propVal);
void get_KeyContainerName(CkString &propVal);
void LastErrorHtml(CkString &propVal);
void LastErrorText(CkString &propVal);
void LastErrorXml(CkString &propVal);
bool get_MachineKeyset(void);
void get_OcspUrl(CkString &propVal);
bool get_PrivateKeyExportable(void);
bool get_Revoked(void);
void get_Rfc822Name(CkString &propVal);
bool get_SelfSigned(void);
void get_SerialNumber(CkString &propVal);
void get_Sha1Thumbprint(CkString &propVal);
bool get_SignatureVerified(void);
bool get_Silent(void);
void get_SubjectC(CkString &propVal);
void get_SubjectCN(CkString &propVal);
void get_SubjectDN(CkString &propVal);
void get_SubjectE(CkString &propVal);
void get_SubjectL(CkString &propVal);
void get_SubjectO(CkString &propVal);
void get_SubjectOU(CkString &propVal);
void get_SubjectS(CkString &propVal);
bool get_TrustedRoot(void);
void get_ValidFrom(SYSTEMTIME &propVal);
void get_ValidFromStr(CkString &propVal);
void get_ValidTo(SYSTEMTIME &propVal);
void get_ValidToStr(CkString &propVal);
bool get_VerboseLogging(void);
void put_VerboseLogging(bool propVal);
void get_Version(CkString &propVal);
int CheckRevoked(void);
bool ExportCertDer(CkByteData &outData);
bool ExportCertDerFile(const wchar_t *w1);
bool ExportCertPem(CkString &outStr);
bool ExportCertPemFile(const wchar_t *w1);
bool ExportCertXml(CkString &outStr);
CkPrivateKeyW *ExportPrivateKey(void);
CkPublicKeyW *ExportPublicKey(void);
#if defined(WIN32) && !defined(__MINGW32__)
bool ExportToPfxFile(const wchar_t *w1,const wchar_t *w2,bool bIncludeChain);
#endif
CkCertW *FindIssuer(void);
bool GetEncoded(CkString &outStr);
bool GetPrivateKeyPem(CkString &outStr);
CkDateTimeW *GetValidFromDt(void);
CkDateTimeW *GetValidToDt(void);
bool HasPrivateKey(void);
#if defined(WIN32) && !defined(__MINGW32__)
bool LinkPrivateKey(const wchar_t *w1,bool bMachineKeyset,bool bForSigning);
#endif
bool LoadByCommonName(const wchar_t *w1);
bool LoadByEmailAddress(const wchar_t *w1);
bool LoadByIssuerAndSerialNumber(const wchar_t *w1,const wchar_t *w2);
bool LoadFromBase64(const wchar_t *w1);
bool LoadFromBinary(const CkByteData &data);
#if !defined(CHILKAT_MONO)
bool LoadFromBinary2(const unsigned char *pByteData,unsigned long szByteData);
#endif
bool LoadFromFile(const wchar_t *w1);
bool LoadPfxData(const CkByteData &pfxData,const wchar_t *w1);
#if !defined(CHILKAT_MONO)
bool LoadPfxData2(const unsigned char *pByteData,unsigned long szByteData,const wchar_t *w1);
#endif
bool LoadPfxFile(const wchar_t *w1,const wchar_t *w2);
bool PemFileToDerFile(const wchar_t *w1,const wchar_t *w2);
bool SaveLastError(const wchar_t *w1);
bool SaveToFile(const wchar_t *w1);
bool SetFromEncoded(const wchar_t *w1);
bool SetPrivateKey(CkPrivateKeyW &privKey);
bool SetPrivateKeyPem(const wchar_t *w1);
const wchar_t *cspName(void);
const wchar_t *debugLogFilePath(void);
const wchar_t *exportCertPem(void);
const wchar_t *exportCertXml(void);
const wchar_t *getEncoded(void);
const wchar_t *getPrivateKeyPem(void);
const wchar_t *issuerC(void);
const wchar_t *issuerCN(void);
const wchar_t *issuerDN(void);
const wchar_t *issuerE(void);
const wchar_t *issuerL(void);
const wchar_t *issuerO(void);
const wchar_t *issuerOU(void);
const wchar_t *issuerS(void);
const wchar_t *keyContainerName(void);
const wchar_t *lastErrorHtml(void);
const wchar_t *lastErrorText(void);
const wchar_t *lastErrorXml(void);
const wchar_t *ocspUrl(void);
const wchar_t *rfc822Name(void);
const wchar_t *serialNumber(void);
const wchar_t *sha1Thumbprint(void);
const wchar_t *subjectC(void);
const wchar_t *subjectCN(void);
const wchar_t *subjectDN(void);
const wchar_t *subjectE(void);
const wchar_t *subjectL(void);
const wchar_t *subjectO(void);
const wchar_t *subjectOU(void);
const wchar_t *subjectS(void);
const wchar_t *validFromStr(void);
const wchar_t *validToStr(void);
const wchar_t *version(void);

};
#ifndef __sun__
#pragma pack (pop)
#endif

#endif
