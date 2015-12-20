// Plugin.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#include "../Common/Log.h"
#include "../Common/PluginInterface.h"
#include "../Common/Config.h"
#include "exporter.h"

	expParams	expp;

class CFlagVar
{
  public:
	DWORD		var;
	char		*name;
  public:
	CFlagVar(char *_name, int _var)
	{
	  name = _name;
	  var  = _var;
	};
};

	CFlagVar	vExportSelected("ExportSelected", 0);
	CFlagVar	vShowConfig("ShowConfig", 0);
static	CFlagVar	*_varList[] = {&vExportSelected, &vShowConfig};

static void ReadConfig(Interface *ip)
{
  char buf[512];
  sprintf(buf, "%s\\TSG_Exporter.cfg", ip->GetDir(APP_PLUGCFG_DIR));
  FILE *f;
  if((f=fopen(buf, "rt")) == NULL)
    return;

  while(!feof(f)) {
    if(fgets(buf, sizeof(buf), f) == NULL) break;

    char  vname[512];
    DWORD var;
    sscanf(buf, "%s %d", vname, &var);
    
    for(int i=0; i<sizeof(_varList)/sizeof(_varList[0]); i++) {
      if(_stricmp(_varList[i]->name, vname) == NULL) {
        _varList[i]->var = var;
        break;
      }
    }
  }

  fclose(f);
  return;
}

static void WriteConfig(Interface *ip)
{
  char buf[512];
  sprintf(buf, "%s\\TSG_Exporter.cfg", ip->GetDir(APP_PLUGCFG_DIR));
  FILE *f;
  if((f=fopen(buf, "wt")) == NULL) {
    U_DLog("!!! WriteConfig failed to open %s, %d\n", buf, _errno);
    return;
  }

  for(int i=0; i<sizeof(_varList)/sizeof(_varList[0]); i++) {
    fprintf(f, "%s %d\n", _varList[i]->name, _varList[i]->var);
  }

  fclose(f);
  return;
}
	

#include "MaxExportAll.h"
CMaxExportAll theAllExport;
#include "MaxSkeletonExport.h"
CMaxSkeletonExport theSkeletonExport;
#include "MaxAnimationExport.h"
CMaxAnimationExport theAnimationExport;
#include "MaxMeshExport.h"
CMaxMeshExport theMeshExport;

void plgExportScene()
{
  theExporter.ExportAll(expp.exportFile);
  return;
}

void plgExportAnimation()
{
  theExporter.ExportSkeleton(expp.exportFile);
  return;
}

void plgExportMesh()
{
  theExporter.ExportMesh(expp.exportFile);
  return;
}

void plgExportSkeleton()
{
  theExporter.ExportSkeleton(expp.exportFile);
  return;
}

void plgExportSetConfig()
{
  theExporter.SetDefaultConfig();
  return;
}

void plgExportBoneAnim()
{
/*
  if(0)
  {
    U_Log("!B!rLoading new biped animation\n");
    CLOG_INDENT;

    // end figure mode (if active) and load new .bip
    if(theSkeleton.pBipMaster && theSkeleton.pBipExport) 
    {
      if(theSkeleton.pBipMaster->GetActiveModes() & BMODE_FIGURE)
        theSkeleton.pBipExport->EndFigureMode(TRUE);

      theSkeleton.pBipMaster->LoadBipFileDlg();
    }

    theSkeleton.AdjustAnimationRange();
    U_Log("!dAnimation frames: %d to %d\n", theExporter.m_iStartFrame, theExporter.m_iEndFrame);

    OPENFILENAME ofn;
    char buf[MAX_PATH] = "";

    extern char* _dscAni;

    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize     = sizeof(OPENFILENAME);
    ofn.hwndOwner       = expp.hWnd;
    ofn.lpstrFilter     = _dscAni + 4;
    ofn.lpstrInitialDir = expp.exportFile;
    ofn.lpstrFile       = buf;
    ofn.nMaxFile        = sizeof(buf)/sizeof(buf[0]);
    ofn.lpstrDefExt     = _dscAni;
    ofn.Flags           = OFN_EXPLORER | OFN_LONGNAMES | OFN_OVERWRITEPROMPT;
    if(GetSaveFileName(&ofn) == 0) 
      return;
    strcpy(expp.exportFile, buf);
  }
*/  

  theExporter.ExportBoneAnimation(expp.exportFile);
  return;
}

