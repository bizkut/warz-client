#include "r3dPCH.h"
#include "r3d.h"
#include "r3dDebug.h"

#include <windows.h>
#include <tlhelp32.h>

#include "dbghelp/include/dbghelp.h"
#ifndef DISABLE_CRASHRPT
#pragma comment(lib, "../External/CrashRpt/Lib/CrashRpt1301.lib")
#include "CrashRpt/include/CrashRpt.h" 
#endif
//#include "errorrep.h"


extern void r3dCloseLogFile();

bool CreateConfigPath(char* dest);
bool CreateWorkPath(char* dest);

static BOOL CALLBACK MyMiniDumpCallback(PVOID                            pParam, 
								 const PMINIDUMP_CALLBACK_INPUT   pInput, 
								 PMINIDUMP_CALLBACK_OUTPUT        pOutput 
								 ) 
{
	BOOL bRet = FALSE; 

	// Check parameters 
	if( pInput == 0 ) 
		return FALSE; 

	if( pOutput == 0 ) 
		return FALSE; 

	// Process the callbacks 
	switch( pInput->CallbackType ) 
	{
	case IncludeModuleCallback: 
		{
			// Include the module into the dump 
			bRet = TRUE; 
		}
		break; 
	case IncludeThreadCallback: 
		{
			// Include the thread into the dump 
			bRet = TRUE; 
		}
		break; 
	case ModuleCallback: 
		{
			// Does the module have ModuleReferencedByMemory flag set ? 
			if( !(pOutput->ModuleWriteFlags & ModuleReferencedByMemory) ) 
			{
				// No, it does not - exclude it 
				//r3dOutToLog( "Excluding module: %s \n", pInput->Module.FullPath ); 
				pOutput->ModuleWriteFlags &= (~ModuleWriteModule); 
			}
			bRet = TRUE; 
		}
		break; 
	case ThreadCallback: 
		{
			// Include all thread information into the minidump 
			bRet = TRUE;  
		}
		break; 
	case ThreadExCallback: 
		{
			// Include this information 
			bRet = TRUE;  
		}
		break; 
	case MemoryCallback: 
		{
			// We do not include any information here -> return FALSE 
			bRet = FALSE; 
		}
		break; 
	case CancelCallback: 
		break; 
	}
	return bRet; 
}

static TCHAR szPath[MAX_PATH+1];

static LONG WINAPI CreateMiniDump( EXCEPTION_POINTERS* pep ) 
{
	r3dOutToLog("Creating minidump!!\n");

	// Open the file 
	char miniDumpPath[1024];
	if(CreateConfigPath(miniDumpPath))
	{
		strcat( miniDumpPath, "MiniDump.dmp" );
		r3dOutToLog("Minidump path: %s\n", miniDumpPath);

		HANDLE hFile = CreateFile( miniDumpPath, GENERIC_READ | GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL ); 
		if( ( hFile != NULL ) && ( hFile != INVALID_HANDLE_VALUE ) ) 
		{
			// Create the minidump 
			MINIDUMP_EXCEPTION_INFORMATION mdei; 
			mdei.ThreadId           = GetCurrentThreadId(); 
			mdei.ExceptionPointers  = pep; 
			mdei.ClientPointers     = FALSE; 

			MINIDUMP_CALLBACK_INFORMATION mci; 
			mci.CallbackRoutine     = (MINIDUMP_CALLBACK_ROUTINE)MyMiniDumpCallback; 
			mci.CallbackParam       = 0; 

			MINIDUMP_TYPE mdt       = (MINIDUMP_TYPE)(MiniDumpWithIndirectlyReferencedMemory | MiniDumpScanMemory); 

			BOOL rv = MiniDumpWriteDump( GetCurrentProcess(), GetCurrentProcessId(), 
				hFile, mdt, (pep != 0) ? &mdei : 0, 0, &mci ); 

			if( !rv ) 
				r3dOutToLog( "MiniDumpWriteDump failed. Error: %u \n", GetLastError() ); 
			else 
				r3dOutToLog( "Minidump created.\n" ); 

			// Close the file 
			CloseHandle( hFile ); 
		}
		else 
		{
			r3dOutToLog( "CreateFile failed. Error: %u \n", GetLastError() ); 
		}

		r3dOutToLog("\n!!!Crash!!!\nPlease send '%s' to support@warz.kongsi.asia\nThank you.", miniDumpPath);
		// hide window, hopefully will work in fullscreen
		ShowWindow(win::hWnd, SW_FORCEMINIMIZE);
		
		// show message box to user
		char tempStr[2048];
		sprintf(tempStr, "Application crashed.\nPlease send '%s' and r3dLog.txt (in install folder of the game) to support@warz.kongsi.asia along with description of what you were doing at the time of crash.\nThank you and sorry for inconvenience.", miniDumpPath);
		MessageBox(0, tempStr, "Crash", MB_OK);
	}
	r3dCloseLogFile(); 

	// call WINDOWS ERROR REPORTING, in case if user will not send us crashdump
	LONG lRet = EXCEPTION_CONTINUE_SEARCH;
	lRet = EXCEPTION_EXECUTE_HANDLER;
	return lRet ;
}

