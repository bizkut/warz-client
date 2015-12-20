//----------------------------------------------------------------------------------------------------------------------
/// \file   ImportPolicy.h
/// \author Rob Bateman [http://robthebloke.org]
/// \brief  This file specifies the policies available when deleting nodes
///         from XMD. This is primarily to allow for some flexibility when
///         deleting a shape as to whether it deletes just the node you told
///         it to, or all related materials & deformers.
/// \note   (C) Copyright 2003-2005 Robert Bateman. All rights reserved.
//----------------------------------------------------------------------------------------------------------------------
#pragma once
//----------------------------------------------------------------------------------------------------------------------
#include "XMD/XFn.h"
//----------------------------------------------------------------------------------------------------------------------
namespace XMD 
{
/// maximum allowable import policies that can be placed on the XImportPolicy stack.
#define XMD_MAX_IMPORT_STACK 32

//----------------------------------------------------------------------------------------------------------------------
/// \class XMD::XImportPolicy
/// \brief a class to determine how files are loaded into the XMD lib. 
//----------------------------------------------------------------------------------------------------------------------
class XMD_EXPORT XImportPolicy
{
public:

  /// \brief  If this flag is set, all XBone derived classes will be imported from the input file
  static const XU32 ImportBones = 0x00000001;

  /// \brief  If this flag is set, all XShape derived classes will be imported from the input file
  static const XU32 ImportShapes = 0x00000002;

  /// \brief  If this flag is set, all XMaterial derived classes will be imported from the input file
  static const XU32 ImportMaterials = 0x00000004;

  /// \brief  If this flag is set, all XTexture derived classes will be imported from the input file
  static const XU32 ImportTextures = 0x00000008;

  /// \brief  If this flag is set, all particle systems, emitters and dynamics fields
  static const XU32 ImportParticles = 0x00000010;

  /// \brief  If this flag is set, all XDeformer derived classes will be imported from the input file
  static const XU32 ImportDeformers = 0x00000020;

  /// \brief  If this flag is set, all custom attributes will be imported from the input file
  static const XU32 ImportAttributes = 0x00000040;

  /// \brief  If this flag is set, all animation data will be imported
  static const XU32 ImportAnimations = 0x00000080;

  /// ctor
  XImportPolicy();

  /// \brief  dtor
  ~XImportPolicy();

  /// \brief  copy ctor
  /// \param  dp  - the policy to copy
  XImportPolicy(const XImportPolicy& dp);

  /// \brief  allows you to enable an option (or set of flag options in a bitfield)
  /// \param  flag  - the flags to enable
  void Enable(XU32 flag);

  /// \brief  allows you to disable an option (or set of flag options in a bitfield)
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
  XU32 m_Flags[XMD_MAX_IMPORT_STACK];

  /// the current element on the stack
  XU32 m_Current;
};
}
