//----------------------------------------------------------------------------------------------------------------------
/// \file   Attribute.h
/// \author Rob Bateman [http://robthebloke.org]
/// \brief  This file defines the mechanism for storing additional attributes
///         exported from Maya.
/// \note   (C) Copyright 2003-2005 Robert Bateman. All rights reserved.
//----------------------------------------------------------------------------------------------------------------------
#pragma once
//----------------------------------------------------------------------------------------------------------------------
#include "XMD/XFn.h"
#include "XM2/Vector2Array.h"
#include "XM2/Vector3Array.h"
#include "XM2/Vector4Array.h"
#include "XM2/ColourArray.h"
#include "XM2/Matrix34Array.h"
//----------------------------------------------------------------------------------------------------------------------
namespace XMD
{
#ifndef DOXYGEN_PROCESSING
class XMD_EXPORT XFileIO;
#endif

//----------------------------------------------------------------------------------------------------------------------
/// \class XMD::XInt2
/// \brief a small utility class used to hold a 2D integer attribute.
//----------------------------------------------------------------------------------------------------------------------
class XMD_EXPORT XInt2
{
  #ifndef DOXYGEN_PROCESSING
  friend std::ostream& operator<<(std::ostream& ofs, const XInt2& out);
  friend std::istream& operator>>(std::istream& ifs, XInt2& dat);
  #endif
public:

  /// \brief  ctor
  XInt2();

  /// \brief  ctor
  XInt2(XS32 a, XS32 b);

  /// \brief  dtor
  ~XInt2();

  /// \brief  copy ctor
  XInt2(const XInt2& i2);

  /// \brief  assignment operator
  const XInt2& operator = (const XInt2& i2);

  /// \brief  equality operator
  bool operator == (const XInt2& i2) const;

  /// \brief  in-equality operator
  bool operator != (const XInt2& i2) const;

  /// \brief  sets the values on the attribute
  /// \param  x_  - the new x value
  /// \param  y_  - the new y value
  void Set(XS32 x_, XS32 y_);

  union 
  {
    struct 
    {
      XS32 u;
      XS32 v;
    };
    struct 
    {
      XS32 x;
      XS32 y;
    };
    XS32 data[2];
  };
};

//----------------------------------------------------------------------------------------------------------------------
/// \class XMD::XInt3
/// \brief a small utility class used to hold a 3D integer attribute.
//----------------------------------------------------------------------------------------------------------------------
class XMD_EXPORT XInt3
{
public:

  #ifndef DOXYGEN_PROCESSING
  friend std::ostream& operator<<(std::ostream& ofs, const XInt3& out);
  friend std::istream& operator>>(std::istream& ifs, XInt3& dat);
  #endif

  /// \brief  ctor
  XInt3();

  /// \brief  ctor
  XInt3(XS32 a, XS32 b, XS32 c);

  /// \brief  dtor
  ~XInt3();

  /// \brief  copy ctor
  /// \param  i3 - the object to copy
  XInt3(const XInt3& i3);

  /// \brief  assignment operator
  /// \param  i3 - the object to copy
  /// \return  a reference to this
  const XInt3& operator = (const XInt3& i3);

  /// \brief  equality operator
  bool operator == (const XInt3& i2) const;

  /// \brief  in-equality operator
  bool operator != (const XInt3& i2) const;

  /// \brief  sets the values on the attribute
  /// \param  x_  - the new x value
  /// \param  y_  - the new y value
  /// \param  z_  - the new z value
  void Set(XS32 x_, XS32 y_, XS32 z_);
  union 
  {
    struct
    {
      XS32 x;
      XS32 y;
      XS32 z;
    };
    XS32 data[3];
  };
};

//----------------------------------------------------------------------------------------------------------------------
/// \class XMD::XInt4
/// \brief a small utility class used to hold a 4D integer attribute.
//----------------------------------------------------------------------------------------------------------------------
class XMD_EXPORT XInt4
{
public:

