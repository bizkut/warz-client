#pragma once

class CUserProfile;

#include "CkHttpRequest.h"
#include "CkHttp.h"
#include "CkHttpResponse.h"
#include "CkByteData.h"

class CWOBackendReq
{
  private:
	const char*	savedUrl_;
	CkHttp		http;
	CkHttpRequest	req;
	CkHttpResponse* resp_;
	
  public:
	/*
	WO API error codes:
	0: operation Ok
	1: session not valid (disconnected)
	5: php<->SQL error
	6: SQL parameters error
	7: SQL logic error (not enough money, etc)
	8: http connection
	9: http output error
	*/
	int		resultCode_;
	
	CkByteData	data_;
	const char*	bodyStr_;
	int		bodyLen_;

	int		ParseResult(CkHttpResponse* resp);

	void		Init(const char* url);
  public:
	CWOBackendReq(const CUserProfile* prof, const char* url);
	CWOBackendReq(const char* url);
	~CWOBackendReq();
	
	void		AddSessionInfo(DWORD id, DWORD sid);
	void		SetTimeouts(int connect, int read)
	{
	  http.put_ConnectTimeout(connect);
	  http.put_ReadTimeout(read);
	}

	void		AddParam(const char* name, const char* val);
	void		AddParam(const char* name, int val);
	
	bool		Issue();
	
	void		ParseXML(pugi::xml_document& xmlFile);
};
