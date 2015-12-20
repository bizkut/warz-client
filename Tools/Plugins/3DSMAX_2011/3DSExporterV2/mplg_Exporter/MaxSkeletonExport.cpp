//----------------------------------------------------------------------------//
// Includes                                                                   //
//----------------------------------------------------------------------------//

#include "StdAfx.h"
#include "MaxSkeletonExport.h"
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

CMaxSkeletonExport::CMaxSkeletonExport()
{
}

//----------------------------------------------------------------------------//
// Destructor                                                                 //
//----------------------------------------------------------------------------//

CMaxSkeletonExport::~CMaxSkeletonExport()
{
}

//----------------------------------------------------------------------------//
// Following methods have to be implemented to make it a valid plugin         //
//----------------------------------------------------------------------------//

const TCHAR *CMaxSkeletonExport::AuthorName()
{
	return _T("");
}

const TCHAR *CMaxSkeletonExport::CopyrightMessage()
{
	return _T("");
}

int CMaxSkeletonExport::DoExport(const TCHAR *name, ExpInterface *ei, Interface *ip, BOOL suppressPrompts, DWORD options)
{
  PREPARE_TO_EXPORT(ip, ei)

  EXPORT_BEGIN1
  {
    // export the skeleton
    theExporter.ExportSkeleton(name);
  }
  EXPORT_END1
}

const TCHAR *CMaxSkeletonExport::Ext(int i)
{
	switch(i)
	{
	case 0:
		return _T("skl");
	default:
		return _T("");
	}
}

BOOL CMaxSkeletonExport::SupportsOptions(int ext, DWORD options) {
	assert(ext == 0);	// We only support one extension
	return(options == SCENE_EXPORT_SELECTED) ? TRUE : FALSE;
	}


int CMaxSkeletonExport::ExtCount()
{
	return 1;
}

const TCHAR *CMaxSkeletonExport::LongDesc()
{
	return _T("TSGroup Skeleton File");
}

const TCHAR *CMaxSkeletonExport::OtherMessage1()
{
	return _T("");
}

const TCHAR *CMaxSkeletonExport::OtherMessage2()
{
	return _T("");
}

const TCHAR *CMaxSkeletonExport::ShortDesc()
{
	return _T("TSGroup Skeleton File");
}

void CMaxSkeletonExport::ShowAbout(HWND hWnd)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CDialog dlg(IDD_ABOUT);
	dlg.DoModal();
}

unsigned int CMaxSkeletonExport::Version()
{
	return 50;
}

//----------------------------------------------------------------------------//
