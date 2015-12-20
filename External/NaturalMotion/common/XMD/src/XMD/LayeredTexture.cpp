//----------------------------------------------------------------------------------------------------------------------
/// \file LayeredTexture.cpp
/// \note (C) Copyright 2003-2005 Robert Bateman. All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#include "XMD/LayeredTexture.h"
#include "XMD/Model.h"
#include "XMD/FileIO.h"

namespace XMD
{
//----------------------------------------------------------------------------------------------------------------------
XLayeredTexture::XLayeredTexture(XModel* m)
  : XTexture(m),m_BlendModes(),m_TextureInputs()
{
}

//----------------------------------------------------------------------------------------------------------------------
XLayeredTexture::~XLayeredTexture()
{
  for(size_t i = 0; i != m_TextureInputs.size() ; ++i)
  {
    delete m_TextureInputs[i];
  }
}

//----------------------------------------------------------------------------------------------------------------------
void XLayeredTexture::PreDelete(XIdSet& extra_nodes)
{
  XTexture::PreDelete(extra_nodes);
}

//----------------------------------------------------------------------------------------------------------------------
bool XLayeredTexture::NodeDeath(XId id) 
{
  return XTexture::NodeDeath(id);
}

//----------------------------------------------------------------------------------------------------------------------
XLayeredTexture::XBlendMode XLayeredTexture::GetBlendMode(const XU32 layer_index) const
{
  return (layer_index<m_BlendModes.size()) ? m_BlendModes[layer_index] : kBlendNone;
}

//----------------------------------------------------------------------------------------------------------------------
bool XLayeredTexture::SetBlendMode(const XU32 layer_index,const XLayeredTexture::XBlendMode mode)
{
  if(layer_index<m_BlendModes.size())
  {
    m_BlendModes[layer_index]=mode;
    return true;
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
void XLayeredTexture::SetNumLayers(const XU32 num_layers)
{
  XU32 curr_size=static_cast<XU32>( m_TextureInputs.size() );
  if(num_layers>curr_size)
  {
    XU32 to_add = num_layers - curr_size;
    for(XU32 i = 0; i != to_add ; ++i)
    {
      m_TextureInputs.push_back( new XColourConnection("texture") );
      m_Connections.push_back(m_TextureInputs.back());
      m_BlendModes.push_back( kBlendNone );
    }
  }
  else 
  if(num_layers<curr_size)
  {
    for (XU32 i=num_layers;i!=curr_size;++i)
    {
      XColourConnection* conn = m_TextureInputs[i];

      XConnectionArray::iterator it = m_Connections.begin();
      for (;it != m_Connections.end();++it)
      {
        if (*it == conn)
        {
          m_Connections.erase( it );
          break;
        }
      }

      delete conn;
    }

    m_BlendModes.resize( num_layers );
    m_TextureInputs.resize( num_layers );
  }
}

//----------------------------------------------------------------------------------------------------------------------
XU32 XLayeredTexture::GetNumLayers() const
{
  return static_cast<XU32>( m_TextureInputs.size() );
}

//----------------------------------------------------------------------------------------------------------------------
bool XLayeredTexture::AddLayer(const XTexture* texture)
{
  if (texture && IsValidObj(texture))
  {
    m_TextureInputs.push_back( new XColourConnection("texture",texture->GetID()) );
    m_Connections.push_back(m_TextureInputs.back());
    m_BlendModes.push_back(kBlendNone);
    return true;
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool XLayeredTexture::AddLayer(const XTexture* texture,const XColour& colour)
{
  if (texture && IsValidObj(texture))
  {
    m_TextureInputs.push_back( new XColourConnection("texture",texture->GetID(),colour) );
    m_Connections.push_back(m_TextureInputs.back());
    m_BlendModes.push_back(kBlendNone);
    return true;
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool XLayeredTexture::AddLayer(const XTexture* texture,const XColour& colour, XBlendMode mode)
{
  if (texture && IsValidObj(texture))
  {
    m_TextureInputs.push_back( new XColourConnection("texture",texture->GetID(),colour) );
    m_Connections.push_back(m_TextureInputs.back());
    m_BlendModes.push_back(mode);
    return true;
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool XLayeredTexture::AddLayer(const XTexPlacement2D* texture)
{
  if (texture && IsValidObj(texture))
  {
    m_TextureInputs.push_back( new XColourConnection("texture",texture->GetID()) );
    m_Connections.push_back(m_TextureInputs.back());
    m_BlendModes.push_back(kBlendNone);
    return true;
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool XLayeredTexture::AddLayer(const XTexPlacement2D* texture,const XColour& colour)
{
  if (texture && IsValidObj(texture))
  {
    m_TextureInputs.push_back( new XColourConnection("texture",texture->GetID(),colour) );
    m_Connections.push_back(m_TextureInputs.back());
    m_BlendModes.push_back(kBlendNone);
    return true;
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool XLayeredTexture::AddLayer(const XTexPlacement2D* texture,const XColour& colour,const XBlendMode mode)
{
  if (texture && IsValidObj(texture))
  {
    m_TextureInputs.push_back( new XColourConnection("texture",texture->GetID(),colour) );
    m_Connections.push_back(m_TextureInputs.back());
    m_BlendModes.push_back(mode);
    return true;
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool XLayeredTexture::RemoveLayer(const XU32 layer_index)
{
  if (layer_index<GetNumLayers())
  {
    XConnectionArray::iterator it = m_Connections.begin();
    for (;it != m_Connections.end();++it)
    {
      if (*it == m_TextureInputs[layer_index])
      {
        m_Connections.erase( it );
        break;
      }
    }
    delete m_TextureInputs[layer_index];
    m_TextureInputs.erase( m_TextureInputs.begin() + layer_index );
    m_BlendModes.erase( m_BlendModes.begin() + layer_index );
    return true;
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool XLayeredTexture::SetLayer(const XU32 layer_index,
                               const XTexture* texture)
{
  if (layer_index<GetNumLayers())
  {
    if (texture && IsValidObj(texture))
    {
      m_TextureInputs[layer_index]->m_InputNode = texture->GetID();
      return true;
    }
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool XLayeredTexture::SetLayer(const XU32 layer_index,
                               const XTexture* texture,
                               const XColour& colour)
{
  if (layer_index<GetNumLayers())
  {
    if (texture && IsValidObj(texture))
    {
      m_TextureInputs[layer_index]->m_InputNode = texture->GetID();
      m_TextureInputs[layer_index]->m_Colour = colour;
      return true;
    }
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool XLayeredTexture::SetLayer(const XU32 layer_index,
                               const XTexture* texture,
                               const XColour& colour,
                               const XBlendMode mode)
{
  if (layer_index<GetNumLayers())
  {
    if (texture && IsValidObj(texture))
    {
      m_TextureInputs[layer_index]->m_InputNode = texture->GetID();
      m_TextureInputs[layer_index]->m_Colour = colour;
      m_BlendModes[layer_index] = mode;
      return true;
    }
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool XLayeredTexture::SetLayer(const XU32 layer_index,
                               const XTexPlacement2D* texture)
{
  if (layer_index<GetNumLayers())
  {
    if (texture && IsValidObj(texture))
    {
      m_TextureInputs[layer_index]->m_InputNode = texture->GetID();
      return true;
    }
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool XLayeredTexture::SetLayer(const XU32 layer_index,
                              const XTexPlacement2D* texture,
                              const XColour& colour)
{
  if (layer_index<GetNumLayers())
  {
    if (texture && IsValidObj(texture))
    {
      m_TextureInputs[layer_index]->m_InputNode = texture->GetID();
      m_TextureInputs[layer_index]->m_Colour = colour;
      return true;
    }
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool XLayeredTexture::SetLayer(const XU32 layer_index,
                               const XTexPlacement2D* texture,
                               const XColour& colour,
                               const XBlendMode mode)
{
  if (layer_index<GetNumLayers())
  {
    if (texture && IsValidObj(texture))
    {
      m_TextureInputs[layer_index]->m_InputNode = texture->GetID();
      m_TextureInputs[layer_index]->m_Colour = colour;
      m_BlendModes[layer_index] = mode;
      return true;
    }
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
XColour XLayeredTexture::GetLayerColour(const XU32 layer_index) const
{
  if (layer_index<m_TextureInputs.size())
  {
    return m_TextureInputs[layer_index]->m_Colour;
  }
  return XColour();
}

//----------------------------------------------------------------------------------------------------------------------
XShadingNode* XLayeredTexture::GetLayer(const XU32 layer_index) const
{
  if (layer_index<m_TextureInputs.size())
  {
    XBase* b = m_pModel->FindNode(m_TextureInputs[layer_index]->m_InputNode);
    if (b)
    {
      return b->HasFn<XShadingNode>();
    }
  }
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
XTexture* XLayeredTexture::GetLayerTexture(const XU32 layer_index) const
{
  if (layer_index<m_TextureInputs.size())
  {
    XBase* b = m_pModel->FindNode(m_TextureInputs[layer_index]->m_InputNode);
    if (b)
    {
      return b->HasFn<XTexture>();
    }
  }
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
XTexPlacement2D* XLayeredTexture::GetLayerTexturePlacement(const XU32 layer_index) const
{
  /// \todo CHANGE to properly use XTexture
  if (layer_index<m_TextureInputs.size())
  {
    XBase* b = m_pModel->FindNode(m_TextureInputs[layer_index]->m_InputNode);
    if (b)
    {
      return b->HasFn<XTexPlacement2D>();
    }
  }
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
XFn::Type XLayeredTexture::GetApiType() const
{
  return XFn::LayeredTexture;
}

//----------------------------------------------------------------------------------------------------------------------
XBase* XLayeredTexture::GetFn(XFn::Type type)
{
  if (type == XFn::LayeredTexture)
  {
    return (XLayeredTexture*)this;
  }
  return XTexture::GetFn(type);
}

//----------------------------------------------------------------------------------------------------------------------
const XBase* XLayeredTexture::GetFn(XFn::Type type) const
{
  if (type == XFn::LayeredTexture)
  {
    return (const XLayeredTexture*)this;
  }
  return XTexture::GetFn(type);
}

//----------------------------------------------------------------------------------------------------------------------
void XLayeredTexture::DoCopy(const XBase* ptr)
{
  XTexture::DoCopy(ptr);
  const XLayeredTexture* cb = ptr->HasFn<XLayeredTexture>();
  if (cb)
  {
    /// \todo   remove this assert and check it properly
    XMD_ASSERT(0); // I think this is going to break, so just make sure we don't do it at the moment.
    m_BlendModes = cb->m_BlendModes;
    m_TextureInputs = cb->m_TextureInputs;
  }
}

//----------------------------------------------------------------------------------------------------------------------
XU32 XLayeredTexture::GetDataSize() const
{
  XU32 sz = XTexture::GetDataSize();
  sz += sizeof(XU32);
  sz += sizeof(XU32);
  sz += (XU32)m_BlendModes.size()*sizeof(XBlendMode);
  for (XU32 i=0;i!=m_TextureInputs.size();++i)
    sz += m_TextureInputs[i]->GetDataSize();
  return sz;
}

//----------------------------------------------------------------------------------------------------------------------
bool XLayeredTexture::DoData(XFileIO& io) 
{
  if(!XTexture::DoData(io))
    return false;

  IO_CHECK( io.DoDataVector(m_BlendModes) );

  XU32 num_inputs = (XU32)m_TextureInputs.size();
  IO_CHECK( io.DoData(&num_inputs) );

  if(io.IsReading()) 
  {
    for (XU32 i=0;i!=num_inputs;++i) 
    {
      m_TextureInputs.push_back( new XColourConnection("texture") );
      m_Connections.push_back( m_TextureInputs.back() );
    }
  }

  for (XU32 i=0;i!=num_inputs;++i) 
  {
    IO_CHECK( m_TextureInputs[i]->DoData(io) );
  }
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool XLayeredTexture::ReadChunk(std::istream& ifs)
{
  IO_CHECK( XTexture::ReadChunk(ifs) );

  XString temp;

  READ_CHECK("inputs",ifs);
  XU32 num_inputs;
  ifs >> num_inputs;

  for (XU32 i=0;i!=num_inputs;++i)
  {
    XColourConnection* conn = new XColourConnection("texture");
    if(!conn->ReadChunk(ifs))
    {
      delete conn;
      return false;
    }
    
    XBlendMode bm;
    READ_CHECK("mode",ifs);
    ifs >> temp;
    if (temp == "none") bm = kBlendNone;
    else
    if (temp == "over") bm = kBlendOver;
    else
    if (temp == "in") bm = kBlendIn;
    else
    if (temp == "out") bm = kBlendOut;
    else
    if (temp == "add") bm = kBlendAdd;
    else
    if (temp == "subtract") bm = kBlendSubtract;
    else
    if (temp == "multiply") bm = kBlendMultiply;
    else
    if (temp == "difference") bm = kBlendDifference;
    else
    if (temp == "lighten") bm = kBlendLighten;
    else
    if (temp == "darken") bm = kBlendDarken;
    else
    if (temp == "saturate") bm = kBlendSaturate;
    else
    if (temp == "desaturate") bm = kBlendDeSaturate;
    else
    if (temp == "illuminate") bm = kBlendIlluminate;
    else bm = kBlendNone;

    m_TextureInputs.push_back( conn );
    m_Connections.push_back( conn );
    m_BlendModes.push_back( bm );
  }

  return ifs.good();
}

//----------------------------------------------------------------------------------------------------------------------
bool XLayeredTexture::WriteChunk(std::ostream& ofs)
{
  IO_CHECK( XTexture::WriteChunk(ofs) );


  XU32 num = static_cast<XU32>( m_TextureInputs.size() );
  ofs << "inputs " << num << std::endl;

  --num;
  XConnectionArray::const_iterator colour = m_TextureInputs.begin();
  XBlendModeArray::const_iterator bm = m_BlendModes.begin();
  for (; colour != m_TextureInputs.end(); ++colour,++bm)
  {
    ofs << "\t\t"
        << (*colour)->m_Name << " "
        << (*colour)->m_Colour
        << " input " << (*colour)->m_InputNode 
        << " mode ";

    switch(*bm)
    {
    case kBlendNone: ofs << "none\n"; break;
    case kBlendOver: ofs << "over\n"; break;
    case kBlendIn: ofs << "in\n"; break;
    case kBlendOut: ofs << "out\n"; break;
    case kBlendAdd: ofs << "add\n"; break;
    case kBlendSubtract: ofs << "subtract\n"; break;
    case kBlendMultiply: ofs << "multiply\n"; break;
    case kBlendDifference: ofs << "difference\n"; break;
    case kBlendLighten: ofs << "lighten\n"; break;
    case kBlendDarken: ofs << "darken\n"; break;
    case kBlendSaturate: ofs << "saturate\n"; break;
    case kBlendDeSaturate: ofs << "desaturate\n"; break;
    case kBlendIlluminate: ofs << "illuminate\n"; break;
    default: break;
    }
  }
  return ofs.good();
}
}
