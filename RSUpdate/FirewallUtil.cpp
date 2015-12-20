#include "r3dPCH.h"
#include "r3d.h"

#include "oleauto.h"

#include "FirewallUtil.h"

/*
fuck OOP.
template<class T> class r3dIUnknown
{
  private:
	T	t_;
  public:
	r3dIUnknown();
	~r3dIUnknown() {
	  SAFE_RELEASE(t_);
	}
	T* ptr() { return &t_; }
	T operator ->() { return t_; }
};
*/

BSTR SysAllocStringAnsi(const char* str)
{
  wchar_t wstr[2048];
  MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, str, -1, wstr, 2048);
  BSTR bstr = SysAllocString(wstr);

  if(bstr == NULL) {
    r3dError("SysAllocString failed\n");
  }
  return bstr;
}


CFirewallUtilXP::CFirewallUtilXP()
{
  m_pFwMgr = NULL;
  m_pFwPolicy = NULL;
  m_pFwProfile = NULL;
}

CFirewallUtilXP::~CFirewallUtilXP()
{
  Uninitialize();
}

HRESULT CFirewallUtilXP::Initialize()
{
  HRESULT hr;

  // Initialize COM.
  hr = CoInitializeEx(0, COINIT_APARTMENTTHREADED);

  // Ignore RPC_E_CHANGED_MODE; this just means that COM has already been
  // initialized with a different mode. Since we don't care what the mode is,
  // we'll just use the existing mode.
  if(FAILED(hr) && hr != RPC_E_CHANGED_MODE)
    return hr;

  // Create an instance of the firewall settings manager.
  hr = CoCreateInstance(
    __uuidof(NetFwMgr),
    NULL,
    CLSCTX_INPROC_SERVER,
    __uuidof(INetFwMgr),
    (void**)&m_pFwMgr
    );
  if(FAILED(hr)) {
    r3dOutToLog("CoCreateInstance NetFwMgr failed: 0x%08lx\n", hr);
    return hr;
  }

  // Retrieve the local firewall policy.
  hr = m_pFwMgr->get_LocalPolicy(&m_pFwPolicy);
  if(FAILED(hr)) {
    r3dOutToLog("get_LocalPolicy failed: 0x%08lx\n", hr);
    return hr;
  }

  // Retrieve the firewall profile currently in effect.
  hr = m_pFwPolicy->get_CurrentProfile(&m_pFwProfile);
  if(FAILED(hr)) {
    r3dOutToLog("get_CurrentProfile failed: 0x%08lx\n", hr);
    return hr;
  }

  return hr;
}

void CFirewallUtilXP::Uninitialize()
{
  // Release the firewall profile.
  SAFE_RELEASE(m_pFwProfile);
  SAFE_RELEASE(m_pFwPolicy);
  SAFE_RELEASE(m_pFwMgr);

  //CoUninitialize();
}

HRESULT CFirewallUtilXP::IsFirewallEnabled(BOOL * pEnabled)
{
  r3d_assert(m_pFwProfile);
  
  HRESULT hr = S_OK;
  *pEnabled = FALSE;

  // Get the current state of the firewall.
  VARIANT_BOOL fwEnabled;
  hr = m_pFwProfile->get_FirewallEnabled(&fwEnabled);
  if(SUCCEEDED(hr)) {
    *pEnabled = fwEnabled != VARIANT_FALSE;
  }

  return hr;
}

