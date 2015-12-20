#pragma once


#include "../CmdProcessor/CmdConsole.h"
#include "Tsg_stl/TArray.h"

#define CONSOLE_MAX_LINES		512
#define CONSOLE_LOG_NAME		"Console.log"
#define CONSOLE_LINE_SIZE		256
#define CONSOLE_MAX_HISTORY		32
#define CONSOLE_MAX_EXECBUF		4
#define CONSOLE_PAGESIZE		10



#ifndef PLATFORM_PS3
	#define CONSOLE_KEY_UP			0x00
	#define CONSOLE_KEY_DOWN		0x01
	#define CONSOLE_KEY_LEFT		0x02
	#define CONSOLE_KEY_RIGHT		0x03
	#define CONSOLE_KEY_END			0x04
	#define CONSOLE_KEY_HOME		0x05
	#define CONSOLE_KEY_DELETE		0x06
	#define CONSOLE_KEY_PRIOR		0x07
	#define CONSOLE_KEY_BACK		VK_BACK		// for back key (index 8)
	#define CONSOLE_KEY_TAB			VK_TAB		// for tab key (index 9)
	#define CONSOLE_KEY_NEXT		0x0A
	#define CONSOLE_KEY_ESCAPE		VK_ESCAPE	// for escape key (index 27)

	#define CONSOLE_KEY_RETURN		VK_RETURN
#else
	#define CONSOLE_KEY_UP			0x00
	#define CONSOLE_KEY_DOWN		0x01
	#define CONSOLE_KEY_LEFT		0x02
	#define CONSOLE_KEY_RIGHT		0x03
	#define CONSOLE_KEY_END			0x04
	#define CONSOLE_KEY_HOME		0x05
	#define CONSOLE_KEY_DELETE		0x06
	#define CONSOLE_KEY_PRIOR		0x07
	#define CONSOLE_KEY_BACK		0x08		// for back key (index 8)
	#define CONSOLE_KEY_TAB			0x09		// for tab key (index 9)
	#define CONSOLE_KEY_NEXT		0x0A
	#define CONSOLE_KEY_ESCAPE		0x1B	// for escape key (index 27)

	#define CONSOLE_KEY_RETURN		0x0D
#endif

class r3dTexture;

///////////////////////////////////////////////////////////////////////////////////////////////
//
//	>	class EngineConsole 
//
///////////////////////////////////////////////////////////////////////////////////////////////
class EngineConsole : public CmdConsole 
{
	typedef	char TString[ CONSOLE_LINE_SIZE ];

private:
		
	TString		m_szLines[ CONSOLE_MAX_LINES ];
	TString		m_szHistory[ CONSOLE_MAX_HISTORY ];
	
	TString		m_szCmdLine;
	
	int			m_iCursorPos;
	float		m_fCursorPosPix;

	int			m_iVisibleLines;

	
	int			m_nFirstVisibleLine;
	int			m_nLastVisibleLine;

	int			m_iCurrentLine;
	int			m_iCurrentHistory;

	int			m_iMiniConsoleLines;

	int			m_iPageSize;

	float		m_fLastPrintTime;

	int			m_iFontSize;
	int			m_iOffsetY;

	int			m_nPosX;
	int			m_nPosY;
	int			m_nWidth;
	int			m_nHeight;

	bool		m_bInited;
	bool		m_bNeedUpdateCursorPos;

	void		AddHistory		( const char * szText );
	int			GetHistoryPrev	( char * szOut );
	int			GetHistoryNext	( char * szOut );


	void		PageUp			();
	void		PageDown		();
	
	void		RequestUpdateCursorPos	();
	void		UpdateCursorPos	();

//	IFont		*m_pFont;

	int			m_nFontSize;
	

	int			LinesUp			();
	int			LinesDown		();


	void		SendChar		( uint8_t send );
	bool		SendControlKey	( uint16_t send );
	
	//void		FlushCmdBuf		();

	void		NewLine			()							{ m_szCmdLine[ 0 ] = '\0';	m_iCursorPos = 0; m_fCursorPosPix = 0.f; };

	//class CStrings *	GetHistory		();

	void			Draw2DSprite	();

	//////////////////////////////////////////////////////////////////////////////////////////////////////////
	//
	//	>	Enumerate cmd
	//
	//////////////////////////////////////////////////////////////////////////////////////////////////////////
	void				NextComplitCmd	();
	void				ResetComplitCmd	();

	r3dTL::TArray< FixedString >	m_dFoundCmd;
	int					m_nFirstCmd;

	void				EnumCmd			();
	virtual void		FoundCmdCallback ( const char * szFullName );
	
public:

	static void			ProcessKey		( uint8_t wParam );
	static void			ProcessChar		( char msg );

	void				AddToCmdHistory ( const char * szString );
	void				Print			( const char * szString );

#if !DISABLE_CONSOLE
	void				Update			();
	void				Render			();
#endif
	
	void				Init			();
	void				Release			();
						
						EngineConsole		();
						~EngineConsole		();
};

extern EngineConsole* g_pEngineConsole;

