#pragma once

#include "Tsg_stl/TString.h"
#include "Tsg_stl/TArray.h"

class CommandProcessor;

//////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	>	
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////
class CmdConsole
{
	bool				m_bVisible;

	FixedString			m_cmdline;

	r3dTL::TArray< FixedString >	m_cmdhistory;
	r3dTL::TArray< FixedString >	m_historylog;

	CommandProcessor * m_pCmdProc;

protected:

	virtual void		EnumCmd( const char * szStr );
	virtual void		FoundCmdCallback ( const char * szFullName )	{}

public:

	void				AddToHistory	( const char * szString );

	virtual void		AddToCmdHistory ( const char * szString ) = 0;
	

	virtual void		Print			( const char * szString ) = 0;


	void				ClearInput		()		{ m_cmdline.SetEmpty(); }							//clears input line
	void				ExecuteInput	();		//executes input line
	void				UpdateInput		( const char * szString )		{ m_cmdline = szString; }	//changes input line

	void				GetInputCompare	()		{} // finds all matches for m_cmdline (when someone presses TAB)


	virtual bool		IsVisible		() const	{ return m_bVisible; };
	virtual void		ShowWindow		()			{ m_bVisible = true; };
	virtual void		HideWindow		()			{ m_bVisible = false; };

	
	void				SetCommandProcessor( CommandProcessor * pCmdProc );

	CmdConsole();
	virtual ~CmdConsole();
};

extern CmdConsole *	g_pDefaultConsole;

void ConPrint( const char* fmt, ... );