void plgVertexCount()
{
  theExporter.ShowVertexCount();
}

void U_DumpParamBlock(IParamBlock2 *pbl)
{
  U_Log("PBlock2: %s [%d]\n", pbl->GetLocalName(), pbl->NumParams());
  CLOG_INDENT;
  for(int i=0; i<pbl->NumParams(); i++) {
    ParamDef &pdef = pbl->GetParamDef(i);
    
    char value[512] = "<type?>";
    switch(pdef.type) {
      case TYPE_FLOAT:		{float v;	pbl->GetValue(pdef.ID, 0, v, FOREVER);	sprintf(value, "%.2f", v);		break;}
      case TYPE_INT:		{int   v;	pbl->GetValue(pdef.ID, 0, v, FOREVER);	sprintf(value, "%d", v);		break;}
      case TYPE_BOOL:		{int   v;	pbl->GetValue(pdef.ID, 0, v, FOREVER);	sprintf(value, "%s", v ? "TRUE" : "FALSE");	break;}
      case TYPE_STRING:		{const MCHAR* v; pbl->GetValue(pdef.ID, 0, v, FOREVER);	sprintf(value, "%s", v);		break;}
      case TYPE_FILENAME:	{const MCHAR* v; pbl->GetValue(pdef.ID, 0, v, FOREVER);	sprintf(value, "%s", v);		break;}
      case TYPE_BITMAP: {
        PBBitmap* v;	
        pbl->GetValue(pdef.ID, 0, v, FOREVER);	
        if(!v) {
          sprintf(value, "no bitmap loaded", v);
          break;
        }
        sprintf(value, "%s", v->bi.Filename());	
        break;
      }
      default:
        sprintf(value, "<unknown_type>");
        break;
    }
    
    U_Log("%2d: Type: %2d %-30s\t %s\n", pdef.ID, pdef.type, pdef.int_name, value);
  }
  
  return;
}

void test_DumpObjInfo(INode *pNode)
{
  U_Log("!B!rDumpObjInfo: %s\n", pNode->GetName());
  CLOG_INDENT;

  U_Log("Node Class_ID(0x%x, 0x%x), SuperClass_ID(0x%x)\n", 
	pNode->ClassID().PartA(), pNode->ClassID().PartB(),
	pNode->SuperClassID());

  ObjectState os = pNode->EvalWorldState(_ip->GetTime());
  U_Log("Object Class_ID(0x%x, 0x%x), SuperClass_ID(0x%x)\n", 
	os.obj->ClassID().PartA(), os.obj->ClassID().PartB(),
	os.obj->SuperClassID());
	

  Control *pControl;
  pControl = pNode->GetTMController();
  U_Log("Controller Class_ID(0x%x, 0x%x)\n", pControl->ClassID().PartA(), pControl->ClassID().PartB());

  // get the object reference of the node
  Object *pObject;
  pObject = pNode->GetObjectRef();
  if(pObject == 0) {
    U_Log("No Object reference\n");
    return;
  }

  // loop through all derived objects
  while(pObject->SuperClassID() == GEN_DERIVOB_CLASS_ID)
  {
    IDerivedObject *pDerivedObject;
    pDerivedObject = static_cast<IDerivedObject *>(pObject);
    U_Log("DerivedObject: %s\n", pDerivedObject->GetObjectName());
    CLOG_INDENT;

    // loop through all modifiers
    int stackId;
    for(stackId = 0; stackId < pDerivedObject->NumModifiers(); stackId++)
    {
      // get the modifier
      Modifier *pMod;
      pMod = pDerivedObject->GetModifier(stackId);

      U_Log("Modifier: %s, Class_ID(0x%x, 0x%x), SuperClass_ID(0x%x)\n", 
        pMod->GetName(), 
	pMod->ClassID().PartA(), pMod->ClassID().PartB(),
	pMod->SuperClassID());
      CLOG_INDENT;

      IParamBlock2 *pbl = pMod->GetParamBlockByID(0);
      if(!pbl) 
        continue;
      U_DumpParamBlock(pbl);
    }

    // continue with next derived object
    pObject = pDerivedObject->GetObjRef();
  }

  return;
}

