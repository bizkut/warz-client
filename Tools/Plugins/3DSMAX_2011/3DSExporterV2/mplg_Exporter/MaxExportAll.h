#ifndef MAX_EXPORT_ALL_H
#define MAX_EXPORT_ALL_H

//----------------------------------------------------------------------------//
// Class declaration                                                          //
//----------------------------------------------------------------------------//

class CMaxExportAll : public SceneExport
{
// constructors/destructor
public:
	CMaxExportAll();
	virtual ~CMaxExportAll();

// interface functions	
public:
	const TCHAR *AuthorName();
	const TCHAR *CopyrightMessage();
	int DoExport(const TCHAR *name, ExpInterface *ei, Interface *i, BOOL suppressPrompts, DWORD options);
	const TCHAR *Ext(int n);
  BOOL SupportsOptions(int ext, DWORD options);
  int ExtCount();
	const TCHAR *LongDesc();
	const TCHAR *OtherMessage1();
	const TCHAR *OtherMessage2();
	const TCHAR *ShortDesc();
	void ShowAbout(HWND hWnd);
	unsigned int Version();
};

#endif

//----------------------------------------------------------------------------//
