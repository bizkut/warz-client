//----------------------------------------------------------------------------------------------------------------------
/// \file CompoundProperties.cpp
/// \note (C) Copyright 2003-2005 Robert Bateman. All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#include "XMD/BaseProperties.h"
#include "XMD/CompoundProperties.h"

namespace XMD
{
//----------------------------------------------------------------------------------------------------------------------
struct IPropertyAccessor
{
  virtual ~IPropertyAccessor() {}

  virtual bool SetProperty(void*,const XU32,const XQuaternion&) { return false; }
  virtual bool SetProperty(void*,const XU32,const XVector3&) { return false; }
  virtual bool SetProperty(void*,const XU32,const bool&) { return false; }
  virtual bool SetProperty(void*,const XU32,const XReal&) { return false; }
  virtual bool SetProperty(void*,const XU32,const XColour&) { return false; }
  virtual bool SetProperty(void*,const XU32,const XString&) { return false; }
  virtual bool SetProperty(void*,const XU32,const XRealArray&) { return false; }
  virtual bool SetProperty(void*,const XU32,const XVector3Array&) { return false; }
  virtual bool SetProperty(void*,const XU32,const XU32Array&) { return false; }
  virtual bool SetProperty(void*,const XU32,const XU32&) { return false; }
  virtual bool SetProperty(void*,const XU32,const XS32&) { return false; }
  virtual bool SetProperty(void*,const XU32,const XMatrix&) { return false; }
  virtual bool SetProperty(void*,const XU32,const XBase*&) { return false; }
  virtual bool SetProperty(void*,const XU32,const XList&) { return false; }
  virtual bool GetProperty(void*,const XU32,XQuaternion&) { return false; }
  virtual bool GetProperty(void*,const XU32,XVector3&) { return false; }
  virtual bool GetProperty(void*,const XU32,bool&) { return false; }
  virtual bool GetProperty(void*,const XU32,XReal&) { return false; }
  virtual bool GetProperty(void*,const XU32,XColour&) { return false; }
  virtual bool GetProperty(void*,const XU32,XString&) { return false; }
  virtual bool GetProperty(void*,const XU32,XRealArray&) { return false; }
  virtual bool GetProperty(void*,const XU32,XU32Array&) { return false; }
  virtual bool GetProperty(void*,const XU32,XVector3Array&) { return false; }
  virtual bool GetProperty(void*,const XU32,XU32&) { return false; }
  virtual bool GetProperty(void*,const XU32,XS32&) { return false; }
  virtual bool GetProperty(void*,const XU32,XMatrix&) { return false; }
  virtual bool GetProperty(void*,const XU32,XBase*&) { return false; }
  virtual bool GetProperty(void*,const XU32,XList&) { return false; }
  virtual bool GetProperty(void*,const XU32,XCompoundPropertyArray&) { return false; }

  virtual XString GetPropertyName(void*,const XU32) { return ""; }
  virtual XPropertyType::Type GetPropertyType(void*,const XU32) { return XPropertyType::kInvalid; }
  virtual XU32 GetPropertyID(void*,const XString&) { return 0; }
  virtual XU32 GetNumProperties(void*) const { return 0; }
  virtual XU32 GetPropertyFlags(void*,const XU32) { return 0; }
  virtual XFn::Type GetPropertyNodeType(void*,const XU32) {return XFn::Base;}