// The following function returns TRUE if the correct CrashRpt DLL was loaded,
// otherwise it returns FALSE
/*BOOL CheckCrashRptVersion()
{
	BOOL bExitCode = FALSE;
	TCHAR szModuleName[_MAX_PATH] = _T("");
	HMODULE hModule = NULL;
	DWORD dwBuffSize = 0;
	LPBYTE pBuff = NULL;
	VS_FIXEDFILEINFO* fi = NULL;
	UINT uLen = 0;

	// Get handle to loaded CrashRpt.dll module
	hModule = GetModuleHandle(_T("CrashRpt.dll"));
	if(hModule==NULL)
		goto cleanup; // No "CrashRpt.dll" module loaded

	// Get module file name
	GetModuleFileName(hModule, szModuleName, _MAX_PATH);

	// Get module version 
	dwBuffSize = GetFileVersionInfoSize(szModuleName, 0);
	if(dwBuffSize==0)
		goto cleanup; // Invalid buffer size

	pBuff = (LPBYTE)GlobalAlloc(GPTR, dwBuffSize);
	if(pBuff==NULL)
		goto cleanup;

	if(0==GetFileVersionInfo(szModuleName, 0, dwBuffSize, pBuff))
		goto cleanup; // No version info found

	VerQueryValue(pBuff, _T("\\"), (LPVOID*)&fi, &uLen);

	WORD dwVerMajor = HIWORD(fi->dwProductVersionMS);
	WORD dwVerMinor = LOWORD(fi->dwProductVersionMS);  
	WORD dwVerBuild = LOWORD(fi->dwProductVersionLS);

	DWORD dwModuleVersion = dwVerMajor*1000+dwVerMinor*100+dwVerBuild;

	if(CRASHRPT_VER==dwModuleVersion)
		bExitCode = TRUE; // Version match!

cleanup:

	if(pBuff)
	{
		// Free buffer
		GlobalFree((HGLOBAL)pBuff);
		pBuff = NULL;
	}

	return bExitCode;
}
*/
BOOL CALLBACK r3dCrashRptCallback(__reserved LPVOID lpvState)
{
	r3dCloseLogFile();
	return TRUE;
}

r3dThreadAutoInstallCrashHelper::r3dThreadAutoInstallCrashHelper(DWORD dwFlags)
{
#ifndef DISABLE_CRASHRPT
	m_nInstallStatus = crInstallToCurrentThread2(dwFlags);    
#endif
}

r3dThreadAutoInstallCrashHelper::~r3dThreadAutoInstallCrashHelper()
{
#ifndef DISABLE_CRASHRPT
	crUninstallFromCurrentThread();
#endif
}

static const wchar_t* crashRpgGetLangFile()
{
	static const wchar_t* en = L"crashrpt_lang.ini";

//	if(_waccess(ru, 0) != 0)
//		return en;

	// russia & ukraine using 1251 codepage
//	if(GetACP() == 1251)
//		return ru;
		
	return en;
}

