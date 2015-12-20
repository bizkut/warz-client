//----------------------------------------------------------------------------//
// Includes                                                                   //
//----------------------------------------------------------------------------//
#include "StdAfx.h"
#include "IDxMaterial.h"
#include "IFileResolutionManager.h"
#include "Exporter.h"
#include "MTString.h"

#define	DXLIGHTMAP_PLUGIN	Class_ID(0x727d33be, 0x3255c000)
#define GNORMAL_CLASS_ID	Class_ID(0x243e22c6, 0x63f6a014)

//----------------------------------------------------------------------------//
// Debug                                                                      //
//----------------------------------------------------------------------------//
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

const char*	CMaxMaterial::eternityTexNames[TEX_COUNT][2] = {
	{"Texture",		"Diffuse"},		// TEX_DIFFUSE
	{"NormalMap",		"Normal"},		// TEX_NORMAL
	{"DecalMask",		"Decal Mask"},		// TEX_DECALMASK
	{"EnvMap",		"Metalness"},		// TEX_ENVPOWER
	{"SpecularMap",		"Roughness"},		// TEX_GLOSS
	{"GlowMap",		"Self Illum"},		// TEX_SELFILLUM
	{"DetailNMap",		"Detail"},		// TEX_NORMALDETAIL
	{"DensityMap",		"Density"},		// TEX_THICKNESS
	{"CamoMask",		"CamoMask"},
	{"DistortionMap",	"DistortionMap"},
	{"SpecPowMap",		"SpecPow"},		// TEX_SPECULARPOW
};
	
//
//
// CMaxMaterials interface
//
//
std::vector<CMaxMaterial*>	CMaxMaterial::gAllSceneMtl;

CMaxMaterial::CMaxMaterial()
{
  m_pMtl            = NULL;
  m_texMaps         = 0;
  m_bTwoSided       = 0;
}

CMaxMaterial::~CMaxMaterial()
{
}

BOOL CMaxMaterial::IsSameParameters(const CMaxMaterial &m2)
{
  if(m_pMtl && !m2.m_pMtl) 
    return FALSE;
  if(!m_pMtl && m2.m_pMtl) 
    return FALSE;
  if(m_pMtl->ClassID() != m2.m_pMtl->ClassID())
    return FALSE;

  if(m_bTwoSided != m2.m_bTwoSided)
    return FALSE;

  // eventually, check other parameters
  return TRUE;
}

BOOL CMaxMaterial::IsSameTextures(const CMaxMaterial& m2)
{
  for(int i=0; i<TEX_COUNT; i++) 
  {
    if(m_slots[i].bUsed != m2.m_slots[i].bUsed)
      return FALSE;

    if(m_slots[i].bUsed == false)
      continue;
      
    if(_stricmp(m_slots[i].file.c_str(), m2.m_slots[i].file.c_str()) != NULL)
      return FALSE;
  }

  return TRUE;
};

int CMaxMaterial::DXPluginCheck(Mtl* pMtl)
{
#if 1
  // DxMaterial or extend ?
  IDxMaterial2* dxMtl = (IDxMaterial2*)pMtl->GetInterface(IDXMATERIAL2_INTERFACE);
  if(!dxMtl)
    return 0;

  if(strstr(dxMtl->GetEffectFile().GetFileName(), "StandardFX.fx") != NULL)
  {
    U_Log("DXMaterial found: StandardFX.fx\n");
    CLOG_INDENT;
    /*
            1: a10_dm.tga, type:1
            3: a10_sm.tga, type:0
            4: a10_nm.tga, type:0    
    */
    PBBitmap* bmp = NULL;
    if((bmp = dxMtl->GetEffectBitmap(1)) != NULL) {
      AddTextureSlot(TEX_DIFFUSE,  bmp->bi.Filename(), bmp->bm);
    }
    if((bmp = dxMtl->GetEffectBitmap(3)) != NULL) {
      AddTextureSlot(TEX_GLOSS, bmp->bi.Filename(), bmp->bm);
    }
    if((bmp = dxMtl->GetEffectBitmap(4)) != NULL) {
      AddTextureSlot(TEX_NORMAL,   bmp->bi.Filename(), bmp->bm);
    }
                
    return 1;
  }
  
  if(strstr(dxMtl->GetEffectFile().GetFileName(), "diffuse_blend_2t_shanel.fx") != NULL)
  {
    U_Log("DXMaterial found: 2UV\n");
    CLOG_INDENT;
    /*
            1: a10_dm.tga, type:1
            3: a10_sm.tga, type:0
            4: a10_nm.tga, type:0    
    */
    PBBitmap* bmp = NULL;
    if((bmp = dxMtl->GetEffectBitmap(0)) != NULL) {
      AddTextureSlot(TEX_DECALMASK, bmp->bi.Filename(), bmp->bm);
    }
    if((bmp = dxMtl->GetEffectBitmap(1)) != NULL) {
      AddTextureSlot(TEX_DIFFUSE, bmp->bi.Filename(), bmp->bm);
    }
    return 1;
  }

  U_Log("!runknown DXMaterial found, %s\n", dxMtl->GetEffectFile().GetFileName());
  CLOG_INDENT;
  
  // Get number of bitmaps used
  int nBmps = dxMtl->GetNumberOfEffectBitmaps();
  { 
    U_Log("%d bitmaps\n", nBmps);
    CLOG_INDENT;
    for(int i=0; i<nBmps; i++) {
      PBBitmap* bmp = dxMtl->GetEffectBitmap(i);
      if(!bmp) continue;
      U_Log("%d: %s, type:%d\n", i, bmp->bi.Filename(), dxMtl->GetBitmapUsage(i));
    }
  }

  // GetParamBlock() work only on the material pointer
  IParamBlock2* pblock = pMtl->GetParamBlock(0);  
  //extern void U_DumpParamBlock(IParamBlock2 *pbl);
  //U_DumpParamBlock(pblock);

#endif  
  return 0;
}