HRESULT CFirewallUtilXP::IsAppEnabledByExe(const char* exe, OUT BOOL* fwAppEnabled)
{
  if(m_pFwProfile == NULL)
    return E_NOTIMPL;
  r3d_assert(m_pFwProfile);
	
  HRESULT hr = S_OK;
  VARIANT_BOOL fwEnabled;
  INetFwAuthorizedApplication* fwApp = NULL;
  INetFwAuthorizedApplications* fwApps = NULL;
  BSTR bstrExe = SysAllocStringAnsi(exe);

  *fwAppEnabled = FALSE;

  // Retrieve the authorized application collection.
  hr = m_pFwProfile->get_AuthorizedApplications(&fwApps);
  if(FAILED(hr)) {
    r3dOutToLog("get_AuthorizedApplications failed: 0x%08lx\n", hr);
    goto error;
  }

  // Attempt to retrieve the authorized application.
  hr = fwApps->Item(bstrExe, &fwApp);
  if(SUCCEEDED(hr))
  {
    // Find out if the authorized application is enabled.
    NET_FW_IP_VERSION ipVersion;
    NET_FW_SCOPE scope;
    BSTR addr;
    fwApp->get_IpVersion(&ipVersion);
    fwApp->get_Scope(&scope);
    fwApp->get_RemoteAddresses(&addr);

    hr = fwApp->get_Enabled(&fwEnabled);
    if(FAILED(hr)) {
      r3dOutToLog("get_Enabled failed: 0x%08lx\n", hr);
      goto error;
    }

    if(fwEnabled != VARIANT_FALSE) {
      // The authorized application is enabled.
      *fwAppEnabled = TRUE;
    }
  }
  else
  {
    // The authorized application was not in the collection.
    hr = S_OK;
  }

error:

  // Free the BSTR.
  SysFreeString(bstrExe);

  // Release the authorized application instance.
  SAFE_RELEASE(fwApp);
  // Release the authorized application collection.
  SAFE_RELEASE(fwApps);

  return hr;
}


HRESULT CFirewallUtilXP::AddApp(const char* exe, const char* fwName)
{
  if(m_pFwProfile == NULL)
    return E_NOTIMPL;
  r3d_assert(m_pFwProfile);

  HRESULT hr = S_OK;
  INetFwAuthorizedApplication* fwApp = NULL;
  INetFwAuthorizedApplications* fwApps = NULL;
  
  BSTR bstrName = SysAllocStringAnsi(fwName);
  BSTR bstrExe = SysAllocStringAnsi(exe);

  // First check to see if the application is already authorized.
  BOOL fwAppEnabled;
  hr = IsAppEnabledByExe(exe, &fwAppEnabled);
  if(FAILED(hr)) {
    r3dOutToLog("IsAppEnabled failed: 0x%08lx\n", hr);
    return hr;
  }
  if(fwAppEnabled)
    return S_OK;

  // Retrieve the authorized application collection.
  hr = m_pFwProfile->get_AuthorizedApplications(&fwApps);
  if(FAILED(hr)) {
    r3dOutToLog("get_AuthorizedApplications failed: 0x%08lx\n", hr);
    goto error;
  }

  // Create an instance of an authorized application.
  hr = CoCreateInstance(
    __uuidof(NetFwAuthorizedApplication),
    NULL,
    CLSCTX_INPROC_SERVER,
    __uuidof(INetFwAuthorizedApplication),
    (void**)&fwApp
    );
  if(FAILED(hr)) {
    r3dOutToLog("CoCreateInstance NetFwAuthorizedApplication failed: 0x%08lx\n", hr);
    goto error;
  }

  // Set the process image file name.
  hr = fwApp->put_ProcessImageFileName(bstrExe);
  if(FAILED(hr)) {
    r3dOutToLog("put_ProcessImageFileName failed: 0x%08lx\n", hr);
    goto error;
  }

  // Set the application friendly name.
  hr = fwApp->put_Name(bstrName);
  if(FAILED(hr)) {
    r3dOutToLog("put_Name failed: 0x%08lx\n", hr);
    goto error;
  }

  hr = fwApp->put_Scope(NET_FW_SCOPE_ALL);
  if(FAILED(hr)) {
    r3dOutToLog("put_Scope failed: 0x%08lx\n", hr);
    goto error;
  }
  //hr = fwApp->put_RemoteAddresses(SysAllocStringAnsi("*"));

  // Add the application to the collection.
  hr = fwApps->Add(fwApp);
  if(FAILED(hr)) {
    r3dOutToLog("Add failed: 0x%08lx\n", hr);
    goto error;
  }

  // "Authorized application %S is now enabled in the firewall\n"

error:

  // Free the BSTRs.
  SysFreeString(bstrName);
  SysFreeString(bstrExe);

    // Release the authorized application instance.
  SAFE_RELEASE(fwApp);
  SAFE_RELEASE(fwApps);

  return hr;
}


