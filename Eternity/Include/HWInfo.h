#pragma once

class CHWInfo
{
  public:
	// copy of login info
	DWORD		CustomerID;
	DWORD		SessionID;
  
	BYTE		macAddress[8]; // MAX_ADAPTER_ADDRESS_LENGTH
	__int64		uniqueId;
	
	char		CPUString[0x20];
	char		CPUBrandString[0x40];
	int		CPUFeatures[4];
	DWORD		CPUFreq;
	DWORD		TotalMemory;
	
	DWORD		DisplayW;
	DWORD		DisplayH;
	int		gfxErrors;	// bitwise error flags
	DWORD		gfxVendorId;
	DWORD		gfxDeviceId;
	char		gfxDescription[256];
	DWORD		gfxD3DVersion;
	
	char		OSVersion[32];

	void		GetMACAddress();
	void		GetCPUInfo();
	void		GetCPUFreq();
	void		GetMemoryInfo();
	void		GetDesktopResolution();
	void		GetD3DInfo();
	void		 CheckIfCapsOk(const D3DCAPS9& caps);
	void		GetD3DVersion();
	void		GetOSInfo();

  public:
	CHWInfo();
	
	void		Grab();
};