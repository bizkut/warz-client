#include "r3dPCH.h"
#include "r3d.h"
#include "script.h"

#include "r3dSys_WIN.h"


#include "r3dFileMan.h"

char Script_c::m_szNumbers[] = "0123456789.-";
char delimiters[] = "'\";,";

namespace
{
	struct OverrunChecker
	{
		OverrunChecker( char* end ) : end( end )
		{
			*end = 0;
		}

		~OverrunChecker()
		{
			r3d_assert( *end == 0 );
		}

		char* end;
	};
}

//--------------------------------------------------------------------------------------------------------
// Name: 
// Desc: 
//--------------------------------------------------------------------------------------------------------
bool IsFloat( const char * szText )
{
	unsigned int dwFlags = 0;

	while ( *szText )
	{
		dwFlags &= ~1;
		unsigned char ch = *szText++;

		switch ( ch )
		{
		case	'+'	:
		case	'-'	:
			if ( dwFlags & 2 ) 
				return false;
			dwFlags |= 2;
			continue;

		case	'.'	:
		case	','	:
			if ( dwFlags & 4 ) 
				return false;
			dwFlags |= 4;
			continue;

		default	:
			dwFlags |= 1;
		}
		
		if ( dwFlags & 1 )
		{
			if ( ( ch > '9' ) || ( ch < '0' ) ) 
				return false;
		}
	}

	return true;
}


//---------------------------------------------------------------------------------------------
// Name: 
// Desc: 
//---------------------------------------------------------------------------------------------
Script_c::~Script_c()	
{ 
	if ( m_pStoredFile ) 
	{ 
		r3dOutToLog( "WARNING: current script \"%s\" need called Script_c::CloseFile()", m_szFileName );
	}
}


//---------------------------------------------------------------------------------------------
// Name: bool OpenFile( const char * szFileName )
// Desc: Open script from file
//---------------------------------------------------------------------------------------------
bool Script_c::OpenFile( const char * szFileName, bool as_binary )
{
	unsigned int dwFileLen;

	r3dscpy( m_szFileName, szFileName);
	
	r3dFile * hFile = r3d_open( szFileName, as_binary?"rb":"rt" );
	if ( ! hFile )
		return false;


	m_pStoredFile = game_new char[ hFile->size + 1 ];

	dwFileLen = fread( m_pStoredFile, 1, hFile->size, hFile );
	m_pStoredFile[ dwFileLen ] = '\0';
		
	OpenMemoryLocation( m_pStoredFile, dwFileLen );

	fclose( hFile );

	return true;
}


//---------------------------------------------------------------------------------------------
// Name: 
// Desc: Actually sets up the script for reading
//---------------------------------------------------------------------------------------------
void Script_c::OpenMemoryLocation( const char * pData, int iSize )
{
	m_pMemoryLocation = pData;
	m_pStartPos		= pData;
	m_iBytesTotal	= iSize;
	m_bEOF			= false;
	m_iLineNumber	= 0;
	m_iBytesRead	= 0;

	if ( ! m_pStoredFile )
	{
//		sprintf( m_szFileName, "Memory mapped: 0x%08X, size: %d", (size_t)pData, iSize );
		sprintf( m_szFileName, "Memory mapped: %p, size: %d", pData, iSize ); // Intel Compiler version
	}
}


//---------------------------------------------------------------------------------------------
// Name: void CloseFile()
// Desc: Do not call this if you open script from memory! ( TODO : checks for opening from memeory )
//---------------------------------------------------------------------------------------------
void Script_c::CloseFile()
{
	SAFE_DELETE_ARRAY( m_pStoredFile );
}


//---------------------------------------------------------------------------------------------
// Name: void GetToken( char * szToken, bool ignore_delimiters = false )
// Desc: Retrieves next token
//---------------------------------------------------------------------------------------------
void Script_c::GetToken( char * szToken, bool ignore_delimiters )
{
	char	temp[ MAX_PATH ];
	char	ch;
	int		iPos = 0;

	OverrunChecker checkOverruns( temp + sizeof temp - 1 );

	szToken[ 0 ] = '\0';

	do
	{
		m_bFindToken = false;
		iPos = 0;

		do
		{
			Read( &ch, 1 );
			temp[ iPos++ ] = ch;

			if ( ( ignore_delimiters && strchr( delimiters, ch )) || ch <= ' ' )
			{
				if ( iPos > 1 )
				{
					// CAUTION: can destroy something
					m_iBytesRead --;	// rollback for a while
					break;
				}
				else
				{
					iPos--;
				}
			}
			else	
			{
				m_bFindToken = true;
			}
		} 
		while ( ! m_bEOF && ( ! m_bFindToken || ( ch != ' ' ) ) );

		if ( iPos <= 0 )
		{
			iPos = 1;
		}
		temp[ --iPos ] = 0;

		if ( strstr( temp, "//" ) != temp )
		{
			r3dscpy( szToken, temp );
			return;
		}
		else
		{
			if ( ch != 0x0a )
			{
				do
				{
					Read( &ch, 1 );
				} 
				while ( ! m_bEOF && ( ch != 0x0a ) );
			}
		}
	} 
	while ( ! m_bEOF );

}


