//----------------------------------------------------------------------------------------------------------------------
/// \file   Geometry.h
/// \author Rob Bateman [http://robthebloke.org]
/// \brief  The base of all geometry things
/// \note   (C) Copyright 2003-2005 Robert Bateman. All rights reserved.
//----------------------------------------------------------------------------------------------------------------------
#pragma once
//----------------------------------------------------------------------------------------------------------------------
#include "XMD/Shape.h"
#include "XM2/Vector3Array.h"
//----------------------------------------------------------------------------------------------------------------------
namespace XMD
{
#ifndef DOXYGEN_PROCESSING
class XMD_EXPORT XDeformer;
typedef XM2::pod_vector<XDeformer*> XDeformerList;
#endif

//----------------------------------------------------------------------------------------------------------------------
/// \class XMD::XGeometry
/// \brief This class is the base class for any items that you wish to 
///        place in the scene, ie locators, lights, cameras, meshes, nurbs etc.
//----------------------------------------------------------------------------------------------------------------------
class XMD_EXPORT XGeometry
  : public XShape
{
  XMD_ABSTRACT_CHUNK(XGeometry);
public:

  //--------------------------------------------------------------------------------------------------------------------
  // property ID's
  static const XU32 kPoints               = XShape::kLast+1; // XVector3Array
  static const XU32 kDeformerQueue        = XShape::kLast+2; // XList
  static const XU32 kIsIntermediateObject = XShape::kLast+3; // bool
  static const XU32 kLast                 = kIsIntermediateObject;
  //--------------------------------------------------------------------------------------------------------------------

  //--------------------------------------------------------------------------------------------------------------------
  /// \name   Geometry Point Access
  /// \brief  allows you to set or get the geometry points.
  //@{

  /// \brief  this function sets the geometry points
  /// \param  points  -  the new set of points?
  void SetPoints(const XVector3Array &points);

  /// \brief  this function sets the geometry points
  /// \param  points  -  the array of points
  /// \param  num    -  the number of points in the array
  void SetPoints(const XReal* points, XU32 num);

  /// \brief  this function gets the geometry points
  /// \param  points  -  the returned array of points
  void GetPoints(XVector3Array& points) const;

  /// \brief  this function gets the geometry points
  /// \param  points  -  the returned array of points
  /// \param  bone  -  the bone that defines the local space in which
  ///           the points will be stored.
  /// \param  instance-  the instance to use for geometry parenting 
  void GetPoints(XVector3Array& points, const XBone* bone, const XInstance* instance) const;

  /// \brief  this function returns the number of points in this geometry surface.
  /// \return the number of points in the geometry
  XU32 GetNumPoints() const;

  /// \brief  allows you to retrieve a single point from the geometry object.
  /// \param  idx - the index of the point to retrieve.
  /// \return a reference to the requested point. 
  const XVector3& GetPoint(XU32 idx) const;

  /// \brief  this function sets a specific point in the geometry object
  /// \param  idx - the index of the point.
  /// \param  x - the x coord for the point
  /// \param  y - the y coord for the point
  /// \param  z - the z coord for the point
  void SetPoint(XU32 idx, XReal x, XReal y, XReal z);

  /// \brief  this function sets a specific point in the geometry object
  /// \param  idx - the index of the point.
  /// \param  p - the new point value
  void SetPoint(XU32 idx, const XVector3& p);

  /// \brief  returns an iterator to the first point in the point list
  /// \return the first iterator in the point array
  XVector3Array::const_iterator PointsBegin() const;

  /// \brief  returns an iterator to the end point in the point list
  /// \return the end iterator in the point array
  XVector3Array::const_iterator PointsEnd() const;

  /// \brief  returns an iterator to the first point in the list
  /// \return the first iterator in the point array
  XVector3Array::iterator PointsBegin();

  /// \brief  returns an iterator to the end point in the point list
  /// \return the end iterator in the point array
  XVector3Array::iterator PointsEnd();

  /// \brief  returns a constant reference to the internal point array
  /// \return point array reference
  const XVector3Array& Points() const;

  /// \brief  this function gets the geometry points
  /// \param  points  -  the returned array of points
  /// \param  num    -  the number of points rturned
  /// \note   make sure you delete the memory allocated after you have
  ///         finished with the point data
  void GetPoints(XReal** points, XU32& num) const;

  //@}

  //--------------------------------------------------------------------------------------------------------------------
  /// \name   Deformer Queue Access
  /// \brief  All geometry objects can be manipulated by a queue of 
  ///         deformations. Each Deformation is derived from the 
  ///         XDeformer base class. 
  //@{

  /// \brief  retrieves the list of deformers that are affecting this node in order.
  /// \param  _list  -  the returned list of deformers
  /// \return  true if any found
  bool GetDeformerQueue(XDeformerList& _list) const;

  /// \brief  Allows you to specify a series of geometry deformations 
  ///         that can affect a piece of geometry. 
  /// \param  _list  -  the new deformer queue for the geometry object.
  /// \return  true if validly set.
  bool SetDeformerQueue(const XDeformerList& _list);

  //@}

  //--------------------------------------------------------------------------------------------------------------------
  /// \name   Intermediate Object Access
  /// \brief  An intermediate object is simply one that exists within 
  ///         the scene, but is not rendered.
  //@{

  /// \brief  returns the intermediate flag on the geometry nodes
  /// \return true if object is intermediate
  bool GetIsIntermediateObject() const;

  /// \brief  sets the intermediate flag on the geometry nodes
  /// \param flag - true if object is intermediate, false otherwise
  void SetIsIntermediateObject(bool flag);

  //@}

protected:
#ifndef DOXYGEN_PROCESSING
  /// the geometry points
  XVector3Array m_Points;
  /// the list of deformers affecting this object
  XIndexList m_DeformerQueue;
  /// is this geometry part of a computation?
  bool m_IsIntermediateObject;
#endif
};
#ifndef DOXYGEN_PROCESSING
DECLARE_API_TYPE(XFn::Geometry,XGeometry,"geometry");
#endif

/// an array of geometry objects
typedef XM2::pod_vector<XGeometry*> XGeometryList;
}
