#ifndef _CkMimeW_H
#define _CkMimeW_H
#pragma once

#include "CkWideCharBase.h"
#include "CkString.h"
#include "CkMime.h"

class CkPrivateKeyW;
#if defined(WIN32) && !defined(__MINGW32__)
class CkCspW;
#endif
class CkCertW;
class CkStringArrayW;



#ifndef __sun__
#pragma pack (push, 8) 
#endif


class CkMimeW : public CkWideCharBase
{
    private:
	CkMime *m_impl;

        // Disallow assignment or copying this object.
	CkMimeW(const CkMimeW &) { }
	CkMimeW &operator=(const CkMimeW &) { return *this; }


    public:
	CkMimeW()
	    {
	    m_impl = new CkMime;
	    m_impl->put_Utf8(true);
	    }
	CkMimeW(CkMime *p, bool bForMono) : m_impl(p)
	    {
	    if (!m_impl) m_impl = new CkMime;
	    m_impl->put_Utf8(true);
	    m_forMono = bForMono;
	    }
	CkMimeW(bool bForMono)
	    {
	    m_impl = new CkMime;
	    m_impl->put_Utf8(true);
	    m_forMono = bForMono;
	    }
	CkMime *getMbPtr(void) { return m_impl; }
	CkMime &getMbRef(void) { return *m_impl; }
	const CkMime *getMbConstPtr(void) const { return m_impl; }
	const CkMime &getMbConstRef(void) const { return *m_impl; }
	void dispose(void) { m_impl->dispose();  }
	virtual ~CkMimeW() { delete m_impl; m_impl = 0; }

void get_Boundary(CkString &propVal);
void put_Boundary(const wchar_t *w1);
void get_Charset(CkString &propVal);
void put_Charset(const wchar_t *w1);
void get_ContentType(CkString &propVal);
void put_ContentType(const wchar_t *w1);
void get_CurrentDateTime(CkString &propVal);
void get_DebugLogFilePath(CkString &propVal);
void put_DebugLogFilePath(const wchar_t *w1);
void get_Disposition(CkString &propVal);
void put_Disposition(const wchar_t *w1);
void get_Encoding(CkString &propVal);
void put_Encoding(const wchar_t *w1);
void get_Filename(CkString &propVal);
void put_Filename(const wchar_t *w1);
void LastErrorHtml(CkString &propVal);
void LastErrorText(CkString &propVal);
void LastErrorXml(CkString &propVal);
void get_Micalg(CkString &propVal);
void put_Micalg(const wchar_t *w1);
void get_Name(CkString &propVal);
void put_Name(const wchar_t *w1);
long get_NumEncryptCerts(void);
long get_NumHeaderFields(void);
long get_NumParts(void);
long get_NumSignerCerts(void);
void get_Pkcs7CryptAlg(CkString &propVal);
void put_Pkcs7CryptAlg(const wchar_t *w1);
int get_Pkcs7KeyLength(void);
void put_Pkcs7KeyLength(int propVal);
void get_Protocol(CkString &propVal);
void put_Protocol(const wchar_t *w1);
void get_SigningHashAlg(CkString &propVal);
void put_SigningHashAlg(const wchar_t *w1);
bool get_UnwrapExtras(void);
void put_UnwrapExtras(bool propVal);
bool get_UseMmDescription(void);
void put_UseMmDescription(bool propVal);
bool get_UseXPkcs7(void);
void put_UseXPkcs7(bool propVal);
bool get_VerboseLogging(void);
void put_VerboseLogging(bool propVal);
void get_Version(CkString &propVal);
void AddContentLength(void);
bool AddDetachedSignature(const CkCertW &cert);
bool AddDetachedSignature2(const CkCertW &cert,bool transferHeaderFields);
bool AddDetachedSignaturePk(const CkCertW &cert,const CkPrivateKeyW &privateKey);
bool AddDetachedSignaturePk2(const CkCertW &cert,const CkPrivateKeyW &privateKey,bool transferHeaderFields);
bool AddEncryptCert(CkCertW &cert);
bool AddHeaderField(const wchar_t *w1,const wchar_t *w2);
bool AddPfxSourceData(const CkByteData &pfxData,const wchar_t *w1);
bool AddPfxSourceFile(const wchar_t *w1,const wchar_t *w2);
bool AppendPart(const CkMimeW &mime);
bool AppendPartFromFile(const wchar_t *w1);
bool AsnBodyToXml(CkString &outStr);
void ClearEncryptCerts(void);
bool ContainsEncryptedParts(void);
bool ContainsSignedParts(void);
void Convert8Bit(void);
void ConvertToMultipartAlt(void);
void ConvertToMultipartMixed(void);
bool ConvertToSigned(const CkCertW &cert);
bool ConvertToSignedPk(const CkCertW &cert,const CkPrivateKeyW &privateKey);
bool Decrypt(void);
bool Decrypt2(const CkCertW &cert,const CkPrivateKeyW &privateKey);
bool DecryptUsingPfxData(const CkByteData &pfxData,const wchar_t *w1);
bool DecryptUsingPfxFile(const wchar_t *w1,const wchar_t *w2);
bool Encrypt(const CkCertW &cert);
bool EncryptN(void);
#ifndef MOBILE_MIME
CkStringArrayW *ExtractPartsToFiles(const wchar_t *w1);
#endif
CkCertW *FindIssuer(CkCertW &cert);
bool GetBodyBinary(CkByteData &outData);
bool GetBodyDecoded(CkString &outStr);
bool GetBodyEncoded(CkString &outStr);
CkCertW *GetEncryptCert(long index);
bool GetEntireBody(CkString &outStr);
bool GetEntireHead(CkString &outStr);
bool GetHeaderField(const wchar_t *w1,CkString &outStr);
bool GetHeaderFieldAttribute(const wchar_t *w1,const wchar_t *w2,CkString &outStr);
bool GetHeaderFieldName(int index,CkString &outStr);
bool GetHeaderFieldValue(int index,CkString &outStr);
bool GetMime(CkString &outStr);
bool GetMimeBytes(CkByteData &outBytes);
CkMimeW *GetPart(long index);
bool GetSignatureSigningTime(int index,SYSTEMTIME &outSysTime);
bool GetSignatureSigningTimeStr(int index,CkString &outStr);
CkCertW *GetSignerCert(long index);
bool GetXml(CkString &outStr);
bool HasSignatureSigningTime(int index);
bool IsApplicationData(void);
bool IsAttachment(void);
bool IsAudio(void);
bool IsEncrypted(void);
bool IsHtml(void);
bool IsImage(void);
bool IsMultipart(void);
bool IsMultipartAlternative(void);
bool IsMultipartMixed(void);
bool IsMultipartRelated(void);
bool IsPlainText(void);
bool IsSigned(void);
bool IsText(void);
bool IsUnlocked(void);
bool IsVideo(void);
bool IsXml(void);
bool LoadMime(const wchar_t *w1);
bool LoadMimeBytes(const CkByteData &bData);
bool LoadMimeFile(const wchar_t *w1);
bool LoadXml(const wchar_t *w1);
bool LoadXmlFile(const wchar_t *w1);
bool NewMessageRfc822(const CkMimeW &mimeObject);
bool NewMultipartAlternative(void);
bool NewMultipartMixed(void);
bool NewMultipartRelated(void);
void RemoveHeaderField(const wchar_t *w1,bool bAllOccurances);
bool RemovePart(int index);
bool SaveBody(const wchar_t *w1);
bool SaveLastError(const wchar_t *w1);
bool SaveMime(const wchar_t *w1);
bool SaveXml(const wchar_t *w1);
void SetBody(const wchar_t *w1);
bool SetBodyFromBinary(const CkByteData &dbuf);
bool SetBodyFromEncoded(const wchar_t *w1,const wchar_t *w2);
bool SetBodyFromFile(const wchar_t *w1);
bool SetBodyFromHtml(const wchar_t *w1);
bool SetBodyFromPlainText(const wchar_t *w1);
bool SetBodyFromXml(const wchar_t *w1);
#if defined(WIN32) && !defined(WINCE) && !defined(__MINGW32__)
bool SetCSP(const CkCspW &csp);
#endif
bool SetHeaderField(const wchar_t *w1,const wchar_t *w2);
void SetVerifyCert(const CkCertW &cert);
bool UnlockComponent(const wchar_t *w1);
bool UnwrapSecurity(void);
void UrlEncodeBody(const wchar_t *w1);
bool Verify(void);
const wchar_t *asnBodyToXml(void);
const wchar_t *bodyDecoded(void);
const wchar_t *bodyEncoded(void);
const wchar_t *boundary(void);
const wchar_t *charset(void);
const wchar_t *contentType(void);
const wchar_t *currentDateTime(void);
const wchar_t *debugLogFilePath(void);
const wchar_t *disposition(void);
const wchar_t *encoding(void);
const wchar_t *entireBody(void);
const wchar_t *entireHead(void);
const wchar_t *filename(void);
const wchar_t *getBodyDecoded(void);
const wchar_t *getBodyEncoded(void);
const wchar_t *getEntireBody(void);
const wchar_t *getEntireHead(void);
const wchar_t *getHeaderField(const wchar_t *w1);
const wchar_t *getHeaderFieldAttribute(const wchar_t *w1,const wchar_t *w2);
const wchar_t *getHeaderFieldName(int index);
const wchar_t *getHeaderFieldValue(int index);
const wchar_t *getMime(void);
const wchar_t *getSignatureSigningTimeStr(int index);
const wchar_t *getXml(void);
const wchar_t *header(const wchar_t *w1);
const wchar_t *headerName(int index);
const wchar_t *headerValue(int index);
const wchar_t *lastErrorHtml(void);
const wchar_t *lastErrorText(void);
const wchar_t *lastErrorXml(void);
const wchar_t *micalg(void);
const wchar_t *mime(void);
const wchar_t *name(void);
const wchar_t *pkcs7CryptAlg(void);
const wchar_t *protocol(void);
const wchar_t *signingHashAlg(void);
const wchar_t *version(void);
const wchar_t *xml(void);

};
#ifndef __sun__
#pragma pack (pop)
#endif

#endif
