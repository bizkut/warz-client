//----------------------------------------------------------------------------------------------------------------------
/// \file   Base.h
/// \author Rob Bateman [http://robthebloke.org]
/// \brief  This file defines the base class of all node types and classes within XMD.
/// \note   (C) Copyright 2003-2005 Robert Bateman. All rights reserved.
//----------------------------------------------------------------------------------------------------------------------
#pragma once
//----------------------------------------------------------------------------------------------------------------------
#include <fstream>
#include "XMD/XFn.h"
#include "XMD/UserData.h"
#include "XMD/NodeTypeRegister.h"
#include "XM2/Colour.h"
#include "XM2/Vector3.h"
#include "XM2/Quaternion.h"
#include "XM2/RealArray.h"
//----------------------------------------------------------------------------------------------------------------------
namespace XMD
{
#ifndef DOXYGEN_PROCESSING
class XMD_EXPORT XModel;
class XMD_EXPORT XBasicLogger;
class XMD_EXPORT XInheritedNode;
class XMD_EXPORT XUserData;
class XMD_EXPORT XBase;
class XMD_EXPORT XAnimCycle;
class XMD_EXPORT XAttribute;
class XMD_EXPORT XExtraAttributes;
class XMD_EXPORT XFileIO;
class XMD_EXPORT XAnimCurve;
class XMD_EXPORT XBase;
class XBaseProperties;

template<typename T>
struct TApiType {
  enum { _apitype = XFn::Base };
};

#ifndef DOXYGEN_PROCESSING
  #if _MSC_VER  >= 1400
    #define XMD_DEPRECATED(INFO) __declspec(deprecated(INFO))
  #else
    #define XMD_DEPRECATED(INFO)
  #endif
#else
  #define XMD_DEPRECATED(INFO)
#endif

/// \brief  An array of XMD nodes
typedef XM2::pod_vector<XBase*> XList;

#define DECLARE_API_TYPE(XFN,CLASS,TYPENAME)                      \
  template<> struct TApiType< CLASS > {                           \
    enum { _apitype = XFN };                                      \
    typedef CLASS _class;                                         \
  };                                                              \
  struct IXCreateBase_##CLASS : public IXCreateBase {             \
    XFn::Type GetType() const { return XFN; }                     \
    XString GetTypeString() const { return TYPENAME; }            \
    XString GetChunkName() const { return ""; }                   \
    XBase* CreateNode(XModel* mod) const{ (void)mod; return 0 ; } \
  };

#define DECLARE_CHUNK_TYPE(XFN,CLASS,TYPENAME,CHUNKNAME)            \
  template<> struct TApiType< CLASS > {                             \
    enum { _apitype = XFN };                                        \
    typedef CLASS _class;                                           \
  };                                                                \
                                                                    \
struct IXCreateBase_##CLASS : public IXCreateBase{                  \
    XFn::Type GetType() const { return XFN; }                       \
    XString GetTypeString() const { return TYPENAME; }              \
    XString GetChunkName() const { return CHUNKNAME; }              \
    XBase* CreateNode(XModel* mod) const { return new CLASS(mod) ; }\
  };


#define XMD_ABSTRACT_CHUNK(___TYPE___)                \
    private:                                          \
      friend class XMD_EXPORT XModel;                 \
    protected:                                        \
    ___TYPE___##(XModel*);                            \
      ~##___TYPE___##();                              \
    protected:                                        \
      virtual XBaseProperties* GetProperties() const; \
      virtual XFn::Type GetApiType() const;           \
      virtual void DoCopy(const XBase*);              \
      virtual XBase* GetFn(XFn::Type);                \
      virtual const XBase* GetFn(XFn::Type) const;    \
      virtual bool NodeDeath(XId);                    \
      virtual void PreDelete(XIdSet&);                \
      virtual bool ReadChunk(std::istream&);          \
      virtual bool WriteChunk(std::ostream&);         \
      virtual bool DoData(XFileIO&);                  \
      virtual XU32 GetDataSize() const; 


#define XMD_CHUNK(___TYPE___)                         \
    private:                                          \
      friend struct IXCreateBase_##___TYPE___ ;       \
      XMD_ABSTRACT_CHUNK(___TYPE___)