//---------------------------------------------------------------------------------------------
// Name: bool RewindAfterToken( const char * szToken, bool from_start = true, bool whole_word = true )
// Desc: Rewinds from beginning to first occurence of the specific token
// Returns false if no token exists (maintains its own copy of the file since it may not be zero-ending string)
//---------------------------------------------------------------------------------------------
bool Script_c::RewindAfterToken( const char * szToken, bool from_start, bool whole_word )
{
	char tempToken[ MAX_PATH ];

	OverrunChecker checkOverruns( tempToken + sizeof tempToken - 1 );

	int		store_BytesRead = m_iBytesRead;
	bool	store_EOF = m_bEOF;
	bool	bFoundIt = false;

	if ( from_start )
	{
		// rewind to start
		m_iBytesRead = 0;
		m_bEOF = false;
	}

	if ( whole_word )
	{
		while( ! m_bEOF && ! bFoundIt )
		{
			GetToken( tempToken );
			if ( ! strcmp( tempToken, szToken ) )
			{
				bFoundIt = true;
			}
		}
	}
	else
	{
		const char * substr = strstr( m_pStartPos + m_iBytesRead, szToken );
		if ( substr )
		{
			bFoundIt = true;
			m_iBytesRead = substr - m_pStartPos;
			assert( m_iBytesRead < m_iBytesTotal );
			GetToken( tempToken );
		}
	}

	if ( ! bFoundIt )
	{
		m_iBytesRead = store_BytesRead;
		m_bEOF = store_EOF;
	}

	return bFoundIt;
}


//---------------------------------------------------------------------------------------------
// Name: void SkipToken( const char * szToken )
// Desc: Skip token
//---------------------------------------------------------------------------------------------
void Script_c::SkipToken( const char * szToken )
{
	char tk[ MAX_PATH ];

	OverrunChecker checkOverruns( tk + sizeof tk - 1 );

	GetToken( tk );
	if ( strcmp( tk, szToken ) )
	{
		ErrorAt( "Expecting token", szToken );
	}
}


//---------------------------------------------------------------------------------------------
// Name: void GetSpecific( char * pData )
// Desc: Get specific token between spaces
//---------------------------------------------------------------------------------------------
void Script_c::GetSpecific( char * pData )
{
	bool bStart = false;

	do
	{
		Read( pData, 1 );

		if ( m_bEOF ) 
			break;

		if ( * pData <= ' ' )
		{
			if ( bStart )
			{
				* pData = '\0';
				break;
			}
		}

		if ( * pData > ' ' )
		{
			bStart = true;
			pData++;
		}

	} 
	while ( ! m_bEOF );
}


//---------------------------------------------------------------------------------------------
// Name: void GetString( char * wheretostore, int nMaxSize, char chClamp = '"' )
// Desc: Get string into wheretostore between chClamp chars
//---------------------------------------------------------------------------------------------
void Script_c::GetString( char * wheretostore, int nMaxSize, char chClamp ) // fixme: add max size
{
	bool bComment = false;
	int nReaded = 0;
	// returns string between '"' and '"'
	
	do // skipping the spaces
	{
		Read( wheretostore, 1 );

		if ( * wheretostore == '/' )
		{
			if ( bComment )
			{
				do
				{
					Read( wheretostore, 1 );
				} 
				while ( ! m_bEOF && ( * wheretostore != 0x0a ) );
			}
			else
			{
				bComment = true;
			}
		}
	}
	while( ! m_bEOF && ( * wheretostore <= ' ' || * wheretostore == '/' ) );

	if ( * wheretostore != chClamp )
	{
		ErrorAt( "Expecting token", "\"" );
		* wheretostore = 0x0;
		return;
	}

	char tmp;

	do
	{
		Read( wheretostore, 1 );
		tmp = * wheretostore;
		if ( tmp == chClamp ) 
			break;

		if ( nReaded >= nMaxSize )
		{
			r3d_assert( false );
			//D_WARNING( Va( "Script_c::GetString : String to big \"%s\" (script: \"%s\")", wheretostore, m_szFileName ) );
			break;
		}
		nReaded++;
		wheretostore++;	
		
	}
	while( ! m_bEOF );
	
	* wheretostore = 0x0;
}


//---------------------------------------------------------------------------------------------
// Name: void GetLine( char * wheretostore, int max_size )
// Desc: Gets line
//---------------------------------------------------------------------------------------------
void Script_c::GetLine( char * wheretostore, int max_size )
{
	do // skipping the spaces
	{
		Read( wheretostore, 1 );
		
		if ( * wheretostore == 0x0D || * wheretostore == 0x0A  )
		{
			* wheretostore = 0x0;
			return;
		}
	}
	while( ! m_bEOF && ( * wheretostore == ' ' ||  *wheretostore == '\t' || * wheretostore == 0x0A ) );

	wheretostore++;
	max_size--;

	char tmp;
	do
	{
		Read( &tmp, 1 );
		if ( tmp == '\n' || tmp == 0 ) 
			break;

		* wheretostore = tmp;
		wheretostore++;	
		max_size--;
	}
	while( ! m_bEOF && max_size > 1 );
	
	* wheretostore = 0x0;
}


