//----------------------------------------------------------------------------//
// Includes                                                                   //
//----------------------------------------------------------------------------//

#include "StdAfx.h"
#include "MaxExportAll.h"
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

//----------------------------------------------------------------------------//
// Constructors                                                               //
//----------------------------------------------------------------------------//

CMaxExportAll::CMaxExportAll()
{
}

//----------------------------------------------------------------------------//
// Destructor                                                                 //
//----------------------------------------------------------------------------//

CMaxExportAll::~CMaxExportAll()
{
}

//----------------------------------------------------------------------------//
// Following methods have to be implemented to make it a valid plugin         //
//----------------------------------------------------------------------------//

const TCHAR *CMaxExportAll::AuthorName()
{
	return _T("");
}

const TCHAR *CMaxExportAll::CopyrightMessage()
{
  return _T("");
}

int CMaxExportAll::DoExport(const TCHAR *name, ExpInterface *ei, Interface *ip, BOOL suppressPrompts, DWORD options)
{
  PREPARE_TO_EXPORT(ip, ei)

  EXPORT_BEGIN1
  {
   // export the animation
   theExporter.ExportAll(name);
  }
  EXPORT_END1
}

const TCHAR *CMaxExportAll::Ext(int i)
{
	switch(i)
	{
	case 0:
		return _T("smc");
	default:
		return _T("");
	}
}

BOOL CMaxExportAll::SupportsOptions(int ext, DWORD options) {
	assert(ext == 0);	// We only support one extension
	return(options == SCENE_EXPORT_SELECTED) ? TRUE : FALSE;
	}

int CMaxExportAll::ExtCount()
{
	return 1;
}

const TCHAR *CMaxExportAll::LongDesc()
{
	return _T("TSGroup Export Files");
}

const TCHAR *CMaxExportAll::OtherMessage1()
{
	return _T("");
}

const TCHAR *CMaxExportAll::OtherMessage2()
{
	return _T("");
}

const TCHAR *CMaxExportAll::ShortDesc()
{
	return _T("TSGroup Export Files");
}

void CMaxExportAll::ShowAbout(HWND hWnd)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CDialog dlg(IDD_ABOUT);
	dlg.DoModal();
}

unsigned int CMaxExportAll::Version()
{
	return 50;
}

//----------------------------------------------------------------------------//
