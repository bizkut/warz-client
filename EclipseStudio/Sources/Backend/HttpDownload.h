#pragma once

#include "CkHttpRequest.h"
#include "CkHttp.h"
#include "CkHttpResponse.h"
#include "CkHttpProgress.h"
#include "CkByteData.h"

class HttpDownload : public CkHttpProgress
{
private:
	friend class HttpProgress;

	CkHttp		http_;

	void		InitHttp();
	char		relocatedUrl_[512];

private:
	// callbacks from CkHttpProgress
	virtual void PercentDone(int pctDone, bool *abort);
	virtual void ProgressInfo(const char *name, const char *value);

	virtual void HttpBeginReceive(void);
	virtual void HttpEndReceive(bool success);

	virtual void ReceiveRate(unsigned long byteCount, unsigned long bytesPerSec);

public:
	HttpDownload();
	~HttpDownload();

	bool		Get(const char* full_url, CkByteData& data, DWORD start = 0, DWORD length = 0);
	const char*	RelocatedUrl() const {
		return relocatedUrl_;
	}
};
