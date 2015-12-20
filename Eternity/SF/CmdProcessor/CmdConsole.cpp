#include "r3dPCH.h"

#include "CmdConsole.h"
#include "CmdProcessor.h"

CmdConsole* g_pDefaultConsole = NULL;

//--------------------------------------------------------------------------------------------------------
CmdConsole::CmdConsole()
{
	m_bVisible = false;
	m_pCmdProc = NULL;
}


//--------------------------------------------------------------------------------------------------------
CmdConsole::~CmdConsole()
{
	SetCommandProcessor( NULL );
}

//--------------------------------------------------------------------------------------------------------
void CmdConsole::ExecuteInput()
{
	assert( m_pCmdProc );
	if ( ! m_pCmdProc )
		return;
	
	m_pCmdProc->Execute( m_cmdline );
	AddToCmdHistory( m_cmdline );
}						

//--------------------------------------------------------------------------------------------------------
void CmdConsole::SetCommandProcessor( CommandProcessor * pCmdProc )
{
	if ( m_pCmdProc )
	{
		m_pCmdProc->RemoveConsole( this );
	}

	m_pCmdProc = pCmdProc;

	if ( pCmdProc )
	{
		pCmdProc->SetConsole( this );
	}
}

//--------------------------------------------------------------------------------------------------------
void CmdConsole::AddToHistory( const char * szString )		
{ 
	m_historylog.PushBack( szString );
}

//--------------------------------------------------------------------------------------------------------
void CmdConsole::EnumCmd( const char * szStr )
{
	assert( m_pCmdProc );

	const Cmd * pCmd = m_pCmdProc->GetCmdList();
	int nLen = ( int ) strlen( szStr );

	while ( pCmd )
	{
		if ( ! strncmp( szStr, pCmd->GetName(), nLen ) && pCmd->GetFlags() != Cmd::eCmdType_Alias )
		{
			FoundCmdCallback( pCmd->GetName() );
		}
		
		pCmd = pCmd->GetNext();
	}
}

void ConPrint( const char* fmt, ... )
{
	if( g_pDefaultConsole )
	{
		va_list args;
		va_start(args, fmt);

		char buffer[ 512 ];

		buffer[ 511 ] = 0;

		vsnprintf( buffer, sizeof buffer - 1, fmt, args );

		g_pDefaultConsole->Print( buffer );
	}
}