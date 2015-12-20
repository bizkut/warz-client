#include "r3dPCH.h"
#include "r3d.h"
#include <conio.h>

#include "..\..\EclipseStudio\Sources\GameLevel.h"
#include "..\..\EclipseStudio\Sources\GameLevel.cpp"

#include "main.h"

#include "dbghelp.h"
#include "CkByteData.h"
#include "CkZip.h"
#include "CkZipEntry.h"

//
//
//
extern	HANDLE		r3d_CurrentProcess;
class r3dSun * Sun;
r3dCamera gCam;
r3dScreenBuffer * DepthBuffer;
r3dLightSystem	WorldLightSystem;
void r3dScaleformBeginFrame() {}
void r3dScaleformEndFrame() {}
unsigned long DriverUpdater(struct HWND__ *,unsigned long,unsigned long,unsigned long,unsigned long,unsigned long,unsigned long) {return 0; }
void SetNewSimpleFogParams() {};
void SetVolumeFogParams() {};
bool g_bEditMode = false;
//
void game::Shutdown(void) { r3dError("not a gfx app"); }
void game::MainLoop(void) { r3dError("not a gfx app"); }
void game::Init(void)     { r3dError("not a gfx app"); }
void game::PreInit(void)  { r3dError("not a gfx app"); }
//
//
//

	bool		g_DeleteOrgZip             = false;
	char		g_SymbolPath[MAX_PATH]     = "j:\\symbol_cache";
	char		g_CrashRptFiles[MAX_PATH]  = ".";
	char		g_ValidReports[MAX_PATH]   = "valid_reports";
	char		g_UnfixedReports[MAX_PATH] = "unfixed_reports";


static bool ParseArgs(int argc, char* argv[])
{
	if(argc == 1)
	{
		r3dOutToLog("CrashDmpFix usage\n");
		r3dOutToLog(" -y <symbol_path> : symbol storage path\n");
		r3dOutToLog(" -z <CrashRtpDir> : specify directory with crash report zipped files\n");
		r3dOutToLog(" -d               : delete original zip files\n");
		return false;
	}

	for(int i=1; i<argc; i++) 
	{
		if(strcmp(argv[i], "-y") == 0 && (i + 1) < argc)
		{
			r3dscpy(g_SymbolPath, argv[++i]);
			continue;
		}
		if(strcmp(argv[i], "-z") == 0 && (i + 1) < argc)
		{
			r3dscpy(g_CrashRptFiles, argv[++i]);
			continue;
		}
		if(strcmp(argv[i], "-d") == 0)
		{
			g_DeleteOrgZip = true;
			continue;
		}
	}

	return true;
}

ULONG64 GetExceptionAddress(const CkByteData& dmp)
{
	MINIDUMP_DIRECTORY* mdir;
	DWORD ssize = 0;	// stream size

	MINIDUMP_EXCEPTION_STREAM* es;
	if(0 == MiniDumpReadDumpStream((PVOID*)dmp.getBytes(), ExceptionStream, &mdir, (PVOID *)&es, &ssize)) {
		r3dOutToLog("MiniDumpReadDumpStream ExceptionStream failed\n");
		return 0;
	}
	
	return es->ExceptionRecord.ExceptionAddress;
}

void GetMiniDumpModuleName(const char* dmpData, MINIDUMP_MODULE& mm, char* mname)
{
	char fullname[MAX_PATH];
	
	// convert from unicode to ansi
	DWORD len = (*(DWORD*)(dmpData + mm.ModuleNameRva)) / 2;
	const char* uniName = dmpData + mm.ModuleNameRva + 4;
	for(DWORD i=0; i<len; i++) {
		fullname[i] = uniName[i*2];
	}
	fullname[len] = 0;
	
	const char* lim = strrchr(fullname, '\\');
	if(lim) 
		strcpy(mname, lim + 1);
	else
		strcpy(mname, fullname);
}

