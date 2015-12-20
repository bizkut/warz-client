#include "r3dPCH.h"
#include "r3d.h"
#include "r3dFileTracker.h"

#include "Tsg_stl/HashTable.h"

//////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	>	
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////


static HANDLE		g_hFileTrackHandle = INVALID_HANDLE_VALUE;
static FixedString		g_sFileTrackPath;
static OVERLAPPED	g_FileTrackOvStruct;
static BYTE		g_FileTrackBuff[4096];

static CallbackFileChange_t * g_pFileTrackCallBack = NULL;

struct FileTrackNameHashFunc_T
{
	inline int operator () ( const char * szKey )
	{
		return r3dHash::MakeHash( szKey );// & ( LENGTH-1 );
	}
};
HashTableDynamic< float, FixedString, FileTrackNameHashFunc_T, 256 >		m_tFileTrackHash;



//--------------------------------------------------------------------------------------------------------
FixedString FileTrackWCharToString ( const WCHAR* wszIn, int iLen )
{
	FixedString s;
	char prebuf[2];
	prebuf[1] = 0;
	for( int i = 0; i < (iLen/2); i++ )
	{
		prebuf[0] = (char)wszIn[ i ];
		if ( wszIn[ i ] <= 255 ) 
			s += prebuf;
	}
	return s.c_str();
}

//--------------------------------------------------------------------------------------------------------
void FileTrackReInit()
{
	DWORD bufsize = sizeof( g_FileTrackBuff );
	DWORD readed = 0;
	::ResetEvent ( g_FileTrackOvStruct.hEvent );
	memset( g_FileTrackBuff, 0, bufsize );
	::ReadDirectoryChangesW( g_hFileTrackHandle , (void*)&g_FileTrackBuff, bufsize-2, true, FILE_NOTIFY_CHANGE_LAST_WRITE | FILE_NOTIFY_CHANGE_FILE_NAME, &readed, &g_FileTrackOvStruct, 0 );
}


//--------------------------------------------------------------------------------------------------------
void FileTrackShutdown()
{
	if ( g_hFileTrackHandle == INVALID_HANDLE_VALUE )
		return;
	
	::CloseHandle ( g_hFileTrackHandle );
	g_hFileTrackHandle = INVALID_HANDLE_VALUE;
	::CloseHandle ( g_FileTrackOvStruct.hEvent );
	g_FileTrackOvStruct.hEvent = INVALID_HANDLE_VALUE;
}

//--------------------------------------------------------------------------------------------------------
void FileTrackDoWork()
{
#ifndef FINAL_BUILD
/*	if( !r_track_file_changes->GetInt() )
		return ;

	if ( g_hFileTrackHandle == INVALID_HANDLE_VALUE )
		return;

	DWORD cnt;
	bool bResult = !!::GetOverlappedResult( g_hFileTrackHandle, &g_FileTrackOvStruct,	&cnt, 0 );
	if ( bResult )
	{
		FILE_NOTIFY_INFORMATION * pBuff = ( FILE_NOTIFY_INFORMATION* ) g_FileTrackBuff;
		while (pBuff)
		{
			FixedString sShortFileName = FileTrackWCharToString ( pBuff->FileName, pBuff->FileNameLength );
			FixedString sFileName = g_sFileTrackPath + "\\" + sShortFileName;
			FixedString sFileExt = sFileName.GetExt();

			FILE * hFile = fopen( sFileName, "r" );
			bool bSkip = hFile == NULL;
			if ( hFile )
				fclose( hFile );
			
			if(stricmp(sShortFileName.c_str(), "r3dlog.txt") == 0)
				bSkip = true;

			if ( ( ( pBuff->Action == FILE_ACTION_MODIFIED )||( pBuff->Action == FILE_ACTION_RENAMED_NEW_NAME ) )&& ( !bSkip ) )
			{
				int nTestCount = 500;
				HANDLE hFile = INVALID_HANDLE_VALUE;
				while( hFile == INVALID_HANDLE_VALUE && nTestCount-- )
				{
					hFile = ::CreateFile ( sFileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN, NULL);
					::Sleep(10);
				}

				if ( nTestCount < 490 )
					r3dOutToLog ( "\n>>>FileTracker: Trying to refresh [%s]( %d lock-test failed ).\n ", sFileName.c_str(), 500 - nTestCount );

				if ( hFile != INVALID_HANDLE_VALUE )
				{
					::CloseHandle( hFile );
					m_tFileTrackHash.Add( sFileName, r3dGetTime() );
				}
			}

			if( pBuff->NextEntryOffset == 0 ) 
				pBuff = 0;
			else
				pBuff = (FILE_NOTIFY_INFORMATION *)(((char*)pBuff) + pBuff->NextEntryOffset);
		}
		FileTrackReInit ();
	}

	m_tFileTrackHash.IterateStart();

	const float fDelay = 1.f; 

	r3dTL::TArray< FixedString >	dTemp;

	while ( m_tFileTrackHash.IterateNext() )
	{
		float fTime = m_tFileTrackHash.IterateGet();
		if ( r3dGetTime() - fTime < fDelay )
			continue;

		dTemp.PushBack( m_tFileTrackHash.IterateGetKey() );
		if ( g_pFileTrackCallBack )
			g_pFileTrackCallBack( m_tFileTrackHash.IterateGetKey() );
	} 

	for ( uint32_t i = 0; i < dTemp.Count(); i++ )
	{
		m_tFileTrackHash.Remove( dTemp[ i ] );
	}
	dTemp.Clear();*/
#endif
}

//--------------------------------------------------------------------------------------------------------
void FileTrackChanges( const char * szPath, CallbackFileChange_t * pFunc )
{
	g_sFileTrackPath = szPath;

	g_hFileTrackHandle = ::CreateFile( g_sFileTrackPath,
		FILE_LIST_DIRECTORY, FILE_SHARE_READ | FILE_SHARE_DELETE,
		NULL, OPEN_EXISTING,
		FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OVERLAPPED, NULL );

	if ( g_hFileTrackHandle == INVALID_HANDLE_VALUE )
	{
		FixedString sDir = NewString ( "FileTrackChanges - Can't open directory \"%s\" handle... \n", szPath );
		r3dOutToLog( sDir );
	}

	memset ( &g_FileTrackOvStruct, 0, sizeof(OVERLAPPED) );
	g_FileTrackOvStruct.hEvent = ::CreateEvent ( 0, 1, 0, 0 );
	FileTrackReInit ();

	g_pFileTrackCallBack = pFunc;
}



//--------------------------------------------------------------------------------------------------------
void FixFileName( const char * szFileName, char * szResult )
{
	assert( szFileName );
	if ( ! szFileName )
		return;

	assert( szResult );
	if ( ! szResult )
		return;

	if ( szFileName == szResult )
	{
		assert( false );
		return;
	}

	
	int		pos	= 0;
	int		nLength = 0;
	char	lastChar = '\0';


	r3dscpy( szResult, szFileName );


	while ( szResult[ pos ] != '\0' )
	{
		if ( szResult[ pos ] == '\\' ) 
		{
			szResult[ pos ] = '/';
		}

		if ( ( szResult[ pos ] == lastChar ) && ( lastChar == '/' ) )
		{
			pos++;
			continue;
		}

		lastChar = szResult[ pos ];

		szResult[ pos ] = tolower( szResult[ pos ] );
		szResult[ nLength++ ] = szResult[ pos++ ];
	}
	szResult[ nLength ] = '\0';


	while ( nLength > 0 ) 
	{
		nLength--;

		if ( szResult[ nLength ] != ' ' )
			break;

		szResult[ nLength ] = '\0';
	}
}