// usage: write .txt file with skeleton hierarchy
// call: exportSkeletonHierarchy(theSkeleton.m_vBones[0], 0);
#include "BoneCandidate.h"
void exportSkeletonHierarchy(CBone *pBone, int depth)
{
  for(int i=0; i<depth; i++) U_Log("  ");
  U_Log("%s\n", pBone->GetName());

  std::list<int>& listChildId = pBone->GetListChildId();
  std::list<int>::iterator iteratorChildId;
  for(iteratorChildId = listChildId.begin(); iteratorChildId != listChildId.end(); ++iteratorChildId) {
    exportSkeletonHierarchy(theSkeleton.m_vBones[*iteratorChildId], depth+1);
  }
  return;
}

void _WalkAllNodes(INode *pNode)
{
  if(pNode == NULL) return;
/*
  if(gMaxHelper.IsMesh(pNode)) 
  {
     U_Log("Mesh: %s\n", pNode->GetName());
     CLOG_INDENT;
     CMaxMaterial::AddMaterial(pNode->GetMtl());
  }
*/

  for(int i = 0; i < pNode->NumberOfChildren(); i++) 
    _WalkAllNodes(pNode->GetChildNode(i));
}

void plgTest()
{
  U_Log("Test\n");
  CLOG_INDENT;

  //_WalkAllNodes(_ip->GetRootNode());
  //ee.ExportMaterialList(expp.exportFile);
  
  if(_ip->GetSelNodeCount() == 0) 
    ThrowError("You must select object to export");

  INode *pNode = _ip->GetSelNode(0);
  pNode->EvalWorldState(_ip->GetTime());
  //test_DumpObjInfo(pNode);

  Object *obj = pNode->EvalWorldState(_ip->GetTime()).obj;
  U_Log("Object Class_ID(0x%x, 0x%x), SuperClass_ID(0x%x)\n", 
	obj->ClassID().PartA(), obj->ClassID().PartB(),
	obj->SuperClassID());

  if(obj->SuperClassID() != SHAPE_CLASS_ID) {
    U_Log("not a shape\n");
    return;
  }

  ShapeObject *so = (ShapeObject *)obj;
  if(!so->CanMakeBezier()) {
    U_Log("!so->CanMakeBezier()\n");
    return;
  }

  BezierShape bs;
  so->MakeBezier(_ip->GetTime(), bs);

  int polys = bs.SplineCount();
  U_Log("Splines: %d\n", polys);
  for(int poly = 0; poly < polys; ++poly) {
    CLOG_INDENT;
    Spline3D *sp = bs.GetSpline(poly);

    U_Log("KnotCount: %d\n", sp->KnotCount());
    {
      CLOG_INDENT;
      for(int j=0; j<sp->KnotCount(); j++) {
        SplineKnot k = sp->GetKnot(j);
        
        U_Log("ktype=%d\n", k.Ktype());
        U_Log("ltype=%d\n", k.Ltype());
        U_Log("point=%f %f %f\n", k.Knot().x, k.Knot().y, k.Knot().z);
        U_Log("invec=%f %f %f\n", k.InVec().x, k.InVec().y, k.InVec().z);
        U_Log("outvec=%f %f %f\n", k.OutVec().x, k.OutVec().y, k.OutVec().z);
      }
    }
    U_Log("Flags: %d\n", sp->Flags());
    U_Log("Segments: %d\n", sp->Segments());
    U_Log("Closed: %d\n", sp->Closed());
    U_Log("Verts: %d\n", sp->Verts());
  }
  
  return;
}

