// CkCsp.h: interface for the CkCsp class.
//
//////////////////////////////////////////////////////////////////////
#if defined(WIN32) && !defined(__MINGW32__)

#ifndef _CkCsp_H
#define _CkCsp_H



class CkStringArray;

#include "CkMultiByteBase.h"
#include "CkString.h"

#ifndef __sun__
#pragma pack (push, 8)
#endif
 

// CLASS: CkCsp
class CkCsp  : public CkMultiByteBase
{
    private:
	// Don't allow assignment or copying these objects.
	CkCsp(const CkCsp &) { } 
	CkCsp &operator=(const CkCsp &) { return *this; }
	CkCsp(void *impl);

    public:

	CkCsp();
	virtual ~CkCsp();

	void dispose(void) { }

	// BEGIN PUBLIC INTERFACE
	// INITIALIZE_BEGIN
	bool Initialize(void);
	// INITIALIZE_END

	// KEYCONTAINERNAME_BEGIN
	void get_KeyContainerName(CkString &str);
	const char *keyContainerName(void);
	void put_KeyContainerName(const char *newVal);
	// KEYCONTAINERNAME_END


	bool SetProviderMicrosoftBase();
	bool SetProviderMicrosoftStrong();
	bool SetProviderMicrosoftEnhanced();
	bool SetProviderMicrosoftRsaAes();
	long get_ProviderType();

	CkStringArray *GetKeyContainerNames(void);

	long SetHashAlgorithm(const char *name);
	bool HasHashAlgorithm(const char *name, long numBits);

	long SetEncryptAlgorithm(const char *name);
	bool HasEncryptAlgorithm(const char *name, long numBits);
	long get_HashAlgorithmID();
	long get_EncryptAlgorithmID();
	long get_HashNumBits();
	long get_EncryptNumBits();
	void get_HashAlgorithm(CkString &alg);
	void get_EncryptAlgorithm(CkString &alg);
	long get_NumKeyContainers();

	bool NthKeyContainerName(long index, CkString &outName);
	long NthSignatureNumBits(long index);
	bool NthSignatureAlgorithm(long index, CkString &outName);
	long NthKeyExchangeNumBits(long index);
	bool NthKeyExchangeAlgorithm(long index, CkString &outName);
	long NthHashNumBits(long index);
	bool NthHashAlgorithmName(long index, CkString &outName);
	long NthEncryptionNumBits(long index);
	bool NthEncryptionAlgorithm(long index, CkString &outName);

	// Changed to get/put_KeyContainerName
	//void get_KeyContainer(CkString &name);
	//void put_KeyContainer(const char *name);

	void get_ProviderName(CkString &name);
	void put_ProviderName(const char *name);
	long get_NumKeyExchangeAlgorithms();
	long get_NumSignatureAlgorithms();
	long get_NumEncryptAlgorithms();
	long get_NumHashAlgorithms();

	bool get_MachineKeyset();
	void put_MachineKeyset(bool val);

	const char *hashAlgorithm(void);
	const char *encryptAlgorithm(void);
	const char *nthKeyContainerName(long index);
	const char *nthSignatureAlgorithm(long index);
	const char *nthKeyExchangeAlgorithm(long index);
	const char *nthHashAlgorithmName(long index);
	const char *nthEncryptionAlgorithm(long index);
	const char *providerName(void);

	// CSP_INSERT_POINT

	// END PUBLIC INTERFACE



};
#ifndef __sun__
#pragma pack (pop)
#endif



#endif

#endif	// WIN32
