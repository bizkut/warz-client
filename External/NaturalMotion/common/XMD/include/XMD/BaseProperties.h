//----------------------------------------------------------------------------------------------------------------------
/// \file   BaseProperties.h
/// \author Rob Bateman [http://robthebloke.org]
/// \brief  Defines a Wrap deformation
/// \note   (C) Copyright 2003-2005 Robert Bateman. All rights reserved.
//----------------------------------------------------------------------------------------------------------------------
#pragma once
//----------------------------------------------------------------------------------------------------------------------
#include "XMD/Model.h"
//----------------------------------------------------------------------------------------------------------------------
namespace XMD
{
// forward declarations
class XMD_EXPORT XCompoundPropertyArray;
class XMD_EXPORT XCompoundProperty;
class XMD_EXPORT XBase;
class XMD_EXPORT XAnimCycle;
class XMD_EXPORT XAnimCurve;
struct IPropertyAccessor;

#ifndef XPROPERTY_TYPE_DEFINED__
#define XPROPERTY_TYPE_DEFINED__
//----------------------------------------------------------------------------------------------------------------------
/// \brief  defines the available property types for use with the generic
///         property access system of XBase.
//---------------------------------------------------------------------------------------------------------------------- 
class XMD_EXPORT XPropertyType 
{
public:
  enum Type
  {
    kInvalid=0,
    kBool,
    kReal,
    kInt,
    kUnsigned,
    kVector3,
    kMatrix,
    kColour,
    kString,
    kNode,
    kQuaternion,
    kRealArray,
    kVector3Array,
    kUnsignedArray,
    kNodeArray,
    kCompoundArray
  };
};
#endif

//----------------------------------------------------------------------------------------------------------------------
/// \brief  This class is used to provide a property access mechanism that
///         is shared between all instances of a specific type 
//---------------------------------------------------------------------------------------------------------------------- 
class XMD_EXPORT XBaseProperties
{
public:

  /// \brief  ctor
  XBaseProperties() {}

  /// \brief  dtor
  virtual ~XBaseProperties() {}

  virtual bool SetProperty(XBase* node, XU32 property_id, const XQuaternion& val);
  virtual bool SetProperty(XBase* node, XU32 property_id, const XVector3& val);
  virtual bool SetProperty(XBase* node, XU32 property_id, const bool& val);
  virtual bool SetProperty(XBase* node, XU32 property_id, const XReal& val);
  virtual bool SetProperty(XBase* node, XU32 property_id, const XColour& val);
  virtual bool SetProperty(XBase* node, XU32 property_id, const XString& val);
  virtual bool SetProperty(XBase* node, XU32 property_id, const XRealArray& val);
  virtual bool SetProperty(XBase* node, XU32 property_id, const XVector3Array& val);
  virtual bool SetProperty(XBase* node, XU32 property_id, const XU32Array& val);
  virtual bool SetProperty(XBase* node, XU32 property_id, const XU32& val);
  virtual bool SetProperty(XBase* node, XU32 property_id, const XS32& val);
  virtual bool SetProperty(XBase* node, XU32 property_id, const XMatrix& val);
  virtual bool SetProperty(XBase* node, XU32 property_id, const XBase*& val);
  virtual bool SetProperty(XBase* node, XU32 property_id, const XList& val);

  virtual bool GetProperty(XBase* node, XU32 property_id, XQuaternion& val);
  virtual bool GetProperty(XBase* node, XU32 property_id, XVector3& val);
  virtual bool GetProperty(XBase* node, XU32 property_id, bool& val);
  virtual bool GetProperty(XBase* node, XU32 property_id, XReal& val);
  virtual bool GetProperty(XBase* node, XU32 property_id, XColour& val);
  virtual bool GetProperty(XBase* node, XU32 property_id, XString& val);
  virtual bool GetProperty(XBase* node, XU32 property_id, XRealArray& val);
  virtual bool GetProperty(XBase* node, XU32 property_id, XU32Array& val);
  virtual bool GetProperty(XBase* node, XU32 property_id, XVector3Array& val);
  virtual bool GetProperty(XBase* node, XU32 property_id, XU32& val);
  virtual bool GetProperty(XBase* node, XU32 property_id, XS32& val);
  virtual bool GetProperty(XBase* node, XU32 property_id, XMatrix& val);
  virtual bool GetProperty(XBase* node, XU32 property_id, XBase*& val);
  virtual bool GetProperty(XBase* node, XU32 property_id, XList& val);

  virtual XString GetPropertyName(XU32);
  virtual XPropertyType::Type GetPropertyType(XU32);
  virtual XU32 GetPropertyID(const XString&);
  virtual XU32 GetNumProperties() const { return 0; }
  virtual XU32 GetPropertyFlags(XU32) { return 0; }
  virtual XFn::Type GetPropertyNodeType(XU32) { return XFn::Base; }

protected:
  /// \brief  internally initialises a compound property to the required data.
  void InitCompound(XCompoundProperty& prop,void*,IPropertyAccessor* accessor);
  static const XU32 kLast=0;
};
}
