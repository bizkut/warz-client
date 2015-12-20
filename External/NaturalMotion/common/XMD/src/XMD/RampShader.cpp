//----------------------------------------------------------------------------------------------------------------------
/// \file RampShader.cpp
/// \note (C) Copyright 2003-2005 Robert Bateman. All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#include "XMD/RampShader.h"
#include "XMD/Model.h"
#include "XMD/FileIO.h"

namespace XMD
{
//----------------------------------------------------------------------------------------------------------------------
XRampShader::XRampShader(XModel* pmod)
  : XSurfaceShader(pmod)
{
}

//----------------------------------------------------------------------------------------------------------------------
XRampShader::~XRampShader()
{
  for(XU32 i = 0; i != 5; ++i)
  {
    XConnectionArray& connection_array = m_RampColourValues[i];
    XShadingNode::XConnectionArray::const_iterator it = connection_array.begin();
    for( ; it != connection_array.end(); ++it )
    {
      delete *it;
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
void XRampShader::PreDelete(XIdSet& extra_nodes)
{
  XSurfaceShader::PreDelete(extra_nodes);
}

//----------------------------------------------------------------------------------------------------------------------
bool XRampShader::NodeDeath(XId id) 
{
  return XSurfaceShader::NodeDeath(id);
}

//----------------------------------------------------------------------------------------------------------------------
void XRampShader::SetNumRampValues(const RampType ramp, const XU32 num_values)
{
  m_RampInterpTypes[ramp].resize(num_values);
  m_RampPositions[ramp].resize(num_values);
  if(ramp==kSpecularRollOffRamp || ramp==kReflectivityRamp)
  {
    m_RampRealValues[ramp-5].resize(num_values);
  }
  else
  {
    XU32 curr_size=static_cast<XU32>( m_RampColourValues[ramp].size() );
    XConnectionArray& connection_array = m_RampColourValues[ramp];
    if (num_values>curr_size)
    {
      XU32 to_add = num_values - curr_size;
      for(XU32 i = 0; i != to_add ; ++i)
      {
        connection_array.push_back( new XColourConnection("value") );
        m_Connections.push_back(connection_array.back());
      }
    }
    else if (num_values<curr_size)
    {
      XU32 to_remove = curr_size - num_values;
      for(XU32 i = 0; i != to_remove ; ++i)
      {
        for(XU32 j = 0; j != static_cast<XU32>( m_Connections.size() ) ; ++j)
        {
          if(connection_array[num_values+i]==m_Connections[j])
          {
            m_Connections.erase(m_Connections.begin() + j);
            delete connection_array[num_values+i];
            connection_array[num_values+i]=0;
            break;
          }
        }
      }
      connection_array.resize(num_values);
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
XU32 XRampShader::GetNumRampValues(const RampType ramp) const
{
  return static_cast<XU32>(m_RampPositions[ramp].size());
}

//----------------------------------------------------------------------------------------------------------------------
bool XRampShader::AddValue(const RampType ramp, const XColour& colour, const XReal position, const XU32 interp_type)
{
  if(ramp!=kSpecularRollOffRamp && ramp!=kReflectivityRamp)
  {
    m_RampColourValues[ramp].push_back(new XColourConnection("value", colour));
    m_RampInterpTypes[ramp].push_back(interp_type);
    m_RampPositions[ramp].push_back(position);
    return true;
  }
      XGlobal::GetLogger()->Log(XBasicLogger::kError, "XRampShader::AddValue : ramp specified does not contain colour connections");
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool XRampShader::AddValue(const RampType ramp, const XShadingNode* colour, const XReal position, const XU32 interp_type)
{
  if(ramp!=kSpecularRollOffRamp && ramp!=kReflectivityRamp)
  {
    m_RampColourValues[ramp].push_back(new XColourConnection("value", (colour ? colour->GetID() : 0)));
    m_RampInterpTypes[ramp].push_back(interp_type);
    m_RampPositions[ramp].push_back(position);
    return true;
  }
      XGlobal::GetLogger()->Log(XBasicLogger::kError, "XRampShader::AddValue : ramp specified does not contain colour connections");
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool XRampShader::AddValue(const RampType ramp, const XReal value, const XReal position, const XU32 interp_type)
{
  if(ramp==kSpecularRollOffRamp || ramp==kReflectivityRamp)
  {
    m_RampRealValues[ramp-5].push_back(value);
    m_RampInterpTypes[ramp].push_back(interp_type);
    m_RampPositions[ramp].push_back(position);
    return true;
  }
  XGlobal::GetLogger()->Log(XBasicLogger::kError, "XRampShader::AddValue : ramp specified does not contain XReal values");
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool XRampShader::RemoveValue(const RampType ramp, const XU32 index)
{
  if(index>=GetNumRampValues(ramp))
  {
    XGlobal::GetLogger()->Log(XBasicLogger::kError, "XRampShader::RemoveValue : invalid layer_index specified");
    return false;
  }

  m_RampInterpTypes[ramp].erase(m_RampInterpTypes[ramp].begin()+index);
  m_RampPositions[ramp].erase(m_RampPositions[ramp].begin()+index);

  if(ramp==kSpecularRollOffRamp || ramp==kReflectivityRamp)
  {
    m_RampRealValues[ramp-5].erase(m_RampRealValues[ramp-5].begin()+index);
  }
  else
  {
    XConnectionArray& connection_array = m_RampColourValues[ramp];
    XShadingNode::XConnectionArray::iterator it = m_Connections.begin();
    for( ; it != m_Connections.end(); ++it )
    {
      if(connection_array[index]==*it)
      {
        m_Connections.erase(it);
        delete connection_array[index];
        connection_array[index]=0;
      }

    }
    connection_array.erase(connection_array.begin()+index);
  }
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool XRampShader::SetValue(const RampType ramp, const XU32 index, const XColour& colour)
{
  if(ramp!=kSpecularRollOffRamp && ramp!=kReflectivityRamp)
  {
    XConnectionArray& connection_array = m_RampColourValues[ramp];
    if(index>=connection_array.size())
    {
      XGlobal::GetLogger()->Log(XBasicLogger::kError, "XRampShader::SetValue : invalid index specified");
      return false;
    }
    connection_array[index]->m_Colour = colour;
    return true;
  }
  XGlobal::GetLogger()->Log(XBasicLogger::kError, "XRampShader::SetValue : ramp specified does not contain colour connections");
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool XRampShader::SetValue(const RampType ramp, const XU32 index, const XShadingNode* input)
{
  if(ramp!=kSpecularRollOffRamp && ramp!=kReflectivityRamp)
  {
    XConnectionArray& connection_array = m_RampColourValues[ramp];
    if(index>=connection_array.size())
    {
      XGlobal::GetLogger()->Log(XBasicLogger::kError, "XRampShader::SetValue : invalid index specified");
      return false;
    }
    connection_array[index]->m_InputNode = (input) ? input->GetID() : 0;
    return true;
  }
  XGlobal::GetLogger()->Log(XBasicLogger::kError, "XRampShader::SetValue : ramp specified does not contain colour connections");
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool XRampShader::SetValue(const RampType ramp, const XU32 index, const XReal value)
{
  if(ramp==kSpecularRollOffRamp || ramp==kReflectivityRamp)
  {
    XRealArray& real_array = m_RampRealValues[ramp];
    if(index>=real_array.size())
    {
      XGlobal::GetLogger()->Log(XBasicLogger::kError, "XRampShader::SetValue : invalid index specified");
      return false;
    }
    real_array[index] = value;
    return true;
  }
  XGlobal::GetLogger()->Log(XBasicLogger::kError, "XRampShader::SetValue : ramp specified does not contain XReal values");
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool XRampShader::SetInterpType(const RampType ramp, const XU32 index, const InterpolationType type)
{
  XU32Array& interp_array = m_RampInterpTypes[ramp];
  if(index>=interp_array.size())
  {
    XGlobal::GetLogger()->Log(XBasicLogger::kError, "XRampShader::SetInterpType : invalid index specified");
    return false;
  }
  interp_array[index] = type;
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool XRampShader::SetPosition(const RampType ramp, const XU32 index, const XReal pos)
{
  XRealArray& position_array = m_RampPositions[ramp];
  if(index>=position_array.size())
  {
    XGlobal::GetLogger()->Log(XBasicLogger::kError, "XRampShader::SetPosition : invalid index specified");
    return false;
  }
  position_array[index] = pos;
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool XRampShader::GetValue(const RampType ramp, const XU32 index, XColour &colour) const
{
  if(ramp!=kSpecularRollOffRamp && ramp!=kReflectivityRamp)
  {
    const XConnectionArray& connection_array = m_RampColourValues[ramp];
    if(index>=connection_array.size())
    {
      XGlobal::GetLogger()->Log(XBasicLogger::kError, "XRampShader::GetValue : invalid index specified");
      return false;
    }
    colour = connection_array[index]->m_Colour;
    return true;
  }
  XGlobal::GetLogger()->Log(XBasicLogger::kError, "XRampShader::GetValue : ramp specified does not contain colour connections");
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool XRampShader::GetValue(const RampType ramp, const XU32 index, XShadingNode *&input) const
{
  if(ramp!=kSpecularRollOffRamp && ramp!=kReflectivityRamp)
  {
    const XConnectionArray& connection_array = m_RampColourValues[ramp];
    if(index>=connection_array.size())
    {
      XGlobal::GetLogger()->Log(XBasicLogger::kError, "XRampShader::GetValue : invalid index specified");
      return false;
    }
    XBase* b = m_pModel->FindNode(connection_array[index]->m_InputNode);
    input = b ? b->HasFn<XShadingNode>() : 0;

    return true;
  }
  XGlobal::GetLogger()->Log(XBasicLogger::kError, "XRampShader::GetValue : ramp specified does not contain colour connections");
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool XRampShader::GetValue(const RampType ramp, const XU32 index, XReal &value) const
{
  if(ramp==kSpecularRollOffRamp || ramp==kReflectivityRamp)
  {
    const XRealArray& real_array = m_RampRealValues[ramp];
    if(index>=real_array.size())
    {
      XGlobal::GetLogger()->Log(XBasicLogger::kError, "XRampShader::GetValue : invalid index specified");
      return false;
    }
    value = real_array[index];
    return true;
  }
  XGlobal::GetLogger()->Log(XBasicLogger::kError, "XRampShader::GetValue : ramp specified does not contain XReal values");
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
XRampShader::InterpolationType XRampShader::GetInterpType(const RampType ramp, const XU32 index) const
{
  const XU32Array& interp_array = m_RampInterpTypes[ramp];
  if(index>=interp_array.size())
  {
    XGlobal::GetLogger()->Log(XBasicLogger::kError, "XRampShader::SetInterpType : invalid index specified");
    return kNone;
  }
  return static_cast<InterpolationType>( interp_array[index] );
}

//----------------------------------------------------------------------------------------------------------------------
XReal XRampShader::GetPosition(const RampType ramp, const XU32 index) const
{
  const XRealArray& position_array = m_RampPositions[ramp];
  if(index>=position_array.size())
  {
    XGlobal::GetLogger()->Log(XBasicLogger::kError, "XRampShader::SetPosition : invalid index specified");
    return 0.0;
  }
  return position_array[index];
}

//----------------------------------------------------------------------------------------------------------------------
XFn::Type XRampShader::GetApiType() const
{
  return XFn::RampShader;
}

//----------------------------------------------------------------------------------------------------------------------
XBase* XRampShader::GetFn(XFn::Type type)
{
  if(XFn::RampShader==type)
    return (XRampShader*)this;
  return XSurfaceShader::GetFn(type);
}

//----------------------------------------------------------------------------------------------------------------------
const XBase* XRampShader::GetFn(XFn::Type type) const
{
  if(XFn::RampShader==type)
    return (const XRampShader*)this;
  return XSurfaceShader::GetFn(type);
}

//----------------------------------------------------------------------------------------------------------------------
void XRampShader::DoCopy(const XBase* rhs)
{
  const XRampShader* cb = rhs->HasFn<XRampShader>();
  XMD_ASSERT(cb);
  if(cb)
  {

    XSurfaceShader::DoCopy(cb);
  }
}

//----------------------------------------------------------------------------------------------------------------------
XU32 XRampShader::GetDataSize() const
{
  XU32 size = XSurfaceShader::GetDataSize();

  for(XU32 i = 0; i != kLastRamp; ++i)
  {
    size += sizeof(XU32);
    size += static_cast<XU32>(m_RampInterpTypes[i].size()) * sizeof(XU32);
    size += static_cast<XU32>(m_RampPositions[i].size()) * sizeof(XReal);
  }
  for(XU32 i = 0; i != 5; ++i)
  {
    XConnectionArray::const_iterator it = m_RampColourValues[i].begin();
    for(; it != m_RampColourValues[i].end(); ++it)
    {
      size += (*it)->GetDataSize();
    }
  }
  size += static_cast<XU32>(m_RampRealValues[0].size()) * sizeof(XReal);
  size += static_cast<XU32>(m_RampRealValues[1].size()) * sizeof(XReal);

  return size;
}

//----------------------------------------------------------------------------------------------------------------------
bool XRampShader::DoData(XFileIO& io)
{
  DUMPER(XRampShader);

  if(io.IsWriting() || GetModel()->GetInputVersion()>=4)
  {
    IO_CHECK( XSurfaceShader::DoData(io) );

    for(XU32 i = 0; i != kLastRamp; ++i)
    {
      XU32 num_values = static_cast<XU32>( m_RampPositions[i].size() );
      io.DoData(&num_values);
      
      if(!io.IsWriting())
      {
        if(i > kEnvironmentRamp)
        {
          m_RampRealValues[i-5].resize(num_values);
        }
        else
        {
          for(XU32 j = 0; j != num_values; ++j)
          {
            XColourConnection* value = new XColourConnection("value");
            m_Connections.push_back( value );
            m_RampColourValues[i].push_back(value);
          } 
        }

        m_RampPositions[i].resize(num_values);
        m_RampInterpTypes[i].resize(num_values);
      }
      
      for(XU32 j = 0; j != num_values; ++j)
      {
        if(i > kEnvironmentRamp)
        {
          io.DoData(&m_RampRealValues[i-kSpecularRollOffRamp][j]);
        }
        else
        {
          m_RampColourValues[i][j]->DoData(io);
        }
        io.DoData(&m_RampPositions[i][j]);
        io.DoData(&m_RampInterpTypes[i][j]);
      }
    }
  }
  else
  {
    IO_CHECK( XBase::DoData(io) );

    XColourConnection temp("ambient");
    IO_CHECK( temp.DoData(io) );

    IO_CHECK( m_Diffuse.DoData(io) );

    temp.m_Name = "specular";
    IO_CHECK( temp.DoData(io) );
    temp.m_Name = "incandescence";
    IO_CHECK( temp.DoData(io) );

    temp.m_Name = "transparency";
    IO_CHECK( temp.DoData(io) );

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
  }

  return true;
}


// forward declarations
const char* RampTypeAsString(const XRampShader::RampType ramp);
const char* InterpTypeAsString(const XRampShader::InterpolationType type);
const XRampShader::RampType RampTypeFromString(std::string ramp);
const XRampShader::InterpolationType InterpTypeFromString(std::string type);

//----------------------------------------------------------------------------------------------------------------------
bool XRampShader::ReadChunk(std::istream& ifs)
{
  if(GetModel()->GetInputVersion()>=4)
  {
    IO_CHECK( XSurfaceShader::ReadChunk(ifs) );

    for(XU32 i = 0; i != kLastRamp; ++i)
    {
      XU32 num_values;
      READ_CHECK(RampTypeAsString(static_cast<RampType>(i)), ifs);
      ifs >> num_values;

      XReal temp = 0.0;
      std::string buffer;
      for(XU32 j = 0; j != num_values; ++j)
      {
        if(i > 4)
        {
          READ_CHECK("value", ifs);
          ifs >> temp;
          m_RampRealValues[i-5].push_back(temp);
        }
        else
        {
          XColourConnection* value = new XColourConnection("value");
          
          m_RampColourValues[i].push_back( value );
          m_Connections.push_back( value );

          IO_CHECK(value->ReadChunk(ifs));
        }
        READ_CHECK("position", ifs);
        ifs >> temp;
        m_RampPositions[i].push_back(temp);
        READ_CHECK("interp_type", ifs);
        ifs >> buffer;
        m_RampInterpTypes[i].push_back(InterpTypeFromString(buffer));
      }
    }
  }
  else
  {
    XId temp_id;
    XColour temp_colour;
    READ_CHECK("ambient_color", ifs);
    ifs >> temp_colour;
    READ_CHECK("tex", ifs);
    ifs >> temp_id;

    READ_CHECK("color", ifs);
    ifs >> m_Diffuse.m_Colour;
    READ_CHECK("tex", ifs);
    ifs >> m_Diffuse.m_InputNode;

    READ_CHECK("specular_color", ifs);
    ifs >> temp_colour;
    READ_CHECK("tex", ifs);
    ifs >> temp_id;

    READ_CHECK("incandescence", ifs);
    ifs >> temp_colour;
    READ_CHECK("tex", ifs);
    ifs >> temp_id;

    READ_CHECK("transparency", ifs);
    ifs >> temp_colour;
    READ_CHECK("tex", ifs);
    ifs >> temp_id;

    READ_CHECK("bump_map",ifs);
    ifs >> temp_id;
    READ_CHECK("env_map",ifs);
    ifs >> temp_id;
    READ_CHECK("hw_shader",ifs);
    ifs >> m_HwShader;
  }

  return ifs.good();
}

//----------------------------------------------------------------------------------------------------------------------
bool XRampShader::WriteChunk(std::ostream& ofs)
{
  IO_CHECK( XSurfaceShader::WriteChunk(ofs) );

  for(XU32 i = 0; i != kLastRamp; ++i)
  {
    XU32 num_values = static_cast<XU32>( m_RampPositions[i].size() );

    ofs << "\t" << RampTypeAsString(static_cast<RampType>(i)) << " " << num_values << std::endl;
    
    for(XU32 j = 0; j != num_values; ++j)
    {
      if(i > 4)
      {
        ofs << "\t\tvalue " << (m_RampRealValues[i-5])[j] << std::endl;;
      }
      else
      {
        IO_CHECK( (m_RampColourValues[i])[j]->WriteChunk(ofs) );
      }
      ofs << "\t\tposition " << (m_RampPositions[i])[j] << std::endl;;
      ofs << "\t\tinterp_type ";
      ofs << InterpTypeAsString(static_cast<InterpolationType>( (m_RampInterpTypes[i])[j] ));
      ofs << std::endl;

      if(j != num_values - 1)
      {
        ofs << std::endl;
      }
    }

    if(i != kLastRamp - 1)
    {
      ofs << std::endl;
    }
  }

  return ofs.good();
}

//----------------------------------------------------------------------------------------------------------------------
const char* RampTypeAsString(const XRampShader::RampType ramp)
{
  switch(ramp)
  {
  case XRampShader::kColourRamp:
    return "colour_ramp";
  case XRampShader::kSpecularRamp:
    return "specular_ramp";
  case XRampShader::kTransparencyRamp:
    return "transparency_ramp";
  case XRampShader::kEmissionRamp:
    return "emission_ramp";
  case XRampShader::kEnvironmentRamp:
    return "environment_ramp";
  case XRampShader::kSpecularRollOffRamp:
    return "specular_roll_off_ramp";
  case XRampShader::kReflectivityRamp:
    return "reflectivity_ramp";
  default:
    return "Invalid";
  }
}

//----------------------------------------------------------------------------------------------------------------------
const char* InterpTypeAsString(const XRampShader::InterpolationType type)
{
  switch(type)
  {
  case XRampShader::kNone:
    return "kNone";
  case XRampShader::kLinear:
    return "kLinear";
  case XRampShader::kSmooth:
    return "kSmooth";
  case XRampShader::kSpline:
    return "kSpline";
  default:
    return "kInvalid";
  }
}

//----------------------------------------------------------------------------------------------------------------------
const XRampShader::RampType RampTypeFromString(std::string ramp)
{
  if(ramp =="colour_ramp" )
    return XRampShader::kColourRamp;
  else if(ramp =="specular_ramp" )
    return XRampShader::kSpecularRamp;
  else if(ramp =="transparency_ramp" )
    return XRampShader::kTransparencyRamp;
  else if(ramp =="emission_ramp" )
    return XRampShader::kEmissionRamp;
  else if(ramp =="environment_ramp" )
    return XRampShader::kEnvironmentRamp;
  else if(ramp =="specular_roll_off_ramp" )
    return XRampShader::kSpecularRollOffRamp;
  else
    return XRampShader::kReflectivityRamp;
}

//----------------------------------------------------------------------------------------------------------------------
const XRampShader::InterpolationType InterpTypeFromString(std::string type)
{
  if(type == "kNone")
    return XRampShader::kNone;
  else if (type == "kLinear")
    return XRampShader::kLinear;
  else if (type == "kSmooth")
    return XRampShader::kSmooth;
  else
    return XRampShader::kSpline;
}
}

