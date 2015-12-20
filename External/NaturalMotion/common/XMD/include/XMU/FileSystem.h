//----------------------------------------------------------------------------------------------------------------------
/// \file   FileSystem.h
/// \author Rob Bateman [http://robthebloke.org]
/// \brief  This file defines some moderately useful file system funcs. 
/// \note   (C) Copyright 2003-2005 Robert Bateman. All rights reserved.
//----------------------------------------------------------------------------------------------------------------------
#pragma once
//----------------------------------------------------------------------------------------------------------------------
#include "XMD/Info.h"
//----------------------------------------------------------------------------------------------------------------------
#pragma pack(push,1)
namespace XMU 
{
//----------------------------------------------------------------------------------------------------------------------
/// \brief  This function returns true if the specified directory exists
/// \param  path - the directory name
/// \return  true if exists
//----------------------------------------------------------------------------------------------------------------------
XMD_EXPORT bool DirExists(const XMD::XChar* path);

//----------------------------------------------------------------------------------------------------------------------
/// \brief  This function can be used to return a list of all items found
///     within the sepcified directory.
/// \param  returned  -  the array of returned strings
/// \param  dirpath    -  the path to the directory to read
/// \param  ext      -  the extension you want listed, ie "bmp", "gif"
/// \return  true if directory opened
//----------------------------------------------------------------------------------------------------------------------
XMD_EXPORT bool GetDirContents(XMD::XStringList& returned, const XMD::XChar* dirpath, const XMD::XChar* ext =0);

//----------------------------------------------------------------------------------------------------------------------
/// \class XMU::FileInfo
/// \brief This structure is provided as an easy way to access info about a specific file. This is a little 
///        bit simplistic, but it does the job for most things. 
//----------------------------------------------------------------------------------------------------------------------
struct XMD_EXPORT FileInfo 
{
  /// \brief  ctor
  /// \param  filename - you MUST specify a filename for the FileInfo structure to work.
  FileInfo(const XMD::XChar* filename);

  /// \brief  ctor
  /// \param  filename - you MUST specify a filename for the FileInfo structure to work.
  /// \param  path - the path to the file
  FileInfo(const XMD::XChar* filename, const XMD::XChar* path);

  /// \brief  dtor
  ~FileInfo();

  /// \brief  sets this info struct to investigate the specified file
  /// \param  filename - you MUST specify a filename for the FileInfo structure to work.
  bool Set(const XMD::XChar* filename);

  /// \brief  sets this info struct to investigate the specified file
  /// \param  filename - you MUST specify a filename for the FileInfo structure to work.
  /// \param  path - the path to the file
  bool Set(const XMD::XChar* filename, const XMD::XChar* path);

  /// \brief  returns true if the file being referenced is a valid file
  /// \return true if this is a valid file
  bool IsValid() const;

  /// \brief  returns the base name for a file, ie path/file_base.ext
  /// \return the file_base part of the above string
  const XMD::XString GetFileBase() const;

  /// \brief  returns the base name for a file and it's extension, ie path/file_name.ext
  /// \return  the "file_name.ext" part of the above string
  const XMD::XString& GetFileName() const;

  /// \brief  returns the full file name including path and extension
  /// \return the "file_name.ext" part of the above string
  const XMD::XString GetFullName() const;

  /// \brief  returns the directory path in which this file is contained
  /// \return the directory in which the file is contained
  const XMD::XString& GetPath() const;

  /// \brief  returns the extension of the file
  /// \return the directory in which the file is contained
  const XMD::XString GetExtension() const;

  /// \brief  returns true if this file is a directory
  /// \return true if this file is a directory
  bool IsDirectory() const;

  /// \brief  returns a timestamp for the date and time this file was last modified
  /// \return the time stamp for the file's last modified time
  const XMD::XTimeStamp& GetModifiedTime() const;

  /// \brief  copy ctor
  /// \param  info - the object to copy
  FileInfo(const FileInfo& info);

  /// \brief  returns the size of the file
  /// \return the file size in bytes
  XMD::XU32 GetSize() const;

private:
  /// \brief  returns the file size of the requested file
  /// \param  filename  -  the name of the file
  /// \return the file size in bytes
  XMD::XU32 FileSize(const XMD::XChar* filename);
  /// the name of the file
  XMD::XString name;
  /// the path of the file
  XMD::XString path;
  /// the size in bytes of the file
  XMD::XU32 fileSize;
  /// a time stamp for the date and time the file was last modified
  XMD::XTimeStamp m_Modified;
};

//----------------------------------------------------------------------------------------------------------------------
/// \class XMU::DirLister
/// \brief This structure is provided as an easy way to walk about through some directories and the files they contain.
//----------------------------------------------------------------------------------------------------------------------
class XMD_EXPORT DirLister
  : private XMD::XStringList 
{
public:

  /// \brief  sets the path for the dir lister to function on
  /// \param  filename - you MUST specify a filename for the FileInfo structure to work.
  /// \return true if path set OK
  bool SetPath(const XMD::XChar* filename);

  /// \brief  returns the current directory path
  const XMD::XChar* GetPath() const;

  /// \brief  This lists the file again within the directory, but allows you to specify an extension to use. 
  ///         See SetExtensionFilter to see how to get the list to filter out specific options. You shouldn't really 
  ///         need to use this since the class is largely automated when moving around directories. Use this to refresh
  ///         the directory listing....
  /// \return number of files listed this time
  /// \note   This is automatically called by SetPath, CurrentWorkngDirectory, EnableSort and SetExtensionFilter. 
  XMD::XU32 List();

  /// \brief  This function allows you to switch between sorted and un-sorted directory modes. The default is 
  ///         for a sorted directory.
  /// \param  is_sorted - if true, all directory listings will be sorted alphabetically. If false they will 
  ///         be in the order read from the disk.
  void EnableSort(bool is_sorted);

  /// \brief  returns the number of files in this current directory listing.
  /// \return the number of files in the directory listing
  XMD::XU32 GetNumFiles() const;

  /// \brief  returns the filename of the file at the specified index.
  /// \param  idx  -  the index of the file name you want
  /// \return the filename or NULL if invalid index
  const XMD::XChar* GetFileName(XMD::XU32 idx) const;

  /// \brief  returns a file info structure about the file at index 'idx' in the directory listing.
  /// \param  idx  -  the index to query
  /// \return a file info structure
  FileInfo GetFileInfo(XMD::XU32 idx) const;

  /// \brief  Sets this instance to the current working directory
  void CurrentWorkingDir();

  /// \brief  This provides a limited way to filter out specific directory results. This only works by 
  ///         checking the extension with the files extensions.
  void SetExtensionFilter(const XMD::XChar* extension);

  /// \brief  ctor. Will be initialised to the CWD
  DirLister();

private:
  void Clear();
  /// the path to this directory
  XMD::XString m_Path;
  /// the path to this directory
  XMD::XString m_ExtFilter;
  /// should the dir contents be sorted
  bool m_Sort;
};
}
#pragma pack(pop)
