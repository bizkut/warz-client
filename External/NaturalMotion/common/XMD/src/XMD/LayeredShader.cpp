//----------------------------------------------------------------------------------------------------------------------
/// \file Lambert.cpp
/// \note (C) Copyright 2003-2005 Robert Bateman. All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#include "XMD/LayeredShader.h"
#include "XMD/Model.h"
#include "XMD/FileIO.h"

namespace XMD
{
//----------------------------------------------------------------------------------------------------------------------
XLayeredShader::XLayeredShader(XModel* pmod)
  : XSurfaceShader(pmod),m_ColourInputs(),m_TransparencyInputs(),
  m_GlowInputs(),m_CompositingType(0)
{
}

//----------------------------------------------------------------------------------------------------------------------
XLayeredShader::~XLayeredShader()
{
  XShadingNode::XConnectionArray::iterator colour = m_ColourInputs.begin();
  XShadingNode::XConnectionArray::iterator transparency = m_TransparencyInputs.begin();
  XShadingNode::XConnectionArray::iterator glow = m_GlowInputs.begin();
  for( ; colour != m_ColourInputs.end(); ++colour, ++transparency, ++glow )
  {
    delete *colour;
    delete *transparency;
    delete *glow;
  }
}

//----------------------------------------------------------------------------------------------------------------------
void XLayeredShader::PreDelete(XIdSet& extra_nodes)
{
  XSurfaceShader::PreDelete(extra_nodes);
}

//----------------------------------------------------------------------------------------------------------------------
bool XLayeredShader::NodeDeath(XId id) 
{
  return XSurfaceShader::NodeDeath(id);
}

//----------------------------------------------------------------------------------------------------------------------
void XLayeredShader::SetNumLayers(XU32 num_layers)
{
  XU32 curr_size=static_cast<XU32>( m_ColourInputs.size() );
  if (num_layers>curr_size)
  {
    XU32 to_add = num_layers - curr_size;
    for(XU32 i = 0; i != to_add ; ++i)
    {
      m_ColourInputs.push_back( new XColourConnection("colour") );
      m_Connections.push_back(m_ColourInputs.back());

      m_TransparencyInputs.push_back( new XColourConnection("transparency") );
      m_Connections.push_back(m_TransparencyInputs.back());

      m_GlowInputs.push_back( new XColourConnection("glow") );
      m_Connections.push_back(m_GlowInputs.back());
    }
  }
  else if (num_layers<curr_size)
  {
    XU32 to_remove = curr_size - num_layers;
    for(XU32 i = 0; i != to_remove ; ++i)
    {
      XU32 done = 0;
      for(XU32 j = 0; j != static_cast<XU32>( m_Connections.size() ); )
      {
        bool increment = true;
        if(!(done&0x1)&&m_ColourInputs[num_layers+i]==m_Connections[j])
        {
          done|=0x1;
          m_Connections.erase(m_Connections.begin() + j);
          delete m_ColourInputs[num_layers+i];
          m_ColourInputs[num_layers+i]=0;
          increment = false;
        }
        if(!(done&0x2)&&m_TransparencyInputs[num_layers+i]==m_Connections[j])
        {
          done|=0x2;
          m_Connections.erase(m_Connections.begin() + j);
          delete m_TransparencyInputs[num_layers+i];
          m_TransparencyInputs[num_layers+i]=0;
          increment = false;
        }
        if(!(done&0x4)&&m_GlowInputs[num_layers+i]==m_Connections[j])
        {
          done|=0x4;
          m_Connections.erase(m_Connections.begin() + j);
          delete m_GlowInputs[num_layers+i];
          m_GlowInputs[num_layers+i]=0;
          increment = false;
        }
        if(done==0x7)
          break;

        if(increment)
          ++j;
      }
    }
    m_ColourInputs.resize(num_layers);
    m_TransparencyInputs.resize(num_layers);
    m_GlowInputs.resize(num_layers);
  }
}

//----------------------------------------------------------------------------------------------------------------------
XU32 XLayeredShader::GetNumLayers() const
{
  return static_cast<XU32>( m_ColourInputs.size() );
}

//----------------------------------------------------------------------------------------------------------------------
void XLayeredShader::AddLayer(const XColour& colour,const XColour& transparency,const XColour& glow)
{
  m_ColourInputs.push_back( new XColourConnection("colour", colour) );
  m_Connections.push_back(m_ColourInputs.back());

  m_TransparencyInputs.push_back( new XColourConnection("transparency", transparency) );
  m_Connections.push_back(m_TransparencyInputs.back());

  m_GlowInputs.push_back( new XColourConnection("glow", glow) );
  m_Connections.push_back(m_GlowInputs.back());
}

//----------------------------------------------------------------------------------------------------------------------
void XLayeredShader::AddLayer(const XShadingNode* colour,const XShadingNode* transparency,const XShadingNode* glow)
{
  m_ColourInputs.push_back( new XColourConnection("colour", (colour ? colour->GetID() : 0)) );
  m_Connections.push_back(m_ColourInputs.back());

  m_TransparencyInputs.push_back( new XColourConnection("transparency", (transparency ? transparency->GetID() : 0)) );
  m_Connections.push_back(m_TransparencyInputs.back());

  m_GlowInputs.push_back( new XColourConnection("glow", (glow ? glow->GetID() : 0)) );
  m_Connections.push_back(m_GlowInputs.back());
}

//----------------------------------------------------------------------------------------------------------------------
bool XLayeredShader::RemoveLayer(const XU32 layer_index)
{
  if(layer_index>=GetNumLayers())
  {
    XGlobal::GetLogger()->Log(XBasicLogger::kError, "invalid layer_index specified");
    return false;
  }

  XU32 done=0;
  XShadingNode::XConnectionArray::iterator it = m_Connections.begin();
  for( ; it != m_Connections.end(); ++it )
  {
    if(!(done&0x1)&&m_ColourInputs[layer_index]==*it)
    {
      done|=0x1;
      m_Connections.erase(it);
      delete m_ColourInputs[layer_index];
      m_ColourInputs[layer_index]=0;
    }
    if(!(done&0x2)&&m_TransparencyInputs[layer_index]==*it)
    {
      done|=0x2;
      m_Connections.erase(it);
      delete m_TransparencyInputs[layer_index];
      m_TransparencyInputs[layer_index]=0;
    }
    if(!(done&0x4)&&m_GlowInputs[layer_index]==*it)
    {
      done|=0x4;
      m_Connections.erase(it);
      delete m_GlowInputs[layer_index];
      m_GlowInputs[layer_index]=0;
    }
    if(done==0x7)
      break;
  }
  m_ColourInputs.erase(m_ColourInputs.begin()+layer_index);
  m_TransparencyInputs.erase(m_TransparencyInputs.begin()+layer_index);
  m_GlowInputs.erase(m_GlowInputs.begin()+layer_index);

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool XLayeredShader::SetLayer(const XU32 layer_index,const XColour& colour,const XColour& transparency,const XColour& glow)
{
  if(layer_index>=GetNumLayers())
  {
    XGlobal::GetLogger()->Log(XBasicLogger::kError, "invalid layer_index specified");
    return false;
  }

  m_ColourInputs[layer_index]->m_Colour = colour;
  m_TransparencyInputs[layer_index]->m_Colour = transparency;
  m_GlowInputs[layer_index]->m_Colour = glow;

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool XLayeredShader::SetLayer(const XU32 layer_index,const XShadingNode* colour,const XShadingNode* transparency,const XShadingNode* glow)
{
  if(layer_index>=GetNumLayers())
  {
    XGlobal::GetLogger()->Log(XBasicLogger::kError, "invalid layer_index specified");
    return false;
  }

  m_ColourInputs[layer_index]->m_InputNode = colour ? colour->GetID() : 0 ;
  m_TransparencyInputs[layer_index]->m_InputNode = transparency ? transparency->GetID() : 0 ;
  m_GlowInputs[layer_index]->m_InputNode = glow ? glow->GetID() : 0 ;

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool XLayeredShader::GetLayerColour(const XU32 layer_index, XColour& colour) const
{
  if (layer_index<GetNumLayers())
  {
    colour = m_ColourInputs[layer_index]->m_Colour;
    return true;
  }
  XGlobal::GetLogger()->Log(XBasicLogger::kError, "invalid layer_index specified");
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
XShadingNode* XLayeredShader::GetLayerColour(const XU32 layer_index) const
{
  if (layer_index<GetNumLayers())
  {
    XBase* b = m_pModel->FindNode(m_ColourInputs[layer_index]->m_InputNode);
    return b ? b->HasFn<XShadingNode>() : 0;
  }
  XGlobal::GetLogger()->Log(XBasicLogger::kError, "invalid layer_index specified");
  return 0; 
}

//----------------------------------------------------------------------------------------------------------------------
bool XLayeredShader::GetLayerTransparency(const XU32 layer_index, XColour& transparency) const
{
  if (layer_index<GetNumLayers())
  {
    transparency = m_TransparencyInputs[layer_index]->m_Colour;
    return true;
  }
  XGlobal::GetLogger()->Log(XBasicLogger::kError, "invalid layer_index specified");
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
XShadingNode* XLayeredShader::GetLayerTransparency(const XU32 layer_index) const
{
  if (layer_index<GetNumLayers())
  {
    XBase* b = m_pModel->FindNode(m_TransparencyInputs[layer_index]->m_InputNode);
    return b ? b->HasFn<XShadingNode>() : 0;
  }
  XGlobal::GetLogger()->Log(XBasicLogger::kError, "invalid layer_index specified");
  return 0;   
}

//----------------------------------------------------------------------------------------------------------------------
bool XLayeredShader::GetLayerGlow(const XU32 layer_index, XColour& glow) const
{
  if (layer_index<GetNumLayers())
  {
    glow = m_GlowInputs[layer_index]->m_Colour;
    return true;
  }
  XGlobal::GetLogger()->Log(XBasicLogger::kError, "invalid layer_index specified");
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
XShadingNode* XLayeredShader::GetLayerGlow(const XU32 layer_index) const
{
  if (layer_index<GetNumLayers())
  {
    XBase* b = m_pModel->FindNode(m_GlowInputs[layer_index]->m_InputNode);
    return b ? b->HasFn<XShadingNode>() : 0;
  }
  XGlobal::GetLogger()->Log(XBasicLogger::kError, "invalid layer_index specified");
  return 0;    
}

//----------------------------------------------------------------------------------------------------------------------
bool XLayeredShader::IsShader() const 
{
  return m_CompositingType==0;
}

//----------------------------------------------------------------------------------------------------------------------
bool XLayeredShader::IsTexture() const 
{
  return m_CompositingType==1;
}

//----------------------------------------------------------------------------------------------------------------------
void XLayeredShader::SetType(const XU32 flag)
{
  if(flag <2)
    m_CompositingType = flag;
}

//----------------------------------------------------------------------------------------------------------------------
XFn::Type XLayeredShader::GetApiType() const
{
  return XFn::LayeredShader;
}

//----------------------------------------------------------------------------------------------------------------------
XBase* XLayeredShader::GetFn(XFn::Type type)
{
  if(XFn::LayeredShader==type)
    return (XLayeredShader*)this;
  return XSurfaceShader::GetFn(type);
}

//----------------------------------------------------------------------------------------------------------------------
const XBase* XLayeredShader::GetFn(XFn::Type type) const
{
  if(XFn::LayeredShader==type)
    return (const XLayeredShader*)this;
  return XSurfaceShader::GetFn(type);
}

//----------------------------------------------------------------------------------------------------------------------
void XLayeredShader::DoCopy(const XBase* rhs)
{
  const XLayeredShader* cb = rhs->HasFn<XLayeredShader>();
  XMD_ASSERT(cb);

  if(cb)
  {
    m_ColourInputs = cb->m_ColourInputs;
    m_TransparencyInputs = cb->m_TransparencyInputs;
    m_GlowInputs = cb->m_GlowInputs;
    m_CompositingType = cb->m_CompositingType;

    XSurfaceShader::DoCopy(cb);
  }
}

//----------------------------------------------------------------------------------------------------------------------
XU32 XLayeredShader::GetDataSize() const
{
  XU32 size = XSurfaceShader::GetDataSize();
  size += sizeof(XU32); // compositing flag
  for(XU32 i=0 ; i != static_cast<XU32>(m_ColourInputs.size()); ++i)
  {
    size += m_ColourInputs[i]->GetDataSize();
    size += m_TransparencyInputs[i]->GetDataSize();
    size += m_GlowInputs[i]->GetDataSize();
  }
  size += static_cast<XU32>(sizeof(XU32));
  return size;
}

//----------------------------------------------------------------------------------------------------------------------
bool XLayeredShader::DoData(XFileIO& io)
{
  DUMPER(XLayeredShader);

  if( io.IsWriting() || m_pModel->GetInputVersion()>=4 )
  {
    IO_CHECK( XSurfaceShader::DoData(io) );
    IO_CHECK( io.DoData(&m_CompositingType) );

    XU32 size=0;
    if( io.IsWriting() )
      size = static_cast<XU32>( m_ColourInputs.size() );

    IO_CHECK( io.DoData(&size) );
    XColourConnection *colour=0, *transparency=0, *glow=0;
    for( XU32 i = 0; i != size; ++i )
    {
      if( io.IsWriting() )
      {
        colour = m_ColourInputs[i];
        transparency = m_TransparencyInputs[i];
        glow = m_GlowInputs[i];
      }
      else
      {
        colour = new XColourConnection("colour");
        transparency = new XColourConnection("transparency");
        glow = new XColourConnection("glow");

        m_ColourInputs.push_back( colour );
        m_Connections.push_back( colour );
        m_TransparencyInputs.push_back( transparency );
        m_Connections.push_back( transparency );
        m_GlowInputs.push_back( glow );
        m_Connections.push_back( glow );
      }
      
      IO_CHECK( colour->DoData(io) );
      IO_CHECK( transparency->DoData(io) );
      IO_CHECK( glow->DoData(io) );
    }
  }
  else
  {
    IO_CHECK( XBase::DoData(io) );

    XColourConnection temp("ambient");
    IO_CHECK( temp.DoData(io) );

    IO_CHECK( m_Diffuse.DoData(io) );
    IO_CHECK( m_Diffuse.m_Name == "color" );
    m_Diffuse.m_Name = "colour";

    temp.m_Name = "specular_color";
    IO_CHECK( temp.DoData(io) );
    temp.m_Name = "incandescence";
    IO_CHECK( temp.DoData(io) );

    IO_CHECK( m_Transparency.DoData(io) );

    // legacy, lambert has no shininess
    XReal shininess = 0;
    IO_CHECK( io.DoData(&shininess) );
    DPARAM( shininess );

    temp.m_Name = "bump_map";
    IO_CHECK( io.DoData(&temp.m_InputNode) );
    DPARAM( temp.m_InputNode );
    temp.m_Name = "env_map";
    IO_CHECK( io.DoData(&temp.m_InputNode) );
    DPARAM( temp.m_InputNode );
    IO_CHECK( io.DoData(&m_HwShader) );
    DPARAM( m_HwShader );

    XIndexList colours, transparencies, glows;
    IO_CHECK( io.DoDataVector(colours) );
    IO_CHECK( io.DoDataVector(transparencies) );
    IO_CHECK( io.DoDataVector(glows) );

    XMD_ASSERT(colours.size() == transparencies.size());
    XMD_ASSERT(colours.size() == glows.size());

    if(m_ColourInputs.size() == m_TransparencyInputs.size() &&
       m_ColourInputs.size() == m_GlowInputs.size())
    {
      XIndexList::const_iterator colour = colours.begin();
      XIndexList::const_iterator transparency = transparencies.begin();
      XIndexList::const_iterator glow = glows.begin();
      for( ; colour != colours.end(); ++colour, ++transparency, ++glow )
      {
        m_ColourInputs.push_back( new XColourConnection("colour", *colour));
        m_Connections.push_back(m_ColourInputs.back());

        m_TransparencyInputs.push_back( new XColourConnection("transparency", *transparency));
        m_Connections.push_back(m_TransparencyInputs.back());

        m_GlowInputs.push_back( new XColourConnection("glow", *glow));
        m_Connections.push_back(m_GlowInputs.back());
      }
    }
    else
    {
      XGlobal::GetLogger()->Log(XBasicLogger::kError,"Layered Shader inputs are invalid. Aborting the reading of the layered shader");
      return false;
    }
    IO_CHECK( io.DoData(&m_CompositingType) );
  }
  
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool XLayeredShader::ReadChunk(std::istream& ifs)
{
  if (GetModel()->GetInputVersion()>=4)
  {
    IO_CHECK( XSurfaceShader::ReadChunk(ifs) );
    READ_CHECK("compositing_flag",ifs);
    ifs >> m_CompositingType;

    XU32 num=0;
    READ_CHECK("inputs",ifs);
    ifs >> num;

    XColourConnection *colour, *transparency, *glow;
    for (XU32 i=0; i != num; ++i)
    {
      colour = new XColourConnection("colour");
      transparency = new XColourConnection("transparency");
      glow = new XColourConnection("glow");

      m_ColourInputs.push_back( colour );
      m_Connections.push_back( colour );
      m_TransparencyInputs.push_back( transparency );
      m_Connections.push_back( transparency );
      m_GlowInputs.push_back( glow );
      m_Connections.push_back( glow );

      IO_CHECK(colour->ReadChunk(ifs));
      IO_CHECK(transparency->ReadChunk(ifs));
      IO_CHECK(glow->ReadChunk(ifs));
    }
  }
  else
  {
    XColourConnection temp("");

    READ_CHECK("ambient_color", ifs);
    ifs >> temp.m_Colour;
    READ_CHECK("tex", ifs);
    ifs >> temp.m_InputNode;

    READ_CHECK("color", ifs);
    ifs >> m_Diffuse.m_Colour;
    READ_CHECK("tex", ifs);
    ifs >> m_Diffuse.m_InputNode;

    READ_CHECK("specular_color", ifs);
    ifs >> temp.m_Colour;
    READ_CHECK("tex", ifs);
    ifs >> temp.m_InputNode;

    READ_CHECK("incandescence", ifs);
    ifs >> temp.m_Colour;
    READ_CHECK("tex", ifs);
    ifs >> temp.m_InputNode;

    READ_CHECK(m_Transparency.m_Name.c_str(), ifs);
    ifs >> m_Transparency.m_Colour;
    READ_CHECK("tex", ifs);
    ifs >> m_Transparency.m_InputNode;

    READ_CHECK("bump_map",ifs);
    ifs >> temp.m_InputNode;
    READ_CHECK("env_map",ifs);
    ifs >> temp.m_InputNode;
    READ_CHECK("hw_shader",ifs);
    ifs >> m_HwShader;

    XU32 num=0;
    XIndexList colours, transparencies, glows;
    READ_CHECK("colour_inputs",ifs);
    ifs >> num;
    colours.resize(num);
    XIndexList::iterator it = colours.begin();
    for (; it != colours.end(); ++it)
    {
      ifs >> *it;
    }

    READ_CHECK("transparency_inputs",ifs);
    ifs >> num;
    if (num!=colours.size())
    {
      XGlobal::GetLogger()->Log(XBasicLogger::kError,"Layered Shader inputs are invalid. Aborting the reading of the layered shader");
      return false;
    }
    transparencies.resize(num);
    it = transparencies.begin();
    for (; it != transparencies.end(); ++it)
    {
      ifs >> *it;
    }

    READ_CHECK("glow_inputs",ifs);
    ifs >> num;
    if (num!=colours.size())
    {
      XGlobal::GetLogger()->Log(XBasicLogger::kError,"Layered Shader inputs are invalid. Aborting the reading of the layered shader");
      return false;
    }
    glows.resize(num);
    it = glows.begin();
    for (; it != glows.end(); ++it)
    {
      ifs >> *it;
    }

    XIndexList::const_iterator colour = colours.begin();
    XIndexList::const_iterator transparency = transparencies.begin();
    XIndexList::const_iterator glow = glows.begin();
    for( ; colour != colours.end(); ++colour, ++transparency, ++glow )
    {
      m_ColourInputs.push_back( new XColourConnection("colour", *colour));
      m_Connections.push_back(m_ColourInputs.back());

      m_TransparencyInputs.push_back( new XColourConnection("transparency", *transparency));
      m_Connections.push_back(m_TransparencyInputs.back());

      m_GlowInputs.push_back( new XColourConnection("glow", *glow));
      m_Connections.push_back(m_GlowInputs.back());
    }
    READ_CHECK("compositing_flag",ifs);
    ifs >> m_CompositingType;
  }

  return ifs.good();
}

//----------------------------------------------------------------------------------------------------------------------
bool XLayeredShader::WriteChunk(std::ostream& ofs)
{
  IO_CHECK( XSurfaceShader::WriteChunk(ofs) );
  ofs << "compositing_flag " << m_CompositingType << std::endl;
  XU32 num = static_cast<XU32>( m_ColourInputs.size() );
  ofs << "inputs " << num << std::endl;
  
  --num;
  XConnectionArray::const_iterator colour = m_ColourInputs.begin();
  XConnectionArray::const_iterator transparency = m_TransparencyInputs.begin();
  XConnectionArray::const_iterator glow = m_GlowInputs.begin();
  for (XU32 i = 0 ; colour != m_ColourInputs.end(); ++colour, ++transparency, ++glow, ++i)
  {
    ofs << "\t";
    IO_CHECK((*colour)->WriteChunk(ofs));
    ofs << "\t";
    IO_CHECK((*transparency)->WriteChunk(ofs));
    ofs << "\t";
    IO_CHECK((*glow)->WriteChunk(ofs));
    if(i != num)
      ofs << std::endl;
  }
  return ofs.good();
}
}
