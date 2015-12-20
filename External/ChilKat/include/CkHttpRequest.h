// CkHttpRequest.h: interface for the CkHttpRequest class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _CKHTTPREQUEST_H
#define _CKHTTPREQUEST_H



#include "CkString.h"
class CkByteData;

/*
    IMPORTANT: Objects returned by methods as non-const pointers must be deleted
    by the calling application. 

  */

#include "CkMultiByteBase.h"

#ifndef __sun__
#pragma pack (push, 8)
#endif
 

// CLASS: CkHttpRequest
class CkHttpRequest  : public CkMultiByteBase
{
    private:
	// Don't allow assignment or copying these objects.
	CkHttpRequest(const CkHttpRequest &) { } 
	CkHttpRequest &operator=(const CkHttpRequest &) { return *this; }
	CkHttpRequest(void *impl);

    public:
	CkHttpRequest();
	virtual ~CkHttpRequest();

	// May be called when finished with the object to free/dispose of any
	// internal resources held by the object. 
	void dispose(void);

	// BEGIN PUBLIC INTERFACE
	// REMOVEHEADER_BEGIN
	bool RemoveHeader(const char *name);
	// REMOVEHEADER_END


	void UsePost(void);
	void UseGet(void);
	void UseHead(void);
	void UseXmlHttp(const char *xmlBody);
	void UseUpload(void);

	void SetFromUrl(const char *url);

	// Path if not set from a URL.
	void put_Path(const char *path);
	void get_Path(CkString &str);
	
	// Default is "utf-8"
	void put_Charset(const char *charset);
	void get_Charset(CkString &str);
	
	// If true, the charset attribute is included in the
	// content-type header field in the HTTP request.
	void put_SendCharset(bool val);
	bool get_SendCharset(void);


	void put_HttpVersion(const char *version);
	void get_HttpVersion(CkString &str);

	// HTTP headers.
	void AddHeader(const char *name, const char *value);

	// Add a string request parameter.
	void AddParam(const char *name, const char *value);
	void RemoveParam(const char *name);

	// Data to upload.
	bool AddFileForUpload(const char *name, const char *path);

	// Generate the HTTP request header for this object.
	bool GenerateRequestText(CkString &outStr);

	// Get a header field by name.
	bool GetHeaderField(const char *name, CkString &outStr);
	int get_NumHeaderFields(void);
	bool GetHeaderName(int index, CkString &outStr);
	bool GetHeaderValue(int index, CkString &outStr);

	// Get request parameters.
	bool GetParam(const char *name, CkString &outStr);
	int get_NumParams(void);
	bool GetParamName(int index, CkString &outStr);
	bool GetParamValue(int index, CkString &outStr);


	const char *path(void);
	const char *charset(void);
	const char *httpVersion(void);
	const char *getHeaderField(const char *name);
	const char *getHeaderName(int index);
	const char *getHeaderValue(int index);
	const char *getParam(const char *name);
	const char *getParamName(int index);
	const char *getParamValue(int index);
	const char *generateRequestText(void);

	// REMOVEALLPARAMS_BEGIN
	void RemoveAllParams(void);
	// REMOVEALLPARAMS_END
	// LOADBODYFROMFILE_BEGIN
	bool LoadBodyFromFile(const char *path);
	// LOADBODYFROMFILE_END
	// LOADBODYFROMBYTES_BEGIN
	bool LoadBodyFromBytes(const CkByteData &binaryData);
	// LOADBODYFROMBYTES_END
	// LOADBODYFROMSTRING_BEGIN
	bool LoadBodyFromString(const char *bodyStr, const char *charset);
	// LOADBODYFROMSTRING_END
	// USEPUT_BEGIN
	void UsePut(void);
	// USEPUT_END
	// ADDBYTESFORUPLOAD_BEGIN
	bool AddBytesForUpload(const char *name, const char *filename, const CkByteData &byteData);
	// ADDBYTESFORUPLOAD_END
	// USEPOSTMULTIPARTFORM_BEGIN
	void UsePostMultipartForm(void);
	// USEPOSTMULTIPARTFORM_END
	// GETURLENCODEDPARAMS_BEGIN
	bool GetUrlEncodedParams(CkString &outStr);
	const char *getUrlEncodedParams(void);
	// GETURLENCODEDPARAMS_END
	// ADDSTRINGFORUPLOAD_BEGIN
	bool AddStringForUpload(const char *name, const char *filename, const char *strData, const char *charset);
	// ADDSTRINGFORUPLOAD_END
	// ADDFILEFORUPLOAD2_BEGIN
	bool AddFileForUpload2(const char *name, const char *path, const char *contentType);
	// ADDFILEFORUPLOAD2_END
	// ADDSTRINGFORUPLOAD2_BEGIN
	bool AddStringForUpload2(const char *name, const char *filename, const char *strData, const char *charset, const char *contentType);
	// ADDSTRINGFORUPLOAD2_END
	// ADDBYTESFORUPLOAD_BEGIN
	bool AddBytesForUpload2(const char *name, const char *filename, const CkByteData &byteData, const char *contentType);
	// ADDBYTESFORUPLOAD_END
	// USEUPLOADPUT_BEGIN
	void UseUploadPut(void);
	// USEUPLOADPUT_END
	// HTTPVERB_BEGIN
	void get_HttpVerb(CkString &str);
	const char *httpVerb(void);
	void put_HttpVerb(const char *newVal);
	// HTTPVERB_END
	// CONTENTTYPE_BEGIN
	void get_ContentType(CkString &str);
	const char *contentType(void);
	void put_ContentType(const char *newVal);
	// CONTENTTYPE_END
	// ENTIREHEADER_BEGIN
	void get_EntireHeader(CkString &str);
	const char *entireHeader(void);
	void put_EntireHeader(const char *newVal);
	// ENTIREHEADER_END
	// STREAMBODYFROMFILE_BEGIN
	bool StreamBodyFromFile(const char *path);
	// STREAMBODYFROMFILE_END

	// HTTPREQUEST_INSERT_POINT

	// END PUBLIC INTERFACE


};
#ifndef __sun__
#pragma pack (pop)
#endif



#endif


