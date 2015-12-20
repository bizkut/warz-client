//----------------------------------------------------------------------------------------------------------------------
/// \file   CompoundProperties.h
/// \author Rob Bateman [http://robthebloke.org]
/// \brief  Defines a Wrap deformation
/// \note   (C) Copyright 2003-2005 Robert Bateman. All rights reserved.
//----------------------------------------------------------------------------------------------------------------------
#pragma once
//----------------------------------------------------------------------------------------------------------------------
#include "XMD/BaseProperties.h"
namespace XMD
{
class XMD_EXPORT XCompoundPropertyArray;

//----------------------------------------------------------------------------------------------------------------------
/// \class XMD::XCompoundProperty
/// \brief allows you to access compound array properties from nodes in XMD. 
//----------------------------------------------------------------------------------------------------------------------
class XMD_EXPORT XCompoundProperty
{
  friend class XBaseProperties;
  friend class XMD_EXPORT XCompoundPropertyArray;
public:

  /// \brief  dtor
  ~XCompoundProperty();

  /// \brief  ctor
  XCompoundProperty();

  /// \brief  copy ctor
  XCompoundProperty(const XCompoundProperty& rhs);

  /// \brief  Sets the specified property via it's ID.
  /// \param  property_id - the ID of the property to set
  /// \param  val - the new value for the property
  /// \return true if the property could be set, false otherwise
  bool SetProperty(XU32 property_id, const XQuaternion& val);

  /// \brief  Sets the specified property via it's ID.
  /// \param  property_id - the ID of the property to set
  /// \param  val - the new value for the property
  /// \return true if the property could be set, false otherwise
  bool SetProperty(XU32 property_id, const XVector3& val);

  /// \brief  Sets the specified property via it's ID.
  /// \param  property_id - the ID of the property to set
  /// \param  val - the new value for the property
  /// \return true if the property could be set, false otherwise
  bool SetProperty(XU32 property_id, const bool& val);

  /// \brief  Sets the specified property via it's ID.
  /// \param  property_id - the ID of the property to set
  /// \param  val - the new value for the property
  /// \return true if the property could be set, false otherwise
  bool SetProperty(XU32 property_id, const XReal& val);

  /// \brief  Sets the specified property via it's ID.
  /// \param  property_id - the ID of the property to set
  /// \param  val - the new value for the property
  /// \return true if the property could be set, false otherwise
  bool SetProperty(XU32 property_id, const XColour& val);

  /// \brief  Sets the specified property via it's ID.
  /// \param  property_id - the ID of the property to set
  /// \param  val - the new value for the property
  /// \return true if the property could be set, false otherwise
  bool SetProperty(XU32 property_id, const XString& val);

  /// \brief  Sets the specified property via it's ID.
  /// \param  property_id - the ID of the property to set
  /// \param  val - the new value for the property
  /// \return true if the property could be set, false otherwise
  bool SetProperty(XU32 property_id, const XRealArray& val);

  /// \brief  Sets the specified property via it's ID.
  /// \param  property_id - the ID of the property to set
  /// \param  val - the new value for the property
  /// \return true if the property could be set, false otherwise
  bool SetProperty(XU32 property_id, const XVector3Array& val);

  /// \brief  Sets the specified property via it's ID.
  /// \param  property_id - the ID of the property to set
  /// \param  val - the new value for the property
  /// \return true if the property could be set, false otherwise
  bool SetProperty(XU32 property_id, const XU32Array& val);

  /// \brief  Sets the specified property via it's ID.
  /// \param  property_id - the ID of the property to set
  /// \param  val - the new value for the property
  /// \return true if the property could be set, false otherwise
  bool SetProperty(XU32 property_id, const XU32& val);

  /// \brief  Sets the specified property via it's ID.
  /// \param  property_id - the ID of the property to set
  /// \param  val - the new value for the property
  /// \return true if the property could be set, false otherwise
  bool SetProperty(XU32 property_id, const XS32& val);

  /// \brief  Sets the specified property via it's ID.
  /// \param  property_id - the ID of the property to set
  /// \param  val - the new value for the property
  /// \return true if the property could be set, false otherwise
  bool SetProperty(XU32 property_id, const XMatrix& val);

