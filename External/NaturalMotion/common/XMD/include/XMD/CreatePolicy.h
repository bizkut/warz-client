//----------------------------------------------------------------------------------------------------------------------
/// \file   CreatePolicy.h
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
//----------------------------------------------------------------------------------------------------------------------
/// maximum allowable create policies that can be placed on the XCreatePolicy
/// stack.
#define XMD_MAX_CREATE_STACK 32

//----------------------------------------------------------------------------------------------------------------------
/// \class XMD::XCreatePolicy
/// \brief By default, the create policy is set to create only the node 
///        requested. 
//----------------------------------------------------------------------------------------------------------------------
class XMD_EXPORT XCreatePolicy
{
public:

  /// should we delete node only. If true all other options are
  /// pretty much ignored. 
  static const XU32 NodeOnly = 0x00000001;

  /// if true, children of nodes will get deleted whenever a transform is deleted
  static const XU32 RelatedChildren = 0x00000002;

  /// if true, when a surface is deleted, xmd will check each assigned material to
  /// see if any other surfaces are using this material. If not, then the material(s)
  /// will be deleted too.
  static const XU32 RelatedMaterials = 0x00000004;

  /// whenever a material is deleted , this will delete any unused texture placements,
  /// bump & env map nodes, and any textures no longer needed.
  static const XU32 RelatedTextures = 0x00000008;

  /// if this is true, all geometry types will delete any unused deformers in their
  /// deformer queue.
  static const XU32 RelatedDeformers = 0x00000010;

  /// if flag set, then any particle shape will also delete any unused fields and emitters
  /// connected. 
  static const XU32 RelatedDynamics = 0x00000020;

  /// if this is true, all deformers deleted will also delete any input objects such as 
  /// curves, lattice shapes etc.
  static const XU32 RelatedDeformerInputs = 0x00000040;

  /// \brief  ctor
  XCreatePolicy();

  /// \brief  dtor
  ~XCreatePolicy();

  /// \brief  copy ctor
  /// \param  dp  - the policy to copy
  XCreatePolicy(const XCreatePolicy& dp);

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
  XU32 m_Flags[XMD_MAX_CREATE_STACK];
  /// the current element on the stack
  XU32 m_Current;
};
}
