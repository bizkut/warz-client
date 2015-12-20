//----------------------------------------------------------------------------------------------------------------------
/// \file   CollisionMesh.h
/// \author Rob Bateman [http://robthebloke.org]
/// \brief  defines a collision mesh primitive 
/// \note   (C) Copyright 2003-2005 Robert Bateman. All rights reserved.
//----------------------------------------------------------------------------------------------------------------------
#pragma once
//----------------------------------------------------------------------------------------------------------------------
#include "XMD/CollisionBase.h"
#include "XM2/Vector3.h"
//----------------------------------------------------------------------------------------------------------------------
namespace XMD 
{
//----------------------------------------------------------------------------------------------------------------------
/// \class XMD::XCollisionMesh
/// \brief This class holds the data for a collision mesh
//---------------------------------------------------------------------------------------------------------------------- 
class XMD_EXPORT XCollisionMesh :
  public XCollisionObject
{
  XMD_CHUNK(XCollisionMesh);
public:

  //--------------------------------------------------------------------------------------------------------------------
  // property ID's
  static const XU32 kPoints  = XCollisionObject::kLast+1; // XVector3Array
  static const XU32 kIndices = XCollisionObject::kLast+2; // XU32Array
  static const XU32 kLast    = kIndices;
  //--------------------------------------------------------------------------------------------------------------------

  /// \name   Collision Mesh Params
  /// \brief  The collision mesh is defined as a set of vertices, and
  ///         a list of zero-based indices that define a list of
  ///         triangles. (anti-clockwise ordering).
  //@{

  /// \brief  Returns the vertex indices as a single vector
  /// \param  point_inds  -  the returned point indices
  void GetIndices(XIndexList& point_inds) const;

  /// \brief  Sets the vertex indices as a single vector
  /// \param  point_inds  -  the new point indices
  ///
  void SetIndices(const XIndexList& point_inds);

  /// \brief  Returns the vertex data as a single vector of points
  /// \param  points  -  the returned points
  void GetVertices(XVector3Array& points) const;

  /// \brief  Sets the vertex indices as a single vector of points
  /// \param  points  -  the new point indices
  void SetVertices(const XVector3Array& points);

  /// \brief  returns a reference to the internal triangle indices of 
  ///         the collision mesh data.
  /// \return the triangle indices for the poly mesh.
  const XIndexList& Indices() const;

  /// \brief  returns a reference to the internal triangle indices of 
  ///         the collision mesh data.
  /// \return the triangle indices for the poly mesh.
  const XVector3Array& Vertices() const;

  //@}

// internal node data
private:
#ifndef DOXYGEN_PROCESSING

  /// the triangle indices
  XIndexList m_Indices;

  /// the mesh vertices
  XVector3Array m_Vertices;

#endif
};
#ifndef DOXYGEN_PROCESSING
DECLARE_CHUNK_TYPE(XFn::CollisionMesh, XCollisionMesh, "collisionMesh", "COLLISION_MESH");
#endif
}
