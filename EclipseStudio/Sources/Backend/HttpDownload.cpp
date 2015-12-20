#include "r3dPCH.h"
#include "r3d.h"

#include "HttpDownload.h"

HttpDownload::HttpDownload()
{
	InitHttp();
}

HttpDownload::~HttpDownload()
{
}

void HttpDownload::InitHttp()
{
	bool success = http_.UnlockComponent("ARKTOSHttp_decCLPWFQXmU");
	if(success != 1) 
	{
		r3dOutToLog("unable to unlock http component", MB_OK);
		return;
	}

	http_.put_EventCallbackObject(this);
	http_.put_UseBgThread(false);
	http_.put_KeepEventLog(false);
	http_.put_FollowRedirects(true);

	relocatedUrl_[0] = 0;
}

//
// callbacks
//

void HttpDownload::PercentDone(int pctDone, bool *abort)
{
	//r3dOutToLog("PercentDone: %d\n", pctDone);
	*abort = false;
}

void HttpDownload::ProgressInfo(const char *name, const char *value)
{
	if(strcmp(name, "ResponseContentLength") == 0) {
		DWORD contentLength;
		sscanf(value, "%u", &contentLength);
		return;
	}

	//r3dOutToLog("ProgressInfo: %s %s\n", name, value);
}

void HttpDownload::HttpBeginReceive(void)
{
	//r3dOutToLog("HttpBeginReceive\n");
}

void HttpDownload::HttpEndReceive(bool success)
{
	//r3dOutToLog("HttpEndReceive\n");
}

void HttpDownload::ReceiveRate(unsigned long byteCount, unsigned long bytesPerSec)
{
	//r3dOutToLog("ReceiveRate %d %d\n", byteCount, bytesPerSec);
}

bool HttpDownload::Get(const char* full_url, CkByteData& data, DWORD start, DWORD length)
{
	//r3dOutToLog("http::Get %s %d-%d\n", full_url, start, start + length);
	r3d_assert(length < 0x7FFF0000);

	relocatedUrl_[0] = 0;

	const char* domain = http_.getDomain(full_url);
	if(!domain) {
		r3dOutToLog("failed to parse url %s\n", full_url);
		return false;
	}

	CkHttpRequest req;
	req.SetFromUrl(full_url);
	if(start > 0 || length > 0)
	{
		char range[128];
		// note range is inclusive
		sprintf_s(range, sizeof(range), "bytes=%u-%u", start, start + length - 1);
		req.AddHeader("Range", range);
	}
	
	int port = 80;
	if(strnicmp(full_url, "https", 5) == 0)
		port = 443;

	CkHttpResponse* resp = http_.SynchronousRequest(domain, port, port != 80, req);
	if(!resp) {
		r3dOutToLog("!resp\n");
		return false;
	}

	// handle '301 relocated permanently' situation
	if(resp->get_StatusCode() == 301)
	{
		const char* reloc_url = resp->getHeaderField("Location");
		if(!reloc_url || reloc_url[0] == 0) {
			r3dOutToLog("failed to get %s, no 'Location' after redirect\n", full_url);
			return false;
		}
		//r3dOutToLog("->%s\n", reloc_url);
		strcpy(relocatedUrl_, reloc_url);

		// make a 2nd request.
		CkHttpRequest req2;
		req2.SetFromUrl(relocatedUrl_);
		if(start > 0 || length > 0) {
			char range[128];
			// note range is inclusive
			sprintf_s(range, sizeof(range), "bytes=%u-%u", start, start + length - 1);
			req2.AddHeader("Range", range);
		}

		delete resp;

		domain = http_.getDomain(relocatedUrl_);
		resp = http_.SynchronousRequest(domain, 80, false, req2);
		if(!resp) {
			r3dOutToLog("!resp\n");
			return false;
		}
	}

	int   code = resp->get_StatusCode();
	DWORD recv = resp->get_ContentLength();

	if(code < 200 || code > 206) {
		r3dOutToLog("failed to get %s, http code %d\n", full_url, code);
		delete resp;
		return false;
	}

	if(length > 0 && recv != length) {
		r3dOutToLog("failed to get %s, recv len mismatch %u vs %u\n", full_url, recv, length);
		delete resp;
		return false;
	}

	resp->get_Body(data);
	delete resp;

	return true;
}