  #ifndef DOXYGEN_PROCESSING
  friend std::ostream& operator<<(std::ostream& ofs, const XInt4& out);
  friend std::istream& operator>>(std::istream& ifs, XInt4& dat);
  #endif

  /// \brief  ctor
  XInt4();

  /// \brief  ctor
  XInt4(XS32 a, XS32 b, XS32 c, XS32 d);

  /// \brief  dtor
  ~XInt4();

  /// \brief  copy ctor
  /// \param  i4 - the object to copy
  XInt4(const XInt4& i4);

  /// \brief  assignment operator
  /// \param  i4 - the object to copy
  /// \return  a reference to this
  const XInt4& operator = (const XInt4& i4);

  /// \brief  equality operator
  bool operator == (const XInt4& i2) const;

  /// \brief  in-equality operator
  bool operator != (const XInt4& i2) const;

  /// \brief  sets the values on the attribute
  /// \param  x_  - the new x value
  /// \param  y_  - the new y value
  /// \param  z_  - the new z value
  /// \param  w_  - the new w value
  void Set(XS32 x_, XS32 y_, XS32 z_, XS32 w_);
  union 
  {
    struct 
    {
      XS32 x;
      XS32 y;
      XS32 z;
      XS32 w;
    };
    XS32 data[4];
  };
};

/// an array of 2D integer values
typedef XMD_EXPORT XM2::pod_vector<XInt2> XInt2Array;
/// an array of 3D integer values
typedef XMD_EXPORT XM2::pod_vector<XInt3> XInt3Array;
/// an array of 4D integer values
typedef XMD_EXPORT XM2::pod_vector<XInt4> XInt4Array;

#ifndef DOXYGEN_PROCESSING
// forward declaration
class XMD_EXPORT XExtraAttributes;
class XMD_EXPORT XBase;
class XMD_EXPORT XModel;
#endif

//----------------------------------------------------------------------------------------------------------------------
/// \class XMD::XAttribute
/// \brief a class to hold a generic attribute type. This class is used to
///        either extend an already existing node type, or to help define
///        a new type without breaking the XMD file format.
//----------------------------------------------------------------------------------------------------------------------
class XMD_EXPORT XAttribute
{
#ifndef DOXYGEN_PROCESSING
  friend class XMD_EXPORT XBase;
  friend class XMD_EXPORT XExtraAttributes;
  friend std::istream &operator>>(std::istream& ifs, XExtraAttributes& ea);
  friend std::ostream &operator<<(std::ostream& ofs, const XExtraAttributes& ea);
protected:

  /// dtor
  ~XAttribute();

  /// ctor
  XAttribute();

  /// copy ctor
  XAttribute(const XAttribute& attr);

  /// \brief  sets the name of the attribute
  /// \param  n - the name of the attribute
  /// \note   This can only be called from XBase
  void SetName(const XChar* n);

#endif
public:

  //--------------------------------------------------------------------------------------------------------------------
  /// \name   Get Methods
  /// \brief  retrieve the values contained within the attribute
  //@{

  /// \brief  gets the attribute as a bool. If an array index is specified
  ///         it will return the i'th array item as a bool.
  /// \param  value - the returned value
  /// \param  array_idx - if the attribute is an array type, then this will
  ///                     be the index of the array element to extract. If
  ///                     the attribute is not an array type, this value is
  ///                     ignored.
  void Get(bool& value, XU32 array_idx=0) const;

  /// \brief  gets the attribute as an int. If an array index is specified
  ///         it will return the i'th array item as an int.
  /// \param  value - the returned value
  /// \param  array_idx - if the attribute is an array type, then this will
  ///                     be the index of the array element to extract. If
  ///                     the attribute is not an array type, this value is
  ///                     ignored.
  void Get(XS32& value, XU32 array_idx=0) const;