//
//
//
struct expfn_s
{
	int		type;
	char*		name;
	void		(*fn)();
	char*		prm1;
};

static	char*		_dscSce   = "dsc\0TSGroup Scene (*.DSC)\0*.dsc;\0\0";
	char*		_dscAni   = "anm\0TSGroup Animation (*.ANM)\0*.anm;\0\0";
static	char*		_dscMes   = "sco\0TSGroup Mesh (*.SCO)\0*.sco;\0\0";
static	char*		_dscSke   = "skl\0TSGroup Skeleton (*.SKL)\0*.skl;\0\0";
static	char*		_dscMat   = "mat\0TSGroup Material List (*.MAT)\0*.mat;\0\0";

#define T_BTN	MAXUTIL_PLUGIN_TYPE_BUTTON
#define T_EXP	MAXUTIL_PLUGIN_TYPE_EXPORTER
#define T_CB	MAXUTIL_PLUGIN_TYPE_CHECKBOX
#define T_DL	MAXUTIL_PLUGIN_TYPE_DELIMITER
static	expfn_s		expList[] = {
	  //{ T_CB,  "Export Selected",		NULL,			(char *)&vExportSelected.var },
	  //{ T_CB,  "Show Config",		NULL,			(char *)&vShowConfig.var },
	  { T_EXP, "Export Scene",		&plgExportScene,	"!"},
	  //{ T_EXP, "Export Animation",	&plgExportAnimation,	_dscAni },
	  { T_DL,  "", NULL, NULL},
	  { T_EXP, ">> Mesh",			&plgExportMesh,		_dscMes},
	  { T_EXP, ">> Skeleton",		&plgExportSkeleton,	_dscSke},
	  { T_EXP, ">> Biped Anim",		&plgExportBoneAnim,	_dscAni},
	  { T_BTN, "Vertex Count",		&plgVertexCount,	""},
	  { T_DL,  "", NULL, NULL},
	  { T_BTN, "--test--",			&plgTest,		""},
	  //{ T_BTN, "(set default config)",	&plgExportSetConfig,    ""},
	  { T_DL,  "", NULL, NULL},
	};

__declspec(dllexport) DWORD expGetVersion()
{
  return MAXUTIL_PLUGIN_VERSION;
}

__declspec(dllexport) DWORD expInit(Interface *ip)
{
  ReadConfig(ip);
  return 1;
}

__declspec(dllexport) int expGetFuncCount()
{
  return sizeof(expList)/sizeof(expList[0]);
}

__declspec(dllexport) TCHAR* expGetFuncName(int id)
{  
  if(id < sizeof(expList)/sizeof(expList[0]))
    return expList[id].name;

  return _T("--illegal--");
}

__declspec(dllexport) DWORD expGetFuncType(int id, char *prm1, char *prm2, char *prm3)
{
  if(id >= sizeof(expList)/sizeof(expList[0])) 
    return 0;

  if(expList[id].type == T_CB) {
    *prm1 = *(char*)expList[id].prm1 + '0';
    return MAXUTIL_PLUGIN_TYPE_CHECKBOX;
  }
  if(expList[id].type == T_DL) {
    return MAXUTIL_PLUGIN_TYPE_DELIMITER;
  }
  if(expList[id].type == T_BTN) {
    return MAXUTIL_PLUGIN_TYPE_BUTTON;
  }
  if(expList[id].type == T_EXP) 
  {
    if(expList[id].prm1[0] != 0)
    {
      // find end of win32 filetype description
      int len1;
      for(len1=0; len1<999; len1++) if(expList[id].prm1[len1] == 0 && expList[id].prm1[len1+1] == 0) break;
      memcpy(prm1, expList[id].prm1, len1 + 2);
    }
    return MAXUTIL_PLUGIN_TYPE_EXPORTER;
  }

  return 0;
}

