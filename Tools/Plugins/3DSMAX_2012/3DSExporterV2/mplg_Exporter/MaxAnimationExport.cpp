//----------------------------------------------------------------------------//
// Includes                                                                   //
//----------------------------------------------------------------------------//

#include "StdAfx.h"
#include "MaxAnimationExport.h"
#include "Exporter.h"
#include "MaxInterface.h"

//----------------------------------------------------------------------------//
// Debug                                                                      //
//----------------------------------------------------------------------------//

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CMaxAnimationExport::CMaxAnimationExport()		
{ 
}

CMaxAnimationExport::~CMaxAnimationExport()		
{ 
}

const TCHAR *CMaxAnimationExport::AuthorName()		
{ 
  return _T("");	
}

const TCHAR *CMaxAnimationExport::CopyrightMessage()	
{ 
  return _T("");	
}

BOOL CMaxAnimationExport::SupportsOptions(int ext, DWORD options) 
{
	assert(ext == 0);	// We only support one extension
	return(options == SCENE_EXPORT_SELECTED) ? TRUE : FALSE;
}

int CMaxAnimationExport::ExtCount()
{
	return 1;
}

const TCHAR *CMaxAnimationExport::LongDesc()
{
	return _T("TSGroup Bone Animation File");
}

const TCHAR *CMaxAnimationExport::OtherMessage1()
{
	return _T("");
}

const TCHAR *CMaxAnimationExport::OtherMessage2()
{
	return _T("");
}

const TCHAR *CMaxAnimationExport::ShortDesc()
{
	return _T("TSGroup Bone Animation File");
}

void CMaxAnimationExport::ShowAbout(HWND hWnd)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CDialog dlg(IDD_ABOUT);
	dlg.DoModal();
}

unsigned int CMaxAnimationExport::Version()
{
	return 50;
}



int CMaxAnimationExport::DoExport(const TCHAR *name, ExpInterface *ei, Interface *ip, BOOL suppressPrompts, DWORD options)
{
  PREPARE_TO_EXPORT(ip, ei)

  EXPORT_BEGIN1
  {
   // export the animation
   theExporter.ExportBoneAnimation(name);
  }
  EXPORT_END1
}

const TCHAR *CMaxAnimationExport::Ext(int i)
{
	switch(i)
	{
	case 0:
		return _T("anm");
	default:
		return _T("");
	}
}
