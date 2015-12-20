#include "r3dPCH.h"
#include "EngineConsole.h"

#include "../CmdProcessor/CmdVar.h"
#include "../CmdProcessor/CmdProcessor.h"
#include "r3d.h"

#include "Config.h"


extern	CD3DFont*	MenuFont_Editor;

EngineConsole* g_pEngineConsole;

bool IsConsoleVisible()
{
	if ( !g_pDefaultConsole )
		return false;
	return g_pDefaultConsole->IsVisible();
}

//--------------------------------------------------------------------------------------------------------
void ShowConsole()
{
	assert( g_pDefaultConsole );
	if ( ! g_pDefaultConsole )
		return;

	if ( g_pDefaultConsole->IsVisible() )
		g_pDefaultConsole->HideWindow();
	else
		g_pDefaultConsole->ShowWindow();

}


//--------------------------------------------------------------------------------------------------------
EngineConsole::EngineConsole() 
	: m_bInited( false )
	, m_nFirstCmd( INVALID_INDEX )
	, m_bNeedUpdateCursorPos( true )
{
	for ( int i = 0; i < CONSOLE_MAX_LINES; i++ )
	{
		m_szLines[ i ][ 0 ] = '\0';
	}

	for ( int i = 0; i < CONSOLE_MAX_HISTORY; i++ )
	{
		m_szHistory[ i ][ 0 ] = '\0';
	}


	m_szCmdLine[ 0 ] = '\0';
	m_iCursorPos = 0;
	m_fCursorPosPix = 0.f;

	
	m_nFirstVisibleLine = -1;
	m_nLastVisibleLine = -1;
	m_iCurrentLine = -1;
	m_iVisibleLines = -1;

	m_iCurrentHistory = 0;

	m_fLastPrintTime = 0;
	m_iMiniConsoleLines = 0;
	

}

//---------------------------------------------------------------------------------------------
EngineConsole::~EngineConsole()
{
}

//---------------------------------------------------------------------------------------------
void EngineConsole::Init()
{
	if ( m_bInited )
		return;
		
	m_nWidth = ( int )r3dRenderer->ScreenW;
	m_nHeight = ( int )( r3dRenderer->ScreenH / 2.f );
	m_nPosX = 0;
	m_nPosY = 0;

	m_nFontSize = 15;

	m_iOffsetY = 0; // FIXME:
	m_iVisibleLines = ( m_nHeight - 5 ) / m_nFontSize - 1;	

	m_iPageSize = 1; 

	m_bInited = true;
}

//---------------------------------------------------------------------------------------------
void EngineConsole::Release()
{

}

//---------------------------------------------------------------------------------------------
void EngineConsole::Print( const char * szText )
{
	if ( m_nLastVisibleLine == m_iCurrentLine )
	{
		m_nLastVisibleLine++;

		// advance first line if already showing at full window
		if ( m_nLastVisibleLine - m_nFirstVisibleLine >= m_iVisibleLines )
		{
			m_nFirstVisibleLine ++;
		}
	}

	if ( m_nLastVisibleLine >= CONSOLE_MAX_LINES )
	{
		m_nLastVisibleLine = 0;
		m_nFirstVisibleLine ++;
	}

	m_iCurrentLine++;
	if ( m_iCurrentLine >= CONSOLE_MAX_LINES )
	{
		m_iCurrentLine = 0;
	}

	r3dscpy_s( m_szLines[ m_iCurrentLine ], CONSOLE_LINE_SIZE, szText );
}

//--------------------------------------------------------------------------------------------------------
void EngineConsole::Draw2DSprite()
{	
	r3dDrawBox2D( 0, 0, r3dRenderer->ScreenW, r3dRenderer->ScreenH / 2, r3dColor24( 0xff, 0xff, 0xff ), NULL );
}

//--------------------------------------------------------------------------------------------------------
#if !DISABLE_CONSOLE
void EngineConsole::Update()
{
	if( IsVisible() )
	{
		void r3dProcessCopyPaste( char* , int, bool );
		r3dProcessCopyPaste( m_szCmdLine, CONSOLE_LINE_SIZE, true );
	}
}

