#include "r3dPCH.h"
#include "r3d.h"

#include "CmdProcessor.h"
#include "CmdConsole.h"

//---------------------------------------------------------------------------------------------
DECLARE_CMD( exec )
{
	if ( ev.NumArgs() < 2 )
	{
		ConPrint( "exec [file_name]" );
		return;
	}

	char concatFileName[ 1024 ] = { 0 };

	for( int i = 1, e = ev.NumArgs(); i < e; i ++ )
	{
		const char * szFileName = ev.GetString( i );
		strcat( concatFileName, szFileName );

		if( i != e - 1 )
			strcat( concatFileName, " " );
	}

	ConPrint( "Reading command file %s...", concatFileName );
	
	r3dFile * hFile = r3d_open( concatFileName, "rb" );
	if ( ! hFile )
	{
		r3dOutToLog( "Can't open file \"%s\"\n", concatFileName );
		return;
	}

	char buff[ 1024 ];
	unsigned char tmpchar;
	int index;
	int nPos = 0;

	fseek( hFile, 0, SEEK_END );
	int nSize = ftell( hFile );
	fseek( hFile, 0, SEEK_SET );

	void * pData = malloc( nSize );
	if(pData == NULL)
		r3dError("Out of memory!");
	fread( pData, nSize, 1, hFile );
	fclose( hFile );

	const BYTE * pD = ( const BYTE * ) pData;
	while ( nPos < nSize )
	{
		index = 0;
		do
		{
			tmpchar = *pD++;
			nPos++;

			if ( ( tmpchar < 0x20 && tmpchar != 0x09 ) || index > 1020 )
				break;

			buff[ index++ ] = tmpchar;		
		} 
		while ( nPos < nSize );

		//buff[ index++ ] = '\n';
		buff[ index ] = '\0';

		if ( index )
			g_pCmdProc->Execute( buff );
	}

	free( (void *)pData );
	ConPrint( "   command file read ok." );
}

//--------------------------------------------------------------------------------------------------------
DECLARE_CMD( list )
{
	if ( ev.NumArgs() == 2 && ! strcmp( ev.GetString( 1 ), "?" ) )
	{
		ConPrint( "list [<substring> [<type> [<flag_mask>]]]" );
		return;
	}

	const char * szSubStr = NULL; 
	const char * szFlagStr = NULL;
	Cmd::CmdType eType = Cmd::eCmdType_None;

	if ( ev.NumArgs() >= 2 )
	{	
		if ( strcmp( ev.GetString( 1 ), "*" ) )
		{
			szSubStr = ev.GetString( 1 );
		}

		if ( ev.NumArgs() >= 3 )
		{
			const char * sType = ev.GetString( 2 );

			if ( ! strcmp( sType, "cvar" ) )
			{	
				eType = Cmd::eCmdType_CVar;
			}
			else if ( ! strcmp( sType, "func" ) )
			{
				eType = Cmd::eCmdType_Func;
			}
			else if ( ! strcmp( sType, "alias" ) )
			{
				eType = Cmd::eCmdType_Alias;
			}
		}

		if ( ev.NumArgs() >= 4 )
		{
			if ( strcmp( ev.GetString( 3 ), "*" ) )
			{
				szFlagStr = ev.GetString( 3 );
			}
		}
	}

	g_pCmdProc->ListCmdToConsole( szSubStr, szFlagStr, eType );
}

//--------------------------------------------------------------------------------------------------------
void RegisterSharedCommands()
{
	REG_CCOMMAND( exec, 0, "Execute file" );
	REG_CCOMMAND( list, 0, "Listing vars, func, aliases" );
}