#ifndef XPROPERTY_TYPE_DEFINED__
#define XPROPERTY_TYPE_DEFINED__
class XPropertyType 
{
public:
  enum Type {
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
    kNodeArray
  };
};
#endif

//----------------------------------------------------------------------------------------------------------------------
/// \brief  The header definition for the binary format
//----------------------------------------------------------------------------------------------------------------------
#pragma pack(push,1)
struct XChunkHeader
{
  /// the XFn::Type to identify the node type in the file
  XU16 Type;
  /// boolean flag to determine if the node chunk has additional attributes
  XU8  HasExtraAttrs;
  /// reserved for future use
  XU8  Reserved;
  /// the total size in bytes of the node chunk data that follows
  XU32 ByteSize;
};
#pragma pack(pop)
#endif // DOXYGEN_PROCESSING

/// a set of ID's used to build up a complete set of nodes that need to be deleted
typedef std::set<XId> XIdSet;

//----------------------------------------------------------------------------------------------------------------------
/// \class XMD::XBase
/// \brief This is the base class of everything stored in the XMD file.
///        From this class you can use GetFn<>() and ApiType() to determine the
///        type of any node. This class also provides mechanisms to add and
///        access generic attributes on the node.
//----------------------------------------------------------------------------------------------------------------------
class XMD_EXPORT XBase
  : public XFn
{
#ifndef DOXYGEN_PROCESSING
  friend class XMD_EXPORT XModel;
  friend class XMD_EXPORT XUserData;
  friend class XMD_EXPORT XExtraAttributes;
  friend XBase XMD_EXPORT * XCreateNode(XModel* pmod, const XFn::Type type);

protected:

  /// dtor
  virtual ~XBase();


protected:
  /// \brief  Deprecated: Causes alloc/free issues across DLL bounds.
  ///         Instead, use the new method CreateAttrribute()
  /// \internal
  bool AddAttribute(XAttribute* attr);

#endif
public:

  //--------------------------------------------------------------------------------------------------------------------
  // property ID's
  static const XU32 kLast = 0;
  //--------------------------------------------------------------------------------------------------------------------

  //--------------------------------------------------------------------------------------------------------------------
  /// \name Dynamic attribute functions
  /// \brief  functions to allow you to query, add and manipulate 
  ///         additional dynamic attributes onto an XBase derived node
  //@{

  /// \brief  Creates a new dynamic attribute on this node.
  /// \param  name  - the name of the attribute to create
  /// \return the new attribute created, or NULL if the attribute name
  ///         was invalid (i.e. the attribute already exists)
  XAttribute* CreateAttribute(const XChar* name);

  /// \brief  returns a pointer to the requested attribute
  /// \param  name  - the name of the attribute to retrieve
  /// \return a pointer to the attribute.
  XAttribute* GetAttribute(const XChar* name);

  /// \brief  returns a pointer to the requested attribute
  /// \param  name  - the name of the attribute to retrieve
  /// \return a pointer to the attribute.
  const XAttribute* GetAttribute(const XChar* name) const;

  /// \brief  returns an array of the attribute names
  /// \param  attrs - the returned list of attributes
  void ListAttributes(XStringList& attrs) const;

  /// \brief  Deletes a user attribute from the node
  /// \param  attr_name - the name of the attribute to delete 
  /// \return true if deleted OK
  bool DeleteAttrribute(const XChar* attr_name);

  /// \brief  Deletes a user attribute from the node
  /// \param  attr - a pointer to the attribute to delete. NOTE:
  ///         this pointer will be invalid immediately after this call!
  /// \return true if deleted OK
  bool DeleteAttrribute(const XAttribute* attr);

  //@}

  //--------------------------------------------------------------------------------------------------------------------
  /// \name   Node and Type query functions
  /// \brief  functions to allow you to query the type and name of a
  ///         node
  //@{

  /// \brief  gets the type of the object
  /// \return The type ID
  XFn::Type ApiType() const;

  /// \brief  gets the type of the object as a string
  /// \return The type string
  const XString ApiTypeString() const;

  /// \brief  a template function to return a specified function set.
  ///         uses the TApiType template
  template<typename T>
  T* HasFn()
  {
    return reinterpret_cast< T* >( this->GetFn( static_cast<XFn::Type>(TApiType<T>::_apitype) ) );
  }

  /// \brief  a template function to return a specified function set.
  ///         uses the TApiType template
  template<typename T>
  const T* HasFn() const
  {
    return reinterpret_cast< const T* >( this->GetFn( static_cast<XFn::Type>(TApiType<T>::_apitype) ) );
  }

  /// \brief  This is used to get access to higher derivation levels 
  ///         of the inheritance hierarchy. 
  /// \param  type   - the type of the function set
  /// \return a pointer to that level in the hierachy or NULL
  XBase* HasFn(XFn::Type type);

  /// \brief  returns the name of the object
  /// \return the object name
  const XChar* GetName() const;

  /// \brief  allows you to set the name of this node
  /// \param  c - the new name
  void SetName(const XChar* c);

  /// \brief  Returns the ID number of this node.
  /// \return the ID num
  XId GetID() const;

  /// \brief  returns a pointer to the model that this node is contained within
  /// \return the model pointer
  XModel* GetModel();

  /// \brief  returns a pointer to the model that this node is contained within
  /// \return the model pointer
  const XModel* GetModel() const;

  //@}

  //--------------------------------------------------------------------------------------------------------------------
  /// \name   User Data Functions
  /// \brief  provide the ability to tag any node with your own
  ///         additional data.
  //@{

  /// \brief  returns a pointer to a custom user data chunk
  /// \param  name  - the name of the user data chunk
  /// \return the user data ptr
  XUserData* GetUserData(const XString& name);

  /// \brief  returns a pointer to a custom user data chunk
  /// \param  name  - the name of the user data chunk
  /// \return the user data ptr
  const XUserData* GetUserData(const XString& name) const;

  /// \brief  This function deletes the user data from the XBase node.
  /// \param  name  - the name of the user data chunk to delete
  /// \return true if deleted OK. False if unknown data
  bool DeleteUserData(const XString& name);

  /// \brief  copies this node and returns the new copy to you.
  /// \return the copy of the object
  XBase* Clone() const;

  //@}

  //--------------------------------------------------------------------------------------------------------------------
  /// \name   Generic Property Access
  /// \brief  provides the ability to generically access all properties 
  ///         on this node. Also allows you to create animation curves 
  ///         on any (key-able) attribute.
  //@{

  /// \brief  Sets the value of the stored quaternion property 
  /// \param  property_id - the ID of the property
  /// \param  val - the new value of the property
  /// \return true if the property could be set, false otherwise
  bool SetProperty(XU32 property_id, const XQuaternion& val);

  /// \brief  Sets the value of the stored vector3 property 
  /// \param  property_id - the ID of the property
  /// \param  val - the new value of the property
  /// \return true if the property could be set, false otherwise
  bool SetProperty(XU32 property_id, const XVector3& val);

  /// \brief  Sets the value of the stored boolean property 
  /// \param  property_id - the ID of the property
  /// \param  val - the new value of the property
  /// \return true if the property could be set, false otherwise
  bool SetProperty(XU32 property_id, const bool& val);

  /// \brief  Sets the value of the stored floating point property 
  /// \param  property_id - the ID of the property
  /// \param  val - the new value of the property
  /// \return true if the property could be set, false otherwise
  bool SetProperty(XU32 property_id, const XReal& val);

  /// \brief  Sets the value of the stored integer property 
  /// \param  property_id - the ID of the property
  /// \param  val - the new value of the property
  /// \return true if the property could be set, false otherwise
  bool SetProperty(XU32 property_id, const XS32& val);

  /// \brief  Sets the value of the stored integer property 
  /// \param  property_id - the ID of the property
  /// \param  val - the new value of the property
  /// \return true if the property could be set, false otherwise
  /// 
  bool SetProperty(XU32 property_id, const XU32& val);

  /// \brief  Sets the value of the stored colour property 
  /// \param  property_id - the ID of the property
  /// \param  val - the new value of the property
  /// \return true if the property could be set, false otherwise
  bool SetProperty(XU32 property_id, const XColour& val);

  /// \brief  Sets the value of the stored string property 
  /// \param  property_id - the ID of the property
  /// \param  val - the new value of the property
  /// \return true if the property could be set, false otherwise
  bool SetProperty(XU32 property_id, const XString& val);

  /// \brief  Sets the value of the stored node property 
  /// \param  property_id - the ID of the property
  /// \param  val - the new value of the property
  /// \return true if the property could be set, false otherwise
  bool SetProperty(XU32 property_id, const XBase* val);

  /// \brief  Sets the value of the stored string property 
  /// \param  property_id - the ID of the property
  /// \param  val - the new value of the property
  /// \return true if the property could be set, false otherwise
  bool SetProperty(XU32 property_id, const XMatrix& val);

  /// \brief  Sets the value of the stored float array property 
  /// \param  property_id - the ID of the property
  /// \param  val - the new value of the property
  /// \return true if the property could be set, false otherwise
  bool SetProperty(XU32 property_id, const XRealArray& val);

  /// \brief  Sets the value of the stored unsigned int array property 
  /// \param  property_id - the ID of the property
  /// \param  val - the new value of the property
  /// \return true if the property could be set, false otherwise
  bool SetProperty(XU32 property_id, const XU32Array& val);

  /// \brief  Sets the value of the stored vector3 array property 
  /// \param  property_id - the ID of the property
  /// \param  val - the new value of the property
  /// \return true if the property could be set, false otherwise
  bool SetProperty(XU32 property_id, const XVector3Array& val);

  /// \brief  Sets the value of the stored node array property 
  /// \param  property_id - the ID of the property
  /// \param  val - the new value of the property
  /// \return true if the property could be set, false otherwise
  bool SetProperty(XU32 property_id, const XList& val);

  /// \brief  Gets the value of the stored quaternion property 
  /// \param  property_id - the ID of the property
  /// \param  val - will store the returned property value
  /// \return true if the property could be retrieved, false otherwise
  bool GetProperty(XU32 property_id, XQuaternion& val) const;

  /// \brief  Gets the value of the stored vector3 property 
  /// \param  property_id - the ID of the property
  /// \param  val - will store the returned property value
  /// \return true if the property could be retrieved, false otherwise
  bool GetProperty(XU32 property_id, XVector3& val) const;

  /// \brief  Gets the value of the stored boolean property 
  /// \param  property_id - the ID of the property
  /// \param  val - will store the returned property value
  /// \return true if the property could be retrieved, false otherwise
  bool GetProperty(XU32 property_id, bool& val) const;

  /// \brief  Gets the value of the stored boolean property 
  /// \param  property_id - the ID of the property
  /// \param  val - will store the returned property value
  /// \return true if the property could be retrieved, false otherwise
  bool GetProperty(XU32 property_id, XS32& val) const;

  /// \brief  Gets the value of the stored boolean property 
  /// \param  property_id - the ID of the property
  /// \param  val - will store the returned property value
  /// \return true if the property could be retrieved, false otherwise
  bool GetProperty(XU32 property_id, XU32& val) const;

  /// \brief  Gets the value of the stored float property 
  /// \param  property_id - the ID of the property
  /// \param  val - will store the returned property value
  /// \return true if the property could be retrieved, false otherwise
  bool GetProperty(XU32 property_id, XReal& val) const;

  /// \brief  Gets the value of the stored colour property 
  /// \param  property_id - the ID of the property
  /// \param  val - will store the returned property value
  /// \return true if the property could be retrieved, false otherwise
  bool GetProperty(XU32 property_id, XColour& val) const;

  /// \brief  Gets the value of the stored string property 
  /// \param  property_id - the ID of the property
  /// \param  val - will store the returned property value
  /// \return true if the property could be retrieved, false otherwise
  bool GetProperty(XU32 property_id, XString& val) const;

  /// \brief  Gets the value of the stored matrix property 
  /// \param  property_id - the ID of the property
  /// \param  val - will store the returned property value
  /// \return true if the property could be retrieved, false otherwise
  bool GetProperty(XU32 property_id, XMatrix& val) const;

  /// \brief  Gets the value of the stored node property 
  /// \param  property_id - the ID of the property
  /// \param  val - will store the returned property value
  /// \return true if the property could be retrieved, false otherwise
  bool GetProperty(XU32 property_id, XBase*& val) const;

  /// \brief  Gets the value of the stored float array property 
  /// \param  property_id - the ID of the property
  /// \param  val - will store the returned property value
  /// \return true if the property could be retrieved, false otherwise
  bool GetProperty(XU32 property_id, XRealArray& val) const;

  /// \brief  Gets the value of the stored unsigned int array property 
  /// \param  property_id - the ID of the property
  /// \param  val - will store the returned property value
  /// \return true if the property could be retrieved, false otherwise
  bool GetProperty(XU32 property_id, XU32Array& val) const;

  /// \brief  Gets the value of the stored vector3 array property 
  /// \param  property_id - the ID of the property
  /// \param  val - will store the returned property value
  /// \return true if the property could be retrieved, false otherwise
  bool GetProperty(XU32 property_id, XVector3Array& val) const;

  /// \brief  Gets the value of the stored node array property 
  /// \param  property_id - the ID of the property
  /// \param  val - will store the returned property value
  /// \return true if the property could be retrieved, false otherwise
  bool GetProperty(XU32 property_id, XList& val) const;

  /// \brief  returns the property name given the specified property id
  /// \param  property_id - the ID of the property to query. 
  /// \return the actual name of the property
  XString GetPropertyName(XU32 property_id) const;

  /// \brief  returns the property name given the specified property id
  /// \param  property_id - the ID of the property to query. 
  /// \return the actual name of the property
  XPropertyType::Type GetPropertyType(XU32 property_id) const;

  /// \brief  returns the property ID associated with the specified 
  ///         property name
  /// \param  property_name - the name of the property to query. 
  /// \return the returned ID for the property
  XU32 GetPropertyID(const XString& property_name) const;

  /// \brief  returns the total number of properties on this node
  /// \return the total number of properties available on this node
  XU32 GetNumProperties() const;

  /// \brief  returns true if the specified property can be animated
  /// \param  property_id - the property id to query
  /// \return true if the property can be animated 
  bool IsPropertyAnimatable(XU32 property_id) const;

  /// \brief  returns true if the specified property is read only
  /// \param  property_id - the property id to query
  /// \return true if the property is read only 
  bool IsPropertyReadOnly(XU32 property_id) const;

  /// \brief  returns true if the specified property is an array that 
  ///         can be resized. (i.e. Some array properties such as blend
  ///         shape weights cannot be re-sized since they depend solely 
  ///         on how many blend shape targets are set...)
  /// \param  property_id - the property id to query
  /// \return true if the property is an array that can be re-sized. 
  bool IsPropertySizeable(XU32 property_id) const;

  /// \brief  returns the type of the node required for a given property
  ///         when the property type is kNode or kNodeArray. 
  /// \param  property_id - the id of the property
  /// \return the property node type
  XFn::Type GetPropertyNodeType(XU32 property_id) const;

  //@}

protected:
#ifndef DOXYGEN_PROCESSING

  /// \brief  returns a reference to the internal property type info 
  ///         for this class. Overridden for each additional class type.
  /// \return the type info for this classes properties.
  virtual XBaseProperties* GetProperties() const;

  /// \brief  ctor
  /// \param  pModel  - the model that contains this node
  XBase(XModel* pModel);

  /// \brief  internal query func to see if the specified object is valid
  /// \param  id  - the id of the node to query
  /// \return true if valid
  bool IsValidObj(XId id);

  /// \brief  internal query func to see if the specified object is valid
  /// \param  ptr  - the base pointer of the node to query
  /// \return true if valid
  bool IsValidObj(const XBase* ptr);

  /// \brief  internal query func to see if the specified object is valid
  /// \param  id  - the id of the node to query
  /// \return true if valid
  const bool IsValidObj(XId id) const;

  /// \brief  internal query func to see if the specified object is valid
  /// \param  ptr  - the base pointer of the node to query
  /// \return true if valid
  const bool IsValidObj(const XBase* ptr) const;

  /// \brief  used to add a user data item to the node
  /// \param  ud  - the user data structure
  /// \return true if OK
  bool AddUserData(XUserData* ud);

  /// \brief  removes the specified user data chunk from the node
  /// \param  ud  - the user data structure to remove
  /// \return true if OK
  bool RemUserData(XUserData* ud);

  /// \brief  gets the type of the object
  /// \return The type of the object XFn::Base
  virtual XFn::Type GetApiType() const;

  /// \brief  This is used to get access to higher derivation levels of
  ///         the inheritance hierarchy.
  /// \param  type  - the type of the function set
  virtual XBase* GetFn(XFn::Type type);
  virtual const XBase* GetFn(XFn::Type type) const;

  /// \brief  Used within binary fileIO. returns the total size in bytes
  ///         of the data so that it can be written to the file.
  /// \return the total size in bytes required to store this node on disk
  virtual XU32 GetDataSize() const;

  /// \brief  This function is used to notify an object about the recent
  ///         death of another node. This allows it to check for any
  ///         broken dependencies and set the offending dependency to NULL.
  /// \param  id  - the id of the transform
  /// \return true if OK
  virtual bool NodeDeath(XId id);

  /// \brief  called before node gets deleted, allows it to check the
  ///         models delete policy to see if anything needs to be deleted.
  /// \param  extra_nodes - insert a node into this list to safely delete
  ///         it.
  virtual void PreDelete(XIdSet& extra_nodes);

  /// \brief  This function writes the data for the chunk. The header,
  ///         name, extra attributes and surrounding braces will have
  ///         been written already.
  /// \param  io  - the output file stream
  /// \return true if OK
  virtual bool DoData(XFileIO& io);

  /// \brief  copies the specified object into this.
  /// \param  rhs - the object to copy.
  virtual void DoCopy(const XBase* rhs);

  /// \brief  This function writes the data for the chunk. The header,
  ///         name, extra attributes and surrounding braces will have
  ///         been written already.
  /// \param  io  - the output file stream
  /// \return true if OK
  ///
  bool DoHeader(XFileIO& io);

  /// \brief  This function writes the data for the chunk. The header,
  ///         name, extra attributes and surrounding braces will have
  ///         been written already.
  /// \param  ofs - the output file stream
  /// \return true if OK
  ///
  virtual bool WriteChunk(std::ostream& ofs);

  /// \brief  This function reads the data for the chunk. The header,
  ///         name, extra attributes and surrounding braces get read
  ///         for you.
  /// \param  ifs - the input file stream
  /// \return true if OK
  virtual bool ReadChunk(std::istream& ifs);

  /// \brief  This function exists so that any additional internal
  ///         references can be setup before any data gets accessed
  ///         by the lib user. ie, some internal ID's are not loaded
  ///         os stored in the file format, however it's handy to be able
  ///         to traverse both ways through the scene graph....
  virtual void PostLoad();

private:

  XString GetTypeString();

  /// \brief  Internal function used to read attribute connections from
  ///         the file
  /// \param  ifs - the input file stream
  /// \return true if OK
  bool ReadConnections(std::istream& ifs);

  /// \brief  Internal function used to write attribute connections to
  ///         the file
  /// \param  ofs - the output file stream
  /// \return true if OK
  bool WriteConnections(std::ostream& ofs);

  /// \brief  Simply reads the Header for this chunk
  /// \param  ifs - the input file stream
  /// \return true if OK
  bool ReadHeader(std::istream& ifs);

  /// \brief  reads any extra attributes etc
  /// \param  ifs - the input file stream
  /// \return true if OK
  bool ReadFooter(std::istream& ifs);

  /// \brief  Simply outputs the Header for this chunk
  /// \param  ofs - the output file stream
  /// \return true if OK
  bool WriteHeader(std::ostream& ofs);

  /// \brief  Simply outputs the Footer for this chunk. This basically
  ///         means any attributes it has stored on this node.
  /// \param  ofs - the output file stream
  /// \return true if OK
  bool WriteFooter(std::ostream& ofs);

  /// \brief  reads any extra attributes etc
  /// \param  ifs - the input file stream
  /// \return true if OK
  bool DoReadChunk(std::istream& ifs);

  /// \brief  writes any extra attributes etc
  /// \param  ofs - the output file stream
  /// \return true if OK
  bool DoWriteChunk(std::ostream& ofs);

protected:

  /// the ID number of this node
  XId m_ThisID;

  /// the name of the item.
  XString m_Name;

  /// a pointer to a structure to hold extra attribute data
  XExtraAttributes* m_pAttributes;

  /// a pointer to the model the chunk belongs to
  mutable XModel* m_pModel;

  /// a pointer to the inherited node info for this class.
  XInheritedNode* m_pInherited;

  /// all of the user data items attached to this node
  XUserDataArray m_UserData;

#endif
};

#ifndef DOXYGEN_PROCESSING
extern XId XMD_EXPORT PosInList(XIndexList& ids,XId orig);
DECLARE_API_TYPE(XFn::Base, XBase, "base");
#endif
};
