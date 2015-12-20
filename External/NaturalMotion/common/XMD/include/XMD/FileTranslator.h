//----------------------------------------------------------------------------------------------------------------------
/// \file   FileTranslator.h
/// \author Robert Bateman    mailto:robthebloke@hotmail.com
/// \brief  defines an interface for a plugin file translator type
/// \note   (C) Copyright 2003-2005 Robert Bateman. All rights reserved.
//----------------------------------------------------------------------------------------------------------------------
#pragma once
//----------------------------------------------------------------------------------------------------------------------
#include "XMD/XFn.h"
//----------------------------------------------------------------------------------------------------------------------
namespace XMD
{
#ifndef DOXYGEN_PROCESSING
// forward declaration
class XMD_EXPORT XModel;
#endif

//----------------------------------------------------------------------------------------------------------------------
/// \class XMD::XFileTranslator
/// \brief Allows you to add support for additional file formats into the XMD 
///        SDK so that you don't have to modify any additional 
//----------------------------------------------------------------------------------------------------------------------
class XMD_EXPORT XFileTranslator
{
public:

  /// \brief  ctor
  XFileTranslator();

  /// \brief  dtor
  virtual ~XFileTranslator();

  /// \brief  override to import a file into the XMD framework
  /// \param  filename  - the name of the file to import
  /// \param  model - the model to import into
  virtual bool Import(const XChar* filename, XModel& model);

  /// \brief  override to import an animation from the file into the XMD model provided. 
  /// \param  filename  - the name of the file to import
  /// \param  model - the model to import into
  virtual bool ImportAnim(const XChar* filename, XModel& model);

  /// \brief  override to export the model file provided as an XMD file.
  /// \param  filename  - the name of the file to import
  /// \param  model - the model to import into
  virtual bool Export(const XChar* filename, XModel& model);

  /// \brief  override and return true if your derived class supports export methods
  /// \return true if export is supported
  virtual bool HasExport() const;

  /// \brief  override and return true if your derived class supports import methods
  /// \return true if export is supported
  virtual bool HasImport() const;

  /// \brief  override and return true if your derived class supports import methods to
  ///         import animation data from the file and attach to the currently loaded model. 
  /// \return true if export is supported
  virtual bool HasImportAnim() const;
};
}
