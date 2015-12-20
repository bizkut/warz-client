//----------------------------------------------------------------------------------------------------------------------
/// \file   BlendShape.h
/// \author Rob Bateman [http://robthebloke.org]
/// \brief  Defines a blend shape deformer
/// \note   (C) Copyright 2003-2005 Robert Bateman. All rights reserved.
//----------------------------------------------------------------------------------------------------------------------
#pragma once
//----------------------------------------------------------------------------------------------------------------------
#include "XMD/Deformer.h"
//----------------------------------------------------------------------------------------------------------------------
namespace XMD 
{
//----------------------------------------------------------------------------------------------------------------------
/// \class XMD::XBlendShape
/// \brief This class is used to hold a blend shape and all of it's data
///        for the target shapes applied to a surface. 
//----------------------------------------------------------------------------------------------------------------------
class XMD_EXPORT XBlendShape :
  public XDeformer
{
  XMD_CHUNK(XBlendShape);
// node data get/set functions
public:

  //--------------------------------------------------------------------------------------------------------------------
  // property ID's
  static const XU32 kWeights   = XDeformer::kLast+1; // XRealArray
  static const XU32 kTargets   = XDeformer::kLast+2; // XList
  static const XU32 kBaseShape = XDeformer::kLast+3; // XBase*
  static const XU32 kLast      = kBaseShape;
  //--------------------------------------------------------------------------------------------------------------------

  //--------------------------------------------------------------------------------------------------------------------
  /// \name   Original Undeformed Geometry
  /// \brief  Allows you to set or get the original un-deformed 
  ///         geometry object. This may or may not exist depending
  ///         on whether that data was present at the time of export.
  ///         You may need to use the GetAffected method to query this
  ///         data if GetBase() returns NULL.
  //@{
  
  /// \brief  returns a pointer to the base geometry object upon which 
  ///         the deformation is applied. 
  /// \return pointer to the base geometry object
  XGeometry* GetBase() const;

  /// \brief  sets a reference to the original undeformed geometry object.
  /// \param  original_geom - the original geometry object.
  bool SetBase(const XBase* original_geom);

  //@}

  //--------------------------------------------------------------------------------------------------------------------
  /// \name   Blend Shape Target Data
  /// \brief  Allows you to set or get information about the blend 
  ///         shape targets.
  //@{

  /// \brief  adds a new blend shape target to the object
  /// \param  points  -  a list of points defining the target
  /// \param  name  -  the name of the blend shape target
  /// \return true if OK
  bool AddTarget(const XVector3Array& points, const XString& name);

  /// \brief  adds a new blend shape target to the object. This version
  ///         will cache the points contained on the geometry object
  ///         provided. If the geometry object is later deleted, the 
  ///         point data will still be stored on the 
  /// \param  geometry  -  the geometry object to use as a target.
  /// \param  name  -  the name of the blend shape target
  /// \return true if OK
  bool AddTarget(const XGeometry* geometry, const XString& name);

  /// \brief  deletes the specified target from the object
  /// \param  target  -  the ID of the blend shape target to delete
  /// \return true if OK
  bool DeleteTarget(XU32 target);

  /// \brief  returns the positions of the points used in the specified
  ///         target shape. 
  /// \param  target  -  the index of the blendshape target
  /// \param  points  -  an array of points that will be filled with 
  ///            the target shape data
  /// \return  true if OK
  bool GetTarget(XU32 target, XVector3Array& points) const;

  /// \brief  If the original blend shaped geometry exists within the
  ///         model, then this function will return a handle to it. This
  ///         can be useful for importing files into app's since you can
  ///         effectively re-create the blend shape deformer network. 
  ///         If the target was deleted after the blend shape was created
  ///         then this function will return NULL. In this case you can 
  ///         access the target points using GetTarget() to retrieve the
  ///         point list for that shape
  /// \param  target  -  the index of the target to return
  /// \return a pointer to the target if it exists.
  XGeometry* GetTarget(XU32 target) const;

  /// \brief  sets the positions of the points used in the specified
  ///         target shape. 
  /// \param  target  -  the index of the blendshape target
  /// \param  points  -  the new array of points that will fill the
  ///         target shape data
  /// \return true if OK
  bool SetTarget(XU32 target, const XVector3Array& points);

  /// \brief  returns the target name 
  /// \param  target  -  the index of the blend shape target to query
  /// \param  name  -  will hold the returned name
  /// \return true if OK
  bool GetTargetName(XU32 target, XString& name) const;

  /// \brief  returns the number of targets contained on the blend shape
  ///         deformer.
  /// \return the number of blend shape targets
  XU32 GetNumTargets() const;

  /// \brief  returns the weight value for the specified target
  /// \param  target_num - index of the target to get the weight for
  /// \return the weight value
  XReal GetWeight(XU32 target_num) const;

  /// \brief  returns the weight value for the specified target
  /// \param  target_num - index of the target to get the weight for
  /// \param  weight - the weight value
  void SetWeight(XU32 target_num, XReal weight);

  //@}

// internal node data
private:
#ifndef DOXYGEN_PROCESSING

  /// \brief internal struct to hold the target data
  struct XBlendTarget
  {
    /// node id of the target geometry (or 0)
    XId m_GeometryID;
    /// the target points
    XVector3Array m_Offsets;
    /// the target name
    XString m_TargetName;
    /// the target weight
    XReal m_Weight;
  };
  typedef XM2::pod_vector<XBlendTarget*> XBlendTargetArray;
  /// the blend shape target data.
  XBlendTargetArray m_BlendTargets;
#endif
};
#ifndef DOXYGEN_PROCESSING
DECLARE_CHUNK_TYPE(XFn::BlendShape, XBlendShape, "blendShape", "BLEND_SHAPE");
#endif
}