  /// \brief  gets the attribute as an int2. If an array index is specified
  ///         it will return the i'th array item as an int2.
  /// \param  value - the returned value
  /// \param  array_idx - if the attribute is an array type, then this will
  ///                     be the index of the array element to extract. If
  ///                     the attribute is not an array type, this value is
  ///                     ignored.
  void Get(XInt2& value, XU32 array_idx=0) const;

  /// \brief  gets the attribute as a int3. If an array index is specified
  ///         it will return the i'th array item as an int3.
  /// \param  value - the returned value
  /// \param  array_idx - if the attribute is an array type, then this will
  ///                     be the index of the array element to extract. If
  ///                     the attribute is not an array type, this value is
  ///                     ignored.
  void Get(XInt3& value, XU32 array_idx=0) const;

  /// \brief  gets the attribute as an int4. If an array index is specified
  ///         it will return the i'th array item as an int4.
  /// \param  value - the returned value
  /// \param  array_idx - if the attribute is an array type, then this will
  ///                     be the index of the array element to extract. If
  ///                     the attribute is not an array type, this value is
  ///                     ignored.
  void Get(XInt4& value, XU32 array_idx=0) const;

  /// \brief  gets the attribute as a float. If an array index is specified
  ///         it will return the i'th array item as a float.
  /// \param  value - the returned value
  /// \param  array_idx - if the attribute is an array type, then this will
  ///                     be the index of the array element to extract. If
  ///                     the attribute is not an array type, this value is
  ///                     ignored.
  void Get(XReal& value, XU32 array_idx=0) const;

  /// \brief  gets the attribute as a float2. If an array index is specified
  ///         it will return the i'th array item as a float2.
  /// \param  value - the returned value
  /// \param  array_idx - if the attribute is an array type, then this will
  ///                     be the index of the array element to extract. If
  ///                     the attribute is not an array type, this value is
  ///                     ignored.
  void Get(XVector2& value, XU32 array_idx=0) const;

  /// \brief  gets the attribute as a float3. If an array index is specified
  ///         it will return the i'th array item as a float3.
  /// \param  value - the returned value
  /// \param  array_idx - if the attribute is an array type, then this will
  ///                     be the index of the array element to extract. If
  ///                     the attribute is not an array type, this value is
  ///                     ignored.
  void Get(XVector3& value, XU32 array_idx=0) const;

  /// \brief  gets the attribute as a float4. If an array index is specified
  ///         it will return the i'th array item as a float4.
  /// \param  value - the returned value
  /// \param  array_idx - if the attribute is an array type, then this will
  ///                     be the index of the array element to extract. If
  ///                     the attribute is not an array type, this value is
  ///                     ignored.
  void Get(XVector4& value, XU32 array_idx=0) const;

  /// \brief  gets the attribute as a string. If an array index is specified
  ///         it will return the i'th array item as a string.
  /// \param  value - the returned value
  /// \param  array_idx - if the attribute is an array type, then this will
  ///                     be the index of the array element to extract. If
  ///                     the attribute is not an array type, this value is
  ///                     ignored.
  void Get(XString& value, XU32 array_idx=0) const;

  /// \brief  gets the attribute as a colour. If an array index is specified
  ///         it will return the i'th array item as a colour.
  /// \param  value - the returned value
  /// \param  array_idx - if the attribute is an array type, then this will
  ///                     be the index of the array element to extract. If
  ///                     the attribute is not an array type, this value is
  ///                     ignored.
  void Get(XColour& value, XU32 array_idx=0) const;

  /// \brief  gets the attribute as a matrix. If an array index is specified
  ///         it will return the i'th array item as a matrix.
  /// \param  value - the returned value
  /// \param  array_idx - if the attribute is an array type, then this will
  ///                     be the index of the array element to extract. If
  ///                     the attribute is not an array type, this value is
  ///                     ignored.
  void Get(XMatrix& value, XU32 array_idx=0) const;

  //@}

  //--------------------------------------------------------------------------------------------------------------------
  /// \name   Set Methods
  /// \brief  set the values contained within the attribute
  //@{