void r3dThreadEntryHelper(threadEntry_fn fn, DWORD in)
{
	/*if(!CheckCrashRptVersion())
	{
		// An invalid CrashRpt.dll loaded!
		MessageBox(NULL, "The version of CrashRpt.dll is invalid.", "CRASH RPT ERROR", MB_OK);
		return;
	}*/

	if(!IsDebuggerPresent())
	{
#ifdef DISABLE_CRASHRPT
		SetUnhandledExceptionFilter(CreateMiniDump);
#else
		// detect language file
		wchar_t curDir[MAX_PATH];
		wchar_t langFile[MAX_PATH];
		GetCurrentDirectoryW(sizeof(curDir), curDir);
		swprintf(langFile, MAX_PATH, L"%s\\%s", curDir, crashRpgGetLangFile());

		// use wide versino of structure, as pszLangFilePath *require* full path by some reasons
		CR_INSTALL_INFOW info;
		memset(&info, 0, sizeof(CR_INSTALL_INFOW));  
		info.cb = sizeof(CR_INSTALL_INFOW);
#ifdef FINAL_BUILD
		info.pszAppName = L"Kongsi WarZ";
#else
		info.pszAppName = L"Studio";
#endif
		info.pszAppVersion = L"1.0";
		info.pszEmailTo = NULL;
		
		info.pszUrl = L"http://apiwarz.kongsi.asia/WarZ/api/php/api_CrashReport.php";
		
		info.pszCrashSenderPath = NULL;
		info.pfnCrashCallback = &r3dCrashRptCallback;
		info.uPriorities[CR_HTTP] = 1;
		info.uPriorities[CR_SMTP] = CR_NEGATIVE_PRIORITY; // skip it
		info.uPriorities[CR_SMAPI] = CR_NEGATIVE_PRIORITY; // skip it
		info.dwFlags |= CR_INST_ALL_EXCEPTION_HANDLERS;
		info.dwFlags |= CR_INST_HTTP_BINARY_ENCODING;
		info.dwFlags |= CR_INST_SEND_QUEUED_REPORTS;
		//we should not restart app, as GNA using command line to pass login info
		//info.dwFlags |= CR_INST_APP_RESTART;
		//info.pszRestartCmdLine   = __r3dCmdLine; 
		info.pszPrivacyPolicyURL = L"https://warz.kongsi.asia/PrivacyPolicy_WarZ.htm";
		info.pszLangFilePath     = langFile;

		int res = crInstallW(&info);
		if(res !=0)
		{
			// Something goes wrong. Get error message.
			TCHAR szErrorMsg[512] = _T("");        
			crGetLastErrorMsg(szErrorMsg, 512);    
			r3dOutToLog(("%s\n"), szErrorMsg);    
			//return 1;
		}

		// add files to crash report
		char filePath[1024];
		res = crAddFile2(_T("r3dlog.txt"), NULL, _T("Log file"), CR_AF_MAKE_FILE_COPY|CR_AF_MISSING_FILE_OK);
		CreateConfigPath(filePath);
		strcat(filePath, "gameSettings.ini");
		res = crAddFile2(filePath, NULL, _T("Game Settings file"), CR_AF_MAKE_FILE_COPY|CR_AF_MISSING_FILE_OK);
		CreateConfigPath(filePath);
		strcat(filePath, "GPU.txt");
		res = crAddFile2(filePath, NULL, _T("GPU information file"), CR_AF_MAKE_FILE_COPY|CR_AF_MISSING_FILE_OK);
#endif
	}

	//crEmulateCrash(CR_CPP_NEW_OPERATOR_ERROR);
	
	fn(in);

#ifndef DISABLE_CRASHRPT
	if(!IsDebuggerPresent())
	{
		crUninstall();
	}
#endif
}

//------------------------------------------------------------------------
#ifndef FINAL_BUILD
static r3dTL::TArray< int > g_BreakPointThreads( 64 );
#endif

static R3D_FORCEINLINE void SetBits(unsigned long& dw, int lowBit, int bits, int newValue)
{
	int mask = (1 << bits) - 1; // e.g. 1 becomes 0001, 2 becomes 0011, 3 becomes 0111

	dw = (dw & ~(mask << lowBit)) | (newValue << lowBit);
}

