#if !defined(AFX_LOG_H__874F78ED_F47C_4AD8_9B74_D8C8AEB5C43F__INCLUDED_)
#define AFX_LOG_H__874F78ED_F47C_4AD8_9B74_D8C8AEB5C43F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Log.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CLog window

#define	CLOG_BUTTON_ID	0x7445

class CLog : public CWnd
{
// Construction
public:
	CLog();

// Attributes
public:
	CWnd*		parentWnd;

	CRichEditCtrl	ed;
	 CRect		ed_Rect();
	CButton		btn1;
	 CRect		btn1_Rect();

	CFont		fnt1;
	DWORD		mCurColor;
	long		mTextLen;
	int		mIndentLevel;
	int		mOkState;
	int		m_disableClose;

	int		m_hadCRLF;

// Operations
public:
	enum EEffects
	{
	  _DefaultClr   = 0,
	  Red,
	  Green,
	  Blue,
	  Debug,
	  Bold		= (1l<<17),
	  Italic	= (1l<<18),
	  Underline	= (1l<<19),
	};

	class CIndent
	{
	  public:
	  int		*mRef;
	  CIndent()				{  if(CLog::gLog) CLog::gLog->IndentAdd();   }
	  ~CIndent()				{  if(CLog::gLog) CLog::gLog->IndentDec();   }
	};
#define CONCATENATE_DIRECT(s1, s2)	s1##s2
#define CONCATENATE(s1, s2)		CONCATENATE_DIRECT(s1, s2)
#define ANONYMOUS_VARIABLE(str)		CONCATENATE(str, __LINE__)
#define	CLOG_INDENT			CLog::CIndent ANONYMOUS_VARIABLE(unique)

static	CLog*		CreateWnd();
static	CLog*		 gLog;
	void		WaitForOk();

	void		SetColor(DWORD clr)	{  mCurColor = clr;	}
	void		IndentAdd() 		{  mIndentLevel++;	}
	void		IndentDec()		{  if(mIndentLevel) mIndentLevel--; }

	void		Log(const char* msg, ...);



// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLog)
	protected:
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CLog();

	// Generated message map functions
protected:
	//{{AFX_MSG(CLog)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnClose();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LOG_H__874F78ED_F47C_4AD8_9B74_D8C8AEB5C43F__INCLUDED_)