  /// \brief  sets the attribute as a bool. If an array index is specified
  ///         it will set the i'th array item as a bool.
  /// \param  value - the new value
  /// \param  array_idx - if the attribute is an array type, then this will
  ///                     be the index of the array element to set. If
  ///                     the attribute is not an array type, this value is
  ///                     ignored.
  void Set(const bool& value, XU32 array_idx = 0);

  /// \brief  sets the attribute as an int. If an array index is specified
  ///         it will set the i'th array item as an int.
  /// \param  value - the new value
  /// \param  array_idx - if the attribute is an array type, then this will
  ///                     be the index of the array element to set. If
  ///                     the attribute is not an array type, this value is
  ///                     ignored.
  void Set(const XS32& value, XU32 array_idx = 0);

  /// \brief  sets the attribute as an int2. If an array index is specified
  ///         it will set the i'th array item as an int2.
  /// \param  value - the new value
  /// \param  array_idx - if the attribute is an array type, then this will
  ///                     be the index of the array element to set. If
  ///                     the attribute is not an array type, this value is
  ///                     ignored.
  void Set(const XInt2& value, XU32 array_idx = 0);

  /// \brief  sets the attribute as an int3. If an array index is specified
  ///         it will set the i'th array item as an int3.
  /// \param  value - the new value
  /// \param  array_idx - if the attribute is an array type, then this will
  ///                     be the index of the array element to set. If
  ///                     the attribute is not an array type, this value is
  ///                     ignored.
  void Set(const XInt3& value, XU32 array_idx = 0);

  /// \brief  sets the attribute as an int4. If an array index is specified
  ///         it will set the i'th array item as an int4.
  /// \param  value - the new value
  /// \param  array_idx - if the attribute is an array type, then this will
  ///                     be the index of the array element to set. If
  ///                     the attribute is not an array type, this value is
  ///                     ignored.
  void Set(const XInt4& value, XU32 array_idx = 0);

  /// \brief  sets the attribute as a float. If an array index is specified
  ///         it will set the i'th array item as a float.
  /// \param  value - the new value
  /// \param  array_idx - if the attribute is an array type, then this will
  ///                     be the index of the array element to set. If
  ///                     the attribute is not an array type, this value is
  ///                     ignored.
  void Set(const XReal value, XU32 array_idx = 0);

  /// \brief  sets the attribute as a float2. If an array index is specified
  ///         it will set the i'th array item as a float2.
  /// \param  value - the new value
  /// \param  array_idx - if the attribute is an array type, then this will
  ///                     be the index of the array element to set. If
  ///                     the attribute is not an array type, this value is
  ///                     ignored.
  void Set(const XVector2& value, XU32 array_idx = 0);

  /// \brief  sets the attribute as a float3. If an array index is specified
  ///         it will set the i'th array item as a float3.
  /// \param  value - the new value
  /// \param  array_idx - if the attribute is an array type, then this will
  ///                     be the index of the array element to set. If
  ///                     the attribute is not an array type, this value is
  ///                     ignored.
  void Set(const XVector3& value, XU32 array_idx = 0);

  /// \brief  sets the attribute as a float4. If an array index is specified
  ///         it will set the i'th array item as a float4.
  /// \param  value - the new value
  /// \param  array_idx - if the attribute is an array type, then this will
  ///                     be the index of the array element to set. If
  ///                     the attribute is not an array type, this value is
  ///                     ignored.
  void Set(const XVector4& value, XU32 array_idx = 0);

  /// \brief  sets the attribute as a string. If an array index is specified
  ///         it will set the i'th array item as a string.
  /// \param  value - the new value
  /// \param  array_idx - if the attribute is an array type, then this will
  ///                     be the index of the array element to set. If
  ///                     the attribute is not an array type, this value is
  ///                     ignored.
  void Set(const XString& value, XU32 array_idx = 0);