//
//
//
//
//////////////////////////////////////////////////////////////////////////

CFirewallUtilVista::CFirewallUtilVista()
{
  m_pFwPolicy = NULL;
  m_pFwRules = NULL;
}

CFirewallUtilVista::~CFirewallUtilVista()
{
  Uninitialize();
}

HRESULT CFirewallUtilVista::Initialize()
{
  HRESULT hr;

  // Initialize COM.
  hr = CoInitializeEx(0, COINIT_APARTMENTTHREADED);

  // Ignore RPC_E_CHANGED_MODE; this just means that COM has already been
  // initialized with a different mode. Since we don't care what the mode is,
  // we'll just use the existing mode.
  if(FAILED(hr) && hr != RPC_E_CHANGED_MODE)
    return hr;
    
  hr = CoCreateInstance(
    __uuidof(NetFwPolicy2), 
    NULL, 
    CLSCTX_INPROC_SERVER, 
    __uuidof(INetFwPolicy2), 
    (void**)&m_pFwPolicy);
  if(FAILED(hr)) {
    r3dOutToLog("CoCreateInstance NetFwPolicy2 failed: 0x%08lx\n", hr);
    return hr;
  }

  hr = m_pFwPolicy->get_Rules(&m_pFwRules);
  if(FAILED(hr)) {
    r3dOutToLog("get_Rules failed: 0x%08lx\n", hr);
    return hr;
  }

  return hr;
}

void CFirewallUtilVista::Uninitialize()
{
  // Release the firewall profile.
  SAFE_RELEASE(m_pFwRules);
  SAFE_RELEASE(m_pFwPolicy);

  //CoUninitialize();
}

HRESULT CFirewallUtilVista::IsFirewallEnabled(BOOL * pEnabled)
{
  r3d_assert(m_pFwPolicy);
  
  *pEnabled = FALSE;

  // Get the current state of the firewall.
  VARIANT_BOOL fwEnabled1;
  VARIANT_BOOL fwEnabled2;
  HRESULT hr1 = m_pFwPolicy->get_FirewallEnabled(NET_FW_PROFILE2_PUBLIC, &fwEnabled1);
  HRESULT hr2 = m_pFwPolicy->get_FirewallEnabled(NET_FW_PROFILE2_PRIVATE, &fwEnabled2);
  if(SUCCEEDED(hr1) && SUCCEEDED(hr2)) {
    *pEnabled = (fwEnabled1 != VARIANT_FALSE) && (fwEnabled2 != VARIANT_FALSE);
  }

  return S_OK;
}

HRESULT CFirewallUtilVista::IsAppEnabledByName(const char* name, OUT BOOL* fwAppEnabled)
{
  r3d_assert(m_pFwRules);

  HRESULT hr = S_OK;
  INetFwRule* pFwRule = NULL;
  BSTR bstrName = SysAllocStringAnsi(name);

  *fwAppEnabled = FALSE;

  hr = m_pFwRules->Item(bstrName, &pFwRule);
  if(SUCCEEDED(hr))
  {
    *fwAppEnabled = TRUE;
  }
  else
  {
    // The authorized application was not in the collection.
    hr = S_OK;
  }

//error:

  // Free the BSTR.
  SysFreeString(bstrName);

  // Release the authorized application instance.
  SAFE_RELEASE(pFwRule);

  return hr;
}