int r3dSetDataBreakpoint( void* address, r3dDataBreakpointByteLen byteLen, int condition )
{
#ifndef FINAL_BUILD
	g_BreakPointThreads.Clear();

	HANDLE hThreadSnap = INVALID_HANDLE_VALUE; 
	THREADENTRY32 te32;

	// Take a snapshot of all running threads  
	hThreadSnap = CreateToolhelp32Snapshot( TH32CS_SNAPTHREAD, 0 ); 
	if( hThreadSnap == INVALID_HANDLE_VALUE ) 
		return 0;

	// Fill in the size of the structure before using it. 
	te32.dwSize = sizeof(THREADENTRY32 ); 

	// Retrieve information about the first thread,
	// and exit if unsuccessful
	if( !Thread32First( hThreadSnap, &te32 ) ) 
	{
		return 0;
	}

	DWORD currProcessID = GetCurrentProcessId();

	// Now walk the thread list of the system,
	// and display information about each thread
	// associated with the specified process
	do 
	{ 
		if( te32.th32OwnerProcessID == currProcessID )
		{
			g_BreakPointThreads.PushBack( te32.th32ThreadID );
		}
	} while( Thread32Next(hThreadSnap, &te32 ) );

	//  Don't forget to clean up the snapshot object.
	CloseHandle( hThreadSnap );

	//------------------------------------------------------------------------

	int currentThreadId = GetCurrentThreadId();

	for( int i = 0; i < (int)g_BreakPointThreads.Count(); i ++ )
	{
		CONTEXT cxt;

		int threadId = g_BreakPointThreads[ i ];

		HANDLE threadHandle = OpenThread( THREAD_ALL_ACCESS, FALSE, threadId );

		if( threadHandle == INVALID_HANDLE_VALUE )
			return 0;

		if( threadId != currentThreadId )
			SuspendThread( threadHandle );

		int len = 0;

		switch( byteLen )
		{
		case R3D_DATABREAKPOINT_1_BYTE: len = 0; break;
		case R3D_DATABREAKPOINT_2_BYTES: len = 1; break;
		case R3D_DATABREAKPOINT_4_BYTES: len = 3; break;
		default: r3d_assert(false); // invalid length
		}

		// The only registers we care about are the debug registers
		cxt.ContextFlags = CONTEXT_DEBUG_REGISTERS;

		// Read the register values
		if( !GetThreadContext(threadHandle, &cxt) )
		{
			CloseHandle( threadHandle );
			return 0;
		}

		// Find an available hardware register

		if( condition )
		{
			int index = 0;
			for( index = 0; index < 4; ++index )
			{
				if( ( cxt.Dr7 & ( 1 << ( index * 2 ) ) ) == 0 )
					break;
			}

			r3d_assert( index < 4 ); // All hardware breakpoint registers are already being used

			switch ( index )
			{
			case 0: cxt.Dr0 = (DWORD) address; break;
			case 1: cxt.Dr1 = (DWORD) address; break;
			case 2: cxt.Dr2 = (DWORD) address; break;
			case 3: cxt.Dr3 = (DWORD) address; break;
			default: assert(false); // m_index has bogus value
			}

			SetBits(cxt.Dr7, 16 + (index*4), 2, condition);
			SetBits(cxt.Dr7, 18 + (index*4), 2, len);
			SetBits(cxt.Dr7, index*2,        1, 1);
		}
		else
		{
			for( int i = 0; i < 4; i ++ )
			{
				int isSet = 0;
				switch( i )
				{
				case 0 : if( (DWORD)address == cxt.Dr0 ) isSet = 1; break;
				case 1 : if( (DWORD)address == cxt.Dr1 ) isSet = 1; break;
				case 2 : if( (DWORD)address == cxt.Dr2 ) isSet = 1; break;
				case 3 : if( (DWORD)address == cxt.Dr3 ) isSet = 1; break;
				}

				if( isSet )
				{
					SetBits(cxt.Dr7, 16 + (i*4), 2, 0 );
					SetBits(cxt.Dr7, 18 + (i*4), 2, R3D_DATABREAKPOINT_1_BYTE );
					SetBits(cxt.Dr7, i*2,        1, 0 );
				}
			}
		}


		// Write out the new debug registers

		int failed = 0;

		if ( !SetThreadContext(threadHandle, &cxt) )
			failed = 1;


		if( threadId != currentThreadId )
			ResumeThread( threadHandle );

		CloseHandle( threadHandle );

		if( failed )
			return 0;
	}
#endif
	return 1;
}

void r3dRemoveDataBreakpoint( void* address )
{
	r3dSetDataBreakpoint( address, R3D_DATABREAKPOINT_1_BYTE, 0 );
}

void* r3dGetMainModuleBaseAddress()
{
	HANDLE hModuleSnap = CreateToolhelp32Snapshot( TH32CS_SNAPMODULE, 0 ); 
	if( hModuleSnap == INVALID_HANDLE_VALUE ) 
		return 0;

	MODULEENTRY32 ment;

	ment.dwSize = sizeof( MODULEENTRY32 );

	void* result( NULL );

	if( Module32First( hModuleSnap, &ment ) )
	{
		do 
		{ 
			if( stristr( ment.szModule, ".exe" ) )
			{
				result = ment.modBaseAddr;
				break;
			}
		} while( Module32Next( hModuleSnap, &ment ) ); 
	}

	CloseHandle( hModuleSnap );

	return result;
}