  /// \brief  sets the attribute as a colour. If an array index is specified
  ///         it will set the i'th array item as a colour.
  /// \param  value - the new value
  /// \param  array_idx - if the attribute is an array type, then this will
  ///                     be the index of the array element to set. If
  ///                     the attribute is not an array type, this value is
  ///                     ignored.
  void Set(const XColour& value, XU32 array_idx = 0);

  /// \brief  sets the attribute as a matrix. If an array index is specified
  ///         it will set the i'th array item as a matrix.
  /// \param  value - the new value
  /// \param  array_idx - if the attribute is an array type, then this will
  ///                     be the index of the array element to set. If
  ///                     the attribute is not an array type, this value is
  ///                     ignored.
  void Set(const XMatrix& value, XU32 array_idx = 0);

  //--------------------------------------------------------------------------------------------------------------------
  /// \name   Get Methods
  /// \brief  retrieve the values contained within the attribute
  //@{

  /// \brief  gets the attribute as a boolean array.
  /// \param  value - the returned array value.
  /// \return false if the array value could not be retrieved, true if OK
  bool Get(XBoolArray& value) const;

  /// \brief  gets the attribute as a signed int array.
  /// \param  value - the returned array value.
  /// \return false if the array value could not be retrieved, true if OK
  bool Get(XS32Array& value) const;

  /// \brief  gets the attribute as a int2 array.
  /// \param  value - the returned array value.
  /// \return false if the array value could not be retrieved, true if OK
  bool Get(XInt2Array& value) const;

  /// \brief  gets the attribute as a int3 array.
  /// \param  value - the returned array value.
  /// \return false if the array value could not be retrieved, true if OK
  bool Get(XInt3Array& value) const;

  /// \brief  gets the attribute as a int4 array.
  /// \param  value - the returned array value.
  /// \return false if the array value could not be retrieved, true if OK
  bool Get(XInt4Array& value) const;

  /// \brief  gets the attribute as a float array.
  /// \param  value - the returned array value.
  /// \return false if the array value could not be retrieved, true if OK
  bool Get(XRealArray& value) const;

  /// \brief  gets the attribute as a vector2 array.
  /// \param  value - the returned array value.
  /// \return false if the array value could not be retrieved, true if OK
  bool Get(XVector2Array& value) const;

  /// \brief  gets the attribute as a vector3 array.
  /// \param  value - the returned array value.
  /// \return false if the array value could not be retrieved, true if OK
  bool Get(XVector3Array& value) const;

  /// \brief  gets the attribute as a vector4 array.
  /// \param  value - the returned array value.
  /// \return false if the array value could not be retrieved, true if OK
  bool Get(XVector4Array& value) const;

  /// \brief  gets the attribute as a string array.
  /// \param  value - the returned array value.
  /// \return false if the array value could not be retrieved, true if OK
  bool Get(XStringList& value) const;

  /// \brief  gets the attribute as a colour array.
  /// \param  value - the returned array value.
  /// \return false if the array value could not be retrieved, true if OK
  bool Get(XColourArray& value) const;

  /// \brief  gets the attribute as a matrix array.
  /// \param  value - the returned array value.
  /// \return false if the array value could not be retrieved, true if OK
  bool Get(XMatrixArray& value) const;

  //@}

  //--------------------------------------------------------------------------------------------------------------------
  /// \name   Set Methods
  /// \brief  set the values contained within the attribute
  //@{

  /// \brief  sets the attribute as a boolean array.
  /// \param  value - the new array value
  /// \return false if the value could not be set, true otherwise.
  bool Set(const XBoolArray& value);

  /// \brief  sets the attribute as a signed int array.
  /// \param  value - the new array value
  /// \return false if the value could not be set, true otherwise.
  bool Set(const XS32Array& value);

  /// \brief  sets the attribute as an int2 array.
  /// \param  value - the new array value
  /// \return false if the value could not be set, true otherwise.
  bool Set(const XInt2Array& value);

  /// \brief  sets the attribute as an int3 array.
  /// \param  value - the new array value
  /// \return false if the value could not be set, true otherwise.
  bool Set(const XInt3Array& value);