void EngineConsole::Render()
{	
	struct States
	{
		States()
		{
			D3DPERF_BeginEvent( 0, L"EngineConsole::Render" ) ;
			r3dRenderer->SetRenderingMode( R3D_BLEND_ALPHA | R3D_BLEND_PUSH ) ;
		}

		~States()
		{
			r3dRenderer->SetRenderingMode( R3D_BLEND_POP ) ;
			D3DPERF_EndEvent() ;
		}

	} states ; (void)states ;

	if ( m_bNeedUpdateCursorPos )
	{
		UpdateCursorPos();
		m_bNeedUpdateCursorPos = false;
	}

	int		iNumViewLines;
	float	fPixelsLeft = 0;
	bool	bMiniMode = cl_miniconsole->GetInt() != 0;


	float fTime = timeGetTime() / 1000.f; //longTimeGet();


	if ( IsVisible() )
	{
		bMiniMode = false; 
	}
	else
	{
		if ( ! cl_miniconsole->GetInt() /*|| !g_developer->ival */ )
			return;
	}


	if ( bMiniMode )
	{
		if ( ( fTime - m_fLastPrintTime ) >= cl_miniconsoletime->GetFloat() )
		{
			if ( ! m_iMiniConsoleLines )
				return;
			
			m_iMiniConsoleLines--;
			m_fLastPrintTime = fTime;

			fPixelsLeft = (float)m_nFontSize;
		}
		else
		{
			if ( cl_miniconsoletime->GetFloat() > 0.0f )
			{
				fPixelsLeft = m_nFontSize * ( 1 - ( fTime - m_fLastPrintTime ) / cl_miniconsoletime->GetFloat() );
			}

			if ( fPixelsLeft < 0 )
			{
				fPixelsLeft = 0;
			}
		}
	}	

	// Draw console background
	if ( ! bMiniMode )
	{
		r3dSetIdentityTransform( 0 ) ;
		r3dSetFwdColorShaders( r3dColor( 0, 0, 255, 133 ) ) ;
		Draw2DSprite();
		r3dRenderer->SetVertexShader() ;
		r3dRenderer->SetPixelShader() ;
	}



	// Draw console lines

	int			iLineNum;
	uint32_t	dwLinesColor;

	if ( bMiniMode )
	{
		iNumViewLines = m_iMiniConsoleLines;
		iLineNum = m_iCurrentLine;
		dwLinesColor = 0xFFC0C0C0;
	}
	else
	{
		iNumViewLines = m_iVisibleLines;
		iLineNum = m_nLastVisibleLine;
		dwLinesColor = 0xFFFFFFFF;
	}

	int iYStart = bMiniMode ? 
		( m_nFontSize * ( iNumViewLines - 2 ) ) + m_iOffsetY + 2 
		: 
		( m_nFontSize * ( iNumViewLines - 1 ) ) + m_iOffsetY - 4;


	if ( iLineNum >= 0 )
	{
		for ( int i = 1, y = iYStart; i <= iNumViewLines; i++, y -= m_nFontSize )
		{
			MenuFont_Editor->PrintF( m_nPosX + 10.f, m_nPosY + y + fPixelsLeft, r3dColor24( dwLinesColor ), m_szLines[ iLineNum ] );

			iLineNum --;
			if ( iLineNum == m_iCurrentLine )
				break; // top

			if ( iLineNum < 0 )
				iLineNum = CONSOLE_MAX_LINES - 1;
		}
	}

	
	// Draw current input
	if ( ! bMiniMode )
	{
		int y = m_nFontSize * iNumViewLines + m_iOffsetY + 1;
		
		MenuFont_Editor->PrintF( (float)m_nPosX + 10.f, (float)m_nPosY + y, r3dColor24( 0xff80ff50 ), m_szCmdLine );

		int cl = (int)(170.f + 85.f * cosf( 15 * fTime ) );
		uint32_t dwCursorColor = 0xff000000 | ( cl << 16 ) | ( cl << 8 ) | cl;
		
		MenuFont_Editor->PrintF( (float)m_nPosX + 8.f + m_fCursorPosPix, (float)m_nPosY + y, r3dColor24( dwCursorColor ), "|" );
	}
}

