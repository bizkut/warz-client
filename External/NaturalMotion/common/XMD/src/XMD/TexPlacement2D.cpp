//----------------------------------------------------------------------------------------------------------------------
/// \file TexPlacement2D.cpp
/// \note (C) Copyright 2003-2005 Robert Bateman. All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#include "XMD/TexPlacement2D.h"
#include "XMD/Model.h"
#include "XMD/FileIO.h"

namespace XMD 
{
//----------------------------------------------------------------------------------------------------------------------
XTexPlacement2D::XTexPlacement2D(XModel* pmod)
  : XShadingNode(pmod)
{
  m_ImageIndex=0;

  m_TranslateFrame[0]=0;
  m_TranslateFrame[1]=0;
  m_RotateFrame=0;
  m_MirrorU=false;
  m_MirrorV=false;
  m_WrapU=false;
  m_WrapV=false;
  m_Stagger=false;

  m_RepeatUV[0]=1;
  m_RepeatUV[1]=1;
  m_OffsetUV[0]=0;
  m_OffsetUV[1]=0;
  m_NoiseUV[0]=0;
  m_NoiseUV[1]=0;
  m_RotateUV=0;
  m_FileTextureName = "";
  m_FilterType = kNone;
  m_FilterAmount = 1.0f;
  m_PreFilter = false;
  m_PreFilterRadius = 2.0f;
}

//----------------------------------------------------------------------------------------------------------------------
XReal XTexPlacement2D::GetFilterAmount() const
{
  return m_FilterAmount;
}

//----------------------------------------------------------------------------------------------------------------------
void XTexPlacement2D::SetFilterAmount(const XReal amount)
{
  m_FilterAmount = amount;
}

//----------------------------------------------------------------------------------------------------------------------
const XChar* XTexPlacement2D::GetFileTextureName() const
{
  return m_FileTextureName.c_str();
}

//----------------------------------------------------------------------------------------------------------------------
void XTexPlacement2D::SetFileTextureName(const XChar* filename)
{
  m_FileTextureName = filename;
}

//----------------------------------------------------------------------------------------------------------------------
XTexPlacement2D::FilterType XTexPlacement2D::GetFilterType() const
{
  return m_FilterType;
}

//----------------------------------------------------------------------------------------------------------------------
void XTexPlacement2D::SetFilterType(const XTexPlacement2D::FilterType type)
{
  m_FilterType = type;
}

//----------------------------------------------------------------------------------------------------------------------
bool XTexPlacement2D::GetPreFilter() const
{
  return m_PreFilter;
}

//----------------------------------------------------------------------------------------------------------------------
void XTexPlacement2D::SetPreFilter(const bool value)
{
  m_PreFilter = value;
}

//----------------------------------------------------------------------------------------------------------------------
XReal XTexPlacement2D::GetPreFilterRadius() const
{
  return m_PreFilterRadius;
}

//----------------------------------------------------------------------------------------------------------------------
void XTexPlacement2D::SetPreFilterRadius(const XReal radius)
{
  m_PreFilterRadius = radius;
}

//----------------------------------------------------------------------------------------------------------------------
XImage* XTexPlacement2D::GetImage() const
{
  return m_pModel->GetImage( m_ImageIndex );
}

//----------------------------------------------------------------------------------------------------------------------
bool XTexPlacement2D::SetImage(const XId id) 
{
  if(GetModel()->GetImage(id)) 
  {
    m_ImageIndex = id;
    return true;
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
XTexPlacement2D::~XTexPlacement2D()
{
}

//----------------------------------------------------------------------------------------------------------------------
XFn::Type XTexPlacement2D::GetApiType() const
{
  return XFn::TexturePlacement2D;
}

//----------------------------------------------------------------------------------------------------------------------
XBase* XTexPlacement2D::GetFn(XFn::Type type)
{
  if(XFn::TexturePlacement2D==type)
    return (XTexPlacement2D*)this;
  return XShadingNode::GetFn(type);
}

//----------------------------------------------------------------------------------------------------------------------
const XBase* XTexPlacement2D::GetFn(XFn::Type type) const
{
  if(XFn::TexturePlacement2D==type)
    return (const XTexPlacement2D*)this;
  return XShadingNode::GetFn(type);
}

//----------------------------------------------------------------------------------------------------------------------
bool XTexPlacement2D::NodeDeath(XId id)
{
  return XShadingNode::NodeDeath(id);
}

//----------------------------------------------------------------------------------------------------------------------
void XTexPlacement2D::PreDelete(XIdSet& extra_nodes)
{
  XShadingNode::PreDelete(extra_nodes);
}

//----------------------------------------------------------------------------------------------------------------------
bool XTexPlacement2D::ReadChunk(std::istream& ifs)
{
  if(GetModel()->GetInputVersion()>=4) 
  {
    IO_CHECK( XShadingNode::ReadChunk(ifs) );
  }
  else
  {
    IO_CHECK( XBase::ReadChunk(ifs) );
  }

  READ_CHECK("image",ifs);
  ifs >> m_ImageIndex;

  READ_CHECK("translate_frame",ifs);
  ifs >> m_TranslateFrame[0] >> m_TranslateFrame[1];

  READ_CHECK("rotate_frame",ifs);
  ifs >> m_RotateFrame;

  READ_CHECK("mirror_u",ifs);
  ifs >> m_MirrorU;

  READ_CHECK("mirror_v",ifs);
  ifs >> m_MirrorV;

  READ_CHECK("stagger",ifs);
  ifs >> m_Stagger;

  READ_CHECK("wrap_u",ifs);
  ifs >> m_WrapU;

  READ_CHECK("wrap_v",ifs);
  ifs >> m_WrapV;

  READ_CHECK("repeat_uv",ifs);
  ifs >> m_RepeatUV[0] >> m_RepeatUV[1];

  READ_CHECK("offset_uv",ifs);
  ifs >> m_OffsetUV[0] >> m_OffsetUV[1];

  READ_CHECK("rotate_uv",ifs);
  ifs >> m_RotateUV;

  READ_CHECK("noise_uv",ifs);
  ifs >> m_NoiseUV[0] >> m_NoiseUV[1];

  if (GetModel()->GetInputVersion()>=4)
  {
    READ_CHECK("file_texture_name",ifs);
    bool readFirst=false;
    bool done=false;
    while(!done)
    {
      XChar c = (XChar)ifs.get();
      switch(c)
      {
      case '\n':
        XGlobal::GetLogger()->Log(XBasicLogger::kError,"Found newline in file_texture_name string.");
        return false;
      case '\"':
        if (readFirst){
          done=true;
        }
        readFirst=true;
        break;
      default:
        if (readFirst)
        {
          m_FileTextureName += c;
        }
      }
    }

    XU32 temp;
    READ_CHECK("filter_type",ifs);
    ifs >> temp;
    m_FilterType = (FilterType)temp;
    READ_CHECK("filter_amount",ifs);
    ifs >> m_FilterAmount;
    READ_CHECK("pre_filter",ifs);
    ifs >> m_PreFilter;
    READ_CHECK("pre_filter_radius",ifs);
    ifs >> m_PreFilterRadius;
  }

  return ifs.good();
}

//----------------------------------------------------------------------------------------------------------------------
bool XTexPlacement2D::WriteChunk(std::ostream& ofs)
{
  IO_CHECK( XShadingNode::WriteChunk(ofs) );
  ofs << "\timage " << m_ImageIndex << "\n";
  ofs << "\ttranslate_frame " << m_TranslateFrame[0] << " " << m_TranslateFrame[1] << "\n";
  ofs << "\trotate_frame " <<  m_RotateFrame << "\n";
  ofs << "\tmirror_u " <<  m_MirrorU << "\n";
  ofs << "\tmirror_v " << m_MirrorV << "\n";
  ofs << "\tstagger " << m_Stagger << "\n";
  ofs << "\twrap_u " << m_WrapU << "\n";
  ofs << "\twrap_v " << m_WrapV << "\n";
  ofs << "\trepeat_uv " << m_RepeatUV[0] << " " << m_RepeatUV[1] << "\n";
  ofs << "\toffset_uv " << m_OffsetUV[0] << " " << m_OffsetUV[1] << "\n";
  ofs << "\trotate_uv " << m_RotateUV << "\n";
  ofs << "\tnoise_uv " << m_NoiseUV[0] << " " << m_NoiseUV[1] << "\n";
  
  ofs << "\tfile_texture_name \"" << m_FileTextureName << "\"\n";
  ofs << "\tfilter_type " << ((XU32)m_FilterType) << "\n";
  ofs << "\tfilter_amount " << m_FilterAmount << "\n";
  ofs << "\tpre_filter " << m_PreFilter << "\n";
  ofs << "\tpre_filter_radius " << m_PreFilterRadius << "\n";

  return ofs.good();
}

//----------------------------------------------------------------------------------------------------------------------
bool XTexPlacement2D::DoData(XFileIO& io) 
{
  DUMPER(XTexPlacement2D);

  if( GetModel()->GetInputVersion() >= 4 )
  {
    IO_CHECK( XShadingNode::DoData(io) );
  }
  else
  {
    IO_CHECK( XBase::DoData(io) );
  }

  IO_CHECK( io.DoData(&m_ImageIndex) );
  DPARAM( m_ImageIndex );

  IO_CHECK( io.DoData(m_TranslateFrame,2) );
  DPARAM( m_TranslateFrame[0] );
  DPARAM( m_TranslateFrame[1] );

  IO_CHECK( io.DoData(&m_RotateFrame) );
  DPARAM( m_RotateFrame );

  IO_CHECK( io.DoData(&m_MirrorU) );
  DPARAM( m_MirrorU );

  IO_CHECK( io.DoData(&m_MirrorV) );
  DPARAM( m_MirrorV );

  IO_CHECK( io.DoData(&m_Stagger) );
  DPARAM( m_Stagger );

  IO_CHECK( io.DoData(&m_WrapU) );
  DPARAM( m_WrapU );

  IO_CHECK( io.DoData(&m_WrapV) );
  DPARAM( m_WrapV );

  IO_CHECK( io.DoData(m_RepeatUV,2) );
  DPARAM( m_RepeatUV[0] );
  DPARAM( m_RepeatUV[1] );

  IO_CHECK( io.DoData(m_OffsetUV,2) );
  DPARAM( m_OffsetUV[0] );
  DPARAM( m_OffsetUV[1] );

  IO_CHECK( io.DoData(&m_RotateUV) );
  DPARAM( m_RotateUV );

  IO_CHECK( io.DoData(m_NoiseUV,2) );
  DPARAM( m_NoiseUV[0] );
  DPARAM( m_NoiseUV[1] );

  if (io.IsWriting() || GetModel()->GetInputVersion()>=4)
  {
    IO_CHECK( io.DoDataString(m_FileTextureName) );
    DPARAM( m_FileTextureName );

    IO_CHECK( io.DoData(&m_FilterType) );
    DPARAM( m_FilterType );

    IO_CHECK( io.DoData(&m_FilterAmount) );
    DPARAM( m_FilterAmount );

    IO_CHECK( io.DoData(&m_PreFilter) );
    DPARAM( m_PreFilter );

    IO_CHECK( io.DoData(&m_PreFilterRadius) );
    DPARAM( m_PreFilterRadius );
  }

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
void XTexPlacement2D::DoCopy(const XBase* rhs)
{
  const XTexPlacement2D* cb = rhs->HasFn<XTexPlacement2D>();
  XMD_ASSERT(cb);

  m_ImageIndex = cb->m_ImageIndex;
  m_TranslateFrame[0] = cb->m_TranslateFrame[0];
  m_TranslateFrame[1] = cb->m_TranslateFrame[1];
  m_RotateFrame = cb->m_RotateFrame;
  m_MirrorU = cb->m_MirrorU;
  m_MirrorV = cb->m_MirrorV;
  m_Stagger = cb->m_Stagger;
  m_WrapU = cb->m_WrapU;
  m_WrapV = cb->m_WrapV;
  m_RepeatUV[0] = cb->m_RepeatUV[0];
  m_RepeatUV[1] = cb->m_RepeatUV[1];
  m_OffsetUV[0] = cb->m_OffsetUV[0];
  m_OffsetUV[1] = cb->m_OffsetUV[1];
  m_NoiseUV[0] = cb->m_NoiseUV[0];
  m_NoiseUV[1] = cb->m_NoiseUV[1];
  m_RotateUV = cb->m_RotateUV;
  m_FileTextureName = cb->m_FileTextureName;
  m_FilterType = cb->m_FilterType;
  m_FilterAmount = cb->m_FilterAmount;
  m_PreFilter = cb->m_PreFilter;
  m_PreFilterRadius = cb->m_PreFilterRadius;

  XShadingNode::DoCopy(cb);
}

//----------------------------------------------------------------------------------------------------------------------
XU32 XTexPlacement2D::GetDataSize() const 
{
  size_t sz = 0;
  if( GetModel()->GetInputVersion() >= 4 )
    sz = XShadingNode::GetDataSize();
  else
    sz = XBase::GetDataSize();
  sz +=  sizeof(XId) +
         6*sizeof(bool) +
         12*sizeof(XReal) +
         sizeof(XU16) +
         m_FileTextureName.size() +
         sizeof(FilterType);
return (XU32)sz;
}

//----------------------------------------------------------------------------------------------------------------------
void XTexPlacement2D::GetTranslateFrame(XReal& u,XReal& v) const
{
  u = m_TranslateFrame[0];
  v = m_TranslateFrame[1];
}

//----------------------------------------------------------------------------------------------------------------------
XReal XTexPlacement2D::GetTranslateFrameU() const 
{
  return m_TranslateFrame[0];
}

//----------------------------------------------------------------------------------------------------------------------
XReal XTexPlacement2D::GetTranslateFrameV() const 
{
  return m_TranslateFrame[1];
}

//----------------------------------------------------------------------------------------------------------------------
XReal XTexPlacement2D::GetRotateFrame() const 
{
  return m_RotateFrame;;
}

//----------------------------------------------------------------------------------------------------------------------
bool XTexPlacement2D::GetIsMirroredU() const
{
  return m_MirrorU;
}

//----------------------------------------------------------------------------------------------------------------------
bool XTexPlacement2D::GetIsMirroredV() const
{
  return m_MirrorV;;
}

//----------------------------------------------------------------------------------------------------------------------
bool XTexPlacement2D::GetIsStaggered() const
{
  return m_Stagger;
}

//----------------------------------------------------------------------------------------------------------------------
bool XTexPlacement2D::GetIsWrappedU() const 
{
  return m_WrapU;
}

//----------------------------------------------------------------------------------------------------------------------
bool XTexPlacement2D::GetIsWrappedV() const 
{
  return m_WrapV;
}

//----------------------------------------------------------------------------------------------------------------------
XReal XTexPlacement2D::GetRepeatU() const
{
  return m_RepeatUV[0];
}

//----------------------------------------------------------------------------------------------------------------------
XReal XTexPlacement2D::GetRepeatV() const 
{
  return m_RepeatUV[1];
}

//----------------------------------------------------------------------------------------------------------------------
XReal XTexPlacement2D::GetOffsetU() const 
{
  return m_OffsetUV[0];
}

//----------------------------------------------------------------------------------------------------------------------
XReal XTexPlacement2D::GetOffsetV() const 
{
  return m_OffsetUV[1];
}

//----------------------------------------------------------------------------------------------------------------------
XReal XTexPlacement2D::GetRotateUV() const
{
  return m_RotateUV;
}

//----------------------------------------------------------------------------------------------------------------------
XReal XTexPlacement2D::GetNoiseU() const 
{
  return m_NoiseUV[0];
}

//----------------------------------------------------------------------------------------------------------------------
XReal XTexPlacement2D::GetNoiseV() const
{
  return m_NoiseUV[1];
}

//----------------------------------------------------------------------------------------------------------------------
void XTexPlacement2D::SetTranslateFrameU(const XReal value)
{
  m_TranslateFrame[0] = value;
}

//----------------------------------------------------------------------------------------------------------------------
void XTexPlacement2D::SetTranslateFrameV(const XReal value)
{
  m_TranslateFrame[1] = value;
}

//----------------------------------------------------------------------------------------------------------------------
void XTexPlacement2D::SetRotateFrame(const XReal value)
{
  m_RotateFrame = value;
}

//----------------------------------------------------------------------------------------------------------------------
void XTexPlacement2D::SetRepeatU(const XReal value) 
{
  m_RepeatUV[0] = value;
}

//----------------------------------------------------------------------------------------------------------------------
void XTexPlacement2D::SetRepeatV(const XReal value)  
{
  m_RepeatUV[1] = value;
}

//----------------------------------------------------------------------------------------------------------------------
void XTexPlacement2D::SetOffsetU(const XReal value) 
{
  m_OffsetUV[0] = value;
}

//----------------------------------------------------------------------------------------------------------------------
void XTexPlacement2D::SetOffsetV(const XReal value)  
{
  m_OffsetUV[1] = value;
}

//----------------------------------------------------------------------------------------------------------------------
void XTexPlacement2D::SetRotateUV(const XReal value)  
{
  m_RotateUV = value;
}

//----------------------------------------------------------------------------------------------------------------------
void XTexPlacement2D::SetNoiseU(const XReal value)  
{
  m_NoiseUV[0] = value;
}

//----------------------------------------------------------------------------------------------------------------------
void XTexPlacement2D::SetNoiseV(const XReal value) 
{
  m_NoiseUV[1] = value;
}

//----------------------------------------------------------------------------------------------------------------------
void XTexPlacement2D::SetIsMirroredU(bool wrapped)
{
  m_MirrorU = wrapped;
}

//----------------------------------------------------------------------------------------------------------------------
void XTexPlacement2D::SetIsMirroredV(bool wrapped)
{
  m_MirrorV = wrapped;
}

//----------------------------------------------------------------------------------------------------------------------
void XTexPlacement2D::SetIsStaggered(bool wrapped)
{
  m_Stagger = wrapped;
}

//----------------------------------------------------------------------------------------------------------------------
void XTexPlacement2D::SetIsWrappedU(bool wrapped)
{
  m_WrapU = wrapped;
}

//----------------------------------------------------------------------------------------------------------------------
void XTexPlacement2D::SetIsWrappedV(bool wrapped)
{
  m_WrapV = wrapped;
}
}