  /// \brief  Sets the specified property via it's ID.
  /// \param  property_id - the ID of the property to set
  /// \param  val - the new value for the property
  /// \return true if the property could be set, false otherwise
  bool SetProperty(XU32 property_id, const XBase*& val);

  /// \brief  Sets the specified property via it's ID.
  /// \param  property_id - the ID of the property to set
  /// \param  val - the new value for the property
  /// \return true if the property could be set, false otherwise
  bool SetProperty(XU32 property_id, const XList& val);

  /// \brief  Gets the specified property via it's ID.
  /// \param  property_id - the ID of the property to retrieve
  /// \param  val - the returned value of the property
  /// \return true if the property could be retrieved, false otherwise
  bool GetProperty(XU32 property_id, XQuaternion& val);

  /// \brief  Gets the specified property via it's ID.
  /// \param  property_id - the ID of the property to retrieve
  /// \param  val - the returned value of the property
  /// \return true if the property could be retrieved, false otherwise
  bool GetProperty(XU32 property_id, XVector3& val);

  /// \brief  Gets the specified property via it's ID.
  /// \param  property_id - the ID of the property to retrieve
  /// \param  val - the returned value of the property
  /// \return true if the property could be retrieved, false otherwise
  bool GetProperty(XU32 property_id, bool& val);

  /// \brief  Gets the specified property via it's ID.
  /// \param  property_id - the ID of the property to retrieve
  /// \param  val - the returned value of the property
  /// \return true if the property could be retrieved, false otherwise
  bool GetProperty(XU32 property_id, XReal& val);

  /// \brief  Gets the specified property via it's ID.
  /// \param  property_id - the ID of the property to retrieve
  /// \param  val - the returned value of the property
  /// \return true if the property could be retrieved, false otherwise
  bool GetProperty(XU32 property_id, XColour& val);

  /// \brief  Gets the specified property via it's ID.
  /// \param  property_id - the ID of the property to retrieve
  /// \param  val - the returned value of the property
  /// \return true if the property could be retrieved, false otherwise
  bool GetProperty(XU32 property_id, XString& val);

  /// \brief  Gets the specified property via it's ID.
  /// \param  property_id - the ID of the property to retrieve
  /// \param  val - the returned value of the property
  /// \return true if the property could be retrieved, false otherwise
  bool GetProperty(XU32 property_id, XRealArray& val);

  /// \brief  Gets the specified property via it's ID.
  /// \param  property_id - the ID of the property to retrieve
  /// \param  val - the returned value of the property
  /// \return true if the property could be retrieved, false otherwise
  bool GetProperty(XU32 property_id, XU32Array& val);

  /// \brief  Gets the specified property via it's ID.
  /// \param  property_id - the ID of the property to retrieve
  /// \param  val - the returned value of the property
  /// \return true if the property could be retrieved, false otherwise
  bool GetProperty(XU32 property_id, XVector3Array& val);

  /// \brief  Gets the specified property via it's ID.
  /// \param  property_id - the ID of the property to retrieve
  /// \param  val - the returned value of the property
  /// \return true if the property could be retrieved, false otherwise
  bool GetProperty(XU32 property_id, XU32& val);

  /// \brief  Gets the specified property via it's ID.
  /// \param  property_id - the ID of the property to retrieve
  /// \param  val - the returned value of the property
  /// \return true if the property could be retrieved, false otherwise
  bool GetProperty(XU32 property_id, XS32& val);

  /// \brief  Gets the specified property via it's ID.
  /// \param  property_id - the ID of the property to retrieve
  /// \param  val - the returned value of the property
  /// \return true if the property could be retrieved, false otherwise
  bool GetProperty(XU32 property_id, XMatrix& val);

  /// \brief  Gets the specified property via it's ID.
  /// \param  property_id - the ID of the property to retrieve
  /// \param  val - the returned value of the property
  /// \return true if the property could be retrieved, false otherwise
  bool GetProperty(XU32 property_id, XBase*& val);

  /// \brief  Gets the specified property via it's ID.
  /// \param  property_id - the ID of the property to retrieve
  /// \param  val - the returned value of the property
  /// \return true if the property could be retrieved, false otherwise
  bool GetProperty(XU32 property_id, XList& val);

  /// \brief  Gets the specified property via it's ID. This is used to 
  ///         retrieve properties of type kCompoundArray
  /// \param  property_id - the ID of the property to retrieve
  /// \param  val - the returned value of the property
  /// \return true if the property could be retrieved, false otherwise
  bool GetProperty(XU32 property_id, XCompoundPropertyArray& val);