HRESULT CFirewallUtilVista::RemoveApp(const char* name)
{
  r3d_assert(m_pFwRules);

  HRESULT hr = S_OK;
  BSTR bstrName = SysAllocStringAnsi(name);

  //NOTE: by some weird reasons it does not work at all
  hr = m_pFwRules->Remove(bstrName);

//error:

  // Free the BSTR.
  SysFreeString(bstrName);

  return hr;
}

HRESULT CFirewallUtilVista::AddApp(const char* exe, const char* name, const char* description)
{
  if(m_pFwRules == NULL)
    return E_NOTIMPL;
  r3d_assert(m_pFwRules);

  HRESULT hr = S_OK;
  BSTR bstrName = SysAllocStringAnsi(name);
  BSTR bstrExe  = SysAllocStringAnsi(exe);
  BSTR bstrDescription = SysAllocStringAnsi(description);
  INetFwRule* pFwRule = NULL;

  // First check to see if the application is already authorized.
  BOOL fwAppEnabled;
  hr = IsAppEnabledByName(name, &fwAppEnabled);
  if(FAILED(hr)) {
    r3dOutToLog("IsAppEnabled failed: 0x%08lx\n", hr);
    return hr;
  }
  if(fwAppEnabled)
    return S_OK;

  // Create a new Firewall Rule object.
  hr = CoCreateInstance(
    __uuidof(NetFwRule),
    NULL,
    CLSCTX_INPROC_SERVER,
    __uuidof(INetFwRule),
    (void**)&pFwRule);
  if(FAILED(hr)) {
    r3dOutToLog("CoCreateInstance NetFwRule failed: 0x%08lx\n", hr);
    goto error;
  }

  // Populate the Firewall Rule object
  pFwRule->put_Name(bstrName);
  pFwRule->put_Description(bstrDescription);
  pFwRule->put_ApplicationName(bstrExe);
  pFwRule->put_Protocol(NET_FW_IP_PROTOCOL_ANY);
  //pFwRule->put_LocalPorts(bstrRuleLPorts);
  //pFwRule->put_Direction(NET_FW_RULE_DIR_OUT);
  //pFwRule->put_Grouping(bstrRuleGroup);
  pFwRule->put_Profiles(NET_FW_PROFILE2_PRIVATE | NET_FW_PROFILE2_PUBLIC);
  pFwRule->put_Action(NET_FW_ACTION_ALLOW);
  pFwRule->put_Enabled(VARIANT_TRUE);

  // Add the Firewall Rule
  hr = m_pFwRules->Add(pFwRule);
  if(FAILED(hr)) {
    r3dOutToLog("Firewall Rule Add failed: 0x%08lx\n", hr);
    goto error;
  }

  // "Authorized application %S is now enabled in the firewall\n"

error:

  // Free the BSTRs.
  SysFreeString(bstrName);
  SysFreeString(bstrExe);
  SysFreeString(bstrDescription);

  // Release the authorized application instance.
  SAFE_RELEASE(pFwRule);

  return hr;
}


bool CFirewallUtil::AddApp(const char* exe, const char* name, const char* description)
{
  r3dOutToLog("Firewall: adding %s (%s)\n", name, exe);
  CLOG_INDENT;

  // exe must be a fully qualified name
  r3d_assert(strchr(exe, '\\') != NULL);
  if(_access(exe, 0) != 0) {
    r3dOutToLog("Firewall: can't find %s\n", exe);
    return false;
  }

  HRESULT hr;
  //BOOL fwEnabled;

  // check for vista firewall
  CFirewallUtilVista fw;
  if(SUCCEEDED(fw.Initialize())) 
  {
    r3dOutToLog("Vista mode\n");
    hr = fw.AddApp(exe, name, description);
  } 
  else 
  {
    r3dOutToLog("XP mode\n");

    CFirewallUtilXP xp;
    hr = xp.Initialize();
    hr = xp.AddApp(exe, name);
  }

  if(FAILED(hr)) {
    r3dOutToLog("Firewall: error adding %s %x\n", name, hr);
    return false;
  }

  r3dOutToLog("Firewall: %s added\n", name);
  return true;
}