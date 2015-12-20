// CkCgi.h: interface for the CkCgi class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _CKCGI_H
#define _CKCGI_H




class CkByteData;
class CkString;

#include "CkMultiByteBase.h"
#include "CkString.h"

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
 


// CLASS: CkCgi
class CkCgi  : public CkMultiByteBase
{
    public:
	CkCgi();
	virtual ~CkCgi();

	void dispose(void) { }

	// BEGIN PUBLIC INTERFACE

	void SleepMs(int millisec) const;

	int get_ReadChunkSize(void) const;
	void put_ReadChunkSize(int numBytes);

	int get_IdleTimeoutMs(void) const;
	void put_IdleTimeoutMs(int millisec);

	int get_HeartbeatMs(void) const;
	void put_HeartbeatMs(int millisec);

	unsigned long get_SizeLimitKB(void) const;
	void put_SizeLimitKB(unsigned long kilobytes);

	void get_UploadDir(CkString &str);
	void put_UploadDir(const char *newVal);

	bool get_StreamToUploadDir(void) const;
	void put_StreamToUploadDir(bool newVal);

	// What do we have?
	bool IsPost(void);
	bool IsGet(void);
	bool IsHead(void);
	bool IsUpload(void);

	// Fetching file upload information.
	int get_NumUploadFiles(void);

	bool GetUploadFilename(int idx, CkString &outStr);
	unsigned long GetUploadSize(int idx);
	bool GetUploadData(int idx, CkByteData &outData);

	// Fetching Query parameters.
	bool GetParam(const char *paramName, CkString &outStr);

	int get_NumParams(void);
	bool GetParamName(int idx, CkString &outStr);
	bool GetParamValue(int idx, CkString &outStr);

	bool GetRawPostData(CkByteData &outData);

	bool GetEnv(const char *varName, CkString &outStr);

	// Read the HTTP request on stdin.
	// Also gathers the environment variables.
	bool ReadRequest(void);
#if defined(WIN32) && !defined(SINGLE_THREADED)
	bool AsyncReadRequest(void);
#endif

	// Asynchronous Progress Monitoring and Abort:
	unsigned long get_AsyncPostSize(void) const;
	unsigned long get_AsyncBytesRead(void) const;
	bool get_AsyncInProgress(void) const;
	bool get_AsyncSuccess(void) const;
	void AbortAsync(void);

	const char *getUploadFilename(int idx);
	const char *getParam(const char *paramName);
	const char *getParamName(int idx);
	const char *getParamValue(int idx);
	const char *getEnv(const char *varName);
	const char *uploadDir(void);


	// CGI_INSERT_POINT

	// END PUBLIC INTERFACE


    // For internal use only.
    private:
	// Don't allow assignment or copying these objects.
	CkCgi(const CkCgi &) { } 
	CkCgi &operator=(const CkCgi &) { return *this; }
	CkCgi(void *impl);


};
#ifndef __sun__
#pragma pack (pop)
#endif



#endif


