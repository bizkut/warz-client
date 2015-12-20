#include "r3dPCH.h"
#include "Config.h"


#include "../CmdProcessor/CmdProcessor.h"

///////////////////////////////////////////////////////////////////////////////

#define REG_VAR( var, value, flag )					CmdVar * var	= NULL;
#define REG_VAR_C( var, value, minv, maxv, flag )	CmdVar * var	= NULL;
	#include "../../Eternity/SF/Console/Vars.h"
#undef REG_VAR
#undef REG_VAR_C

const char * Va( const char * str, ... );

void ExecVarIni( const char* path )
{
	char command[ 1024 ];

	_snprintf( command, sizeof command - 1, "exec %s", path );

	g_pCmdProc->Execute( command, CommandProcessor::eExecPrior_Immediate );

	g_pCmdProc->FlushBuffer();
}

//--------------------------------------------------------------------------------------------------------
void RegisterAllVars()
{

	g_pCmdProc = game_new CommandProcessor;
	g_pCmdProc->Init();

#define REG_VAR( var, value, flag )					var = g_pCmdProc->Register( #var, value, flag );
#define REG_VAR_C( var, value, minv, maxv, flag )	var = g_pCmdProc->Register( #var, value, minv, maxv, flag );
	#include "../../Eternity/SF/Console/Vars.h"
#undef REG_VAR
#undef REG_VAR_C

	ExecVarIni( "game.ini" );
	ExecVarIni( "local.ini" );
}

//--------------------------------------------------------------------------------------------------------
void UnregisterAllVars()
{
	g_pCmdProc->Release();
	SAFE_DELETE( g_pCmdProc );
}
