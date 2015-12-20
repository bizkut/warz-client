#include "r3dPCH.h"
#include "r3d.h"

#include "WOBackendAPI.h"

//#if 1
	const char*	gDomainIP     = "apiwarz.kongsi.asia";
	const char*	gDomainBaseUrl= "/WarZ/api/";
	int		gDomainPort   = 80;
	bool		gDomainUseSSL = false;
/*#else
	// local site version 
	const char*	gDomainIP     = "localhost";
	const char*	gDomainBaseUrl= "/Site/";
	int		gDomainPort   = 56016;
	bool		gDomainUseSSL = false;
#endif*/	
	
CWOBackendReq::CWOBackendReq(const char* url)
{
	Init(url);
}

void CWOBackendReq::Init(const char* url)
{
	resp_       = NULL;
	
	savedUrl_   = url;
	resultCode_ = 0;
	bodyStr_    = "";
	bodyLen_    = 0;
	
	bool success;
	success = http.UnlockComponent("ARKTOSHttp_decCLPWFQXmU");
	if(!success || !http.IsUnlocked()) {
		r3dError("failed to unlock CkHttp");
		return;
	}
	http.put_ConnectTimeout(30);
	http.put_ReadTimeout(60);

	// create request
	char fullUrl[512];
	if(url[0] != '/')
	  sprintf(fullUrl, "%s%s", gDomainBaseUrl, url);
	else 
	  sprintf(fullUrl, "%s", url);

	req.put_HttpVerb("POST");
	req.put_ContentType("application/x-www-form-urlencoded");
	req.put_Path(fullUrl);
	req.put_Utf8(true);
}

CWOBackendReq::~CWOBackendReq()
{
	SAFE_DELETE(resp_);
}

void CWOBackendReq::AddSessionInfo(DWORD id, DWORD sid)
{
	r3d_assert(id);
	
	AddParam("s_id",  (int)id);
	AddParam("s_key", (int)sid);
}

void CWOBackendReq::AddParam(const char* name, const char* val)
{
	req.AddParam(name, val);
}

void CWOBackendReq::AddParam(const char* name, int val)
{
	char	buf[1024];
	sprintf(buf, "%d", val);
	AddParam(name, buf);
}

int CWOBackendReq::ParseResult(CkHttpResponse* resp)
{
	if(resp == NULL)
	{
		r3dOutToLog("WO_API: http timeout\n");
		return 8;
	}

	if(resp->get_StatusCode() != 200)
	{
		r3dOutToLog("WO_API: returned http%d\n", resp->get_StatusCode());
		return 8;
	}
	
	// NOTE: we can't use getBodyStr() because it skip zeroes inside answer body
	resp->get_Body(data_);
	data_.appendChar(0);
	bodyStr_ = (const char*)data_.getData();
	bodyLen_ = data_.getSize() - 1;
	
	// validate http answer
	if(bodyLen_ < 4)
	{
		r3dOutToLog("WO_API: too small answer\n");
		return 9;
	}
	
	// check if we got XML header "<?xml"
	if(bodyStr_[0] == '<' && bodyStr_[1] == '?' && bodyStr_[2] == 'x')
	{
		return 0;
	}
	
	// check for 'WO_x' header
	if(bodyStr_[0] != 'W' || bodyStr_[1] != 'O' || bodyStr_[2] != '_')
	{
		r3dOutToLog("WO_API: wrong header: %s\n", bodyStr_);
		return 9;
	}

	char resultStr[2];
	resultStr[0] = bodyStr_[3];
	resultStr[1] = 0;
	int resultCode = 8;
	sscanf(resultStr, "%x", &resultCode);
	
	// offset body content past header
	bodyStr_ += 4;
	bodyLen_ -= 4;

	// parse result code
	switch(resultCode)
	{
		case 0:	// operation ok
			break;
			
		case 1:	// session not valid
			r3dOutToLog("WO_API: session disconnected\n");
			break;

		default:
			r3dOutToLog("WO_API: failed with error code %d %s\n", resultCode, bodyStr_);
			break;
	}

	return resultCode;
}

bool CWOBackendReq::Issue()
{
	SAFE_DELETE(resp_);

	float t1 = r3dGetTime();
	resp_ = http.SynchronousRequest(gDomainIP, gDomainPort, gDomainUseSSL, req);
	#ifndef FINAL_BUILD
	//r3dOutToLog("WOApi: %s NETWORK time: %.4f\n", savedUrl_, r3dGetTime()-t1);
	#endif

	resultCode_ = ParseResult(resp_);
	return resultCode_ == 0;
}


void CWOBackendReq::ParseXML(pugi::xml_document& xmlFile)
{
	pugi::xml_parse_result parseResult = xmlFile.load_buffer_inplace((void*)bodyStr_, bodyLen_);
	if(!parseResult)
		r3dError("Failed to parse server XML, error: %s", parseResult.description());
}