  /// \brief  Gets the name for the specified property ID
  /// \param  property_id - the ID of the property name to retrieve
  /// \return the name of the property
  XString GetPropertyName(XU32 property_id);

  /// \brief  Gets the type for the specified property ID
  /// \param  property_id - the ID of the property name to retrieve
  /// \return the type of the property 
  XPropertyType::Type GetPropertyType(XU32 property_id);

  /// \brief  Gets the id for the specified property name
  /// \param  property_name - the name of the property id to retrieve
  /// \return the id of the property
  XU32 GetPropertyID(const XString& property_name);

  /// \brief  Gets the number of properties contained within this node
  ///         or compound attribute
  /// \return the total number of properties available
  XU32 GetNumProperties() const;

  /// \brief  Gets the number of properties contained within this node
  ///         or compound attribute
  /// \return the total number of properties available
  XU32 GetPropertyFlags(XU32 property_id);

  /// \brief  Gets the number of properties contained within this node
  ///         or compound attribute
  /// \return the total number of properties available
  XFn::Type GetPropertyNodeType(XU32 property_id);

private:
  /// the data pointed, i.e. XFace, XPointIndex, XSkinWeight ....
  XCompoundPropertyArray* m_parent;
  /// the index of this element
  void* m_data;
};

/// func type used to set the data element within an array
typedef void (*XSetElementIndexFunc)(const void* array_data, XU32 index, void*& data);

//----------------------------------------------------------------------------------------------------------------------
/// \class XMD::XCompoundPropertyArray
/// \brief This class is used to encapsulate a compound array property.
/// \note  This class must always be initialised before usage otherwise 
///        all operations will fail. To initialise the class, first call
///        XBase::GetProperty( some_compound_id, XCompoundPropertyArray).
///        This will ensure that the class is referencing the correct data
///        to alow you to start modifying and addaing array elements. You
///        cannot say, resize this array and set with the data for a polyface 
///        without having first called GetProperty to initialise this clas
///        to reference the faces. 
//---------------------------------------------------------------------------------------------------------------------- 
class XMD_EXPORT XCompoundPropertyArray
{
  friend class XCompoundProperty;
  friend class XBaseProperties;
  typedef XM2::pod_vector<XCompoundProperty> XPropertyArray;
public:

  typedef XPropertyArray::iterator iterator;
  typedef XPropertyArray::const_iterator const_iterator;
  typedef XCompoundProperty value_type;
  typedef XPropertyArray::size_type size_type;

  /// \brief  returns true if this class is currently initialised and valid.
  /// \return true if the class can currently access data
  bool valid() const;

  /// \brief  returns an iterator to the start of the array
  /// \return an iterator that references the start of the array 
  iterator begin();

  /// \brief  returns a const iterator to the start of the array
  /// \return a const iterator that references the start of the array 
  const_iterator begin() const;

  /// \brief  returns an iterator to the start of the array
  /// \return an iterator that references the start of the array 
  iterator end();

  /// \brief  returns a const iterator to the end of the array
  /// \return a const iterator that references the end of the array 
  const_iterator end() const;

  /// \brief  returns the number of array elements present.
  /// \return the number of elements in the array
  size_type size() const;

  /// \brief  sets the size of the array. This may fail if there 
  ///         is the compound array that this class is referencing is
  ///         not allowed to be re-sized. 
  /// \param  size_ - the new number of elements in the array
  /// \return true if it could be properly re-sized.
  bool resize(size_type size_);

  /// \brief  array [] operator
  /// \param  index - the index in the array to access.
  /// \return a const reference to that element
  const XCompoundProperty& operator [] (XU32 index) const;

  /// \brief  array [] operator
  /// \param  index - the index in the array to access.
  /// \return a reference to that element
  XCompoundProperty& operator [] (XU32 index);

private:
  /// the array elements of all the 
  XPropertyArray mData;
  /// data pointer for the full array
  void* m_data;
  /// a function to set the data pointer for the array elements
  XSetElementIndexFunc m_setIndexFunc;
  /// pointer to the owning node
  XBase* m_node;
  /// an accessor to retrieve each 
  IPropertyAccessor* m_interface;
};
}
