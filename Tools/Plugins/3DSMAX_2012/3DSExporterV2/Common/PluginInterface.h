#ifndef __TOOLS_MAXPLUGININTERFACE_H
#define __TOOLS_MAXPLUGININTERFACE_H


	//
	// my plugin libraries must export following functions
	//
#define MAXUTIL_PLUGIN_VERSION          0x00010020
typedef DWORD	(*expGetVersion_fn)();
typedef DWORD	(*expInit_fn)(Interface *ip);
typedef int	(*expGetFuncCount_fn)();
typedef TCHAR* 	(*expGetFuncName_fn)(int id);
#define MAXUTIL_PLUGIN_TYPE_BUTTON	1		// just button
#define MAXUTIL_PLUGIN_TYPE_EXPORTER	2		// button that will invoke exporter interface
#define	MAXUTIL_PLUGIN_TYPE_CHECKBOX	3		// checkbox
#define MAXUTIL_PLUGIN_TYPE_DELIMITER	4		// small delimiter line
typedef	DWORD	(*expGetFuncType_fn)(int id, char *data1, char *data2, char *data3);

	//
	// when calling plugin function this struct will be passed
	//
#define IDC_DLL_UNLOAD	0x6033		// lparam is dllId
struct expParams
{
        int             dllId;
        Interface*      ip;
        IUtil*          iu;
        HWND            hWnd;

	int		bShowLog;
        char		exportFile[MAX_PATH];	// if we selected file - otherwise NULL

	// if type == MAXUTIL_PLUGIN_TYPE_CHECKBOX, val1 will be desired state
	DWORD		dw1;

	char		*FP_cmd;
	Tab<char *>	*FP_args;
};

#define	FP_FUNC_ID	0x1111		// used for interfacing with MaxScript
// if modeless is 1 then DLL will not be unloaded, and will wait for IDC_DLL_UNLOAD message
// if funcId == FP_FUNC_ID that will mean we're called from MaxScript
typedef int    (*expExecFunc_fn)(int funcId, expParams *parm, int *modeless);

#endif //__TOOLS_MAXPLUGININTERFACE_H

