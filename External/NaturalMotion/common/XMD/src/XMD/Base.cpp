//----------------------------------------------------------------------------------------------------------------------
/// \file Base.cpp
/// \note (C) Copyright 2003-2005 Robert Bateman. All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#include "XMD/XFn.h"
#include "XMD/UserData.h"
#include "XMD/Model.h"
#include "XMD/Base.h"
#include "XMD/NewNode.h"
#include "XMD/InheritedNode.h"
#include "XMD/ExtraAttributes.h"
#include "XMD/NodeTypeRegister.h"
#include "XMD/FileIO.h"
#include "XMD/NodeProperties.h"
#include "XMD/BaseProperties.h"

NMTL_POD_VECTOR_EXPORT(bool,XMD_EXPORT,XM2);
NMTL_POD_VECTOR_EXPORT(XMD::XS8,XMD_EXPORT,XM2);
NMTL_POD_VECTOR_EXPORT(XMD::XS16,XMD_EXPORT,XM2);
NMTL_POD_VECTOR_EXPORT(XMD::XS32,XMD_EXPORT,XM2);
NMTL_POD_VECTOR_EXPORT(XMD::XU8,XMD_EXPORT,XM2);
NMTL_POD_VECTOR_EXPORT(XMD::XU16,XMD_EXPORT,XM2);
NMTL_POD_VECTOR_EXPORT(XMD::XU32,XMD_EXPORT,XM2);
NMTL_VECTOR_EXPORT(XMD::XString,XMD_EXPORT,XM2);