int handle_FP_Call(expParams *exp_prm)
{
  Tab<char *>&prm = *exp_prm->FP_args;
  
  struct opt_s
  {
    const char* name;
    int         type;
    void*       var;
  };
  opt_s opts[] = {
    {"CopyTex",         0, &theExporter.m_ExpCopyTex },
    {"ConvertToDDS",    0, &theExporter.m_ExpConvertToDDS },
    {"nvdxtPath",       1, theExporter.m_nvdxtPath },
    {"nvdxtParams",     1, theExporter.m_nvdxtParams },
    {"ExportMats",      0, &theExporter.m_ExpSaveMats },
    {"ExportVtxColors", 0, &theExporter.m_ExpVtxColors },
    {"NormalTolerance", 0, &theExporter.m_NormalTolerance },
  };

  // helper to show all available options
  // TSGroup_Tools.UtilMgr.RunCommand "ShowOptions" #()
  if(_stricmp(exp_prm->FP_cmd, "ShowOptions") == NULL) 
  {
    mprintf("Current Options:\n");
    for(int k=0; k<_ARRAYSIZE(opts); k++)
    {
      if(opts[k].type == 0)
        mprintf(" %-15s\tint   \tcurrent:%d\n", opts[k].name, *(int*)opts[k].var);
      else 
        mprintf(" %-15s\tstring\tcurrent:\"%s\"\n", opts[k].name, (char*)opts[k].var);
    }
    return 1;
  }

  // first set of parameters is "OutFile xxx"
  for(int i=2; i<prm.Count(); i += 2) 
  {
    bool found = false;
    for(int k=0; k<_ARRAYSIZE(opts); k++) 
    {
      if(_stricmp(prm[i], opts[k].name) == 0) 
      {
        found = true;
        U_DLog("option %s changed to %s\n", opts[k].name, prm[i+1]);
        if(opts[k].type == 0)
        {
          *(int*)opts[k].var = atoi(prm[i+1]);
        }
        else
        {
          strcpy((char*)opts[k].var, prm[i+1]);
        }
        break;
      }
    }

    if(!found)
    {
      char buf[512];
      sprintf(buf, "Invalid parameter: \"%s\"\n", prm[i]);
      MessageBox(NULL, buf, EXPORTER_MSG_HEADER, MB_OK | MB_ICONEXCLAMATION);
      return -1;
    }
  }
  
  if(prm.Count() < 2)
  {
    char buf[512];
    sprintf(buf, "first set pf parameters must be OutFile xxx");
    MessageBox(NULL, buf, EXPORTER_MSG_HEADER, MB_OK | MB_ICONEXCLAMATION);
    return -2;
  }

  char *exportFile = prm[1];
  U_DLog("exportFile: %s\n", exportFile);

  char curDir[MAX_PATH];
  GetCurrentDirectory(sizeof(curDir), curDir);
  char exportPath[MAX_PATH];
  strcpy(exportPath, exportFile);
  if(strrchr(exportPath, '\\')) *strrchr(exportPath, '\\') = 0;
  SetCurrentDirectory(exportPath);

  theExporter.m_bBatchExport = 1;

  int iExpResult = 1;

  try 
  {
    // TSGroup_Tools.UtilMgr.RunCommand "ExportScene" #("OutDir", "c:\\2\\")
    if(_stricmp(exp_prm->FP_cmd, "ExportScene") == NULL) {
      theExporter.ExportAll(exportFile);
    } else
    if(_stricmp(exp_prm->FP_cmd, "ExportAnimation") == NULL) {
      theExporter.ExportBoneAnimation(exportFile);
    } else {
      char buf[512];
      sprintf(buf, "Invalid command: \"%s\"\n", exp_prm->FP_cmd);
      MessageBox(NULL, buf, EXPORTER_MSG_HEADER, MB_OK | MB_ICONEXCLAMATION);
    }
  } 
  catch(char *msg)
  {
    iExpResult = 0;
    gMaxHelper.StopProgressInfo();
    MessageBox(NULL, msg, EXPORTER_MSG_HEADER, MB_OK | MB_ICONEXCLAMATION);
  }
  expp.ip->ProgressEnd();

  SetCurrentDirectory(curDir);
  U_CloseLog(0);
  expp.ip->RedrawViews(expp.ip->GetTime());

  return iExpResult;
}

