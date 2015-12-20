/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/

/**************************************************************************

PublicHeader:   Kernel
Filename    :   KY_SysFile.h
Content     :   Header for all internal file management-
                functions and structures to be inherited by OS
                specific subclasses.
Created     :   July 29, 1998
Authors     :   Michael Antonov, Brendan Iribe, Andrew Reisse

Notes       :   errno may not be preserved across use of GBaseFile member functions
            :   Directories cannot be deleted while files opened from them are in use
                (For the GetFullName function)

**************************************************************************/

#ifndef INC_KY_Kernel_SysFile_H
#define INC_KY_Kernel_SysFile_H

#include "gwnavruntime/kernel/SF_File.h"

namespace Kaim {

// ***** Declared classes
class   SysFile;

// *** File Statistics

// This class contents are similar to _stat, providing
// creation, modify and other information about the file.
struct FileStat
{
    // No change or create time because they are not available on most systems
    SInt64  ModifyTime;
    SInt64  AccessTime;
    SInt64  FileSize;

    bool operator== (const FileStat& stat) const
    {
        return ( (ModifyTime == stat.ModifyTime) &&
                 (AccessTime == stat.AccessTime) &&
                 (FileSize == stat.FileSize) );
    }
};

// *** System File

// System file is created to access objects on file system directly
// This file can refer directly to path.
// System file can be open & closed several times; however, such use is not recommended
// This class is realy a wrapper around an implementation of File interface for a 
// particular platform.

class SysFile : public DelegatedFile
{
protected:
  SysFile(const SysFile &source) : DelegatedFile () { KY_UNUSED(source); }
public:

    // ** Constructor
    KY_EXPORT SysFile();
    // Opens a file
    // For some platforms (PS2,PSP,Wii) buffering will be used even if Open_Buffered flag is not set because 
    // of these platforms' file system limitation.
    KY_EXPORT SysFile(const String& path, int flags = Open_Read|Open_Buffered, int mode = Mode_ReadWrite); 

    // ** Open & management 
    // Will fail if file's already open
    // For some platforms (PS2,PSP,Wii) buffering will be used even if Open_Buffered flag is not set because 
    // of these platforms' file system limitation.
    KY_EXPORT bool  Open(const String& path, int flags = Open_Read|Open_Buffered, int mode = Mode_ReadWrite);
        
    KY_INLINE bool  Create(const String& path, int mode = Mode_ReadWrite)
        {   return Open(path, Open_ReadWrite|Open_Create, mode); }

    // Helper function: obtain file statistics information. In GFx, this is used to detect file changes.
    // Return 0 if function failed, most likely because the file doesn't exist.
    KY_EXPORT static bool KY_CDECL GetFileStat(FileStat* pfileStats, const String& path);
    
    // ** Overrides
    // Overridden to provide re-open support
    KY_EXPORT virtual int   GetErrorCode();

    KY_EXPORT virtual bool  IsValid();

    KY_EXPORT virtual bool  Close();
    //SF_EXPORT   virtual bool    CloseCancel();

#ifdef KY_OS_WIIU
	static void initializeSysFileSystem();
	static void shutdownSysFileSystem();
#endif
};

} // Kaim

#endif
