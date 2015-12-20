// CkXmp.h: interface for the CkXmp class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _CkXmp_H
#define _CkXmp_H
	
#ifndef WIN32
#include "int64.h"
#endif

#include "CkString.h"
#include "CkMultiByteBase.h"

class CkXml;
class CkStringArray;
class CkByteData;


#if !defined(WIN32) && !defined(WINCE)
#include "SystemTime.h"
#endif

#ifndef __sun__
#pragma pack (push, 8)
#endif
 


// CLASS: CkXmp
class CkXmp  : public CkMultiByteBase
{
    private:
	
	// Don't allow assignment or copying these objects.
	CkXmp(const CkXmp &);
	CkXmp &operator=(const CkXmp &);

    public:
	CkXmp(void);
	virtual ~CkXmp(void);

	static CkXmp *createNew(void);
	void inject(void *impl);

	// May be called when finished with the object to free/dispose of any
	// internal resources held by the object. 
	void dispose(void);

	// BEGIN PUBLIC INTERFACE

	// ----------------------
	// Properties
	// ----------------------
	int get_NumEmbedded(void);

	bool get_StructInnerDescrip(void);
	void put_StructInnerDescrip(bool newVal);


	// ----------------------
	// Methods
	// ----------------------
	bool AddArray(CkXml &xml, const char *arrType, const char *propName, CkStringArray &values);

	void AddNsMapping(const char *ns, const char *uri);

	bool AddSimpleDate(CkXml &xml, const char *propName, SYSTEMTIME &sysTime);

	bool AddSimpleInt(CkXml &xml, const char *propName, int propVal);

	bool AddSimpleStr(CkXml &xml, const char *propName, const char *propVal);

	bool AddStructProp(CkXml &xml, const char *structName, const char *propName, const char *propVal);

	bool Append(CkXml &xml);

	bool DateToString(SYSTEMTIME &sysTime, CkString &outStr);

	CkStringArray *GetArray(CkXml &xml, const char *propName);

	CkXml *GetEmbedded(int index);

	CkXml *GetProperty(CkXml &xml, const char *propName);

	bool GetSimpleDate(CkXml &xml, const char *propName, SYSTEMTIME &outSysTime);

	int GetSimpleInt(CkXml &xml, const char *propName);

	bool GetSimpleStr(CkXml &xml, const char *propName, CkString &outStr);

	CkStringArray *GetStructPropNames(CkXml &xml, const char *structName);

	bool GetStructValue(CkXml &xml, const char *structName, const char *propName, CkString &outStr);

	bool LoadAppFile(const char *path);

	bool LoadFromBuffer(const CkByteData &byteData, const char *ext);

	CkXml *NewXmp(void);

	void RemoveAllEmbedded(void);

	bool RemoveArray(CkXml &xml, const char *propName);

	void RemoveEmbedded(int index);

	void RemoveNsMapping(const char *ns);

	bool RemoveSimple(CkXml &xml, const char *propName);

	bool RemoveStruct(CkXml &xml, const char *structName);

	bool RemoveStructProp(CkXml &xml, const char *structName, const char *propName);

	bool SaveAppFile(const char *path);

	//bool SaveLastError(const char *path);

	bool SaveToBuffer(CkByteData &outBytes);

	bool StringToDate(const char *str, SYSTEMTIME &outSysTime);

	bool UnlockComponent(const char *unlockCode);


	const char *getSimpleStr(CkXml &xml, const char *propName);
	const char *getStructValue(CkXml &xml, const char *structName, const char *propName);
	const char *simpleStr(CkXml &xml, const char *propName);
	const char *structValue(CkXml &xml, const char *structName, const char *propName);
	const char *dateToString(SYSTEMTIME &sysTime);

	// END PUBLIC INTERFACE


};
#ifndef __sun__
#pragma pack (pop)
#endif


#endif