  /// \brief  sets the attribute as an int4 array.
  /// \param  value - the new array value
  /// \return false if the value could not be set, true otherwise.
  bool Set(const XInt4Array& value);

  /// \brief  sets the attribute as a float array.
  /// \param  value - the new array value
  /// \return false if the value could not be set, true otherwise.
  bool Set(const XRealArray value);

  /// \brief  sets the attribute as a vector2 array.
  /// \param  value - the new array value
  /// \return false if the value could not be set, true otherwise.
  bool Set(const XVector2Array& value);

  /// \brief  sets the attribute as a vector3 array.
  /// \param  value - the new array value
  /// \return false if the value could not be set, true otherwise.
  bool Set(const XVector3Array& value);

  /// \brief  sets the attribute as a vector4 array.
  /// \param  value - the new array value
  /// \return false if the value could not be set, true otherwise.
  bool Set(const XVector4Array& value);

  /// \brief  sets the attribute as a string array.
  /// \param  value - the new array value
  /// \return false if the value could not be set, true otherwise.
  bool Set(const XStringList& value);

  /// \brief  sets the attribute as a colour array.
  /// \param  value - the new array value
  /// \return false if the value could not be set, true otherwise.
  bool Set(const XColourArray& value);

  /// \brief  sets the attribute as a matrix array.
  /// \param  value - the new array value
  /// \return false if the value could not be set, true otherwise.
  bool Set(const XMatrixArray& value);

  //@}

  //--------------------------------------------------------------------------------------------------------------------
  /// \name   Attribute connections
  /// \brief  functions to query or create input and output connections
  ///         between dynamic attributes. 
  //@{

  /// \brief  returns true if this attribute has an incomming connection
  /// \param  idx - the array index
  /// \return true if this attribue has an input connection
  ///
  bool HasInputConnection(XU32 idx = 0) const;

  /// \brief  returns true if this attribute has any output connections
  /// \param  idx - the array index
  /// \return true if this attribue has any output connections
  bool HasOutputConnection(XU32 idx = 0) const;

  /// \brief  sets an input connection on this attribute. Connections of
  ///         this type require both the node and name of the attribute
  ///         to which this attr is going to be connected. You can
  ///         optionally make connections to elements within an array by
  ///         specifying the idx array.
  /// \param  inputnode - the input node to connect
  /// \param  idx - the array index
  /// \param  extra_info  - not needed, however normally the maya output
  ///                       attribute name of inputnode, eg "outputMesh"
  bool SetInputConnection(const XBase* inputnode, XU32 idx = 0, const XChar* extra_info = 0);

  /// \brief  sets an input connection on this attribute. Connections of
  ///         this type require both the node and name of the attribute
  ///         to which this attr is going to be connected. You can
  ///         optionally make connections to elements within an array by
  ///         specifying the idx array.
  /// \param  inputnode - the input node to connect
  /// \param  idx - the array index
  /// \param  extra_info  - not needed, however normally the maya output
  ///                       attribute name of inputnode, eg "outputMesh"
  bool SetInputConnection( XId inputnode, XU32 idx = 0, const XChar* extra_info = 0);

  /// \brief  sets an output connection on this attribute. Connections of
  ///         this type require both the node and name of the attribute
  ///         to which this attr is going to be connected. You can
  ///         optionally make connections to elements within an array by
  ///         specifying the idx index parameter.
  /// \param  outputnode - the output node to connect
  /// \param  add_connection - true if you want to add a connection, false
  ///                          if you want any existing connections to be
  ///                          removed
  /// \param  idx - the array index
  /// \param  extra_info  - not needed, however normally the maya input
  ///                       attribute name of outputnode, eg "inputMesh"
  bool SetOutputConnection(const XBase* outputnode, bool add_connection, XU32 idx = 0, const XChar* extra_info=0);

