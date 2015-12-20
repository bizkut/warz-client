#include "r3dPCH.h"
//#include "r3dSys_WIN.h"
#include "r3d.h"

#include "CmdProcessor.h"
#include "CmdConsole.h"

// PunkBuster SDK
#ifdef __WITH_PB__
#include "../../../External/PunkBuster/pbcommon.h"
#endif


// define this if you want the aliases to be inserted as several lines
// (slow, but should 100% work in all cases)
//#define SLOW_MULTILINE_ALIASES

#ifdef SLOW_MULTILINE_ALIASES
#define SEPARATE_METHOD			SEP_PARENTHESIS
#else
#define SEPARATE_METHOD			SEP_SEMICOLON
#endif // SLOW_MULTILINE_ALIASES


CommandProcessor * g_pCmdProc = NULL;
		

void RegisterSharedCommands();


//////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	>	
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////

//--------------------------------------------------------------------------------------------------------
Cmd::Cmd( Cmd * pNext, const char * szName, DWORD dwFlags, const char * szHelp )
	: m_eType( eCmdType_None )
	, m_sName( szName )
	, m_pFunc( NULL )
	, m_pNext( pNext )
	, m_dwFlags( dwFlags )
	, m_szHelp( szHelp )
	, m_nAliasSize( 0 )
{
}

