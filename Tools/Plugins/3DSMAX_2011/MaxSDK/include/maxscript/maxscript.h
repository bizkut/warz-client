/*		MAXScript.h - main include for MAXScript sources
 *
 *		Copyright (c) John Wainwright, 1996
 *		
 *
 */

#pragma once

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN		// trims win32 includes
#endif

#include <stdlib.h> 
#include <stdio.h>
#include <windows.h>
#include <float.h>
#include <math.h>

// Includes to make this file compile
#include "ScripterExport.h"
#include "kernel\exceptions.h"
#include "kernel\interupts.h"
#include "..\WindowsDefines.h"
#include "..\strbasic.h"
#include "..\ref.h"
#include "..\DefaultActions.h" // for DEFAULTACTIONS_LOGMSG

// Includes that are useful for the maxscript library
#include "..\iFnPub.h"
#include "..\IParamm2.h"
#include "kernel\value.h"
#include "util\sceneio.h"
#include "foundation\arrays.h"
#include "foundation\hashtable.h"
#include "foundation\name.h"
#include "foundation\streams.h"
#include "foundation\strings.h"
#include "foundation\functions.h"
#include "compiler\thunks.h"

// forward declarations
class Value;
class CharStream;
class Rollout;
class MAXScriptException;
class MSPlugin;
class Struct;
class MSZipPackage;
class String;
class IMXSDebugger;
class Object;
class Interface;
class FPValue;
class Control;
class INode;
class Bitmap;
class BitmapInfo;
class HashTable;
class RandGenerator;
class Listener;

