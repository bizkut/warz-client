#pragma once


#include "CmdEvent.h"
#include "CmdVar.h"

#include "Tsg_stl/HashTable.h"
#include "r3dHash.h"


typedef void ( CmdFunc_t )( const VarsContainer_c& ev );


#define DECLARE_CMD( name )					void CMD_##name( const VarsContainer_c& ev )
#define REG_CCOMMAND( name, flags, szHelp )	g_pCmdProc->AddCommand( #name, CMD_##name, flags, szHelp );



class CmdConsole;



///////////////////////////////////////////////////////////////////////////////////////////////
//
//	>	class Cmd
//
///////////////////////////////////////////////////////////////////////////////////////////////
class Cmd
{
private: 

	DWORD				m_dwFlags;
	FixedString			m_sName;	// fixme:
	const char *		m_szHelp;
	Cmd *				m_pNext;

public:

	enum CmdType
	{
		eCmdType_None,
		eCmdType_CVar,
		eCmdType_Func,
		eCmdType_Alias
	};

	CmdType			m_eType;

	union
	{
		CmdFunc_t *		m_pFunc;
		CmdVar *		m_pVar;
		char *			m_szAlias;	// caution! aliases are stored in system memory
	};

	int					m_nAliasSize;

	//////////////////////////////////////////////////////////////////////////////////////////////////////////
	//
	//	>	
	//
	//////////////////////////////////////////////////////////////////////////////////////////////////////////

	inline void			ExcludeNext	()				{ assert( m_pNext ); m_pNext = m_pNext->m_pNext; };
	inline const char *	GetName		() const		{ return m_sName; };
	inline Cmd *		GetNext		() const		{ return m_pNext; };
	inline DWORD		GetFlags	() const		{ return m_dwFlags; };


	Cmd			( Cmd * pNext, const char * szName, DWORD dwFlags = 0, const char * szHelp = NULL );
	virtual ~Cmd	();
};

struct CmdHashFunc_T
{
	inline int operator () ( const char * szKey )
	{
		return r3dHash::MakeHash( szKey );// & ( LENGTH-1 );
	}
};


///////////////////////////////////////////////////////////////////////////////////////////////
//
//	>	class CommandProcessor
//
///////////////////////////////////////////////////////////////////////////////////////////////
class CommandProcessor
{
	enum //	cmdproc limits
	{
		CMD_MAX_COMMANDLINE		=	1024,
		CMD_MAX_FRAMECOMMANDS	=	32,
		CMD_MAX_ALIASLOOP		=	100,

		CMD_BUFSIZE				=	CMD_MAX_FRAMECOMMANDS * CMD_MAX_COMMANDLINE,
	};


	////////////////////////////////
	// data types
	struct CTextBuf
	{
		char			m_szText[ CMD_BUFSIZE ];
		DWORD			m_dwSize;
	};


	CTextBuf				m_FrameCommands;

	Cmd *					m_pCommands;

	HashTableDynamic< Cmd *, FixedString, CmdHashFunc_T, 256 >		m_tCmdHash;

	int						m_iAliasCount;
	
	CmdConsole *			m_pConsole;

	void					AddVar				( CmdVar * pVar );
	void					DelVar				( CmdVar * pVar );

	Cmd *					AddCmd				( const char * szName, DWORD dwFlags, const char * szHelp );
	bool					DelCmd				( Cmd * pCmd );

	void					InsertLine			( const char * szLine, CTextBuf * pBuffer = NULL );
	void					AppendLine			( const char * szLine, CTextBuf * pBuffer = NULL );
	void					ExecuteLine			( const char * szLine, DWORD dwFlags = 0 );

public:

	enum ExecPrior_e
	{
		eExecPrior_Append,
		eExecPrior_Insert,
		eExecPrior_Immediate
	};

	//////////////////////////////////////////////////////////////////////////////////////////////////////////
	//
	//	>	For console
	//
	//////////////////////////////////////////////////////////////////////////////////////////////////////////
	void					SetConsole				( CmdConsole * pConsole )		{ m_pConsole = pConsole; }
	void					RemoveConsole			( CmdConsole * pConsole )		{ m_pConsole = NULL; }

	const Cmd *				GetCmdList				() const						{ return m_pCommands; };

	void					ListCmdToConsole		( const char * szSubStr, const char * szFlagMask, Cmd::CmdType eType );

	void					Print				( const char * szString ) ;

		
	//////////////////////////////////////////////////////////////////////////////////////////////////////////
	//
	//	>	
	//
	//////////////////////////////////////////////////////////////////////////////////////////////////////////

	void					Init				();
	void					Release				();

	CmdVar *				Register			( const char * szName, const char * szVal, DWORD dwFlags );
	CmdVar *				Register			( const char * szName, int nVal, DWORD dwFlags );
	CmdVar *				Register			( const char * szName, float fVal, DWORD dwFlags );
	CmdVar *				Register			( const char * szName, float fVal, float fMinVal, float fMaxVal, DWORD dwFlags );

	void					AddCommand			( const char * szCmdName, CmdFunc_t * pHandler, DWORD dwFlags, const char * szHelp );

	// Moved public for PunkBuster functionality
	CmdVar *				FindVar				( const char * szName );

	void					Execute				( const char * szString, ExecPrior_e ePrior = eExecPrior_Append );
	void					FlushBuffer			();

	void					SaveVars			( const char * szFileName );


							CommandProcessor		();
	virtual					~CommandProcessor		();	
};


extern CommandProcessor * g_pCmdProc;