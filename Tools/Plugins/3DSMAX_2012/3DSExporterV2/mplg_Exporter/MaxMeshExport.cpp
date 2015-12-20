//----------------------------------------------------------------------------//
// Includes                                                                   //
//----------------------------------------------------------------------------//

#include "StdAfx.h"
#include "MaxMeshExport.h"
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

CMaxMeshExport::CMaxMeshExport()
{
}

//----------------------------------------------------------------------------//
// Destructor                                                                 //
//----------------------------------------------------------------------------//

CMaxMeshExport::~CMaxMeshExport()
{
}

//----------------------------------------------------------------------------//
// Following methods have to be implemented to make it a valid plugin         //
//----------------------------------------------------------------------------//

const TCHAR *CMaxMeshExport::AuthorName()
{
	return _T("");
}

const TCHAR *CMaxMeshExport::CopyrightMessage()
{
	return _T("");
}

int CMaxMeshExport::DoExport(const TCHAR *name, ExpInterface *ei, Interface *ip, BOOL suppressPrompts, DWORD options)
{
  PREPARE_TO_EXPORT(ip, ei)

  EXPORT_BEGIN1
  {
    // export the mesh
   theExporter.ExportMesh(name);
  }
  EXPORT_END1
}

const TCHAR *CMaxMeshExport::Ext(int i)
{
	switch(i)
	{
	case 0:
		return _T("am");
	default:
		return _T("");
	}
}

BOOL CMaxMeshExport::SupportsOptions(int ext, DWORD options) {
	assert(ext == 0);	// We only support one extension
	return(options == SCENE_EXPORT_SELECTED) ? TRUE : FALSE;
	}


int CMaxMeshExport::ExtCount()
{
	return 1;
}

const TCHAR *CMaxMeshExport::LongDesc()
{
	return _T("TSGroup Mesh File");
}

const TCHAR *CMaxMeshExport::OtherMessage1()
{
	return _T("");
}

const TCHAR *CMaxMeshExport::OtherMessage2()
{
	return _T("");
}

const TCHAR *CMaxMeshExport::ShortDesc()
{
	return _T("TSGroup Mesh File");
}

void CMaxMeshExport::ShowAbout(HWND hWnd)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CDialog dlg(IDD_ABOUT);
	dlg.DoModal();
}

unsigned int CMaxMeshExport::Version()
{
	return 50;
}

//----------------------------------------------------------------------------//
