// CkZipCrc.h: interface for the CkZipCrc class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _CKZIPCRC_H
#define _CKZIPCRC_H




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
 

#ifndef WIN32
#include <stdint.h>
#endif

// CLASS: CkZipCrc
class CkZipCrc  : public CkMultiByteBase
{
    public:
	CkZipCrc();
	virtual ~CkZipCrc();

	// May be called when finished with the object to free/dispose of any
	// internal resources held by the object. 
	void dispose(void);

	// BEGIN PUBLIC INTERFACE
	bool ToHex(unsigned long crc, CkString &outStr) const;

#ifdef WIN32
	unsigned long FileCrc(const char *path);
	unsigned long CalculateCrc(const CkByteData &byteData);
	unsigned long EndStream(void);
#else
	uint32_t FileCrc(const char *path);
	uint32_t CalculateCrc(const CkByteData &byteData);
	uint32_t EndStream(void);
#endif
	void MoreData(const CkByteData &byteData);
	void BeginStream(void);

	const char *toHex(unsigned long crc);

	// CK_INSERT_POINT

	// END PUBLIC INTERFACE


    // For internal use only.
    private:
	// Don't allow assignment or copying these objects.
	CkZipCrc(const CkZipCrc &) { } 
	CkZipCrc &operator=(const CkZipCrc &) { return *this; }

    public:
	CkZipCrc(void *impl);


};
#ifndef __sun__
#pragma pack (pop)
#endif



#endif