//--------------------------------------------------------------------------------------------------------
Cmd::~Cmd()
{
	if ( m_eType == eCmdType_Alias )
		free( m_szAlias );
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	>	
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////

//--------------------------------------------------------------------------------------------------------
CommandProcessor::CommandProcessor()
	: m_pConsole( NULL )
{
	m_pCommands = NULL;
	
	m_iAliasCount = 0;

	m_FrameCommands.m_dwSize = 0;
	m_FrameCommands.m_szText[ 0 ] = 0x0;
}


//--------------------------------------------------------------------------------------------------------
CommandProcessor::~CommandProcessor()
{	
}

//--------------------------------------------------------------------------------------------------------
void CommandProcessor::Init()
{
	RegisterSharedCommands();
}


//--------------------------------------------------------------------------------------------------------
void CommandProcessor::Release()
{
//	if ( m_pConsole ) 
//		m_pConsole->SetCommandProcessor( NULL );


	Cmd *pNext, *pCmd;
	for ( pCmd = m_pCommands; pCmd; pCmd = pNext )
	{
		pNext = pCmd->GetNext();

		if(pCmd->m_eType == Cmd::eCmdType_CVar)
			SAFE_DELETE(pCmd->m_pVar);
		SAFE_DELETE( pCmd );
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	>	
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////
//--------------------------------------------------------------------------------------------------------
CmdVar * CommandProcessor::FindVar( const char * szName )
{
	FixedString s( szName );
	s.ToLower();

	Cmd * pCmd = m_tCmdHash.GetObject( s );
	if ( ! pCmd )
		return NULL;

	if ( pCmd->m_eType != Cmd::eCmdType_CVar )
		return NULL;
	
	return pCmd->m_pVar;
}

//--------------------------------------------------------------------------------------------------------
CmdVar * CommandProcessor::Register( const char * szName, int nVal, DWORD dwFlags )
{
	CmdVar * pVar = FindVar( szName );

	if ( pVar ) 
		return pVar;

	pVar = game_new CmdVar( szName, nVal, dwFlags );

	AddVar( pVar );
	return pVar;
}

//--------------------------------------------------------------------------------------------------------
CmdVar * CommandProcessor::Register( const char * szName, float fVal, DWORD dwFlags )
{
	CmdVar * pVar = FindVar( szName );

	if ( pVar ) 
		return pVar;

	pVar = game_new CmdVar( szName, fVal, dwFlags );

	AddVar( pVar );
	return pVar;
}

//--------------------------------------------------------------------------------------------------------
CmdVar * CommandProcessor::Register( const char * szName, const char * szVal, DWORD dwFlags )
{
	CmdVar * pVar = FindVar( szName );

	if ( pVar ) 
		return pVar;

	pVar = game_new CmdVar( szName, szVal, dwFlags );

	AddVar( pVar );
	return pVar;
}

//------------------------------------------------------------------------

CmdVar * CommandProcessor::Register( const char * szName, float fVal, float fMinVal, float fMaxVal, DWORD dwFlags )
{
	CmdVar * pVar = FindVar( szName );

	if ( pVar ) 
		return pVar;

	pVar = game_new CmdVar( szName, fVal, fMinVal, fMaxVal, dwFlags );

	AddVar( pVar );
	return pVar;
}

//--------------------------------------------------------------------------------------------------------
Cmd * CommandProcessor::AddCmd( const char * szName, DWORD dwFlags, const char * szHelp )
{
	assert( szName );

	FixedString s( szName );
	s.ToLower();

	if ( m_tCmdHash.IsExists( s ) )
	{
		r3dOutToLog( "CommandProcessor::AddCmd : Warning : '%s' is already registered", szName );
		return NULL;
	}

	Cmd * pNew = game_new Cmd( m_pCommands, szName, dwFlags, szHelp );

	m_tCmdHash.Add( s, pNew );
	m_pCommands = pNew;

	return pNew;
}

//--------------------------------------------------------------------------------------------------------
void CommandProcessor::AddVar( CmdVar * pVar )
{
	assert( pVar && pVar->GetName() );

	Cmd * pNew = AddCmd( pVar->GetName(), pVar->GetFlags(), NULL );
	if ( ! pNew )
		return;
	
	pNew->m_eType = Cmd::eCmdType_CVar;
	pNew->m_pVar = pVar;
}

//--------------------------------------------------------------------------------------------------------
void CommandProcessor::AddCommand( const char * szCmdName, CmdFunc_t * pHandler, DWORD dwFlags, const char * szHelp )
{
	Cmd * pNew = AddCmd( szCmdName, dwFlags, szHelp );
	if ( ! pNew )
		return;
	
	pNew->m_eType = Cmd::eCmdType_Func;
	pNew->m_pFunc = pHandler;
}

//--------------------------------------------------------------------------------------------------------
bool CommandProcessor::DelCmd( Cmd * pCmd )
{
	assert( pCmd && pCmd->GetName() );

	Cmd * pFind = m_pCommands;
	Cmd * pPrev = NULL;

	static char szLower[ CMD_MAX_COMMANDLINE ];
	

	while ( pFind )
	{
		if ( pFind == pCmd )
		{
			assert( ( pPrev == NULL ) || ( pPrev->GetNext() == pCmd ) );

			FixedString s( pCmd->GetName() );
			s.ToLower();

			m_tCmdHash.Remove( s );

			if ( pPrev ) 
			{
				pPrev->ExcludeNext();
			}
			else
			{
				assert( m_pCommands == pFind );
				m_pCommands = m_pCommands->GetNext();
			}

			SAFE_DELETE( pCmd );
			return true;
		}

		pPrev = pFind;
		pFind = pFind->GetNext();
	}

	return false;
}

//--------------------------------------------------------------------------------------------------------
void CommandProcessor::DelVar( CmdVar * pVar )
{
	assert( pVar && pVar->GetName() );

	FixedString s( pVar->GetName() );
	s.ToLower();

	Cmd * pCmd = m_tCmdHash.GetObject( s );
	if ( ! pCmd )
		return;

	if ( pCmd->m_eType != Cmd::eCmdType_CVar || pVar != pCmd->m_pVar )
		return;
	
	assert( strcmp( pVar->GetName(), pCmd->GetName() ) == 0 );

	if ( ! DelCmd( pCmd ) )
	{
		assert( false );
		return;
	}

	r3dOutToLog( "Var %s deleted from command proccessor", pVar->GetName() );
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	>	Exec
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////

//--------------------------------------------------------------------------------------------------------
void CommandProcessor::Execute( const char * szString, ExecPrior_e ePrior )
{
	switch ( ePrior )
	{
		case eExecPrior_Immediate:
			ExecuteLine( szString );
			break;

		case eExecPrior_Insert:
			InsertLine( szString );
			break;

		case eExecPrior_Append:
			AppendLine( szString );
			break;

		default:
			r3dOutToLog( "CommandProcessor::Execute : Bad priority (%d)", ePrior );
			assert( false );
			break;
	}
}


//--------------------------------------------------------------------------------------------------------
void CommandProcessor::ExecuteLine( const char * szLine, DWORD dwFlags )
{
	VarsContainer_c vc;

#ifdef __WITH_PB__
#ifndef WO_SERVER
	// Handle PunkBuster Commands
	if ( ( strnicmp ( szLine, "/pb_", 4) == 0 ) || ( strnicmp ( szLine, "\\pb_", 4) == 0 ) )
	{
		PbClAddEvent ( PB_EV_CMD, strlen ( szLine )-1, (char*)szLine+1 ) ;
		return ;
	}
#endif
#endif

	vc.Tokenize( szLine );

	if ( ! vc.NumArgs() )
		return;	

	FixedString token( vc.GetString( 0 ) );
	token.ToLower();
 
	if ( strstr( token.c_str(), "//" ) == token.c_str() )
		return;		// skip comment

	Cmd * pCmd = m_tCmdHash.GetObject( token );
	if ( ! pCmd )
	{
		// empty command is chat
		//ExecuteLine( Va( "say %s", szLine ) ); // va need fix

		if ( m_pConsole )
			m_pConsole->Print( Va( "say %s", szLine ) );
		return;
	}


//	r3dOutToLog( "CommandProcessor::ExecuteLine : exec \"%s\"", szLine );

	
	if ( pCmd->GetFlags() & VF_FORCEECHO )
	{
		// echoed to console
		m_pConsole->Print( szLine );
	}

	if ( pCmd->GetFlags() & VF_CONST )
	{
		// echoed to console
		m_pConsole->Print( szLine );
		return;
	}

	// execute the command
	switch ( pCmd->m_eType )
	{
		case Cmd::eCmdType_Func:
			pCmd->m_pFunc( vc );
			break;

		case Cmd::eCmdType_CVar:
			if ( vc.NumArgs() > 1 )
			{
				if( const char* start = strstr( szLine, vc.GetString( 0 ) ) )
				{
					start += strlen( vc.GetString( 0 ) ) ;

					for( ; *start == ' ' || *start == '\t' ; ) start ++ ;
					char buff[ 8192 ] ;

					strcpy_s( buff, start ) ;

					int i = strlen( buff ) ;
					for( ; i >= 0 ; i -- ) 
					{
						if( buff[ i ] == ' ' || buff[ i ] == '\t' || buff[ i ] == '\n' || buff[ i ] == '\r' )
							buff[ i ] = 0 ;
						else
							break ;
					}

					pCmd->m_pVar->SetString( buff ) ;
				}
				else
				{
					pCmd->m_pVar->SetString( vc.GetString( 1 ) );
				}
			}
			if ( m_pConsole )
				m_pConsole->Print( Va( "\"%s\" is \"%s\"", pCmd->m_pVar->GetName(), pCmd->m_pVar->GetString() ) );
			break;

		case Cmd::eCmdType_Alias:
			if ( m_iAliasCount >= CMD_MAX_ALIASLOOP )
			{
				r3dOutToLog( "CommandProcessor::ExecuteLine : possible alias loop, stopping execution" );
				return;
			}

#ifdef SLOW_MULTILINE_ALIASES
			{
				// reusing vc!
				vc.ClearParams();
				vc.Tokenize( pCmd->m_szAlias );
				for ( int i = 0; i < vc.NumArgs(); i ++ )
				{
					InsertLine( Va( "%s\n", vc.GetString( i ) ) );
				};
			};		
#else
			InsertLine( pCmd->m_szAlias );

#endif // SLOW_MULTILINE_ALIASES
			break;

		case Cmd::eCmdType_None:
		default:		
			r3dOutToLog( "CommandProcessor::ExecuteLine() : bad command type" );
			assert( false );
			break;
	}

}

//--------------------------------------------------------------------------------------------------------
void CommandProcessor::InsertLine( const char * szLine, CTextBuf * pBuffer )
{
	if ( !pBuffer )
	{
		pBuffer = &m_FrameCommands;
	}

	DWORD dwLine = strlen( szLine ) + 1;

	if ( dwLine == 1 )
		return;		// no text

	if ( pBuffer->m_dwSize + dwLine + 5 > CMD_BUFSIZE )
	{
		r3dOutToLog( "CommandProcessor::InsertLine : Execution buffer overflow" );
		assert( false );
		return;
	}

	// not moving the buffer since it is not cache-friendly
	
	char * pTemp = ( char * ) malloc( pBuffer->m_dwSize + 1 );
	if(pTemp==NULL)
		r3dError("Out of memory!");

	memcpy( pTemp, pBuffer->m_szText, pBuffer->m_dwSize + 1 );
	pBuffer->m_szText[ 0 ] = 0x0;
	pBuffer->m_dwSize = 0;

	AppendLine( szLine, pBuffer );
	AppendLine( pTemp, pBuffer );

	free( pTemp );
}

//--------------------------------------------------------------------------------------------------------
void CommandProcessor::AppendLine( const char * szLine, CTextBuf * pBuffer )
{
	if ( ! pBuffer )
	{
		pBuffer = &m_FrameCommands;
	}

	DWORD dwLine = strlen( szLine );

	if ( dwLine == 0 )
		return;		// no text

	if ( pBuffer->m_dwSize + dwLine + 5 > CMD_BUFSIZE )
	{
		r3dOutToLog( "CommandProcessor::InsertLine : Execution buffer overflow" );
		assert( false );
		return;
	}

	strcat( pBuffer->m_szText, szLine );
	pBuffer->m_dwSize += dwLine;
	strcat( pBuffer->m_szText, "\n" );
	pBuffer->m_dwSize++;
}

//--------------------------------------------------------------------------------------------------------
void CommandProcessor::FlushBuffer()
{
	char szLine[ CMD_MAX_COMMANDLINE ];
	m_iAliasCount = 0;
	
	while ( m_FrameCommands.m_dwSize )
	{
		char * pText = m_FrameCommands.m_szText;
		int nQuotes = 0;
		int nParenthesis = 0;
		DWORD i;

		for ( i = 0; i < m_FrameCommands.m_dwSize; i ++ )
		{
			assert( pText[ i ] != '\0' );
			
			if ( pText[ i ] == '"' )
				nQuotes ++;

			if ( pText[ i ] == '(' )
				nParenthesis ++;

			if ( pText[ i ] == ')' )
				nParenthesis --;

			if ( pText[ i ] == ';' && !( nQuotes & 1 ) && !nParenthesis )
				break;

			if ( pText[ i ] == '\n' || pText[ i ] == 0x0A )
				break;

			if ( i >= CMD_MAX_COMMANDLINE )
				break;
		}

		memcpy( szLine, pText, i );
		szLine[ i ] = '\0';

		// since some commands can insert text in the beginning of the buffer,
		// it is necessary to delete current command from it first

		if ( i == m_FrameCommands.m_dwSize )
		{
			m_FrameCommands.m_dwSize = 0;
			m_FrameCommands.m_szText[ 0 ] = '\0';
		}
		else
		{
			i++;
			m_FrameCommands.m_dwSize -= i;
			memmove( pText, pText + i, m_FrameCommands.m_dwSize + 1 );
		}

		if ( szLine[ 0 ] )
		{
			//r3dOutToLog( "CommandProcessor::FlushBuffer: Executing line %s\n", szLine );
			ExecuteLine( szLine );
		}
	}
}
//------------------------------------------------------------------------

void CommandProcessor::SaveVars( const char * szFileName )
{
	FILE* f = fopen_for_write( szFileName, "w" );

	if( !f )
	{
		r3dOutToLog( "Couldn't open %s for writing!\n", szFileName );
		return;
	}

	for( Cmd* cmd = m_pCommands; cmd; cmd = cmd->GetNext() )
	{
		if( cmd->m_eType == Cmd::eCmdType_CVar && cmd->m_pVar->GetFlags() & VF_SAVE )
		{
			fprintf( f, "%s %s\n", cmd->m_pVar->GetName(), cmd->m_pVar->GetString() );
		}
	}

	fclose( f );
}

//--------------------------------------------------------------------------------------------------------
const char * ParseCmdFlags( int flags )
{
	static char szCmdFlags[ 32 ];

	int flagNum = 0;

	// hidden flag is skipped

	szCmdFlags[ flagNum++ ] = ( flags & VF_CONST ) ? 'C' : '-';
	szCmdFlags[ flagNum++ ] = ( flags & VF_NEVERDEFER ) ? 'N' : '-';
	szCmdFlags[ flagNum++ ] = ( flags & VF_FORCEECHO ) ? 'E' : '-';
	szCmdFlags[ flagNum++ ] = 0;

	return szCmdFlags;
}

//--------------------------------------------------------------------------------------------------------
int ParseMask( const char *mask )
{
	int flags = 0;

	// hidden flag is skipped
	flags |= strstr( mask, "C" ) ? VF_CONST : 0;
	flags |= strstr( mask, "N" ) ? VF_NEVERDEFER : 0;
	flags |= strstr( mask, "E" ) ? VF_FORCEECHO : 0;

	return flags;
}


#define FORMAT_CON_LINE		"%-46s%-10s%-10s"

const char * g_szCmdTypes[] =
{
	"empty",
	"cvar",
	"func",
	"alias",
	"setalias"
};

//--------------------------------------------------------------------------------------------------------
void CommandProcessor::ListCmdToConsole( const char * szSubStr, const char * szFlagMask, Cmd::CmdType eType )
{
	const char	szSepar[] = "----------------------------------------------------------------";
	char	buffer[ 256 ];
	Cmd *	pEntry = m_pCommands;	
	int		mask = ( szFlagMask ) ? ParseMask( szFlagMask ) : -1;
	int		nFound = 0;


	sprintf( buffer, FORMAT_CON_LINE, "Name:", "Type", "Flags:" );
	m_pConsole->Print( "" );
	m_pConsole->Print( buffer );
	m_pConsole->Print( szSepar );


	for ( ; pEntry; pEntry = pEntry->GetNext() )
	{
		if ( pEntry->GetFlags() & VF_HIDDEN ) // never show hidden commands
			continue;

		if ( szSubStr && ! strstr( pEntry->GetName(), szSubStr ) ) // filter by substr (if any)
			continue;

		if ( ( ( eType == Cmd::eCmdType_None ) || ( pEntry->m_eType == eType ) ) 
			&& ( ! pEntry->GetFlags() || ( pEntry->GetFlags() & mask ) ) )
		{
			sprintf( buffer, FORMAT_CON_LINE, pEntry->GetName(), g_szCmdTypes[ pEntry->m_eType ], ParseCmdFlags( pEntry->GetFlags() ) );
			m_pConsole->Print( buffer );
			nFound++;
		}
	}

	m_pConsole->Print( szSepar );
	sprintf( buffer, "Total: %d", nFound );
	m_pConsole->Print( buffer );
}

//--------------------------------------------------------------------------------------------------------
void CommandProcessor::Print ( const char * szString )
{
	if ( m_pConsole )
		m_pConsole->Print ( szString ) ;
}