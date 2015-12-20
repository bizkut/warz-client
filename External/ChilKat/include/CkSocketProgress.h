// CkSocketProgress.h: interface for the CkSocketProgress class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _CKSOCKETPROGRESS_H
#define _CKSOCKETPROGRESS_H

#include "CkObject.h"

class CkString;

#ifndef __sun__
#pragma pack (push, 8)
#endif
 

class CkSocketProgress : public CkObject 
{
    public:
	CkSocketProgress() { }
	virtual ~CkSocketProgress() { }

	// Called periodically to check to see if the method call should be aborted.
	virtual void AbortCheck(bool *abort) { }
	virtual void PercentDone(int pctDone, bool *abort) { }

	virtual void ProgressInfo(const char *name, const char *value) { }

	// Asynch socket completion events.
	virtual void DnsComplete(int objectId, const char *hostname, const char *ipAddress, bool success) { }
	virtual void ConnectComplete(int objectId, const char *hostname, long port, bool success) { }
	virtual void AcceptComplete(int objectId, const char *ipAddress, long port, bool success) { }
	virtual void ReverseDnsComplete(int objectId, const char *ipAddress, const char *hostname, bool success) { }
	virtual void SendComplete(int objectId, long numBytes, bool success) { }
	virtual void ReceiveBytesComplete(int objectId, const unsigned char *bytesReceived, unsigned long numBytesReceived, bool success) { }
	virtual void ReceiveStringComplete(int objectId, CkString &strReceived, bool success) { }

};
#ifndef __sun__
#pragma pack (pop)
#endif


#endif
