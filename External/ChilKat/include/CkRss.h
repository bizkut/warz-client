// CkRss.h: interface for the CkRss class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _CKRss_H
#define _CKRss_H



#include "CkString.h"
class CkByteData;

#if !defined(WIN32) && !defined(WINCE)
#include "SystemTime.h"
#endif

/*
    IMPORTANT: Objects returned by methods as non-const pointers must be deleted
    by the calling application. 

  */

#include "CkMultiByteBase.h"

class CkRssProgress;

#ifndef __sun__
#pragma pack (push, 8)
#endif
 

// CLASS: CkRss
class CkRss  : public CkMultiByteBase
{
    private:
	CkRssProgress *m_callback;

	// Don't allow assignment or copying these objects.
	CkRss(const CkRss &) { } 
	CkRss &operator=(const CkRss &) { return *this; }
	CkRss(void *impl);

    public:
	CkRss();
	virtual ~CkRss();

	// May be called when finished with the object to free/dispose of any
	// internal resources held by the object. 
	void dispose(void);

	// BEGIN PUBLIC INTERFACE
	// SETDATESTR_BEGIN
	void SetDateStr(const char *tag, const char *dateTimeStr);
	// SETDATESTR_END

	// GETDATESTR_BEGIN
	bool GetDateStr(const char *tag, CkString &outStr);
	const char *getDateStr(const char *tag);
	// GETDATESTR_END

	
	CkRssProgress *get_EventCallbackObject(void) const;
	void put_EventCallbackObject(CkRssProgress *progress);


    	// NUMCHANNELS_BEGIN
	int get_NumChannels(void);
	// NUMCHANNELS_END
	// NUMITEMS_BEGIN
	int get_NumItems(void);
	// NUMITEMS_END
	// ADDNEWCHANNEL_BEGIN
	CkRss *AddNewChannel(void);
	// ADDNEWCHANNEL_END
	// ADDNEWIMAGE_BEGIN
	CkRss *AddNewImage(void);
	// ADDNEWIMAGE_END
	// ADDNEWITEM_BEGIN
	CkRss *AddNewItem(void);
	// ADDNEWITEM_END
	// DOWNLOADRSS_BEGIN
	// eventCallbacks
	bool DownloadRss(const char *url);
	// DOWNLOADRSS_END
	// GETATTR_BEGIN
	bool GetAttr(const char *tag, const char *attrName, CkString &outStr);
	const char *getAttr(const char *tag, const char *attrName);
	// GETATTR_END
	// GETCHANNEL_BEGIN
	CkRss *GetChannel(int index);
	// GETCHANNEL_END
	// GETCOUNT_BEGIN
	int GetCount(const char *tag);
	// GETCOUNT_END
	// GETDATE_BEGIN
	bool GetDate(const char *tag, SYSTEMTIME &outSysTime);
	// GETDATE_END
	// GETIMAGE_BEGIN
	CkRss *GetImage(void);
	// GETIMAGE_END
	// GETINT_BEGIN
	int GetInt(const char *tag);
	// GETINT_END
	// GETITEM_BEGIN
	CkRss *GetItem(int index);
	// GETITEM_END
	// GETSTRING_BEGIN
	bool GetString(const char *tag, CkString &outStr);
	const char *getString(const char *tag);
	// GETSTRING_END
	// LOADRSSFILE_BEGIN
	bool LoadRssFile(const char *path);
	// LOADRSSFILE_END
	// LOADRSSSTRING_BEGIN
	bool LoadRssString(const char *rssString);
	// LOADRSSSTRING_END
	// MGETATTR_BEGIN
	bool MGetAttr(const char *tag, int index, const char *attrName, CkString &outStr);
	const char *mGetAttr(const char *tag, int index, const char *attrName);
	// MGETATTR_END
	// MGETSTRING_BEGIN
	bool MGetString(const char *tag, int index, CkString &outStr);
	const char *mGetString(const char *tag, int index);
	// MGETSTRING_END
	// MSETATTR_BEGIN
	bool MSetAttr(const char *tag, int index, const char *attrName, const char *value);
	// MSETATTR_END
	// MSETSTRING_BEGIN
	bool MSetString(const char *tag, int index, const char *value);
	// MSETSTRING_END
	// NEWRSS_BEGIN
	void NewRss(void);
	// NEWRSS_END
	// REMOVE_BEGIN
	void Remove(const char *tag);
	// REMOVE_END
	// SETATTR_BEGIN
	void SetAttr(const char *tag, const char *attrName, const char *value);
	// SETATTR_END
	// SETDATE_BEGIN
	void SetDate(const char *tag, SYSTEMTIME &dateTime);
	// SETDATE_END
	// SETDATENOW_BEGIN
	void SetDateNow(const char *tag);
	// SETDATENOW_END
	// SETINT_BEGIN
	void SetInt(const char *tag, int value);
	// SETINT_END
	// SETSTRING_BEGIN
	void SetString(const char *tag, const char *value);
	// SETSTRING_END
	// TOXMLSTRING_BEGIN
	bool ToXmlString(CkString &outStr);
	const char *toXmlString(void);
	// TOXMLSTRING_END

	// RSS_INSERT_POINT

	// END PUBLIC INTERFACE


};
#ifndef __sun__
#pragma pack (pop)
#endif



#endif