__declspec(dllexport) int expExecFunc(int id, expParams* prm, int* modeless)
{
  AFX_MANAGE_STATE(AfxGetStaticModuleState());

  // we have to read config, because DLL just got loaded before this call 
  ReadConfig(prm->ip);
  vExportSelected.var = 0; //@disabled vExportSelected

  char buf[512];
  sprintf(buf, "%s\\TSG_ExporterConfig.cfg", prm->ip->GetDir(APP_PLUGCFG_DIR));
  g_Config.LoadConfig(buf);

  expp        = *prm;

  if(id == FP_FUNC_ID) {
    if(modeless) *modeless = 0;

    // initialize exporter
    DWORD options = vExportSelected.var ? SCENE_EXPORT_SELECTED : 0;
    PREPARE_TO_EXPORT(prm->ip, NULL);

    int res = handle_FP_Call(prm);
    
    // have to close d3d here, otherwise UnloadLibrary will stall for long time
    d3d9.Close();
    
    return res;
  }

  if(id < 0 || id >= sizeof(expList)/sizeof(expList[0]))
    return 0;

  if(expList[id].type == T_CB) {
    *(DWORD*)expList[id].prm1 = expp.dw1;
    WriteConfig(expp.ip);
    return 1;
  }
  if(expList[id].type == T_DL) 
    return 1;

  if(prm->bShowLog)
    U_OpenLog();

  extern void U_CloseLog(int wait);

  try 
  {
    // initialize exporter
    DWORD options    = vExportSelected.var ? SCENE_EXPORT_SELECTED : 0;
    PREPARE_TO_EXPORT(prm->ip, NULL);
    expList[id].fn();
  } 
  catch(char *msg)
  {
    gMaxHelper.StopProgressInfo();
    MessageBox(NULL, msg, EXPORTER_MSG_HEADER, MB_OK | MB_ICONEXCLAMATION);
    U_CloseLog(0);
  }
  catch(...)
  {
    gMaxHelper.StopProgressInfo();
    MessageBox(NULL, "plugin module crashed", EXPORTER_MSG_HEADER, MB_OK | MB_ICONSTOP);
    U_CloseLog(0);
  }
  if(modeless) *modeless = 0;

  expp.ip->ProgressEnd();

  // force log closing
  U_CloseLog(1);

  d3d9.Close();

  g_Config.SaveConfig();

  expp.ip->RedrawViews(expp.ip->GetTime());

  return 1;
}


class CPluginApp : public CWinApp
{
public:
	CPluginApp();

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPluginApp)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CPluginApp)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////
// CPluginApp

BEGIN_MESSAGE_MAP(CPluginApp, CWinApp)
	//{{AFX_MSG_MAP(CPluginApp)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// The one and only CPluginApp object

	CPluginApp	theApp;

CPluginApp::CPluginApp()
{
}

BOOL CPluginApp::InitInstance() 
{

  HINSTANCE hInstance;

  //Get the DLL's HINSTANCE
  hInstance = AfxGetResourceHandle();

  //Init Win32 Controls
  InitCommonControls();

  return TRUE;				
}

int CPluginApp::ExitInstance() 
{
  return CWinApp::ExitInstance();
}

//TODO
// exec maxscript code
/*
  GUP * gp = (GUP*)CreateInstance(GUP_CLASS_ID, Class_ID(470000002,0);
  gp->ExecuteStringScript("Bp = $Bip01.Controller");
  gp->ExecuteStringScript("Bp.figuremode = TRUE");
*/