void CMaxMaterial::AddTextureSlot(TexType slot, const char* fname, Bitmap* bmp)
{
  if(m_slots[slot].bUsed) {
    U_Log("!rmaterial slot %d already used by %s!\n", slot, m_slots[slot].GetTargetFileName());
    //ThrowError("wrong material slot\n");
    return;
  }
  
  IFileResolutionManager* pathMgr = IFileResolutionManager::GetInstance();
  CStr _resolvedPath = pathMgr->GetFullFilePath(fname, MaxSDK::AssetManagement::kBitmapAsset);
  const char* resolvedPath = _resolvedPath.data();
  if(resolvedPath == NULL || *resolvedPath == 0)
    resolvedPath = fname;
  //U_Log("%s->%s\n", fname, resolvedPath);

  if(!bmp) {
    //@ somehow load the new bitmap 
    // bmp = TheManager->Load(&bbmp1->bi, &status);
  }

  m_slots[slot].bUsed = true;
  m_slots[slot].file  = resolvedPath;
  m_slots[slot].mat_file = resolvedPath;
  m_slots[slot].bmp   = bmp;
  //U_Log("TexMap%d: %s\n", slot, resolvedPath);
  U_Log("!d%s:\t%s\n", eternityTexNames[slot][1], m_slots[slot].GetTargetFileName());

  /*
  if(bmp) 
  {
    U_Log("TexMap%d: %s %dx%d %s\n", slot, resolvedPath, bmp->Width(), bmp->Height(), bmp->HasAlpha() ? "(alpha)" : "");
  } else {
    U_Log("TexMap%d: %s - file not loaded\n", slot, resolvedPath);
  }
  */

  m_texMaps++;
  return;
}


