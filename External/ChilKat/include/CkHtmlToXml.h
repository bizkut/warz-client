// CkHtmlToXml.h: interface for the CkHtmlToXml class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _CkHtmlToXml_H
#define _CkHtmlToXml_H




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
 

// CLASS: CkHtmlToXml
class CkHtmlToXml  : public CkMultiByteBase
{
    public:
	CkHtmlToXml();
	virtual ~CkHtmlToXml();

	// May be called when finished with the object to free/dispose of any
	// internal resources held by the object. 
	void dispose(void);

	// BEGIN PUBLIC INTERFACE

	bool UnlockComponent(const char *code);
	bool IsUnlocked(void) const;

	// 0 (default) = convert to space, 1 = convert to &#160, 2 = drop.
	long get_Nbsp(void);
	void put_Nbsp(long v);
	
	void get_Html(CkString &outStr);
	void put_Html(const char *html);
	
	void get_XmlCharset(CkString &str);
	void put_XmlCharset(const char *charset);
	
	bool SetHtmlFromFile(const char *path);
	
	// Do it this way because we might want to 
	// extract other information out of the HTML.
	bool ToXml(CkString &outStr);
	bool Xml(CkString &outStr);    // Same as ToXml.
		
	bool ReadFileToString(const char *path, const char *srcCharset, CkString &outStr);

	bool WriteStringToFile(const char *str, const char *path, const char *charset);

	bool ConvertFile(const char *inHtmlPath, const char *destXmlPath);
		
	// Drop any tag type, such as "b", "br", etc.
	void DropTagType(const char *tagName);
	void UndropTagType(const char *tagName);

	// Drop b, i, u, em, strong, font, 
	void DropTextFormattingTags(void);
	void UndropTextFormattingTags(void);

	// Drop any tags not recognized as HTML tags.
	void put_DropCustomTags(bool v);
	bool get_DropCustomTags(void);
		
	const char *html(void);
	const char *xmlCharset(void);
	const char *xml(void);

	const char *toXml(void);
	const char *readFileToString(const char *path, const char *srcCharset);
	// SETHTMLBYTES_BEGIN
	void SetHtmlBytes(const CkByteData &inData);
	// SETHTMLBYTES_END

	// HTMLTOXML_INSERT_POINT

	// END PUBLIC INTERFACE



    // For internal use only.
    private:
	// Don't allow assignment or copying these objects.
	CkHtmlToXml(const CkHtmlToXml &) { } 
	CkHtmlToXml &operator=(const CkHtmlToXml &) { return *this; }



};
#ifndef __sun__
#pragma pack (pop)
#endif



#endif