#endif

//--------------------------------------------------------------------------------------------------------
void EngineConsole::ProcessChar( char msg )
{
	if ( ! g_pEngineConsole )
		return;

	g_pEngineConsole->SendChar( msg );
}

//--------------------------------------------------------------------------------------------------------
void EngineConsole::ProcessKey( uint8_t wParam )
{
	if ( ! g_pEngineConsole )
		return;

	switch ( wParam )
	{
	case VK_UP:			g_pEngineConsole->SendControlKey( CONSOLE_KEY_UP );			return;
	case VK_DOWN:		g_pEngineConsole->SendControlKey( CONSOLE_KEY_DOWN );		return;
	case VK_LEFT:		g_pEngineConsole->SendControlKey( CONSOLE_KEY_LEFT );		return;
	case VK_RIGHT:		g_pEngineConsole->SendControlKey( CONSOLE_KEY_RIGHT );		return;
	case VK_END:		g_pEngineConsole->SendControlKey( CONSOLE_KEY_END );		return;
	case VK_HOME:		g_pEngineConsole->SendControlKey( CONSOLE_KEY_HOME );		return;
	case VK_DELETE:		g_pEngineConsole->SendControlKey( CONSOLE_KEY_DELETE );		return;
	case VK_PRIOR:		g_pEngineConsole->SendControlKey( CONSOLE_KEY_PRIOR );		return;
	case VK_NEXT:		g_pEngineConsole->SendControlKey( CONSOLE_KEY_NEXT );		return;
	case VK_BACK:		g_pEngineConsole->SendControlKey( CONSOLE_KEY_BACK );		return;
	case VK_TAB:		g_pEngineConsole->SendControlKey( CONSOLE_KEY_TAB );		return;
	case VK_ESCAPE:		g_pEngineConsole->SendControlKey( CONSOLE_KEY_ESCAPE );		return;
	case VK_RETURN:		g_pEngineConsole->SendControlKey( CONSOLE_KEY_RETURN );		return;
#if !DISABLE_CONSOLE
	case VK_OEM_3:		ShowConsole();
#endif
	}
}

//--------------------------------------------------------------------------------------------------------
bool EngineConsole::SendControlKey( uint16_t send )
{
	if ( ! IsVisible() )
		return false;

	if ( send != CONSOLE_KEY_TAB &&
		send != CONSOLE_KEY_ESCAPE &&
		send != CONSOLE_KEY_RETURN )
	{
		ResetComplitCmd(); 
	}


	if ( send == CONSOLE_KEY_ESCAPE )
	{
		NewLine();		
		return true;
	}

	if ( send == CONSOLE_KEY_BACK )
	{
		if ( m_iCursorPos == 0 ) 
			return true;

		r3dscpy( m_szCmdLine + m_iCursorPos - 1, m_szCmdLine + m_iCursorPos );
		m_iCursorPos--;
		RequestUpdateCursorPos();		

		return true;
	}

	if ( send == CONSOLE_KEY_DELETE )
	{
		if ( m_iCursorPos >= ( int )strlen( m_szCmdLine ) )
			return true;
		r3dscpy( m_szCmdLine + m_iCursorPos, m_szCmdLine + m_iCursorPos + 1 );
		RequestUpdateCursorPos();
		return true;
	}

	if ( send == CONSOLE_KEY_LEFT )
	{
		if ( m_iCursorPos == 0 )
			return true;

		m_iCursorPos--;
		RequestUpdateCursorPos();
		return true;
	}

	if ( send == CONSOLE_KEY_RIGHT )
	{
		if ( m_iCursorPos >= ( int ) strlen( m_szCmdLine ) )
			return true;

		m_iCursorPos++;
		RequestUpdateCursorPos();
		return true;
	}

	if ( send == CONSOLE_KEY_END )
	{
		m_iCursorPos = ( int )strlen( m_szCmdLine );
		RequestUpdateCursorPos();
		return true;
	}

	if ( send == CONSOLE_KEY_HOME )
	{
		m_iCursorPos = 0;
		RequestUpdateCursorPos();
		return true;
	}

	if ( send == CONSOLE_KEY_PRIOR )
	{
		PageUp();
		return true;
	}


	if ( send == CONSOLE_KEY_NEXT )
	{
		PageDown();
		return true;
	}


	if ( send == CONSOLE_KEY_UP )
	{
		m_iCursorPos = GetHistoryPrev( m_szCmdLine );
		RequestUpdateCursorPos();
		return true;
	}

	if ( send == CONSOLE_KEY_DOWN )
	{
		m_iCursorPos = GetHistoryNext( m_szCmdLine );
		RequestUpdateCursorPos();
		return true;
	}


	if ( send == CONSOLE_KEY_TAB )
	{
		NextComplitCmd();
		return true;
	}


	if ( send == CONSOLE_KEY_RETURN )
	{
		ExecuteInput();
		NewLine();
		return true;
	}
	
	return false;
}

