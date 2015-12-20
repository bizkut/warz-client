//----------------------------------------------------------------------------------------------------------------------
/// \file   DeletePolicy.h
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
/// \brief maximum allowable delete policies that can be placed on the XDeletePolicy
///       stack.
#define XMD_MAX_DELETE_STACK 32

//----------------------------------------------------------------------------------------------------------------------
/// \class XMD::XDeletePolicy
/// \brief This class determines what happens when a node gets deleted
///        from an XModel. For example, if you delete a mesh, you might
///        want all of it's deformers to be deleted as well, or you
///        might want it transform or assigned materials to be deleted.
///        This class allows you to control that behaviour.
//----------------------------------------------------------------------------------------------------------------------
class XMD_EXPORT XDeletePolicy
{
public:

  /// \brief  if true, children of nodes will get deleted whenever a transform is deleted
  static const XU32 RelatedChildren = 0x00000002;

  /// \brief  if true, when a surface is deleted, xmd will check each assigned material to
  /// see if any other surfaces are using this material. If not, then the material(s)
  /// will be deleted too.
  static const XU32 RelatedMaterials = 0x00000004;

  /// \brief  whenever a material is deleted , this will delete any unused texture placements,
  /// bump & env map nodes, and any textures no longer needed.
  static const XU32 RelatedTextures = 0x00000008;

  /// \brief  if this is true, all geometry types will delete any unused deformers in their
  /// deformer queue.
  static const XU32 RelatedDeformers = 0x00000010;

  /// \brief  if flag set, then any particle shape will also delete any unused fields and emitters
  /// connected. 
  static const XU32 RelatedDynamics = 0x00000020;

  /// \brief  if this is true, all deformers deleted will also delete any input objects such as 
  /// curves, lattice shapes etc.
  static const XU32 RelatedDeformerInputs = 0x00000040;

  /// \brief  should we delete node only. If true all other options are
  /// pretty much ignored. 
  static const XU32 RelatedTransforms = 0x00000080;

  /// \brief  If true, all incomming connections to attributes will be deleted. When False, no connection
  /// will get deleted (on the connected object) so you can hold onto pointers to those connections
  /// to reconnect them to another node if you wish. This may prove useful in some circumstances, 
  /// but it's not suggested to leave this on by default. It's also worth noting that you will have
  /// to manually delete all zerod connections on ALL connected objects.
  static const XU32 Inputs = 0x00000100;

  /// \brief  If true, all outgoing connections from attributes will be deleted. When False, no connection
  /// will get deleted (on the connected object) so you can hold onto pointers to those connections
  /// to reconnect them to another node if you wish. This may prove useful in some circumstances, 
  /// but it's not suggested to leave this on by default. It's also worth noting that you will have
  /// to manually delete all zerod connections on ALL connected objects.
  static const XU32 Outputs = 0x00000200;

  /// \brief  if set, the parent transform of any shape will be deleted along with it's child node. Setting
  /// this prevents buildup of unused transforms when shapes are explicitly being deleted. 
  static const XU32 ShapesDeleteTransform = 0x00000400;

  /// \brief  ctor
  XDeletePolicy();

  /// \brief  dtor
  ~XDeletePolicy();

  /// \brief  copy ctor
  /// \param  dp  - the policy to copy
  XDeletePolicy(const XDeletePolicy& dp);

  /// \brief  allows you to enable an option (or set of flag options 
  ///         in a bitfield)
  /// \param  flag  - the flags to enable
  void Enable(XU32 flag);

  /// \brief  allows you to disable an option (or set of flag options 
  ///         in a bitfield)
  /// \param   - the flags to disable
  void Disable(const XU32 flag);

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
  XU32 m_Flags[XMD_MAX_DELETE_STACK];
  /// the current element on the stack
  XU32 m_Current;
};
}
