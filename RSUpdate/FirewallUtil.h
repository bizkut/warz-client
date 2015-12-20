#pragma once

#include "Netfw.h"

class CFirewallUtil
{
  public:
static	bool	AddApp(const char* exe, const char* name, const char* description);
};

class CFirewallUtilXP
{
  private:
	INetFwMgr* m_pFwMgr;
	INetFwPolicy* m_pFwPolicy;
	INetFwProfile* m_pFwProfile;
	
  public:
	CFirewallUtilXP();
	~CFirewallUtilXP();

	HRESULT Initialize();
	void Uninitialize();

	HRESULT IsFirewallEnabled(BOOL* pEnabled);

	HRESULT	IsAppEnabledByExe(const char* exe, OUT BOOL* fwAppEnabled);
	HRESULT AddApp(const char* exe, const char* fwName);
};

class CFirewallUtilVista
{
  private:
	INetFwPolicy2* m_pFwPolicy;
	INetFwRules*   m_pFwRules;
	
  public:
	CFirewallUtilVista();
	~CFirewallUtilVista();

	HRESULT Initialize();
	void Uninitialize();

	HRESULT IsFirewallEnabled(BOOL* pEnabled);

	HRESULT	IsAppEnabledByName(const char* name, OUT BOOL* fwAppEnabled);
	HRESULT	RemoveApp(const char* name);
	HRESULT AddApp(const char* exe, const char* name, const char* description);
};