bool GetExceptionModule(const CkByteData& dmpDataF, char* crashmodule)
{
	const char* dmpData = (char*)dmpDataF.getBytes();

	MINIDUMP_DIRECTORY* mdir;
	DWORD ssize = 0;	// stream size

	// get exception address
	MINIDUMP_EXCEPTION_STREAM* es;
	if(0 == MiniDumpReadDumpStream((PVOID*)dmpData, ExceptionStream, &mdir, (PVOID *)&es, &ssize)) {
		r3dOutToLog("MiniDumpReadDumpStream ExceptionStream failed\n");
		return false;
	}
	ULONG64 exAddr = es->ExceptionRecord.ExceptionAddress;

	// search for modules
	MINIDUMP_MODULE_LIST* ml;
	if(0 == MiniDumpReadDumpStream((PVOID*)dmpData, ModuleListStream, &mdir, (PVOID *)&ml, &ssize)) {
		r3dOutToLog("MiniDumpReadDumpStream ModuleListStream failed\n");
		return false;
	}
	
	for(ULONG32 i=0; i<ml->NumberOfModules; i++)
	{
		if(exAddr >= ml->Modules[i].BaseOfImage && exAddr <= (ml->Modules[i].BaseOfImage + ml->Modules[i].SizeOfImage))
		{
			char mname[MAX_PATH];
			GetMiniDumpModuleName(dmpData, ml->Modules[i], mname);
			
			sprintf(crashmodule, "%s+%08I64X", mname, exAddr - ml->Modules[i].BaseOfImage);
			return true;
		}
	}
	
	sprintf(crashmodule, "%08I64X", exAddr);
	return true;
}

DWORD GetImageSizeFromSymbol(DWORD TimeDateStamp)
{
	WIN32_FIND_DATA ffblk;
	char fname[MAX_PATH];
	sprintf(fname, "%s\\Infestation.exe\\%08X*", g_SymbolPath, TimeDateStamp);
	HANDLE h = FindFirstFile(fname, &ffblk);
	if(h == INVALID_HANDLE_VALUE) {
		r3dOutToLog("no symbol files found in %s\n", fname);
		return 0;
	}
	
	// should be only one entry
	DWORD size;
	sscanf(ffblk.cFileName + 8, "%X", &size);
	FindClose(h);
	
	return size;
}

DWORD GetImageSizeFromDump(CkByteData& dmpData)
{
	MINIDUMP_DIRECTORY* mdir;
	DWORD ssize = 0;	// stream size

	MINIDUMP_MODULE_LIST* ml;
	if(0 == MiniDumpReadDumpStream((PVOID*)dmpData.getBytes(), ModuleListStream, &mdir, (PVOID *)&ml, &ssize)) {
		r3dOutToLog("MiniDumpReadDumpStream ModuleListStream failed\n");
		return 0;
	}

	DWORD newSize = GetImageSizeFromSymbol(ml->Modules[0].TimeDateStamp);
	return newSize;
}

void FixDumpSize(const char* dumpfname, DWORD newSize)
{
	CkByteData dmpDataF;
	if(!dmpDataF.loadFile(dumpfname)) {
		r3dOutToLog("failed to load %s\n", dumpfname);
		return;
	}
	
	void* dmpData = (void*)dmpDataF.getBytes();
	
	MINIDUMP_DIRECTORY* mdir;
	DWORD ssize = 0;	// stream size

	MINIDUMP_MODULE_LIST* ml;
	if(0 == MiniDumpReadDumpStream(dmpData, ModuleListStream, &mdir, (PVOID *)&ml, &ssize)) {
		r3dOutToLog("MiniDumpReadDumpStream ModuleListStream failed\n");
		return;
	}

	DWORD oldSize = ml->Modules[0].SizeOfImage;
	ml->Modules[0].SizeOfImage = newSize;

	if(!dmpDataF.saveFile(dumpfname)) {
		r3dOutToLog("failed to save %s\n", dumpfname);
		return;
	}
	
	//r3dOutToLog("%d->%d\n", oldSize, newSize);
	return;
}

