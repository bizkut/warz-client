#ifndef _CkCrypt2W_H
#define _CkCrypt2W_H
#pragma once

#include "CkWideCharBase.h"
#include "CkString.h"
#include "CkCrypt2.h"

class CkCrypt2Progress;
class CkPrivateKeyW;
#if defined(WIN32) && !defined(__MINGW32__)
class CkCspW;
#endif
class CkCertW;



#ifndef __sun__
#pragma pack (push, 8) 
#endif


class CkCrypt2W : public CkWideCharBase
{
    private:
	CkCrypt2 *m_impl;

        // Disallow assignment or copying this object.
	CkCrypt2W(const CkCrypt2W &) { }
	CkCrypt2W &operator=(const CkCrypt2W &) { return *this; }


    public:
	CkCrypt2W()
	    {
	    m_impl = new CkCrypt2;
	    m_impl->put_Utf8(true);
	    }
	CkCrypt2W(CkCrypt2 *p, bool bForMono) : m_impl(p)
	    {
	    if (!m_impl) m_impl = new CkCrypt2;
	    m_impl->put_Utf8(true);
	    m_forMono = bForMono;
	    }
	CkCrypt2W(bool bForMono)
	    {
	    m_impl = new CkCrypt2;
	    m_impl->put_Utf8(true);
	    m_forMono = bForMono;
	    }
	CkCrypt2 *getMbPtr(void) { return m_impl; }
	CkCrypt2 &getMbRef(void) { return *m_impl; }
	const CkCrypt2 *getMbConstPtr(void) const { return m_impl; }
	const CkCrypt2 &getMbConstRef(void) const { return *m_impl; }
	void dispose(void) { m_impl->dispose();  }
	virtual ~CkCrypt2W() { delete m_impl; m_impl = 0; }

int get_BlockSize(void);
bool get_CadesEnabled(void);
void put_CadesEnabled(bool propVal);
void get_CadesSigPolicyHash(CkString &propVal);
void put_CadesSigPolicyHash(const wchar_t *w1);
void get_CadesSigPolicyId(CkString &propVal);
void put_CadesSigPolicyId(const wchar_t *w1);
void get_CadesSigPolicyUri(CkString &propVal);
void put_CadesSigPolicyUri(const wchar_t *w1);
void get_Charset(CkString &propVal);
void put_Charset(const wchar_t *w1);
void get_CipherMode(CkString &propVal);
void put_CipherMode(const wchar_t *w1);
void get_CompressionAlgorithm(CkString &propVal);
void put_CompressionAlgorithm(const wchar_t *w1);
void get_CryptAlgorithm(CkString &propVal);
void put_CryptAlgorithm(const wchar_t *w1);
void get_DebugLogFilePath(CkString &propVal);
void put_DebugLogFilePath(const wchar_t *w1);
void get_EncodingMode(CkString &propVal);
void put_EncodingMode(const wchar_t *w1);
CkCrypt2Progress *get_EventCallbackObject(void);
void put_EventCallbackObject(CkCrypt2Progress * propVal);
bool get_FirstChunk(void);
void put_FirstChunk(bool propVal);
void get_HashAlgorithm(CkString &propVal);
void put_HashAlgorithm(const wchar_t *w1);
long get_HavalRounds(void);
void put_HavalRounds(long propVal);
void get_IV(CkByteData &propVal);
void put_IV(CkByteData & propVal);
bool get_IncludeCertChain(void);
void put_IncludeCertChain(bool propVal);
int get_IterationCount(void);
void put_IterationCount(int propVal);
long get_KeyLength(void);
void put_KeyLength(long propVal);
bool get_LastChunk(void);
void put_LastChunk(bool propVal);
void LastErrorHtml(CkString &propVal);
void LastErrorText(CkString &propVal);
void LastErrorXml(CkString &propVal);
int get_NumSignerCerts(void);
long get_PaddingScheme(void);
void put_PaddingScheme(long propVal);
void get_PbesAlgorithm(CkString &propVal);
void put_PbesAlgorithm(const wchar_t *w1);
void get_PbesPassword(CkString &propVal);
void put_PbesPassword(const wchar_t *w1);
void get_Pkcs7CryptAlg(CkString &propVal);
void put_Pkcs7CryptAlg(const wchar_t *w1);
long get_Rc2EffectiveKeyLength(void);
void put_Rc2EffectiveKeyLength(long propVal);
void get_Salt(CkByteData &propVal);
void put_Salt(CkByteData & propVal);
void get_SecretKey(CkByteData &propVal);
void put_SecretKey(CkByteData & propVal);
void get_UuFilename(CkString &propVal);
void put_UuFilename(const wchar_t *w1);
void get_UuMode(CkString &propVal);
void put_UuMode(const wchar_t *w1);
bool get_VerboseLogging(void);
void put_VerboseLogging(bool propVal);
void get_Version(CkString &propVal);
void AddEncryptCert(CkCertW &cert);
bool AddPfxSourceData(const CkByteData &pfxData,const wchar_t *w1);
bool AddPfxSourceFile(const wchar_t *w1,const wchar_t *w2);
bool ByteSwap4321(const CkByteData &data,CkByteData &outBytes);
bool BytesToString(const CkByteData &inData,const wchar_t *w1,CkString &outStr);
bool CkDecryptFile(const wchar_t *w1,const wchar_t *w2);
bool CkEncryptFile(const wchar_t *w1,const wchar_t *w2);
void ClearEncryptCerts(void);
bool CompressBytes(const CkByteData &bData,CkByteData &outData);
bool CompressBytesENC(const CkByteData &bData,CkString &outStr);
bool CompressString(const wchar_t *w1,CkByteData &outData);
bool CompressStringENC(const wchar_t *w1,CkString &outStr);
bool CreateDetachedSignature(const wchar_t *w1,const wchar_t *w2);
bool CreateP7M(const wchar_t *w1,const wchar_t *w2);
bool CreateP7S(const wchar_t *w1,const wchar_t *w2);
bool Decode(const wchar_t *w1,const wchar_t *w2,CkByteData &outData);
bool DecodeString(const wchar_t *w1,const wchar_t *w2,const wchar_t *w3,CkString &outStr);
bool DecryptBytes(const CkByteData &bData,CkByteData &outData);
bool DecryptBytesENC(const wchar_t *w1,CkByteData &outData);
bool DecryptEncoded(const wchar_t *w1,CkString &outStr);
bool DecryptString(const CkByteData &bData,CkString &outStr);
bool DecryptStringENC(const wchar_t *w1,CkString &outStr);
bool Encode(const CkByteData &bData,const wchar_t *w1,CkString &outStr);
bool EncodeBytes(const void *pByteData,unsigned long szByteData,const wchar_t *w1,CkString &outStr);
bool EncodeString(const wchar_t *w1,const wchar_t *w2,const wchar_t *w3,CkString &outStr);
bool EncryptBytes(const CkByteData &bData,CkByteData &outData);
bool EncryptBytesENC(const CkByteData &bData,CkString &outStr);
bool EncryptEncoded(const wchar_t *w1,CkString &outStr);
bool EncryptString(const wchar_t *w1,CkByteData &outData);
bool EncryptStringENC(const wchar_t *w1,CkString &outStr);
bool GenEncodedSecretKey(const wchar_t *w1,const wchar_t *w2,CkString &outStr);
bool GenRandomBytesENC(int numBytes,CkString &outStr);
bool GenerateSecretKey(const wchar_t *w1,CkByteData &outData);
bool GetEncodedIV(const wchar_t *w1,CkString &outIV);
bool GetEncodedKey(const wchar_t *w1,CkString &outKey);
bool GetEncodedSalt(const wchar_t *w1,CkString &outStr);
CkCertW *GetLastCert(void);
bool GetSignatureSigningTime(int index,SYSTEMTIME &outSysTime);
bool GetSignatureSigningTimeStr(int index,CkString &outStr);
CkCertW *GetSignerCert(int index);
bool HasSignatureSigningTime(int index);
bool HashBeginBytes(const CkByteData &data);
bool HashBeginString(const wchar_t *w1);
bool HashBytes(const CkByteData &bData,CkByteData &outData);
bool HashBytesENC(const CkByteData &bData,CkString &outStr);
bool HashFile(const wchar_t *w1,CkByteData &outData);
bool HashFileENC(const wchar_t *w1,CkString &outStr);
bool HashFinal(CkByteData &outBytes);
bool HashFinalENC(CkString &outStr);
bool HashMoreBytes(const CkByteData &data);
bool HashMoreString(const wchar_t *w1);
bool HashString(const wchar_t *w1,CkByteData &outData);
bool HashStringENC(const wchar_t *w1,CkString &outStr);
bool HmacBytes(const CkByteData &inBytes,CkByteData &outHmac);
bool HmacBytesENC(const CkByteData &inBytes,CkString &outEncodedHmac);
bool HmacString(const wchar_t *w1,CkByteData &outHmac);
bool HmacStringENC(const wchar_t *w1,CkString &outEncodedHmac);
bool InflateBytes(const CkByteData &bData,CkByteData &outData);
bool InflateBytesENC(const wchar_t *w1,CkByteData &outData);
bool InflateString(const CkByteData &bData,CkString &outStr);
bool InflateStringENC(const wchar_t *w1,CkString &outStr);
bool IsUnlocked(void);
bool MySqlAesDecrypt(const wchar_t *w1,const wchar_t *w2,CkString &outStr);
bool MySqlAesEncrypt(const wchar_t *w1,const wchar_t *w2,CkString &outStr);
bool OpaqueSignBytes(const CkByteData &bData,CkByteData &outData);
bool OpaqueSignBytesENC(const CkByteData &bData,CkString &outStr);
bool OpaqueSignString(const wchar_t *w1,CkByteData &outData);
bool OpaqueSignStringENC(const wchar_t *w1,CkString &outStr);
bool OpaqueVerifyBytes(const CkByteData &p7s,CkByteData &outOriginal);
bool OpaqueVerifyBytesENC(const wchar_t *w1,CkByteData &outOriginal);
bool OpaqueVerifyString(const CkByteData &p7s,CkString &outOriginal);
bool OpaqueVerifyStringENC(const wchar_t *w1,CkString &outOriginal);
bool Pbkdf1(const wchar_t *w1,const wchar_t *w2,const wchar_t *w3,const wchar_t *w4,int iterationCount,int outputKeyBitLen,const wchar_t *w5,CkString &outStr);
bool Pbkdf2(const wchar_t *w1,const wchar_t *w2,const wchar_t *w3,const wchar_t *w4,int iterationCount,int outputKeyBitLen,const wchar_t *w5,CkString &outStr);
void RandomizeIV(void);
void RandomizeKey(void);
bool ReEncode(const wchar_t *w1,const wchar_t *w2,const wchar_t *w3,CkString &outStr);
bool ReadFile(const wchar_t *w1,CkByteData &outBytes);
bool SaveLastError(const wchar_t *w1);
#if defined(WIN32) && !defined(WINCE) && !defined(__MINGW32__)
bool SetCSP(const CkCspW &csp);
#endif
bool SetDecryptCert(CkCertW &cert);
bool SetDecryptCert2(const CkCertW &cert,CkPrivateKeyW &key);
void SetEncodedIV(const wchar_t *w1,const wchar_t *w2);
void SetEncodedKey(const wchar_t *w1,const wchar_t *w2);
void SetEncodedSalt(const wchar_t *w1,const wchar_t *w2);
bool SetEncryptCert(const CkCertW &cert);
void SetHmacKeyBytes(const CkByteData &keyBytes);
void SetHmacKeyEncoded(const wchar_t *w1,const wchar_t *w2);
void SetHmacKeyString(const wchar_t *w1);
void SetIV(const unsigned char *pByteData,unsigned long szByteData);
void SetSecretKey(const unsigned char *pByteData,unsigned long szByteData);
void SetSecretKeyViaPassword(const wchar_t *w1);
bool SetSigningCert(const CkCertW &cert);
bool SetSigningCert2(const CkCertW &cert,CkPrivateKeyW &key);
bool SetVerifyCert(const CkCertW &cert);
bool SignBytes(const CkByteData &bData,CkByteData &outData);
bool SignBytesENC(const CkByteData &bData,CkString &outStr);
bool SignString(const wchar_t *w1,CkByteData &outData);
bool SignStringENC(const wchar_t *w1,CkString &outStr);
bool StringToBytes(const wchar_t *w1,const wchar_t *w2,CkByteData &outBytes);
bool TrimEndingWith(const wchar_t *w1,const wchar_t *w2,CkString &outStr);
bool UnlockComponent(const wchar_t *w1);
bool VerifyBytes(const CkByteData &bData1,const CkByteData &sigData);
bool VerifyBytesENC(const CkByteData &bData,const wchar_t *w1);
bool VerifyDetachedSignature(const wchar_t *w1,const wchar_t *w2);
bool VerifyP7M(const wchar_t *w1,const wchar_t *w2);
bool VerifyP7S(const wchar_t *w1,const wchar_t *w2);
bool VerifyString(const wchar_t *w1,const CkByteData &sigData);
bool VerifyStringENC(const wchar_t *w1,const wchar_t *w2);
bool WriteFile(const wchar_t *w1,const CkByteData &fileData);
const wchar_t *bytesToString(const CkByteData &inData,const wchar_t *w1);
const wchar_t *cadesSigPolicyHash(void);
const wchar_t *cadesSigPolicyId(void);
const wchar_t *cadesSigPolicyUri(void);
const wchar_t *charset(void);
const wchar_t *cipherMode(void);
const wchar_t *compressBytesENC(const CkByteData &bData);
const wchar_t *compressStringENC(const wchar_t *w1);
const wchar_t *compressionAlgorithm(void);
const wchar_t *cryptAlgorithm(void);
const wchar_t *debugLogFilePath(void);
const wchar_t *decodeString(const wchar_t *w1,const wchar_t *w2,const wchar_t *w3);
const wchar_t *decryptEncoded(const wchar_t *w1);
const wchar_t *decryptString(const CkByteData &bData);
const wchar_t *decryptStringENC(const wchar_t *w1);
const wchar_t *encode(const CkByteData &bData,const wchar_t *w1);
const wchar_t *encodeBytes(const void *pByteData,unsigned long szByteData,const wchar_t *w1);
const wchar_t *encodeString(const wchar_t *w1,const wchar_t *w2,const wchar_t *w3);
const wchar_t *encodingMode(void);
const wchar_t *encryptBytesENC(const CkByteData &bData);
const wchar_t *encryptEncoded(const wchar_t *w1);
const wchar_t *encryptStringENC(const wchar_t *w1);
const wchar_t *genEncodedSecretKey(const wchar_t *w1,const wchar_t *w2);
const wchar_t *genRandomBytesENC(int numBytes);
const wchar_t *getEncodedIV(const wchar_t *w1);
const wchar_t *getEncodedKey(const wchar_t *w1);
const wchar_t *getEncodedSalt(const wchar_t *w1);
const wchar_t *getSignatureSigningTimeStr(int index);
const wchar_t *hashAlgorithm(void);
const wchar_t *hashBytesENC(const CkByteData &bData);
const wchar_t *hashFileENC(const wchar_t *w1);
const wchar_t *hashFinalENC(void);
const wchar_t *hashStringENC(const wchar_t *w1);
const wchar_t *hmacBytesENC(const CkByteData &inBytes);
const wchar_t *hmacStringENC(const wchar_t *w1);
const wchar_t *inflateString(const CkByteData &bData);
const wchar_t *inflateStringENC(const wchar_t *w1);
const wchar_t *lastErrorHtml(void);
const wchar_t *lastErrorText(void);
const wchar_t *lastErrorXml(void);
const wchar_t *mySqlAesDecrypt(const wchar_t *w1,const wchar_t *w2);
const wchar_t *mySqlAesEncrypt(const wchar_t *w1,const wchar_t *w2);
const wchar_t *opaqueSignBytesENC(const CkByteData &bData);
const wchar_t *opaqueSignStringENC(const wchar_t *w1);
const wchar_t *opaqueVerifyString(const CkByteData &p7s);
const wchar_t *opaqueVerifyStringENC(const wchar_t *w1);
const wchar_t *pbesAlgorithm(void);
const wchar_t *pbesPassword(void);
const wchar_t *pbkdf1(const wchar_t *w1,const wchar_t *w2,const wchar_t *w3,const wchar_t *w4,int iterationCount,int outputKeyBitLen,const wchar_t *w5);
const wchar_t *pbkdf2(const wchar_t *w1,const wchar_t *w2,const wchar_t *w3,const wchar_t *w4,int iterationCount,int outputKeyBitLen,const wchar_t *w5);
const wchar_t *pkcs7CryptAlg(void);
const wchar_t *reEncode(const wchar_t *w1,const wchar_t *w2,const wchar_t *w3);
const wchar_t *signBytesENC(const CkByteData &bData);
const wchar_t *signStringENC(const wchar_t *w1);
const wchar_t *trimEndingWith(const wchar_t *w1,const wchar_t *w2);
const wchar_t *uuFilename(void);
const wchar_t *uuMode(void);
const wchar_t *version(void);

};
#ifndef __sun__
#pragma pack (pop)
#endif

#endif
