//----------------------------------------------------------------------------------------------------------------------
/// \file   ObjectSet.h
/// \author Rob Bateman [http://robthebloke.org]
/// \brief  Defines the interface for an object set
/// \note   (C) Copyright 2003-2005 Robert Bateman. All rights reserved.
//----------------------------------------------------------------------------------------------------------------------
#pragma once
//----------------------------------------------------------------------------------------------------------------------
#include "XMD/NodeCollection.h"
//----------------------------------------------------------------------------------------------------------------------
namespace XMD 
{
//----------------------------------------------------------------------------------------------------------------------
/// \class XMD::XObjectSet
/// \brief Holds information about an object set within the scene.
//----------------------------------------------------------------------------------------------------------------------
class XMD_EXPORT XObjectSet
  : public XNodeCollection
{
  XMD_CHUNK(XObjectSet);
public:

  //--------------------------------------------------------------------------------------------------------------------
  // property ID's
  static const XU32 kAnnotation = XNodeCollection::kLast+1; // XString
  static const XU32 kLast       = kAnnotation;
  //--------------------------------------------------------------------------------------------------------------------

  /// \name   Annotation Information
  /// \brief  Sets/Gets the annotation that describes what is contained
  ///         within this set.
  //@{

  /// \brief  returns the annotation string attached to this object set
  /// \return the annotation string
  const XChar* GetAnnotation() const;

  /// \brief  sets the annotation string attached to this object set
  /// \return annotation - the annotation string
  void SetAnnotation(const XChar* annotation);

  //@}

private:
#ifndef DOXYGEN_PROCESSING
  XString m_Annotation;
  XIndexList m_Objects;
#endif
};
#ifndef DOXYGEN_PROCESSING
DECLARE_CHUNK_TYPE(XFn::ObjectSet, XObjectSet, "objectSet", "OBJECT_SET");
#endif
}
