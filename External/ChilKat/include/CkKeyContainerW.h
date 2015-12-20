#ifndef _CkKeyContainerW_H
#define _CkKeyContainerW_H
#pragma once

#include "CkWideCharBase.h"
#include "CkString.h"
#include "CkKeyContainer.h"

class CkPrivateKeyW;
class CkPublicKeyW;



#ifndef __sun__
#pragma pack (push, 8) 
#endif


class CkKeyContainerW : public CkWideCharBase
{
    private:
	CkKeyContainer *m_impl;

        // Disallow assignment or copying this object.
	CkKeyContainerW(const CkKeyContainerW &) { }
	CkKeyContainerW &operator=(const CkKeyContainerW &) { return *this; }


    public:
	CkKeyContainerW()
	    {
	    m_impl = new CkKeyContainer;
	    m_impl->put_Utf8(true);
	    }
	CkKeyContainerW(CkKeyContainer *p, bool bForMono) : m_impl(p)
	    {
	    if (!m_impl) m_impl = new CkKeyContainer;
	    m_impl->put_Utf8(true);
	    m_forMono = bForMono;
	    }
	CkKeyContainerW(bool bForMono)
	    {
	    m_impl = new CkKeyContainer;
	    m_impl->put_Utf8(true);
	    m_forMono = bForMono;
	    }
	CkKeyContainer *getMbPtr(void) { return m_impl; }
	CkKeyContainer &getMbRef(void) { return *m_impl; }
	const CkKeyContainer *getMbConstPtr(void) const { return m_impl; }
	const CkKeyContainer &getMbConstRef(void) const { return *m_impl; }
	void dispose(void) { m_impl->dispose();  }
	virtual ~CkKeyContainerW() { delete m_impl; m_impl = 0; }

void get_ContainerName(CkString &propVal);
void get_DebugLogFilePath(CkString &propVal);
void put_DebugLogFilePath(const wchar_t *w1);
bool get_IsMachineKeyset(void);
bool get_IsOpen(void);
void LastErrorHtml(CkString &propVal);
void LastErrorText(CkString &propVal);
void LastErrorXml(CkString &propVal);
bool get_VerboseLogging(void);
void put_VerboseLogging(bool propVal);
void get_Version(CkString &propVal);
void CloseContainer(void);
bool CreateContainer(const wchar_t *w1,bool machineKeyset);
bool DeleteContainer(void);
bool FetchContainerNames(bool bMachineKeyset);
bool GenerateKeyPair(bool bKeyExchangePair,int keyLengthInBits);
void GenerateUuid(CkString &outGuid);
void GetNthContainerName(bool bMachineKeyset,int index,CkString &outName);
int GetNumContainers(bool bMachineKeyset);
CkPrivateKeyW *GetPrivateKey(bool bKeyExchangePair);
CkPublicKeyW *GetPublicKey(bool bKeyExchangePair);
bool ImportPrivateKey(CkPrivateKeyW &key,bool bKeyExchangePair);
bool ImportPublicKey(CkPublicKeyW &key,bool bKeyExchangePair);
bool OpenContainer(const wchar_t *w1,bool needPrivateKeyAccess,bool machineKeyset);
bool SaveLastError(const wchar_t *w1);
const wchar_t *containerName(void);
const wchar_t *debugLogFilePath(void);
const wchar_t *generateUuid(void);
const wchar_t *getNthContainerName(bool bMachineKeyset,int index);
const wchar_t *lastErrorHtml(void);
const wchar_t *lastErrorText(void);
const wchar_t *lastErrorXml(void);
const wchar_t *version(void);

};
#ifndef __sun__
#pragma pack (pop)
#endif

#endif
