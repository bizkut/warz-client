#pragma once

#include "../CmdProcessor/CmdVar.h"

#define REG_VAR(var,value,flag)					extern CmdVar * var;
#define REG_VAR_C(var,value,minv,maxv,flag)		extern CmdVar * var;
	#include "../../Eternity/SF/Console/Vars.h"
#undef REG_VAR
#undef REG_VAR_C


void RegisterAllVars();
void UnregisterAllVars();

void ExecVarIni( const char* path );