  /// \brief  sets an output connection on this attribute. Connections of
  ///         this type require both the node and name of the attribute
  ///         to which this attr is going to be connected. You can
  ///         optionally make connections to elements within an array by
  ///         specifying the idx index parameter.
  /// \param  outputnode - the output node to connect
  /// \param  add_connection - true if you want to add a connection, false
  ///                          if you want any existing connections to be
  ///                          removed
  /// \param  idx - the array index
  /// \param  extra_info  - not needed, however normally the maya input
  ///                       attribute name of outputnode, eg "inputMesh"
  bool SetOutputConnection(XId outputnode, bool add_connection, XU32 idx = 0, const XChar* extra_info=0);

  /// \brief  returns the node that is connected to this attribute
  /// \param  inmodel - the input model
  /// \param  idx - the array index
  /// \return the node
  XBase* GetInputConnectionNode(XModel* inmodel, XU32 idx = 0) const;

  /// \brief  returns the attribute name on the node that is connected to
  ///         this attribute
  /// \param  inmodel - the input model
  /// \param  idx - the array index
  /// \return the attribute name
  const XString& GetInputConnectionName(XModel* inmodel, XU32 idx = 0) const;

  /// \brief  returns the node that is connected to this attribute
  /// \param  inmodel - the input model
  /// \param  connection_num - the array index
  /// \return the node
  XBase* GetOutputConnectionNode(XModel* inmodel, XU32 connection_num) const;

  /// \brief  returns the node that is connected to this attribute
  /// \param  inmodel - the input model
  /// \param  array_idx - the array index
  /// \param  connection_num - the array index
  /// \return the node
  XBase* GetOutputConnectionNode(XModel* inmodel, XU32 array_idx, XU32 connection_num) const;

  /// \brief  returns the attribute name on the node that is connected to
  ///         this attribute
  /// \param  inmodel - the input model
  /// \param  array_idx - the array index
  /// \param  connection_num - the array index
  /// \return the attribute name
  const XString& GetOutputConnectionName(XModel* inmodel, XU32 array_idx, XU32 connection_num) const;

  /// \brief  returns the attribute name on the node that is connected to
  ///         this attribute
  /// \param  inmodel - the input model
  /// \param  connection_num - the array index
  /// \return the attribute name
  const XString& GetOutputConnectionName(XModel* inmodel, XU32 connection_num) const;

  /// \brief  returns the number of output connections for the specified
  ///         array element of the attribute
  /// \param  array_idx - the array index
  /// \return the number of connections on that attribute
  XU32 GetNumOutputConnections(XU32 array_idx = 0) const;

  /// \brief  breaks the incoming connections to this attr on the specified index
  /// \param  array_idx - the index of the array element
  /// \return true if OK
  bool BreakInputConnections(XU32 array_idx);

  /// \brief  breaks all incoming connections to this attr
  /// \return true if OK
  bool BreakInputConnections();

  /// \brief  breaks outgoing connections on the specified array item
  /// \param  array_idx - the index of the array element
  /// \param  connection_num  - the index on that array element to delete
  /// \return true if OK
  bool BreakOutputConnection(XU32 array_idx, XU32 connection_num);

  /// \brief  breaks all outgoing connections
  /// \param  connection_num  - the index of the outgoing connection to delete
  /// \return true if OK
  bool BreakOutputConnection(XU32 connection_num);

  /// \brief  breaks all outgoing connections from this attr array element
  /// \param  array_idx - the index of the array element
  /// \return true if OK
  bool BreakAllOutputConnections(XU32 array_idx);

  /// \brief  deletes all outgoing connections from this node
  /// \return true if OK
  bool BreakAllOutputConnections();

  //@}

  //--------------------------------------------------------------------------------------------------------------------
  /// \name   Array functions
  /// \brief  array attribute manipulation
  //@{

  /// \brief  query whether the specified attribute is an array
  /// \return true if attribute is an array, false for a single item
  bool IsArray() const;

  /// \brief  If the attribute type is an array type, this returns the
  ///         number of elements in the array
  /// \return the array size
  XU32 NumElements() const;

