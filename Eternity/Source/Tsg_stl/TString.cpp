#include "r3dPCH.h"
#include "Tsg_stl/TString.h"


#ifdef STATISTIC_STRING

static bool g_bCheckStr = false;

static CRITICAL_SECTION	gs_strlock;

struct StrStat_t
{
	size_t		nSize;
	uint32_t	dwIndex;
};

typedef r3dgameHashMap( void *, StrStat_t )	TStrHash;

//----------------------------------------------------------------------------------------------------------------
TStrHash & GetHash()
{
	static TStrHash m_hash;
	static bool bInit = false;
	if ( ! bInit )
	{
		InitializeCriticalSection( &gs_strlock );
		bInit = true;
	}

	return m_hash;
}

//----------------------------------------------------------------------------------------------------------------
void _CheckStr()
{
	if ( !g_bCheckStr )
		return;

	TStrHash & hash = GetHash();

	size_t nMemSize = 0;
	size_t nStrSize = 0;

	StrHash_T< int > store;	

	Log( "--- Dump String_T<> statistic ---" );		

	EnterCriticalSection( &gs_strlock );
	TStrHash::iterator it = hash.begin();

	for ( ; it != hash.end(); ++it )
	{
		const StrStat_t &stat = it->second;
		nMemSize += stat.nSize;
		const char * szStr = ( const char * ) it->first;
		size_t nLen = strlen( szStr );
		nStrSize += nLen + 1;
		//Log( "   \"%s\" - %d", szStr, stat.dwIndex );

		int nCount = 0;
		if ( store.Find( szStr, &nCount ) )
			store.ReplaceByName( szStr, ++nCount );
		else
			store.Add( ++nCount, szStr );
	}

	LeaveCriticalSection( &gs_strlock );
	
	store.IterateStart ();
	while ( store.IterateNext () )
	{
		const char * s = store.IterateGetKey ();
		int nCount = store.IterateGet ();
		if ( nCount < 2 )
			continue;

		Log( " %d -   \"%s\"", nCount, s );
	}

	Log( "Total: %d, allocate: %d, actual: %d", hash.size(), nMemSize, nStrSize );
	Log( "--- End dump ---" );		
}



//----------------------------------------------------------------------------------------------------------------
void _AddStaticStr( const char * szPtr, size_t nSize )
{
	_CheckStr();

	TStrHash & hashe = GetHash();

	void * pVal = ( TStrHash::key_type ) szPtr;

	static uint32_t dwIndex = 0;
	StrStat_t stat;
	stat.dwIndex = dwIndex++;
	stat.nSize = nSize;

	EnterCriticalSection( &gs_strlock );
	hashe[ pVal ] = stat;
	LeaveCriticalSection( &gs_strlock );
}

//----------------------------------------------------------------------------------------------------------------
void _RemoveStaticStr( const char * szPtr, size_t nSize )
{
	_CheckStr();

	TStrHash & hash = GetHash();

	void * pVal = ( TStrHash::key_type ) szPtr;

	EnterCriticalSection( &gs_strlock );

	TStrHash::iterator it = hash.find( pVal );
	if ( it != hash.end() )
	{
		hash.erase( it );
	}
	else
	{
		Assert( false );
	}
	LeaveCriticalSection( &gs_strlock );
}

#endif	//	STATISTIC_STRING

///////////////////////////////////////////////////////////////////////////
FixedString NewString ( const char * sFmt, ... )
{
	assert ( sFmt );

	FixedString sRes;

	va_list ap;
	va_start ( ap, sFmt );
	vsnprintf ( sRes.GetDataPtr(), sRes.GetMaxLength (), sFmt, ap );
	va_end ( ap );

	return sRes;
};