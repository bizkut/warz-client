#pragma once
/*
DWORD		r3dCountMemUsage(); // return active heap of the process
void		r3dCreateCrashDump();
void		r3dDebugDumpCallStack(bool selectMainThread = false);
void		r3dDebugRemoveAllDumps();
const char*	r3dDebugGetComputerName();*/

typedef	void	(*threadEntry_fn)(DWORD data);
void		r3dThreadEntryHelper(threadEntry_fn fn, DWORD in);

class r3dThreadAutoInstallCrashHelper
{
public:
	r3dThreadAutoInstallCrashHelper(DWORD dwFlags=0);
	~r3dThreadAutoInstallCrashHelper();
	int m_nInstallStatus;
};

//------------------------------------------------------------------------

enum r3dDataBreakpointByteLen
{
	R3D_DATABREAKPOINT_1_BYTE,
	R3D_DATABREAKPOINT_2_BYTES,
	R3D_DATABREAKPOINT_4_BYTES
};

enum r3dDataBreakpointCondition
{
	R3D_DATABREAKPOINT_WRITE = 1,
	R3D_DATABREAKPOINT_READWRITE = 3
};

int r3dSetDataBreakpoint( void* address, r3dDataBreakpointByteLen byteLen, int condition );
void r3dRemoveDataBreakpoint( void* address );

void* r3dGetMainModuleBaseAddress();