void ProcessReport(const char* zipfile, const char* crashname)
{
	CkZip zip;
	zip.UnlockComponent("ARKTOSZIP_cRvE6e7mpSqD");
	if(!zip.OpenZip(zipfile))
	{
		r3dOutToLog("failed to open %s - %s\n", zipfile, zip.lastErrorText());
		return;
	}
	
	CkZipEntry* zdmp = zip.GetEntryByName("crashdump.dmp");
	if(!zdmp) {
		r3dOutToLog("%s - no crashdump.dmp\n", zipfile);
		return;
	}
	
	CkByteData dmpData;
	if(!zdmp->Inflate(dmpData)) {
		r3dOutToLog("%s - failed to inflate crashdump.dmp\n", zipfile);
		return;
	}
	
	//ULONG64 ExceptionAddr = GetExceptionAddress(dmpData);
	//r3dOutToLog("%s: %08I64X\n", crashname, ExceptionAddr); CLOG_INDENT;
	
	char ExceptionModule[MAX_PATH] = "unknown";
	GetExceptionModule(dmpData, ExceptionModule);
	r3dOutToLog("%s: %s\n", crashname, ExceptionModule); CLOG_INDENT;

	// get fixed exe size
	DWORD newSize         = GetImageSizeFromDump(dmpData);
	
	// extract files to new dir
	char dir[MAX_PATH];
	sprintf(dir, "%s\\%s\\%s\\", newSize ? g_ValidReports : g_UnfixedReports, ExceptionModule, crashname);
	if(!zip.Extract(dir)) {
		r3dOutToLog("%s - failed to extract to %s\n", zipfile, dir);
		return;
	}
	
	if(newSize > 0)
	{
		char dmpfile[MAX_PATH];
		sprintf(dmpfile, "%s\\crashdump.dmp", dir);
		FixDumpSize(dmpfile, newSize);
	}
	
	SAFE_DELETE(zdmp);
	zip.CloseZip();
	
	if(g_DeleteOrgZip)
	{
		::DeleteFile(zipfile);
	}
}

int CountDirsInDir(const char* baseDir, const char* dir)
{
	WIN32_FIND_DATA ffblk;
	char fname[MAX_PATH];
	sprintf(fname, "%s\\%s\\*", baseDir, dir);
	HANDLE h = FindFirstFile(fname, &ffblk);
	if(h == INVALID_HANDLE_VALUE) {
		return 0;
	}

	int numDirs = 0;
	do 
	{
		numDirs++;
	} while(FindNextFile(h, &ffblk) != 0);
	FindClose(h);
	
	return numDirs - 2; // exclude . & ..
}

void RenameValidReportDirs()
{
	WIN32_FIND_DATA ffblk;
	char fname[MAX_PATH];
	sprintf(fname, "%s\\*", g_ValidReports);
	HANDLE h = FindFirstFile(fname, &ffblk);
	if(h == INVALID_HANDLE_VALUE) {
		return;
	}

	// fill array of directories
	std::vector<std::string> dirs;
	do 
	{
		if(ffblk.cFileName[0] == '.')
			continue;
			
		dirs.push_back(ffblk.cFileName);
	} while(FindNextFile(h, &ffblk) != 0);
	FindClose(h);
	
	// rename them with number of inside crash reports
	for(size_t i=0, size=dirs.size(); i < size; ++i)
	{
		int numDirs = CountDirsInDir(g_ValidReports, dirs[i].c_str());

		char fname2[MAX_PATH];
		sprintf(fname,  "%s\\%s", g_ValidReports, dirs[i].c_str());
		sprintf(fname2, "%s\\(%03d) %s", g_ValidReports, numDirs, dirs[i].c_str());
		
		::MoveFile(fname, fname2);
		r3dOutToLog("%s->%s\n", fname, fname2);
	}
}

void ProcessCrashReportFiles()
{
	r3dOutToLog("CrashDmpFix, y=%s, z=%s, d=%d\n\n", g_SymbolPath, g_CrashRptFiles, g_DeleteOrgZip);

	WIN32_FIND_DATA ffblk;
	char fname[MAX_PATH];
	sprintf(fname, "%s\\*.zip", g_CrashRptFiles);
	HANDLE h = FindFirstFile(fname, &ffblk);
	if(h == INVALID_HANDLE_VALUE) {
		r3dOutToLog("no .zip files found\n");
		return;
	}

	do 
	{
		sprintf(fname, "%s\\%s", g_CrashRptFiles, ffblk.cFileName);

		char crashname[MAX_PATH];
		r3dscpy(crashname, ffblk.cFileName);
		crashname[strlen(crashname)-4] = 0; // strip .zip
		
		ProcessReport(fname, crashname);
	} while(FindNextFile(h, &ffblk) != 0);
	FindClose(h);
	
	RenameValidReportDirs();
	
	r3dOutToLog("\nFinished, press any key to exit\n");
	getch();
}

//
// server entry
//
int main(int argc, char* argv[])
{
	extern int _r3d_bLogToConsole;
	_r3d_bLogToConsole = 1;
  
	r3d_CurrentProcess = GetCurrentProcess();
	try
	{
		if(ParseArgs(argc, argv))
		{
			ProcessCrashReportFiles();
		}
	} 
	catch(const char* what)
	{
		r3dOutToLog("!!! Exception: %s\n", what);
		// fall thru
	}
  
	return 0;
}
