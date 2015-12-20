//----------------------------------------------------------------------------------------------------------------------
/// \file   ClonePolicy.h
/// \author Rob Bateman [http://robthebloke.org]
/// \brief  This file specifies the policies available when duplicating an
///         XBase derived class. 
/// \note   (C) Copyright 2003-2005 Robert Bateman. All rights reserved.
//----------------------------------------------------------------------------------------------------------------------
#pragma once
//----------------------------------------------------------------------------------------------------------------------
#include "XMD/XFn.h"
//----------------------------------------------------------------------------------------------------------------------
namespace XMD 
{
//----------------------------------------------------------------------------------------------------------------------
/// maximum allowable create policies that can be placed on the XClonePolicy
/// stack.
#define XMD_MAX_CLONE_STACK 32

//----------------------------------------------------------------------------------------------------------------------
/// \brief  The create policy allows you to determine the actions that occur
///         when you explicitly create a node. 
//----------------------------------------------------------------------------------------------------------------------
class XMD_EXPORT XClonePolicy
{
public:

  /// \brief  when duplicating a geometry object, if this flag is set, the geometry 
  /// will create duplicates of all it's deformers. If itis not set, it 
  /// will simply re-use the same deformer objects
  static const XU32 DuplicateGeometryDeformers = 0x00000001;

  /// \brief  if this flag is set, duplicating an XBone derived class will then 
  /// duplicate all of it's chilld bones. Ifnot set, the node will be 
  /// generated on it's own. 
  static const XU32 DuplicateAllChildren = 0x00000002;

  /// \brief  if this flag is set, then when you duplicate a bone, any shape 
  /// instances under it will also be duplicated. If this is not
  /// set then the bone will be created with no child shapes.
  static const XU32 DuplicateAllInstances = 0x00000004;

  /// \brief  If DuplicateAllInstances is set to true, and this flag is set,
  /// then duplicating a bone with child shapes will duplicate all 
  /// child shapes (rather than just creating a new instance of them).
  /// Basically this is equivalent to the difference between copy
  /// and instance in a package such as Maya. 
  static const XU32 DuplicateInstanceShapes = 0x00000008;

  /// \brief  if this flag is set, then a duplicate XBone derived class will be 
  /// generated under the same parent as the original. If this flag 
  /// is turned off, then the object will be created without a parent (ie, 
  /// parented under the world). 
  static const XU32 LeaveCurrentParent = 0x00000010;

  /// \brief  if this flag is set then if we happen to duplicate a particle
  /// emitter, the shape into which it emit's will be duplicated as well.
  static const XU32 DuplicateParticleShapes = 0x00000020;

  /// \brief  if this flag is set, then when you create a new particle shape
  /// it's dynamics fields will also be set. If this flag is not set, 
  /// then the shape will be assigned to the same dynamics fields.
  static const XU32 DuplicateDynamicsFields = 0x00000040;

  /// \brief  ctor
  XClonePolicy();

  /// \brief  dtor
  ~XClonePolicy();

  /// \brief  copy ctor
  /// \param  dp  - the policy to copy
  XClonePolicy(const XClonePolicy& dp);

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
  XU32 m_Flags[XMD_MAX_CLONE_STACK];
  /// the current element on the stack
  XU32 m_Current;
};
}
