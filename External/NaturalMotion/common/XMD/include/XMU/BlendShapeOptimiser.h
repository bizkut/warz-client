//----------------------------------------------------------------------------------------------------------------------
/// \file   BlendShapeOptimiser.h
/// \author Rob Bateman [http://robthebloke.org]
/// \brief  This file defines routines to take an input geometry object, and some blend shape targets. It then proceeds 
///         to reduce these down to a set of targets defined by offsets of only those vertices that actually change.
/// \note    (C) Copyright 2003-2005 Robert Bateman. All rights reserved.
//----------------------------------------------------------------------------------------------------------------------
#pragma once
//----------------------------------------------------------------------------------------------------------------------
#include "XMD/XFn.h"
#include "XM2/Vector3Array.h"
#include "XM2/RealArray.h"
//----------------------------------------------------------------------------------------------------------------------
namespace XMU  
{
//----------------------------------------------------------------------------------------------------------------------
/// \class XMU::XMUOffset
/// \brief A blend shape vertex offset. This is a vector offset from the original un-deformed point. 
//----------------------------------------------------------------------------------------------------------------------
struct XMD_EXPORT XMUOffset
{
  /// \brief  ctor
  XMUOffset();

  /// \brief  ctor
  /// \param  i - the vertex index
  /// \param  x_ - the x component of the offset vector
  /// \param  y_ - the y component of the offset vector
  /// \param  z_ - the z component of the offset vector
  XMUOffset(XMD::XU32 i, XM2::XReal x_, XM2::XReal y_, XM2::XReal z_);

  /// \brief  copy ctor
  XMUOffset(const XMUOffset& o);

  /// the index of the vertex the offset vector applies to
  XM2::XU32 index;

  /// the x component of the offset vector
  XM2::XReal x;

  /// the y component of the offset vector
  XM2::XReal y;

  /// the z component of the offset vector
  XM2::XReal z;
};

//----------------------------------------------------------------------------------------------------------------------
/// \class XMU::XMUOffsetList
/// \brief A blend shape target consisting of an array of offsets for vertices that are modified by this 
///        blend shape target. 
//----------------------------------------------------------------------------------------------------------------------
struct XMD_EXPORT XMUOffsetList
  : public XM2::pod_vector<XMUOffset>
{
  /// \brief  ctor
  XMUOffsetList();

  /// \brief  copy ctor
  XMUOffsetList(const XMUOffsetList& ol);

  /// \brief  used to evaluate the influence of this target on the deformed mesh. 
  /// \param  points  -  the point array to modify
  /// \param  w - the weight to apply to this target
  void SumInfluence(XM2::XVector3Array& points, XM2::XReal w) const;
};

//----------------------------------------------------------------------------------------------------------------------
/// \class XMU::XMUBlendShapeOptimiser
/// \brief A class to optimise blend shapes for you from a set of geometry objects, into a set of indexed offset 
///        vectors for each blend shape target. This can radically reduce the amount of data required to store a 
///        blend shape, and makes it much easier to sum the weighted influences of each target. 
//----------------------------------------------------------------------------------------------------------------------
struct XMD_EXPORT XMUBlendShapeOptimiser
  : protected XM2::vector<XMUOffsetList>
{
  /// \brief  ctor
  XMUBlendShapeOptimiser();

  /// \brief  This sets the original vertex data so the class can compare against the incoming target shapes.
  /// \param  points - the vertex data array. Assumes x,y,z order
  void SetPoints(const XM2::XVector3Array &points);

  /// \brief  This sets the original vertex data so the class can compare against the incoming target shapes.
  /// \param  num -  the number of points
  /// \param  points -  the vertex data array. Assumes x,y,z order
  void SetPoints(XM2::XU32 num, XM2::XReal* points);

  /// \brief  Adds a blend shape target to the Blend shape optimiser. This assumes that the blend target has 
  ///         the same number of vertices as the original mesh
  /// \param  points - the raw point data
  /// \param  stride - the stride over each vertex
  /// \param  offset - the offset from the memory location where the target data is stored. 
  void NewTarget(const XM2::XReal* points, XM2::XU32 stride = sizeof(XM2::XReal) * 3, XM2::XU32 offset = 0);

  /// \brief  Adds a blend shape target to the Blend shape optimiser. This assumes that the blend target has the 
  ///         same number of vertices as the original mesh
  /// \param  points - the blend shape target points
  void AddTarget(const XM2::XVector3Array& points);

  /// \brief  returns the vertex offsets for the requested target
  /// \param  Target  - the target index
  /// \return a list of vertex offsets
  const XMUOffsetList& GetBlendTarget(XM2::XU32 Target) const;

  /// \brief  returns the total number of targets
  /// \return the number of target shapes for the blend shape
  XM2::XU32 GetNumTargets() const;

  /// \brief  Gets the target weights
  /// \param  weights - the returned array of weights
  void GetWeights(XM2::XRealArray& weights);

  /// \brief  Sets the target weights 
  /// \param  weights - the new set of target weights
  /// \return true if OK
  bool SetWeights(const XM2::XRealArray& weights);

  /// \brief  evaluates the blend shape given the currently set weight values
  /// \param  defp - the returned array of deformed points
  void GetDeformedPoints(XM2::XVector3Array &defp);

  /// \brief  evaluates the blend shape given the currently set weight values
  /// \param  defp - the returned array of deformed points
  void GetDeformedPoints(XM2::XRealArray &defp);

  /// \brief  evaluates the blend shape given the currently set weight values
  /// \param  defp - a block of memory to fill with the deformed point positions
  void GetDeformedPoints(XM2::XReal *defp);

  /// \brief  returns a reference to the internal weights array
  /// \return the weights array 
  const XM2::XRealArray& Weights() const;

  /// \brief  returns a reference to the internal array of deformed vertices
  /// \return the vertex array 
  const XM2::XVector3Array& DeformedVertices() const;

  /// \brief  returns a reference to the internal array of un-deformed vertices
  /// \return the un-deformed vertex array 
  const XM2::XVector3Array& OriginalVertices() const;

protected:

  /// \brief  Internal function used to sum the evaluated target shapes.
  void EvalDeformedPoints();

  /// an array of the deformer weights
  XM2::XRealArray m_aWeights;

  /// an array of the deformed vertices 
  XM2::XVector3Array m_aDeformedVertices;

  /// an array of the original vertices 
  XM2::XVector3Array m_aOrigVertices;
};
}