//---------------------------------------------------------------------------------------------
// Name: int GetInt( const char * szToken )
// Desc: Get integer value
//---------------------------------------------------------------------------------------------
int Script_c::GetInt( const char * szToken )
{
	char tk[ MAX_PATH ];
	bool bFind = false;

	OverrunChecker checkOverruns( tk + sizeof tk - 1 );

	while ( ! m_bEOF && ! bFind )
	{
		GetToken( tk );
		if ( ! strcmp( tk, szToken ) )
		{
			GetSpecific( tk );
			bFind = true;
		}
		else
		{
			ErrorAt( "Expecting token", szToken );
		}
	}

	return atoi( tk );
}


//---------------------------------------------------------------------------------------------
// Name: int GetInt()
// Desc: Get integer value
//---------------------------------------------------------------------------------------------
int Script_c::GetInt()
{
	char tmp[ MAX_PATH ];
	memset( tmp, 0, MAX_PATH );

	OverrunChecker checkOverruns( tmp + sizeof tmp - 1 );

	char * curnum = tmp;
	
	while( ! m_bEOF && ! strchr( m_szNumbers, Peek() ) )
	{
		Read( curnum, 1 );
	}

	do
	{
		Read( curnum, 1 );
		curnum++;
	}
	while( ! m_bEOF && strchr( m_szNumbers, Peek() ) );

	return ( int ) atoi( tmp );
}


//---------------------------------------------------------------------------------------------
// Name: float GetFloat( const char * szToken )
// Desc: Get float value
//---------------------------------------------------------------------------------------------
float Script_c::GetFloat( const char * szToken )
{
	char tk[ MAX_PATH ];
	bool bFind = false;

	OverrunChecker checkOverruns( tk + sizeof tk - 1 );

	while ( ! m_bEOF && ! bFind )
	{
		GetToken( tk );
		if ( ! strcmp( tk, szToken ) )
		{
			GetSpecific( tk );
			bFind = true;
		}
		else
		{
			ErrorAt( "Expecting token", szToken );
		}
	}

	return ( float ) atof( tk );
}


//---------------------------------------------------------------------------------------------
// Name: float GetFloat()
// Desc: Get float value
//---------------------------------------------------------------------------------------------
float Script_c::GetFloat()
{
	char tmp[ MAX_PATH ];
	memset( tmp, 0, MAX_PATH );

	OverrunChecker checkOverruns( tmp + sizeof tmp - 1 );

	GetToken( tmp );
	float fVal = ( float ) atof( tmp );

	if ( fVal == 0.0f && ! IsFloat( tmp ) )
	{
		ErrorAt( "Expecting token", tmp );
	}

	return fVal;
}


//---------------------------------------------------------------------------------------------
// Name: bool Read( char * pBuffer, int iSize )
// Desc: Reads the sequence from memory
//---------------------------------------------------------------------------------------------
bool Script_c::Read( char * pBuffer, int iSize )
{
	// returns true if EOF
	if ( m_iBytesRead + iSize > m_iBytesTotal )
	{
		iSize = m_iBytesTotal - m_iBytesRead;
		m_bEOF = true;
	}

	if ( iSize > 0 )
	{
		memcpy( pBuffer, m_pStartPos + m_iBytesRead, iSize );
		m_iBytesRead += iSize;

		// find new line code
		for ( int i = 0; i < iSize; i++ )
		{
			if ( * ( pBuffer + i ) == '\n' )
			{
				m_iLineNumber++;
			}
		}
	}
	else
	{
		* pBuffer = 0x00;
	}

	return m_bEOF;
}


//---------------------------------------------------------------------------------------------
// Name: 
// Desc: Print error string
//---------------------------------------------------------------------------------------------
void Script_c::ErrorAt( const char * pErrorName, const char * szToken )
{
	if ( m_pStoredFile )
	{
		r3dOutToLog( "Script_c::ErrorAt : %s : %s : in line %d, filed script %s", pErrorName, szToken, m_iLineNumber, m_szFileName );
	}
	else
	{
		r3dOutToLog( "Script_c::ErrorAt : %s : %s : in line %d, memory script :\n %s", pErrorName, szToken, m_iLineNumber, m_pMemoryLocation );
	}

	r3d_assert( false );
}



///////////////////////////////////////////////////////////////////////////////////////////////
//
//	>	EXTENSION
//
///////////////////////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------------------------
// Name: 
// Desc: 
//---------------------------------------------------------------------------------------------
void Script_c::SetPosition( int pos )
{
	if ( pos > m_iBytesTotal )
	{
		m_iBytesRead = m_iBytesTotal;
		m_bEOF = true;
	}
	else
	{
		m_iBytesRead = pos;
	}
}


//---------------------------------------------------------------------------------------------
// Name: 
// Desc: 
//---------------------------------------------------------------------------------------------
void Script_c::GetDump( char * pBuffer, int start, int end )
{
	assert( end < m_iBytesTotal && end > start );
	memcpy( pBuffer, m_pStartPos + start, end - start );
	m_iBytesRead = end;
}