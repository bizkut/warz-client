/*		MAXScript.h - main include for MAXScript sources
 *
 *		Copyright (c) John Wainwright, 1996
 *		
 *
 */

#pragma once

#include "..\ScripterExport.h"
#include "..\..\WindowsDefines.h"
#include "..\..\maxtypes.h"
#include "..\..\maxapi.h"

// forward declarations
class Value;
class CharStream;
class Rollout;
class MAXScriptException;
class MSPlugin;
class Struct;
class MSZipPackage;
class String;
class Control;

/* thread-local storage struct decl & access macros */

struct MAXScript_TLS
{
	MAXScript_TLS*	next;				/* links...							*/
	MAXScript_TLS*	prev;
	HANDLE			my_thread;			/* thread that owns this TLS struct	*/
	DWORD			my_thread_id;

#undef def_thread_local
#define def_thread_local(type, lcl, init_val) type lcl
#	include "..\protocols\thread_locals.h"

};

#define thread_local(x)			(((MAXScript_TLS*)TlsGetValue(thread_locals_index))->x)

/* index, tls struct list globals */

extern ScripterExport int thread_locals_index;
extern int thread_id_index;
extern MAXScript_TLS* MAXScript_TLS_list;


#define needs_redraw_set()			thread_local(needs_redraw) = 1
#define needs_complete_redraw_set()	thread_local(needs_redraw) = 2
#define needs_redraw_clear()		thread_local(needs_redraw) = 0

#define MAXScript_time()	\
	(thread_local(use_time_context) ? thread_local(current_time) : GetCOREInterface()->GetTime())

			   void alloc_thread_locals();
ScripterExport void init_thread_locals();
			   void free_thread_locals();
			   void copy_thread_locals(MAXScript_TLS* source, MAXScript_TLS* target);