//--------------------------------------------------------------------------------------------------------
void EngineConsole::AddToCmdHistory( const char * szString )
{
	int iLineLen = strlen( szString );
	if ( iLineLen > CONSOLE_LINE_SIZE )
	{
		iLineLen = CONSOLE_LINE_SIZE - 1;
	}

	r3dscpy_s( m_szHistory[ m_iCurrentHistory ], iLineLen + 1, szString );
	m_szHistory[ m_iCurrentHistory ][ iLineLen ] = '\0';

	m_iCurrentHistory = ( m_iCurrentHistory == ( CONSOLE_MAX_HISTORY - 1 ) ) ? ( 0 ) : ( m_iCurrentHistory + 1 );
}

//--------------------------------------------------------------------------------------------------------
void EngineConsole::SendChar( uint8_t send )
{
	if ( ! IsVisible() )
		return;

	if ( ( send < ' ' ) || ( send == '`' ) )
		return;

	TString str;
	r3dscpy( str, m_szCmdLine + m_iCursorPos );
	r3dscpy( m_szCmdLine + m_iCursorPos + 1, str );
	m_szCmdLine[ m_iCursorPos ] = send;
	m_iCursorPos++;
	RequestUpdateCursorPos();
}

//--------------------------------------------------------------------------------------------------------
int EngineConsole::GetHistoryPrev( char * szOut )
{
	int i;

	for ( i = 0; i < CONSOLE_MAX_HISTORY; i++ )
	{
		m_iCurrentHistory = ( m_iCurrentHistory == 0 ) ? ( CONSOLE_MAX_HISTORY - 1 ) : ( m_iCurrentHistory - 1 );
		
		if ( m_szHistory[ m_iCurrentHistory ] ) 
			break;
	}

	if ( m_szHistory[ m_iCurrentHistory ] )
	{
		r3dscpy( szOut, m_szHistory[ m_iCurrentHistory ] );
		return strlen( szOut );
	}

	return strlen( szOut );
}

//--------------------------------------------------------------------------------------------------------
int EngineConsole::GetHistoryNext( char * szOut )
{
	int i;

	for ( i = 0; i < CONSOLE_MAX_HISTORY; i++ )
	{
		m_iCurrentHistory = ( m_iCurrentHistory == ( CONSOLE_MAX_HISTORY - 1 ) ) ? ( 0 ) : ( m_iCurrentHistory + 1 );
	
		if ( m_szHistory[ m_iCurrentHistory ] ) 
			break;
	}

	if ( m_szHistory[ m_iCurrentHistory ] )
	{
		r3dscpy( szOut, m_szHistory[ m_iCurrentHistory ] );
		return strlen( szOut );
	}

	return strlen( szOut );
}