void CMaxMaterial::Create(Mtl *pMtl)
{
  m_pMtl = pMtl;
  m_texMaps = 0;

  // create empty material..
  if(pMtl == NULL) {
    U_Log("Mat: (empty)\n");
    strcpy(m_materialName, "_DEFAULT_");
    return;
  }

  strcpy(m_materialName, U_FixName(pMtl->GetName()));
  if(pMtl->ClassID() == Class_ID(DMTL_CLASS_ID, 0)) {
    m_bTwoSided = ((StdMat *)pMtl)->GetTwoSided();
  }

  U_Log("Mat: '%s' %s\n", pMtl->GetName(), m_bTwoSided ? "(2-Sided)" : "");
  CLOG_INDENT;

  // check for DirectX plugin - if it active, don't check for texmaps
  if(DXPluginCheck(pMtl))
    return;

  for(int i=0; i < pMtl->NumSubTexmaps(); i++) 
  {
    Texmap* subTex = pMtl->GetSubTexmap(i);
    if(!subTex) continue;

    if(subTex->ClassID() == Class_ID(BMTEX_CLASS_ID, 0x00)) 
    {
      BitmapTex* bmtex = (BitmapTex *)subTex;

      switch(i) 
      {
        case ID_AM:   //!< Ambient
          // [5:01:17 PM] WOz: Gary Brunetti: we only put the AO map into ambient color in max because there is no AO slot in max
          AddTextureSlot(TEX_DECALMASK, bmtex->GetMapName(), bmtex->GetBitmap(0)); 
          break;
        case ID_DI: 
          AddTextureSlot(TEX_DIFFUSE, bmtex->GetMapName(), bmtex->GetBitmap(0)); 
          break;
        case ID_SH: //!< Glossiness (Shininess in 3ds Max release 2.0 and earlier)
          AddTextureSlot(TEX_GLOSS, bmtex->GetMapName(), bmtex->GetBitmap(0)); 
          break;
        case ID_SS: //!< Specular Level (Shininess strength in 3ds Max release 2.0 and earlier)
          AddTextureSlot(TEX_ENVPOWER, bmtex->GetMapName(), bmtex->GetBitmap(0)); 
          AddTextureSlot(TEX_SPECULARPOW, bmtex->GetMapName(), bmtex->GetBitmap(0)); 
          break;
        case ID_SI: //!< Self-illumination
          AddTextureSlot(TEX_SELFILLUM, bmtex->GetMapName(), bmtex->GetBitmap(0)); 
          break;
        case ID_FI: //!< Filter color
          // used as density map
          AddTextureSlot(TEX_THICKNESS, bmtex->GetMapName(), bmtex->GetBitmap(0)); 
          break;
        default:
          U_Log("!r: unknown slot %d for texture %s\n", i, bmtex->GetMapName());
          break;
      }
    }
    else if(subTex->ClassID() == GNORMAL_CLASS_ID)
    {
      // normal at slot0 there
      Texmap* sub = subTex->GetSubTexmap(0); 
      if(sub && sub->ClassID() == Class_ID(BMTEX_CLASS_ID, 0)) {
        BitmapTex* bmtex = (BitmapTex *)sub;
        AddTextureSlot(TEX_NORMAL, bmtex->GetMapName(), bmtex->GetBitmap(0));
      }
      
      // additional bump at slo1
      sub = subTex->GetSubTexmap(1); 
      if(sub && sub->ClassID() == Class_ID(BMTEX_CLASS_ID, 0)) {
        BitmapTex* bmtex = (BitmapTex *)sub;
        AddTextureSlot(TEX_NORMALDETAIL, bmtex->GetMapName(), bmtex->GetBitmap(0));
      }
    }
    else
    {
      U_Log("!runknown subTex->ClassID(0x%x,0x%x)\n", subTex->ClassID().PartA(), subTex->ClassID().PartB());
    }
  }

  return;
}


void CMaxMaterial::AddMaterial(Mtl *pMtl)
{
  // check if we already have material in our base
  while(FindMaterial(pMtl, 0) == NULL) 
  {
  // skip MULTI_CLASS_ID materials
    if(pMtl && pMtl->ClassID().PartA() == MULTI_CLASS_ID)
      break;

    CMaxMaterial *mat = new CMaxMaterial;
    mat->Create(pMtl);

    // check if we already have SAME material
    int bIsNew = 1;
    
    /* // disabled, because we don't need that functionality here
    for(int i=0; i < gAllSceneMtl.size(); i++) {
      if(gAllSceneMtl[i]->IsSameParameters(*mat) && gAllSceneMtl[i]->IsSameTextures(*mat)) {
	U_Log("-> Material is exactly the same as %s\n", gAllSceneMtl[i]->m_materialName);
	gAllSceneMtl[i]->m_vectorSameMtl.push_back(pMtl);
        bIsNew = 0;
	break;
      }
    }
    */

    if(bIsNew) {
      CMaxMaterial::gAllSceneMtl.push_back(mat);
    }
    break;
  }

  if(pMtl) 
    for(int i=0; i<pMtl->NumSubMtls(); i++) 
      AddMaterial(pMtl->GetSubMtl(i));

  return;
}

CMaxMaterial *CMaxMaterial::FindMaterial(Mtl *pMtl, int report_fail)
{
  for(int i=0; i < gAllSceneMtl.size(); i++) {
    if(gAllSceneMtl[i]->m_pMtl == pMtl)
      return gAllSceneMtl[i];

    for(int j=0; j < gAllSceneMtl[i]->m_vectorSameMtl.size(); j++) {
      if(gAllSceneMtl[i]->m_vectorSameMtl[j] == pMtl) {
        return gAllSceneMtl[i];  
      }
    }
  }

  if(report_fail) {
    ThrowError("Can't find Material\n");
  }

  return NULL;
}

static void ReadAndHandleOutput(HANDLE hPipeRead)
{
  char  lpBuffer[256];
  DWORD nBytesRead;

  while(TRUE)
  {
    if(!ReadFile(hPipeRead, lpBuffer, sizeof(lpBuffer), &nBytesRead, NULL) || !nBytesRead)
    {
      if(GetLastError() == ERROR_BROKEN_PIPE)
        break; // pipe done - normal exit path.
      else
        U_Log("!rReadFile failed"); // Something bad happened.
    }

    // Display the character read on the screen.
    lpBuffer[nBytesRead] = 0;
    U_Log("!d%s", lpBuffer);
  }
}


