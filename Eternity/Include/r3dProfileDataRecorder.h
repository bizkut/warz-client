//=========================================================================
//	Module: r3dProfileDataRecorder.h
//	Copyright (C) 2011.
//=========================================================================

#pragma once
#include "r3dString.h"

//////////////////////////////////////////////////////////////////////////

extern r3dString gProfileDataXMLOutFile;
extern r3dString gProfileDataTXTOutFile;
extern float gScheduledProfileTime;
extern bool gProfileD3DFromCommandLine;
//	Delay (in seconds) after loading screen was removed.

void DumpProfileData();
void ScheduleProfileDataDump();
void DumpAutomaticProfileInfo( const char* LevelFolder );
bool DoDumpProfilerData();
bool CreateProfileDumpFolders( const char* LevelFolder );
void DumpProfileScreenShot();

