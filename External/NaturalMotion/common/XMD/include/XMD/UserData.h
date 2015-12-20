//----------------------------------------------------------------------------------------------------------------------
/// \file   UserData.h
/// \author Rob Bateman [http://robthebloke.org]
/// \brief  User data allows you to add you own unique data structures with XMD nodes. This should negate the 
///         need to create your own referencing structures (i.e. std::map<XMD::XBase*,MyNode*>).
/// \note   (C) Copyright 2003-2005 Robert Bateman. All rights reserved.
//----------------------------------------------------------------------------------------------------------------------
#pragma once
//----------------------------------------------------------------------------------------------------------------------
#include "XMD/XFn.h"
#include "XM2/vector.h"
//----------------------------------------------------------------------------------------------------------------------
namespace XMD
{
class XMD_EXPORT XBase;
//----------------------------------------------------------------------------------------------------------------------
///  \brief  The XUserData class simply allows you to store your own C++
///         on any XMD node. This is primarily useful when writing importers
///         when you need to store any references between the XMD nodes and
///         the equivalent data types within the API or SDK you are using.
///         Each user data item should be assigned a unique name which can be
///         used to access the data.
///
///  \note  To use this class, simply inherit from the main XUserData
///      class, the following example demonstrates a simple lookup between
///     the XMD node and a respective MObject within the Maya API.
/// \code
///     class MyMayaLookup : public XMD::XUserData {
///      public:
///
///        MyMayaLookup(XBase* node)
///          : XMD::XUserData(node,"maya") {
///        }
///
///        // the name of the node in maya
///        MString m_MayaNodeName;
///
///        // the handle to the node within maya
///        MObject m_MayaHandle;
///
///       // You must implement a release method, since it is you who will
///       // be allocating the data structure.
///       virtual void Release() { delete this; }
///      };
/// \endcode
///
///      The user data should be automatically added to the node that
///      was passed in. The user data's destructor should automatically
///      remove the data from the node, and the node's destructor will
///      automatically destroy the user data. To add the user data is
///      simply a case of....
///
///  \code
///      // get some node to attach the data to
///      XBase* ptr = GetSomeDataNode();
///
///      // allocate user data and provide node pointer
///      MyMayaLookup* myUserData = new MyMayaLookup(ptr);
/// \endcode
///
///      To retrieve the data from the node, do something like this...
/// \code
///      // get a node to retrieve data from
///      XBase* ptr = GetSomeDataNode();
///
///      // search for user data named "maya", and cast returned pointer to correct type
///      MyMayaLookup* myUserData = static_cast<MyMayaLookup*>(ptr->GetUserData("maya"));
///  \endcode
//----------------------------------------------------------------------------------------------------------------------
class XMD_EXPORT XUserData
{
  friend class XMD_EXPORT XBase;
public:

  /// virtual dtor
  virtual ~XUserData();

  /// \brief  returns the identifier for the user data
  /// \return  the user data identifier as a string
  const XString& GetIdentifier() const;

protected:

  /// \brief  In order to allow you to create your own User Data structures and
  ///         to have that work safely with XMD over DLL bounds, you MUST implement
  ///         a release method to delete the memory, for example :
  ///
  ///           virtual void Release() { delete this; }
  virtual void Release() = 0;

  /// \brief  ctor
  /// \param  ptr    -  pointer to the XMD node to attach the data to
  /// \param  data_id  -  a string to uniquely identify this user data on the node
  XUserData(XBase* ptr, const XString& data_id);

  /// \brief  returns the node assoiated with the user data
  XBase* GetNode() { return pdata; }

  /// \brief  returns the node assoiated with the user data
  const XBase* GetNode() const { return pdata; }

private:
  XUserData(const XUserData& ud);
  /// the text string that identifies this used data element.
  XString identifier;
  /// pointer to the node associated with the user data
  XBase* pdata;
};
/// \brief  A collection of additional user data to be assigned to a node
typedef XM2::pod_vector<XUserData*> XUserDataArray;
}