//--------------------------------------------------------------------------------------------------------
void EngineConsole::PageUp()
{
	int nHowManyToSubtract = LinesUp();

	m_nFirstVisibleLine -= nHowManyToSubtract;
	m_nLastVisibleLine -= nHowManyToSubtract;

	
/*	if ( m_nLastVisibleLine - m_iCurrentLine > 0 && m_nLastVisibleLine - m_iCurrentLine < m_iPageSize )
		return;

	m_nLastVisibleLine -= m_iPageSize;
*/
	if ( m_nLastVisibleLine < 0 )
	{
		m_nLastVisibleLine = CONSOLE_MAX_LINES - 1;
	}
}

//--------------------------------------------------------------------------------------------------------
void EngineConsole::ResetComplitCmd()
{
	m_nFirstCmd = INVALID_INDEX;
	m_dFoundCmd.Clear();
}

//--------------------------------------------------------------------------------------------------------
void EngineConsole::NextComplitCmd()
{
	if ( m_nFirstCmd == INVALID_INDEX )
	{
		EnumCmd();
	}

	if ( ! m_dFoundCmd.Count() )
		return;

	FixedString s = m_dFoundCmd[ m_nFirstCmd ] + " ";

	r3dscpy_s( m_szCmdLine, sizeof( TString ), s );

	m_iCursorPos = strlen( m_szCmdLine );
	RequestUpdateCursorPos();

	m_nFirstCmd++;

	if ( m_nFirstCmd >= ( int ) m_dFoundCmd.Count() )
	{
		m_nFirstCmd = 0;
	}
}

//--------------------------------------------------------------------------------------------------------
void EngineConsole::EnumCmd()
{
	CmdConsole::EnumCmd( m_szCmdLine );

	if ( m_dFoundCmd.Count() )
	{
		m_nFirstCmd = 0;
	}
}


//--------------------------------------------------------------------------------------------------------
void EngineConsole::FoundCmdCallback( const char * szFullName )
{
	m_dFoundCmd.PushBack( szFullName );
}

//--------------------------------------------------------------------------------------------------------
void EngineConsole::PageDown()
{
	int nHowManyToAdd = LinesDown();

	m_nFirstVisibleLine += nHowManyToAdd;
	m_nLastVisibleLine += nHowManyToAdd;

	/*
	m_nLastVisibleLine += m_iPageSize;
	
	if ( m_nLastVisibleLine > m_iCurrentLine )
	{
		m_nLastVisibleLine = m_iCurrentLine;
	}*/
}

//--------------------------------------------------------------------------------------------------------
int EngineConsole::LinesUp()
{
	int nMinToSub =  r3dTL::Min( CONSOLE_PAGESIZE, m_nFirstVisibleLine - ( m_iCurrentLine - CONSOLE_MAX_LINES + 1 ) );
	return r3dTL::Min( nMinToSub, m_nFirstVisibleLine );	// to avoid getting to negative values
}

//--------------------------------------------------------------------------------------------------------
int EngineConsole::LinesDown()
{
	return r3dTL::Min( CONSOLE_PAGESIZE, m_iCurrentLine - m_nLastVisibleLine );
}

//--------------------------------------------------------------------------------------------------------
void EngineConsole::RequestUpdateCursorPos()
{
	m_bNeedUpdateCursorPos = true;
}

//--------------------------------------------------------------------------------------------------------
void EngineConsole::UpdateCursorPos()
{
	TString buffer;

	memcpy( buffer, m_szCmdLine, m_iCursorPos );
	buffer[ m_iCursorPos ] = '\0';

	/*float fWidthPixels, fHeightPixels;
	GetTextDimensionsAbs ( buffer, fWidthPixels, fHeightPixels );
	m_fCursorPosPix = fWidthPixels * m_nWidth;*/

	SIZE sz1;
	MenuFont_Editor->GetTextExtent( buffer, &sz1);
	m_fCursorPosPix = ( float )sz1.cx;

	UpdateInput( m_szCmdLine );
}
