//--------------------------------------------------------------------------
/// \file   Deformer.h
/// \author Rob Bateman [http://robthebloke.org]
/// \brief  The base of all geometry things
/// \note   (C) Copyright 2003-2005 Robert Bateman. All rights reserved.
//----------------------------------------------------------------------------------------------------------------------
#pragma once
//----------------------------------------------------------------------------------------------------------------------
#include "XMD/Geometry.h"
//----------------------------------------------------------------------------------------------------------------------
namespace XMD
{
//----------------------------------------------------------------------------------------------------------------------
/// \brief  This class forms the base class of all surface deformation types. 
///         The surface deformers only have to manipulate an array of point 
///         data. The geometry node itself should know how o generate any 
///         required surface normals.
///         Each geometry object can have a queue of deformations that are
///         applied in order to the geometry. (ie, blend shape->skin->lattice)
//----------------------------------------------------------------------------------------------------------------------
class XMD_EXPORT XDeformer
  : public XBase
{
  XMD_ABSTRACT_CHUNK(XDeformer);
public:

  //--------------------------------------------------------------------------------------------------------------------
  // property ID's
  static const XU32 kAffected       = XBase::kLast+1; // XBase*
  static const XU32 kEnvelopeWeight = XBase::kLast+2;
  static const XU32 kLast           = kEnvelopeWeight;
  //--------------------------------------------------------------------------------------------------------------------

  /// \name   Common Deformer Functions
  /// \brief  Allows you to set or get which geometry objects are 
  ///         affected by the deformer class.
  //@{

  /// \brief  returns a list of the object(s) affected by this deformer
  /// \return the index list
  const XIndexList& GetAffected() const;

  /// \brief  returns the number of objects affected by this
  /// \return the number of affected deformers
  XU32 GetNumAffected() const;

  /// \brief  returns a the requested object affected by this deformer
  /// \param  id - the index of the affected object
  /// \return the object
  XBase* GetAffected(XU32 id);

  /// \brief  This function adds the specified object to the influence
  ///         of this deformer.
  /// \param  object - The surface to deform
  /// \return true if OK
  bool AddAffected(const XBase* object);

  /// \brief  removes the affected geometry at the specified index
  /// \param  index - the index to remove. 
  /// \return true if OK
  bool RemoveAffected(XU32 index);

  /// \brief  removes the affected geometry at the specified index
  /// \param  object - the affected geometry to remove. 
  /// \return true if OK
  bool RemoveAffected(const XBase* object);

  /// \brief  returns the envelope weight for the deformation. This 
  ///         informs you how much the deformation is applied from
  ///         full (1.0) to none (0.0)
  /// \return the envelope weight of the deformer
  XReal GetEnvelopeWeight() const;

  /// \brief  sets the envelope weight that controls the amount of 
  ///         deformation that this deformer node applies. zero means
  ///         no deformation, one means full deformation
  /// \param  envelope - the envelope weight
  void SetEnvelopeWeight(XReal envelope);

  //@}

  /// \todo SetAffected method

protected:
#ifndef DOXYGEN_PROCESSING
  /// a float value from zero to one that controls the amount of deformation
  /// applied to a surface. 
  XReal m_EnvelopeWeight;
  /// a list of the affected geometry objects
  XIndexList m_AffectedGeometry;
#endif
};
#ifndef DOXYGEN_PROCESSING
DECLARE_API_TYPE(XFn::Deformer, XDeformer, "deformer");
#endif
/// an array of deformers
typedef XM2::pod_vector<XDeformer*> XDeformerList;
}
