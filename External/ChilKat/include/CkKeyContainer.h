// CkKeyContainer.h: interface for the CkKeyContainer class.
//
//////////////////////////////////////////////////////////////////////
#ifndef _CKKEYCONTAINER_H
#define _CKKEYCONTAINER_H



class CkString;

#include "CkMultiByteBase.h"
#include "CkString.h"
#include "CkByteData.h"

class CkPrivateKey;
class CkPublicKey;
class CkStringArray;

#ifndef __sun__
#pragma pack (push, 8)
#endif
 

// CLASS: CkKeyContainer
class CkKeyContainer : public CkMultiByteBase 
{
    private:
	// Don't allow assignment or copying these objects.
	CkKeyContainer(const CkKeyContainer &) { } 
	CkKeyContainer &operator=(const CkKeyContainer &) { return *this; }
	CkKeyContainer(void *impl);

    public:

	CkKeyContainer();
	virtual ~CkKeyContainer();

	// May be called when finished with the object to free/dispose of any
	// internal resources held by the object. 
	void dispose(void);

	// BEGIN PUBLIC INTERFACE

	// If false, no key container has been opened.
	bool get_IsOpen(void);

	// If true, this key container is in the machine keyset.
	bool get_IsMachineKeyset(void);

	void get_ContainerName(CkString &name);

	// Useful for creating new key containers with unique names.
	void GenerateUuid(CkString &outGuid);

	// Create a new key container.  (If a container of the same name exists???)
	bool CreateContainer(const char *name, bool machineKeyset);
	bool OpenContainer(const char *name, bool needPrivateKeyAccess, bool machineKeyset);
	bool DeleteContainer(void);	// Delete this key container.
	void CloseContainer(void);

//	    RSA Base Provider
//	    Signature and ExchangeKeys
//		(minimum)     (default) (max)
//		    384 	512 	16,384
//
//	    RSA Strong and Enhanced Providers
//	    Signature and Exchange Keys
//		(minimum)     (default) (max)
//		    384 	1,024 	16,384

	// Pass 0 for the default key length.
	bool GenerateKeyPair(bool bKeyExchangePair, int keyLengthInBits);
	//bool GenerateKeyExchangePair(int keyLengthInBits);

	CkPrivateKey *GetPrivateKey(bool bKeyExchangePair);
	//CkPrivateKey *GetKeyExchangePrivateKey(void);

	CkPublicKey *GetPublicKey(bool bKeyExchangePair);
	//CkPublicKey *GetKeyExchangePublicKey(void);

	bool ImportPublicKey(CkPublicKey &key, bool bKeyExchangePair);
	bool ImportPrivateKey(CkPrivateKey &key, bool bKeyExchangePair);

	bool FetchContainerNames(bool bMachineKeyset);

	int GetNumContainers(bool bMachineKeyset);
	void GetNthContainerName(bool bMachineKeyset, int index, CkString &outName);

	const char *getNthContainerName(bool bMachineKeyset, int index);
	const char *containerName(void);
	const char *generateUuid(void);


	// CK_INSERT_POINT

	// END PUBLIC INTERFACE
	


};
#ifndef __sun__
#pragma pack (pop)
#endif


#endif