namespace XMD
{


//----------------------------------------------------------------------------------------------------------------------
XString XBase::GetPropertyName(const XU32 property_id) const
{
  XBaseProperties* props = GetProperties();
  return props->GetPropertyName(property_id);
}

//----------------------------------------------------------------------------------------------------------------------
XPropertyType::Type XBase::GetPropertyType(const XU32 property_id) const
{
  XBaseProperties* props = GetProperties();
  return props->GetPropertyType(property_id);
}

//----------------------------------------------------------------------------------------------------------------------
XFn::Type XBase::GetPropertyNodeType(const XU32 property_id) const
{
  XBaseProperties* props = GetProperties();
  return props->GetPropertyNodeType(property_id);
}

//----------------------------------------------------------------------------------------------------------------------
XU32 XBase::GetPropertyID(const XString& property_name) const
{
  XBaseProperties* props = GetProperties();
  return props->GetPropertyID(property_name);
}

//----------------------------------------------------------------------------------------------------------------------
XU32 XBase::GetNumProperties() const
{
  XBaseProperties* props = GetProperties();
  return props->GetNumProperties();
}

//----------------------------------------------------------------------------------------------------------------------
bool XBase::SetProperty(const XU32 property_id,const XQuaternion& val)
{
  XBaseProperties* props = GetProperties();
  return props->SetProperty(this,property_id,val);
}

//----------------------------------------------------------------------------------------------------------------------
bool XBase::SetProperty(const XU32 property_id,const XVector3& val)
{
  XBaseProperties* props = GetProperties();
  return props->SetProperty(this,property_id,val);
}

//----------------------------------------------------------------------------------------------------------------------
bool XBase::SetProperty(const XU32 property_id,const bool& val)
{
  XBaseProperties* props = GetProperties();
  return props->SetProperty(this,property_id,val);
}

//----------------------------------------------------------------------------------------------------------------------
bool XBase::SetProperty(const XU32 property_id,const XS32& val)
{
  XBaseProperties* props = GetProperties();
  return props->SetProperty(this,property_id,val);
}

//----------------------------------------------------------------------------------------------------------------------
bool XBase::SetProperty(const XU32 property_id,const XU32& val)
{
  XBaseProperties* props = GetProperties();
  return props->SetProperty(this,property_id,val);
}

//----------------------------------------------------------------------------------------------------------------------
bool XBase::SetProperty(const XU32 property_id,const XReal& val)
{
  XBaseProperties* props = GetProperties();
  return props->SetProperty(this,property_id,val);
}

//----------------------------------------------------------------------------------------------------------------------
bool XBase::SetProperty(const XU32 property_id,const XColour& val)
{
  XBaseProperties* props = GetProperties();
  return props->SetProperty(this,property_id,val);
}

//----------------------------------------------------------------------------------------------------------------------
bool XBase::SetProperty(const XU32 property_id,const XString& val)
{
  XBaseProperties* props = GetProperties();
  return props->SetProperty(this,property_id,val);
}

//----------------------------------------------------------------------------------------------------------------------
bool XBase::SetProperty(const XU32 property_id,const XMatrix& val)
{
  XBaseProperties* props = GetProperties();
  return props->SetProperty(this,property_id,val);
}

//----------------------------------------------------------------------------------------------------------------------
bool XBase::SetProperty(const XU32 property_id,const XBase* val)
{
  XBaseProperties* props = GetProperties();
  return props->SetProperty(this,property_id,val);
}

//----------------------------------------------------------------------------------------------------------------------
bool XBase::SetProperty(const XU32 property_id,const XRealArray& val)
{
  XBaseProperties* props = GetProperties();
  return props->SetProperty(this,property_id,val);
}

//----------------------------------------------------------------------------------------------------------------------
bool XBase::SetProperty(const XU32 property_id,const XU32Array& val)
{
  XBaseProperties* props = GetProperties();
  return props->SetProperty(this,property_id,val);
}

//----------------------------------------------------------------------------------------------------------------------
bool XBase::SetProperty(const XU32 property_id,const XVector3Array& val)
{
  XBaseProperties* props = GetProperties();
  return props->SetProperty(this,property_id,val);
}

//----------------------------------------------------------------------------------------------------------------------
bool XBase::SetProperty(const XU32 property_id,const XList& val)
{
  XBaseProperties* props = GetProperties();
  return props->SetProperty(this,property_id,val);
}

//----------------------------------------------------------------------------------------------------------------------
bool XBase::GetProperty(const XU32 property_id,XQuaternion& val) const
{
  XBaseProperties* props = GetProperties();
  return props->GetProperty(m_pModel->FindNode(GetID()),property_id,val);
}

//----------------------------------------------------------------------------------------------------------------------
bool XBase::GetProperty(const XU32 property_id,XVector3& val) const
{
  XBaseProperties* props = GetProperties();
  return props->GetProperty(m_pModel->FindNode(GetID()),property_id,val);
}

//----------------------------------------------------------------------------------------------------------------------
bool XBase::GetProperty(const XU32 property_id,bool& val) const
{
  XBaseProperties* props = GetProperties();
  return props->GetProperty(m_pModel->FindNode(GetID()),property_id,val);
}

//----------------------------------------------------------------------------------------------------------------------
bool XBase::GetProperty(const XU32 property_id,XS32& val) const
{
  XBaseProperties* props = GetProperties();
  return props->GetProperty(m_pModel->FindNode(GetID()),property_id,val);
}

//----------------------------------------------------------------------------------------------------------------------
bool XBase::GetProperty(const XU32 property_id,XU32& val) const
{
  XBaseProperties* props = GetProperties();
  return props->GetProperty(m_pModel->FindNode(GetID()),property_id,val);
}

//----------------------------------------------------------------------------------------------------------------------
bool XBase::GetProperty(const XU32 property_id,XReal& val) const
{
  XBaseProperties* props = GetProperties();
  return props->GetProperty(m_pModel->FindNode(GetID()),property_id,val);
}

//----------------------------------------------------------------------------------------------------------------------
bool XBase::GetProperty(const XU32 property_id,XColour& val) const
{
  XBaseProperties* props = GetProperties();
  return props->GetProperty(m_pModel->FindNode(GetID()),property_id,val);
}

//----------------------------------------------------------------------------------------------------------------------
bool XBase::GetProperty(const XU32 property_id,XString& val) const
{
  XBaseProperties* props = GetProperties();
  return props->GetProperty(m_pModel->FindNode(GetID()),property_id,val);
}

//----------------------------------------------------------------------------------------------------------------------
bool XBase::GetProperty(const XU32 property_id,XMatrix& val) const
{
  XBaseProperties* props = GetProperties();
  return props->GetProperty(m_pModel->FindNode(GetID()),property_id,val);
}

//----------------------------------------------------------------------------------------------------------------------
bool XBase::GetProperty(const XU32 property_id,XBase*& val) const
{
  XBaseProperties* props = GetProperties();
  return props->GetProperty(m_pModel->FindNode(GetID()),property_id,val);
}

//----------------------------------------------------------------------------------------------------------------------
bool XBase::GetProperty(const XU32 property_id,XRealArray& val) const
{
  XBaseProperties* props = GetProperties();
  return props->GetProperty(m_pModel->FindNode(GetID()),property_id,val);
}

//----------------------------------------------------------------------------------------------------------------------
bool XBase::GetProperty(const XU32 property_id,XU32Array& val) const
{
  XBaseProperties* props = GetProperties();
  return props->GetProperty(m_pModel->FindNode(GetID()),property_id,val);
}

//----------------------------------------------------------------------------------------------------------------------
bool XBase::GetProperty(const XU32 property_id,XVector3Array& val) const
{
  XBaseProperties* props = GetProperties();
  return props->GetProperty(m_pModel->FindNode(GetID()),property_id,val);
}

//----------------------------------------------------------------------------------------------------------------------
bool XBase::GetProperty(const XU32 property_id,XList& val) const
{
  XBaseProperties* props = GetProperties();
  return props->GetProperty(m_pModel->FindNode(GetID()),property_id,val);
}

//----------------------------------------------------------------------------------------------------------------------
bool XBase::IsPropertyAnimatable(const XU32 property_id) const
{
  XBaseProperties* props = GetProperties();
  return (props->GetPropertyFlags(property_id) & XPropertyFlags::kAnimatable) != 0;
}

//----------------------------------------------------------------------------------------------------------------------
bool XBase::IsPropertyReadOnly(const XU32 property_id) const
{
  XBaseProperties* props = GetProperties();
  return (props->GetPropertyFlags(property_id) & XPropertyFlags::kReadOnly) != 0;
}

//----------------------------------------------------------------------------------------------------------------------
bool XBase::IsPropertySizeable(const XU32 property_id) const
{
  XBaseProperties* props = GetProperties();
  return (props->GetPropertyFlags(property_id) & XPropertyFlags::kArrayIsResizable) != 0;
}

//----------------------------------------------------------------------------------------------------------------------
const XString XBase::ApiTypeString() const
{
  return XGlobal::ApiTypeStrFromNodeType(ApiType());
}

//----------------------------------------------------------------------------------------------------------------------
XBase* XBase::Clone() const
{
  XBase* copy = m_pModel->CreateNode( ApiType() );
  if(copy)
  {
    // this will generate a unique name for the copy
    m_pModel->RenameNode(copy,m_Name);
    copy->m_pInherited = copy->m_pInherited;

    if(m_pAttributes)
    {
      copy->m_pAttributes = new XExtraAttributes( *m_pAttributes );
    }

    // pass up to the derived copy methods to duplicate the data for 
    // derived types.
    // 
    copy->DoCopy(this);
  }
  return copy;
}

//----------------------------------------------------------------------------------------------------------------------
void XBase::DoCopy(const XBase*)
{
}

//----------------------------------------------------------------------------------------------------------------------
XId XBase::GetID() const 
{
  return m_ThisID;
}

//----------------------------------------------------------------------------------------------------------------------
XModel* XBase::GetModel() 
{
  return m_pModel;
}

//----------------------------------------------------------------------------------------------------------------------
const XModel* XBase::GetModel() const
{
  return m_pModel;
}

//----------------------------------------------------------------------------------------------------------------------
XString XBase::GetTypeString() 
{
  if (m_pInherited)
  {
    return m_pInherited->GetTypeName();
  }
  return XGlobal::ApiTypeStrFromNodeType( this->GetApiType() );
}

//----------------------------------------------------------------------------------------------------------------------
void XBase::SetName(const XChar* c)
{
  GetModel()->RenameNode(this,c);
}

//----------------------------------------------------------------------------------------------------------------------
bool XBase::DeleteUserData(const XString& name)
{
  DUMPER(XBase__DeleteUserData);
  XUserDataArray::iterator it = m_UserData.begin();
  for( ; it != m_UserData.end(); ++it ) 
  {
    if( (*it)->GetIdentifier() == name ) 
    {
      (*it)->Release();
      m_UserData.erase(it);
      break;
    }
  }
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
XUserData* XBase::GetUserData(const XString& name) 
{
  DUMPER(XBase__GetUserData);
  XUserDataArray::iterator it = m_UserData.begin();
  for( ; it != m_UserData.end(); ++it ) 
  {
    if( (*it)->GetIdentifier() == name ) 
    {
      return *it;
    }
  }
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
const XUserData* XBase::GetUserData(const XString& name) const
{
  DUMPER(XBase__GetUserData);
  XUserDataArray::const_iterator it = m_UserData.begin();
  for( ; it != m_UserData.end(); ++it ) 
  {
    if( (*it)->GetIdentifier() == name ) 
    {
      return *it;
    }
  }
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
bool XBase::IsValidObj(XId id) 
{
  // if you assert here, something disastrous has happened.
  // Any XBase derived class should always be created with an
  // assigned XModel.
  XMD_ASSERT( GetModel() );
  return GetModel()->IsValidNode(id); 
}

//----------------------------------------------------------------------------------------------------------------------
bool XBase::IsValidObj(const XBase* ptr) 
{
  // if you assert here, something disastrous has happened.
  // Any XBase derived class should always be created with an
  // assigned XModel.
  XMD_ASSERT( GetModel() );
  return GetModel()->IsValidNode(ptr); 
}

//----------------------------------------------------------------------------------------------------------------------
const bool XBase::IsValidObj(XId id) const 
{
  // if you assert here, something disastrous has happened.
  // Any XBase derived class should always be created with an
  // assigned XModel.
  XMD_ASSERT( GetModel() );
  return GetModel()->IsValidNode(id); 
}

//----------------------------------------------------------------------------------------------------------------------
const bool XBase::IsValidObj(const XBase* ptr) const 
{
  // if you assert here, something disastrous has happened.
  // Any XBase derived class should always be created with an
  // assigned XModel.
  XMD_ASSERT( GetModel() );
  return GetModel()->IsValidNode(ptr); 
}

//----------------------------------------------------------------------------------------------------------------------
bool XBase::AddUserData(XUserData* ud)
{
  DUMPER(XBase__AddUserData);
  if(!ud)
    return false;
  if( GetUserData(ud->GetIdentifier()) ) 
  {
    XGlobal::GetLogger()->Logf(XBasicLogger::kError,
      "User data of type %s already added to node %s",
      ud->GetIdentifier().c_str(), GetName());
    return false;
  }
  m_UserData.push_back(ud);
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool XBase::RemUserData(XUserData* ud) 
{
  DUMPER(XBase__RemUserData);
  XUserDataArray::iterator it = m_UserData.begin();
  for( ; it != m_UserData.end(); ++it ) 
  {
    if( *it == ud ) 
    {
      m_UserData.erase(it);
      return true;
    }
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
XId PosInList(XIndexList& ids,XId orig) 
{
  XIndexList::iterator it = ids.begin();
  for( XId i=1 ; it != ids.end(); ++it,++i ) 
  {
    if( orig == *it )
      return i;
  }
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
XBase::XBase(XModel* pModel) 
  : XFn(),m_ThisID(0),m_Name(),
    m_pAttributes(0),m_pModel(pModel),m_pInherited(0),m_UserData()
{
  // If you assert here something terrible has happened. Any XBase 
  // derived class must be created with a valid XModel. Try to determine
  // why this has happened - ie, where it has been created from,
  // since only a valid XModel should be able to create an XBase derived
  // class.
  XMD_ASSERT(m_pModel);
}

//----------------------------------------------------------------------------------------------------------------------
XBase::~XBase() 
{
  delete m_pAttributes;

  // copy user data array so that auto kill mechanism doesn't work
  XUserDataArray acopy = m_UserData;
  
  // delete all user data on this node
  XUserDataArray::iterator it = acopy.begin();
  for( ; it != acopy.end(); ++it ) 
  {
    (*it)->Release();
  }

  // should have removed all user data from the XBase
  XMD_ASSERT(m_UserData.empty());
}

//----------------------------------------------------------------------------------------------------------------------
XFn::Type XBase::GetApiType() const 
{
  return XFn::Base;
}

//----------------------------------------------------------------------------------------------------------------------
XFn::Type XBase::ApiType() const 
{
  return GetApiType();
}

//----------------------------------------------------------------------------------------------------------------------
bool XBase::DoData(XFileIO& io)
{
  DUMPER(XBase);
  CHECK_IO( io.DoDataString(m_Name) );
  DPARAM( m_Name );

  XModel::FormatName(m_Name);

  CHECK_IO( io.DoData(&m_ThisID) );
  DPARAM( m_ThisID );
  if(io.IsWriting())
  {
    if(m_pAttributes && GetModel()->GetExportPolicy().IsEnabled(XExportPolicy::WriteAttributes)) 
    {
      CHECK_IO( m_pAttributes->DoData(io,GetModel()) );
    }
  }
  else
  {
    if(m_pAttributes) 
    {
      CHECK_IO( m_pAttributes->DoData(io,GetModel()) );
    }
  }
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
XU32 XBase::GetDataSize() const 
{
  return (XU32)(sizeof(XChunkHeader) + sizeof(XId) +
       (m_pAttributes?m_pAttributes->GetDataSize():0) +
       sizeof(XU16) +
       m_Name.size());
}

//----------------------------------------------------------------------------------------------------------------------
bool XBase::DoHeader(XFileIO& io) 
{
  DUMPER(DoHeader);
  XChunkHeader header;
  memset(&header,0,sizeof(XChunkHeader));
  if(io.IsWriting())
  {
    if(m_pInherited)
      header.Type = (XU16) m_pInherited->CustomTypeID();
    else
      header.Type = (XU16) this->GetApiType();

    if(!GetModel()->GetExportPolicy().IsEnabled(XExportPolicy::WriteAttributes)) 
      header.HasExtraAttrs=0;
    else
      header.HasExtraAttrs = (m_pAttributes) ? 1 : 0;

    header.Reserved = 0;
    header.ByteSize = this->GetDataSize();
  }
  DPARAM( header.Type );
  DPARAM( (XS32)header.HasExtraAttrs );
  DPARAM( header.ByteSize );
  CHECK_IO( io.DoData(&header) );

  if(header.HasExtraAttrs && io.IsReading())
    m_pAttributes = new XExtraAttributes;

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool XBase::DoReadChunk(std::istream& ifs) 
{
  DUMPER(XBase__DoReadChunk);
  if(!ReadHeader(ifs))
    return false;
  if(!ReadChunk(ifs))
    return false;
  return ReadFooter(ifs);
}

//----------------------------------------------------------------------------------------------------------------------
bool XBase::DoWriteChunk(std::ostream& ofs)
{
  DUMPER(XBase__DoWriteChunk);
  if(!WriteHeader(ofs))
    return false;
  if(!WriteChunk(ofs))
    return false;
  return WriteFooter(ofs);
}

//----------------------------------------------------------------------------------------------------------------------
XAttribute* XBase::CreateAttribute(const XChar* name)
{
  if (GetAttribute(name)) 
  {
    return 0;
  }
  if (!m_pAttributes) 
  {
    m_pAttributes = new XExtraAttributes();
  }
  XAttribute* attr = new XAttribute();
  attr->SetName(name);
  m_pAttributes->push_back(attr);
  return attr;
}

//----------------------------------------------------------------------------------------------------------------------
bool XBase::DeleteAttrribute(const XChar* attr_name)
{
  if (m_pAttributes)
  {
    XString temp(attr_name);
    XExtraAttributes::iterator it = m_pAttributes->begin();
    for (;it != m_pAttributes->end();++it)
    {
      if(temp == (*it)->Name())
      {
        delete *it;
        m_pAttributes->erase( it );
        if (m_pAttributes->size()==0)
        {
          delete m_pAttributes;
          m_pAttributes=0;
        }
        return true;
      }
    }
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool XBase::DeleteAttrribute(const XAttribute* attr)
{
  if (m_pAttributes && attr)
  {
    XExtraAttributes::iterator it = m_pAttributes->begin();
    for (;it != m_pAttributes->end();++it)
    {
      if(attr == (*it))
      {
        delete *it;
        m_pAttributes->erase( it );
        if (m_pAttributes->size()==0)
        {
          delete m_pAttributes;
          m_pAttributes=0;
        }
        return true;
      }
    }
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool XBase::AddAttribute(XAttribute* attr)
{
  if (GetAttribute(attr->Name())) 
  {
    return false;
  }
  if (!m_pAttributes) 
  {
    m_pAttributes = new XExtraAttributes();
  }
  XMD_ASSERT(m_pAttributes);
  m_pAttributes->push_back(attr);
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
XAttribute* XBase::GetAttribute(const XChar* name)
{
  if(!m_pAttributes)
    return 0;
  return m_pAttributes->GetAttr(name);
}

//----------------------------------------------------------------------------------------------------------------------
const XAttribute* XBase::GetAttribute(const XChar* name) const 
{
  if(!m_pAttributes)
    return 0;
  return m_pAttributes->GetAttr(name);
}

//----------------------------------------------------------------------------------------------------------------------
const XChar* XBase::GetName() const 
{
  return m_Name.c_str();
}

//----------------------------------------------------------------------------------------------------------------------
XBase* XBase::GetFn(XFn::Type type) 
{
  if(type == XFn::Base)
    return static_cast<XBase*>(this);
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
const XBase* XBase::GetFn(XFn::Type type) const
{
  if(XFn::Base==type)
    return (const XBase*)this;
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
XBase* XBase::HasFn(XFn::Type type) 
{
  return this->GetFn(type);
}

//----------------------------------------------------------------------------------------------------------------------
void XBase::ListAttributes(XStringList& attrs) const 
{
  DUMPER(XBase__ListAttributes);
  attrs.clear();
  if(m_pAttributes)
    m_pAttributes->ListAttr(attrs);
}

//----------------------------------------------------------------------------------------------------------------------
void XBase::PreDelete(XIdSet&)
{
}

//----------------------------------------------------------------------------------------------------------------------
bool XBase::NodeDeath(XId id)
{
  DUMPER(XBase__NodeDeath);
  // just for deleting attribute connections
  if (m_pAttributes) 
  {
    m_pAttributes->NodeDeath(GetModel(),id);
  }
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
void XBase::PostLoad() 
{
}

//----------------------------------------------------------------------------------------------------------------------
bool XBase::ReadChunk(std::istream& ifs) 
{
  DUMPER(XBase__ReadChunk);
  return ifs.good();
}

//----------------------------------------------------------------------------------------------------------------------
bool XBase::ReadFooter(std::istream& ifs)
{
  DUMPER(XBase__ReadFooter);
  XString buffer;
  ifs >> buffer;
  if (ifs.fail())
    return false;
  if (buffer[0] == '}' && buffer.size() == 1) 
  {
    return ifs.good();
  }
  checkAgain:
  if (buffer=="EXTRA_ATTRIBUTES") 
  {
    m_pAttributes = new XExtraAttributes;
    ifs >> *m_pAttributes;
  }
  else
  if (buffer=="CONNECTIONS") 
  {
    if(m_pAttributes)
    {
      ReadConnections(ifs);
    }
  }

  ifs >> buffer;

  if (ifs.fail())
  {
    XGlobal::GetLogger()->Logf(XBasicLogger::kError,"corrupt file");
    return false;
  }

  if (buffer[0] != '}' && !ifs.eof()) 
  {
    goto checkAgain;
  }

  if (ifs.eof())
  {
    XGlobal::GetLogger()->Logf(XBasicLogger::kError,"Unexpected end of file");
    return false;
  }

  return ifs.good();
}

//----------------------------------------------------------------------------------------------------------------------
bool XBase::ReadHeader(std::istream& ifs) 
{
  DUMPER(XBase__ReadHeader);
  XString buffer;
  ifs >> buffer;
  if (buffer[0]!='{') 
  {
    XGlobal::GetLogger()->Logf(XBasicLogger::kError,"File IO error, malformed chunk header in chunk %s",buffer.c_str());
    return false;
  }
  ifs >> buffer;
  if ( (buffer[0]!='i') || (buffer[1] != 'd') ) 
  {
    /// \todo add error reporting
    return false;
  }
  ifs >> m_ThisID;
  return ifs.good();
}

//----------------------------------------------------------------------------------------------------------------------
bool XBase::WriteChunk(std::ostream& ofs) 
{
  DUMPER(XBase__WriteChunk);
  (void)ofs;
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool XBase::ReadConnections(std::istream& ifs)
{
  DUMPER(XBase__ReadConnections);
  XString buffer;
  READ_CHECK("{",ifs);
  bool done=false;
  while (!done) 
  {
    ifs >> buffer;
    if (buffer=="}") 
    {
      break;
    }
    if (buffer=="ATTR") 
    {
      XString attr_name;
      XU32 idx;
      ifs >> attr_name >> idx;
      READ_CHECK("{",ifs);
      READ_CHECK("input",ifs);
      XU32 nidx=0;
      XString attr;
      ifs >> nidx;
      if (nidx) 
      {
        ifs >> attr;
      }
      READ_CHECK("output",ifs);
      XIntArray conn_ids;
      XStringList conn_names;
      XU32 nOutputs;
      ifs >> nOutputs;
      conn_ids.resize(nOutputs);
      conn_names.resize(nOutputs);
      for (XU32 ii=0;ii!=conn_names.size();++ii) 
      {
        ifs >> conn_ids[ii] >> conn_names[ii];
      }
      READ_CHECK("}",ifs);

      XAttribute* pattr = GetAttribute(attr_name.c_str());
      if (pattr) 
      {
        if(nidx)
        {
          pattr->SetInputConnection(nidx,idx,attr.c_str());
        }

        for (XU32 ii=0;ii!=conn_names.size();++ii) 
        {
          pattr->SetOutputConnection(conn_ids[ii],true,idx,conn_names[ii].c_str());
        }
      }
    }
    else
    {
      return false;
    }
  }
  return true;    
}

//----------------------------------------------------------------------------------------------------------------------
bool XBase::WriteConnections(std::ostream& ofs)
{
  DUMPER(XBase__WriteConnections);
  if(!m_pAttributes)
    return true;

  // flag to determine if we have to write any connections
  bool has_connections = false;

  // check for connections on the node's attrs
  XExtraAttributes::iterator it = m_pAttributes->begin();
  for (; it != m_pAttributes->end() && !has_connections; ++it) 
  {
    XAttribute* item = *it;
    if(item->IsArray())
    {
      for(XU32 ii=0;ii!=item->NumElements();++ii)
      {
        if( item->HasInputConnection(ii) ||
            item->HasOutputConnection(ii) )
        {
          has_connections = true;
          break;
        }
      }
    }
    else
    {
      if( item->HasInputConnection() ||
          item->HasOutputConnection() )
      {
        has_connections = true;
      }
    }
  }
  if(!has_connections)
  {
    return true;
  }

  // found a connection(s), lets write them
  ofs << "\tCONNECTIONS\n";
  ofs << "\t{\n";

  it = m_pAttributes->begin();
  for (; it != m_pAttributes->end(); ++it) 
  {
    XAttribute* item = *it;
    if(item->IsArray())
    {
      for(XU32 kk=0;kk!=item->NumElements();++kk)
      {
        if( item->HasInputConnection(kk) ||
            item->HasOutputConnection(kk) )
        {
          ofs << "\t\tATTR " << item->Name() << " " << kk << "\n";
          ofs << "\t\t{\n";

          ofs << "\t\t\tinput ";
          XBase* b = item->GetInputConnectionNode(GetModel(),kk);
          if( b )
          {
            ofs << b->GetID() << " " << item->GetInputConnectionName(GetModel(),kk) << "\n"; 
          }
          else
            ofs << "0\n";

          ofs << "\t\t\toutput " << item->GetNumOutputConnections(kk) << "\n";
          for (XU32 jj=0; jj!=item->GetNumOutputConnections(kk);++jj)
          {
            XBase* b = item->GetOutputConnectionNode(GetModel(),kk,jj);
            if( b )
            {
              ofs << "\t\t\t\t" << b->GetID() << " " << item->GetOutputConnectionName(GetModel(),kk,jj) << "\n"; 
            }
            else
              ofs << "\t\t\t\t0 " << item->GetOutputConnectionName(GetModel(),kk,jj) << "\n"; ;
          }

          ofs << "\t\t}\n";
        }
      }
    }
    else
    {
      if( item->HasInputConnection() ||
          item->HasOutputConnection() )
      {
        ofs << "\t\tATTR " << item->Name() << " 0\n";
        ofs << "\t\t{\n\t\t\tinput ";
        XBase* b = item->GetInputConnectionNode(GetModel(),0);
        if( b )
        {
          ofs << b->GetID() << " " << item->GetInputConnectionName(GetModel(),0) << "\n"; 
        }
        else
          ofs << "0\n";

        ofs << "\t\t\toutput " << item->GetNumOutputConnections() << "\n";
        for (XU32 jj=0; jj!=item->GetNumOutputConnections();++jj)
        {
          XBase* b = item->GetOutputConnectionNode(GetModel(),0,jj);
          if( b )
          {
            ofs << "\t\t\t\t" << b->GetID() << " " << item->GetOutputConnectionName(GetModel(),jj) << "\n"; 
          }
          else
            ofs << "\t\t\t\t0 " << item->GetOutputConnectionName(GetModel(),jj) << "\n"; ;
        }

        ofs << "\t\t}\n";
      }
    }
  }
  ofs << "\t}\n";

  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool XBase::WriteFooter(std::ostream& ofs) 
{
  DUMPER(XBase__WriteFooter);

  if(GetModel()->GetExportPolicy().IsEnabled(XExportPolicy::WriteAttributes))
  {
    if (m_pAttributes) 
    {
      ofs << *m_pAttributes;
      WriteConnections(ofs);
    }
  }
  ofs << "}\n";
  return ofs.good();
}

//----------------------------------------------------------------------------------------------------------------------
template<typename T>
bool OutputAsExtended(XBase* node,std::ostream& ofs)
{
  T* ptr = node->HasFn<T>();
  if (ptr) 
  {
    const PxNewNodeDescription* info = ptr->GetTypeInfo();
    XMD_ASSERT(info);
    ofs << info->GetChunkName();
    return true;
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool XBase::WriteHeader(std::ostream& ofs) 
{
  DUMPER(XBase__WriteHeader);
  if(!GetName() || (GetID()==0) )
    return false;

  if(m_pInherited) 
  {
    ofs << m_pInherited->GetHeader();
  }
  else 
  if( OutputAsExtended<XMD::PxNode>(this,ofs) ||
      OutputAsExtended<XMD::PxShapeNode>(this,ofs) ||
      OutputAsExtended<XMD::PxDeformerNode>(this,ofs) ||
      OutputAsExtended<XMD::PxMaterialNode>(this,ofs) ||
      OutputAsExtended<XMD::PxTransformNode>(this,ofs) ||
      OutputAsExtended<XMD::PxHwShaderNode>(this,ofs) ||
      OutputAsExtended<XMD::PxEmitterNode>(this,ofs) ||
      OutputAsExtended<XMD::PxFieldNode>(this,ofs) )
  {
  }
  else
  {
    ofs << XGlobal::ChunkNameFromNodeType(GetApiType()) ;
  }
  ofs << " "
      << GetName() << "\n{\n\tid "
      << GetID() << std::endl;
  return ofs.good();
}
}
