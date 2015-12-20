//----------------------------------------------------------------------------------------------------------------------
/// \file   FileSystem.cpp
/// \date   11-9-2004
/// \author Rob Bateman [http://robthebloke.org]
/// \brief  This file defines some moderately file system funcs. 
/// \note   (C) Copyright 2003-2005 Robert Bateman. All rights reserved.
//----------------------------------------------------------------------------------------------------------------------
#ifdef WIN32
# ifndef WIN32_EXTRA_LEAN
#  define WIN32_EXTRA_LEAN
# endif
# include <windows.h>
#else
# include <unistd.h>
# include <dirent.h>
#endif
#include <algorithm>
#include "XMU/FileSystem.h"
#include "nmtl/algorithm.h"
namespace XMU
{
//----------------------------------------------------------------------------------------------------------------------
FileInfo::FileInfo(const FileInfo& info)
  : name(info.name),path(info.path),fileSize(info.fileSize),m_Modified(info.m_Modified)
{
}

//----------------------------------------------------------------------------------------------------------------------
FileInfo::FileInfo(const XMD::XChar* filename)
{
  Set(filename);
}

//----------------------------------------------------------------------------------------------------------------------
FileInfo::FileInfo(const XMD::XChar* fullpath,const XMD::XChar* filename)
{
  Set(fullpath,filename);
}

//----------------------------------------------------------------------------------------------------------------------
FileInfo::~FileInfo()
{
}

//----------------------------------------------------------------------------------------------------------------------
XMD::XU32 FileInfo::GetSize() const
{
  return fileSize;
}

//----------------------------------------------------------------------------------------------------------------------
bool FileInfo::Set(const XMD::XChar* filename)
{
  path=name="";
  fileSize=0;

  // get last modified file time
  m_Modified = XMD::XTimeStamp(filename);

  // the name of the file
  path = filename;

  // convert all '\' to '/' chars
  for(XMD::XU32 i=0;i!=path.size();++i)
  {
    if(path[i] == '\\')
      path[i] = '/';
  }

  // get path size
  XMD::XU32 sz = static_cast<XMD::XU32>(path.size());

  // loop back through file till we reach last '/'
  while(sz > 0 && path[--sz] != '/') ;

  // if no path
  if(sz==0)
  {
    name = path;
    path = "./";
    fileSize = FileSize(name.c_str());
  }
  else
  {
    name = path.c_str() + sz + 1;
    path.resize(sz);
  }
  fileSize = FileSize(filename);
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
XMD::XU32 FileInfo::FileSize(const XMD::XChar* filename)
{
  XMD::XU32 fsize =0;
  #ifdef WIN32
    HANDLE file = CreateFileA( filename,
                READ_CONTROL,
                0,
                NULL,
                OPEN_EXISTING,
                FILE_FLAG_BACKUP_SEMANTICS,
                NULL );

    if(!file)
      return 0;

    // the size in bytes of the file
    fsize = GetFileSize(file,NULL);

    // close the file, we're done
    CloseHandle(file);

  #else

    FILE* fp = fopen(filename,"rb");

    if(!fp)
      return 0;

    fseek(fp,0,SEEK_END);
    fsize = ftell(fp);

    fclose(fp);

  #endif

  return fsize;
}

//----------------------------------------------------------------------------------------------------------------------
bool FileInfo::Set(const XMD::XChar* fullpath,const XMD::XChar* filename)
{
  path = fullpath;

  // convert all '\' to '/' chars
  for(XMD::XU32 i=0;i!=path.size();++i)
  {
    if(path[i] == '\\')
      path[i] = '/';
  }

  if(path.size()==0)
  {
    path = "./";
  }
  else
  if(path[path.size()-1] != '/' )
    path += "/";

  name = filename;
  fileSize = FileSize( GetFullName().c_str() );

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool FileInfo::IsValid() const
{
  if(name!="")
  {
    return true;
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
const XMD::XString FileInfo::GetFileBase() const
{

  XMD::XS32 i=static_cast<XMD::XS32>(name.size());
  while(i>=0)
  {
    if( name[i] == '.' )
    {
      break;
    }
    --i;
  }

  if(i<0)
    return GetFileName();

  return name.c_str() + i + 1;
}

//----------------------------------------------------------------------------------------------------------------------
const XMD::XString& FileInfo::GetFileName() const
{
  return name;
}

//----------------------------------------------------------------------------------------------------------------------
const XMD::XString FileInfo::GetFullName() const
{
  return path + name;
}

//----------------------------------------------------------------------------------------------------------------------
const XMD::XString& FileInfo::GetPath() const
{
  return path;
}

//----------------------------------------------------------------------------------------------------------------------
const XMD::XString FileInfo::GetExtension() const
{
  XMD::XS32 i=static_cast<XMD::XS32>(name.size());
  while(i>=0)
  {
    if( name[i] == '.' )
    {
      return name.c_str() + i + 1;
    }
    --i;
  }
  return "";
}

//----------------------------------------------------------------------------------------------------------------------
bool FileInfo::IsDirectory() const
{
  return DirExists( GetFullName().c_str() );
}

//----------------------------------------------------------------------------------------------------------------------
const XMD::XTimeStamp& FileInfo::GetModifiedTime() const
{
  return m_Modified;
}

//----------------------------------------------------------------------------------------------------------------------
DirLister::DirLister() : m_Path(),m_ExtFilter(),m_Sort(true)
{
  CurrentWorkingDir();
}

//----------------------------------------------------------------------------------------------------------------------
void DirLister::Clear()
{
  clear();
}

//----------------------------------------------------------------------------------------------------------------------
bool DirLister::SetPath(const XMD::XChar* p)
{
  // copy new path and make all '\' unix '/' types
  XMD::XString newpath = p;
  XMD::XString relpath = m_Path;
  for(XMD::XU32 i=0;i!=newpath.size();++i)
  {
    if(newpath[i]=='\\')
      newpath[i] = '/';
  }

  // if this directory
  if(newpath==".")
  {
    // do nothing
  }
  else
  // if parent directory
  if(newpath=="..")
  {
    relpath = m_Path;

    // test for C:/ or / as smallest possible path
    #ifdef WIN32
    if(m_Path.size() > 3)
    {
    #else
    if(m_Path.size() > 1)
    {
    #endif

      XMD::XU32 sz = static_cast<XMD::XU32>(relpath.size()) - 2;
      while( sz > 0 && relpath[sz] != '/')
      {
        --sz;
      }
      relpath.resize(sz);

      m_Path = relpath;
    }

  }
  else
  // if relative to some parent dir
  if(newpath[0] == '.' && newpath[1] == '.' && newpath[2] == '/')
  {
    while(newpath[0] == '.' && newpath[1] == '.' && newpath[2] == '/')
    {
      newpath = newpath.c_str() + 3;
      XMD::XU32 sz = static_cast<XMD::XU32>(relpath.size()) - 2;
      while( sz > 0 && relpath[sz] != '/')
      {
        --sz;
      }
      relpath.resize(sz);
    }
    newpath = relpath + newpath;
    if(DirExists(newpath.c_str()))
    {
      m_Path = newpath;
    }
  }
  else
  // check for absolute path
  #ifdef WIN32
  if(newpath[1] == ':' && newpath[2] == '/' )
  {
    if(DirExists(newpath.c_str()))
    {
      m_Path = newpath;
    }
  }
  #else
  if(newpath[0] == '/' )
  {
    if(DirExists(newpath.c_str()))
    {
      m_Path = newpath;
    }
  }
  #endif
  // if relative to current dir
  else
  {
    // if we need to strip the ./ from the front of the string
    if(newpath[0] == '.' && newpath[1] == '/')
    {
      newpath = newpath.c_str()+2;
    }
    relpath = m_Path + newpath;
    if(DirExists(relpath.c_str()))
    {
      m_Path = relpath;
    }
  }
  List();

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
const XMD::XChar* DirLister::GetPath() const
{
  return m_Path.c_str();
}

//----------------------------------------------------------------------------------------------------------------------
XMD::XU32 DirLister::List()
{
  clear();
  if(GetDirContents( *this ,m_Path.c_str(),m_ExtFilter.c_str()))
  {
    if(m_Sort)
      nmtl::sort(begin(),end());
    return static_cast<XMD::XU32>(size());
  }
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
void DirLister::EnableSort(bool is_sorted)
{
  m_Sort = is_sorted;
  List();
}

//----------------------------------------------------------------------------------------------------------------------
XMD::XU32 DirLister::GetNumFiles() const
{
  return static_cast< XMD::XU32 >( size() );
}

//----------------------------------------------------------------------------------------------------------------------
const XMD::XChar* DirLister::GetFileName(XMD::XU32 idx) const
{
  if(idx<GetNumFiles())
  {
    return at(idx).c_str();
  }
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
FileInfo DirLister::GetFileInfo(XMD::XU32 idx) const
{
  return FileInfo( m_Path.c_str(), GetFileName(idx) );
}

//----------------------------------------------------------------------------------------------------------------------
void DirLister::CurrentWorkingDir()
{
  XMD::XChar buffer[512];

  #ifdef WIN32
  GetCurrentDirectoryA(512,buffer);
  #else
  getcwd(buffer,512);
  #endif

  m_Path = buffer;

  List();
}

//----------------------------------------------------------------------------------------------------------------------
void DirLister::SetExtensionFilter(const XMD::XChar* extension)
{
  m_ExtFilter = extension;
  List();
}

#ifdef WIN32
//----------------------------------------------------------------------------------------------------------------------
bool DirExists(const XMD::XChar* fname)
{
  // store current directory path
  XMD::XChar buffer[512];
  GetCurrentDirectoryA(512,buffer);

  // attempt to goto specified directory. store returned flag
  DWORD res = SetCurrentDirectoryA(fname);

  // reset what was the previous current working dir
  SetCurrentDirectoryA(buffer);

  // if succeeded in changing to specified dir, return true
  return res == TRUE;
}

//----------------------------------------------------------------------------------------------------------------------
bool GetDirContents(XMD::XStringList& returned,const XMD::XChar* dirname,const XMD::XChar* ext)
{
  returned.clear();

  // we need to build a search string for FindFirstFile
  XMD::XString s = dirname;

  // make sure we have a '/' at the end of the path
  if(s[s.size()-1]!='/' && s[s.size()-1]!='\\')
  {
    s += '\\';
  }

  // we want  "path/*ext" for the Win32 search string
  s +="*";
  s += ext;

  // structure to hold returned file info
  WIN32_FIND_DATAA wfind = {0};

  // get handle to start the search with
  HANDLE hfind = FindFirstFileA( s.c_str(), &wfind );

  // check valid
  if( hfind == INVALID_HANDLE_VALUE )
  {
    return false;
  }

  // read remaining files
  do
  {
    returned.push_back( wfind.cFileName );
  }
  while( FindNextFileA(hfind, &wfind) != 0 );

  // stop searching
  FindClose(hfind);

  // done
  return true;
}

#else

//----------------------------------------------------------------------------------------------------------------------
bool DirExists(const XMD::XChar* fname)
{
  // attempt to open the specified dir
  DIR* pdir = opendir(fname);

  // if OK, close dir and return true
  if(pdir)
  {
    closedir(pdir);
    return true;
  }

  // could not open dir, probably not a valid dir path
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool GetDirContents(XMD::XStringList& returned,const XMD::XChar* dirname,const XMD::XChar* ext)
{
  returned.clear();

  // length of ext to check. Ignore '\0' as an optimisation...
  XMD::XU32 extlen = 0;
  if(ext)
    extlen = strlen(ext);

  // open directory
  DIR* pdir = opendir(dirname);
  if(pdir)
  {

    dirent* entry = 0;

    // loop through each directory entry
    while( (entry = readdir(pdir)) != NULL )
    {

      // if we are checking for a specific extension
      if(ext)
      {
        XMD::XU32 namelen = strlen( entry->d_name );
        if(namelen>extlen)
        {
          // if extension doesn't match, continue to next entry
          if( strcmp(entry->d_name+namelen-extlen,ext) )
            continue;
        }
        else continue;
      }
      // passed our rather lame conditions, add it to the returned list
      returned.push_back(entry->d_name);
    }

    // finished reading directory
    closedir(pdir);

    return true;
  }

  return false;
}
#endif
}
