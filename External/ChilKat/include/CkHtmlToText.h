// CkHtmlToText.h: interface for the CkHtmlToText class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _CkHtmlToText_H
#define _CkHtmlToText_H




class CkByteData;
#include "CkString.h"
#include "CkMultiByteBase.h"

#ifdef WIN32
#pragma warning( disable : 4068 )
#pragma unmanaged
#endif


/*
    IMPORTANT: Objects returned by methods as non-const pointers must be deleted
    by the calling application. 

  */

#ifndef __sun__
#pragma pack (push, 8)
#endif
 

// CLASS: CkHtmlToText
class CkHtmlToText  : public CkMultiByteBase
{
    public:
	CkHtmlToText();
	virtual ~CkHtmlToText();

	// May be called when finished with the object to free/dispose of any
	// internal resources held by the object. 
	void dispose(void);

	// BEGIN PUBLIC INTERFACE

	bool UnlockComponent(const char *code);
	bool IsUnlocked(void) const;

	const char *readFileToString(const char *path, const char *srcCharset);
	bool ReadFileToString(const char *path, const char *srcCharset, CkString &outStr);
	bool WriteStringToFile(const char *str, const char *path, const char *charset);

	// TOTEXT_BEGIN
	bool ToText(const char *html, CkString &outStr);
	const char *toText(const char *html);
	// TOTEXT_END
	// RIGHTMARGIN_BEGIN
	int get_RightMargin(void);
	void put_RightMargin(int newVal);
	// RIGHTMARGIN_END
	// SUPPRESSLINKS_BEGIN
	bool get_SuppressLinks(void);
	void put_SuppressLinks(bool newVal);
	// SUPPRESSLINKS_END
	// DECODEHTMLENTITIES_BEGIN
	bool get_DecodeHtmlEntities(void);
	void put_DecodeHtmlEntities(bool newVal);
	// DECODEHTMLENTITIES_END

	// HTMLTOTEXT_INSERT_POINT

	// END PUBLIC INTERFACE



    // For internal use only.
    private:

	// Don't allow assignment or copying these objects.
	CkHtmlToText(const CkHtmlToText &) { } 
	CkHtmlToText &operator=(const CkHtmlToText &) { return *this; }

};
#ifndef __sun__
#pragma pack (pop)
#endif



#endif