void CMaxMaterial::DoCopyTexture(texslot_s& slot, const char* outDir)
{
  char out_file[MAX_PATH];
  const char* in_file = slot.file.c_str();
  MTFileName mtName(slot.file);

  if(theExporter.m_ExpConvertToDDS == 0 || _stricmp(in_file + strlen(in_file) - 3, "dds") == 0)
  {
    sprintf(out_file, "%s\\%s", outDir, mtName.GetName());
    U_Log("!d copying '%s'\n", mtName.GetName());
    if(::CopyFile(in_file, out_file, FALSE) == 0) {
      U_Log("!r failed to copy %s to %s, %x\n", in_file, out_file, GetLastError());
    }
    return;
  }
  
  // converting to dds path
  sprintf(out_file, "%s\\%s", outDir, mtName.GetName("dds"));
  

  //
  // some magic with redirecting stdout
  //
  SECURITY_ATTRIBUTES sa = {0};
  sa.nLength= sizeof(SECURITY_ATTRIBUTES);
  sa.lpSecurityDescriptor = NULL;
  sa.bInheritHandle = TRUE;

  HANDLE hOutputReadTmp,hOutputRead,hOutputWrite;
  HANDLE hErrorWrite;
  // Create the child output pipe.
  if(!CreatePipe(&hOutputReadTmp,&hOutputWrite,&sa,0))
    U_Log("!rCreatePipe");
  // Create new output read handle and the input write handles. Set
  // the Properties to FALSE. Otherwise, the child inherits the
  // properties and, as a result, non-closeable handles to the pipes
  // are created.
  if(!DuplicateHandle(GetCurrentProcess(), hOutputReadTmp, GetCurrentProcess(), &hOutputRead, 0, FALSE, DUPLICATE_SAME_ACCESS))
    U_Log("!rDupliateHandle");
  // Create a duplicate of the output write handle for the std error
  if(!DuplicateHandle(GetCurrentProcess(), hOutputWrite, GetCurrentProcess(), &hErrorWrite, 0, TRUE,DUPLICATE_SAME_ACCESS))
    U_Log("!rDupliateHandle");
  CloseHandle(hOutputReadTmp);
  //
  // end of some magic with redirecting stdout
  //
  
  char params[512];
  sprintf(params, "\"%s\" %s -file \"%s\" -output \"%s\"", 
    theExporter.m_nvdxtPath,
    theExporter.m_nvdxtParams,
    in_file,
    out_file);

  STARTUPINFO si = {0};
  si.cb = sizeof(si);
  si.dwFlags    = STARTF_USESTDHANDLES;
  si.hStdOutput = hOutputWrite; 
  si.hStdError  = hErrorWrite;
  PROCESS_INFORMATION pi = {0};
  
  U_Log("!dnvdxt: %s\n", params);
  BOOL res = CreateProcess(
    NULL,
    params,
    NULL,
    NULL,
    TRUE, // inherit handles
    DETACHED_PROCESS, // no console
    NULL,
    NULL,
    &si,
    &pi);

  // Close pipe handles (do not continue to modify the parent).
  // You need to make sure that no handles to the write end of the
  // output pipe are maintained in this process or else the pipe will
  // not close when the child process exits and the ReadFile will hang.
  CloseHandle(hOutputWrite);
  CloseHandle(hErrorWrite);
    
  if(res == 0)
  {
    char lpMsgBuf[512];
    DWORD dw = GetLastError();
    FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, 
                  NULL, dw, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), lpMsgBuf, sizeof(lpMsgBuf), NULL);
 
    ThrowError("there was error running %s\n\n%s", params, lpMsgBuf);
    return;
  }

  // wait for finish
  ReadAndHandleOutput(hOutputRead);
  CloseHandle(hOutputRead);

  WaitForSingleObject(pi.hProcess, INFINITE);

  DWORD exitCode;
  GetExitCodeProcess(pi.hProcess, &exitCode);
  if(exitCode != 0)
  {
    ThrowError("there was error running %s\n\nerror code: %d", params, exitCode);
  }

  // replace with new name
  slot.mat_file = out_file;
  
  return;
}

void CMaxMaterial::CopyTextures(const char* pOutDir)
{
  gMaxHelper.StartProgressInfo("Copying Textures");

  int usedTex = 0;
  for(int iTex=0; iTex < TEX_COUNT; iTex++)
    if(m_slots[iTex].bUsed) usedTex++;

  for(int iTex=0, procTex=0; iTex < TEX_COUNT; iTex++)
  {
    if(!m_slots[iTex].bUsed)
      continue;

    DoCopyTexture(m_slots[iTex], pOutDir);

    procTex++;
    gMaxHelper.SetProgressInfo((int)(100.0f * (float)procTex / (float)usedTex));
    U_LogIdleTick();
  }
  
  gMaxHelper.StopProgressInfo();
  return;
}
