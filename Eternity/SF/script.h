#pragma once

/* --- script format ---


token1:		value
token2:		"string_value"

section_1
{
}

section_1
{
	token3:		value
	token3:		value
}

*/


#ifndef MAX_PATH
#define MAX_PATH 260
#endif


class Script_c
{
private:

	static char	m_szNumbers[];

	char		m_szFileName[ MAX_PATH ];
	bool		m_bFindToken;
	int			m_iLineNumber;

	const char *m_pStartPos;
	int			m_iBytesRead;
	int			m_iBytesTotal;
	bool		m_bEOF;
	
	char *		m_pStoredFile;
	const char * m_pMemoryLocation;

	char		Peek			()		{ return * ( m_pStartPos + m_iBytesRead ); };
	bool		Read			( char * pBuffer, int iSize );

protected:
	void		ErrorAt			( const char *pErrorName, const char * szToken );

public:

	const char * GetName		() const { return m_szFileName; };

	bool		OpenFile		( const char * szFileName, bool as_binary=false );
	void		CloseFile		();
	void		OpenMemoryLocation( const char * pData, int iSize );
	bool		EndOfFile		()		{ return m_bEOF; }
	bool		RewindAfterToken( const char * szToken, bool from_start = true, bool whole_word = true );
	void		GetToken		( char * szToken, bool ignore_delimiters = false );
	void		SkipToken		( const char * szToken );
	void		GetLine			( char * wheretostore, int max_size );
	void		GetSpecific		( char * pData );

	void		GetString		( char * wheretostore, int nMaxSize, char chClamp = '"' );

	int			GetInt			( const char * szToken );
	int			GetInt			();

	float		GetFloat		( const char *token );
	float		GetFloat		();


	int			GetPosition		() const	{ return m_iBytesRead; };
	void		SetPosition		( int pos );
	void		GetDump			( char * pBuffer, int start, int end );
	
	const char *GetBuf			()	{ return m_pStartPos; };
	
				Script_c			() : m_pStoredFile( NULL ), m_pMemoryLocation( NULL ) { m_szFileName[ 0 ] = '\0'; };
				~Script_c		();
};