  /// \brief  If the attribute type is an array type, this allows you to
  ///         set the number of elements in it.
  /// \param  sz  - the array size
  void Resize(XU32 sz);

  //@}

  //--------------------------------------------------------------------------------------------------------------------
  /// \name   General attribute properties
  /// \brief  functions to query or create input and output connections
  ///         between dynamic attributes. 
  //@{

  /// \brief  Sets the type of the attribute
  /// \param  type  - the new attribute type
  void SetType(XFn::Type type);

  /// \brief  returns the type of the attribute
  /// \return the type id
  const XFn::Type& Type() const;

  /// \brief  Retrieves the attribute name
  /// \return the name of the attribute
  const XChar* Name() const;
  //@}

private:
#ifndef DOXYGEN_PROCESSING

  bool DoData(XFileIO& io,XModel* model);
  XU32 GetDataSize() const;
  void NodeDeath(XModel* inmodel,XId node);
  void Clean();

  /// the data type of the attribute
  XFn::Type m_AttributeType;

  /// \brief  Internal structure used to hold info about an attribute connection
  struct XMD_EXPORT Connection
  {
    /// the ID of the connected node
    XId node_id;

    /// the array index for the connection
    XId array_idx;

    /// the name of the attribute to which it is connected
    XString extra_flag;

    XU32 GetDataSize() const;
    bool DoData(XFileIO& io);

    /// ctor
    Connection();

    /// ctor
    Connection(const XId id_,const XChar* str,const XId array_idx_=0);

    /// copy ctor
    Connection(const Connection& n);

  };

  typedef XM2::pod_vector<Connection> ConnectionArray;

  // a union of all the possible attribute types
  union
  {
    /// attribute data as a bool
    bool b;
    /// attribute data as an integer
    XS32 i;
    /// attribute data as an integer
    XInt2* i2;
    /// attribute data as an integer
    XInt3* i3;
    /// attribute data as an integer
    XInt4* i4;
    /// attribute data as a float
    XReal f;
    /// attribute data as a 3D vector
    XVector2* f2;
    /// attribute data as a 3D vector
    XVector3* f3;
    /// attribute d4ata as a 3D vector
    XVector4* f4;
    /// attribute data as a string
    XString* s;
    /// attribute data as a colour
    XColour* col;
    /// attribute data as a matrix
    XMatrix* m;
    /// attribute data as a bool array
    XBoolArray* ab;
    /// attribute data as an integer array
    XS32Array* ai;
    /// attribute data as an integer array
    XInt2Array* ai2;
    /// attribute data as an integer array
    XInt3Array* ai3;
    /// attribute data as an integer array
    XInt4Array* ai4;
    /// attribute data as a float array
    XRealArray* af;
    /// attribute data as a 3D vector array
    XVector2Array* af2;
    /// attribute data as a 3D vector array
    XVector3Array* af3;
    /// attribute data as a 3D vector array
    XVector4Array* af4;
    /// attribute data as a string array
    XStringList* as;
    /// attribute data as a colour array
    XColourArray* acol;
    /// attribute data as a matrix array
    XMatrixArray* am;
  };
  const Connection* FindInput(const XU32 idx) const;
  const Connection* FindOutput(const XU32 idx,const XU32 connection_num) const;
  const Connection* FindOutputFronNode(const XU32 idx,const XU32 node_num) const;
  Connection* FindInput(const XU32 idx);
  Connection* FindOutput(const XU32 idx,const XU32 connection_num);
  Connection* FindOutputFronNode(const XU32 idx,const XU32 node_num);
  /// an array of input connections to this attribute
  ConnectionArray* m_Inputs;
  /// an array of output connections
  ConnectionArray* m_Outputs;
  /// the name of the custom attribute
  XString m_AttrName;
#endif
};
/// an array of Attributes
typedef XM2::pod_vector<XAttribute*> XMD_EXPORT XAttributeList;
};
