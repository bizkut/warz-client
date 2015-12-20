//----------------------------------------------------------------------------------------------------------------------
/// \file   Shape.h
/// \author Rob Bateman [http://robthebloke.org]
/// \brief  The base of all objects that can be parented. 
/// \note   (C) Copyright 2003-2005 Robert Bateman. All rights reserved.
//----------------------------------------------------------------------------------------------------------------------
#pragma once
//----------------------------------------------------------------------------------------------------------------------
#include "XMD/Base.h"
//----------------------------------------------------------------------------------------------------------------------
namespace XMD
{
#ifndef DOXYGEN_PROCESSING
class XMD_EXPORT XInstance;
typedef XM2::pod_vector<XInstance*> XInstanceList;
#endif

//----------------------------------------------------------------------------------------------------------------------
/// \class XMD::XShape
/// \brief This class is the base class for any items that you wish to 
///        place in the scene, ie locators, lights, cameras, meshes, nurbs etc.
//----------------------------------------------------------------------------------------------------------------------
class XMD_EXPORT XShape
  : public XBase
{
  XMD_ABSTRACT_CHUNK(XShape);
public:

  //--------------------------------------------------------------------------------------------------------------------
  // property ID's
  static const XU32 kLast       = XBase::kLast;
  //--------------------------------------------------------------------------------------------------------------------

  /// \name   Instancing Information
  /// \brief  Gets the instances of this shape. To Create an instance, 
  ///         see XModel::CreateGeometryInstance
  //@{

  /// \brief  returns a list of all object instances
  /// \param  _list  -  the returned list of instances
  /// \return  true if any found
  ///
  bool GetObjectInstances(XInstanceList& _list) const;

  //@}

protected:
#ifndef DOXYGEN_PROCESSING
  void PostLoad();
  XIndexList m_Instances;
#endif
};
#ifndef DOXYGEN_PROCESSING
DECLARE_API_TYPE(XFn::Shape, XShape, "shape");
#endif

/// \brief  An array of shapes
typedef XM2::pod_vector<XShape*> XShapeList;
}
//----------------------------------------------------------------------------------------------------------------------
#include "XMD/Instance.h"
