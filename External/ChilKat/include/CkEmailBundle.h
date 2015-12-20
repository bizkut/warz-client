// CkEmailBundle.h: interface for the CkEmailBundle class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _CKEMAILBUNDLE_H
#define _CKEMAILBUNDLE_H




class CkStringArray;
class CkEmail;
#include "CkMultiByteBase.h"
#include "CkString.h"

/*
    IMPORTANT: Objects returned by methods as non-const pointers must be deleted
    by the calling application. 

  */

#ifndef __sun__
#pragma pack (push, 8)
#endif
 

// CLASS: CkEmailBundle
class CkEmailBundle  : public CkMultiByteBase
{
    private:
	// Don't allow assignment or copying these objects.
	CkEmailBundle(const CkEmailBundle &) { } 
	CkEmailBundle &operator=(const CkEmailBundle &) { return *this; }
	CkEmailBundle(void *impl);

    public:
	void inject(void *impl);

	CkEmailBundle();
	virtual ~CkEmailBundle();

	// May be called when finished with the object to free/dispose of any
	// internal resources held by the object. 
	void dispose(void);

	bool RemoveEmail(const CkEmail *email);
	bool AddEmail(const CkEmail *email);

	// BEGIN PUBLIC INTERFACE

	CkStringArray *GetUidls(void);
	bool RemoveEmail(const CkEmail &email) { return this->RemoveEmail(&email); }
	CkEmail *GetEmail(long index);
	bool AddEmail(const CkEmail &email) { return this->AddEmail(&email); }

	long get_MessageCount(void);

	bool GetXml(CkString &outXml);
	bool SaveXml(const char *path);
	
	bool LoadXml(const char *path);
	bool LoadXmlString(const char *xmlStr);

	void SortBySubject(bool ascending);
	void SortBySender(bool ascending);
	void SortByRecipient(bool ascending);
	void SortByDate(bool ascending);

	const char *getXml(void);
	// FINDBYHEADER_BEGIN
	CkEmail *FindByHeader(const char *name, const char *value);
	// FINDBYHEADER_END
	// REMOVEEMAILBYINDEX_BEGIN
	bool RemoveEmailByIndex(int index);
	// REMOVEEMAILBYINDEX_END

	// EMAILBUNDLE_INSERT_POINT

	// END PUBLIC INTERFACE



};
#ifndef __sun__
#pragma pack (pop)
#endif



#endif
