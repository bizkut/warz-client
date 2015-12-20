#include "r3dPCH.h"
#include "r3d.h"

#include "CkHttpRequest.h"
#include "CkHttp.h"
#include "CkHttpResponse.h"
#include "CkByteData.h"

#include "HWInfo.h"
#include "HWInfoPost.h"
#include "WOBackendAPI.h"

	CHWInfoPoster	gHwInfoPoster;

static unsigned int __stdcall PostHWInfoThread(void* in_data)
{
	const CHWInfo& hw = *(CHWInfo*)in_data;

	CWOBackendReq req("api_ReportHWInfo.aspx");
	req.AddSessionInfo(hw.CustomerID, hw.SessionID);

	char buf[1024];
	sprintf(buf, "%I64d", hw.uniqueId);
	req.AddParam("r00", buf);
	req.AddParam("r10", hw.CPUString);
	req.AddParam("r11", hw.CPUBrandString);
	req.AddParam("r12", hw.CPUFreq);
	req.AddParam("r13", hw.TotalMemory);
	
	req.AddParam("r20", hw.DisplayW);
	req.AddParam("r21", hw.DisplayH);
	req.AddParam("r22", hw.gfxErrors);
	req.AddParam("r23", hw.gfxVendorId);
	req.AddParam("r24", hw.gfxDeviceId);
	req.AddParam("r25", hw.gfxDescription);
	req.AddParam("r26", hw.gfxD3DVersion);

	req.AddParam("r30", hw.OSVersion);
	
	if(!req.Issue()) {
		return 0;
	}
		
	// mark that we reported it
	HKEY hKey;
	int hr;
	hr = RegCreateKeyEx(HKEY_CURRENT_USER, 
		"Software\\Arktos Entertainment Group\\WarZ", 
		0, 
		NULL,
		REG_OPTION_NON_VOLATILE, 
		KEY_ALL_ACCESS,
		NULL,
		&hKey,
		NULL);
	if(hr == ERROR_SUCCESS)
	{
		__int64 repTime = _time64(NULL);
		DWORD size = sizeof(repTime);
		
		hr = RegSetValueEx(hKey, "UpdaterTime1", NULL, REG_QWORD, (BYTE*)&repTime, size);
		RegCloseKey(hKey);
	}
	
	return 1;
}

CHWInfoPoster::CHWInfoPoster()
{
	postHwInfoH_ = NULL;
}

CHWInfoPoster::~CHWInfoPoster()
{
	Stop();
}

bool CHWInfoPoster::NeedUploadReport()
{
	// query for game registry node
	HKEY hKey;
	int hr;
	hr = RegOpenKeyEx(HKEY_CURRENT_USER, 
		"Software\\Arktos Entertainment Group\\WarZ", 
		0, 
		KEY_ALL_ACCESS, 
		&hKey);
	if(hr != ERROR_SUCCESS)
		return true;

	__int64 repTime = 0;
	DWORD size = sizeof(repTime);
	hr = RegQueryValueEx(hKey, "UpdaterTime1", NULL, NULL, (BYTE*)&repTime, &size);
	RegCloseKey(hKey);

	if(hr == ERROR_SUCCESS)
	{
		__int64 time = _time64(NULL);
		// report every 2 days
		if(time < repTime + (60 * 60 * 24 * 2))
			return false;
	}
	
	return true;
}

void CHWInfoPoster::Start()
{
	if(hw.uniqueId == 0)
		return;
	
	if(!NeedUploadReport())
		return;

	postHwInfoH_ = (HANDLE)_beginthreadex(NULL, 0, &PostHWInfoThread, &hw, 0, NULL);
}

void CHWInfoPoster::Stop()
{
	if(postHwInfoH_ == NULL)
		return;

	// give it like 10 sec to stop. if not finished - abort
	int hr = ::WaitForSingleObject(postHwInfoH_, 10000);
	if(hr == WAIT_TIMEOUT)
		TerminateThread(postHwInfoH_, 0);

	CloseHandle(postHwInfoH_);
	postHwInfoH_ = NULL;
}