#define NOT_SUPPORTED_BY_PRODUCT(fn)	\
    throw RuntimeError (_M("Feature not available: "), _M(#fn)); 

#define END				NULL					// null varargs arg list terminator

#define MAXSCRIPT_UTILITY_CLASS_ID	Class_ID(0x4d64858, 0x16d1751d)
#define MAX_SCRIPT_DIR				_M("scripts")
#define SCRIPT_AUTOLOAD_DIR			_M("Startup\\")

// check whether we are UNICODE or Code page 0 (==> no mbcs code)
#ifdef _UNICODE
#	define	no_mb_chars		TRUE
#	define	bytelen(s)		(sizeof(mwchar_t) * wcslen(s))
#else
#	define	no_mb_chars     (MB_CUR_MAX == 1)
#	define	bytelen(s)		strlen(s)
#endif

inline float EPS(float v) { return _isnan(v) ? (v) : fabs(v) < FLT_EPSILON ? 0.0 : (v); }  // small number round down for %g float printing
inline double EPS(double v) { return _isnan(v) ? (v) : fabs(v) < DBL_EPSILON ? 0.0 : (v); }  // small number round down for %g double printing
inline int EPS(int v) { return v; }
inline INT64 EPS(INT64 v) { return v; }
inline DWORD EPS(DWORD v) { return v; }


/* MAXScript-specific window messages */

#define MXS_ADD_ROLLOUT_PAGE		(WM_USER + 0x100)
#define MXS_DELETE_ROLLOUT_PAGE		(WM_USER + 0x101)
#define MXS_REDRAW_VIEWS			(WM_USER + 0x102)
#define MXS_EDIT_SCRIPT				(WM_USER + 0x103)
#define MXS_NEW_SCRIPT				(WM_USER + 0x104)
#define MXS_DISPLAY_BITMAP			(WM_USER + 0x105)
#define MXS_ERROR_MESSAGE_BOX		(WM_USER + 0x106)
#define MXS_PRINT_STRING			(WM_USER + 0x107)
#define MXS_LISTENER_EVAL			(WM_USER + 0x108)
#define MXS_MESSAGE_BOX				(WM_USER + 0x109)
#define MXS_INITIALIZE_MAXSCRIPT	(WM_USER + 0x10A)
#define MXS_KEYBOARD_INPUT			(WM_USER + 0x10B)
#define MXS_SHOW_SOURCE				(WM_USER + 0x10C)
#define MXS_TAKE_FOCUS				(WM_USER + 0x10D)
#define MXS_STOP_CREATING			(WM_USER + 0x10E)
#define MXS_CLOSE_DOWN				(WM_USER + 0x10F)
#define MXS_STOP_EDITING			(WM_USER + 0x110)
#define MXS_LOAD_STARTUP_SCRIPTS	(WM_USER + 0x111)
#define MXS_EXECUTE_MACRO			(WM_USER + 0x112)
#define MXS_RESTART_EDITING			(WM_USER + 0x113)
#define MXS_NOTIFY_REF_DEPENDENTS	(WM_USER + 0x114)
#define MSPLUGINCLASS_STOPEDITING	(WM_USER + 0x115)
#define MSPLUGINCLASS_RESTARTEDITING	(WM_USER + 0x116)

struct message_box_data 	// LPARAM for MXS_MESSAGE_BOX contains a pointer to this structure
{
	MCHAR* title;
	MCHAR* message;
	BOOL beep;
	int flags;
	BOOL result;
};

class mxs_notify_ref_data			// LPARAM for MXS_NOTIFY_REF_DEPENDENTS contains a pointer to this class
{
public:
	RefTargetHandle hSender;
	Interval changeInt;
	PartID partID;
	RefMessage message;
	SClass_ID sclass;
	BOOL propagate;
	RefTargetHandle hTarg;
	RefResult result;
	
	mxs_notify_ref_data(RefTargetHandle hSender, PartID partID = PART_ALL, RefMessage message = REFMSG_CHANGE) : 
		changeInt(FOREVER), partID(partID), message(message), sclass(NOTIFY_ALL), propagate(TRUE),
		hTarg(NULL), hSender(hSender), result(REF_SUCCEED)
		{
			DbgAssert(hSender);
		}
};

#include "kernel\MAXScript_TLS.h"

/* error handlers */

void out_of_memory();
void bad_delete();

/* arg count check & keyword arg accessors (assume conventional names for arg_list & count in using function) */

extern ScripterExport Value* _get_key_arg(Value** arg_list, int count, Value* key_name);
extern ScripterExport Value* _get_key_arg_or_default(Value** arg_list, int count, Value* key_name, Value* def);

#define key_arg(key)					_get_key_arg(arg_list, count, n_##key)
#define key_arg_or_default(key, def)	_get_key_arg_or_default(arg_list, count, n_##key##, def)
#define int_key_arg(key, var, def)		((var = _get_key_arg(arg_list, count, n_##key)) == &unsupplied ? def : var->to_int())
#define intptr_key_arg(key, var, def)	((var = _get_key_arg(arg_list, count, n_##key)) == &unsupplied ? def : var->to_intptr())
#define float_key_arg(key, var, def)	((var = _get_key_arg(arg_list, count, n_##key)) == &unsupplied ? def : var->to_float())
#define bool_key_arg(key, var, def)		((var = _get_key_arg(arg_list, count, n_##key)) == &unsupplied ? def : var->to_bool())
#define interval_key_arg(key, var, def)		((var = _get_key_arg(arg_list, count, n_##key)) == &unsupplied ? def : var->to_interval())
#define timevalue_key_arg(key, var, def)	((var = _get_key_arg(arg_list, count, n_##key)) == &unsupplied ? def : var->to_timevalue())
#define node_key_arg(key, var, def)			((var = _get_key_arg(arg_list, count, n_##key)) == &unsupplied ? def : var->to_node())
#define string_key_arg(key, var, def)		((var = _get_key_arg(arg_list, count, n_##key)) == &unsupplied ? def : var->to_string())

#define check_arg_count(fn, w, g)			if ((w) != (g)) throw ArgCountError (_M(#fn), w, g)
#define check_gen_arg_count(fn, w, g)		if ((w) != (g + 1)) throw ArgCountError (_M(#fn), w, g + 1)
#define check_arg_count_with_keys(fn, w, g) if (!(g == w || (g > w && arg_list[w] == &keyarg_marker))) throw ArgCountError (_M(#fn), w, count_with_keys())
#define check_gen_arg_count_with_keys(fn, w, g) if (!(g == w || (g > w && arg_list[w-1] == &keyarg_marker))) throw ArgCountError (_M(#fn), w, count_with_keys() + 1)
#define count_with_keys()					_count_with_keys(arg_list, count)

/* for functions that return a boolean */

#define bool_result(val)					((val) ? &true_value : &false_value)

/* for testing a value to ensure that it is within range */

// following only valid for integer and float range checking
#define MXS_range_check(_val, _lowerLimit, _upperLimit, _desc)				\
	if (_val < _lowerLimit || _val > _upperLimit) {							\
		MCHAR buf[256];														\
		MCHAR buf2[128];													\
		_tcscpy(buf,_desc);													\
		_tcscat(buf,_M(" < "));												\
		_sntprintf(buf2, 128, _M("%g"), (double)EPS(_lowerLimit));			\
		_tcscat(buf,buf2);													\
		_tcscat(buf,_M(" or > "));											\
		_sntprintf(buf2, 128, _M("%g"), (double)EPS(_upperLimit));			\
		_tcscat(buf,buf2);													\
		_tcscat(buf,_M(": "));												\
		_sntprintf(buf2, 128, _M("%g"), (double)EPS(_val));					\
		_tcscat(buf,buf2);													\
		throw RuntimeError (buf);											\
	}


/* value local macros - for managing C local variable references to Value*'s for the collector - see Collectable.cpp */

#include "macros\value_locals.h"

/* general utilities */

ScripterExport MCHAR* save_string(const MCHAR* str);
MCHAR  wputch(HWND w, MCHAR* buf, MCHAR* bufp, const MCHAR c);					/* edit window output... */
MCHAR* wputs(HWND w, MCHAR* buf, MCHAR* bufp, const MCHAR *str);		
int    wprintf(HWND w, MCHAR* buf, MCHAR* bufp, const MCHAR *format, ...);
void   wflush(HWND w, MCHAR* buf, MCHAR* bufp);		
#define mputs	thread_local(current_stdout)->puts	/* current MAXScript stdout output... */
#define mprintf	thread_local(current_stdout)->printf
#define mflush	thread_local(current_stdout)->flush
extern MCHAR *GetString(int id);
extern ScripterExport Object* Get_Object_Or_XRef_BaseObject(Object* obj);

// Converts string to valid property or class name, replacing invalid characters with an underscore.
// For example, 'Vol. Select' would be converted to 'Vol__Select'
// Type corresponds to PROPNAME, CLASSNAME, or DROPSPACES where:
//	PROPNAME - spaces in string are converted to underscore
//	CLASSNAME - spaces and punctuation in string are converted to underscore
//	DROPSPACES - removes underscore characters in string
// if bStripTrailing is true, after conversion any underscore characters at end of string are removed
// Return value is how many characters were converted to an underscore
// This method is MBCS-aware
static const int PROPNAME = 1;
static const int CLASSNAME = 2;
static const int DROPSPACES	= 3;

extern ScripterExport int namify(MCHAR* n, int type, bool bStripTrailing = true);

 
ScripterExport void  install_utility_page(Rollout* rollout);

#include "kernel\MaxscriptTypedefs.h"

/* MAXScript signal flags */

#define INTERRUPT_EVAL		0x0001
#define EXIT_LISTENER		0x0002

extern ScripterExport Interface* MAXScript_interface;
extern ScripterExport Interface7* MAXScript_interface7;
extern ScripterExport Interface8* MAXScript_interface8;
extern ScripterExport Interface9* MAXScript_interface9;
extern ScripterExport Interface11* MAXScript_interface11;
extern ScripterExport Interface13* MAXScript_interface13;
extern ScripterExport BOOL		escape_enabled; // RK: 05/20/02, 5.0 or later only
extern ScripterExport BOOL		MAXScript_detaching;
extern ScripterExport int		mxs_rand(); // resolution is 0 - 2^31-1
extern ScripterExport INT64		mxs_rand64(); // resolution is 0 - 2^63-1
extern ScripterExport void		mxs_seed(int);
extern ScripterExport int		random_range(int from, int to);
extern ScripterExport INT64		random_range(INT64 from, INT64 to);
extern ScripterExport float		random_range(float from, float to);
extern ScripterExport double	random_range(double from, double to);
extern ScripterExport void		dlx_detaching(HINSTANCE hinstance);
extern ScripterExport void		define_system_global(MCHAR* name, Value* (*getter)(), Value* (*setter)(Value*));
// LAM 4/1/00 - added following to be able to overwrite existing global value in hash table.
extern ScripterExport void		define_system_global_replace(MCHAR* name, Value* (*getter)(), Value* (*setter)(Value*));
extern ScripterExport void		define_struct_global(MCHAR* name, MCHAR* struct_name, Value* (*getter)(), Value* (*setter)(Value*));
extern ScripterExport HashTable* english_to_local;
extern ScripterExport HashTable* local_to_english;
extern ScripterExport BOOL		non_english_numerics;
extern ScripterExport void	    printable_name(MSTR& name);
extern ScripterExport void		show_source_pos();
extern ScripterExport void		show_listener();

extern ScripterExport void		init_MAXScript();
extern ScripterExport BOOL		MAXScript_running;
extern ScripterExport HWND		main_thread_window;
extern ScripterExport BOOL		progress_bar_up;
extern ScripterExport BOOL		trace_back_active;
extern ScripterExport BOOL		disable_trace_back;
extern ScripterExport int		trace_back_levels;
typedef void (*utility_installer)(Rollout* ro);
extern ScripterExport void		set_utility_installer(utility_installer ui);
extern ScripterExport void		reset_utility_installer();
extern ScripterExport void		error_message_box(MAXScriptException& e, const MCHAR* caption);
typedef Value* (*autocad_point_reader)(MCHAR* str);
extern ScripterExport void		set_autocad_point_reader(autocad_point_reader apr);

// LAM - 4/28/03
//! \brief Function compiles and evaluates the specified string.
/*! If the string is successfully compiled and evaluated, and a pointer to an FPValue is provided, 
the return value is converted to an FPValue and returned through the referenced FPValue. 
If the string is not successfully compiled and evaluated, error messages are logged to LogSys if net rendering. 
If not net rendering, error messages are written to Listener or logged to LogSys.
\par Parameters:
<b>MCHAR *s</b>
Points to a null-terminated string that specifies the MAXScript commands to compile and evaluate.
<b>BOOL *quietErrors</b>
If TRUE, or net rendering, errors are logged to LogSys. If FALSE and not net rendering, errors are logged to Listener.
<b>FPValue *fpv</b>
Optionally points to an FPValue. If not NULL, the result of the script evaluation is converted to an FPValue and 
stored in the specified FPValue.
\return Returns TRUE if script was executed successfully.
*/
extern ScripterExport BOOL		ExecuteMAXScriptScript(MCHAR* s, BOOL quietErrors = FALSE, FPValue* fpv = NULL);

// LAM - 8/19/05
// Returns Value* resulting from evaluating source. If res is non-NULL, will contain TRUE on return if script was executed 
// successfully, FALSE if not. Evaluation is within a try/catch, so this function will not throw.
extern ScripterExport Value*	ExecuteScript(CharStream* source, bool *res);

// LAM - 6/24/03
// Method for processing input value or array for default action value. Current recognized values are:
// #logMsg, #logToFile, #abort, and integer values. Actions are converted to DWORD where bit 0 is log to Default Action 
// system log, bit 1 is log to log file, and bit 2 is abort/cancel (if applicable).
extern ScripterExport DWORD		ProcessDefaultActionVal (Value* inpActionVal, DWORD defaultAction = DEFAULTACTIONS_LOGMSG);

// LAM - 3/25/05 - the MXS debugger interface. A Singleton.
extern ScripterExport IMXSDebugger* theMXSDebugger;

#define type_check(val, cl, where) if (val->tag != class_tag(cl)) throw TypeError (where, val, &cl##_class);

// macros for setting numeric printing to English locale and back again - all numeric output in MAXScript is English
#define set_english_numerics()						\
	MCHAR* locale;									\
	MCHAR slocale[256];								\
	if (non_english_numerics != NULL)				\
	{												\
		locale = setlocale(LC_NUMERIC, NULL);		\
		_tcsncpy(slocale, locale, sizeof(slocale)/sizeof(slocale[0]));	\
		setlocale(LC_NUMERIC, "C");					\
	}

#define reset_numerics()							\
	if (non_english_numerics != NULL)				\
		setlocale(LC_NUMERIC, slocale);	

#define SOURCE_STREAM_ENCRYPTED_FLAG 1 /* flag set in thread local source_flags when source is encrypted  */

extern ScripterExport HashTable* globals;
extern ScripterExport HashTable* persistents;
extern ScripterExport Listener*  the_listener;
extern ScripterExport HWND		 the_listener_window;
extern                HINSTANCE  hInstance;
extern ScripterExport void       listener_message(UINT iMsg, WPARAM wParam, LPARAM lParam, BOOL block_flag);


extern ScripterExport RandGenerator* ClassIDRandGenerator;

inline int _count_with_keys(Value** arg_list, int count)
{
	// compute # args before any key-args
	for (int i = 0; i < count; i++)
		if (arg_list[i] == (Value*)&keyarg_marker)
			return i;
	return count;
}

extern ScripterExport BOOL GetPrintAllElements(); // get whether to print all elements of arrays, meshselection, BigMatrix, etc.
extern ScripterExport BOOL SetPrintAllElements(BOOL); // set whether to print all elements of arrays, meshselection, BigMatrix, etc. Returns old value

extern ScripterExport bool CanChangeGroupFlags(INode* node); // Since grouping break the node hierarchy used to represent ALOs, we don't allow it

// get MXS path to object. This is the path shown in macro recorder. If resolved, returns TRUE and places path in name
extern ScripterExport BOOL Find_MXS_Name_For_Obj(Animatable* obj, MSTR &name, BOOL explicitName = TRUE); 

// methods to convert LF strings to CR/LF strings and back
extern ScripterExport void Replace_LF_with_CRLF(MSTR &string);
extern ScripterExport void Replace_CRLF_with_LF(MSTR &string);

extern ScripterExport void checkFileOpenModeValidity(MCHAR* mode); // checks validity of mode for fopen commands, and throws error if not valid

extern ScripterExport BOOL max_name_match(MCHAR* max_name, MCHAR* pattern, bool caseSensitive = false);// returns true if name matches pattern. Will test original and translated name.

// create and return a Bitmap* based on the BitmapInfo. Basically just a wrapped version of
// TheManager->Create(bitmapInfo) that throws a runtime error if creation is unsuccessful.
extern ScripterExport Bitmap* CreateBitmapFromBitmapInfo(BitmapInfo& bitmapInfo);

// get the storable bitmapinfo type for the specified bitmapinfo type
extern ScripterExport int GetStorableBitmapInfoTypeForBitmapInfoType(int type);

// MAXScript preferences.  An instance of this class lives in CORE and is accessible to both CORE and MAXScript
class MAXScriptPrefs
{
public:
	int		loadStartupScripts;
	int		loadSaveSceneScripts;
	int		loadSavePersistentGlobals;
	MSTR	font;
	int		fontSize;
	int		autoOpenListener;
	float	initialHeapSize;
	int		enableMacroRecorder;
	int		showCommandPanelSwitch;
	int		showToolSelections;
	int		showMenuSelections;
	int		absoluteSceneNames;
	int		absoluteSubObjects;
	int		absoluteTransforms;
	int		explicitCoordinates;
	int		useFastNodeNameLookup;
	int		showGCStatus;
	int		showEditorPath;

	MAXScriptPrefs() { Reset(); }
	virtual ~MAXScriptPrefs() { }
	void	Reset()
	{
		// MAXScript preference defaults
		loadStartupScripts =		TRUE;
		loadSaveSceneScripts =		TRUE;
		loadSavePersistentGlobals = TRUE;
		font =						_M("Courier New");
		fontSize =					9;
		initialHeapSize =			15.0;
		autoOpenListener =			FALSE;
		enableMacroRecorder =		FALSE;
		showCommandPanelSwitch =	FALSE;
		showToolSelections =		FALSE;
		showMenuSelections =		FALSE;
		absoluteSceneNames =		FALSE;
		absoluteSubObjects =		FALSE;
		absoluteTransforms =		FALSE;
		explicitCoordinates =		FALSE;
		useFastNodeNameLookup =		TRUE;
		showGCStatus =				FALSE;
		showEditorPath =			TRUE;
	}

	virtual void LoadMAXScriptPreferences();
	virtual void SaveMAXScriptPreferences();
};

// temporarily adds license for various activeX controls
// ctor registers the license if needed, dtor removes them
class ActiveXLicensor
{
	DWORD licenseAdded; // acts as bitfield. Each bit signifies whether a specifc license was added.
public:
	ScripterExport ActiveXLicensor();
	ScripterExport ~ActiveXLicensor();
};

// Exception safe helper class for controlling temporary changing of quiet mode. ctor sets quiet mode to 
// specified state, Restore resets to initial state, and dtor resets to initial state if Restore has not been run.
class TempQuietMode
{
public:
	ScripterExport TempQuietMode (BOOL newState = TRUE);
	ScripterExport ~TempQuietMode();
	ScripterExport void Restore();
private:
	BOOL oldState;
	bool needsRestore;
};

// Exception safe helper class for controlling temporary changing of BitmapManager silent mode. ctor sets silent mode to 
// specified state, Restore resets to initial state, and dtor resets to initial state if Restore has not been run.
class TempBitmapManagerSilentMode
{
public:
	ScripterExport TempBitmapManagerSilentMode (BOOL newState = TRUE);
	ScripterExport ~TempBitmapManagerSilentMode();
	ScripterExport void Restore();
private:
	BOOL oldState;
	bool needsRestore;
};

