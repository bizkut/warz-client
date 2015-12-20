#ifndef MAX_SKELETON_EXPORT_H
#define MAX_SKELETON_EXPORT_H

//----------------------------------------------------------------------------//
// Class declaration                                                          //
//----------------------------------------------------------------------------//

class CMaxSkeletonExport : public SceneExport
{
// constructors/destructor
public:
	CMaxSkeletonExport();
	virtual ~CMaxSkeletonExport();

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
