//----------------------------------------------------------------------------------------------------------------------
/// \file   ExportPolicy.h
/// \author Rob Bateman [http://robthebloke.org]
/// \brief  This file specifies the policies available when exporting nodes
///         from XMD. This is primarily to allow for some flexibility when
///         writing out an XMD file. 
/// \note   (C) Copyright 2003-2005 Robert Bateman. All rights reserved.
//----------------------------------------------------------------------------------------------------------------------
#pragma once
//----------------------------------------------------------------------------------------------------------------------
#include "XMD/XFn.h"
//----------------------------------------------------------------------------------------------------------------------
namespace XMD 
{
//----------------------------------------------------------------------------------------------------------------------
/// maximum allowable export policies that can be placed on the XExportPolicy
/// stack.
#define XMD_MAX_EXPORT_STACK 32

//----------------------------------------------------------------------------------------------------------------------
/// \class XMD::XExportPolicy
/// \brief A class that determines what will be written out from an XMD
///        file. By default, everything is enabled to be written. This
///        policy can then be used to control what gets output when the 
///        file is written. 
///          This class can also simplify the process of writing your own
///        export options for a specific file format when using the 
///        XMD::XFileTranslator interface.
//----------------------------------------------------------------------------------------------------------------------
class XMD_EXPORT XExportPolicy
{
public:

  /// \brief  If this flag is set, all XBone derived classes will be written
  /// to the XMD file
  static const XU32 WriteBones = 0x00000001;

  /// \brief  if true, all shapes and geometry objects will be written to the 
  ///         output XMD file.
  static const XU32 WriteShapes = 0x00000002;

  /// \brief  if this flag is set, materials will be written to the output XMD 
  ///         file
  static const XU32 WriteMaterials = 0x00000004;

  /// \brief  If true, any textures, bump maps or environment maps will 
  ///         be written to the output XMD file. 
  static const XU32 WriteTextures = 0x00000008;

  /// \brief  If true this will write any particles systems and dynamics 
  ///         fields to the output XMD when saving a file.
  static const XU32 WriteParticles = 0x00000010;

  /// \brief  If true this will write geometry deformers to the output 
  ///         XMD when saving a file.
  static const XU32 WriteDeformers = 0x00000020;

  /// \brief  If this flag is set, then additional custom attributes 
  ///         will be written to the output XMD file when saving.
  static const XU32 WriteAttributes = 0x00000040;

  /// \brief  If this flag is set, thenanimation data
  ///         will be written to the output XMD file when saving.
  static const XU32 WriteAnimations = 0x00000080;

  /// \brief  If this flag is set, an ascii XMD file will be written;
  ///         otherwise the binary version of the format will be saved.
  static const XU32 WriteAscii = 0x00000100;

  /// ctor
  XExportPolicy();

  /// \brief  dtor
  ~XExportPolicy();

  /// \brief  copy ctor
  /// \param  dp  - the policy to copy
  XExportPolicy(const XExportPolicy& dp);

  /// \brief  allows you to enable an option (or set of flag options 
  ///         in a bitfield)
  /// \param  flag  - the flags to enable
  void Enable(XU32 flag);

  /// \brief  allows you to disable an option (or set of flag options 
  ///         in a bitfield)
  /// \param  flag  - the flags to disable
  void Disable(XU32 flag);

  /// \brief  quick func to enable all flags
  void EnableAll();

  /// \brief  quick func to disable all flags
  void DisableAll();

  /// \brief  a quick query func to see is a specified flag is enabled
  /// \param  flag  - the flag to check
  /// \return true if enabled
  bool IsEnabled(XU32 flag) const;

  /// \brief  stores the current set of options so you can modify them
  ///         and revert to the previous set using Pop().
  void Push();

  /// \brief  reverts the options to the previously stored set of values
  void Pop();

private:

  /// the internally stored flags
  XU32 m_Flags[XMD_MAX_EXPORT_STACK];

  /// the current element on the stack
  XU32 m_Current;
};
}
