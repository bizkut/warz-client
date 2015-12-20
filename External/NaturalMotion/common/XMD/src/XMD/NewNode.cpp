//----------------------------------------------------------------------------------------------------------------------
/// \file NewNode.h
/// \note (C) Copyright 2003-2005 Robert Bateman. All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#include "XMD/NewNode.h"
#include "XMD/FileIO.h"

namespace XMD 
{
//----------------------------------------------------------------------------------------------------------------------
XFn::Type PxNode::GetApiType() const 
{  
  return XFn::PxNode; 
}  

//----------------------------------------------------------------------------------------------------------------------
XBase* PxNode::GetFn(XFn::Type type) 
{
  if(type==XFn::PxNode)
    return (PxNode*)this;
  return PxNewNode<XBase>::GetFn(type);
}

//----------------------------------------------------------------------------------------------------------------------
const XBase* PxNode::GetFn(XFn::Type type) const
{
  if(XFn::PxNode==type)
    return (const PxNode*)this;
  return PxNewNode<XBase>::GetFn(type);
}

//----------------------------------------------------------------------------------------------------------------------
PxNode::PxNode(XModel* pmod,PxNewNodeDescription* desc)
  : PxNewNode<XBase>(pmod,desc) 
{
}

//----------------------------------------------------------------------------------------------------------------------
XFn::Type PxMaterialNode::GetApiType() const 
{
  return XFn::PxMaterial; 
}

//----------------------------------------------------------------------------------------------------------------------
PxMaterialNode::PxMaterialNode(XModel* pmod,PxNewNodeDescription* desc)
  : PxNewNode<XMaterial>(pmod,desc) 
{
}

//----------------------------------------------------------------------------------------------------------------------
XBase* PxMaterialNode::GetFn(XFn::Type type) 
{
  if(type==XFn::PxMaterial)
    return (PxMaterialNode*)this;
  return PxNewNode<XMaterial>::GetFn(type);
}

//----------------------------------------------------------------------------------------------------------------------
const XBase* PxMaterialNode::GetFn(XFn::Type type) const
{
  if(XFn::PxMaterial==type)
    return (const PxMaterialNode*)this;
  return PxNewNode<XMaterial>::GetFn(type);
}

//----------------------------------------------------------------------------------------------------------------------
XFn::Type PxHwShaderNode::GetApiType() const 
{
  return XFn::PxHwShader; 
}

//----------------------------------------------------------------------------------------------------------------------
PxHwShaderNode::PxHwShaderNode(XModel* pmod,PxNewNodeDescription* desc)
  : PxNewNode<XHwShaderNode>(pmod,desc)
{
}

//----------------------------------------------------------------------------------------------------------------------
XBase* PxHwShaderNode::GetFn(XFn::Type type) 
{
  if(type==XFn::PxHwShader)
    return (PxHwShaderNode*)this;
  return PxNewNode<XHwShaderNode>::GetFn(type);
}

//----------------------------------------------------------------------------------------------------------------------
const XBase* PxHwShaderNode::GetFn(XFn::Type type) const
{
  if(type==XFn::PxHwShader)
    return (const PxHwShaderNode*)this;
  return PxNewNode<XHwShaderNode>::GetFn(type);
}

//----------------------------------------------------------------------------------------------------------------------
XFn::Type PxShapeNode::GetApiType() const 
{ 
  return XFn::PxShape; 
}

//----------------------------------------------------------------------------------------------------------------------
PxShapeNode::PxShapeNode(XModel* pmod,PxNewNodeDescription* desc)
  : PxNewNode<XShape>(pmod,desc)
{
}

//----------------------------------------------------------------------------------------------------------------------
XBase* PxShapeNode::GetFn(XFn::Type type) 
{
  if(type==XFn::PxShape)
    return (PxShapeNode*)this;
  return PxNewNode<XShape>::GetFn(type);
}

//----------------------------------------------------------------------------------------------------------------------
const XBase* PxShapeNode::GetFn(XFn::Type type) const
{
  if(type==XFn::PxShape)
    return (const PxShapeNode*)this;
  return PxNewNode<XShape>::GetFn(type);
}

//----------------------------------------------------------------------------------------------------------------------
XFn::Type PxGeometryNode::GetApiType() const 
{
  return XFn::PxGeometry; 
}

//----------------------------------------------------------------------------------------------------------------------
PxGeometryNode::PxGeometryNode(XModel* pmod,PxNewNodeDescription* desc)
  : PxNewNode<XGeometry>(pmod,desc) 
{
}

//----------------------------------------------------------------------------------------------------------------------
XBase* PxGeometryNode::GetFn(XFn::Type type) 
{
  if(type==XFn::PxGeometry)
    return (PxGeometryNode*)this;
  return PxNewNode<XGeometry>::GetFn(type);
}

//----------------------------------------------------------------------------------------------------------------------
const XBase* PxGeometryNode::GetFn(XFn::Type type) const
{
  if(type==XFn::PxGeometry)
    return (PxGeometryNode*)this;
  return PxNewNode<XGeometry>::GetFn(type);
}

//----------------------------------------------------------------------------------------------------------------------
XFn::Type PxLightNode::GetApiType() const
{
  return XFn::PxLight; 
}

//----------------------------------------------------------------------------------------------------------------------
PxLightNode::PxLightNode(XModel* pmod,PxNewNodeDescription* desc)
  : PxNewNode<XLight>(pmod,desc) 
{
}

//----------------------------------------------------------------------------------------------------------------------
XBase* PxLightNode::GetFn(XFn::Type type)
{
  if(type==XFn::PxLight)
    return (PxLightNode*)this;
  return PxNewNode<XLight>::GetFn(type);
}

//----------------------------------------------------------------------------------------------------------------------
const XBase* PxLightNode::GetFn(XFn::Type type) const
{
  if(type==XFn::PxLight)
    return (const PxLightNode*)this;
  return PxNewNode<XLight>::GetFn(type);
}

//----------------------------------------------------------------------------------------------------------------------
XFn::Type PxTransformNode::GetApiType() const 
{
  return XFn::PxTransform; 
}

//----------------------------------------------------------------------------------------------------------------------
PxTransformNode::PxTransformNode(XModel* pmod,PxNewNodeDescription* desc)
  : PxNewNode<XBone>(pmod,desc) 
{
}

//----------------------------------------------------------------------------------------------------------------------
XBase* PxTransformNode::GetFn(XFn::Type type) 
{
  if(type==XFn::PxTransform)
    return (PxTransformNode*)this;
  return PxNewNode<XBone>::GetFn(type);
}

//----------------------------------------------------------------------------------------------------------------------
const XBase* PxTransformNode::GetFn(XFn::Type type) const
{
  if(type==XFn::PxTransform)
    return (const PxTransformNode*)this;
  return PxNewNode<XBone>::GetFn(type);
}

//----------------------------------------------------------------------------------------------------------------------
XFn::Type PxEmitterNode::GetApiType() const 
{ 
  return XFn::PxEmitter; 
}

//----------------------------------------------------------------------------------------------------------------------
PxEmitterNode::PxEmitterNode(XModel* pmod,PxNewNodeDescription* desc)
  : PxNewNode<XEmitter>(pmod,desc) 
{
}

//----------------------------------------------------------------------------------------------------------------------
XBase* PxEmitterNode::GetFn(XFn::Type type) 
{
  if(type==XFn::PxEmitter)
    return (PxEmitterNode*)this;
  return PxNewNode<XEmitter>::GetFn(type);
}

//----------------------------------------------------------------------------------------------------------------------
const XBase* PxEmitterNode::GetFn(XFn::Type type) const
{
  if(type==XFn::PxEmitter)
    return (const PxEmitterNode*)this;
  return PxNewNode<XEmitter>::GetFn(type);
}

//----------------------------------------------------------------------------------------------------------------------
XFn::Type PxFieldNode::GetApiType() const 
{ 
  return XFn::PxField; 
}

//----------------------------------------------------------------------------------------------------------------------
PxFieldNode::PxFieldNode(XModel* pmod,PxNewNodeDescription* desc)
  : PxNewNode<XField>(pmod,desc) 
{
}

//----------------------------------------------------------------------------------------------------------------------
XBase* PxFieldNode::GetFn(XFn::Type type) 
{
  if(type==XFn::PxField)
    return (PxFieldNode*)this;
  return PxNewNode<XField>::GetFn(type);
}

//----------------------------------------------------------------------------------------------------------------------
const XBase* PxFieldNode::GetFn(XFn::Type type) const
{
  if(type==XFn::PxField)
    return (const PxFieldNode*)this;
  return PxNewNode<XField>::GetFn(type);
}

//----------------------------------------------------------------------------------------------------------------------
XFn::Type PxConstraintNode::GetApiType() const 
{
  return XFn::PxConstraint; 
}

//----------------------------------------------------------------------  PxConstraintNode :: PxConstraintNode
//
PxConstraintNode::PxConstraintNode(XModel* pmod,PxNewNodeDescription* desc) 
  : PxNewNode<XConstraint>(pmod,desc)
{
}

//----------------------------------------------------------------------------------------------------------------------
XBase* PxConstraintNode::GetFn(XFn::Type type)
{
  if(type==XFn::PxConstraint)
    return (PxConstraintNode*)this;
  return PxNewNode<XConstraint>::GetFn(type);
}

//----------------------------------------------------------------------------------------------------------------------
const XBase* PxConstraintNode::GetFn(XFn::Type type) const
{
  if(type==XFn::PxConstraint)
    return (PxConstraintNode*)this;
  return PxNewNode<XConstraint>::GetFn(type);
}

//----------------------------------------------------------------------------------------------------------------------
XFn::Type PxDeformerNode::GetApiType() const 
{ 
  return XFn::PxDeformer; 
}

//----------------------------------------------------------------------------------------------------------------------
PxDeformerNode::PxDeformerNode(XModel* pmod,PxNewNodeDescription* desc)
  : PxNewNode<XDeformer>(pmod,desc) 
{
}

//----------------------------------------------------------------------------------------------------------------------
XBase* PxDeformerNode::GetFn(XFn::Type type) 
{
  if(type==XFn::PxDeformer)
    return (PxDeformerNode*)this;
  return PxNewNode<XDeformer>::GetFn(type);
}

//----------------------------------------------------------------------------------------------------------------------
const XBase* PxDeformerNode::GetFn(XFn::Type type) const
{
  if(type==XFn::PxDeformer)
    return (const PxDeformerNode*)this;
  return PxNewNode<XDeformer>::GetFn(type);
}

//----------------------------------------------------------------------------------------------------------------------
XFn::Type PxAnimNode::GetApiType() const 
{
  return XFn::PxAnim; 
}

//----------------------------------------------------------------------------------------------------------------------
PxAnimNode::PxAnimNode(XModel* pmod,PxNewNodeDescription* desc)
  : PxNewNode<XBase>(pmod,desc) 
{
}

//----------------------------------------------------------------------------------------------------------------------
XBase* PxAnimNode::GetFn(XFn::Type type)
{
  if(type==XFn::PxAnim)
    return (PxDeformerNode*)this;
  return PxNewNode<XBase>::GetFn(type);
}

//----------------------------------------------------------------------------------------------------------------------
const XBase* PxAnimNode::GetFn(XFn::Type type) const
{
  if(type==XFn::PxAnim)
    return (const PxAnimNode*)this;
  return PxNewNode<XBase>::GetFn(type);
}

//----------------------------------------------------------------------------------------------------------------------
PxNewNodeDescription::PxNewNodeDescription() 
{
  m_BaseType = PX_NONE;
  m_TypeID = 0x1000;
  m_TypeName = "";
  m_ChunkHeader = "";
  m_Creator = 0;
}

//----------------------------------------------------------------------------------------------------------------------
PxNewNodeDescription::PxNewNodeDescription(const XChar* header,PxType base,PxCreator _create,XU32 type_id,const XChar* type) 
{
  m_BaseType = base;
  m_TypeID = type_id;
  m_TypeName = type;
  m_ChunkHeader = header;
  m_Creator = _create;
}

//----------------------------------------------------------------------------------------------------------------------
PxNewNodeDescription::~PxNewNodeDescription()
{
}

//----------------------------------------------------------------------------------------------------------------------
XBase* PxNewNodeDescription::Create(XModel* pMod) 
{
  if (m_Creator) 
  {
    return m_Creator(pMod,this);
  }
  switch(m_BaseType)
  {
  case PX_NONE:
    return new PxNode(pMod,this);
  case PX_MATERIAL:
    return new PxMaterialNode(pMod,this);
  case PX_OBJECT:
    return new PxShapeNode(pMod,this);
  case PX_LIGHT:
    return new PxLightNode(pMod,this);
  case PX_TRANSFORM:
    return new PxTransformNode(pMod,this);
  case PX_EMITTER:
    return new PxEmitterNode(pMod,this);
  case PX_FIELD:
    return new PxFieldNode(pMod,this);
//  case PX_CONSTRAINT:
//      return new PxConstraintNode(pMod,this);
  case PX_DEFORMER:
    return new PxDeformerNode(pMod,this);
  case PX_ANIM:
    return new PxAnimNode(pMod,this);
  case PX_HW_SHADER:
    return new PxHwShaderNode(pMod,this);
  default:
    break;
  }
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
bool PxNewNodeDescription::DoData(XFileIO& io)
{
  DUMPER(PxNewNodeDescription);

  CHECK_IO( io.DoDataString( m_ChunkHeader ) );
  DPARAM(m_ChunkHeader);

  CHECK_IO( io.DoDataString( m_TypeName ) );
  DPARAM(m_TypeName);

  CHECK_IO( io.DoData( &m_BaseType ) );
  DPARAM(m_BaseType);

  return true;

}

//----------------------------------------------------------------------------------------------------------------------
bool PxNewNodeDescription::DoHeader(XFileIO& io) 
{
  DUMPER(DoHeader);
  XChunkHeader header;
  memset(&header,0,sizeof(XChunkHeader));
  if(io.IsWriting())
  {
    header.Type = (XU16) XFn::NewFileFlag;
    header.HasExtraAttrs = 0;
    header.Reserved = 0;
    header.ByteSize = GetDataSize();
  }
  DPARAM( header.Type );
  DPARAM( header.HasExtraAttrs );
  DPARAM( header.ByteSize );
  CHECK_IO( io.DoData(&header) );
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
XU32 PxNewNodeDescription::GetDataSize() const 
{
  return static_cast<XU32>(4 + m_ChunkHeader.size() + m_TypeName.size() + sizeof(PxType) + sizeof(XChunkHeader));
}

//----------------------------------------------------------------------------------------------------------------------
const PxType PxNewNodeDescription::GetBaseType() const
{
  return m_BaseType;
}

//----------------------------------------------------------------------------------------------------------------------
const XChar* PxNewNodeDescription::GetChunkName() const
{
  return m_ChunkHeader.c_str();
}

//----------------------------------------------------------------------------------------------------------------------
const XChar* PxNewNodeDescription::GetTypeName() const
{
  return m_TypeName.c_str();
}

//----------------------------------------------------------------------------------------------------------------------
bool PxNewNodeDescription::Read(std::istream& ifs)
{
  XString temp;
  READ_CHECK("base_type",ifs);
  ifs >> temp;
  READ_CHECK("type_name",ifs);
  ifs >> m_TypeName;
  READ_CHECK("header",ifs);
  ifs >>m_ChunkHeader;
  READ_CHECK("}",ifs);

  if(temp=="material") m_BaseType = PX_MATERIAL;
  else
  if(temp=="object") m_BaseType = PX_OBJECT;
  else
  if(temp=="light") m_BaseType = PX_LIGHT;
  else
  if(temp=="transform") m_BaseType = PX_TRANSFORM;
  else
  if(temp=="emitter") m_BaseType = PX_EMITTER;
  else
  if(temp=="field") m_BaseType = PX_FIELD;
  else
  if(temp=="deformer") m_BaseType = PX_DEFORMER;
  else
  if(temp=="anim") m_BaseType = PX_ANIM;
  else
  if(temp=="hw_shader") m_BaseType = PX_HW_SHADER;
  else
  m_BaseType = PX_NONE;

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool PxNewNodeDescription::Write(std::ostream& ofs) 
{
  ofs << "NEW_NODE\n{\n\tbase_type ";
  switch(m_BaseType)
  {
  case PX_MATERIAL:
    ofs << "material\n";
    break;
  case PX_OBJECT:
    ofs << "object\n";
    break;
  case PX_LIGHT:
    ofs << "light\n";
    break;
  case PX_TRANSFORM:
    ofs << "transform\n";
    break;
  case PX_EMITTER:
    ofs << "emitter\n";
    break;
  case PX_FIELD:
    ofs << "field\n";
    break;
    //  case PX_CONSTRAINT:
    //  ofs << "constraint\n";
    // break;
  case PX_DEFORMER:
    ofs << "deformer\n";
    break;
  case PX_ANIM:
    ofs << "anim\n";
    break;
  case PX_HW_SHADER:
    ofs << "hw_shader\n";
    break;
  default:
    ofs << "none\n";
    break;
  }

  ofs << "\ttype_name " << m_TypeName.c_str() << "\n";
  ofs << "\theader " << m_ChunkHeader.c_str() << "\n";
  ofs << "}\n";

  return true;
}
}
