#ifndef _CkEmailW_H
#define _CkEmailW_H
#pragma once

#include "CkWideCharBase.h"
#include "CkString.h"
#include "CkEmail.h"

class CkMimeW;
class CkPrivateKeyW;
#if defined(WIN32) && !defined(__MINGW32__)
class CkCspW;
#endif
class CkCertW;
class CkStringArrayW;
class CkDateTimeW;



#ifndef __sun__
#pragma pack (push, 8) 
#endif


class CkEmailW : public CkWideCharBase
{
    private:
	CkEmail *m_impl;

        // Disallow assignment or copying this object.
	CkEmailW(const CkEmailW &) { }
	CkEmailW &operator=(const CkEmailW &) { return *this; }


    public:
	CkEmailW()
	    {
	    m_impl = new CkEmail;
	    m_impl->put_Utf8(true);
	    }
	CkEmailW(CkEmail *p, bool bForMono) : m_impl(p)
	    {
	    if (!m_impl) m_impl = new CkEmail;
	    m_impl->put_Utf8(true);
	    m_forMono = bForMono;
	    }
	CkEmailW(bool bForMono)
	    {
	    m_impl = new CkEmail;
	    m_impl->put_Utf8(true);
	    m_forMono = bForMono;
	    }
	CkEmail *getMbPtr(void) { return m_impl; }
	CkEmail &getMbRef(void) { return *m_impl; }
	const CkEmail *getMbConstPtr(void) const { return m_impl; }
	const CkEmail &getMbConstRef(void) const { return *m_impl; }
	void dispose(void) { m_impl->dispose();  }
	virtual ~CkEmailW() { delete m_impl; m_impl = 0; }

void get_Body(CkString &propVal);
void put_Body(const wchar_t *w1);
void get_BounceAddress(CkString &propVal);
void put_BounceAddress(const wchar_t *w1);
void get_Charset(CkString &propVal);
void put_Charset(const wchar_t *w1);
void get_DebugLogFilePath(CkString &propVal);
void put_DebugLogFilePath(const wchar_t *w1);
bool get_Decrypted(void);
void get_EmailDate(SYSTEMTIME &propVal);
void put_EmailDate(SYSTEMTIME & propVal);
void get_EmailDateStr(CkString &propVal);
void put_EmailDateStr(const wchar_t *w1);
void get_EncryptedBy(CkString &propVal);
void get_FileDistList(CkString &propVal);
void put_FileDistList(const wchar_t *w1);
void get_From(CkString &propVal);
void put_From(const wchar_t *w1);
void get_FromAddress(CkString &propVal);
void put_FromAddress(const wchar_t *w1);
void get_FromName(CkString &propVal);
void put_FromName(const wchar_t *w1);
void get_Header(CkString &propVal);
void get_Language(CkString &propVal);
void LastErrorHtml(CkString &propVal);
void LastErrorText(CkString &propVal);
void LastErrorXml(CkString &propVal);
void get_LocalDate(SYSTEMTIME &propVal);
void put_LocalDate(SYSTEMTIME & propVal);
void get_LocalDateStr(CkString &propVal);
void put_LocalDateStr(const wchar_t *w1);
void get_Mailer(CkString &propVal);
void put_Mailer(const wchar_t *w1);
long get_NumAlternatives(void);
long get_NumAttachedMessages(void);
long get_NumAttachments(void);
long get_NumBcc(void);
long get_NumCC(void);
int get_NumDaysOld(void);
long get_NumHeaderFields(void);
long get_NumRelatedItems(void);
long get_NumReplacePatterns(void);
int get_NumReports(void);
long get_NumTo(void);
bool get_OverwriteExisting(void);
void put_OverwriteExisting(bool propVal);
void get_Pkcs7CryptAlg(CkString &propVal);
void put_Pkcs7CryptAlg(const wchar_t *w1);
int get_Pkcs7KeyLength(void);
void put_Pkcs7KeyLength(int propVal);
void get_PreferredCharset(CkString &propVal);
void put_PreferredCharset(const wchar_t *w1);
bool get_PrependHeaders(void);
void put_PrependHeaders(bool propVal);
bool get_ReceivedEncrypted(void);
bool get_ReceivedSigned(void);
void get_ReplyTo(CkString &propVal);
void put_ReplyTo(const wchar_t *w1);
bool get_ReturnReceipt(void);
void put_ReturnReceipt(bool propVal);
bool get_SendEncrypted(void);
void put_SendEncrypted(bool propVal);
bool get_SendSigned(void);
void put_SendSigned(bool propVal);
bool get_SignaturesValid(void);
void get_SignedBy(CkString &propVal);
void get_SigningHashAlg(CkString &propVal);
void put_SigningHashAlg(const wchar_t *w1);
long get_Size(void);
void get_Subject(CkString &propVal);
void put_Subject(const wchar_t *w1);
void get_Uidl(CkString &propVal);
bool get_UnpackUseRelPaths(void);
void put_UnpackUseRelPaths(bool propVal);
bool get_VerboseLogging(void);
void put_VerboseLogging(bool propVal);
void get_Version(CkString &propVal);
void AddAttachmentHeader(int index,const wchar_t *w1,const wchar_t *w2);
bool AddBcc(const wchar_t *w1,const wchar_t *w2);
bool AddCC(const wchar_t *w1,const wchar_t *w2);
bool AddDataAttachment(const wchar_t *w1,const unsigned char *pByteData,unsigned long szByteData);
bool AddDataAttachment2(const wchar_t *w1,const CkByteData &content,const wchar_t *w2);
bool AddEncryptCert(CkCertW &cert);
bool AddFileAttachment(const wchar_t *w1,CkString &outStrContentType);
bool AddFileAttachment2(const wchar_t *w1,const wchar_t *w2);
void AddHeaderField(const wchar_t *w1,const wchar_t *w2);
void AddHeaderField2(const wchar_t *w1,const wchar_t *w2);
bool AddHtmlAlternativeBody(const wchar_t *w1);
bool AddMultipleBcc(const wchar_t *w1);
bool AddMultipleCC(const wchar_t *w1);
bool AddMultipleTo(const wchar_t *w1);
bool AddPfxSourceData(const CkByteData &pfxData,const wchar_t *w1);
bool AddPfxSourceFile(const wchar_t *w1,const wchar_t *w2);
bool AddPlainTextAlternativeBody(const wchar_t *w1);
bool AddRelatedData(const wchar_t *w1,const CkByteData &inData,CkString &outStr);
void AddRelatedData2(const CkByteData &inData,const wchar_t *w1);
#if !defined(CHILKAT_MONO)
void AddRelatedData2P(const unsigned char *pByteData,unsigned long szByteData,const wchar_t *w1);
#endif
#if !defined(CHILKAT_MONO)
bool AddRelatedDataP(const wchar_t *w1,const unsigned char *pByteData,unsigned long szByteData,CkString &outStrContentId);
#endif
bool AddRelatedFile(const wchar_t *w1,CkString &outStrContentID);
bool AddRelatedFile2(const wchar_t *w1,const wchar_t *w2);
void AddRelatedHeader(int index,const wchar_t *w1,const wchar_t *w2);
bool AddRelatedString(const wchar_t *w1,const wchar_t *w2,const wchar_t *w3,CkString &outCid);
void AddRelatedString2(const wchar_t *w1,const wchar_t *w2,const wchar_t *w3);
bool AddStringAttachment(const wchar_t *w1,const wchar_t *w2);
bool AddStringAttachment2(const wchar_t *w1,const wchar_t *w2,const wchar_t *w3);
bool AddTo(const wchar_t *w1,const wchar_t *w2);
bool AddiCalendarAlternativeBody(const wchar_t *w1,const wchar_t *w2);
bool AesDecrypt(const wchar_t *w1);
bool AesEncrypt(const wchar_t *w1);
void AppendToBody(const wchar_t *w1);
bool AspUnpack(const wchar_t *w1,const wchar_t *w2,const wchar_t *w3,bool cleanFiles);
bool AspUnpack2(const wchar_t *w1,const wchar_t *w2,const wchar_t *w3,bool cleanFiles,CkByteData &outHtml);
bool AttachMessage(const CkByteData &mimeBytes);
bool BEncodeBytes(const unsigned char *pByteData,unsigned long szByteData,const wchar_t *w1,CkString &outEncodedStr);
bool BEncodeString(const wchar_t *w1,const wchar_t *w2,CkString &outEncodedStr);
void ClearBcc(void);
void ClearCC(void);
void ClearEncryptCerts(void);
void ClearTo(void);
CkEmailW *Clone(void);
bool ComputeGlobalKey(const wchar_t *w1,bool bFold,CkString &outStr);
CkEmailW *CreateDsn(const wchar_t *w1,const wchar_t *w2,bool bHeaderOnly);
CkEmailW *CreateForward(void);
CkEmailW *CreateMdn(const wchar_t *w1,const wchar_t *w2,bool bHeaderOnly);
CkEmailW *CreateReply(void);
bool CreateTempMht(const wchar_t *w1,CkString &outPath);
void DropAttachments(void);
void DropRelatedItem(long index);
void DropRelatedItems(void);
bool DropSingleAttachment(long index);
CkCertW *FindIssuer(CkCertW &cert);
bool GenerateFilename(CkString &outStrFilename);
bool GetAltHeaderField(int index,const wchar_t *w1,CkString &outStr);
bool GetAlternativeBody(long index,CkString &outStrBody);
bool GetAlternativeBodyByContentType(const wchar_t *w1,CkString &outStr);
bool GetAlternativeContentType(long index,CkString &outStrContentType);
CkEmailW *GetAttachedMessage(long index);
bool GetAttachedMessageFilename(int index,CkString &outStr);
bool GetAttachmentContentID(long index,CkString &outStrContentID);
bool GetAttachmentContentType(long index,CkString &outStrContentType);
bool GetAttachmentData(long index,CkByteData &outData);
bool GetAttachmentFilename(long index,CkString &outStrFilename);
bool GetAttachmentHeader(long index,const wchar_t *w1,CkString &outFieldValue);
long GetAttachmentSize(long index);
bool GetAttachmentString(long index,const wchar_t *w1,CkString &outStr);
bool GetAttachmentStringCrLf(long index,const wchar_t *w1,CkString &outStrData);
bool GetBcc(long index,CkString &outStr);
bool GetBccAddr(long index,CkString &outStr);
bool GetBccName(long index,CkString &outStr);
bool GetCC(long index,CkString &outStr);
bool GetCcAddr(long index,CkString &outStr);
bool GetCcName(long index,CkString &outStr);
bool GetDeliveryStatusInfo(const wchar_t *w1,CkString &outFieldValue);
CkStringArrayW *GetDsnFinalRecipients(void);
CkDateTimeW *GetDt(void);
CkCertW *GetEncryptCert(void);
CkCertW *GetEncryptedByCert(void);
bool GetFileContent(const wchar_t *w1,CkByteData &outData);
bool GetHeaderField(const wchar_t *w1,CkString &outStrFieldData);
bool GetHeaderFieldName(long index,CkString &outStrFieldName);
bool GetHeaderFieldValue(long index,CkString &outStrFieldValue);
bool GetHtmlBody(CkString &outStrBody);
int GetImapUid(void);
void GetLinkedDomains(CkStringArrayW &sa);
bool GetMbHeaderField(const wchar_t *w1,CkByteData &outData);
bool GetMbHeaderField2(const wchar_t *w1,const wchar_t *w2,CkByteData &outData);
bool GetMbHtmlBody(const wchar_t *w1,CkByteData &outData);
bool GetMbPlainTextBody(const wchar_t *w1,CkByteData &outData);
bool GetMime(CkString &outStrMime);
bool GetMimeBinary(CkByteData &outBytes);
CkMimeW *GetMimeObject(void);
bool GetPlainTextBody(CkString &outStrBody);
bool GetRelatedContentID(long index,CkString &outStrContentID);
bool GetRelatedContentLocation(int index,CkString &outStr);
bool GetRelatedContentType(long index,CkString &outStrContentType);
bool GetRelatedData(long index,CkByteData &outBuffer);
bool GetRelatedFilename(long index,CkString &outStrFilename);
bool GetRelatedString(long index,const wchar_t *w1,CkString &outStrData);
bool GetRelatedStringCrLf(long index,const wchar_t *w1,CkString &outStr);
bool GetReplacePattern(long index,CkString &outStrPattern);
bool GetReplaceString(long index,CkString &outStr);
bool GetReplaceString2(const wchar_t *w1,CkString &outStr);
bool GetReport(int index,CkString &outStr);
CkCertW *GetSignedByCert(void);
CkCertW *GetSigningCert(void);
bool GetTo(long index,CkString &outStr);
bool GetToAddr(long index,CkString &outStr);
bool GetToName(long index,CkString &outStr);
bool GetXml(CkString &outStrXml);
bool HasHeaderMatching(const wchar_t *w1,const wchar_t *w2,bool caseInsensitive);
bool HasHtmlBody(void);
bool HasPlainTextBody(void);
bool IsMultipartReport(void);
bool LoadEml(const wchar_t *w1);
bool LoadXml(const wchar_t *w1);
bool LoadXmlString(const wchar_t *w1);
bool QEncodeBytes(const unsigned char *pByteData,unsigned long szByteData,const wchar_t *w1,CkString &outEncodedStr);
bool QEncodeString(const wchar_t *w1,const wchar_t *w2,CkString &outEncodedStr);
void RemoveAttachedMessage(long index);
void RemoveAttachedMessages(void);
void RemoveAttachmentPaths(void);
void RemoveHeaderField(const wchar_t *w1);
void RemoveHtmlAlternative(void);
void RemovePlainTextAlternative(void);
bool SaveAllAttachments(const wchar_t *w1);
bool SaveAttachedFile(long index,const wchar_t *w1);
bool SaveEml(const wchar_t *w1);
bool SaveLastError(const wchar_t *w1);
bool SaveRelatedItem(long index,const wchar_t *w1);
bool SaveXml(const wchar_t *w1);
bool SetAttachmentCharset(int index,const wchar_t *w1);
bool SetAttachmentDisposition(int index,const wchar_t *w1);
bool SetAttachmentFilename(long index,const wchar_t *w1);
#if defined(WIN32) && !defined(WINCE) && !defined(__MINGW32__)
bool SetCSP(const CkCspW &csp);
#endif
bool SetDecryptCert2(const CkCertW &cert,CkPrivateKeyW &key);
bool SetDt(CkDateTimeW &dt);
bool SetEncryptCert(const CkCertW &cert);
bool SetFromMimeBytes(const CkByteData &mimeBytes);
bool SetFromMimeBytes2(const CkByteData &mimeBytes,const wchar_t *w1);
bool SetFromMimeObject(CkMimeW &mime);
bool SetFromMimeText(const wchar_t *w1);
bool SetFromXmlText(const wchar_t *w1);
void SetHtmlBody(const wchar_t *w1);
bool SetMbHtmlBody(const wchar_t *w1,const unsigned char *pByteData,unsigned long szByteData);
bool SetMbPlainTextBody(const wchar_t *w1,const unsigned char *pByteData,unsigned long szByteData);
bool SetRelatedFilename(int index,const wchar_t *w1);
bool SetReplacePattern(const wchar_t *w1,const wchar_t *w2);
bool SetSigningCert(const CkCertW &cert);
bool SetSigningCert2(const CkCertW &cert,CkPrivateKeyW &key);
void SetTextBody(const wchar_t *w1,const wchar_t *w2);
bool UidlEquals(const CkEmailW &e);
void UnSpamify(void);
bool UnpackHtml(const wchar_t *w1,const wchar_t *w2,const wchar_t *w3);
bool UnzipAttachments(void);
bool ZipAttachments(const wchar_t *w1);
const wchar_t *addFileAttachment(const wchar_t *w1);
const wchar_t *addRelatedData(const wchar_t *w1,const CkByteData &inData);
const wchar_t *addRelatedDataP(const wchar_t *w1,const unsigned char *pByteData,unsigned long szByteData);
const wchar_t *addRelatedFile(const wchar_t *w1);
const wchar_t *addRelatedString(const wchar_t *w1,const wchar_t *w2,const wchar_t *w3);
const wchar_t *bEncodeBytes(const unsigned char *pByteData,unsigned long szByteData,const wchar_t *w1);
const wchar_t *bEncodeString(const wchar_t *w1,const wchar_t *w2);
const wchar_t *body(void);
const wchar_t *bounceAddress(void);
const wchar_t *charset(void);
const wchar_t *ck_from(void);
const wchar_t *computeGlobalKey(const wchar_t *w1,bool bFold);
const wchar_t *createTempMht(const wchar_t *w1);
const wchar_t *debugLogFilePath(void);
const wchar_t *emailDateStr(void);
const wchar_t *encryptedBy(void);
const wchar_t *fileDistList(void);
const wchar_t *fromAddress(void);
const wchar_t *fromName(void);
const wchar_t *generateFilename(void);
const wchar_t *getAltHeaderField(int index,const wchar_t *w1);
const wchar_t *getAlternativeBody(long index);
const wchar_t *getAlternativeBodyByContentType(const wchar_t *w1);
const wchar_t *getAlternativeContentType(long index);
const wchar_t *getAttachedMessageFilename(int index);
const wchar_t *getAttachmentContentID(long index);
const wchar_t *getAttachmentContentType(long index);
const wchar_t *getAttachmentFilename(long index);
const wchar_t *getAttachmentHeader(long index,const wchar_t *w1);
const wchar_t *getAttachmentString(long index,const wchar_t *w1);
const wchar_t *getAttachmentStringCrLf(long index,const wchar_t *w1);
const wchar_t *getBcc(long index);
const wchar_t *getBccAddr(long index);
const wchar_t *getBccName(long index);
const wchar_t *getCC(long index);
const wchar_t *getCcAddr(long index);
const wchar_t *getCcName(long index);
const wchar_t *getDeliveryStatusInfo(const wchar_t *w1);
const wchar_t *getHeaderField(const wchar_t *w1);
const wchar_t *getHeaderFieldName(long index);
const wchar_t *getHeaderFieldValue(long index);
const wchar_t *getHtmlBody(void);
const wchar_t *getMime(void);
const wchar_t *getPlainTextBody(void);
const wchar_t *getRelatedContentID(long index);
const wchar_t *getRelatedContentLocation(int index);
const wchar_t *getRelatedContentType(long index);
const wchar_t *getRelatedFilename(long index);
const wchar_t *getRelatedString(long index,const wchar_t *w1);
const wchar_t *getRelatedStringCrLf(long index,const wchar_t *w1);
const wchar_t *getReplacePattern(long index);
const wchar_t *getReplaceString(long index);
const wchar_t *getReplaceString2(const wchar_t *w1);
const wchar_t *getReport(int index);
const wchar_t *getTo(long index);
const wchar_t *getToAddr(long index);
const wchar_t *getToName(long index);
const wchar_t *getXml(void);
const wchar_t *header(void);
const wchar_t *language(void);
const wchar_t *lastErrorHtml(void);
const wchar_t *lastErrorText(void);
const wchar_t *lastErrorXml(void);
const wchar_t *localDateStr(void);
const wchar_t *mailer(void);
const wchar_t *pkcs7CryptAlg(void);
const wchar_t *preferredCharset(void);
const wchar_t *qEncodeBytes(const unsigned char *pByteData,unsigned long szByteData,const wchar_t *w1);
const wchar_t *qEncodeString(const wchar_t *w1,const wchar_t *w2);
const wchar_t *replyTo(void);
const wchar_t *signedBy(void);
const wchar_t *signingHashAlg(void);
const wchar_t *subject(void);
const wchar_t *uidl(void);
const wchar_t *version(void);

};
#ifndef __sun__
#pragma pack (pop)
#endif

#endif