  virtual void ReSize(void*,const XU32) = 0;
  virtual XU32 Size(void*) const = 0;
};

//----------------------------------------------------------------------------------------------------------------------
struct XPointIndexAccessor : public IPropertyAccessor 
{
};

//----------------------------------------------------------------------------------------------------------------------
struct XFaceArrayAccessor : public IPropertyAccessor
{
};

//----------------------------------------------------------------------------------------------------------------------
struct XSkinWieghtAccessor : public IPropertyAccessor
{
};

//----------------------------------------------------------------------------------------------------------------------
struct XTrimRegionAccessor : public IPropertyAccessor
{
};

//----------------------------------------------------------------------------------------------------------------------
struct XTrimBoundaryAccessor : public IPropertyAccessor
{
};

//----------------------------------------------------------------------------------------------------------------------
struct XTrimCurve2DAccessor : public IPropertyAccessor
{
};

//----------------------------------------------------------------------------------------------------------------------
struct XTrimCurve3DAccessor : public IPropertyAccessor
{
};
//----------------------------------------------------------------------------------------------------------------------
XCompoundProperty::~XCompoundProperty() 
{
}

//----------------------------------------------------------------------------------------------------------------------
XCompoundProperty::XCompoundProperty() 
  : m_parent(0), m_data(0)
{
}

//----------------------------------------------------------------------------------------------------------------------
XCompoundProperty::XCompoundProperty(const XCompoundProperty& rhs) 
  : m_parent(rhs.m_parent), m_data(rhs.m_data) 
{
}

//----------------------------------------------------------------------------------------------------------------------
bool XCompoundProperty::SetProperty(const XU32 property_id,const XQuaternion& val) 
{ 
  if(m_parent && m_parent->valid()) 
  {
    return m_parent->m_interface->SetProperty(m_data,property_id,val);
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool XCompoundProperty::SetProperty(const XU32 property_id,const XVector3& val)
{ 
  if(m_parent && m_parent->valid()) 
  {
    return m_parent->m_interface->SetProperty(m_data,property_id,val);
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool XCompoundProperty::SetProperty(const XU32 property_id,const bool& val)
{ 
  if(m_parent && m_parent->valid()) 
  {
    return m_parent->m_interface->SetProperty(m_data,property_id,val);
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool XCompoundProperty::SetProperty(const XU32 property_id,const XReal& val)
{ 
  if(m_parent && m_parent->valid()) 
  {
    return m_parent->m_interface->SetProperty(m_data,property_id,val);
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool XCompoundProperty::SetProperty(const XU32 property_id,const XColour& val)
{ 
  if(m_parent && m_parent->valid()) 
  {
    return m_parent->m_interface->SetProperty(m_data,property_id,val);
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool XCompoundProperty::SetProperty(const XU32 property_id,const XString& val)
{ 
  if(m_parent && m_parent->valid()) 
  {
    return m_parent->m_interface->SetProperty(m_data,property_id,val);
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool XCompoundProperty::SetProperty(const XU32 property_id,const XRealArray& val)
{ 
  if(m_parent && m_parent->valid()) 
  {
    return m_parent->m_interface->SetProperty(m_data,property_id,val);
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool XCompoundProperty::SetProperty(const XU32 property_id,const XVector3Array& val)
{ 
  if(m_parent && m_parent->valid()) 
  {
    return m_parent->m_interface->SetProperty(m_data,property_id,val);
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool XCompoundProperty::SetProperty(const XU32 property_id,const XU32Array& val)
{ 
  if(m_parent && m_parent->valid()) 
  {
    return m_parent->m_interface->SetProperty(m_data,property_id,val);
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool XCompoundProperty::SetProperty(const XU32 property_id,const XU32& val)
{ 
  if(m_parent && m_parent->valid()) 
  {
    return m_parent->m_interface->SetProperty(m_data,property_id,val);
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool XCompoundProperty::SetProperty(const XU32 property_id,const XS32& val)
{ 
  if(m_parent && m_parent->valid()) 
  {
    return m_parent->m_interface->SetProperty(m_data,property_id,val);
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool XCompoundProperty::SetProperty(const XU32 property_id,const XMatrix& val)
{ 
  if(m_parent && m_parent->valid()) 
  {
    return m_parent->m_interface->SetProperty(m_data,property_id,val);
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool XCompoundProperty::SetProperty(const XU32 property_id,const XBase*& val)
{ 
  if(m_parent && m_parent->valid()) 
  {
    return m_parent->m_interface->SetProperty(m_data,property_id,val);
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool XCompoundProperty::SetProperty(const XU32 property_id,const XList& val)
{ 
  if(m_parent && m_parent->valid()) 
  {
    return m_parent->m_interface->SetProperty(m_data,property_id,val);
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool XCompoundProperty::GetProperty(const XU32 property_id,XQuaternion& val)
{ 
  if(m_parent && m_parent->valid()) 
  {
    return m_parent->m_interface->GetProperty(m_data,property_id,val);
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool XCompoundProperty::GetProperty(const XU32 property_id,XVector3& val)
{ 
  if(m_parent && m_parent->valid()) 
  {
    return m_parent->m_interface->GetProperty(m_data,property_id,val);
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool XCompoundProperty::GetProperty(const XU32 property_id,bool& val)
{ 
  if(m_parent && m_parent->valid()) 
  {
    return m_parent->m_interface->GetProperty(m_data,property_id,val);
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool XCompoundProperty::GetProperty(const XU32 property_id,XReal& val)
{ 
  if(m_parent && m_parent->valid()) 
  {
    return m_parent->m_interface->GetProperty(m_data,property_id,val);
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool XCompoundProperty::GetProperty(const XU32 property_id,XColour& val)
{ 
  if(m_parent && m_parent->valid()) 
  {
    return m_parent->m_interface->GetProperty(m_data,property_id,val);
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool XCompoundProperty::GetProperty(const XU32 property_id,XString& val)
{ 
  if(m_parent && m_parent->valid()) 
  {
    return m_parent->m_interface->GetProperty(m_data,property_id,val);
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool XCompoundProperty::GetProperty(const XU32 property_id,XRealArray& val)
{ 
  if(m_parent && m_parent->valid()) 
  {
    return m_parent->m_interface->GetProperty(m_data,property_id,val);
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool XCompoundProperty::GetProperty(const XU32 property_id,XU32Array& val)
{ 
  if(m_parent && m_parent->valid()) 
  {
    return m_parent->m_interface->GetProperty(m_data,property_id,val);
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool XCompoundProperty::GetProperty(const XU32 property_id,XVector3Array& val)
{ 
  if(m_parent && m_parent->valid()) 
  {
    return m_parent->m_interface->GetProperty(m_data,property_id,val);
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool XCompoundProperty::GetProperty(const XU32 property_id,XU32& val)
{ 
  if(m_parent && m_parent->valid()) 
  {
    return m_parent->m_interface->GetProperty(m_data,property_id,val);
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool XCompoundProperty::GetProperty(const XU32 property_id,XS32& val) 
{ 
  if(m_parent && m_parent->valid()) 
  {
    return m_parent->m_interface->GetProperty(m_data,property_id,val);
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool XCompoundProperty::GetProperty(const XU32 property_id,XMatrix& val) 
{ 
  if(m_parent && m_parent->valid()) 
  {
    return m_parent->m_interface->GetProperty(m_data,property_id,val);
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool XCompoundProperty::GetProperty(const XU32 property_id,XBase*& val) 
{ 
  if(m_parent && m_parent->valid()) 
  {
    return m_parent->m_interface->GetProperty(m_data,property_id,val);
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool XCompoundProperty::GetProperty(const XU32 property_id,XList& val) 
{ 
  if(m_parent && m_parent->valid()) 
  {
    return m_parent->m_interface->GetProperty(m_data,property_id,val);
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool XCompoundProperty::GetProperty(const XU32 property_id,XCompoundPropertyArray& val) 
{ 
  if(m_parent && m_parent->valid()) 
  {
    return m_parent->m_interface->GetProperty(m_data,property_id,val);
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
XString XCompoundProperty::GetPropertyName(const XU32 property_id) 
{ 
  return m_parent->m_interface->GetPropertyName(m_data,property_id); 
}

//----------------------------------------------------------------------------------------------------------------------
XPropertyType::Type XCompoundProperty::GetPropertyType(const XU32 property_id) 
{ 
  return m_parent->m_interface->GetPropertyType(m_data,property_id);  
}

//----------------------------------------------------------------------------------------------------------------------
XU32 XCompoundProperty::GetPropertyID(const XString& property_name) 
{ 
  return m_parent->m_interface->GetPropertyID(m_data,property_name); 
}

//----------------------------------------------------------------------------------------------------------------------
XU32 XCompoundProperty::GetNumProperties() const 
{ 
  return m_parent->m_interface->GetNumProperties(m_data); 
}

//----------------------------------------------------------------------------------------------------------------------
XU32 XCompoundProperty::GetPropertyFlags(const XU32 property_id)
{ 
  return m_parent->m_interface->GetPropertyFlags(m_data,property_id); 
}

//----------------------------------------------------------------------------------------------------------------------
XFn::Type XCompoundProperty::GetPropertyNodeType(const XU32 property_id) 
{
  return m_parent->m_interface->GetPropertyNodeType(m_data,property_id);
}

//----------------------------------------------------------------------------------------------------------------------
bool XCompoundPropertyArray::valid() const
{
  return m_setIndexFunc && m_node && m_interface && m_data;
}

//----------------------------------------------------------------------------------------------------------------------
XCompoundPropertyArray::iterator XCompoundPropertyArray::begin()
{
  return mData.begin();
}

//----------------------------------------------------------------------------------------------------------------------
XCompoundPropertyArray::const_iterator XCompoundPropertyArray::begin() const
{
  return mData.begin();
}

//----------------------------------------------------------------------------------------------------------------------
XCompoundPropertyArray::iterator XCompoundPropertyArray::end()
{
  return mData.end();
}

//----------------------------------------------------------------------------------------------------------------------
XCompoundPropertyArray::const_iterator XCompoundPropertyArray::end() const
{
  return mData.end();
}
/*
//----------------------------------------------------------------------------------------------------------------------
XCompoundPropertyArray::reverse_iterator XCompoundPropertyArray::rbegin()
{
  return mData.rbegin();
}

//----------------------------------------------------------------------------------------------------------------------
XCompoundPropertyArray::const_reverse_iterator XCompoundPropertyArray::rbegin() const
{
  return mData.rbegin();
}

//----------------------------------------------------------------------------------------------------------------------
XCompoundPropertyArray::reverse_iterator XCompoundPropertyArray::rend()
{
  return mData.rend();
}

//----------------------------------------------------------------------------------------------------------------------
XCompoundPropertyArray::const_reverse_iterator XCompoundPropertyArray::rend() const
{
  return mData.rend();
}
*/

//----------------------------------------------------------------------------------------------------------------------
XCompoundPropertyArray::size_type XCompoundPropertyArray::size() const
{
  return mData.size();
}

//----------------------------------------------------------------------------------------------------------------------
bool XCompoundPropertyArray::resize(XCompoundPropertyArray::size_type size_)
{
  if(valid())
  {
    mData.resize(size_);

    iterator it = begin();
    iterator end_ = end();

    for (XU32 i=0;it !=end_;++it,++i)
    {
      // assign internal data pointers to the array element
      m_setIndexFunc(m_data,i,it->m_data);

      // assign pointer to parent array on each element index
      it->m_parent = this;
    }

    return true;
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
const XCompoundProperty& XCompoundPropertyArray::operator[](const XU32 index) const
{
  XMD_ASSERT(index<size());
  return mData[index];
}

//----------------------------------------------------------------------------------------------------------------------
XCompoundProperty& XCompoundPropertyArray::operator[](const XU32 index)
{
  XMD_ASSERT(index<size());
  return mData[index];
}
}
