//----------------------------------------------------------------------------------------------------------------------
/// \file     NodeProperties.cpp
/// \note    (C) Copyright 2003-2005 Robert Bateman. All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#include "XMD/BaseProperties.h"
#include "XMD/CompoundProperties.h"
#include "XMD/NodeProperties.h"
#include "XMD/Model.h"
#include "XMD/FileIO.h"
#include "XMD/AnimCycle.h"
#include "XMD/Bone.h"
#include "XMD/Deformer.h"
#include "XMD/Factory.h"
#include "XMD/Info.h"
#include "XMD/Logger.h"
#include "XMD/ExtraAttributes.h"
#include "XMD/Geometry.h"
#include "XMD/Instance.h"
#include "XMD/Material.h"
#include "XMD/Texture.h"
#include "XMD/Air.h"
#include "XMD/AmbientLight.h"
#include "XMD/Anisotropic.h"
#include "XMD/AreaLight.h"
#include "XMD/BaseLattice.h"
#include "XMD/BezierCurve.h"
#include "XMD/BezierPatch.h"
#include "XMD/BlendShape.h"
#include "XMD/Blinn.h"
#include "XMD/BumpMap.h"
#include "XMD/Camera.h"
#include "XMD/Cluster.h"
#include "XMD/CollisionBox.h"
#include "XMD/CollisionCapsule.h"
#include "XMD/CollisionCone.h"
#include "XMD/CollisionCylinder.h"
#include "XMD/CollisionMesh.h"
#include "XMD/CollisionSphere.h"
#include "XMD/CubeMap.h"
#include "XMD/DirectionalLight.h"
#include "XMD/DisplayLayer.h"
#include "XMD/Drag.h"
#include "XMD/Emitter.h"
#include "XMD/EnvmapBaseTexture.h"
#include "XMD/FFD.h"
#include "XMD/Field.h"
#include "XMD/Gravity.h"
#include "XMD/HermiteCurve.h"
#include "XMD/HermitePatch.h"
#include "XMD/HwShaderNode.h"
#include "XMD/IkChain.h"
#include "XMD/Image.h"
#include "XMD/JiggleDeformer.h"
#include "XMD/JointCluster.h"
#include "XMD/Lambert.h"
#include "XMD/Lattice.h"
#include "XMD/LayeredShader.h"
#include "XMD/Light.h"
#include "XMD/Locator.h"
#include "XMD/LookAtConstraint.h"
#include "XMD/Mesh.h"
#include "XMD/MeshVertexColours.h"
#include "XMD/Newton.h"
#include "XMD/NodeCollection.h"
#include "XMD/NonLinearDeformer.h"
#include "XMD/NurbsCurve.h"
#include "XMD/NurbsSurface.h"
#include "XMD/ObjectSet.h"
#include "XMD/ParametricBase.h"
#include "XMD/ParametricCurve.h"
#include "XMD/ParametricSurface.h"
#include "XMD/ParticleShape.h"
#include "XMD/Phong.h"
#include "XMD/PhongE.h"
#include "XMD/PointLight.h"
#include "XMD/PoleVectorConstraint.h"
#include "XMD/Radial.h"
#include "XMD/RampShader.h"
#include "XMD/RenderLayer.h"
#include "XMD/SculptDeformer.h"
#include "XMD/SkinCluster.h"
#include "XMD/SphereMap.h"
#include "XMD/SpotLight.h"
#include "XMD/SurfaceShader.h"
#include "XMD/TexPlacement2D.h"
#include "XMD/TexPlacement3D.h"
#include "XMD/TrimData.h"
#include "XMD/Turbulence.h"
#include "XMD/Uniform.h"
#include "XMD/UseBackgroundShader.h"
#include "XMD/VolumeAxis.h"
#include "XMD/VolumeBox.h"
#include "XMD/VolumeCone.h"
#include "XMD/VolumeCylinder.h"
#include "XMD/VolumeLight.h"
#include "XMD/VolumeSphere.h"
#include "XMD/Vortex.h"
#include "XMD/WireDeformer.h"
#include "XMD/WrapDeformer.h"
#include "XMD/Constraint.h"
#include "XMD/AimConstraint.h"
#include "XMD/GeometryConstraint.h"
#include "XMD/NormalConstraint.h"
#include "XMD/OrientConstraint.h"
#include "XMD/ParentConstraint.h"
#include "XMD/PointConstraint.h"
#include "XMD/ScaleConstraint.h"
#include "XMD/TangentConstraint.h"

#include "XMD/AnimationTake.h"
#include "XMD/Mesh.h"
#include "XMD/PolygonMesh.h"
#include "XMD/AreaLight.h"
#include "XMD/DisplayLayer.h"
#include "XMD/RenderLayer.h"
#include "XMD/ParametricBase.h"
#include "XMD/HermiteCurve.h"
#include "XMD/HermitePatch.h"
#include "XMD/BezierCurve.h"
#include "XMD/BezierPatch.h"
#include "XMD/LayeredTexture.h"
#include "XMD/Locator.h"
#include "XMD/TexPlacement3D.h"
#include "XMD/UseBackgroundShader.h"
#include "XMD/HwShaderNode.h"

namespace XMD
{

#define PROPERTY_COUNT(XTYPE,BASE_XTYPE) virtual XU32 GetNumProperties() const { return XTYPE ::kNumProperties + BASE_XTYPE ::GetNumProperties(); } \
                                         virtual XString GetPropertyName(XU32); \
                                         virtual XPropertyType::Type GetPropertyType(XU32); \
                                         virtual XU32 GetPropertyID(const XString&); \
                                         virtual XU32 GetPropertyFlags(XU32); \
                                         virtual XFn::Type GetPropertyNodeType(XU32);
#define PROPERTY_TYPE(_type_) virtual bool SetProperty(XBase* node,XU32,const _type_ &); \
                              virtual bool GetProperty(XBase* node,XU32,_type_ &); 

XBaseProperties* XAnimationTake::GetProperties() const
{
  return XBase::GetProperties();
}
XBaseProperties* XMesh::GetProperties() const 
{
  return XGeometry::GetProperties();
}
XBaseProperties* XPolygonMesh::GetProperties() const 
{
  return XGeometry::GetProperties();
}
XBaseProperties* XAreaLight::GetProperties() const 
{
  return XExtendedLight::GetProperties();
}
XBaseProperties* XDisplayLayer::GetProperties() const 
{
  return XNodeCollection::GetProperties();
}
XBaseProperties* XRenderLayer::GetProperties() const 
{
  return XNodeCollection::GetProperties();
}
XBaseProperties* XParametricBase::GetProperties() const 
{
  return XGeometry::GetProperties();
}
XBaseProperties* XHermiteCurve::GetProperties() const 
{
  return XParametricCurve::GetProperties();
}
XBaseProperties* XHermitePatch::GetProperties() const 
{
  return XParametricSurface::GetProperties();
}
XBaseProperties* XBezierCurve::GetProperties() const 
{
  return XParametricCurve::GetProperties();
}
XBaseProperties* XBezierPatch::GetProperties() const 
{
  return XParametricSurface::GetProperties();
}
XBaseProperties* XLayeredTexture::GetProperties() const 
{
  return XTexture::GetProperties();
}
XBaseProperties* XLocator::GetProperties() const 
{
  return XShape::GetProperties();
}
XBaseProperties* XTexPlacement3D::GetProperties() const 
{
  return XBone::GetProperties();
}
XBaseProperties* XUseBackground::GetProperties() const 
{
  return XMaterial::GetProperties();
}
XBaseProperties* XShadingNode::GetProperties() const 
{
  return XBase::GetProperties();
}
XBaseProperties* XHwShaderNode::GetProperties() const 
{
  return XBase::GetProperties();
}
XBaseProperties* XParentConstraint::GetProperties() const 
{
  return XConstraint::GetProperties();
}
XBaseProperties* XGeometryConstraint::GetProperties() const 
{
  return XConstraint::GetProperties();
}
XBaseProperties* XRampShader::GetProperties() const 
{
  return XSurfaceShader::GetProperties();
}
XBaseProperties* XPointLight::GetProperties() const 
{
  return XExtendedLight::GetProperties();
}
XBaseProperties* XPhong::GetProperties() const 
{
  return XSpecular::GetProperties();
}
XBaseProperties* XMeshVertexColours::GetProperties() const 
{
  return XBase::GetProperties();
}

struct XPropertyInfo
{
  /// the data type of the property
  XPropertyType::Type m_Type;
  /// the name of the property
  XString m_PropertyName;
  /// the ID of the property
  XU32 m_PropertyID;
  /// is the property animatable
  XU32 m_Flags;
  /// the type of the XBase* property
  XFn::Type m_NodeType;
};


#define DECLARE_NODE_PROPERTIES(NODE_TYPE) NODE_TYPE##Properties g##NODE_TYPE##Properties; \
  XBaseProperties* NODE_TYPE ::GetProperties() const { return &g##NODE_TYPE##Properties; }

#define IMPLEMENT_PROPERTY_COMMON(XTYPE,XBASE) \
  XString XTYPE ::GetPropertyName(const XU32 property_id){ \
    const XPropertyInfo* p = XTYPE ::m_Properties; \
    const XPropertyInfo* end = (XTYPE ::m_Properties) + (XTYPE ::kNumProperties); \
    for (;p!=end;++p) { \
      if(property_id==p->m_PropertyID) \
        return p->m_PropertyName; \
    } \
    return XBASE ::GetPropertyName(property_id); \
  } \
  XPropertyType::Type XTYPE ::GetPropertyType(const XU32 property_id) { \
    const XPropertyInfo* p = XTYPE ::m_Properties; \
    const XPropertyInfo* end = (XTYPE ::m_Properties) + (XTYPE ::kNumProperties); \
    for (;p!=end;++p) { \
      if(property_id==p->m_PropertyID) \
        return p->m_Type; \
    } \
    return XBASE ::GetPropertyType(property_id); \
  } \
  XU32 XTYPE ::GetPropertyID(const XString& property_name) { \
    const XPropertyInfo* p = XTYPE ::m_Properties; \
    const XPropertyInfo* end = (XTYPE ::m_Properties) + (XTYPE ::kNumProperties); \
    for (;p!=end;++p) { \
      if(property_name==p->m_PropertyName) \
        return p->m_PropertyID; \
    } \
    return XBASE ::GetPropertyID(property_name); \
  } \
  XU32 XTYPE ::GetPropertyFlags(const XU32 property_id) { \
    const XPropertyInfo* p = (XTYPE ::m_Properties); \
    const XPropertyInfo* end = (XTYPE ::m_Properties) + (XTYPE ::kNumProperties); \
    for (;p!=end;++p) { \
      if(property_id==p->m_PropertyID) \
        return p->m_Flags; \
    } \
    return XBASE ::GetPropertyFlags(property_id); \
  } \
  XFn::Type XTYPE ::GetPropertyNodeType(const XU32 property_id) { \
    const XPropertyInfo* p = (XTYPE ::m_Properties); \
    const XPropertyInfo* end = (XTYPE ::m_Properties) + (XTYPE ::kNumProperties); \
    for (;p!=end;++p) { \
      if(property_id==p->m_PropertyID) \
      { \
        if( (p->m_Type == XPropertyType::kNode) || (p->m_Type == XPropertyType::kNodeArray) ) \
          return p->m_NodeType; \
        return XFn::Base; \
      } \
    } \
    return XBASE ::GetPropertyNodeType(property_id); \
  } 


#define DO_TYPE_CHECK(NODE,TYPE) TYPE* object = (NODE)->HasFn<TYPE>(); if(!object) return false;

//----------------------------------------------------------------------------------------------------------------------
class XAnimCycleProperties : public XBaseProperties
{
  static const XU32 kNumProperties=4;
  /// the properties for the bone
  static XPropertyInfo m_Properties[kNumProperties];
public:
  PROPERTY_COUNT(XAnimCycleProperties,XBaseProperties);
  PROPERTY_TYPE(XReal);
  PROPERTY_TYPE(XU32);
};
XPropertyInfo XAnimCycleProperties::m_Properties[4] = 
{
  {XPropertyType::kReal,"start_frame",XAnimCycle::kStartFrame,0},
  {XPropertyType::kReal,"end_frame",XAnimCycle::kEndFrame,0},
  {XPropertyType::kReal,"frames_per_second",XAnimCycle::kFramesPerSecond,0},
  {XPropertyType::kUnsigned,"num_frames",XAnimCycle::kNumFrames,0}
};

DECLARE_NODE_PROPERTIES(XAnimCycle)
IMPLEMENT_PROPERTY_COMMON(XAnimCycleProperties,XBaseProperties);

bool XAnimCycleProperties::SetProperty(XBase* node,const XU32 property_id,const XU32& val)
{
  DO_TYPE_CHECK(node,XAnimCycle);
  switch(property_id)
  {
  case XAnimCycle::kNumFrames: object->SetFrameTimes(object->GetStartTime(),object->GetStartTime()+val,val); return true;
  default: break;
  }
  return XBaseProperties::SetProperty(node,property_id,val);
}

bool XAnimCycleProperties::SetProperty(XBase* node,const XU32 property_id,const XReal& val)
{
  DO_TYPE_CHECK(node,XAnimCycle);
  switch(property_id)
  {
  case XAnimCycle::kStartFrame:      object->SetFrameTimes(val,val+object->GetNumFrames(),object->GetNumFrames()); return true;
  case XAnimCycle::kEndFrame:        object->SetFrameTimes(val-object->GetNumFrames(),val,object->GetNumFrames()); return true;
  case XAnimCycle::kFramesPerSecond: object->SetFrameRate(val); return true;
  default: break;
  }
  return XBaseProperties::SetProperty(node,property_id,val);
}

bool XAnimCycleProperties::GetProperty(XBase* node,const XU32 property_id,XU32& val)
{
  DO_TYPE_CHECK(node,XAnimCycle);
  switch(property_id)
  {
  case XAnimCycle::kNumFrames: val = object->GetNumFrames(); return true;
  default: break;
  }
  return XBaseProperties::GetProperty(node,property_id,val);
}

bool XAnimCycleProperties::GetProperty(XBase* node,const XU32 property_id,XReal& val)
{
  DO_TYPE_CHECK(node,XAnimCycle);
  switch(property_id)
  {
  case XAnimCycle::kStartFrame:      val = object->GetStartTime(); return true;
  case XAnimCycle::kEndFrame:        val = object->GetEndTime(); return true;
  case XAnimCycle::kFramesPerSecond: val = object->GetFramesPerSecond(); return true;
  default: break;
  }
  return XBaseProperties::SetProperty(node,property_id,val);
}

//----------------------------------------------------------------------------------------------------------------------
class XBoneProperties : public XBaseProperties
{
  static const XU32 kNumProperties=55;
  /// the properties for the bone
  static XPropertyInfo m_Properties[kNumProperties];
public:
  PROPERTY_COUNT(XBoneProperties,XBaseProperties);
  PROPERTY_TYPE(bool);
  PROPERTY_TYPE(XU32);
  PROPERTY_TYPE(XReal);
  PROPERTY_TYPE(XQuaternion);
  PROPERTY_TYPE(XVector3);
  PROPERTY_TYPE(XMatrix);
  PROPERTY_TYPE(XList);
  PROPERTY_TYPE(XBase*);
};
XPropertyInfo XBoneProperties::m_Properties[55] = 
{
  {XPropertyType::kQuaternion,"rotation",XBone::kRotation,XPropertyFlags::kAnimatable},
  {XPropertyType::kVector3,"scale",XBone::kScale,XPropertyFlags::kAnimatable},
  {XPropertyType::kVector3,"translate",XBone::kTranslation,XPropertyFlags::kAnimatable},
  {XPropertyType::kBool,"visible",XBone::kVisibility,XPropertyFlags::kAnimatable},
  {XPropertyType::kQuaternion,"joint_orient",XBone::kJointOrient,0},
  {XPropertyType::kQuaternion,"rotation_orient",XBone::kRotationOrient,0},
  {XPropertyType::kVector3,"rotate_pivot",XBone::kRotatePivot,0},
  {XPropertyType::kVector3,"scale_pivot",XBone::kScalePivot,0},
  {XPropertyType::kVector3,"min_bound",XBone::kBoundingBoxMin,XPropertyFlags::kReadOnly},
  {XPropertyType::kVector3,"max_bound",XBone::kBoundingBoxMax,XPropertyFlags::kReadOnly},
  {XPropertyType::kMatrix,"local",XBone::kLocal,XPropertyFlags::kReadOnly},
  {XPropertyType::kMatrix,"world",XBone::kWorld,XPropertyFlags::kReadOnly},
  {XPropertyType::kMatrix,"inverse",XBone::kInverse,XPropertyFlags::kReadOnly},
  {XPropertyType::kMatrix,"inverse_transpose",XBone::kInverseTranspose,XPropertyFlags::kReadOnly},
  {XPropertyType::kNodeArray,"children",XBone::kChildren,XPropertyFlags::kArrayIsResizable,XFn::Bone},
  {XPropertyType::kNode,"parent",XBone::kParent,XPropertyFlags::kReadOnly,XFn::Bone},
  {XPropertyType::kVector3,"shearing",XBone::kShearing,XPropertyFlags::kAnimatable},
  {XPropertyType::kUnsigned,"rotate_order",XBone::kRotationOrder,0},
  {XPropertyType::kBool,"inherits_transform",XBone::kInheritsTransform,0},
  {XPropertyType::kBool,"has_min_translateX_limit",XBone::kHasMinTranslateXLimit,XPropertyFlags::kReadOnly},
  {XPropertyType::kBool,"has_min_translateY_limit",XBone::kHasMinTranslateYLimit,XPropertyFlags::kReadOnly},
  {XPropertyType::kBool,"has_min_translateZ_limit",XBone::kHasMinTranslateZLimit,XPropertyFlags::kReadOnly},
  {XPropertyType::kBool,"has_max_translateX_limit",XBone::kHasMaxTranslateXLimit,XPropertyFlags::kReadOnly},
  {XPropertyType::kBool,"has_max_translateY_limit",XBone::kHasMaxTranslateYLimit,XPropertyFlags::kReadOnly},
  {XPropertyType::kBool,"has_max_translateZ_limit",XBone::kHasMaxTranslateZLimit,XPropertyFlags::kReadOnly},
  {XPropertyType::kBool,"has_min_rotateX_limit",XBone::kHasMinRotateXLimit,XPropertyFlags::kReadOnly},
  {XPropertyType::kBool,"has_min_rotateY_limit",XBone::kHasMinRotateYLimit,XPropertyFlags::kReadOnly},
  {XPropertyType::kBool,"has_min_rotateZ_limit",XBone::kHasMinRotateZLimit,XPropertyFlags::kReadOnly},
  {XPropertyType::kBool,"has_max_rotateX_limit",XBone::kHasMaxRotateXLimit,XPropertyFlags::kReadOnly},
  {XPropertyType::kBool,"has_max_rotateY_limit",XBone::kHasMaxRotateYLimit,XPropertyFlags::kReadOnly},
  {XPropertyType::kBool,"has_max_rotateZ_limit",XBone::kHasMaxRotateZLimit,XPropertyFlags::kReadOnly},
  {XPropertyType::kBool,"has_min_scaleX_limit",XBone::kHasMinScaleXLimit,XPropertyFlags::kReadOnly},
  {XPropertyType::kBool,"has_min_scaleY_limit",XBone::kHasMinScaleYLimit,XPropertyFlags::kReadOnly},
  {XPropertyType::kBool,"has_min_scaleZ_limit",XBone::kHasMinScaleZLimit,XPropertyFlags::kReadOnly},
  {XPropertyType::kBool,"has_max_scaleX_limit",XBone::kHasMaxScaleXLimit,XPropertyFlags::kReadOnly},
  {XPropertyType::kBool,"has_max_scaleY_limit",XBone::kHasMaxScaleYLimit,XPropertyFlags::kReadOnly},
  {XPropertyType::kBool,"has_max_scaleZ_limit",XBone::kHasMaxScaleZLimit,XPropertyFlags::kReadOnly},
  {XPropertyType::kReal,"min_translateX_limit",XBone::kMinTranslateXLimit,0},
  {XPropertyType::kReal,"min_translateY_limit",XBone::kMinTranslateYLimit,0},
  {XPropertyType::kReal,"min_translateZ_limit",XBone::kMinTranslateZLimit,0},
  {XPropertyType::kReal,"max_translateX_limit",XBone::kMaxTranslateXLimit,0},
  {XPropertyType::kReal,"max_translateY_limit",XBone::kMaxTranslateYLimit,0},
  {XPropertyType::kReal,"max_translateZ_limit",XBone::kMaxTranslateZLimit,0},
  {XPropertyType::kReal,"min_rotateX_limit",XBone::kMinRotateXLimit,0},
  {XPropertyType::kReal,"min_rotateY_limit",XBone::kMinRotateYLimit,0},
  {XPropertyType::kReal,"min_rotateZ_limit",XBone::kMinRotateZLimit,0},
  {XPropertyType::kReal,"max_rotateX_limit",XBone::kMaxRotateXLimit,0},
  {XPropertyType::kReal,"max_rotateY_limit",XBone::kMaxRotateYLimit,0},
  {XPropertyType::kReal,"max_rotateZ_limit",XBone::kMaxRotateZLimit,0},
  {XPropertyType::kReal,"min_scaleX_limit",XBone::kMinScaleXLimit,0},
  {XPropertyType::kReal,"min_scaleY_limit",XBone::kMinScaleYLimit,0},
  {XPropertyType::kReal,"min_scaleZ_limit",XBone::kMinScaleZLimit,0},
  {XPropertyType::kReal,"max_scaleX_limit",XBone::kMaxScaleXLimit,0},
  {XPropertyType::kReal,"max_scaleY_limit",XBone::kMaxScaleYLimit,0},
  {XPropertyType::kReal,"max_scaleZ_limit",XBone::kMaxScaleZLimit,0},
};

DECLARE_NODE_PROPERTIES(XBone)
IMPLEMENT_PROPERTY_COMMON(XBoneProperties,XBaseProperties);

bool XBoneProperties::SetProperty(XBase* node,const XU32 property_id,const XReal& val)
{
  DO_TYPE_CHECK(node,XBone);
  switch(property_id)
  {
  case XBone::kMinTranslateXLimit: object->SetLimit(XBone::kTranslateMinX,val); return true;
  case XBone::kMinTranslateYLimit: object->SetLimit(XBone::kTranslateMinY,val); return true;
  case XBone::kMinTranslateZLimit: object->SetLimit(XBone::kTranslateMinZ,val); return true;
  case XBone::kMaxTranslateXLimit: object->SetLimit(XBone::kTranslateMaxX,val); return true;
  case XBone::kMaxTranslateYLimit: object->SetLimit(XBone::kTranslateMaxY,val); return true;
  case XBone::kMaxTranslateZLimit: object->SetLimit(XBone::kTranslateMaxZ,val); return true;

  case XBone::kMinRotateXLimit: object->SetLimit(XBone::kRotateMinX,val); return true;
  case XBone::kMinRotateYLimit: object->SetLimit(XBone::kRotateMinY,val); return true;
  case XBone::kMinRotateZLimit: object->SetLimit(XBone::kRotateMinZ,val); return true;
  case XBone::kMaxRotateXLimit: object->SetLimit(XBone::kRotateMaxX,val); return true;
  case XBone::kMaxRotateYLimit: object->SetLimit(XBone::kRotateMaxY,val); return true;
  case XBone::kMaxRotateZLimit: object->SetLimit(XBone::kRotateMaxZ,val); return true;

  case XBone::kMinScaleXLimit: object->SetLimit(XBone::kScaleMinX,val); return true;
  case XBone::kMinScaleYLimit: object->SetLimit(XBone::kScaleMinY,val); return true;
  case XBone::kMinScaleZLimit: object->SetLimit(XBone::kScaleMinZ,val); return true;
  case XBone::kMaxScaleXLimit: object->SetLimit(XBone::kScaleMaxX,val); return true;
  case XBone::kMaxScaleYLimit: object->SetLimit(XBone::kScaleMaxY,val); return true;
  case XBone::kMaxScaleZLimit: object->SetLimit(XBone::kScaleMaxZ,val); return true;
  default: break;
  }
  return XBaseProperties::SetProperty(node,property_id,val);
}

bool XBoneProperties::SetProperty(XBase* node,const XU32 property_id,const XU32& val)
{
  DO_TYPE_CHECK(node,XBone);
  switch(property_id)
  {
  case XBone::kRotationOrder: object->SetRotationOrder((XBone::RotationOrder)val); return true;
  default: break;
  }
  return XBaseProperties::SetProperty(node,property_id,val);
}

bool XBoneProperties::SetProperty(XBase* node,const XU32 property_id,const XQuaternion& val)
{
  DO_TYPE_CHECK(node,XBone);
  switch(property_id)
  {
  case XBone::kRotation: object->SetRotation(val); return true;
  case XBone::kJointOrient: object->SetJointOrient(val); return true;
  case XBone::kRotationOrient: object->SetRotationOrient(val); return true;
  default: break;
  }
  return XBaseProperties::SetProperty(node,property_id,val);
}

bool XBoneProperties::SetProperty(XBase* node,const XU32 property_id,const XVector3& val)
{
  DO_TYPE_CHECK(node,XBone);
  switch(property_id)
  {
  case XBone::kScale:       object->SetScale(val);       return true;
  case XBone::kTranslation: object->SetTranslation(val); return true;
  case XBone::kRotatePivot: object->SetRotatePivot(val); return true;
  case XBone::kScalePivot:  object->SetScalePivot(val);  return true;
  default: break;
  }
  return XBaseProperties::SetProperty(node,property_id,val);
}


bool XBoneProperties::SetProperty(XBase* node,const XU32 property_id,const bool& val)
{
  DO_TYPE_CHECK(node,XBone);
  switch(property_id)
  {
  case XBone::kVisibility:        object->SetVisible(val);           return true;
  case XBone::kInheritsTransform: object->SetInheritsTransform(val); return true;
  default: break;
  }
  return XBaseProperties::SetProperty(node,property_id,val);
}

bool XBoneProperties::SetProperty(XBase* node,const XU32 property_id,const XList& val)
{
  DO_TYPE_CHECK(node,XBone);
  switch(property_id)
  {
  case XBone::kChildren: 
    {
      XBoneList bones;
      XList::const_iterator it = val.begin();
      for (;it != val.end(); ++it)
      {
        XBase* b = *it;
        if(b->HasFn<XBone>())
        {
          bones.push_back( b->HasFn<XBone>() );
        }
      }
      object->SetChildren(bones);
    }
    return true;
  default: break;
  }
  return XBaseProperties::SetProperty(node,property_id,val);
}

bool XBoneProperties::SetProperty(XBase* node,const XU32 property_id,const XBase*& val)
{
  DO_TYPE_CHECK(node,XBone);
  switch(property_id)
  {
  case XBone::kParent: return false;
  default: break;
  }
  return XBaseProperties::SetProperty(node,property_id,val);
}

bool XBoneProperties::SetProperty(XBase* node,const XU32 property_id,const XMatrix& val)
{
  DO_TYPE_CHECK(node,XBone);
  return XBaseProperties::SetProperty(node,property_id,val);
}

bool XBoneProperties::GetProperty(XBase* node,const XU32 property_id,XU32& val)
{
  DO_TYPE_CHECK(node,XBone);
  switch(property_id)
  {
  case XBone::kRotationOrder: val = (XU32)object->GetRotationOrder(); return true;
  default: break;
  }
  return XBaseProperties::GetProperty(node,property_id,val);
}

bool XBoneProperties::GetProperty(XBase* node,const XU32 property_id,XQuaternion& val)
{
  DO_TYPE_CHECK(node,XBone);
  switch(property_id)
  {
  case XBone::kRotation: val       = object->GetRotation(false); return true;
  case XBone::kJointOrient: val    = object->GetJointOrient(); return true;
  case XBone::kRotationOrient: val = object->GetRotationOrient(); return true;
  default: break;
  }
  return XBaseProperties::GetProperty(node,property_id,val);
}

bool XBoneProperties::GetProperty(XBase* node,const XU32 property_id,XVector3& val)
{
  DO_TYPE_CHECK(node,XBone);
  switch(property_id)
  {
  case XBone::kScale:          val = object->GetScale(false);       return true;
  case XBone::kTranslation:    val = object->GetTranslation(false); return true;
  case XBone::kRotatePivot:    val = object->GetRotatePivot();      return true;
  case XBone::kScalePivot:     val = object->GetScalePivot();       return true;
  case XBone::kBoundingBoxMin: val = object->GetBoundingBoxMin();   return true;
  case XBone::kBoundingBoxMax: val = object->GetBoundingBoxMax();   return true;
  default: break;
  }
  return XBaseProperties::GetProperty(node,property_id,val);
}

bool XBoneProperties::GetProperty(XBase* node,const XU32 property_id,XReal& val)
{
  DO_TYPE_CHECK(node,XBone);
  switch(property_id)
  {
  case XBone::kMinTranslateXLimit: val = object->GetLimitValue(XBone::kTranslateMinX); return true;
  case XBone::kMinTranslateYLimit: val = object->GetLimitValue(XBone::kTranslateMinY); return true;
  case XBone::kMinTranslateZLimit: val = object->GetLimitValue(XBone::kTranslateMinZ); return true;
  case XBone::kMaxTranslateXLimit: val = object->GetLimitValue(XBone::kTranslateMaxX); return true;
  case XBone::kMaxTranslateYLimit: val = object->GetLimitValue(XBone::kTranslateMaxY); return true;
  case XBone::kMaxTranslateZLimit: val = object->GetLimitValue(XBone::kTranslateMaxZ); return true;

  case XBone::kMinRotateXLimit: val = object->GetLimitValue(XBone::kRotateMinX); return true;
  case XBone::kMinRotateYLimit: val = object->GetLimitValue(XBone::kRotateMinY); return true;
  case XBone::kMinRotateZLimit: val = object->GetLimitValue(XBone::kRotateMinZ); return true;
  case XBone::kMaxRotateXLimit: val = object->GetLimitValue(XBone::kRotateMaxX); return true;
  case XBone::kMaxRotateYLimit: val = object->GetLimitValue(XBone::kRotateMaxY); return true;
  case XBone::kMaxRotateZLimit: val = object->GetLimitValue(XBone::kRotateMaxZ); return true;

  case XBone::kMinScaleXLimit: val = object->GetLimitValue(XBone::kScaleMinX); return true;
  case XBone::kMinScaleYLimit: val = object->GetLimitValue(XBone::kScaleMinY); return true;
  case XBone::kMinScaleZLimit: val = object->GetLimitValue(XBone::kScaleMinZ); return true;
  case XBone::kMaxScaleXLimit: val = object->GetLimitValue(XBone::kScaleMaxX); return true;
  case XBone::kMaxScaleYLimit: val = object->GetLimitValue(XBone::kScaleMaxY); return true;
  case XBone::kMaxScaleZLimit: val = object->GetLimitValue(XBone::kScaleMaxZ); return true;
  default: break;
  }
  return XBaseProperties::SetProperty(node,property_id,val);
}

bool XBoneProperties::GetProperty(XBase* node,const XU32 property_id,bool& val)
{
  DO_TYPE_CHECK(node,XBone);
  switch(property_id)
  {
  case XBone::kVisibility:            val = object->IsVisible();            return true;
  case XBone::kInheritsTransform:     val = object->GetInheritsTransform(); return true;
  case XBone::kHasMinTranslateXLimit: val = object->HasLimit(XBone::kTranslateMinX); return true;
  case XBone::kHasMinTranslateYLimit: val = object->HasLimit(XBone::kTranslateMinY); return true;
  case XBone::kHasMinTranslateZLimit: val = object->HasLimit(XBone::kTranslateMinZ); return true;
  case XBone::kHasMinScaleXLimit:     val = object->HasLimit(XBone::kScaleMinX); return true;
  case XBone::kHasMinScaleYLimit:     val = object->HasLimit(XBone::kScaleMinY); return true;
  case XBone::kHasMinScaleZLimit:     val = object->HasLimit(XBone::kScaleMinZ); return true;
  case XBone::kHasMinRotateXLimit:    val = object->HasLimit(XBone::kRotateMinX); return true;
  case XBone::kHasMinRotateYLimit:    val = object->HasLimit(XBone::kRotateMinY); return true;
  case XBone::kHasMinRotateZLimit:    val = object->HasLimit(XBone::kRotateMinZ); return true;
  case XBone::kHasMaxTranslateXLimit: val = object->HasLimit(XBone::kTranslateMaxX); return true;
  case XBone::kHasMaxTranslateYLimit: val = object->HasLimit(XBone::kTranslateMaxY); return true;
  case XBone::kHasMaxTranslateZLimit: val = object->HasLimit(XBone::kTranslateMaxZ); return true;
  case XBone::kHasMaxScaleXLimit:     val = object->HasLimit(XBone::kScaleMaxX); return true;
  case XBone::kHasMaxScaleYLimit:     val = object->HasLimit(XBone::kScaleMaxY); return true;
  case XBone::kHasMaxScaleZLimit:     val = object->HasLimit(XBone::kScaleMaxZ); return true;
  case XBone::kHasMaxRotateXLimit:    val = object->HasLimit(XBone::kRotateMaxX); return true;
  case XBone::kHasMaxRotateYLimit:    val = object->HasLimit(XBone::kRotateMaxY); return true;
  case XBone::kHasMaxRotateZLimit:    val = object->HasLimit(XBone::kRotateMaxZ); return true;
  default: break;
  }
  return XBaseProperties::SetProperty(node,property_id,val);
}

bool XBoneProperties::GetProperty(XBase* node,const XU32 property_id,XMatrix& val)
{
  DO_TYPE_CHECK(node,XBone);
  switch(property_id)
  {
  case XBone::kLocal:            val = object->GetLocal();            return true;
  case XBone::kWorld:            val = object->GetWorld();            return true;
  case XBone::kInverse:          val = object->GetInverse();          return true;
  case XBone::kInverseTranspose: val = object->GetInverseTranspose(); return true;
  default: break;
  }
  return XBaseProperties::SetProperty(node,property_id,val);
}

bool XBoneProperties::GetProperty(XBase* node,const XU32 property_id,XList& val)
{
  DO_TYPE_CHECK(node,XBone);
  switch(property_id)
  {
  case XBone::kChildren: 
    {
      XBoneList bones;
      object->GetChildren(bones);
      val.clear();

      XBoneList::iterator it = bones.begin();
      for (;it != bones.end(); ++it)
      {
        val.push_back(*it);
      }
    }
    return true;
  default: break;
  }
  return XBaseProperties::GetProperty(node,property_id,val);
}

bool XBoneProperties::GetProperty(XBase* node,const XU32 property_id,XBase*& val)
{
  DO_TYPE_CHECK(node,XBone);
  switch(property_id)
  {
  case XBone::kParent: val = object->GetParent(); return true;
  default: break;
  }
  return XBaseProperties::GetProperty(node,property_id,val);
}

//----------------------------------------------------------------------------------------------------------------------
class XJointProperties : public XBoneProperties
{
  static const XU32 kNumProperties=1;
  static XPropertyInfo m_Properties[kNumProperties];
public:
  PROPERTY_COUNT(XJointProperties,XBoneProperties);
  PROPERTY_TYPE(bool);
};
DECLARE_NODE_PROPERTIES(XJoint)
IMPLEMENT_PROPERTY_COMMON(XJointProperties,XBoneProperties);

XPropertyInfo XJointProperties::m_Properties[1] = 
{
  {XPropertyType::kBool,"segment_scale_compensate",XJoint::kSegmentScaleCompensate,0}
};

bool XJointProperties::SetProperty(XBase* node,const XU32 property_id,const bool& val)
{
  DO_TYPE_CHECK(node,XJoint);
  switch(property_id)
  {
  case XJoint::kSegmentScaleCompensate: object->SetSegmentScaleCompensate(val);   return true;
  default: break;
  }
  return XBaseProperties::SetProperty(node,property_id,val);
}

bool XJointProperties::GetProperty(XBase* node,const XU32 property_id,bool& val)
{
  DO_TYPE_CHECK(node,XJoint);
  switch(property_id)
  {
  case XJoint::kSegmentScaleCompensate: val = object->GetSegmentScaleCompensate();   return true;
  default: break;
  }
  return XBaseProperties::SetProperty(node,property_id,val);
}

//----------------------------------------------------------------------------------------------------------------------
class XFieldProperties : public XBoneProperties
{
  static const XU32 kNumProperties=11;
  static XPropertyInfo m_Properties[kNumProperties];
public:
  PROPERTY_COUNT(XFieldProperties,XBoneProperties);
  PROPERTY_TYPE(XReal);
  PROPERTY_TYPE(bool);
  PROPERTY_TYPE(XList);
  PROPERTY_TYPE(XVector3);
  PROPERTY_TYPE(XU32);
};
DECLARE_NODE_PROPERTIES(XField)

XPropertyInfo XFieldProperties::m_Properties[11] = 
{
  {XPropertyType::kReal,"magnitude",XField::kMagnitude,XPropertyFlags::kAnimatable},
  {XPropertyType::kReal,"attenuation",XField::kAttenuation,XPropertyFlags::kAnimatable},
  {XPropertyType::kReal,"max_distance",XField::kMaxDistance,XPropertyFlags::kAnimatable},
  {XPropertyType::kBool,"use_max_distance",XField::kIsDistanceUsed,XPropertyFlags::kAnimatable},
  {XPropertyType::kBool,"per_vertex",XField::kIsPerVertex,XPropertyFlags::kAnimatable},
  {XPropertyType::kNodeArray,"shapes",XField::kShapes,XPropertyFlags::kArrayIsResizable,XFn::ParticleShape},

  {XPropertyType::kBool,"volume_exclusion",XField::kVolumeExclusion,0},
  {XPropertyType::kBool,"volume_offset",XField::kVolumeOffset,0},
  {XPropertyType::kBool,"section_radius",XField::kSectionRadius,0},
  {XPropertyType::kBool,"sweep_angle",XField::kSweepAngle,0},
  {XPropertyType::kBool,"volume_type",XField::kVolumeType,0}
};

IMPLEMENT_PROPERTY_COMMON(XFieldProperties,XBoneProperties);


bool XFieldProperties::SetProperty(XBase* node,const XU32 property_id,const XU32& val)
{
  DO_TYPE_CHECK(node,XField);
  switch(property_id)
  {
  case XField::kVolumeType:  object->SetVolumeType((XField::XVolumeType)val); return true;
  default: break;
  }
  return XBoneProperties::SetProperty(node,property_id,val);
}

bool XFieldProperties::GetProperty(XBase* node,const XU32 property_id,XU32& val)
{
  DO_TYPE_CHECK(node,XField);
  switch(property_id)
  {
  case XField::kVolumeType:  val = (XU32)object->GetVolumeType(); return true;
  default: break;
  }
  return XBoneProperties::GetProperty(node,property_id,val);
}


bool XFieldProperties::SetProperty(XBase* node,const XU32 property_id,const XVector3& val)
{
  DO_TYPE_CHECK(node,XField);
  switch(property_id)
  {
  case XField::kVolumeOffset:  object->SetVolumeOffset(val); return true;
  default: break;
  }
  return XBoneProperties::SetProperty(node,property_id,val);
}

bool XFieldProperties::GetProperty(XBase* node,const XU32 property_id,XVector3& val)
{
  DO_TYPE_CHECK(node,XField);
  switch(property_id)
  {
  case XField::kVolumeOffset:  val = object->GetVolumeOffset(); return true;
  default: break;
  }
  return XBoneProperties::GetProperty(node,property_id,val);
}


bool XFieldProperties::SetProperty(XBase* node,const XU32 property_id,const XReal& val)
{
  DO_TYPE_CHECK(node,XField);
  switch(property_id)
  {
  case XField::kMagnitude:     object->SetMagnitude(val);     return true;
  case XField::kAttenuation:   object->SetAttenuation(val);   return true;
  case XField::kMaxDistance:   object->SetMaxDistance(val);   return true;
  case XField::kSectionRadius: object->SetSectionRadius(val); return true;
  case XField::kSweepAngle:    object->SetSweepAngle(val);    return true;
  default: break;
  }
  return XBaseProperties::SetProperty(node,property_id,val);
}

bool XFieldProperties::GetProperty(XBase* node,const XU32 property_id,XReal& val)
{
  DO_TYPE_CHECK(node,XField);
  switch(property_id)
  {
  case XField::kMagnitude:     val = object->GetMagnitude();     return true;
  case XField::kAttenuation:   val = object->GetAttenuation();   return true;
  case XField::kMaxDistance:   val = object->GetMaxDistance();   return true;
  case XField::kSectionRadius: val = object->GetSectionRadius(); return true;
  case XField::kSweepAngle:    val = object->GetSweepAngle();    return true;
  default: break;
  }
  return XBaseProperties::GetProperty(node,property_id,val);
}

bool XFieldProperties::SetProperty(XBase* node,const XU32 property_id,const bool& val)
{
  DO_TYPE_CHECK(node,XField);
  switch(property_id)
  {
  case XField::kIsDistanceUsed:  object->SetMaxDistanceUsed(val); return true;
  case XField::kIsPerVertex:     object->SetPerVertex(val);       return true;
  case XField::kVolumeExclusion: object->SetVolumeExclusion(val); return true;
  default: break;
  }
  return XBoneProperties::SetProperty(node,property_id,val);
}

bool XFieldProperties::GetProperty(XBase* node,const XU32 property_id,bool& val)
{
  DO_TYPE_CHECK(node,XField);
  switch(property_id)
  {
  case XField::kIsDistanceUsed:  val = object->GetMaxDistanceUsed(); return true;
  case XField::kIsPerVertex:     val = object->GetPerVertex();       return true;
  case XField::kVolumeExclusion: val = object->GetVolumeExclusion(); return true;
  default: break;
  }
  return XBoneProperties::GetProperty(node,property_id,val);
}

bool XFieldProperties::SetProperty(XBase* node,const XU32 property_id,const XList& val)
{
  DO_TYPE_CHECK(node,XField);
  switch(property_id)
  {
  case XField::kShapes: 
    {
      // delete all old shape nodes
      while(object->GetNumShapes())
        object->DeleteShape(0);

      // now add all new ones... 
      for (XList::const_iterator it = val.begin();it!=val.end();++it)
        object->AddShape(*it);

      return true;
    }
    break;
  default: break;
  }
  return XBaseProperties::SetProperty(node,property_id,val);
}

bool XFieldProperties::GetProperty(XBase* node,const XU32 property_id,XList& val)
{
  DO_TYPE_CHECK(node,XField);
  switch(property_id)
  {
  case XField::kShapes: 
    {
      for (XU32 i=0;i!=object->GetNumShapes();++i)
      {
        val.push_back(object->GetShape(i));
      }
      return true;
    }
  default: break;
  }
  return XBaseProperties::GetProperty(node,property_id,val);
}

//----------------------------------------------------------------------------------------------------------------------
class XAirProperties : public XFieldProperties
{
  static const XU32 kNumProperties=7;
  static XPropertyInfo m_Properties[kNumProperties];
public:
  PROPERTY_COUNT(XAirProperties,XFieldProperties);
  PROPERTY_TYPE(XReal);
  PROPERTY_TYPE(XVector3);
  PROPERTY_TYPE(bool);
};
XPropertyInfo XAirProperties::m_Properties[7] = 
{
  {XPropertyType::kReal,"speed",XAir::kSpeed,XPropertyFlags::kAnimatable},
  {XPropertyType::kReal,"spread",XAir::kSpread,XPropertyFlags::kAnimatable},
  {XPropertyType::kVector3,"direction",XAir::kDirection,XPropertyFlags::kAnimatable},
  {XPropertyType::kBool,"enable_spread",XAir::kIsSpreadEnabled,XPropertyFlags::kAnimatable},
  {XPropertyType::kBool,"component_only",XAir::kIsComponentOnly,XPropertyFlags::kAnimatable},
  {XPropertyType::kBool,"inherit_rotation",XAir::kInheritsRotation,XPropertyFlags::kAnimatable},
  {XPropertyType::kReal,"inherit_velocity",XAir::kInheritsVelocity,XPropertyFlags::kAnimatable}
};

DECLARE_NODE_PROPERTIES(XAir)
IMPLEMENT_PROPERTY_COMMON(XAirProperties,XFieldProperties);

bool XAirProperties::SetProperty(XBase* node,const XU32 property_id,const bool& val)
{
  DO_TYPE_CHECK(node,XAir);
  switch(property_id)
  {
  case XAir::kIsSpreadEnabled:  object->SetSpreadEnabled(val);    return true;
  case XAir::kIsComponentOnly:  object->SetComponentOnly(val);    return true;
  case XAir::kInheritsRotation: object->SetInheritsRotation(val); return true;
  default: break;
  }
  return XFieldProperties::SetProperty(node,property_id,val);
}

bool XAirProperties::GetProperty(XBase* node,const XU32 property_id,bool& val)
{
  DO_TYPE_CHECK(node,XAir);
  switch(property_id)
  {
  case XAir::kIsSpreadEnabled:  val = object->GetSpreadEnabled();  return true;
  case XAir::kIsComponentOnly:  val = object->GetComponentOnly();  return true;
  case XAir::kInheritsRotation: val = object->GetInheritsRotation(); return true;
  default: break;
  }
  return XFieldProperties::GetProperty(node,property_id,val);
}
bool XAirProperties::SetProperty(XBase* node,const XU32 property_id,const XReal& val)
{
  DO_TYPE_CHECK(node,XAir);
  switch(property_id)
  {
  case XAir::kSpeed:            object->SetSpeed(val);            return true;
  case XAir::kSpread:           object->SetSpread(val);           return true;
  case XAir::kInheritsVelocity: object->SetInheritsVelocity(val); return true;
  default: break;
  }
  return XFieldProperties::SetProperty(node,property_id,val);
}

bool XAirProperties::GetProperty(XBase* node,const XU32 property_id,XReal& val)
{
  DO_TYPE_CHECK(node,XAir);
  switch(property_id)
  {
  case XAir::kSpeed:            val = object->GetSpeed();            return true;
  case XAir::kSpread:           val = object->GetSpread();           return true;
  case XAir::kInheritsVelocity: val = object->GetInheritsVelocity(); return true;
  default: break;
  }
  return XFieldProperties::GetProperty(node,property_id,val);
}

bool XAirProperties::SetProperty(XBase* node,const XU32 property_id,const XVector3& val)
{
  DO_TYPE_CHECK(node,XAir);
  switch(property_id)
  {
  case XAir::kDirection: object->SetDirection(val);  return true;
  default: break;
  }
  return XFieldProperties::SetProperty(node,property_id,val);
}

bool XAirProperties::GetProperty(XBase* node,const XU32 property_id,XVector3& val)
{
  DO_TYPE_CHECK(node,XAir);
  switch(property_id)
  {
  case XAir::kDirection: val = object->GetDirection(); return true;
  default: break;
  }
  return XFieldProperties::GetProperty(node,property_id,val);
}

//----------------------------------------------------------------------------------------------------------------------
class XDragProperties : public XFieldProperties
{
  static const XU32 kNumProperties=2;
  static XPropertyInfo m_Properties[kNumProperties];
public:
  PROPERTY_COUNT(XDragProperties,XFieldProperties);
  PROPERTY_TYPE(XVector3);
  PROPERTY_TYPE(bool);
};
XPropertyInfo XDragProperties::m_Properties[2] = 
{
  {XPropertyType::kVector3,"direction",XDrag::kDirection,XPropertyFlags::kAnimatable},
  {XPropertyType::kBool,"use_direction",XDrag::kDirectionUsed,XPropertyFlags::kAnimatable}
};

DECLARE_NODE_PROPERTIES(XDrag)
IMPLEMENT_PROPERTY_COMMON(XDragProperties,XFieldProperties);

bool XDragProperties::SetProperty(XBase* node,const XU32 property_id,const XVector3& val)
{
  DO_TYPE_CHECK(node,XDrag);
  switch(property_id)
  {
  case XDrag::kDirection: object->SetDirection(val);  return true;
  default: break;
  }
  return XFieldProperties::SetProperty(node,property_id,val);
}

bool XDragProperties::GetProperty(XBase* node,const XU32 property_id,XVector3& val)
{
  DO_TYPE_CHECK(node,XDrag);
  switch(property_id)
  {
  case XDrag::kDirection: val = object->GetDirection(); return true;
  default: break;
  }
  return XFieldProperties::GetProperty(node,property_id,val);
}

bool XDragProperties::SetProperty(XBase* node,const XU32 property_id,const bool& val)
{
  DO_TYPE_CHECK(node,XDrag);
  switch(property_id)
  {
  case XDrag::kDirectionUsed: object->SetIsDirectionUsed(val);  return true;
  default: break;
  }
  return XFieldProperties::SetProperty(node,property_id,val);
}

bool XDragProperties::GetProperty(XBase* node,const XU32 property_id,bool& val)
{
  DO_TYPE_CHECK(node,XDrag);
  switch(property_id)
  {
  case XDrag::kDirectionUsed: val = object->GetIsDirectionUsed(); return true;
  default: break;
  }
  return XFieldProperties::GetProperty(node,property_id,val);
}

//----------------------------------------------------------------------------------------------------------------------
class XGravityProperties : public XFieldProperties
{
  static const XU32 kNumProperties=1;
  static XPropertyInfo m_Properties[kNumProperties];
public:
  PROPERTY_COUNT(XGravityProperties,XFieldProperties);
  PROPERTY_TYPE(XVector3);
};
XPropertyInfo XGravityProperties::m_Properties[1] = 
{
  {XPropertyType::kVector3,"direction",XGravity::kDirection,XPropertyFlags::kAnimatable}
};

DECLARE_NODE_PROPERTIES(XGravity)
IMPLEMENT_PROPERTY_COMMON(XGravityProperties,XFieldProperties);

bool XGravityProperties::SetProperty(XBase* node,const XU32 property_id,const XVector3& val)
{
  DO_TYPE_CHECK(node,XGravity);
  switch(property_id)
  {
  case XGravity::kDirection: object->SetDirection(val);  return true;
  default: break;
  }
  return XFieldProperties::SetProperty(node,property_id,val);
}

bool XGravityProperties::GetProperty(XBase* node,const XU32 property_id,XVector3& val)
{
  DO_TYPE_CHECK(node,XGravity);
  switch(property_id)
  {
  case XGravity::kDirection: val = object->GetDirection(); return true;
  default: break;
  }
  return XFieldProperties::GetProperty(node,property_id,val);
}

//----------------------------------------------------------------------------------------------------------------------
class XNewtonProperties : public XFieldProperties
{
  static const XU32 kNumProperties=1;
  static XPropertyInfo m_Properties[kNumProperties];
public:
  PROPERTY_COUNT(XNewtonProperties,XFieldProperties);
  PROPERTY_TYPE(XReal);
};

XPropertyInfo XNewtonProperties::m_Properties[1] = 
{
  {XPropertyType::kReal,"min_distance",XNewton::kMinDistance,XPropertyFlags::kAnimatable}
};

DECLARE_NODE_PROPERTIES(XNewton)
IMPLEMENT_PROPERTY_COMMON(XNewtonProperties,XFieldProperties);

bool XNewtonProperties::SetProperty(XBase* node,const XU32 property_id,const XReal& val)
{
  DO_TYPE_CHECK(node,XNewton);
  switch(property_id)
  {
  case XNewton::kMinDistance: object->SetMinDistance(val);  return true;
  default: break;
  }
  return XFieldProperties::SetProperty(node,property_id,val);
}

bool XNewtonProperties::GetProperty(XBase* node,const XU32 property_id,XReal& val)
{
  DO_TYPE_CHECK(node,XNewton);
  switch(property_id)
  {
  case XNewton::kMinDistance: val = object->GetMinDistance(); return true;
  default: break;
  }
  return XFieldProperties::GetProperty(node,property_id,val);
}

//----------------------------------------------------------------------------------------------------------------------
class XRadialProperties : public XFieldProperties
{
  static const XU32 kNumProperties=1;
  static XPropertyInfo m_Properties[kNumProperties];
public:
  PROPERTY_COUNT(XRadialProperties,XFieldProperties);
  PROPERTY_TYPE(XReal);
};

XPropertyInfo XRadialProperties::m_Properties[1] = 
{
  {XPropertyType::kReal,"radial_type",XRadial::kRadialType,XPropertyFlags::kAnimatable}
};

DECLARE_NODE_PROPERTIES(XRadial)
IMPLEMENT_PROPERTY_COMMON(XRadialProperties,XFieldProperties);

bool XRadialProperties::SetProperty(XBase* node,const XU32 property_id,const XReal& val)
{
  DO_TYPE_CHECK(node,XRadial);
  switch(property_id)
  {
  case XRadial::kRadialType: object->SetRadialType(val);  return true;
  default: break;
  }
  return XFieldProperties::SetProperty(node,property_id,val);
}

bool XRadialProperties::GetProperty(XBase* node,const XU32 property_id,XReal& val)
{
  DO_TYPE_CHECK(node,XRadial);
  switch(property_id)
  {
  case XRadial::kRadialType: val = object->GetRadialType(); return true;
  default: break;
  }
  return XFieldProperties::GetProperty(node,property_id,val);
}

//----------------------------------------------------------------------------------------------------------------------
class XTurbulenceProperties : public XFieldProperties
{
  static const XU32 kNumProperties=2;
  static XPropertyInfo m_Properties[kNumProperties];
public:
  PROPERTY_COUNT(XTurbulenceProperties,XFieldProperties);
  PROPERTY_TYPE(XReal);
  PROPERTY_TYPE(XVector3);
};

XPropertyInfo XTurbulenceProperties::m_Properties[2] = 
{
  {XPropertyType::kReal,"frequency",XTurbulence::kFrequency,XPropertyFlags::kAnimatable},
  {XPropertyType::kVector3,"phase",XTurbulence::kPhase,XPropertyFlags::kAnimatable}
};

DECLARE_NODE_PROPERTIES(XTurbulence)
IMPLEMENT_PROPERTY_COMMON(XTurbulenceProperties,XFieldProperties);

bool XTurbulenceProperties::SetProperty(XBase* node,const XU32 property_id,const XReal& val)
{
  DO_TYPE_CHECK(node,XTurbulence);
  switch(property_id)
  {
  case XTurbulence::kFrequency: object->SetFrequency(val);  return true;
  default: break;
  }
  return XFieldProperties::SetProperty(node,property_id,val);
}

bool XTurbulenceProperties::GetProperty(XBase* node,const XU32 property_id,XReal& val)
{
  DO_TYPE_CHECK(node,XTurbulence);
  switch(property_id)
  {
  case XTurbulence::kFrequency: val = object->GetFrequency(); return true;
  default: break;
  }
  return XFieldProperties::GetProperty(node,property_id,val);
}

bool XTurbulenceProperties::SetProperty(XBase* node,const XU32 property_id,const XVector3& val)
{
  DO_TYPE_CHECK(node,XTurbulence);
  switch(property_id)
  {
  case XTurbulence::kPhase:     object->SetPhase(val);      return true;
  default: break;
  }
  return XFieldProperties::SetProperty(node,property_id,val);
}

bool XTurbulenceProperties::GetProperty(XBase* node,const XU32 property_id,XVector3& val)
{
  DO_TYPE_CHECK(node,XTurbulence);
  switch(property_id)
  {
  case XTurbulence::kPhase:     object->GetPhase(val);     return true;
  default: break;
  }
  return XFieldProperties::GetProperty(node,property_id,val);
}

//----------------------------------------------------------------------------------------------------------------------
class XUniformProperties : public XFieldProperties
{
  static const XU32 kNumProperties=1;
  static XPropertyInfo m_Properties[kNumProperties];
public:
  PROPERTY_COUNT(XUniformProperties,XFieldProperties);
  PROPERTY_TYPE(XVector3);
};

XPropertyInfo XUniformProperties::m_Properties[1] = 
{
  {XPropertyType::kVector3,"direction",XUniform::kDirection,XPropertyFlags::kAnimatable}
};

DECLARE_NODE_PROPERTIES(XUniform)
IMPLEMENT_PROPERTY_COMMON(XUniformProperties,XFieldProperties);

bool XUniformProperties::SetProperty(XBase* node,const XU32 property_id,const XVector3& val)
{
  DO_TYPE_CHECK(node,XUniform);
  switch(property_id)
  {
  case XUniform::kDirection: object->SetDirection(val);  return true;
  default: break;
  }
  return XFieldProperties::SetProperty(node,property_id,val);
}

bool XUniformProperties::GetProperty(XBase* node,const XU32 property_id,XVector3& val)
{
  DO_TYPE_CHECK(node,XUniform);
  switch(property_id)
  {
  case XUniform::kDirection: val = object->GetDirection(); return true;
  default: break;
  }
  return XFieldProperties::GetProperty(node,property_id,val);
}

//----------------------------------------------------------------------------------------------------------------------
class XVolumeAxisProperties : public XFieldProperties
{
  static const XU32 kNumProperties=12;
  static XPropertyInfo m_Properties[kNumProperties];
public:
  PROPERTY_COUNT(XVolumeAxisProperties,XFieldProperties);
  PROPERTY_TYPE(XVector3);
  PROPERTY_TYPE(XReal);
  PROPERTY_TYPE(bool);
};

XPropertyInfo XVolumeAxisProperties::m_Properties[12] = 
{
  {XPropertyType::kVector3,"direction",XVolumeAxis::kDirection,XPropertyFlags::kAnimatable},
  {XPropertyType::kReal,"directional_speed",XVolumeAxis::kDirectionalSpeed,XPropertyFlags::kAnimatable},
  {XPropertyType::kBool,"invert_attenuation",XVolumeAxis::kInvertAttenuation,XPropertyFlags::kAnimatable},
  {XPropertyType::kReal,"speed_along_axis",XVolumeAxis::kSpeedAlongAxis,XPropertyFlags::kAnimatable},
  {XPropertyType::kReal,"speed_around_axis",XVolumeAxis::kSpeedAroundAxis,XPropertyFlags::kAnimatable},
  {XPropertyType::kReal,"speed_away_from_axis",XVolumeAxis::kSpeedAwayFromAxis,XPropertyFlags::kAnimatable},
  {XPropertyType::kReal,"speed_away_from_center",XVolumeAxis::kSpeedAwayFromCenter,XPropertyFlags::kAnimatable},
  {XPropertyType::kVector3,"turbulence_frequency",XVolumeAxis::kTurbulenceFrequency,XPropertyFlags::kAnimatable},
  {XPropertyType::kVector3,"turbulence_offset",XVolumeAxis::kTurbulenceOffset,XPropertyFlags::kAnimatable},
  {XPropertyType::kReal,"turbulence",XVolumeAxis::kTurbulence,XPropertyFlags::kAnimatable},
  {XPropertyType::kReal,"turbulence_speed",XVolumeAxis::kTurbulenceSpeed,XPropertyFlags::kAnimatable},
  {XPropertyType::kReal,"detail_turbulence",XVolumeAxis::kDetailTurbulence,XPropertyFlags::kAnimatable}
};

DECLARE_NODE_PROPERTIES(XVolumeAxis)
IMPLEMENT_PROPERTY_COMMON(XVolumeAxisProperties,XFieldProperties);

bool XVolumeAxisProperties::SetProperty(XBase* node,const XU32 property_id,const XVector3& val)
{
  DO_TYPE_CHECK(node,XVolumeAxis);
  switch(property_id)
  {
  case XVolumeAxis::kDirection:           object->SetDirection(val);            return true;
  case XVolumeAxis::kTurbulenceFrequency: object->SetTurbulenceFrequency(val);  return true;
  case XVolumeAxis::kTurbulenceOffset:    object->SetTurbulenceOffset(val);     return true;
  default: break;
  }
  return XBoneProperties::SetProperty(node,property_id,val);
}

bool XVolumeAxisProperties::GetProperty(XBase* node,const XU32 property_id,XVector3& val)
{
  DO_TYPE_CHECK(node,XVolumeAxis);
  switch(property_id)
  {
  case XVolumeAxis::kDirection:           val = object->GetDirection();           return true;
  case XVolumeAxis::kTurbulenceFrequency: val = object->GetTurbulenceFrequency(); return true;
  case XVolumeAxis::kTurbulenceOffset:    val = object->GetTurbulenceOffset();    return true;
  default: break;
  }
  return XBoneProperties::GetProperty(node,property_id,val);
}

bool XVolumeAxisProperties::SetProperty(XBase* node,const XU32 property_id,const bool& val)
{
  DO_TYPE_CHECK(node,XVolumeAxis);
  switch(property_id)
  {
  case XVolumeAxis::kInvertAttenuation:  object->SetInvertAttenuation(val); return true;
  default: break;
  }
  return XFieldProperties::SetProperty(node,property_id,val);
}

bool XVolumeAxisProperties::GetProperty(XBase* node,const XU32 property_id,bool& val)
{
  DO_TYPE_CHECK(node,XVolumeAxis);
  switch(property_id)
  {
  case XVolumeAxis::kInvertAttenuation:  val = object->GetInvertAttenuation(); return true;
  default: break;
  }
  return XFieldProperties::GetProperty(node,property_id,val);
}

bool XVolumeAxisProperties::SetProperty(XBase* node,const XU32 property_id,const XReal& val)
{
  DO_TYPE_CHECK(node,XVolumeAxis);
  switch(property_id)
  {
  case XVolumeAxis::kDirectionalSpeed:    object->SetDirectionalSpeed(val);     return true;
  case XVolumeAxis::kSpeedAlongAxis:      object->SetSpeedAlongAxis(val);       return true;
  case XVolumeAxis::kSpeedAroundAxis:     object->SetSpeedAroundAxis(val);      return true;
  case XVolumeAxis::kSpeedAwayFromAxis:   object->SetSpeedAwayFromAxis(val);    return true;
  case XVolumeAxis::kSpeedAwayFromCenter: object->SetSpeedAwayFromCenter(val);  return true;
  case XVolumeAxis::kTurbulence:          object->SetTurbulence(val);           return true;
  case XVolumeAxis::kTurbulenceSpeed:     object->SetTurbulenceSpeed(val);      return true;
  case XVolumeAxis::kDetailTurbulence:    object->SetDetailTurbulence(val);     return true;
  default: break;
  }
  return XFieldProperties::SetProperty(node,property_id,val);
}

bool XVolumeAxisProperties::GetProperty(XBase* node,const XU32 property_id,XReal& val)
{
  DO_TYPE_CHECK(node,XVolumeAxis);
  switch(property_id)
  {
  case XVolumeAxis::kDirectionalSpeed:    val = object->GetDirectionalSpeed();    return true;
  case XVolumeAxis::kSpeedAlongAxis:      val = object->GetSpeedAlongAxis();      return true;
  case XVolumeAxis::kSpeedAroundAxis:     val = object->GetSpeedAroundAxis();     return true;
  case XVolumeAxis::kSpeedAwayFromAxis:   val = object->GetSpeedAwayFromAxis();   return true;
  case XVolumeAxis::kSpeedAwayFromCenter: val = object->GetSpeedAwayFromCenter(); return true;
  case XVolumeAxis::kTurbulence:          val = object->GetTurbulence();          return true;
  case XVolumeAxis::kTurbulenceSpeed:     val = object->GetTurbulenceSpeed();     return true;
  case XVolumeAxis::kDetailTurbulence:    val = object->GetDetailTurbulence();    return true;
  default: break;
  }
  return XFieldProperties::GetProperty(node,property_id,val);
}

//----------------------------------------------------------------------------------------------------------------------
class XVortexProperties : public XFieldProperties
{
  static const XU32 kNumProperties=1;
  static XPropertyInfo m_Properties[kNumProperties];
public:
  PROPERTY_COUNT(XVortexProperties,XFieldProperties);
  PROPERTY_TYPE(XVector3);
};

XPropertyInfo XVortexProperties::m_Properties[1] = 
{
  {XPropertyType::kVector3,"axis",XVortex::kAxis,XPropertyFlags::kAnimatable}
};

DECLARE_NODE_PROPERTIES(XVortex)
IMPLEMENT_PROPERTY_COMMON(XVortexProperties,XFieldProperties);

bool XVortexProperties::SetProperty(XBase* node,const XU32 property_id,const XVector3& val)
{
  DO_TYPE_CHECK(node,XVortex);
  switch(property_id)
  {
  case XVortex::kAxis: object->SetAxis(val);  return true;
  default: break;
  }
  return XBoneProperties::SetProperty(node,property_id,val);
}

bool XVortexProperties::GetProperty(XBase* node,const XU32 property_id,XVector3& val)
{
  DO_TYPE_CHECK(node,XVortex);
  switch(property_id)
  {
  case XVortex::kAxis: val = object->GetAxis(); return true;
  default: break;
  }
  return XBoneProperties::GetProperty(node,property_id,val);
}

//----------------------------------------------------------------------------------------------------------------------
class XEmitterProperties : public XBoneProperties
{
  static const XU32 kNumProperties=24;
  static XPropertyInfo m_Properties[kNumProperties];
public:
  PROPERTY_COUNT(XEmitterProperties,XBoneProperties);
  PROPERTY_TYPE(XVector3);
  PROPERTY_TYPE(XReal);
  PROPERTY_TYPE(XColour);
  PROPERTY_TYPE(XBase*);
  PROPERTY_TYPE(XU32);
};

XPropertyInfo XEmitterProperties::m_Properties[24] = 
{
  {XPropertyType::kReal,"rate",XEmitter::kRate},
  {XPropertyType::kReal,"max_distance",XEmitter::kMaxDistance,XPropertyFlags::kAnimatable},
  {XPropertyType::kReal,"min_distance",XEmitter::kMinDistance,XPropertyFlags::kAnimatable},
  {XPropertyType::kVector3,"direction",XEmitter::kDirection,XPropertyFlags::kAnimatable},
  {XPropertyType::kReal,"spread",XEmitter::kSpread,XPropertyFlags::kAnimatable},
  {XPropertyType::kColour,"particle_colour",XEmitter::kColour,XPropertyFlags::kAnimatable},
  {XPropertyType::kUnsigned,"emitter_type",XEmitter::kEmitterType,XPropertyFlags::kAnimatable},
  {XPropertyType::kNode,"emitter_object",XEmitter::kEmitterObject,XPropertyFlags::kAnimatable},
  {XPropertyType::kNode,"emitter_transform",XEmitter::kEmitterTransform,XPropertyFlags::kAnimatable},
  {XPropertyType::kUnsigned,"volume_shape",XEmitter::kVolumeType,XPropertyFlags::kAnimatable},
  {XPropertyType::kVector3,"volume_offset",XEmitter::kVolumeOffset,XPropertyFlags::kAnimatable},
  {XPropertyType::kReal,"volume_sweep",XEmitter::kVolumeSweep,XPropertyFlags::kAnimatable},
  {XPropertyType::kReal,"inner_radius",XEmitter::kSectionRadius,XPropertyFlags::kAnimatable},
  {XPropertyType::kReal,"speed",XEmitter::kSpeed,XPropertyFlags::kAnimatable},
  {XPropertyType::kReal,"speed_random",XEmitter::kSpeedRandom,XPropertyFlags::kAnimatable},
  {XPropertyType::kReal,"size",XEmitter::kSize,XPropertyFlags::kAnimatable},
  {XPropertyType::kReal,"size_random",XEmitter::kSizeRandom,XPropertyFlags::kAnimatable},
  {XPropertyType::kReal,"mass",XEmitter::kMass,XPropertyFlags::kAnimatable},
  {XPropertyType::kReal,"mass_random",XEmitter::kMassRandom,XPropertyFlags::kAnimatable},
  {XPropertyType::kReal,"alpha",XEmitter::kAlpha,XPropertyFlags::kAnimatable},
  {XPropertyType::kReal,"alpha_random",XEmitter::kAlphaRandom,XPropertyFlags::kAnimatable},
  {XPropertyType::kReal,"hue_offset",XEmitter::kHueOffset,XPropertyFlags::kAnimatable},
  {XPropertyType::kReal,"saturation_offset",XEmitter::kSaturationOffset,XPropertyFlags::kAnimatable},
  {XPropertyType::kReal,"luminance_offset",XEmitter::kLuminanceOffset,XPropertyFlags::kAnimatable}
};

DECLARE_NODE_PROPERTIES(XEmitter)
IMPLEMENT_PROPERTY_COMMON(XEmitterProperties,XBoneProperties);

bool XEmitterProperties::SetProperty(XBase* node,const XU32 property_id,const XVector3& val)
{
  DO_TYPE_CHECK(node,XEmitter);
  switch(property_id)
  {
  case XEmitter::kDirection:    object->SetDirection(val);  return true;
  case XEmitter::kVolumeOffset: object->SetOffset(val);     return true;
  default: break;
  }
  return XBoneProperties::SetProperty(node,property_id,val);
}

bool XEmitterProperties::GetProperty(XBase* node,const XU32 property_id,XVector3& val)
{
  DO_TYPE_CHECK(node,XEmitter);
  switch(property_id)
  {
  case XEmitter::kDirection:    val = object->GetDirection(); return true;
  case XEmitter::kVolumeOffset: val = object->GetOffset();    return true;
  default: break;
  }
  return XBoneProperties::GetProperty(node,property_id,val);
}

bool XEmitterProperties::SetProperty(XBase* node,const XU32 property_id,const XColour& val)
{
  DO_TYPE_CHECK(node,XEmitter);
  switch(property_id)
  {
  case XEmitter::kColour: object->SetColour(val);  return true;
  default: break;
  }
  return XBaseProperties::SetProperty(node,property_id,val);
}

bool XEmitterProperties::GetProperty(XBase* node,const XU32 property_id,XColour& val)
{
  DO_TYPE_CHECK(node,XEmitter);
  switch(property_id)
  {
  case XEmitter::kColour: val = object->GetColour(); return true;
  default: break;
  }
  return XBaseProperties::GetProperty(node,property_id,val);
}

bool XEmitterProperties::SetProperty(XBase* node,const XU32 property_id,const XU32& val)
{
  DO_TYPE_CHECK(node,XEmitter);
  switch(property_id)
  {
  case XEmitter::kEmitterType: 
    {
      if(val>=0 && val<= ((XU32)XEmitter::kVolume))
        object->SetEmitterType((XEmitter::XEmitterType)val);
      else
        return false;
      return true;
    }
  case XEmitter::kVolumeType: 
    {
      if(val>=0 && val<= ((XU32)XEmitter::kGrid))
        object->SetVolumeType((XEmitter::XEmitterVolumeType)val);
      else
        return false;
      return true;
    }
  default: break;
  }
  return XBaseProperties::SetProperty(node,property_id,val);
}

bool XEmitterProperties::GetProperty(XBase* node,const XU32 property_id,XU32& val)
{
  DO_TYPE_CHECK(node,XEmitter);
  switch(property_id)
  {
  case XEmitter::kEmitterType: val = (XU32)object->GetEmitterType(); return true;
  case XEmitter::kVolumeType:  val = (XU32)object->GetVolumeType();  return true;
  default: break;
  }
  return XBaseProperties::GetProperty(node,property_id,val);
}

bool XEmitterProperties::SetProperty(XBase* node,const XU32 property_id,const XBase*& val)
{
  DO_TYPE_CHECK(node,XEmitter);
  switch(property_id)
  {
  case XEmitter::kEmitterObject:    object->SetEmitterGeometry((XGeometry*)val->HasFn<XGeometry>(),object->GetEmitterTransform());  return true;
  case XEmitter::kEmitterTransform: object->SetEmitterGeometry(object->GetEmitterGeometry(),(XBone*)val->HasFn<XBone>());  return true;
  default: break;
  }
  return XBaseProperties::SetProperty(node,property_id,val);
}

bool XEmitterProperties::GetProperty(XBase* node,const XU32 property_id,XBase*& val)
{
  DO_TYPE_CHECK(node,XEmitter);
  switch(property_id)
  {
  case XEmitter::kEmitterObject:    val = object->GetEmitterGeometry();  return true;
  case XEmitter::kEmitterTransform: val = object->GetEmitterTransform(); return true;
  default: break;
  }
  return XBaseProperties::GetProperty(node,property_id,val);
}


bool XEmitterProperties::SetProperty(XBase* node,const XU32 property_id,const XReal& val)
{
  DO_TYPE_CHECK(node,XEmitter);
  switch(property_id)
  {
  case XEmitter::kRate:             object->SetRate(val);                   return true;
  case XEmitter::kMaxDistance:      object->SetMaxDistance(val);            return true;
  case XEmitter::kMinDistance:      object->SetMinDistance(val);            return true;
  case XEmitter::kSpread:           object->SetSpread(val);                 return true;
  case XEmitter::kVolumeSweep:      object->SetVolumeSweep(val);            return true;
  case XEmitter::kSectionRadius:    object->SetInnerRadius(val);            return true;
  case XEmitter::kSpeed:            object->SetSpeed(val);                  return true;
  case XEmitter::kSpeedRandom:      object->SetSpeedRandom(val);            return true;
  case XEmitter::kSize:             object->SetSize(val);                   return true;
  case XEmitter::kSizeRandom:       object->SetSizeRandom(val);             return true;
  case XEmitter::kMass:             object->SetMass(val);                   return true;
  case XEmitter::kMassRandom:       object->SetMassRandom(val);             return true;
  case XEmitter::kAlpha:            object->SetAlpha(val);                  return true;
  case XEmitter::kAlphaRandom:      object->SetAlphaRandom(val);            return true;
  case XEmitter::kHueOffset:        object->SetColourOffsetHue(val);        return true;
  case XEmitter::kSaturationOffset: object->SetColourOffsetSaturation(val); return true;
  case XEmitter::kLuminanceOffset:  object->SetColourOffsetLuminance(val);  return true;
  default: break;
  }
  return XBaseProperties::SetProperty(node,property_id,val);
}

bool XEmitterProperties::GetProperty(XBase* node,const XU32 property_id,XReal& val)
{
  DO_TYPE_CHECK(node,XEmitter);
  switch(property_id)
  {
  case XEmitter::kRate:             val = object->GetRate();                   return true;
  case XEmitter::kMaxDistance:      val = object->GetMaxDistance();            return true;
  case XEmitter::kMinDistance:      val = object->GetMinDistance();            return true;
  case XEmitter::kSpread:           val = object->GetSpread();                 return true;
  case XEmitter::kVolumeSweep:      val = object->GetVolumeSweep();            return true;
  case XEmitter::kSectionRadius:    val = object->GetInnerRadius();            return true;
  case XEmitter::kSpeed:            val = object->GetSpeed();                  return true;
  case XEmitter::kSpeedRandom:      val = object->GetSpeedRandom();            return true;
  case XEmitter::kSize:             val = object->GetSize();                   return true;
  case XEmitter::kSizeRandom:       val = object->GetSizeRandom();             return true;
  case XEmitter::kMass:             val = object->GetMass();                   return true;
  case XEmitter::kMassRandom:       val = object->GetMassRandom();             return true;
  case XEmitter::kAlpha:            val = object->GetAlpha();                  return true;
  case XEmitter::kAlphaRandom:      val = object->GetAlphaRandom();            return true;
  case XEmitter::kHueOffset:        val = object->GetColourOffsetHue();        return true;
  case XEmitter::kSaturationOffset: val = object->GetColourOffsetSaturation(); return true;
  case XEmitter::kLuminanceOffset:  val = object->GetColourOffsetLuminance();  return true;
  default: break;
  }
  return XBaseProperties::GetProperty(node,property_id,val);
}

//----------------------------------------------------------------------------------------------------------------------
class XConstraintProperties : public XBoneProperties
{
  static const XU32 kNumProperties=3;
  static XPropertyInfo m_Properties[kNumProperties];
public:
  PROPERTY_COUNT(XConstraintProperties,XBoneProperties);
  PROPERTY_TYPE(XRealArray);
  PROPERTY_TYPE(XList);
  PROPERTY_TYPE(XBase*);
};

XPropertyInfo XConstraintProperties::m_Properties[3] = 
{
  {XPropertyType::kRealArray,"weights",XConstraint::kWeights,XPropertyFlags::kAnimatable},
  {XPropertyType::kNodeArray,"targets",XConstraint::kTargets,XPropertyFlags::kArrayIsResizable,XFn::Bone},
  {XPropertyType::kNode,"constrained",XConstraint::kConstrained,0,XFn::Bone}
};

DECLARE_NODE_PROPERTIES(XConstraint)
IMPLEMENT_PROPERTY_COMMON(XConstraintProperties,XBoneProperties);


bool XConstraintProperties::SetProperty(XBase* node,const XU32 property_id,const XBase*& val)
{
  DO_TYPE_CHECK(node,XConstraint);
  switch(property_id)
  {
  case XConstraint::kConstrained: 
    {
      if(val->HasFn<XBone>())
        return object->SetConstrained(val);;
    }
    return false;
  default: break;
  }
  return XBaseProperties::SetProperty(node,property_id,val);
}

bool XConstraintProperties::GetProperty(XBase* node,const XU32 property_id,XBase*& val)
{
  DO_TYPE_CHECK(node,XConstraint);
  switch(property_id)
  {
  case XConstraint::kConstrained: 
    {
      val = object->GetConstrained();
    }
    return true;
  default: break;
  }
  return XBaseProperties::GetProperty(node,property_id,val);
}

bool XConstraintProperties::SetProperty(XBase* node,const XU32 property_id,const XRealArray& val)
{
  DO_TYPE_CHECK(node,XConstraint);
  switch(property_id)
  {
  case XConstraint::kWeights: 
    {
      if(val.size() != object->GetNumTargets()) 
      {
        XGlobal::GetLogger()->Logf(XBasicLogger::kError,
          "Number of target weights is not equal to the number of targets - %s.weights", object->GetName());
        return false;
      }
      for(XU32 i=0;i!=object->GetNumTargets();++i)
      {
        object->SetTargetWeight(i,val[i]); 
      }
    }
    return true;
  default: break;
  }
  return XBaseProperties::SetProperty(node,property_id,val);
}

bool XConstraintProperties::GetProperty(XBase* node,const XU32 property_id,XRealArray& val)
{
  DO_TYPE_CHECK(node,XConstraint);
  switch(property_id)
  {
  case XConstraint::kWeights:  val = object->Weights(); return true;
  default: break;
  }
  return XBaseProperties::GetProperty(node,property_id,val);
}

bool XConstraintProperties::SetProperty(XBase* node,const XU32 property_id,const XList& val)
{
  DO_TYPE_CHECK(node,XConstraint);
  switch(property_id)
  {
  case XConstraint::kTargets: 
    {
      XBoneList targets;
      for (XList::const_iterator it = val.begin();it != val.end();++it)
      {
        const XBase* base = (*it);
        if(base)
        {
          const XBone* bone = base->HasFn<XBone>();
          if (bone)
          {
            targets.push_back((XBone*)bone);
          }
        }
      }
      object->SetTargets(targets);
    }
    return true;
  default: break;
  }
  return XBaseProperties::SetProperty(node,property_id,val);
}

bool XConstraintProperties::GetProperty(XBase* node,const XU32 property_id,XList& val)
{
  DO_TYPE_CHECK(node,XConstraint);
  switch(property_id)
  {
  case XConstraint::kTargets: 
    {
      val.clear();
      for (XU32 i=0;i!=object->GetNumTargets();++i)
      {
        val.push_back( object->GetTarget(i) );
      }
    }
    return true;
  default: break;
  }
  return XBaseProperties::GetProperty(node,property_id,val);
}

//----------------------------------------------------------------------------------------------------------------------
class XAimConstraintProperties : public XConstraintProperties
{
  static const XU32 kNumProperties=3;
  static XPropertyInfo m_Properties[kNumProperties];
public:
  PROPERTY_COUNT(XAimConstraintProperties,XConstraintProperties);
  PROPERTY_TYPE(XVector3);
};

XPropertyInfo XAimConstraintProperties::m_Properties[3] = 
{
  {XPropertyType::kVector3,"offset",XAimConstraint::kOffset,XPropertyFlags::kAnimatable},
  {XPropertyType::kVector3,"aim_vector",XAimConstraint::kAimVector,0},
  {XPropertyType::kVector3,"up_vector",XAimConstraint::kUpVector,0}
};

DECLARE_NODE_PROPERTIES(XAimConstraint)
IMPLEMENT_PROPERTY_COMMON(XAimConstraintProperties,XConstraintProperties);

bool XAimConstraintProperties::SetProperty(XBase* node,const XU32 property_id,const XVector3& val)
{
  DO_TYPE_CHECK(node,XAimConstraint);
  switch(property_id)
  {
  case XAimConstraint::kOffset:    object->SetOffset(val);    return true;
  case XAimConstraint::kAimVector: object->SetAimVector(val); return true;
  case XAimConstraint::kUpVector:  object->SetUpVector(val);  return true;
  default: break;
  }
  return XBoneProperties::SetProperty(node,property_id,val);
}

bool XAimConstraintProperties::GetProperty(XBase* node,const XU32 property_id,XVector3& val)
{
  DO_TYPE_CHECK(node,XAimConstraint);
  switch(property_id)
  {
  case XAimConstraint::kOffset:    val = object->GetOffset();    return true;
  case XAimConstraint::kAimVector: val = object->GetAimVector(); return true;
  case XAimConstraint::kUpVector:  val = object->GetUpVector();  return true;
  default: break;
  }
  return XBoneProperties::GetProperty(node,property_id,val);
}

//----------------------------------------------------------------------------------------------------------------------
class XLookAtConstraintProperties : public XAimConstraintProperties
{
  static const XU32 kNumProperties=3;
  static XPropertyInfo m_Properties[kNumProperties];
public:
  PROPERTY_COUNT(XLookAtConstraintProperties,XAimConstraintProperties);
  PROPERTY_TYPE(XReal);
};

XPropertyInfo XLookAtConstraintProperties::m_Properties[3] = 
{
  {XPropertyType::kReal,"distance_between",XLookAtConstraint::kDistanceBetween,XPropertyFlags::kAnimatable},
  {XPropertyType::kReal,"twist",XLookAtConstraint::kTwist,XPropertyFlags::kAnimatable}
};

DECLARE_NODE_PROPERTIES(XLookAtConstraint)
IMPLEMENT_PROPERTY_COMMON(XLookAtConstraintProperties,XAimConstraintProperties);

bool XLookAtConstraintProperties::SetProperty(XBase* node,const XU32 property_id,const XReal& val)
{
  DO_TYPE_CHECK(node,XLookAtConstraint);
  switch(property_id)
  {
  case XLookAtConstraint::kDistanceBetween: object->SetDistanceBetween(val); return true;
  case XLookAtConstraint::kTwist:           object->SetTwist(val);           return true;
  default: break;
  }
  return XBoneProperties::SetProperty(node,property_id,val);
}

bool XLookAtConstraintProperties::GetProperty(XBase* node,const XU32 property_id,XReal& val)
{
  DO_TYPE_CHECK(node,XLookAtConstraint);
  switch(property_id)
  {
  case XLookAtConstraint::kDistanceBetween: val = object->GetDistanceBetween(); return true;
  case XLookAtConstraint::kTwist:           val = object->GetTwist();           return true;
  default: break;
  }
  return XBoneProperties::GetProperty(node,property_id,val);
}

//----------------------------------------------------------------------------------------------------------------------
class XNormalConstraintProperties : public XConstraintProperties
{
  static const XU32 kNumProperties=2;
  static XPropertyInfo m_Properties[kNumProperties];
public:
  PROPERTY_COUNT(XNormalConstraintProperties,XConstraintProperties);
  PROPERTY_TYPE(XVector3);
};

XPropertyInfo XNormalConstraintProperties::m_Properties[2] = 
{
  {XPropertyType::kVector3,"aim_vector",XNormalConstraint::kAimVector,0},
  {XPropertyType::kVector3,"up_vector",XNormalConstraint::kUpVector,0}
};

DECLARE_NODE_PROPERTIES(XNormalConstraint)
IMPLEMENT_PROPERTY_COMMON(XNormalConstraintProperties,XConstraintProperties);

bool XNormalConstraintProperties::SetProperty(XBase* node,const XU32 property_id,const XVector3& val)
{
  DO_TYPE_CHECK(node,XNormalConstraint);
  switch(property_id)
  {
  case XNormalConstraint::kAimVector: object->SetAimVector(val); return true;
  case XNormalConstraint::kUpVector:  object->SetUpVector(val);  return true;
  default: break;
  }
  return XBoneProperties::SetProperty(node,property_id,val);
}

bool XNormalConstraintProperties::GetProperty(XBase* node,const XU32 property_id,XVector3& val)
{
  DO_TYPE_CHECK(node,XNormalConstraint);
  switch(property_id)
  {
  case XNormalConstraint::kAimVector: val = object->GetAimVector(); return true;
  case XNormalConstraint::kUpVector:  val = object->GetUpVector();  return true;
  default: break;
  }
  return XBoneProperties::GetProperty(node,property_id,val);
}

//----------------------------------------------------------------------------------------------------------------------
class XOrientConstraintProperties : public XConstraintProperties
{
  static const XU32 kNumProperties=1;
  static XPropertyInfo m_Properties[kNumProperties];
public:
  PROPERTY_COUNT(XOrientConstraintProperties,XConstraintProperties);
  PROPERTY_TYPE(XVector3);
};

XPropertyInfo XOrientConstraintProperties::m_Properties[1] = 
{
  {XPropertyType::kVector3,"offset",XOrientConstraint::kOffset,XPropertyFlags::kAnimatable}
};

DECLARE_NODE_PROPERTIES(XOrientConstraint)
IMPLEMENT_PROPERTY_COMMON(XOrientConstraintProperties,XConstraintProperties);

bool XOrientConstraintProperties::SetProperty(XBase* node,const XU32 property_id,const XVector3& val)
{
  DO_TYPE_CHECK(node,XOrientConstraint);
  switch(property_id)
  {
  case XOrientConstraint::kOffset: object->SetOffset(val); return true;
  default: break;
  }
  return XBoneProperties::SetProperty(node,property_id,val);
}

bool XOrientConstraintProperties::GetProperty(XBase* node,const XU32 property_id,XVector3& val)
{
  DO_TYPE_CHECK(node,XOrientConstraint);
  switch(property_id)
  {
  case XOrientConstraint::kOffset: val = object->GetOffset(); return true;
  default: break;
  }
  return XBoneProperties::GetProperty(node,property_id,val);
}

//----------------------------------------------------------------------------------------------------------------------
class XPointConstraintProperties : public XConstraintProperties
{
  static const XU32 kNumProperties=1;
  static XPropertyInfo m_Properties[kNumProperties];
public:
  PROPERTY_COUNT(XPointConstraintProperties,XConstraintProperties);
  PROPERTY_TYPE(XVector3);
};

XPropertyInfo XPointConstraintProperties::m_Properties[1] = 
{
  {XPropertyType::kVector3,"offset",XPointConstraint::kOffset,XPropertyFlags::kAnimatable}
};

DECLARE_NODE_PROPERTIES(XPointConstraint)
IMPLEMENT_PROPERTY_COMMON(XPointConstraintProperties,XConstraintProperties);

bool XPointConstraintProperties::SetProperty(XBase* node,const XU32 property_id,const XVector3& val)
{
  DO_TYPE_CHECK(node,XPointConstraint);
  switch(property_id)
  {
  case XPointConstraint::kOffset: object->SetOffset(val); return true;
  default: break;
  }
  return XBoneProperties::SetProperty(node,property_id,val);
}

bool XPointConstraintProperties::GetProperty(XBase* node,const XU32 property_id,XVector3& val)
{
  DO_TYPE_CHECK(node,XPointConstraint);
  switch(property_id)
  {
  case XPointConstraint::kOffset: val = object->GetOffset(); return true;
  default: break;
  }
  return XBoneProperties::GetProperty(node,property_id,val);
}

//----------------------------------------------------------------------------------------------------------------------
class XPoleVectorConstraintProperties : public XPointConstraintProperties
{
  static const XU32 kNumProperties=1;
  static XPropertyInfo m_Properties[kNumProperties];
public:
  PROPERTY_COUNT(XPoleVectorConstraintProperties,XPointConstraintProperties);
  PROPERTY_TYPE(XMatrix);
};

XPropertyInfo XPoleVectorConstraintProperties::m_Properties[1] = 
{
  {XPropertyType::kMatrix,"pivot_space",XPoleVectorConstraint::kPivotSpace,0}
};

DECLARE_NODE_PROPERTIES(XPoleVectorConstraint)
IMPLEMENT_PROPERTY_COMMON(XPoleVectorConstraintProperties,XPointConstraintProperties);

bool XPoleVectorConstraintProperties::SetProperty(XBase* node,const XU32 property_id,const XMatrix& val)
{
  DO_TYPE_CHECK(node,XPoleVectorConstraint);
  switch(property_id)
  {
  case XPoleVectorConstraint::kPivotSpace: object->SetPivotSpace(val); return true;
  default: break;
  }
  return XBoneProperties::SetProperty(node,property_id,val);
}

bool XPoleVectorConstraintProperties::GetProperty(XBase* node,const XU32 property_id,XMatrix& val)
{
  DO_TYPE_CHECK(node,XPoleVectorConstraint);
  switch(property_id)
  {
  case XPoleVectorConstraint::kPivotSpace: val = object->GetPivotSpace(); return true;
  default: break;
  }
  return XBoneProperties::GetProperty(node,property_id,val);
}

//----------------------------------------------------------------------------------------------------------------------
class XScaleConstraintProperties : public XConstraintProperties
{
  static const XU32 kNumProperties=1;
  static XPropertyInfo m_Properties[kNumProperties];
public:
  PROPERTY_COUNT(XScaleConstraintProperties,XConstraintProperties);
  PROPERTY_TYPE(XVector3);
};

XPropertyInfo XScaleConstraintProperties::m_Properties[1] = 
{
  {XPropertyType::kVector3,"offset",XScaleConstraint::kOffset,XPropertyFlags::kAnimatable}
};

DECLARE_NODE_PROPERTIES(XScaleConstraint)
IMPLEMENT_PROPERTY_COMMON(XScaleConstraintProperties,XConstraintProperties);

bool XScaleConstraintProperties::SetProperty(XBase* node,const XU32 property_id,const XVector3& val)
{
  DO_TYPE_CHECK(node,XScaleConstraint);
  switch(property_id)
  {
  case XScaleConstraint::kOffset: object->SetOffset(val); return true;
  default: break;
  }
  return XBoneProperties::SetProperty(node,property_id,val);
}

bool XScaleConstraintProperties::GetProperty(XBase* node,const XU32 property_id,XVector3& val)
{
  DO_TYPE_CHECK(node,XScaleConstraint);
  switch(property_id)
  {
  case XScaleConstraint::kOffset: val = object->GetOffset(); return true;
  default: break;
  }
  return XBoneProperties::GetProperty(node,property_id,val);
}

//----------------------------------------------------------------------------------------------------------------------
class XTangentConstraintProperties : public XConstraintProperties
{
  static const XU32 kNumProperties=2;
  static XPropertyInfo m_Properties[kNumProperties];
public:
  PROPERTY_COUNT(XTangentConstraintProperties,XConstraintProperties);
  PROPERTY_TYPE(XVector3);
};

XPropertyInfo XTangentConstraintProperties::m_Properties[2] = 
{
  {XPropertyType::kVector3,"aim_vector",XTangentConstraint::kAimVector,0},
  {XPropertyType::kVector3,"up_vector",XTangentConstraint::kUpVector,0}
};

DECLARE_NODE_PROPERTIES(XTangentConstraint)
IMPLEMENT_PROPERTY_COMMON(XTangentConstraintProperties,XConstraintProperties);

bool XTangentConstraintProperties::SetProperty(XBase* node,const XU32 property_id,const XVector3& val)
{
  DO_TYPE_CHECK(node,XTangentConstraint);
  switch(property_id)
  {
  case XTangentConstraint::kAimVector: object->SetAimVector(val); return true;
  case XTangentConstraint::kUpVector:  object->SetUpVector(val);  return true;
  default: break;
  }
  return XBoneProperties::SetProperty(node,property_id,val);
}

bool XTangentConstraintProperties::GetProperty(XBase* node,const XU32 property_id,XVector3& val)
{
  DO_TYPE_CHECK(node,XTangentConstraint);
  switch(property_id)
  {
  case XTangentConstraint::kAimVector: val = object->GetAimVector(); return true;
  case XTangentConstraint::kUpVector:  val = object->GetUpVector();  return true;
  default: break;
  }
  return XBoneProperties::GetProperty(node,property_id,val);
}

//----------------------------------------------------------------------------------------------------------------------
class XDeformerProperties : public XBaseProperties
{
  static const XU32 kNumProperties=2;
  static XPropertyInfo m_Properties[kNumProperties];
public:
  PROPERTY_COUNT(XDeformerProperties,XBaseProperties);
  PROPERTY_TYPE(XList);
  PROPERTY_TYPE(XReal);
};

XPropertyInfo XDeformerProperties::m_Properties[2] =
{
  {XPropertyType::kNode,"affected",XDeformer::kAffected,0,XFn::Geometry},
  {XPropertyType::kReal,"envelope_weight",XDeformer::kEnvelopeWeight,XPropertyFlags::kAnimatable}
};

static const XU32 kEnvelopeWeight = XBase::kLast+2;

DECLARE_NODE_PROPERTIES(XDeformer)
IMPLEMENT_PROPERTY_COMMON(XDeformerProperties,XBaseProperties);

bool XDeformerProperties::SetProperty(XBase* node,const XU32 property_id,const XReal& val)
{
  DO_TYPE_CHECK(node,XDeformer);
  switch(property_id)
  {
  case XDeformer::kEnvelopeWeight: object->SetEnvelopeWeight(val); return true;
  default: break;
  }
  return XBaseProperties::SetProperty(node,property_id,val);
}

bool XDeformerProperties::GetProperty(XBase* node,const XU32 property_id,XReal& val)
{
  DO_TYPE_CHECK(node,XDeformer);
  switch(property_id)
  {
  case XDeformer::kEnvelopeWeight: val = object->GetEnvelopeWeight(); return true;
  default: break;
  }
  return XBaseProperties::GetProperty(node,property_id,val);
}

bool XDeformerProperties::SetProperty(XBase* node,const XU32 property_id,const XList& val)
{
  DO_TYPE_CHECK(node,XDeformer);
  switch(property_id)
  {
  case XDeformer::kAffected: 
    {
      XU32 id=0;
      while(object->GetNumAffected())
        object->RemoveAffected(id);

      XList::const_iterator it = val.begin();
      for (;it != val.end(); ++it)
      {
        if((*it)->HasFn<XBone*>())
          object->AddAffected( *it );
      }
    }
    return true;
  default: break;
  }
  return XBaseProperties::SetProperty(node,property_id,val);
}

bool XDeformerProperties::GetProperty(XBase* node,const XU32 property_id,XList& val)
{
  DO_TYPE_CHECK(node,XDeformer);
  switch(property_id)
  {
  case XDeformer::kAffected:
    {
      val.clear();
      for(XU32 i=0;i!=object->GetNumAffected();++i)
        val.push_back( object->GetAffected(i) );
    }
    return true;
  default: break;
  }
  return XBaseProperties::GetProperty(node,property_id,val);
}

//----------------------------------------------------------------------------------------------------------------------
class XBlendShapeProperties : public XDeformerProperties
{
  static const XU32 kNumProperties=3;
  static XPropertyInfo m_Properties[kNumProperties];
public:
  PROPERTY_COUNT(XBlendShapeProperties,XDeformerProperties);
  PROPERTY_TYPE(XRealArray);
  PROPERTY_TYPE(XList);
  PROPERTY_TYPE(XBase*);
};

XPropertyInfo XBlendShapeProperties::m_Properties[3] = 
{
  {XPropertyType::kRealArray,"weights",XBlendShape::kWeights,XPropertyFlags::kAnimatable},
  {XPropertyType::kRealArray,"targets",XBlendShape::kTargets,XPropertyFlags::kArrayIsResizable},
  {XPropertyType::kRealArray,"base_shape",XBlendShape::kBaseShape,0}
};

DECLARE_NODE_PROPERTIES(XBlendShape)
IMPLEMENT_PROPERTY_COMMON(XBlendShapeProperties,XDeformerProperties);

bool XBlendShapeProperties::SetProperty(XBase* node,const XU32 property_id,const XRealArray& val)
{
  DO_TYPE_CHECK(node,XBlendShape);
  switch(property_id)
  {
  case XBlendShape::kWeights: 
    {
      if(val.size() != object->GetNumTargets()) return false;
      for (XU32 i=0;i<object->GetNumTargets();++i)
        object->SetWeight(i,val[i]);
    }
    return true;
  default: break;
  }
  return XBaseProperties::SetProperty(node,property_id,val);
}

bool XBlendShapeProperties::GetProperty(XBase* node,const XU32 property_id,XRealArray& val)
{
  DO_TYPE_CHECK(node,XBlendShape);
  switch(property_id)
  {
  case XBlendShape::kWeights: 
    {
      val.resize(object->GetNumTargets());
      for (XU32 i=0;i<object->GetNumTargets();++i)
        val[i] = object->GetWeight(i);
    }
    return true;
  default: break;
  }
  return XBaseProperties::GetProperty(node,property_id,val);
}

bool XBlendShapeProperties::SetProperty(XBase* node,const XU32 property_id,const XBase*& val)
{
  DO_TYPE_CHECK(node,XBlendShape);
  switch(property_id)
  {
  case XBlendShape::kBaseShape:  object->SetBase(val); return true;
  default: break;
  }
  return XBaseProperties::SetProperty(node,property_id,val);
}

bool XBlendShapeProperties::GetProperty(XBase* node,const XU32 property_id,XBase*& val)
{
  DO_TYPE_CHECK(node,XBlendShape);
  switch(property_id)
  {
  case XBlendShape::kBaseShape:  val = object->GetBase(); return true;
  default: break;
  }
  return XBaseProperties::GetProperty(node,property_id,val);
}

bool XBlendShapeProperties::SetProperty(XBase* node,const XU32 property_id,const XList& val)
{
  DO_TYPE_CHECK(node,XBlendShape);
  switch(property_id)
  {
  case XBlendShape::kTargets: 
    {
      /// \todo
    }
    return true;
  default: break;
  }
  return XDeformerProperties::SetProperty(node,property_id,val);
}

bool XBlendShapeProperties::GetProperty(XBase* node,const XU32 property_id,XList& val)
{
  DO_TYPE_CHECK(node,XBlendShape);
  switch(property_id)
  {
  case XBlendShape::kTargets: 
    {
      XU32 ntargets = object->GetNumTargets();
      val.resize(ntargets);
      for (XU32 i=0;i!=ntargets;++i)
      {
        val[i] = object->GetTarget(i);
      }
    }
    return true;
  default: break;
  }
  return XDeformerProperties::GetProperty(node,property_id,val);
}

//----------------------------------------------------------------------------------------------------------------------
class XClusterProperties : public XDeformerProperties
{
  static const XU32 kNumProperties=3;
  static XPropertyInfo m_Properties[kNumProperties];
public:
  PROPERTY_COUNT(XClusterProperties,XDeformerProperties);
  PROPERTY_TYPE(XRealArray);
  PROPERTY_TYPE(XU32Array);
  PROPERTY_TYPE(XBase*);
};

XPropertyInfo XClusterProperties::m_Properties[3] = 
{
  {XPropertyType::kRealArray,"weights",XCluster::kWeights,XPropertyFlags::kArrayIsResizable},
  {XPropertyType::kUnsignedArray,"indices",XCluster::kIndices,XPropertyFlags::kArrayIsResizable},
  {XPropertyType::kNode,"influence",XCluster::kInfluenceJoint,0,XFn::Bone}
};

DECLARE_NODE_PROPERTIES(XCluster)
IMPLEMENT_PROPERTY_COMMON(XClusterProperties,XDeformerProperties);

bool XClusterProperties::SetProperty(XBase* node,const XU32 property_id,const XRealArray& val)
{
  DO_TYPE_CHECK(node,XCluster);
  switch(property_id)
  {
  case XCluster::kWeights: object->SetWeights(val); return true;
  default: break;
  }
  return XBaseProperties::SetProperty(node,property_id,val);
}

bool XClusterProperties::GetProperty(XBase* node,const XU32 property_id,XRealArray& val)
{
  DO_TYPE_CHECK(node,XCluster);
  switch(property_id)
  {
  case XCluster::kWeights:  object->GetWeights(val); return true;
  default: break;
  }
  return XBaseProperties::GetProperty(node,property_id,val);
}

bool XClusterProperties::SetProperty(XBase* node,const XU32 property_id,const XU32Array& val)
{
  DO_TYPE_CHECK(node,XCluster);
  switch(property_id)
  {
  case XCluster::kIndices:  object->SetIndices(val); return true;
  default: break;
  }
  return XBaseProperties::SetProperty(node,property_id,val);
}

bool XClusterProperties::GetProperty(XBase* node,const XU32 property_id,XU32Array& val)
{
  DO_TYPE_CHECK(node,XCluster);
  switch(property_id)
  {
  case XCluster::kIndices:  object->GetIndices(val); return true;
  default: break;
  }
  return XBaseProperties::GetProperty(node,property_id,val);
}

bool XClusterProperties::SetProperty(XBase* node,const XU32 property_id,const XBase*& val)
{
  DO_TYPE_CHECK(node,XCluster);
  switch(property_id)
  {
  case XCluster::kInfluenceJoint:  return object->SetInfluenceJoint(val);
  default: break;
  }
  return XBaseProperties::SetProperty(node,property_id,val);
}

bool XClusterProperties::GetProperty(XBase* node,const XU32 property_id,XBase*& val)
{
  DO_TYPE_CHECK(node,XCluster);
  switch(property_id)
  {
  case XCluster::kInfluenceJoint:  val = object->GetInfluenceJoint(); return true;
  default: break;
  }
  return XBaseProperties::GetProperty(node,property_id,val);
}

//----------------------------------------------------------------------------------------------------------------------
class XFFDProperties : public XDeformerProperties
{
  static const XU32 kNumProperties=2;
  static XPropertyInfo m_Properties[kNumProperties];
public:
  PROPERTY_COUNT(XFFDProperties,XDeformerProperties);
  PROPERTY_TYPE(XBase*);
};

XPropertyInfo XFFDProperties::m_Properties[2] = 
{
  {XPropertyType::kNode,"base_lattice",XFFD::kBaseLattice,0,XFn::BaseLattice},
  {XPropertyType::kNode,"deform_lattice",XFFD::kDeformLattice,0,XFn::Lattice}
};

DECLARE_NODE_PROPERTIES(XFFD)
IMPLEMENT_PROPERTY_COMMON(XFFDProperties,XDeformerProperties);

bool XFFDProperties::SetProperty(XBase* node,const XU32 property_id,const XBase*& val)
{
  DO_TYPE_CHECK(node,XFFD);
  switch(property_id)
  {
  case XFFD::kBaseLattice:    return object->SetBaseLattice(val);
  case XFFD::kDeformLattice:  return object->SetDeformedLattice(val);
  default: break;
  }
  return XBaseProperties::SetProperty(node,property_id,val);
}

bool XFFDProperties::GetProperty(XBase* node,const XU32 property_id,XBase*& val)
{
  DO_TYPE_CHECK(node,XFFD);
  switch(property_id)
  {
  case XFFD::kBaseLattice:   val = object->GetBaseLattice(); return true;
  case XFFD::kDeformLattice: val = object->GetDeformedLattice(); return true;
  default: break;
  }
  return XBaseProperties::GetProperty(node,property_id,val);
}

//----------------------------------------------------------------------------------------------------------------------
class XJiggleDeformerProperties : public XDeformerProperties
{
  static const XU32 kNumProperties=10;
  static XPropertyInfo m_Properties[kNumProperties];
public:
  PROPERTY_COUNT(XJiggleDeformerProperties,XDeformerProperties);
  PROPERTY_TYPE(XReal);
  PROPERTY_TYPE(XU32Array);
  PROPERTY_TYPE(bool);
};

XPropertyInfo XJiggleDeformerProperties::m_Properties[10] = 
{
  {XPropertyType::kReal,"motion_multiplier",XJiggleDeformer::kMotionMultiplier,XPropertyFlags::kAnimatable},
  {XPropertyType::kReal,"force_along_normal",XJiggleDeformer::kForceAlongNormal,XPropertyFlags::kAnimatable},
  {XPropertyType::kReal,"force_on_tangent",XJiggleDeformer::kForceOnTangent,XPropertyFlags::kAnimatable},
  {XPropertyType::kReal,"stiffness",XJiggleDeformer::kStiffness,XPropertyFlags::kAnimatable},
  {XPropertyType::kReal,"damping",XJiggleDeformer::kDamping,XPropertyFlags::kAnimatable},
  {XPropertyType::kReal,"jiggle_weight",XJiggleDeformer::kJiggleWeight,XPropertyFlags::kAnimatable},
  {XPropertyType::kReal,"direction_bias",XJiggleDeformer::kDirectionBias,XPropertyFlags::kAnimatable},
  {XPropertyType::kUnsignedArray,"point_indices",XJiggleDeformer::kPointIndices,0},
  {XPropertyType::kBool,"ignore_transform",XJiggleDeformer::kIgnoreTransform,0},
  {XPropertyType::kBool,"enable",XJiggleDeformer::kEnable,0}
};

DECLARE_NODE_PROPERTIES(XJiggleDeformer)
IMPLEMENT_PROPERTY_COMMON(XJiggleDeformerProperties,XDeformerProperties);

bool XJiggleDeformerProperties::SetProperty(XBase* node,const XU32 property_id,const bool& val)
{
  DO_TYPE_CHECK(node,XJiggleDeformer);
  switch(property_id)
  {
  case XJiggleDeformer::kIgnoreTransform: object->SetIgnoreTransform(val); return true;
  case XJiggleDeformer::kEnable: object->SetEnable(val); return true;
  default: break;
  }
  return XBaseProperties::SetProperty(node,property_id,val);
}

bool XJiggleDeformerProperties::GetProperty(XBase* node,const XU32 property_id,bool& val)
{
  DO_TYPE_CHECK(node,XJiggleDeformer);
  switch(property_id)
  {
  case XJiggleDeformer::kIgnoreTransform: val = object->GetIgnoreTransform(); return true;
  case XJiggleDeformer::kEnable:          val = object->GetEnable(); return true;
  default: break;
  }
  return XBaseProperties::GetProperty(node,property_id,val);
}

bool XJiggleDeformerProperties::SetProperty(XBase* node,const XU32 property_id,const XU32Array& val)
{
  DO_TYPE_CHECK(node,XJiggleDeformer);
  switch(property_id)
  {
  case XJiggleDeformer::kPointIndices: object->SetPointIndices(val); return true;
  default: break;
  }
  return XBaseProperties::SetProperty(node,property_id,val);
}

bool XJiggleDeformerProperties::GetProperty(XBase* node,const XU32 property_id,XU32Array& val)
{
  DO_TYPE_CHECK(node,XJiggleDeformer);
  switch(property_id)
  {
  case XJiggleDeformer::kPointIndices: val = object->GetPointIndices(); return true;
  default: break;
  }
  return XBaseProperties::GetProperty(node,property_id,val);
}

bool XJiggleDeformerProperties::SetProperty(XBase* node,const XU32 property_id,const XReal& val)
{
  DO_TYPE_CHECK(node,XJiggleDeformer);
  switch(property_id)
  {
  case XJiggleDeformer::kMotionMultiplier: object->SetMotionMultiplier(val); return true;
  case XJiggleDeformer::kForceAlongNormal: object->SetForceAlongNormal(val); return true;
  case XJiggleDeformer::kForceOnTangent: object->SetForceOnTangent(val); return true;
  case XJiggleDeformer::kStiffness: object->SetStiffness(val); return true;
  case XJiggleDeformer::kDamping: object->SetDamping(val); return true;
  case XJiggleDeformer::kJiggleWeight: object->SetJiggleWeight(val); return true;
  case XJiggleDeformer::kDirectionBias: object->SetDirectionBias(val); return true;
  default: break;
  }
  return XBaseProperties::SetProperty(node,property_id,val);
}

bool XJiggleDeformerProperties::GetProperty(XBase* node,const XU32 property_id,XReal& val)
{
  DO_TYPE_CHECK(node,XJiggleDeformer);
  switch(property_id)
  {
  case XJiggleDeformer::kMotionMultiplier: val = object->GetMotionMultiplier(); return true;
  case XJiggleDeformer::kForceAlongNormal: val = object->GetForceAlongNormal(); return true;
  case XJiggleDeformer::kForceOnTangent: val = object->GetForceOnTangent(); return true;
  case XJiggleDeformer::kStiffness: val = object->GetStiffness(); return true;
  case XJiggleDeformer::kDamping: val = object->GetDamping(); return true;
  case XJiggleDeformer::kJiggleWeight: val = object->GetJiggleWeight(); return true;
  case XJiggleDeformer::kDirectionBias: val = object->GetDirectionBias(); return true;
  default: break;
  }
  return XBaseProperties::GetProperty(node,property_id,val);
}

//----------------------------------------------------------------------------------------------------------------------
class XJointClusterProperties : public XDeformerProperties
{
  static const XU32 kNumProperties=3;
  static XPropertyInfo m_Properties[kNumProperties];
public:
  PROPERTY_COUNT(XJointClusterProperties,XDeformerProperties);
  PROPERTY_TYPE(XRealArray);
  PROPERTY_TYPE(XU32Array);
  PROPERTY_TYPE(XBase*);
};

XPropertyInfo XJointClusterProperties::m_Properties[3] = 
{
  {XPropertyType::kRealArray,"weights",XJointCluster::kWeights,0},
  {XPropertyType::kRealArray,"indices",XJointCluster::kIndices,0},
  {XPropertyType::kRealArray,"influence_joint",XJointCluster::kInfluenceJoint,0,XFn::Bone}
};

DECLARE_NODE_PROPERTIES(XJointCluster)
IMPLEMENT_PROPERTY_COMMON(XJointClusterProperties,XDeformerProperties);

bool XJointClusterProperties::SetProperty(XBase* node,const XU32 property_id,const XRealArray& val)
{
  DO_TYPE_CHECK(node,XJointCluster);
  switch(property_id)
  {
  case XJointCluster::kWeights: object->SetWeights(val); return true;
  default: break;
  }
  return XBaseProperties::SetProperty(node,property_id,val);
}

bool XJointClusterProperties::GetProperty(XBase* node,const XU32 property_id,XRealArray& val)
{
  DO_TYPE_CHECK(node,XJointCluster);
  switch(property_id)
  {
  case XJointCluster::kWeights:  object->GetWeights(val); return true;
  default: break;
  }
  return XBaseProperties::GetProperty(node,property_id,val);
}

bool XJointClusterProperties::SetProperty(XBase* node,const XU32 property_id,const XU32Array& val)
{
  DO_TYPE_CHECK(node,XJointCluster);
  switch(property_id)
  {
  case XJointCluster::kIndices:  object->SetIndices(val); return true;
  default: break;
  }
  return XBaseProperties::SetProperty(node,property_id,val);
}

bool XJointClusterProperties::GetProperty(XBase* node,const XU32 property_id,XU32Array& val)
{
  DO_TYPE_CHECK(node,XJointCluster);
  switch(property_id)
  {
  case XJointCluster::kIndices:  object->GetIndices(val); return true;
  default: break;
  }
  return XBaseProperties::GetProperty(node,property_id,val);
}

bool XJointClusterProperties::SetProperty(XBase* node,const XU32 property_id,const XBase*& val)
{
  DO_TYPE_CHECK(node,XJointCluster);
  switch(property_id)
  {
  case XJointCluster::kInfluenceJoint: return object->SetInfluenceJoint(val);
  default: break;
  }
  return XBaseProperties::SetProperty(node,property_id,val);
}

bool XJointClusterProperties::GetProperty(XBase* node,const XU32 property_id,XBase*& val)
{
  DO_TYPE_CHECK(node,XJointCluster);
  switch(property_id)
  {
  case XJointCluster::kInfluenceJoint: val = object->GetInfluenceJoint(); return true;
  default: break;
  }
  return XBaseProperties::GetProperty(node,property_id,val);
}

//----------------------------------------------------------------------------------------------------------------------
class XNonLinearDeformerProperties : public XDeformerProperties
{
  static const XU32 kNumProperties=1;
  static XPropertyInfo m_Properties[kNumProperties];
public:
  PROPERTY_COUNT(XNonLinearDeformerProperties,XDeformerProperties);
  PROPERTY_TYPE(XBase*);
};

XPropertyInfo XNonLinearDeformerProperties::m_Properties[1] = 
{
  {XPropertyType::kNode,"handle",XNonLinearDeformer::kHandle,0,XFn::Bone}
};

DECLARE_NODE_PROPERTIES(XNonLinearDeformer)
IMPLEMENT_PROPERTY_COMMON(XNonLinearDeformerProperties,XDeformerProperties);

bool XNonLinearDeformerProperties::SetProperty(XBase* node,const XU32 property_id,const XBase*& val)
{
  DO_TYPE_CHECK(node,XNonLinearDeformer);
  switch(property_id)
  {
  case XNonLinearDeformer::kHandle: return object->SetHandle(val); 
  default: break;
  }
  return XBaseProperties::SetProperty(node,property_id,val);
}

bool XNonLinearDeformerProperties::GetProperty(XBase* node,const XU32 property_id,XBase*& val)
{
  DO_TYPE_CHECK(node,XNonLinearDeformer);
  switch(property_id)
  {
  case XNonLinearDeformer::kHandle: val = object->GetHandle(); return true;
  default: break;
  }
  return XBaseProperties::GetProperty(node,property_id,val);
}


//----------------------------------------------------------------------------------------------------------------------
class XNonLinearBendProperties : public XNonLinearDeformerProperties
{
  static const XU32 kNumProperties=3;
  static XPropertyInfo m_Properties[kNumProperties];
public:
  PROPERTY_COUNT(XNonLinearBendProperties,XNonLinearDeformerProperties);
  PROPERTY_TYPE(XReal);
};

XPropertyInfo XNonLinearBendProperties::m_Properties[3] = 
{
  {XPropertyType::kReal,"low_bound",XNonLinearBend::kLowBound,XPropertyFlags::kAnimatable},
  {XPropertyType::kReal,"high_bound",XNonLinearBend::kHighBound,XPropertyFlags::kAnimatable},
  {XPropertyType::kReal,"curvature",XNonLinearBend::kCurvature,XPropertyFlags::kAnimatable}
};

DECLARE_NODE_PROPERTIES(XNonLinearBend)
IMPLEMENT_PROPERTY_COMMON(XNonLinearBendProperties,XNonLinearDeformerProperties);

bool XNonLinearBendProperties::SetProperty(XBase* node,const XU32 property_id,const XReal& val)
{
  DO_TYPE_CHECK(node,XNonLinearBend);
  switch(property_id)
  {
  case XNonLinearBend::kLowBound: object->SetLowBound(val); return true;
  case XNonLinearBend::kHighBound: object->SetHighBound(val); return true;
  case XNonLinearBend::kCurvature: object->SetCurvature(val); return true;
  default: break;
  }
  return XBaseProperties::SetProperty(node,property_id,val);
}

bool XNonLinearBendProperties::GetProperty(XBase* node,const XU32 property_id,XReal& val)
{
  DO_TYPE_CHECK(node,XNonLinearBend);
  switch(property_id)
  {
  case XNonLinearBend::kLowBound: val = object->GetLowBound(); return true;
  case XNonLinearBend::kHighBound: val = object->GetHighBound(); return true;
  case XNonLinearBend::kCurvature: val = object->GetCurvature(); return true;
  default: break;
  }
  return XBaseProperties::GetProperty(node,property_id,val);
}

//----------------------------------------------------------------------------------------------------------------------
class XNonLinearFlareProperties : public XNonLinearDeformerProperties
{
  static const XU32 kNumProperties=7;
  static XPropertyInfo m_Properties[kNumProperties];
public:
  PROPERTY_COUNT(XNonLinearFlareProperties,XNonLinearDeformerProperties);
  PROPERTY_TYPE(XReal);
};

XPropertyInfo XNonLinearFlareProperties::m_Properties[7] = 
{
  {XPropertyType::kReal,"low_bound",XNonLinearFlare::kLowBound,XPropertyFlags::kAnimatable},
  {XPropertyType::kReal,"high_bound",XNonLinearFlare::kHighBound,XPropertyFlags::kAnimatable},
  {XPropertyType::kReal,"start_flare_x",XNonLinearFlare::kStartFlareX,XPropertyFlags::kAnimatable},
  {XPropertyType::kReal,"start_flare_z",XNonLinearFlare::kStartFlareZ,XPropertyFlags::kAnimatable},
  {XPropertyType::kReal,"end_flare_x",XNonLinearFlare::kEndFlareX,XPropertyFlags::kAnimatable},
  {XPropertyType::kReal,"end_flare_z",XNonLinearFlare::kEndFlareZ,XPropertyFlags::kAnimatable},
  {XPropertyType::kReal,"curve",XNonLinearFlare::kCurve,XPropertyFlags::kAnimatable}
};

DECLARE_NODE_PROPERTIES(XNonLinearFlare)
IMPLEMENT_PROPERTY_COMMON(XNonLinearFlareProperties,XNonLinearDeformerProperties);

bool XNonLinearFlareProperties::SetProperty(XBase* node,const XU32 property_id,const XReal& val)
{
  DO_TYPE_CHECK(node,XNonLinearFlare);
  switch(property_id)
  {
  case XNonLinearFlare::kLowBound: object->SetLowBound(val); return true;
  case XNonLinearFlare::kHighBound: object->SetHighBound(val); return true;
  case XNonLinearFlare::kStartFlareX: object->SetStartFlareX(val); return true;
  case XNonLinearFlare::kStartFlareZ: object->SetStartFlareZ(val); return true;
  case XNonLinearFlare::kEndFlareX: object->SetEndFlareX(val); return true;
  case XNonLinearFlare::kEndFlareZ: object->SetEndFlareZ(val); return true;
  case XNonLinearFlare::kCurve: object->SetCurve(val); return true;
  default: break;
  }
  return XBaseProperties::SetProperty(node,property_id,val);
}

bool XNonLinearFlareProperties::GetProperty(XBase* node,const XU32 property_id,XReal& val)
{
  DO_TYPE_CHECK(node,XNonLinearFlare);
  switch(property_id)
  {
  case XNonLinearFlare::kLowBound: val = object->GetLowBound(); return true;
  case XNonLinearFlare::kHighBound: val = object->GetHighBound(); return true;
  case XNonLinearFlare::kStartFlareX: val = object->GetStartFlareX(); return true;
  case XNonLinearFlare::kStartFlareZ: val = object->GetStartFlareZ(); return true;
  case XNonLinearFlare::kEndFlareX: val = object->GetEndFlareX(); return true;
  case XNonLinearFlare::kEndFlareZ: val = object->GetEndFlareZ(); return true;
  case XNonLinearFlare::kCurve: val = object->GetCurve(); return true;
  default: break;
  }
  return XBaseProperties::GetProperty(node,property_id,val);
}

//----------------------------------------------------------------------------------------------------------------------
class XNonLinearSineProperties : public XNonLinearDeformerProperties
{
  static const XU32 kNumProperties=6;
  static XPropertyInfo m_Properties[kNumProperties];
public:
  PROPERTY_COUNT(XNonLinearSineProperties,XNonLinearDeformerProperties);
  PROPERTY_TYPE(XReal);
};

XPropertyInfo XNonLinearSineProperties::m_Properties[6] = 
{
  {XPropertyType::kReal,"low_bound",XNonLinearSine::kLowBound,XPropertyFlags::kAnimatable},
  {XPropertyType::kReal,"high_bound",XNonLinearSine::kHighBound,XPropertyFlags::kAnimatable},
  {XPropertyType::kReal,"amplitude",XNonLinearSine::kAmplitude,XPropertyFlags::kAnimatable},
  {XPropertyType::kReal,"wavelength",XNonLinearSine::kWavelength,XPropertyFlags::kAnimatable},
  {XPropertyType::kReal,"dropoff",XNonLinearSine::kDropoff,XPropertyFlags::kAnimatable},
  {XPropertyType::kReal,"offset",XNonLinearSine::kOffset,XPropertyFlags::kAnimatable}
};

DECLARE_NODE_PROPERTIES(XNonLinearSine)
IMPLEMENT_PROPERTY_COMMON(XNonLinearSineProperties,XNonLinearDeformerProperties);

bool XNonLinearSineProperties::SetProperty(XBase* node,const XU32 property_id,const XReal& val)
{
  DO_TYPE_CHECK(node,XNonLinearSine);
  switch(property_id)
  {
  case XNonLinearSine::kLowBound: object->SetLowBound(val); return true;
  case XNonLinearSine::kHighBound: object->SetHighBound(val); return true;
  case XNonLinearSine::kAmplitude: object->SetAmplitude(val); return true;
  case XNonLinearSine::kWavelength: object->SetWavelength(val); return true;
  case XNonLinearSine::kDropoff: object->SetDropoff(val); return true;
  case XNonLinearSine::kOffset: object->SetOffset(val); return true;
  default: break;
  }
  return XBaseProperties::SetProperty(node,property_id,val);
}

bool XNonLinearSineProperties::GetProperty(XBase* node,const XU32 property_id,XReal& val)
{
  DO_TYPE_CHECK(node,XNonLinearSine);
  switch(property_id)
  {
  case XNonLinearSine::kLowBound: val = object->GetLowBound(); return true;
  case XNonLinearSine::kHighBound: val = object->GetHighBound(); return true;
  case XNonLinearSine::kAmplitude: val = object->GetAmplitude(); return true;
  case XNonLinearSine::kWavelength: val = object->GetWavelength(); return true;
  case XNonLinearSine::kDropoff: val = object->GetDropoff(); return true;
  case XNonLinearSine::kOffset: val = object->GetOffset(); return true;
  default: break;
  }
  return XBaseProperties::GetProperty(node,property_id,val);
}

//----------------------------------------------------------------------------------------------------------------------
class XNonLinearSquashProperties : public XNonLinearDeformerProperties
{
  static const XU32 kNumProperties=7;
  static XPropertyInfo m_Properties[kNumProperties];
public:
  PROPERTY_COUNT(XNonLinearSquashProperties,XNonLinearDeformerProperties);
  PROPERTY_TYPE(XReal);
};

XPropertyInfo XNonLinearSquashProperties::m_Properties[7] = 
{
  {XPropertyType::kReal,"low_bound",XNonLinearSquash::kLowBound,XPropertyFlags::kAnimatable},
  {XPropertyType::kReal,"high_bound",XNonLinearSquash::kHighBound,XPropertyFlags::kAnimatable},
  {XPropertyType::kReal,"start_smoothness",XNonLinearSquash::kStartSmoothness,XPropertyFlags::kAnimatable},
  {XPropertyType::kReal,"end_smoothness",XNonLinearSquash::kEndSmoothness,XPropertyFlags::kAnimatable},
  {XPropertyType::kReal,"expand",XNonLinearSquash::kExpand,XPropertyFlags::kAnimatable},
  {XPropertyType::kReal,"factor",XNonLinearSquash::kFactor,XPropertyFlags::kAnimatable},
  {XPropertyType::kReal,"max_expand_position",XNonLinearSquash::kMaxExpandPosition,XPropertyFlags::kAnimatable}
};

DECLARE_NODE_PROPERTIES(XNonLinearSquash)
IMPLEMENT_PROPERTY_COMMON(XNonLinearSquashProperties,XNonLinearDeformerProperties);

bool XNonLinearSquashProperties::SetProperty(XBase* node,const XU32 property_id,const XReal& val)
{
  DO_TYPE_CHECK(node,XNonLinearSquash);
  switch(property_id)
  {
  case XNonLinearSquash::kLowBound: object->SetLowBound(val); return true;
  case XNonLinearSquash::kHighBound: object->SetHighBound(val); return true;
  case XNonLinearSquash::kStartSmoothness: object->SetStartSmoothness(val); return true;
  case XNonLinearSquash::kEndSmoothness: object->SetEndSmoothness(val); return true;
  case XNonLinearSquash::kExpand: object->SetExpand(val); return true;
  case XNonLinearSquash::kFactor: object->SetFactor(val); return true;
  case XNonLinearSquash::kMaxExpandPosition: object->SetMaxExpandPosition(val); return true;
  default: break;
  }
  return XBaseProperties::SetProperty(node,property_id,val);
}

bool XNonLinearSquashProperties::GetProperty(XBase* node,const XU32 property_id,XReal& val)
{
  DO_TYPE_CHECK(node,XNonLinearSquash);
  switch(property_id)
  {
  case XNonLinearSquash::kLowBound: val = object->GetLowBound(); return true;
  case XNonLinearSquash::kHighBound: val = object->GetHighBound(); return true;
  case XNonLinearSquash::kStartSmoothness: val = object->GetStartSmoothness(); return true;
  case XNonLinearSquash::kEndSmoothness: val = object->GetEndSmoothness(); return true;
  case XNonLinearSquash::kExpand: val = object->GetExpand(); return true;
  case XNonLinearSquash::kFactor: val = object->GetFactor(); return true;
  case XNonLinearSquash::kMaxExpandPosition: val = object->GetMaxExpandPosition(); return true;
  default: break;
  }
  return XBaseProperties::GetProperty(node,property_id,val);
}

//----------------------------------------------------------------------------------------------------------------------
class XNonLinearTwistProperties : public XNonLinearDeformerProperties
{
  static const XU32 kNumProperties=4;
  static XPropertyInfo m_Properties[kNumProperties];
public:
  PROPERTY_COUNT(XNonLinearTwistProperties,XNonLinearDeformerProperties);
  PROPERTY_TYPE(XReal);
};

XPropertyInfo XNonLinearTwistProperties::m_Properties[4] = 
{
  {XPropertyType::kReal,"low_bound",XNonLinearTwist::kLowBound,XPropertyFlags::kAnimatable},
  {XPropertyType::kReal,"high_bound",XNonLinearTwist::kHighBound,XPropertyFlags::kAnimatable},
  {XPropertyType::kReal,"start_angle",XNonLinearTwist::kStartAngle,XPropertyFlags::kAnimatable},
  {XPropertyType::kReal,"end_angle",XNonLinearTwist::kEndAngle,XPropertyFlags::kAnimatable}
};

DECLARE_NODE_PROPERTIES(XNonLinearTwist)
IMPLEMENT_PROPERTY_COMMON(XNonLinearTwistProperties,XNonLinearDeformerProperties);

bool XNonLinearTwistProperties::SetProperty(XBase* node,const XU32 property_id,const XReal& val)
{
  DO_TYPE_CHECK(node,XNonLinearTwist);
  switch(property_id)
  {
  case XNonLinearTwist::kLowBound: object->SetLowBound(val); return true;
  case XNonLinearTwist::kHighBound: object->SetHighBound(val); return true;
  case XNonLinearTwist::kStartAngle: object->SetStartAngle(val); return true;
  case XNonLinearTwist::kEndAngle: object->SetEndAngle(val); return true;
  default: break;
  }
  return XBaseProperties::SetProperty(node,property_id,val);
}

bool XNonLinearTwistProperties::GetProperty(XBase* node,const XU32 property_id,XReal& val)
{
  DO_TYPE_CHECK(node,XNonLinearTwist);
  switch(property_id)
  {
  case XNonLinearTwist::kLowBound: val = object->GetLowBound(); return true;
  case XNonLinearTwist::kHighBound: val = object->GetHighBound(); return true;
  case XNonLinearTwist::kStartAngle: val = object->GetStartAngle(); return true;
  case XNonLinearTwist::kEndAngle: val = object->GetEndAngle(); return true;
  default: break;
  }
  return XBaseProperties::GetProperty(node,property_id,val);
}

//----------------------------------------------------------------------------------------------------------------------
class XNonLinearWaveProperties : public XNonLinearDeformerProperties
{
  static const XU32 kNumProperties=7;
  static XPropertyInfo m_Properties[kNumProperties];
public:
  PROPERTY_COUNT(XNonLinearWaveProperties,XNonLinearDeformerProperties);
  PROPERTY_TYPE(XReal);
};

XPropertyInfo XNonLinearWaveProperties::m_Properties[7] = 
{
  {XPropertyType::kReal,"min_radius",XNonLinearWave::kMinRadius,XPropertyFlags::kAnimatable},
  {XPropertyType::kReal,"max_radius",XNonLinearWave::kMaxRadius,XPropertyFlags::kAnimatable},
  {XPropertyType::kReal,"amplitude",XNonLinearWave::kAmplitude,XPropertyFlags::kAnimatable},
  {XPropertyType::kReal,"wavelength",XNonLinearWave::kWavelength,XPropertyFlags::kAnimatable},
  {XPropertyType::kReal,"dropoff",XNonLinearWave::kDropoff,XPropertyFlags::kAnimatable},
  {XPropertyType::kReal,"dropoff_position",XNonLinearWave::kDropoffPosition,XPropertyFlags::kAnimatable},
  {XPropertyType::kReal,"offset",XNonLinearWave::kOffset,XPropertyFlags::kAnimatable}
};

DECLARE_NODE_PROPERTIES(XNonLinearWave)
IMPLEMENT_PROPERTY_COMMON(XNonLinearWaveProperties,XNonLinearDeformerProperties);

bool XNonLinearWaveProperties::SetProperty(XBase* node,const XU32 property_id,const XReal& val)
{
  DO_TYPE_CHECK(node,XNonLinearWave);
  switch(property_id)
  {
  case XNonLinearWave::kMinRadius: object->SetMinRadius(val); return true;
  case XNonLinearWave::kMaxRadius: object->SetMaxRadius(val); return true;
  case XNonLinearWave::kAmplitude: object->SetAmplitude(val); return true;
  case XNonLinearWave::kWavelength: object->SetWavelength(val); return true;
  case XNonLinearWave::kDropoff: object->SetDropoff(val); return true;
  case XNonLinearWave::kDropoffPosition: object->SetDropoffPosition(val); return true;
  case XNonLinearWave::kOffset: object->SetOffset(val); return true;
  default: break;
  }
  return XBaseProperties::SetProperty(node,property_id,val);
}

bool XNonLinearWaveProperties::GetProperty(XBase* node,const XU32 property_id,XReal& val)
{
  DO_TYPE_CHECK(node,XNonLinearWave);
  switch(property_id)
  {
  case XNonLinearWave::kMinRadius: val = object->GetMinRadius(); return true;
  case XNonLinearWave::kMaxRadius: val = object->GetMaxRadius(); return true;
  case XNonLinearWave::kAmplitude: val = object->GetAmplitude(); return true;
  case XNonLinearWave::kWavelength: val = object->GetWavelength(); return true;
  case XNonLinearWave::kDropoff: val = object->GetDropoff(); return true;
  case XNonLinearWave::kDropoffPosition: val = object->GetDropoffPosition(); return true;
  case XNonLinearWave::kOffset: val = object->GetOffset(); return true;
  default: break;
  }
  return XBaseProperties::GetProperty(node,property_id,val);
}

//----------------------------------------------------------------------------------------------------------------------
class XSkinClusterProperties : public XDeformerProperties
{
  static const XU32 kNumProperties=5;
  static XPropertyInfo m_Properties[kNumProperties];
public:
  PROPERTY_COUNT(XSkinClusterProperties,XDeformerProperties);
  PROPERTY_TYPE(XRealArray);
  PROPERTY_TYPE(XU32Array);
  PROPERTY_TYPE(XBase*);
  PROPERTY_TYPE(XList);
};

XPropertyInfo XSkinClusterProperties::m_Properties[5] = 
{
  {XPropertyType::kNodeArray,"influences",XSkinCluster::kInfluences,0,XFn::Bone},
  {XPropertyType::kNode,"affected",XSkinCluster::kSkinnedGeometry,0,XFn::Geometry},
  {XPropertyType::kUnsignedArray,"indices",XSkinCluster::kVertexIndices,0},
  {XPropertyType::kRealArray,"weights",XSkinCluster::kWeights,0},
  {XPropertyType::kUnsignedArray,"joint_indices",XSkinCluster::kJointIndices,0}
};

DECLARE_NODE_PROPERTIES(XSkinCluster)
IMPLEMENT_PROPERTY_COMMON(XSkinClusterProperties,XDeformerProperties);

bool XSkinClusterProperties::SetProperty(XBase* node,const XU32 property_id,const XU32Array& val)
{
  DO_TYPE_CHECK(node,XSkinCluster);
  switch(property_id)
  {
  case XSkinCluster::kVertexIndices: XMD_ASSERT(0); return true;
  case XSkinCluster::kJointIndices:  XMD_ASSERT(0); return true;
  default: break;
  }
  return XBaseProperties::SetProperty(node,property_id,val);
}

bool XSkinClusterProperties::GetProperty(XBase* node,const XU32 property_id,XU32Array& val)
{
  DO_TYPE_CHECK(node,XSkinCluster);
  switch(property_id)
  {
  case XSkinCluster::kVertexIndices: XMD_ASSERT(0); return true;
  case XSkinCluster::kJointIndices:  XMD_ASSERT(0); return true;
  default: break;
  }
  return XBaseProperties::GetProperty(node,property_id,val);
}

bool XSkinClusterProperties::SetProperty(XBase* node,const XU32 property_id,const XList& val)
{
  DO_TYPE_CHECK(node,XSkinCluster);
  switch(property_id)
  {
  case XSkinCluster::kInfluences: 
    {
      XBoneList bones;
      for (XU32 i=0;i!=val.size();++i)
      {
        if(val[i])
        {
          XBone* bone = val[i]->HasFn<XBone>();
          if(bone) bones.push_back(bone);
        }
      }
      object->SetInfluences(bones);
    }
    return true;

  default: break;
  }
  return XBaseProperties::SetProperty(node,property_id,val);
}

bool XSkinClusterProperties::GetProperty(XBase* node,const XU32 property_id,XList& val)
{
  DO_TYPE_CHECK(node,XSkinCluster);
  switch(property_id)
  {
  case XSkinCluster::kInfluences: 
    {
      XBoneList bones;
      object->GetInfluences(bones);
      XBoneList::iterator it = bones.begin();
      for (;it!=bones.end();++it)
      {
        val.push_back(*it);
      }
    }
    return true;
  default: break;
  }
  return XBaseProperties::GetProperty(node,property_id,val);
}

bool XSkinClusterProperties::SetProperty(XBase* node,const XU32 property_id,const XBase*& val)
{
  DO_TYPE_CHECK(node,XSkinCluster);
  switch(property_id)
  {
  case XSkinCluster::kVertexIndices: 
    if(val->HasFn<XGeometry>()) 
      object->SetAffected(val->HasFn<XGeometry>()); 
    else
      return false;
    return true;
  default: break;
  }
  return XBaseProperties::SetProperty(node,property_id,val);
}

bool XSkinClusterProperties::GetProperty(XBase* node,const XU32 property_id,XBase*& val)
{
  DO_TYPE_CHECK(node,XSkinCluster);
  switch(property_id)
  {
  case XSkinCluster::kVertexIndices: val = object->GetAffected(); return true;
  default: break;
  }
  return XBaseProperties::GetProperty(node,property_id,val);
}

bool XSkinClusterProperties::SetProperty(XBase* node,const XU32 property_id,const XRealArray& val)
{
  DO_TYPE_CHECK(node,XSkinCluster);
  switch(property_id)
  {
  case XSkinCluster::kVertexIndices: 
    {
      // MORPH-21321: property accessors
    }
    return true;
  case XSkinCluster::kJointIndices:   
    {
      // MORPH-21321: property accessors
    }
    return true;
  default: break;
  }
  return XBaseProperties::SetProperty(node,property_id,val);
}

bool XSkinClusterProperties::GetProperty(XBase* node,const XU32 property_id,XRealArray& val)
{
  DO_TYPE_CHECK(node,XSkinCluster);
  switch(property_id)
  {
  case XSkinCluster::kVertexIndices: 
    {
      // MORPH-21321: property accessors
    }
    return true;
  case XSkinCluster::kJointIndices:   
    {
      // MORPH-21321: property accessors
    }
    return true;
  default: break;
  }
  return XBaseProperties::GetProperty(node,property_id,val);
}

//----------------------------------------------------------------------------------------------------------------------
class XSculptDeformerProperties : public XDeformerProperties
{
  static const XU32 kNumProperties=8;
  static XPropertyInfo m_Properties[kNumProperties];
public:
  PROPERTY_COUNT(XSculptDeformerProperties,XDeformerProperties);
  PROPERTY_TYPE(XReal);
  PROPERTY_TYPE(XVector3);
  PROPERTY_TYPE(XU32);
  PROPERTY_TYPE(XBase*);
};

XPropertyInfo XSculptDeformerProperties::m_Properties[8] = 
{
  {XPropertyType::kReal,"max_displacement",XSculptDeformer::kMaxDisplacement,XPropertyFlags::kAnimatable},
  {XPropertyType::kVector3,"start_position",XSculptDeformer::kStartPosition,XPropertyFlags::kAnimatable},
  {XPropertyType::kReal,"dropoff_distance",XSculptDeformer::kDropoffDistance,XPropertyFlags::kAnimatable},
  {XPropertyType::kUnsigned,"mode",XSculptDeformer::kMode,0},
  {XPropertyType::kUnsigned,"inside_mode",XSculptDeformer::kInsideMode,0},
  {XPropertyType::kUnsigned,"dropoff_type",XSculptDeformer::kDropoffType,0},
  {XPropertyType::kNode,"sculpt_object",XSculptDeformer::kSculptObject,0,XFn::Shape},
  {XPropertyType::kNode,"start_object",XSculptDeformer::kStartObject,0,XFn::Shape}
};

DECLARE_NODE_PROPERTIES(XSculptDeformer)
IMPLEMENT_PROPERTY_COMMON(XSculptDeformerProperties,XDeformerProperties);

bool XSculptDeformerProperties::SetProperty(XBase* node,const XU32 property_id,const XU32& val)
{
  DO_TYPE_CHECK(node,XSculptDeformer);
  switch(property_id)
  {
  case XSculptDeformer::kMode:        object->SetMode(val);        return true;
  case XSculptDeformer::kInsideMode:  object->SetInsideMode(val);  return true;
  case XSculptDeformer::kDropoffType: object->SetDropoffType(val); return true;
  default: break;
  }
  return XBaseProperties::SetProperty(node,property_id,val);
}

bool XSculptDeformerProperties::GetProperty(XBase* node,const XU32 property_id,XU32& val)
{
  DO_TYPE_CHECK(node,XSculptDeformer);
  switch(property_id)
  {
  case XSculptDeformer::kMode:        val = object->GetMode();        return true;
  case XSculptDeformer::kInsideMode:  val = object->GetInsideMode();  return true;
  case XSculptDeformer::kDropoffType: val = object->GetDropoffType(); return true;
  default: break;
  }
  return XBaseProperties::GetProperty(node,property_id,val);
}

bool XSculptDeformerProperties::SetProperty(XBase* node,const XU32 property_id,const XBase*& val)
{
  DO_TYPE_CHECK(node,XSculptDeformer);
  switch(property_id)
  {
  case XSculptDeformer::kSculptObject: object->SetSculptObject(val); return true;
  case XSculptDeformer::kStartObject:  object->SetStartObject(val);  return true;
  default: break;
  }
  return XBaseProperties::SetProperty(node,property_id,val);
}

bool XSculptDeformerProperties::GetProperty(XBase* node,const XU32 property_id,XBase*& val)
{
  DO_TYPE_CHECK(node,XSculptDeformer);
  switch(property_id)
  {
  case XSculptDeformer::kSculptObject: val = object->GetSculptObject(); return true;
  case XSculptDeformer::kStartObject:  val = object->GetStartObject();  return true;
  default: break;
  }
  return XBaseProperties::GetProperty(node,property_id,val);
}

bool XSculptDeformerProperties::SetProperty(XBase* node,const XU32 property_id,const XReal& val)
{
  DO_TYPE_CHECK(node,XSculptDeformer);
  switch(property_id)
  {
  case XSculptDeformer::kMaxDisplacement: object->SetMaxDisplacement(val); return true;
  case XSculptDeformer::kDropoffDistance: object->SetDropoffDistance(val); return true;
  default: break;
  }
  return XBaseProperties::SetProperty(node,property_id,val);
}

bool XSculptDeformerProperties::GetProperty(XBase* node,const XU32 property_id,XReal& val)
{
  DO_TYPE_CHECK(node,XSculptDeformer);
  switch(property_id)
  {
  case XSculptDeformer::kMaxDisplacement: val = object->GetMaxDisplacement(); return true;
  case XSculptDeformer::kDropoffDistance: val = object->GetDropoffDistance(); return true;
  default: break;
  }
  return XBaseProperties::GetProperty(node,property_id,val);
}

bool XSculptDeformerProperties::SetProperty(XBase* node,const XU32 property_id,const XVector3& val)
{
  DO_TYPE_CHECK(node,XSculptDeformer);
  switch(property_id)
  {
  case XSculptDeformer::kStartPosition: object->SetStartPosition(val); return true;
  default: break;
  }
  return XBaseProperties::SetProperty(node,property_id,val);
}

bool XSculptDeformerProperties::GetProperty(XBase* node,const XU32 property_id,XVector3& val)
{
  DO_TYPE_CHECK(node,XSculptDeformer);
  switch(property_id)
  {
  case XSculptDeformer::kStartPosition: val = object->GetStartPosition(); return true;
  default: break;
  }
  return XBaseProperties::GetProperty(node,property_id,val);
}

//----------------------------------------------------------------------------------------------------------------------
class XWireDeformerProperties : public XDeformerProperties
{
  static const XU32 kNumProperties=7;
  static XPropertyInfo m_Properties[kNumProperties];
public:
  PROPERTY_COUNT(XWireDeformerProperties,XDeformerProperties);
  PROPERTY_TYPE(XReal);
  PROPERTY_TYPE(XRealArray);
  PROPERTY_TYPE(XList);
};

XPropertyInfo XWireDeformerProperties::m_Properties[7] = 
{
  {XPropertyType::kRealArray,"wire_scale",XWireDeformer::kWireScale,XPropertyFlags::kAnimatable},
  {XPropertyType::kRealArray,"wire_dropoff_distance",XWireDeformer::kWireDropoffDistance,XPropertyFlags::kAnimatable},
  {XPropertyType::kReal,"local_intensity",XWireDeformer::kLocalIntensity,XPropertyFlags::kAnimatable},
  {XPropertyType::kReal,"rotation",XWireDeformer::kRotation,XPropertyFlags::kAnimatable},
  {XPropertyType::kNodeArray,"wires",XWireDeformer::kWires,XPropertyFlags::kArrayIsResizable},
  {XPropertyType::kReal,"crossing_effect",XWireDeformer::kCrossingEffect,0},
  {XPropertyType::kNodeArray,"base_wires",XWireDeformer::kWires,XPropertyFlags::kArrayIsResizable}
};

DECLARE_NODE_PROPERTIES(XWireDeformer)
IMPLEMENT_PROPERTY_COMMON(XWireDeformerProperties,XDeformerProperties);

bool XWireDeformerProperties::SetProperty(XBase* node,const XU32 property_id,const XReal& val)
{
  DO_TYPE_CHECK(node,XWireDeformer);
  switch(property_id)
  {
  case XWireDeformer::kLocalIntensity: object->SetLocalIntensity(val); return true;
  case XWireDeformer::kRotation:       object->SetRotation(val);       return true;
  case XWireDeformer::kCrossingEffect: object->SetCrossingEffect(val); return true;
  default: break;
  }
  return XBaseProperties::SetProperty(node,property_id,val);
}

bool XWireDeformerProperties::GetProperty(XBase* node,const XU32 property_id,XReal& val)
{
  DO_TYPE_CHECK(node,XWireDeformer);
  switch(property_id)
  {
  case XWireDeformer::kLocalIntensity: val = object->GetLocalIntensity(); return true;
  case XWireDeformer::kRotation:       val = object->GetRotation();       return true;
  case XWireDeformer::kCrossingEffect: val = object->GetCrossingEffect(); return true;
  default: break;
  }
  return XBaseProperties::GetProperty(node,property_id,val);
}

bool XWireDeformerProperties::SetProperty(XBase* node,const XU32 property_id,const XList& val)
{
  DO_TYPE_CHECK(node,XWireDeformer);
  switch(property_id)
  {
  case XWireDeformer::kBaseWires: 
    {
      XList wires;
      XList bwires;
      object->GetWires(wires);
      object->GetBaseWires(bwires);
      if (bwires.size() == val.size())
      {
        object->SetWires(wires,val);
      }
      else
      {
        object->SetWires(val,val);
      }
    }
    return true;
  case XWireDeformer::kWires: 
    {
      XList wires;
      XList bwires;
      object->GetWires(wires);
      object->GetBaseWires(bwires);
      if (bwires.size() == val.size())
      {
        object->SetWires(val,bwires);
      }
      else
      {
        object->SetWires(val,val);
      }
    }
    return true;
  default: break;
  }
  return XDeformerProperties::SetProperty(node,property_id,val);
}

bool XWireDeformerProperties::GetProperty(XBase* node,const XU32 property_id,XList& val)
{
  DO_TYPE_CHECK(node,XWireDeformer);
  switch(property_id)
  {
  case XWireDeformer::kWires: object->GetWires(val); return true;
  case XWireDeformer::kBaseWires: object->GetBaseWires(val); return true;
  default: break;
  }
  return XDeformerProperties::GetProperty(node,property_id,val);
}

bool XWireDeformerProperties::SetProperty(XBase* node,const XU32 property_id,const XRealArray& val)
{
  DO_TYPE_CHECK(node,XWireDeformer);
  switch(property_id)
  {
  case XWireDeformer::kWireScale: 
    {
      if(val.size() != object->GetNumWires())
      {
        XGlobal::GetLogger()->Logf(XBasicLogger::kError,
          "cannot set the %s.wireScale attr because the array provided does not match the number of wires",
          node->GetName());
        return false;
      }
      for(XU32 i=0;i<val.size();++i)
        object->SetWireScale(i,val[i]);        
      return true;
    }
  case XWireDeformer::kWireDropoffDistance: 
    {
      if(val.size() != object->GetNumWires())
      {
        XGlobal::GetLogger()->Logf(XBasicLogger::kError,
          "cannot set the %s.wireDropoffDistanc attr because the array provided does not match the number of wires",
          node->GetName());
        return false;
      }
      for(XU32 i=0;i<val.size();++i)
        object->SetWireDropoffDistance(i,val[i]);        
      return true;
    }
  default: break;
  }
  return XBaseProperties::SetProperty(node,property_id,val);
}

bool XWireDeformerProperties::GetProperty(XBase* node,const XU32 property_id,XRealArray& val)
{
  DO_TYPE_CHECK(node,XWireDeformer);
  switch(property_id)
  {
  case XWireDeformer::kWireScale:
    {
      val.resize(object->GetNumWires());
      for(XU32 i=0;i<val.size();++i)
        val[i] = object->GetWireScale(i); 
    }
    return true;
  case XWireDeformer::kWireDropoffDistance: 
    {
      val.resize(object->GetNumWires());
      for(XU32 i=0;i<val.size();++i)
        val[i] = object->GetWireDropoffDistance(i); 
    }
    return true;
  default: break;
  }
  return XBaseProperties::GetProperty(node,property_id,val);
}

//----------------------------------------------------------------------------------------------------------------------
class XWrapDeformerProperties : public XDeformerProperties
{
  static const XU32 kNumProperties=7;
  static XPropertyInfo m_Properties[kNumProperties];
public:
  PROPERTY_COUNT(XWrapDeformerProperties,XDeformerProperties);
  PROPERTY_TYPE(XReal);
  PROPERTY_TYPE(XU32);
  PROPERTY_TYPE(XBase*);
};

XPropertyInfo XWrapDeformerProperties::m_Properties[7] = 
{
  {XPropertyType::kReal,"dropoff",XWrapDeformer::kDropoff,XPropertyFlags::kAnimatable},
  {XPropertyType::kReal,"smoothness",XWrapDeformer::kSmoothness,XPropertyFlags::kAnimatable},
  {XPropertyType::kReal,"weight_threshold",XWrapDeformer::kWeightThreshold,XPropertyFlags::kAnimatable},
  {XPropertyType::kReal,"max_distance",XWrapDeformer::kMaxDistance,XPropertyFlags::kAnimatable},
  {XPropertyType::kUnsigned,"influence_type",XWrapDeformer::kInfluenceType,0},
  {XPropertyType::kUnsigned,"nurbs_samples",XWrapDeformer::kNurbsSamples,0},
  {XPropertyType::kNode,"driver_geometry",XWrapDeformer::kDriver,0,XFn::Geometry}
};

DECLARE_NODE_PROPERTIES(XWrapDeformer)
IMPLEMENT_PROPERTY_COMMON(XWrapDeformerProperties,XDeformerProperties);

bool XWrapDeformerProperties::SetProperty(XBase* node,const XU32 property_id,const XReal& val)
{
  DO_TYPE_CHECK(node,XWrapDeformer);
  switch(property_id)
  {
  case XWrapDeformer::kDropoff: object->SetDropoff(val); return true;
  case XWrapDeformer::kSmoothness: object->SetSmoothness(val); return true;
  case XWrapDeformer::kWeightThreshold: object->SetWeightThreshold(val); return true;
  case XWrapDeformer::kMaxDistance: object->SetMaxDistance(val); return true;
  default: break;
  }
  return XBaseProperties::SetProperty(node,property_id,val);
}

bool XWrapDeformerProperties::GetProperty(XBase* node,const XU32 property_id,XReal& val)
{
  DO_TYPE_CHECK(node,XWrapDeformer);
  switch(property_id)
  {
  case XWrapDeformer::kDropoff: val = object->GetDropoff(); return true;
  case XWrapDeformer::kSmoothness: val = object->GetSmoothness(); return true;
  case XWrapDeformer::kWeightThreshold: val = object->GetWeightThreshold(); return true;
  case XWrapDeformer::kMaxDistance: val = object->GetMaxDistance(); return true;
  default: break;
  }
  return XBaseProperties::GetProperty(node,property_id,val);
}

bool XWrapDeformerProperties::SetProperty(XBase* node,const XU32 property_id,const XU32& val)
{
  DO_TYPE_CHECK(node,XWrapDeformer);
  switch(property_id)
  {
  case XWrapDeformer::kInfluenceType: object->SetInfluenceType(val); return true;
  case XWrapDeformer::kNurbsSamples:  object->SetNurbsSamples(val);  return true;
  default: break;
  }
  return XBaseProperties::SetProperty(node,property_id,val);
}

bool XWrapDeformerProperties::GetProperty(XBase* node,const XU32 property_id,XU32& val)
{
  DO_TYPE_CHECK(node,XWrapDeformer);
  switch(property_id)
  {
  case XWrapDeformer::kInfluenceType: val = object->GetInfluenceType(); return true;
  case XWrapDeformer::kNurbsSamples:  val = object->GetNurbsSamples();  return true;
  default: break;
  }
  return XBaseProperties::GetProperty(node,property_id,val);
}

bool XWrapDeformerProperties::SetProperty(XBase* node,const XU32 property_id,const XBase*& val)
{
  DO_TYPE_CHECK(node,XWrapDeformer);
  switch(property_id)
  {
  case XWrapDeformer::kDriver: object->SetDriver(val); return true;
  default: break;
  }
  return XBaseProperties::SetProperty(node,property_id,val);
}

bool XWrapDeformerProperties::GetProperty(XBase* node,const XU32 property_id,XBase*& val)
{
  DO_TYPE_CHECK(node,XWrapDeformer);
  switch(property_id)
  {
  case XWrapDeformer::kDriver: val = object->GetDriver(); return true;
  default: break;
  }
  return XBaseProperties::GetProperty(node,property_id,val);
}

//----------------------------------------------------------------------------------------------------------------------
class XNodeCollectionProperties : public XBaseProperties
{
  static const XU32 kNumProperties=1;
  static XPropertyInfo m_Properties[kNumProperties];
public:
  PROPERTY_COUNT(XNodeCollectionProperties,XBaseProperties);
  PROPERTY_TYPE(XList);
};

XPropertyInfo XNodeCollectionProperties::m_Properties[1] = 
{
  {XPropertyType::kNodeArray,"items",XNodeCollection::kItems,XPropertyFlags::kArrayIsResizable,XFn::Bone}
};

DECLARE_NODE_PROPERTIES(XNodeCollection)
IMPLEMENT_PROPERTY_COMMON(XNodeCollectionProperties,XBaseProperties);

bool XNodeCollectionProperties::SetProperty(XBase* node,const XU32 property_id,const XList& val)
{
  DO_TYPE_CHECK(node,XNodeCollection);
  switch(property_id)
  {
  case XNodeCollection::kItems:
    {
      object->SetItems(val); 
      return true;
    }
  default: break;
  }
  return XBaseProperties::SetProperty(node,property_id,val);
}

bool XNodeCollectionProperties::GetProperty(XBase* node,const XU32 property_id,XList& val)
{
  DO_TYPE_CHECK(node,XNodeCollection);
  switch(property_id)
  {
  case XNodeCollection::kItems: 
    {
      object->GetItems(val);
      return true;
    }

  default: break;
  }
  return XBaseProperties::GetProperty(node,property_id,val);
}

//----------------------------------------------------------------------------------------------------------------------
class XIkChainProperties : public XBaseProperties
{
  static const XU32 kNumProperties=10;
  static XPropertyInfo m_Properties[kNumProperties];
public:
  PROPERTY_COUNT(XIkChainProperties,XBaseProperties);
  PROPERTY_TYPE(XList);
  PROPERTY_TYPE(XBase*);
  PROPERTY_TYPE(XS32);
  PROPERTY_TYPE(XU32);
  PROPERTY_TYPE(XReal);
  PROPERTY_TYPE(XVector3);
};

XPropertyInfo XIkChainProperties::m_Properties[10] = 
{
  {XPropertyType::kNodeArray,"chain",XIkChain::kChain,XPropertyFlags::kArrayIsResizable,XFn::Bone},
  {XPropertyType::kNode,"handle",XIkChain::kHandle,0,XFn::Bone},
  {XPropertyType::kInt,"priority",XIkChain::kPriority,0},
  {XPropertyType::kUnsigned,"max_iterations",XIkChain::kMaxIterations,0},
  {XPropertyType::kReal,"weight",XIkChain::kWeight,0},
  {XPropertyType::kReal,"poweight",XIkChain::kPoWeight,0},
  {XPropertyType::kReal,"tolerance",XIkChain::kTolerance,0},
  {XPropertyType::kReal,"stickiness",XIkChain::kStickiness,0},
  {XPropertyType::kVector3,"pole_vector",XIkChain::kPoleVector,0},
  {XPropertyType::kNode,"pole_constraint",XIkChain::kPoleConstraint,0,XFn::Bone}
};

DECLARE_NODE_PROPERTIES(XIkChain)
IMPLEMENT_PROPERTY_COMMON(XIkChainProperties,XBaseProperties);

bool XIkChainProperties::SetProperty(XBase* node,const XU32 property_id,const XList& val)
{
  DO_TYPE_CHECK(node,XIkChain);
  switch(property_id)
  {
  case XIkChain::kChain:
    {
      XBoneList items;
      XList::const_iterator it = val.begin();
      for (;it != val.end(); ++it)
      {
        const XBase* base = *it;
        const XBone* bone = base ? base->HasFn<XBone>() : 0 ;
        if(bone)
          items.push_back( (XBone*)bone );
      }
      object->SetBonesInChain(items); 
      return true;
    }
  default: break;
  }
  return XBaseProperties::SetProperty(node,property_id,val);
}

bool XIkChainProperties::GetProperty(XBase* node,const XU32 property_id,XList& val)
{
  DO_TYPE_CHECK(node,XIkChain);
  switch(property_id)
  {
  case XIkChain::kChain: 
    {
      XBoneList items;
      val.clear();
      object->GetBonesInChain(items);
      XBoneList::iterator it = items.begin();
      for (;it != items.end(); ++it)
      {
        val.push_back( *it );
      }
      return true;
    }

  default: break;
  }
  return XBaseProperties::GetProperty(node,property_id,val);
}
// unsigned
bool XIkChainProperties::SetProperty(XBase* node,const XU32 property_id,const XU32& val)
{
  DO_TYPE_CHECK(node,XIkChain);
  switch(property_id)
  {
  case XIkChain::kMaxIterations: object->SetMaxIterations(val); return true;
  default: break;
  }
  return XBaseProperties::SetProperty(node,property_id,val);
}

bool XIkChainProperties::GetProperty(XBase* node,const XU32 property_id,XU32& val)
{
  DO_TYPE_CHECK(node,XIkChain);
  switch(property_id)
  {
  case XIkChain::kMaxIterations: val = object->GetMaxIterations(); return true;
  default: break;
  }
  return XBaseProperties::GetProperty(node,property_id,val);
}

bool XIkChainProperties::SetProperty(XBase* node,const XU32 property_id,const XS32& val)
{
  DO_TYPE_CHECK(node,XIkChain);
  switch(property_id)
  {
  case XIkChain::kPriority: object->SetPriority(val); return true;
  default: break;
  }
  return XBaseProperties::SetProperty(node,property_id,val);
}

bool XIkChainProperties::GetProperty(XBase* node,const XU32 property_id,XS32& val)
{
  DO_TYPE_CHECK(node,XIkChain);
  switch(property_id)
  {
  case XIkChain::kPriority: val = object->GetPriority(); return true;
  default: break;
  }
  return XBaseProperties::GetProperty(node,property_id,val);
}

// real
bool XIkChainProperties::SetProperty(XBase* node,const XU32 property_id,const XReal& val)
{
  DO_TYPE_CHECK(node,XIkChain);
  switch(property_id)
  {
  case XIkChain::kWeight:     object->SetWeight(val);     return true;
  case XIkChain::kPoWeight:   object->SetPoWeight(val);   return true;
  case XIkChain::kTolerance:  object->SetTolerance(val);  return true;
  case XIkChain::kStickiness: object->SetStickiness(val); return true;
  default: break;
  }
  return XBaseProperties::SetProperty(node,property_id,val);
}

bool XIkChainProperties::GetProperty(XBase* node,const XU32 property_id,XReal& val)
{
  DO_TYPE_CHECK(node,XIkChain);
  switch(property_id)
  {
  case XIkChain::kWeight:     val = object->GetWeight();     return true;
  case XIkChain::kPoWeight:   val = object->GetPoWeight();   return true;
  case XIkChain::kTolerance:  val = object->GetTolerance();  return true;
  case XIkChain::kStickiness: val = object->GetStickiness(); return true;
  default: break;
  }
  return XBaseProperties::GetProperty(node,property_id,val);
}

// vector3
bool XIkChainProperties::SetProperty(XBase* node,const XU32 property_id,const XVector3& val)
{
  DO_TYPE_CHECK(node,XIkChain);
  switch(property_id)
  {
  case XIkChain::kPoleVector: object->SetPoleVector(val); return true;
  default: break;
  }
  return XBaseProperties::SetProperty(node,property_id,val);
}

bool XIkChainProperties::GetProperty(XBase* node,const XU32 property_id,XVector3& val)
{
  DO_TYPE_CHECK(node,XIkChain);
  switch(property_id)
  {
  case XIkChain::kPoleVector: val = object->GetPoleVector(); return true;
  default: break;
  }
  return XBaseProperties::GetProperty(node,property_id,val);
}

// node
bool XIkChainProperties::SetProperty(XBase* node,const XU32 property_id,const XBase*& val)
{
  DO_TYPE_CHECK(node,XIkChain);
  switch(property_id)
  {
  case XIkChain::kPoleConstraint: object->SetPoleConstraint(val); return true;
  case XIkChain::kHandle:         object->SetHandle(val);         return true;
  default: break;
  }
  return XBaseProperties::SetProperty(node,property_id,val);
}

bool XIkChainProperties::GetProperty(XBase* node,const XU32 property_id,XBase*& val)
{
  DO_TYPE_CHECK(node,XIkChain);
  switch(property_id)
  {
  case XIkChain::kPoleConstraint: val = object->GetPoleConstraint(); return true;
  case XIkChain::kHandle:         val = object->GetHandle();         return true;
  default: break;
  }
  return XBaseProperties::GetProperty(node,property_id,val);
}

//----------------------------------------------------------------------------------------------------------------------
class XMaterialProperties : public XBaseProperties
{
  static const XU32 kNumProperties=2;
  static XPropertyInfo m_Properties[kNumProperties];
public:
  PROPERTY_COUNT(XMaterialProperties,XBaseProperties);
  PROPERTY_TYPE(XColour);
  PROPERTY_TYPE(XBase*);
};

XPropertyInfo XMaterialProperties::m_Properties[2] = 
{
  {XPropertyType::kNode,"hardwareShader",XMaterial::kHardwareShader,0},
  {XPropertyType::kColour,"diffuse",XMaterial::kDiffuse,XPropertyFlags::kAnimatable}
};

DECLARE_NODE_PROPERTIES(XMaterial)
IMPLEMENT_PROPERTY_COMMON(XMaterialProperties,XBaseProperties);

bool XMaterialProperties::SetProperty(XBase* node,const XU32 property_id,const XColour& val)
{
  DO_TYPE_CHECK(node,XMaterial);
  switch(property_id)
  {
  case XMaterial::kDiffuse: object->SetDiffuse(val); return true;
  default: break;
  }
  return XBaseProperties::SetProperty(node,property_id,val);
}

bool XMaterialProperties::GetProperty(XBase* node,const XU32 property_id,XColour& val)
{
  DO_TYPE_CHECK(node,XMaterial);
  switch(property_id)
  {
  case XMaterial::kDiffuse: object->GetDiffuse(val); return true;
  default: break;
  }
  return XBaseProperties::GetProperty(node,property_id,val);
}

bool XMaterialProperties::SetProperty(XBase* node,const XU32 property_id,const XBase*& val)
{
  DO_TYPE_CHECK(node,XMaterial);
  switch(property_id)
  {
  case XMaterial::kHardwareShader: object->SetHwShader(val); return true;
  default: break;
  }
  return XBaseProperties::SetProperty(node,property_id,val);
}

bool XMaterialProperties::GetProperty(XBase* node,const XU32 property_id,XBase*& val)
{
  DO_TYPE_CHECK(node,XMaterial);
  switch(property_id)
  {
  case XMaterial::kHardwareShader: val = object->GetHwShader(); return true;
  default: break;
  }
  return XBaseProperties::GetProperty(node,property_id,val);
}

//----------------------------------------------------------------------------------------------------------------------
class XSurfaceShaderProperties : public XMaterialProperties
{
  static const XU32 kNumProperties=1;
  static XPropertyInfo m_Properties[kNumProperties];
public:
  PROPERTY_COUNT(XSurfaceShaderProperties,XMaterialProperties);
  PROPERTY_TYPE(XColour);
};

XPropertyInfo XSurfaceShaderProperties::m_Properties[1] = 
{
  {XPropertyType::kColour,"transparency",XSurfaceShader::kTransparency,XPropertyFlags::kAnimatable}
};

DECLARE_NODE_PROPERTIES(XSurfaceShader)
IMPLEMENT_PROPERTY_COMMON(XSurfaceShaderProperties,XMaterialProperties);

bool XSurfaceShaderProperties::SetProperty(XBase* node,const XU32 property_id,const XColour& val)
{
  DO_TYPE_CHECK(node,XSurfaceShader);
  switch(property_id)
  {
  case XSurfaceShader::kTransparency: object->SetTransparency(val); return true;
  default: break;
  }
  return XMaterialProperties::SetProperty(node,property_id,val);
}

bool XSurfaceShaderProperties::GetProperty(XBase* node,const XU32 property_id,XColour& val)
{
  DO_TYPE_CHECK(node,XSurfaceShader);
  switch(property_id)
  {
  case XSurfaceShader::kTransparency: object->GetTransparency(val); return true;
  default: break;
  }
  return XMaterialProperties::GetProperty(node,property_id,val);
}

//----------------------------------------------------------------------------------------------------------------------
class XLambertProperties : public XMaterialProperties
{
  static const XU32 kNumProperties=5;
  static XPropertyInfo m_Properties[kNumProperties];
public:
  PROPERTY_COUNT(XLambertProperties,XMaterialProperties);
  PROPERTY_TYPE(XColour);
  PROPERTY_TYPE(XBase*);
};

XPropertyInfo XLambertProperties::m_Properties[5] = 
{
  {XPropertyType::kColour,"ambient",XLambert::kAmbient,XPropertyFlags::kAnimatable},
  {XPropertyType::kColour,"emission",XLambert::kEmission,XPropertyFlags::kAnimatable},
  {XPropertyType::kColour,"transparency",XLambert::kTransparency,XPropertyFlags::kAnimatable},
  {XPropertyType::kNode,"bump_map",XLambert::kBumpMap,0,XMD::XFn::BumpMap},
  {XPropertyType::kNode,"env_map",XLambert::kEnvMap,0,XMD::XFn::EnvMap}
};

DECLARE_NODE_PROPERTIES(XLambert)
IMPLEMENT_PROPERTY_COMMON(XLambertProperties,XMaterialProperties);

bool XLambertProperties::SetProperty(XBase* node,const XU32 property_id,const XBase*& val)
{
  DO_TYPE_CHECK(node,XLambert);
  switch(property_id)
  {
  case XLambert::kBumpMap:  object->SetBumpMap(val->HasFn<XBumpMap>()); return true;
  case XLambert::kEnvMap:   object->SetEnvMap(val->HasFn<XEnvmapBaseTexture>()); return true;
  default: break;
  }
  return XMaterialProperties::SetProperty(node,property_id,val);
}

bool XLambertProperties::GetProperty(XBase* node,const XU32 property_id,XBase*& val)
{
  DO_TYPE_CHECK(node,XLambert);
  switch(property_id)
  {
  case XLambert::kBumpMap: val = object->GetBumpMap(); return true;
  case XLambert::kEnvMap:  val = object->GetEnvMap();  return true;
  default: break;
  }
  return XMaterialProperties::GetProperty(node,property_id,val);
}

bool XLambertProperties::SetProperty(XBase* node,const XU32 property_id,const XColour& val)
{
  DO_TYPE_CHECK(node,XLambert);
  switch(property_id)
  {
  case XLambert::kAmbient:      object->SetAmbient(val); return true;
  case XLambert::kEmission:     object->SetEmission(val); return true;
  case XLambert::kTransparency: object->SetTransparency(val); return true;
  default: break;
  }
  return XMaterialProperties::SetProperty(node,property_id,val);
}

bool XLambertProperties::GetProperty(XBase* node,const XU32 property_id,XColour& val)
{
  DO_TYPE_CHECK(node,XLambert);
  switch(property_id)
  {
  case XLambert::kAmbient:      object->GetAmbient(val); return true;
  case XLambert::kEmission:     object->GetEmission(val); return true;
  case XLambert::kTransparency: object->GetTransparency(val); return true;
  default: break;
  }
  return XMaterialProperties::GetProperty(node,property_id,val);
}

//----------------------------------------------------------------------------------------------------------------------
class XSpecularProperties : public XLambertProperties
{
  static const XU32 kNumProperties=2;
  static XPropertyInfo m_Properties[kNumProperties];
public:
  PROPERTY_COUNT(XSpecularProperties,XLambertProperties);
  PROPERTY_TYPE(XColour);
  PROPERTY_TYPE(XReal);
};

XPropertyInfo XSpecularProperties::m_Properties[2] = 
{
  {XPropertyType::kColour,"specular",XSpecular::kSpecular,XPropertyFlags::kAnimatable},
  {XPropertyType::kReal,"shininess",XSpecular::kShininess,XPropertyFlags::kAnimatable}
};

DECLARE_NODE_PROPERTIES(XSpecular)
IMPLEMENT_PROPERTY_COMMON(XSpecularProperties,XLambertProperties);

bool XSpecularProperties::SetProperty(XBase* node,const XU32 property_id,const XColour& val)
{
  DO_TYPE_CHECK(node,XSpecular);
  switch(property_id)
  {
  case XSpecular::kSpecular: object->SetSpecular(val); return true;
  default: break;
  }
  return XLambertProperties::SetProperty(node,property_id,val);
}

bool XSpecularProperties::GetProperty(XBase* node,const XU32 property_id,XColour& val)
{
  DO_TYPE_CHECK(node,XSpecular);
  switch(property_id)
  {
  case XSpecular::kSpecular: object->GetSpecular(val); return true;
  default: break;
  }
  return XLambertProperties::GetProperty(node,property_id,val);
}

bool XSpecularProperties::SetProperty(XBase* node,const XU32 property_id,const XReal& val)
{
  DO_TYPE_CHECK(node,XSpecular);
  switch(property_id)
  {
  case XSpecular::kShininess: object->SetShininess(val); return true;
  default: break;
  }
  return XBaseProperties::SetProperty(node,property_id,val);
}

bool XSpecularProperties::GetProperty(XBase* node,const XU32 property_id,XReal& val)
{
  DO_TYPE_CHECK(node,XSpecular);
  switch(property_id)
  {
  case XSpecular::kShininess: val = object->GetShininess(); return true;
  default: break;
  }
  return XBaseProperties::GetProperty(node,property_id,val);
}

//----------------------------------------------------------------------------------------------------------------------
class XAnisotropicProperties : public XSpecularProperties
{
  static const XU32 kNumProperties=7;
  static XPropertyInfo m_Properties[kNumProperties];
public:
  PROPERTY_COUNT(XAnisotropicProperties,XSpecularProperties);
  PROPERTY_TYPE(XColour);
  PROPERTY_TYPE(XReal);
};

XPropertyInfo XAnisotropicProperties::m_Properties[7] = 
{
  {XPropertyType::kColour,"tangentu",XAnisotropic::kTangentUCamera,XPropertyFlags::kAnimatable},
  {XPropertyType::kColour,"tangentv",XAnisotropic::kTangentVCamera,XPropertyFlags::kAnimatable},
  {XPropertyType::kReal,"correlation_x",XAnisotropic::kCorrelationX,XPropertyFlags::kAnimatable},
  {XPropertyType::kReal,"correlation_y",XAnisotropic::kCorrelationY,XPropertyFlags::kAnimatable},
  {XPropertyType::kReal,"roughness",XAnisotropic::kRoughness,XPropertyFlags::kAnimatable},
  {XPropertyType::kReal,"rotate_angle",XAnisotropic::kRotateAngle,XPropertyFlags::kAnimatable},
  {XPropertyType::kReal,"refractive_index",XAnisotropic::kRefractiveIndex,XPropertyFlags::kAnimatable}
};

DECLARE_NODE_PROPERTIES(XAnisotropic)
IMPLEMENT_PROPERTY_COMMON(XAnisotropicProperties,XSpecularProperties);

bool XAnisotropicProperties::SetProperty(XBase* node,const XU32 property_id,const XColour& val)
{
  DO_TYPE_CHECK(node,XAnisotropic);
  switch(property_id)
  {
  case XAnisotropic::kTangentUCamera: object->SetTangentUCamera(val); return true;
  case XAnisotropic::kTangentVCamera: object->SetTangentVCamera(val); return true;
  default: break;
  }
  return XSpecularProperties::SetProperty(node,property_id,val);
}

bool XAnisotropicProperties::GetProperty(XBase* node,const XU32 property_id,XColour& val)
{
  DO_TYPE_CHECK(node,XAnisotropic);
  switch(property_id)
  {
  case XAnisotropic::kTangentUCamera: val = object->GetTangentUCamera(); return true;
  case XAnisotropic::kTangentVCamera: val = object->GetTangentVCamera(); return true;
  default: break;
  }
  return XSpecularProperties::GetProperty(node,property_id,val);
}

bool XAnisotropicProperties::SetProperty(XBase* node,const XU32 property_id,const XReal& val)
{
  DO_TYPE_CHECK(node,XAnisotropic);
  switch(property_id)
  {
  case XAnisotropic::kCorrelationX: object->SetCorrelationX(val); return true;
  case XAnisotropic::kCorrelationY: object->SetCorrelationY(val); return true;
  case XAnisotropic::kRoughness: object->SetRoughness(val); return true;
  case XAnisotropic::kRotateAngle: object->SetRotateAngle(val); return true;
  case XAnisotropic::kRefractiveIndex: object->SetRefractiveIndex(val); return true;
  default: break;
  }
  return XSpecularProperties::SetProperty(node,property_id,val);
}

bool XAnisotropicProperties::GetProperty(XBase* node,const XU32 property_id,XReal& val)
{
  DO_TYPE_CHECK(node,XAnisotropic);
  switch(property_id)
  {
  case XAnisotropic::kCorrelationX: val = object->GetCorrelationX(); return true;
  case XAnisotropic::kCorrelationY: val = object->GetCorrelationY(); return true;
  case XAnisotropic::kRoughness: val = object->GetRoughness(); return true;
  case XAnisotropic::kRotateAngle: val = object->GetRotateAngle(); return true;
  case XAnisotropic::kRefractiveIndex: val = object->GetRefractiveIndex(); return true;
  default: break;
  }
  return XSpecularProperties::GetProperty(node,property_id,val);
}

//----------------------------------------------------------------------------------------------------------------------
class XBlinnProperties : public XSpecularProperties
{
  static const XU32 kNumProperties=2;
  static XPropertyInfo m_Properties[kNumProperties];
public:
  PROPERTY_COUNT(XBlinnProperties,XSpecularProperties);
  PROPERTY_TYPE(XReal);
};

XPropertyInfo XBlinnProperties::m_Properties[2] = 
{
  {XPropertyType::kReal,"eccentricity",XBlinn::kEccentricity,XPropertyFlags::kAnimatable},
  {XPropertyType::kReal,"specular_roll_off",XBlinn::kSpecularRollOff,XPropertyFlags::kAnimatable}
};

DECLARE_NODE_PROPERTIES(XBlinn)
IMPLEMENT_PROPERTY_COMMON(XBlinnProperties,XSpecularProperties);

bool XBlinnProperties::SetProperty(XBase* node,const XU32 property_id,const XReal& val)
{
  DO_TYPE_CHECK(node,XBlinn);
  switch(property_id)
  {
  case XBlinn::kEccentricity: object->SetEccentricity(val); return true;
  case XBlinn::kSpecularRollOff: object->SetSpecularRollOff(val); return true;
  default: break;
  }
  return XSpecularProperties::SetProperty(node,property_id,val);
}

bool XBlinnProperties::GetProperty(XBase* node,const XU32 property_id,XReal& val)
{
  DO_TYPE_CHECK(node,XBlinn);
  switch(property_id)
  {
  case XBlinn::kEccentricity: val = object->GetEccentricity(); return true;
  case XBlinn::kSpecularRollOff: val = object->GetSpecularRollOff(); return true;
  default: break;
  }
  return XSpecularProperties::GetProperty(node,property_id,val);
}

//----------------------------------------------------------------------------------------------------------------------
class XPhongEProperties : public XSpecularProperties
{
  static const XU32 kNumProperties=2;
  static XPropertyInfo m_Properties[kNumProperties];
public:
  PROPERTY_COUNT(XPhongEProperties,XSpecularProperties);
  PROPERTY_TYPE(XReal);
};
XPropertyInfo XPhongEProperties::m_Properties[2] = 
{
  {XPropertyType::kReal,"highlight_size",XPhongE::kHighlightSize,XPropertyFlags::kAnimatable},
  {XPropertyType::kReal,"roughness",XPhongE::kRoughness,XPropertyFlags::kAnimatable}
};

DECLARE_NODE_PROPERTIES(XPhongE)
IMPLEMENT_PROPERTY_COMMON(XPhongEProperties,XSpecularProperties);

bool XPhongEProperties::SetProperty(XBase* node,const XU32 property_id,const XReal& val)
{
  DO_TYPE_CHECK(node,XPhongE);
  switch(property_id)
  {
  case XPhongE::kHighlightSize: object->SetHighlightSize(val); return true;
  case XPhongE::kRoughness: object->SetRoughness(val); return true;
  default: break;
  }
  return XSpecularProperties::SetProperty(node,property_id,val);
}

bool XPhongEProperties::GetProperty(XBase* node,const XU32 property_id,XReal& val)
{
  DO_TYPE_CHECK(node,XPhongE);
  switch(property_id)
  {
  case XPhongE::kHighlightSize: val = object->GetHighlightSize(); return true;
  case XPhongE::kRoughness: val = object->GetRoughness(); return true;
  default: break;
  }
  return XSpecularProperties::GetProperty(node,property_id,val);
}

//----------------------------------------------------------------------------------------------------------------------
class XLayeredShaderProperties : public XMaterialProperties
{
  static const XU32 kNumProperties=5;
  static XPropertyInfo m_Properties[kNumProperties];
public:
  PROPERTY_COUNT(XLayeredShaderProperties,XMaterialProperties);
  PROPERTY_TYPE(bool);
  PROPERTY_TYPE(XList);
  PROPERTY_TYPE(XU32);
};
XPropertyInfo XLayeredShaderProperties::m_Properties[5] = 
{
  {XPropertyType::kUnsigned,"num_layers",XLayeredShader::kNumLayers,0},
  {XPropertyType::kNodeArray,"glow_layers",XLayeredShader::kGlowLayers,0,XFn::Material},
  {XPropertyType::kNodeArray,"transparency_layers",XLayeredShader::kTransparencyLayers,0,XFn::Material},
  {XPropertyType::kNodeArray,"colour_layers",XLayeredShader::kColourLayers,0,XFn::Material},
  {XPropertyType::kBool,"is_texture",XLayeredShader::kIsTexture,0}
};

DECLARE_NODE_PROPERTIES(XLayeredShader)
IMPLEMENT_PROPERTY_COMMON(XLayeredShaderProperties,XMaterialProperties);

bool XLayeredShaderProperties::SetProperty(XBase* node,const XU32 property_id,const XU32& val)
{
  DO_TYPE_CHECK(node,XLayeredShader);
  switch(property_id)
  {
  case XLayeredShader::kNumLayers: object->SetNumLayers(val); return true;
  default: break;
  }
  return XBaseProperties::SetProperty(node,property_id,val);
}

bool XLayeredShaderProperties::GetProperty(XBase* node,const XU32 property_id,XU32& val)
{
  DO_TYPE_CHECK(node,XLayeredShader);
  switch(property_id)
  {
  case XLayeredShader::kNumLayers: val = object->GetNumLayers(); return true;
  default: break;
  }
  return XBaseProperties::GetProperty(node,property_id,val);
}

bool XLayeredShaderProperties::SetProperty(XBase* node,const XU32 property_id,const XList& val)
{
  DO_TYPE_CHECK(node,XLayeredShader);
  switch(property_id)
  {
  case XLayeredShader::kGlowLayers:
    {
      if(val.size()!=object->GetNumLayers()) 
        return false;
      for (XU32 i=0;i!=object->GetNumLayers();++i)
      {
        object->SetLayer(i,object->GetLayerColour(i),object->GetLayerTransparency(i),val[i]->HasFn<XMaterial>() );
      }
    }
    return true;
  case XLayeredShader::kTransparencyLayers:
    {
      if(val.size()!=object->GetNumLayers()) 
        return false;
      for (XU32 i=0;i!=object->GetNumLayers();++i)
      {
        object->SetLayer(i,object->GetLayerColour(i),val[i]->HasFn<XMaterial>(),object->GetLayerGlow(i) );
      }
    }
    return true;
  case XLayeredShader::kColourLayers: 
    {
      if(val.size()!=object->GetNumLayers()) 
        return false;
      for (XU32 i=0;i!=object->GetNumLayers();++i)
      {
        object->SetLayer(i,val[i]->HasFn<XMaterial>(),object->GetLayerTransparency(i),object->GetLayerGlow(i) );
      }
    }
    return true;
  default: break;
  }
  return XBaseProperties::SetProperty(node,property_id,val);
}

bool XLayeredShaderProperties::GetProperty(XBase* node,const XU32 property_id,XList& val)
{
  DO_TYPE_CHECK(node,XLayeredShader);
  switch(property_id)
  {
  case XLayeredShader::kGlowLayers:  
    {
      val.clear();
      for (XU32 i=0;i!=object->GetNumLayers();++i)
      {
        val.push_back( object->GetLayerGlow(i) );
      }
    }
    return true;
  case XLayeredShader::kTransparencyLayers:
    {
      val.clear();
      for (XU32 i=0;i!=object->GetNumLayers();++i)
      {
        val.push_back( object->GetLayerTransparency(i) );
      }
    }
    return true;
  case XLayeredShader::kColourLayers:
    {
      val.clear();
      for (XU32 i=0;i!=object->GetNumLayers();++i)
      {
        val.push_back( object->GetLayerGlow(i) );
      }
    }
    return true;
  default: break;
  }
  return XBaseProperties::GetProperty(node,property_id,val);
}

bool XLayeredShaderProperties::SetProperty(XBase* node,const XU32 property_id,const bool& val)
{
  DO_TYPE_CHECK(node,XLayeredShader);
  switch(property_id)
  {
  case XLayeredShader::kIsTexture: object->SetType(val ? 1 : 0); return true;
  default: break;
  }
  return XBaseProperties::SetProperty(node,property_id,val);
}

bool XLayeredShaderProperties::GetProperty(XBase* node,const XU32 property_id,bool& val)
{
  DO_TYPE_CHECK(node,XLayeredShader);
  switch(property_id)
  {
  case XLayeredShader::kIsTexture: val = object->IsTexture(); return true;
  default: break;
  }
  return XBaseProperties::GetProperty(node,property_id,val);
}

//----------------------------------------------------------------------------------------------------------------------
class XObjectSetProperties : public XNodeCollectionProperties
{
  static const XU32 kNumProperties=1;
  static XPropertyInfo m_Properties[kNumProperties];
public:
  PROPERTY_COUNT(XObjectSetProperties,XNodeCollectionProperties);
  PROPERTY_TYPE(XString);
};

XPropertyInfo XObjectSetProperties::m_Properties[1] = 
{
  {XPropertyType::kString,"annotation",XObjectSet::kAnnotation,0}
};

DECLARE_NODE_PROPERTIES(XObjectSet)
IMPLEMENT_PROPERTY_COMMON(XObjectSetProperties,XNodeCollectionProperties);

bool XObjectSetProperties::SetProperty(XBase* node,const XU32 property_id,const XString& val)
{
  DO_TYPE_CHECK(node,XObjectSet);
  switch(property_id)
  {
  case XObjectSet::kAnnotation: object->SetAnnotation(val.c_str()); return true;
  default: break;
  }
  return XBaseProperties::SetProperty(node,property_id,val);
}

bool XObjectSetProperties::GetProperty(XBase* node,const XU32 property_id,XString& val)
{
  DO_TYPE_CHECK(node,XObjectSet);
  switch(property_id)
  {
  case XObjectSet::kAnnotation: val = object->GetAnnotation(); return true;
  default: break;
  }
  return XBaseProperties::GetProperty(node,property_id,val);
}

//----------------------------------------------------------------------------------------------------------------------
class XShapeProperties : public XBaseProperties
{
  static XPropertyInfo* m_Properties;
  static const XU32 kNumProperties=0;
public:
  static const XU32 kLast=0;
  PROPERTY_COUNT(XShapeProperties,XBaseProperties);
};
XPropertyInfo* XShapeProperties::m_Properties=0;

DECLARE_NODE_PROPERTIES(XShape)
IMPLEMENT_PROPERTY_COMMON(XShapeProperties,XBaseProperties);

//----------------------------------------------------------------------------------------------------------------------
class XBaseLatticeProperties : public XShapeProperties
{
  static const XU32 kNumProperties=3;
  static XPropertyInfo m_Properties[kNumProperties];
public:
  PROPERTY_COUNT(XBaseLatticeProperties,XShapeProperties);
  PROPERTY_TYPE(XReal);
};

XPropertyInfo XBaseLatticeProperties::m_Properties[3] = 
{
  {XPropertyType::kReal,"x_size",XBaseLattice::kXSize,0},
  {XPropertyType::kReal,"y_size",XBaseLattice::kYSize,0},
  {XPropertyType::kReal,"z_size",XBaseLattice::kZSize,0}
};

DECLARE_NODE_PROPERTIES(XBaseLattice)
IMPLEMENT_PROPERTY_COMMON(XBaseLatticeProperties,XShapeProperties);

bool XBaseLatticeProperties::SetProperty(XBase* node,const XU32 property_id,const XReal& val)
{
  DO_TYPE_CHECK(node,XBaseLattice);
  switch(property_id)
  {
  case XBaseLattice::kXSize: object->SetXSize(val); return true;
  case XBaseLattice::kYSize: object->SetYSize(val); return true;
  case XBaseLattice::kZSize: object->SetZSize(val); return true;
  default: break;
  }
  return XBaseProperties::SetProperty(node,property_id,val);
}

bool XBaseLatticeProperties::GetProperty(XBase* node,const XU32 property_id,XReal& val)
{
  DO_TYPE_CHECK(node,XBaseLattice);
  switch(property_id)
  {
  case XBaseLattice::kXSize: val = object->GetXSize(); return true;
  case XBaseLattice::kYSize: val = object->GetYSize(); return true;
  case XBaseLattice::kZSize: val = object->GetZSize(); return true;
  default: break;
  }
  return XBaseProperties::GetProperty(node,property_id,val);
}

//----------------------------------------------------------------------------------------------------------------------
class XCameraProperties : public XShapeProperties
{
  static const XU32 kNumProperties=5;
  static XPropertyInfo m_Properties[kNumProperties];
public:
  PROPERTY_COUNT(XCameraProperties,XShapeProperties);
  PROPERTY_TYPE(XReal);
};

XPropertyInfo XCameraProperties::m_Properties[5] = 
{
  {XPropertyType::kReal,"aspect",XCamera::kAspect,XPropertyFlags::kAnimatable},
  {XPropertyType::kReal,"near",XCamera::kNear,XPropertyFlags::kAnimatable},
  {XPropertyType::kReal,"far",XCamera::kFar,XPropertyFlags::kAnimatable},
  {XPropertyType::kReal,"horiz_aspect",XCamera::kHorizontalAspect,XPropertyFlags::kAnimatable},
  {XPropertyType::kReal,"vert_aspect",XCamera::kVerticalAspect,XPropertyFlags::kAnimatable}
};

DECLARE_NODE_PROPERTIES(XCamera)
IMPLEMENT_PROPERTY_COMMON(XCameraProperties,XShapeProperties);

bool XCameraProperties::SetProperty(XBase* node,const XU32 property_id,const XReal& val)
{
  DO_TYPE_CHECK(node,XCamera);
  switch(property_id)
  {
  case XCamera::kAspect: object->SetAspect(val); return true;
  case XCamera::kNear: object->SetNear(val); return true;
  case XCamera::kFar: object->SetFar(val); return true;
  case XCamera::kHorizontalAspect: object->SetHorizontalAspect(val); return true;
  case XCamera::kVerticalAspect: object->SetVerticalAspect(val); return true;
  default: break;
  }
  return XBaseProperties::SetProperty(node,property_id,val);
}

bool XCameraProperties::GetProperty(XBase* node,const XU32 property_id,XReal& val)
{
  DO_TYPE_CHECK(node,XCamera);
  switch(property_id)
  {
  case XCamera::kAspect: val = object->GetAspect(); return true;
  case XCamera::kNear: val = object->GetNear(); return true;
  case XCamera::kFar: val = object->GetFar(); return true;
  case XCamera::kHorizontalAspect: val = object->GetHorizontalAspect(); return true;
  case XCamera::kVerticalAspect: val = object->GetVerticalAspect(); return true;
  default: break;
  }
  return XBaseProperties::GetProperty(node,property_id,val);
}

//----------------------------------------------------------------------------------------------------------------------
class XCollisionObjectProperties : public XShapeProperties
{
  static const XU32 kNumProperties=2;
  static XPropertyInfo m_Properties[kNumProperties];
public:
  PROPERTY_COUNT(XCollisionObjectProperties,XShapeProperties);
  PROPERTY_TYPE(XReal);
};

XPropertyInfo XCollisionObjectProperties::m_Properties[2] = 
{
  {XPropertyType::kReal,"gravity_scale",XCollisionObject::kGravityScale,0},
  {XPropertyType::kReal,"mass",XCollisionObject::kMass,0}     
};

DECLARE_NODE_PROPERTIES(XCollisionObject)
IMPLEMENT_PROPERTY_COMMON(XCollisionObjectProperties,XShapeProperties);

bool XCollisionObjectProperties::SetProperty(XBase* node,const XU32 property_id,const XReal& val)
{
  DO_TYPE_CHECK(node,XCollisionObject);
  switch(property_id)
  {
  case XCollisionObject::kGravityScale: object->SetGravityScale(val); return true;
  case XCollisionObject::kMass:         object->SetMass(val);         return true;
  default: break;
  }
  return XShapeProperties::SetProperty(node,property_id,val);
}

bool XCollisionObjectProperties::GetProperty(XBase* node,const XU32 property_id,XReal& val)
{
  DO_TYPE_CHECK(node,XCollisionObject);
  switch(property_id)
  {
  case XCollisionObject::kGravityScale: val = object->GetGravityScale(); return true;
  case XCollisionObject::kMass:         val = object->GetMass();         return true;
  default: break;
  }
  return XShapeProperties::GetProperty(node,property_id,val);
}

//----------------------------------------------------------------------------------------------------------------------
class XCollisionBoxProperties : public XCollisionObjectProperties
{
  static const XU32 kNumProperties=3;
  static XPropertyInfo m_Properties[kNumProperties];
public:
  PROPERTY_COUNT(XCollisionBoxProperties,XCollisionObjectProperties);
  PROPERTY_TYPE(XReal);
};

XPropertyInfo XCollisionBoxProperties::m_Properties[3] = 
{
  {XPropertyType::kReal,"x_size",XCollisionBox::kSizeX,0},
  {XPropertyType::kReal,"y_size",XCollisionBox::kSizeY,0},
  {XPropertyType::kReal,"z_size",XCollisionBox::kSizeZ,0}
};

DECLARE_NODE_PROPERTIES(XCollisionBox)
IMPLEMENT_PROPERTY_COMMON(XCollisionBoxProperties,XCollisionObjectProperties);

bool XCollisionBoxProperties::SetProperty(XBase* node,const XU32 property_id,const XReal& val)
{
  DO_TYPE_CHECK(node,XCollisionBox);
  switch(property_id)
  {
  case XCollisionBox::kSizeX: object->SetX(val); return true;
  case XCollisionBox::kSizeY: object->SetY(val); return true;
  case XCollisionBox::kSizeZ: object->SetZ(val); return true;
  default: break;
  }
  return XCollisionObjectProperties::SetProperty(node,property_id,val);
}

bool XCollisionBoxProperties::GetProperty(XBase* node,const XU32 property_id,XReal& val)
{
  DO_TYPE_CHECK(node,XCollisionBox);
  switch(property_id)
  {
  case XCollisionBox::kSizeX: val = object->GetX(); return true;
  case XCollisionBox::kSizeY: val = object->GetY(); return true;
  case XCollisionBox::kSizeZ: val = object->GetZ(); return true;
  default: break;
  }
  return XCollisionObjectProperties::GetProperty(node,property_id,val);
}

//----------------------------------------------------------------------------------------------------------------------
class XCollisionCapsuleProperties : public XCollisionObjectProperties
{
  static const XU32 kNumProperties=2;
  static XPropertyInfo m_Properties[kNumProperties];
public:
  PROPERTY_COUNT(XCollisionCapsuleProperties,XCollisionObjectProperties);
  PROPERTY_TYPE(XReal);
};

XPropertyInfo XCollisionCapsuleProperties::m_Properties[2] = 
{
  {XPropertyType::kReal,"length",XCollisionCapsule::kLength,0},
  {XPropertyType::kReal,"radius",XCollisionCapsule::kRadius,0}
};

DECLARE_NODE_PROPERTIES(XCollisionCapsule)
IMPLEMENT_PROPERTY_COMMON(XCollisionCapsuleProperties,XCollisionObjectProperties);

bool XCollisionCapsuleProperties::SetProperty(XBase* node,const XU32 property_id,const XReal& val)
{
  DO_TYPE_CHECK(node,XCollisionCapsule);
  switch(property_id)
  {
  case XCollisionCapsule::kLength: object->SetLength(val); return true;
  case XCollisionCapsule::kRadius: object->SetRadius(val); return true;
  default: break;
  }
  return XCollisionObjectProperties::SetProperty(node,property_id,val);
}

bool XCollisionCapsuleProperties::GetProperty(XBase* node,const XU32 property_id,XReal& val)
{
  DO_TYPE_CHECK(node,XCollisionCapsule);
  switch(property_id)
  {
  case XCollisionCapsule::kLength: val = object->GetLength(); return true;
  case XCollisionCapsule::kRadius: val = object->GetRadius(); return true;
  default: break;
  }
  return XCollisionObjectProperties::GetProperty(node,property_id,val);
}

//----------------------------------------------------------------------------------------------------------------------
class XCollisionConeProperties : public XCollisionObjectProperties
{
  static const XU32 kNumProperties=2;
  static XPropertyInfo m_Properties[kNumProperties];
public:
  PROPERTY_COUNT(XCollisionConeProperties,XCollisionObjectProperties);
  PROPERTY_TYPE(XReal);
};

XPropertyInfo XCollisionConeProperties::m_Properties[2] = 
{
  {XPropertyType::kReal,"angle",XCollisionCone::kAngle,0},
  {XPropertyType::kReal,"cap",XCollisionCone::kCap,0}
};

DECLARE_NODE_PROPERTIES(XCollisionCone)
IMPLEMENT_PROPERTY_COMMON(XCollisionConeProperties,XCollisionObjectProperties);

bool XCollisionConeProperties::SetProperty(XBase* node,const XU32 property_id,const XReal& val)
{
  DO_TYPE_CHECK(node,XCollisionCone);
  switch(property_id)
  {
  case XCollisionCone::kAngle: object->SetAngle(val); return true;
  case XCollisionCone::kCap:   object->SetCap(val); return true;
  default: break;
  }
  return XCollisionObjectProperties::SetProperty(node,property_id,val);
}

bool XCollisionConeProperties::GetProperty(XBase* node,const XU32 property_id,XReal& val)
{
  DO_TYPE_CHECK(node,XCollisionCone);
  switch(property_id)
  {
  case XCollisionCone::kAngle: val = object->GetAngle(); return true;
  case XCollisionCone::kCap:   val = object->GetCap(); return true;
  default: break;
  }
  return XCollisionObjectProperties::GetProperty(node,property_id,val);
}

//----------------------------------------------------------------------------------------------------------------------
class XCollisionCylinderProperties : public XCollisionObjectProperties
{
  static const XU32 kNumProperties=2;
  static XPropertyInfo m_Properties[kNumProperties];
public:
  PROPERTY_COUNT(XCollisionCylinderProperties,XCollisionObjectProperties);
  PROPERTY_TYPE(XReal);
};

XPropertyInfo XCollisionCylinderProperties::m_Properties[2] = 
{
  {XPropertyType::kReal,"length",XCollisionCylinder::kLength,0},
  {XPropertyType::kReal,"radius",XCollisionCylinder::kRadius,0}
};

DECLARE_NODE_PROPERTIES(XCollisionCylinder)
IMPLEMENT_PROPERTY_COMMON(XCollisionCylinderProperties,XCollisionObjectProperties);

bool XCollisionCylinderProperties::SetProperty(XBase* node,const XU32 property_id,const XReal& val)
{
  DO_TYPE_CHECK(node,XCollisionCylinder);
  switch(property_id)
  {
  case XCollisionCylinder::kLength: object->SetLength(val); return true;
  case XCollisionCylinder::kRadius: object->SetRadius(val); return true;
  default: break;
  }
  return XCollisionObjectProperties::SetProperty(node,property_id,val);
}

bool XCollisionCylinderProperties::GetProperty(XBase* node,const XU32 property_id,XReal& val)
{
  DO_TYPE_CHECK(node,XCollisionCylinder);
  switch(property_id)
  {
  case XCollisionCylinder::kLength: val = object->GetLength(); return true;
  case XCollisionCylinder::kRadius: val = object->GetRadius(); return true;
  default: break;
  }
  return XCollisionObjectProperties::GetProperty(node,property_id,val);
}

//----------------------------------------------------------------------------------------------------------------------
class XCollisionMeshProperties : public XCollisionObjectProperties
{
  static const XU32 kNumProperties=2;
  static XPropertyInfo m_Properties[kNumProperties];
public:
  PROPERTY_COUNT(XCollisionMeshProperties,XCollisionObjectProperties);
  PROPERTY_TYPE(XVector3Array);
  PROPERTY_TYPE(XU32Array);
};

XPropertyInfo XCollisionMeshProperties::m_Properties[2] = 
{
  {XPropertyType::kVector3Array,"points",XCollisionMesh::kPoints,XPropertyFlags::kArrayIsResizable},
  {XPropertyType::kUnsignedArray,"indices",XCollisionMesh::kIndices,XPropertyFlags::kArrayIsResizable}
};

DECLARE_NODE_PROPERTIES(XCollisionMesh)
IMPLEMENT_PROPERTY_COMMON(XCollisionMeshProperties,XCollisionObjectProperties);

bool XCollisionMeshProperties::SetProperty(XBase* node,const XU32 property_id,const XVector3Array& val)
{
  DO_TYPE_CHECK(node,XCollisionMesh);
  switch(property_id)
  {
  case XCollisionMesh::kPoints: object->SetVertices(val); return true;
  default: break;
  }
  return XBaseProperties::SetProperty(node,property_id,val);
}

bool XCollisionMeshProperties::GetProperty(XBase* node,const XU32 property_id,XVector3Array& val)
{
  DO_TYPE_CHECK(node,XCollisionMesh);
  switch(property_id)
  {
  case XCollisionMesh::kPoints: val = object->Vertices(); return true;
  default: break;
  }
  return XBaseProperties::GetProperty(node,property_id,val);
}


bool XCollisionMeshProperties::SetProperty(XBase* node,const XU32 property_id,const XU32Array& val)
{
  DO_TYPE_CHECK(node,XCollisionMesh);
  switch(property_id)
  {
  case XCollisionMesh::kIndices: object->SetIndices(val); return true;
  default: break;
  }
  return XBaseProperties::SetProperty(node,property_id,val);
}

bool XCollisionMeshProperties::GetProperty(XBase* node,const XU32 property_id,XU32Array& val)
{
  DO_TYPE_CHECK(node,XCollisionMesh);
  switch(property_id)
  {
  case XCollisionMesh::kIndices: val = object->Indices(); return true;
  default: break;
  }
  return XBaseProperties::GetProperty(node,property_id,val);
}

//----------------------------------------------------------------------------------------------------------------------
class XCollisionSphereProperties : public XCollisionObjectProperties
{
  static const XU32 kNumProperties=1;
  static XPropertyInfo m_Properties[kNumProperties];
public:
  PROPERTY_COUNT(XCollisionSphereProperties,XCollisionObjectProperties);
  PROPERTY_TYPE(XReal);
};

XPropertyInfo XCollisionSphereProperties::m_Properties[1] = 
{
  {XPropertyType::kReal,"radius",XCollisionSphere::kRadius,0}
};

DECLARE_NODE_PROPERTIES(XCollisionSphere)
IMPLEMENT_PROPERTY_COMMON(XCollisionSphereProperties,XCollisionObjectProperties);

bool XCollisionSphereProperties::SetProperty(XBase* node,const XU32 property_id,const XReal& val)
{
  DO_TYPE_CHECK(node,XCollisionSphere);
  switch(property_id)
  {
  case XCollisionSphere::kRadius: object->SetRadius(val); return true;
  default: break;
  }
  return XCollisionObjectProperties::SetProperty(node,property_id,val);
}

bool XCollisionSphereProperties::GetProperty(XBase* node,const XU32 property_id,XReal& val)
{
  DO_TYPE_CHECK(node,XCollisionSphere);
  switch(property_id)
  {
  case XCollisionSphere::kRadius: val = object->GetRadius(); return true;
  default: break;
  }
  return XCollisionObjectProperties::GetProperty(node,property_id,val);
}

//----------------------------------------------------------------------------------------------------------------------
class XGeometryProperties : public XShapeProperties
{
  static const XU32 kNumProperties=3;
  static XPropertyInfo m_Properties[kNumProperties];
public:
  PROPERTY_COUNT(XGeometryProperties,XShapeProperties);
  PROPERTY_TYPE(XVector3Array);
  PROPERTY_TYPE(XList);
  PROPERTY_TYPE(bool);
};

XPropertyInfo XGeometryProperties::m_Properties[3] = 
{
  {XPropertyType::kVector3Array,"points",XGeometry::kPoints,XPropertyFlags::kAnimatable|XPropertyFlags::kArrayIsResizable},
  {XPropertyType::kNodeArray,"deformer_queue",XGeometry::kDeformerQueue,XPropertyFlags::kArrayIsResizable,XFn::Deformer},
  {XPropertyType::kBool,"intermediate_object",XGeometry::kIsIntermediateObject,0}
};

DECLARE_NODE_PROPERTIES(XGeometry)
IMPLEMENT_PROPERTY_COMMON(XGeometryProperties,XShapeProperties);

bool XGeometryProperties::SetProperty(XBase* node,const XU32 property_id,const XVector3Array& val)
{
  DO_TYPE_CHECK(node,XGeometry);
  switch(property_id)
  {
  case XGeometry::kPoints: object->SetPoints(val); return true;
  default: break;
  }
  return XBaseProperties::SetProperty(node,property_id,val);
}

bool XGeometryProperties::GetProperty(XBase* node,const XU32 property_id,XVector3Array& val)
{
  DO_TYPE_CHECK(node,XGeometry);
  switch(property_id)
  {
  case XGeometry::kPoints: val = object->Points(); return true;
  default: break;
  }
  return XBaseProperties::GetProperty(node,property_id,val);
}

bool XGeometryProperties::SetProperty(XBase* node,const XU32 property_id,const bool& val)
{
  DO_TYPE_CHECK(node,XGeometry);
  switch(property_id)
  {
  case XGeometry::kIsIntermediateObject: object->SetIsIntermediateObject(val); return true;
  default: break;
  }
  return XBaseProperties::SetProperty(node,property_id,val);
}

bool XGeometryProperties::GetProperty(XBase* node,const XU32 property_id,bool& val)
{
  DO_TYPE_CHECK(node,XGeometry);
  switch(property_id)
  {
  case XGeometry::kIsIntermediateObject: val = object->GetIsIntermediateObject(); return true;
  default: break;
  }
  return XBaseProperties::GetProperty(node,property_id,val);
}

bool XGeometryProperties::SetProperty(XBase* node,const XU32 property_id,const XList& val)
{
  DO_TYPE_CHECK(node,XGeometry);
  switch(property_id)
  {
  case XGeometry::kDeformerQueue: 
    {
      XDeformerList defs;
      XList::const_iterator it = val.begin();
      for (;it != val.end(); ++it)
      {
        if( *it )
        {
          XDeformer * def = ((*it))->HasFn<XDeformer>();
          if(def)
            defs.push_back(def);
        }
      }
      object->SetDeformerQueue(defs); 
      return true;
    }
  default: break;
  }
  return XBaseProperties::SetProperty(node,property_id,val);
}

bool XGeometryProperties::GetProperty(XBase* node,const XU32 property_id,XList& val)
{
  DO_TYPE_CHECK(node,XGeometry);
  switch(property_id)
  {
  case XGeometry::kDeformerQueue: 
    {
      val.clear();
      XDeformerList defs;
      object->GetDeformerQueue(defs); 
      XDeformerList::iterator it = defs.begin();
      for (;it != defs.end(); ++it)
      {
        val.push_back(*it);
      }
      return true;
    }
  default: break;
  }
  return XBaseProperties::GetProperty(node,property_id,val);
}

//----------------------------------------------------------------------------------------------------------------------
class XLatticeProperties : public XGeometryProperties
{
  static const XU32 kNumProperties=5;
  static XPropertyInfo m_Properties[kNumProperties];
public:
  PROPERTY_COUNT(XLatticeProperties,XGeometryProperties);
  PROPERTY_TYPE(XU32);
  PROPERTY_TYPE(XBase*);
};

XPropertyInfo XLatticeProperties::m_Properties[5] = 
{
  {XPropertyType::kUnsigned,"divisions_s",XLattice::kDivisionsS,0},
  {XPropertyType::kUnsigned,"divisions_t",XLattice::kDivisionsT,0},
  {XPropertyType::kUnsigned,"divisions_u",XLattice::kDivisionsU,0},
  {XPropertyType::kNode,"ffd",XLattice::kFFD,XPropertyFlags::kReadOnly,XFn::FFd},
  {XPropertyType::kNode,"base_lattice",XLattice::kBaseLattice,XPropertyFlags::kReadOnly,XFn::BaseLattice}
};

DECLARE_NODE_PROPERTIES(XLattice)
IMPLEMENT_PROPERTY_COMMON(XLatticeProperties,XGeometryProperties);

bool XLatticeProperties::GetProperty(XBase* node,const XU32 property_id,XBase*& val)
{
  DO_TYPE_CHECK(node,XLattice);
  switch(property_id)
  {
  case XLattice::kFFD: val = object->GetFFD(); return true;
  case XLattice::kBaseLattice: val = object->GetBaseLattice(); return true;
  default: break;
  }
  return XShapeProperties::GetProperty(node,property_id,val);
}

bool XLatticeProperties::SetProperty(XBase* node,const XU32 property_id,const XBase*& val)
{
  return XShapeProperties::SetProperty(node,property_id,val);
}

bool XLatticeProperties::SetProperty(XBase* node,const XU32 property_id,const XU32& val)
{
  DO_TYPE_CHECK(node,XLattice);
  XU32 s,t,u;
  object->GetNumDivisions(s,t,u);
  switch(property_id)
  {
  case XLattice::kDivisionsS: s = val; object->SetNumDivisions(s,t,u); return true;
  case XLattice::kDivisionsT: t = val; object->SetNumDivisions(s,t,u); return true;
  case XLattice::kDivisionsU: u = val; object->SetNumDivisions(s,t,u); return true;
  default: break;
  }
  return XBaseProperties::SetProperty(node,property_id,val);
}

bool XLatticeProperties::GetProperty(XBase* node,const XU32 property_id,XU32& val)
{
  DO_TYPE_CHECK(node,XLattice);
  XU32 s,t,u;
  object->GetNumDivisions(s,t,u);
  switch(property_id)
  {
  case XLattice::kDivisionsS: val = s; return true;
  case XLattice::kDivisionsT: val = t; return true;
  case XLattice::kDivisionsU: val = u; return true;
  default: break;
  }
  return XBaseProperties::GetProperty(node,property_id,val);
}

//---------------------------------------------------------------------------------------------------------------------- 
class XParametricSurfaceProperties : public XGeometryProperties
{
  static const XU32 kNumProperties=4;
  static XPropertyInfo m_Properties[kNumProperties];
public:
  PROPERTY_COUNT(XParametricSurfaceProperties,XGeometryProperties);
  PROPERTY_TYPE(XU32);
};

XPropertyInfo XParametricSurfaceProperties::m_Properties[4] = 
{
  {XPropertyType::kUnsigned,"type_u",XParametricSurface::kTypeInU,0},
  {XPropertyType::kUnsigned,"type_v",XParametricSurface::kTypeInV,0},
  {XPropertyType::kRealArray,"num_cvs_u",XParametricSurface::kNumCvsInU,0},
  {XPropertyType::kRealArray,"num_cvs_v",XParametricSurface::kNumCvsInV,0}
};

DECLARE_NODE_PROPERTIES(XParametricSurface)
IMPLEMENT_PROPERTY_COMMON(XParametricSurfaceProperties,XGeometryProperties);

bool XParametricSurfaceProperties::SetProperty(XBase* node,const XU32 property_id,const XU32& val)
{
  DO_TYPE_CHECK(node,XParametricSurface);
  switch(property_id)
  {
  case XParametricSurface::kTypeInU:   object->SetTypeInU(val);   return true;
  case XParametricSurface::kTypeInV:   object->SetTypeInV(val);   return true;
  case XParametricSurface::kNumCvsInU: object->SetNumCvsInU(val); return true;
  case XParametricSurface::kNumCvsInV: object->SetNumCvsInV(val); return true;
  default: break;
  }
  return XBaseProperties::SetProperty(node,property_id,val);
}

bool XParametricSurfaceProperties::GetProperty(XBase* node,const XU32 property_id,XU32& val)
{
  DO_TYPE_CHECK(node,XParametricSurface);
  switch(property_id)
  {
  case XParametricSurface::kTypeInU:   val = object->GetTypeInU(); return true;
  case XParametricSurface::kTypeInV:   val = object->GetTypeInV(); return true;
  case XParametricSurface::kNumCvsInU: val = object->GetNumCvsInU();  return true;
  case XParametricSurface::kNumCvsInV: val = object->GetNumCvsInV();  return true;
  default: break;
  }
  return XBaseProperties::GetProperty(node,property_id,val);
}

//----------------------------------------------------------------------------------------------------------------------
class XNurbsSurfaceProperties : public XParametricSurfaceProperties
{
  static const XU32 kNumProperties=4;
  static XPropertyInfo m_Properties[kNumProperties];
public:
  PROPERTY_COUNT(XNurbsSurfaceProperties,XParametricSurfaceProperties);
  PROPERTY_TYPE(XU32);
  PROPERTY_TYPE(XRealArray);
};

XPropertyInfo XNurbsSurfaceProperties::m_Properties[4] = 
{
  {XPropertyType::kUnsigned,"degree_u",XNurbsSurface::kDegreeU,0},
  {XPropertyType::kUnsigned,"degree_u",XNurbsSurface::kDegreeV,0},
  {XPropertyType::kRealArray,"knots_u",XNurbsSurface::kKnotsU,XPropertyFlags::kArrayIsResizable},
  {XPropertyType::kRealArray,"knots_v",XNurbsSurface::kKnotsV,XPropertyFlags::kArrayIsResizable}
};

DECLARE_NODE_PROPERTIES(XNurbsSurface)
IMPLEMENT_PROPERTY_COMMON(XNurbsSurfaceProperties,XParametricSurfaceProperties);


bool XNurbsSurfaceProperties::SetProperty(XBase* node,const XU32 property_id,const XU32& val)
{
  DO_TYPE_CHECK(node,XNurbsSurface);
  switch(property_id)
  {
  case XNurbsSurface::kDegreeU: object->SetDegreeU((XU8)val); return true;
  case XNurbsSurface::kDegreeV: object->SetDegreeV((XU8)val); return true;
  default: break;
  }
  return XParametricSurfaceProperties::SetProperty(node,property_id,val);
}

bool XNurbsSurfaceProperties::GetProperty(XBase* node,const XU32 property_id,XU32& val)
{
  DO_TYPE_CHECK(node,XNurbsSurface);
  switch(property_id)
  {
  case XNurbsSurface::kDegreeU: val = object->GetDegreeU(); return true;
  case XNurbsSurface::kDegreeV: val = object->GetDegreeV(); return true;
  default: break;
  }
  return XParametricSurfaceProperties::GetProperty(node,property_id,val);
}

bool XNurbsSurfaceProperties::SetProperty(XBase* node,const XU32 property_id,const XRealArray& val)
{
  DO_TYPE_CHECK(node,XNurbsSurface);
  switch(property_id)
  {
  case XNurbsSurface::kKnotsU: object->SetKnotsU(val); return true;
  case XNurbsSurface::kKnotsV: object->SetKnotsV(val); return true;
  default: break;
  }
  return XBaseProperties::SetProperty(node,property_id,val);
}

bool XNurbsSurfaceProperties::GetProperty(XBase* node,const XU32 property_id,XRealArray& val)
{
  DO_TYPE_CHECK(node,XNurbsSurface);
  switch(property_id)
  {
  case XNurbsSurface::kKnotsU: object->GetKnotsU(val); return true;
  case XNurbsSurface::kKnotsV: object->GetKnotsV(val); return true;
  default: break;
  }
  return XBaseProperties::GetProperty(node,property_id,val);
}

//----------------------------------------------------------------------------------------------------------------------
class XParametricCurveProperties : public XGeometryProperties
{
  static const XU32 kNumProperties=1;
  static XPropertyInfo m_Properties[kNumProperties];
public:
  PROPERTY_COUNT(XParametricCurveProperties,XGeometryProperties);
  PROPERTY_TYPE(XU32);
};

XPropertyInfo XParametricCurveProperties::m_Properties[1] = 
{
  {XPropertyType::kUnsigned,"curve_type",XNurbsCurve::kCurveType,0}
};

DECLARE_NODE_PROPERTIES(XParametricCurve)
IMPLEMENT_PROPERTY_COMMON(XParametricCurveProperties,XGeometryProperties);

bool XParametricCurveProperties::SetProperty(XBase* node,const XU32 property_id,const XU32& val)
{
  DO_TYPE_CHECK(node,XParametricCurve);
  switch(property_id)
  {
  case XParametricCurve::kCurveType: object->SetCurveType(val); return true;
  default: break;
  }
  return XBaseProperties::SetProperty(node,property_id,val);
}

bool XParametricCurveProperties::GetProperty(XBase* node,const XU32 property_id,XU32& val)
{
  DO_TYPE_CHECK(node,XParametricCurve);
  switch(property_id)
  {
  case XParametricCurve::kCurveType: val = object->GetCurveType(); return true;
  default: break;
  }
  return XBaseProperties::GetProperty(node,property_id,val);
}

//----------------------------------------------------------------------------------------------------------------------
class XNurbsCurveProperties : public XParametricCurveProperties
{
  static const XU32 kNumProperties=2;
  static XPropertyInfo m_Properties[kNumProperties];
public:
  PROPERTY_COUNT(XNurbsCurveProperties,XParametricCurveProperties);
  PROPERTY_TYPE(XU32);
  PROPERTY_TYPE(XRealArray);
};

XPropertyInfo XNurbsCurveProperties::m_Properties[2] = 
{
  {XPropertyType::kUnsigned,"degree",XNurbsCurve::kDegree,0},
  {XPropertyType::kRealArray,"knots",XNurbsCurve::kKnots,0}
};

DECLARE_NODE_PROPERTIES(XNurbsCurve)
IMPLEMENT_PROPERTY_COMMON(XNurbsCurveProperties,XParametricCurveProperties);


bool XNurbsCurveProperties::SetProperty(XBase* node,const XU32 property_id,const XU32& val)
{
  DO_TYPE_CHECK(node,XNurbsCurve);
  switch(property_id)
  {
  case XNurbsCurve::kDegree: object->SetDegree((XU16)val); return true;
  default: break;
  }
  return XParametricCurveProperties::SetProperty(node,property_id,val);
}

bool XNurbsCurveProperties::GetProperty(XBase* node,const XU32 property_id,XU32& val)
{
  DO_TYPE_CHECK(node,XNurbsCurve);
  switch(property_id)
  {
  case XNurbsCurve::kDegree: val = object->GetDegree(); return true;
  default: break;
  }
  return XParametricCurveProperties::GetProperty(node,property_id,val);
}

bool XNurbsCurveProperties::SetProperty(XBase* node,const XU32 property_id,const XRealArray& val)
{
  DO_TYPE_CHECK(node,XNurbsCurve);
  switch(property_id)
  {
  case XNurbsCurve::kKnots: object->SetKnots(val); return true;
  default: break;
  }
  return XBaseProperties::SetProperty(node,property_id,val);
}

bool XNurbsCurveProperties::GetProperty(XBase* node,const XU32 property_id,XRealArray& val)
{
  DO_TYPE_CHECK(node,XNurbsCurve);
  switch(property_id)
  {
  case XNurbsCurve::kKnots: object->GetKnots(val); return true;
  default: break;
  }
  return XBaseProperties::GetProperty(node,property_id,val);
}

//----------------------------------------------------------------------------------------------------------------------
class XParticleShapeProperties : public XGeometryProperties
{
  static const XU32 kNumProperties=7;
  static XPropertyInfo m_Properties[kNumProperties];
public:
  PROPERTY_COUNT(XParticleShapeProperties,XGeometryProperties);
  PROPERTY_TYPE(XList);
  PROPERTY_TYPE(XU32);
  PROPERTY_TYPE(XReal);
  PROPERTY_TYPE(bool);
};

XPropertyInfo XParticleShapeProperties::m_Properties[7] = 
{
  {XPropertyType::kNodeArray,"emitters",XParticleShape::kEmitters,XPropertyFlags::kArrayIsResizable,XFn::Emitter},
  {XPropertyType::kNodeArray,"fields",XParticleShape::kFields,XPropertyFlags::kArrayIsResizable,XFn::Field},
  {XPropertyType::kBool,"depth_sort",XParticleShape::kDepthSort,0},
  {XPropertyType::kUnsigned,"lifespan_type",XParticleShape::kLifespanType,0},
  {XPropertyType::kReal,"lifespan",XParticleShape::kLifespan,0},
  {XPropertyType::kReal,"lifespan_random",XParticleShape::kLifespanRandom,0},
  {XPropertyType::kUnsigned,"particle_type",XParticleShape::kParticleType,0}
};

DECLARE_NODE_PROPERTIES(XParticleShape)
IMPLEMENT_PROPERTY_COMMON(XParticleShapeProperties,XGeometryProperties);


bool XParticleShapeProperties::SetProperty(XBase* node,const XU32 property_id,const bool& val)
{
  DO_TYPE_CHECK(node,XParticleShape);
  switch(property_id)
  {
  case XParticleShape::kDepthSort: object->SetDepthSort(val); return true;
  default: break;
  }
  return XShapeProperties::SetProperty(node,property_id,val);
}

bool XParticleShapeProperties::GetProperty(XBase* node,const XU32 property_id,bool& val)
{
  DO_TYPE_CHECK(node,XParticleShape);
  switch(property_id)
  {
  case XParticleShape::kDepthSort: val = object->GetDepthSort(); return true;
  default: break;
  }
  return XShapeProperties::GetProperty(node,property_id,val);
}

bool XParticleShapeProperties::SetProperty(XBase* node,const XU32 property_id,const XReal& val)
{
  DO_TYPE_CHECK(node,XParticleShape);
  switch(property_id)
  {
  case XParticleShape::kLifespan: object->SetLifespan(val); return true;
  case XParticleShape::kLifespanRandom: object->SetLifespanRandom(val); return true;
  default: break;
  }
  return XShapeProperties::SetProperty(node,property_id,val);
}

bool XParticleShapeProperties::GetProperty(XBase* node,const XU32 property_id,XReal& val)
{
  DO_TYPE_CHECK(node,XParticleShape);
  switch(property_id)
  {
  case XParticleShape::kLifespan: val = object->GetLifespan(); return true;
  case XParticleShape::kLifespanRandom: val = object->GetLifespanRandom(); return true;
  default: break;
  }
  return XShapeProperties::GetProperty(node,property_id,val);
}

bool XParticleShapeProperties::SetProperty(XBase* node,const XU32 property_id,const XList& val)
{
  DO_TYPE_CHECK(node,XParticleShape);
  switch(property_id)
  {
  case XParticleShape::kEmitters: object->SetEmitters(val); return true;
  case XParticleShape::kFields:   object->SetFields(val); return true;
  default: break;
  }
  return XGeometryProperties::SetProperty(node,property_id,val);
}

bool XParticleShapeProperties::GetProperty(XBase* node,const XU32 property_id,XList& val)
{
  DO_TYPE_CHECK(node,XParticleShape);
  switch(property_id)
  {
  case XParticleShape::kEmitters: object->GetEmitters(val); return true;
  case XParticleShape::kFields:   object->GetFields(val); return true;
  default: break;
  }
  return XGeometryProperties::GetProperty(node,property_id,val);
}

bool XParticleShapeProperties::SetProperty(XBase* node,const XU32 property_id,const XU32& val)
{
  DO_TYPE_CHECK(node,XParticleShape);
  switch(property_id)
  {
  case XParticleShape::kLifespanType: 
    {
      if (val>=0 && val<= ((XU32)XParticleShape::kRandom))
      {
        object->SetLifespanType((XParticleShape::XLifespanType)val); 
        return true;
      }
      return false;
    }
  case XParticleShape::kParticleType: 
    {
      if (val>=0 && val<= ((XU32)XParticleShape::kNumeric))
      {
        object->SetParticleType((XParticleShape::XParticleType)val); 
        return true;
      }
      return false;
    }
  default: break;
  }
  return XShapeProperties::SetProperty(node,property_id,val);
}

bool XParticleShapeProperties::GetProperty(XBase* node,const XU32 property_id,XU32& val)
{
  DO_TYPE_CHECK(node,XParticleShape);
  switch(property_id)
  {
  case XParticleShape::kLifespanType: val = (XU32)object->GetLifespanType(); return true;
  case XParticleShape::kParticleType: val = (XU32)object->GetParticleType(); return true;
  default: break;
  }
  return XShapeProperties::GetProperty(node,property_id,val);
}

//----------------------------------------------------------------------------------------------------------------------
class XLightProperties : public XShapeProperties
{
  static const XU32 kNumProperties=6;
  static XPropertyInfo m_Properties[kNumProperties];
public:
  PROPERTY_COUNT(XLightProperties,XShapeProperties);
  PROPERTY_TYPE(XColour);
  PROPERTY_TYPE(XReal);
  PROPERTY_TYPE(bool);
};

XPropertyInfo XLightProperties::m_Properties[6] = 
{
  {XPropertyType::kReal,"intensity",XLight::kIntensity,XPropertyFlags::kAnimatable},
  {XPropertyType::kColour,"colour",XLight::kColour,XPropertyFlags::kAnimatable},
  {XPropertyType::kColour,"shadow_colour",XLight::kShadowColour,XPropertyFlags::kAnimatable},
  {XPropertyType::kBool,"emit_ambient",XLight::kEmitAmbient,0},
  {XPropertyType::kBool,"emit_diffuse",XLight::kEmitDiffuse,0},
  {XPropertyType::kBool,"emit_specular",XLight::kEmitSpecular,0}
};

DECLARE_NODE_PROPERTIES(XLight)
IMPLEMENT_PROPERTY_COMMON(XLightProperties,XShapeProperties);

bool XLightProperties::SetProperty(XBase* node,const XU32 property_id,const bool& val)
{
  DO_TYPE_CHECK(node,XLight);
  switch(property_id)
  {
  case XLight::kEmitAmbient:  object->SetEmitAmbient(val);  return true;
  case XLight::kEmitDiffuse:  object->SetEmitDiffuse(val);  return true;
  case XLight::kEmitSpecular: object->SetEmitSpecular(val); return true;
  default: break;
  }
  return XBaseProperties::SetProperty(node,property_id,val);
}

bool XLightProperties::GetProperty(XBase* node,const XU32 property_id,bool& val)
{
  DO_TYPE_CHECK(node,XLight);
  switch(property_id)
  {
  case XLight::kEmitAmbient:  val = object->GetEmitAmbient();  return true;
  case XLight::kEmitDiffuse:  val = object->GetEmitDiffuse();  return true;
  case XLight::kEmitSpecular: val = object->GetEmitSpecular(); return true;
  default: break;
  }
  return XBaseProperties::GetProperty(node,property_id,val);
}

bool XLightProperties::SetProperty(XBase* node,const XU32 property_id,const XReal& val)
{
  DO_TYPE_CHECK(node,XLight);
  switch(property_id)
  {
  case XLight::kIntensity: object->SetIntensity(val); return true;
  default: break;
  }
  return XBaseProperties::SetProperty(node,property_id,val);
}

bool XLightProperties::GetProperty(XBase* node,const XU32 property_id,XReal& val)
{
  DO_TYPE_CHECK(node,XLight);
  switch(property_id)
  {
  case XLight::kIntensity: val = object->GetIntensity(); return true;
  default: break;
  }
  return XBaseProperties::GetProperty(node,property_id,val);
}

bool XLightProperties::SetProperty(XBase* node,const XU32 property_id,const XColour& val)
{
  DO_TYPE_CHECK(node,XLight);
  switch(property_id)
  {
  case XLight::kColour: object->SetColour(val); return true;
  case XLight::kShadowColour: object->SetShadowColour(val); return true;
  default: break;
  }
  return XBaseProperties::SetProperty(node,property_id,val);
}

bool XLightProperties::GetProperty(XBase* node,const XU32 property_id,XColour& val)
{
  DO_TYPE_CHECK(node,XLight);
  switch(property_id)
  {
  case XLight::kColour: object->GetColour(val); return true;
  case XLight::kShadowColour: object->GetShadowColour(val); return true;
  default: break;
  }
  return XBaseProperties::GetProperty(node,property_id,val);
}

//----------------------------------------------------------------------------------------------------------------------
class XAmbientLightProperties : public XLightProperties
{
  static const XU32 kNumProperties=3;
  static XPropertyInfo m_Properties[kNumProperties];
public:
  PROPERTY_COUNT(XAmbientLightProperties,XLightProperties);
  PROPERTY_TYPE(XReal);
  PROPERTY_TYPE(bool);
};

XPropertyInfo XAmbientLightProperties::m_Properties[3] = 
{
  {XPropertyType::kReal,"ambient_shade",XAmbientLight::kAmbientShade,XPropertyFlags::kAnimatable},
  {XPropertyType::kReal,"shadow_radius",XAmbientLight::kShadowRadius,XPropertyFlags::kAnimatable},
  {XPropertyType::kBool,"casts_soft_shadows",XAmbientLight::kCastsSoftShadows,0}
};

DECLARE_NODE_PROPERTIES(XAmbientLight)
IMPLEMENT_PROPERTY_COMMON(XAmbientLightProperties,XLightProperties);

bool XAmbientLightProperties::SetProperty(XBase* node,const XU32 property_id,const XReal& val)
{
  DO_TYPE_CHECK(node,XAmbientLight);
  switch(property_id)
  {
  case XAmbientLight::kAmbientShade: object->SetAmbientShade(val); return true;
  case XAmbientLight::kShadowRadius: object->SetShadowRadius(val); return true;
  default: break;
  }
  return XLightProperties::SetProperty(node,property_id,val);
}

bool XAmbientLightProperties::GetProperty(XBase* node,const XU32 property_id,XReal& val)
{
  DO_TYPE_CHECK(node,XAmbientLight);
  switch(property_id)
  {
  case XAmbientLight::kAmbientShade: val = object->GetAmbientShade(); return true;
  case XAmbientLight::kShadowRadius: val = object->GetShadowRadius(); return true;
  default: break;
  }
  return XLightProperties::GetProperty(node,property_id,val);
}

bool XAmbientLightProperties::SetProperty(XBase* node,const XU32 property_id,const bool& val)
{
  DO_TYPE_CHECK(node,XAmbientLight);
  switch(property_id)
  {
  case XAmbientLight::kCastsSoftShadows: object->SetCastSoftShadows(val); return true;
  default: break;
  }
  return XLightProperties::SetProperty(node,property_id,val);
}

bool XAmbientLightProperties::GetProperty(XBase* node,const XU32 property_id,bool& val)
{
  DO_TYPE_CHECK(node,XAmbientLight);
  switch(property_id)
  {
  case XAmbientLight::kCastsSoftShadows: val = object->GetCastSoftShadows(); return true;
  default: break;
  }
  return XLightProperties::GetProperty(node,property_id,val);
}

//----------------------------------------------------------------------------------------------------------------------
class XExtendedLightProperties : public XLightProperties
{
  static const XU32 kNumProperties=7;
  static XPropertyInfo m_Properties[kNumProperties];
public:
  PROPERTY_COUNT(XExtendedLightProperties,XLightProperties);
  PROPERTY_TYPE(XReal);
  PROPERTY_TYPE(bool);
};

XPropertyInfo XExtendedLightProperties::m_Properties[7] = 
{
  {XPropertyType::kReal,"shadow_radius",XExtendedLight::kShadowRadius,XPropertyFlags::kAnimatable},
  {XPropertyType::kReal,"decay_rate",XExtendedLight::kDecayRate,XPropertyFlags::kAnimatable},
  {XPropertyType::kReal,"depth_map_filter_size",XExtendedLight::kDepthMapFilterSize,XPropertyFlags::kAnimatable},
  {XPropertyType::kReal,"depth_map_resolution",XExtendedLight::kDepthMapResolution,XPropertyFlags::kAnimatable},
  {XPropertyType::kReal,"depth_map_bias",XExtendedLight::kDepthMapBias,XPropertyFlags::kAnimatable},
  {XPropertyType::kBool,"use_depth_map_shadows",XExtendedLight::kUseDepthMapShadows,0},
  {XPropertyType::kBool,"casts_soft_shadows",XExtendedLight::kCastsSoftShadows,0}
};

DECLARE_NODE_PROPERTIES(XExtendedLight)
IMPLEMENT_PROPERTY_COMMON(XExtendedLightProperties,XLightProperties);

bool XExtendedLightProperties::SetProperty(XBase* node,const XU32 property_id,const XReal& val)
{
  DO_TYPE_CHECK(node,XExtendedLight);
  switch(property_id)
  {
  case XExtendedLight::kShadowRadius: object->SetShadowRadius(val); return true;
  case XExtendedLight::kDecayRate: object->SetDecayRate(val); return true;
  case XExtendedLight::kDepthMapFilterSize: object->SetDepthMapFilterSize(val); return true;
  case XExtendedLight::kDepthMapResolution: object->SetDepthMapResolution(val); return true;
  case XExtendedLight::kDepthMapBias: object->SetDepthMapBias(val); return true;
  default: break;
  }
  return XLightProperties::SetProperty(node,property_id,val);
}

bool XExtendedLightProperties::GetProperty(XBase* node,const XU32 property_id,XReal& val)
{
  DO_TYPE_CHECK(node,XExtendedLight);
  switch(property_id)
  {
  case XExtendedLight::kShadowRadius: val = object->GetShadowRadius(); return true;
  case XExtendedLight::kDecayRate: val = object->GetDecayRate(); return true;
  case XExtendedLight::kDepthMapFilterSize: val = object->GetDepthMapFilterSize(); return true;
  case XExtendedLight::kDepthMapResolution: val = object->GetDepthMapResolution(); return true;
  case XExtendedLight::kDepthMapBias: val = object->GetDepthMapBias(); return true;
  default: break;
  }
  return XLightProperties::GetProperty(node,property_id,val);
}

bool XExtendedLightProperties::SetProperty(XBase* node,const XU32 property_id,const bool& val)
{
  DO_TYPE_CHECK(node,XExtendedLight);
  switch(property_id)
  {
  case XExtendedLight::kUseDepthMapShadows: object->SetUseDepthMapShadows(val); return true;
  case XExtendedLight::kCastsSoftShadows: object->SetCastSoftShadows(val); return true;
  default: break;
  }
  return XLightProperties::SetProperty(node,property_id,val);
}

bool XExtendedLightProperties::GetProperty(XBase* node,const XU32 property_id,bool& val)
{
  DO_TYPE_CHECK(node,XExtendedLight);
  switch(property_id)
  {
  case XExtendedLight::kUseDepthMapShadows: val = object->GetUseDepthMapShadows(); return true;
  case XExtendedLight::kCastsSoftShadows: val = object->GetCastSoftShadows(); return true;
  default: break;
  }
  return XLightProperties::GetProperty(node,property_id,val);
}

//----------------------------------------------------------------------------------------------------------------------
class XDirectionalLightProperties : public XExtendedLightProperties
{
  static const XU32 kNumProperties=1;
  static XPropertyInfo m_Properties[kNumProperties];
public:
  PROPERTY_COUNT(XDirectionalLightProperties,XExtendedLightProperties);
  PROPERTY_TYPE(XReal);
};

XPropertyInfo XDirectionalLightProperties::m_Properties[1] = 
{
  {XPropertyType::kReal,"shadow_angle",XDirectionalLight::kShadowAngle,XPropertyFlags::kAnimatable},
};

DECLARE_NODE_PROPERTIES(XDirectionalLight)
IMPLEMENT_PROPERTY_COMMON(XDirectionalLightProperties,XExtendedLightProperties);

bool XDirectionalLightProperties::SetProperty(XBase* node,const XU32 property_id,const XReal& val)
{
  DO_TYPE_CHECK(node,XDirectionalLight);
  switch(property_id)
  {
  case XDirectionalLight::kShadowAngle: object->SetShadowAngle(val); return true;
  default: break;
  }
  return XExtendedLightProperties::SetProperty(node,property_id,val);
}

bool XDirectionalLightProperties::GetProperty(XBase* node,const XU32 property_id,XReal& val)
{
  DO_TYPE_CHECK(node,XDirectionalLight);
  switch(property_id)
  {
  case XDirectionalLight::kShadowAngle: val = object->GetShadowAngle(); return true;
  default: break;
  }
  return XExtendedLightProperties::GetProperty(node,property_id,val);
}

//----------------------------------------------------------------------------------------------------------------------
class XSpotLightProperties : public XExtendedLightProperties
{
  static const XU32 kNumProperties=15;
  static XPropertyInfo m_Properties[kNumProperties];
public:
  PROPERTY_COUNT(XSpotLightProperties,XExtendedLightProperties);
  PROPERTY_TYPE(XReal);
  PROPERTY_TYPE(bool);
};

XPropertyInfo XSpotLightProperties::m_Properties[15] = 
{
  {XPropertyType::kReal,"cone_angle",XSpotLight::kConeAngle,XPropertyFlags::kAnimatable},
  {XPropertyType::kReal,"penumbra_angle",XSpotLight::kPenumbraAngle,XPropertyFlags::kAnimatable},
  {XPropertyType::kReal,"drop_off",XSpotLight::kDropOff,XPropertyFlags::kAnimatable},
  {XPropertyType::kReal,"left_barn_door_angle",XSpotLight::kLeftBarnDoorAngle,XPropertyFlags::kAnimatable},
  {XPropertyType::kReal,"right_barn_door_angle",XSpotLight::kRightBarnDoorAngle,XPropertyFlags::kAnimatable},
  {XPropertyType::kReal,"top_barn_door_angle",XSpotLight::kTopBarnDoorAngle,XPropertyFlags::kAnimatable},
  {XPropertyType::kReal,"bottom_barn_door_angle",XSpotLight::kBottomBarnDoorAngle,XPropertyFlags::kAnimatable},

  {XPropertyType::kBool,"has_barn_doors",XSpotLight::kHasBarnDoors,0},
  {XPropertyType::kBool,"use_decay_regions",XSpotLight::kUseDecayRegions,0},

  {XPropertyType::kReal,"first_decay_region_start",XSpotLight::kFirstDecayRegionStart,0},
  {XPropertyType::kReal,"first_decay_region_end",XSpotLight::kFirstDecayRegionEnd,0},
  {XPropertyType::kReal,"second_decay_region_start",XSpotLight::kSecondDecayRegionStart,0},
  {XPropertyType::kReal,"second_decay_region_end",XSpotLight::kSecondDecayRegionEnd,0},
  {XPropertyType::kReal,"third_decay_region_start",XSpotLight::kThirdDecayRegionStart,0},
  {XPropertyType::kReal,"third_decay_region_end",XSpotLight::kThirdDecayRegionEnd,0}
};

DECLARE_NODE_PROPERTIES(XSpotLight)
IMPLEMENT_PROPERTY_COMMON(XSpotLightProperties,XExtendedLightProperties);

bool XSpotLightProperties::SetProperty(XBase* node,const XU32 property_id,const bool& val)
{
  DO_TYPE_CHECK(node,XSpotLight);
  switch(property_id)
  {
  case XSpotLight::kHasBarnDoors: object->SetHasBarnDoors(val); return true;
  case XSpotLight::kUseDecayRegions: object->SetUseDecayRegions(val); return true;
  default: break;
  }
  return XExtendedLightProperties::SetProperty(node,property_id,val);
}

bool XSpotLightProperties::GetProperty(XBase* node,const XU32 property_id,bool& val)
{
  DO_TYPE_CHECK(node,XSpotLight);
  switch(property_id)
  {
  case XSpotLight::kHasBarnDoors: val = object->GetHasBarnDoors(); return true;
  case XSpotLight::kUseDecayRegions: val = object->GetUseDecayRegions(); return true;
  default: break;
  }
  return XExtendedLightProperties::GetProperty(node,property_id,val);
}

static const XU32 kFirstDecayRegionStart  = XExtendedLight::kLast+10;// XReal
static const XU32 kFirstDecayRegionEnd    = XExtendedLight::kLast+11;// XReal
static const XU32 kSecondDecayRegionStart = XExtendedLight::kLast+12;// XReal
static const XU32 kSecondDecayRegionEnd   = XExtendedLight::kLast+13;// XReal
static const XU32 kThirdDecayRegionStart  = XExtendedLight::kLast+14;// XReal
static const XU32 kThirdDecayRegionEnd    = XExtendedLight::kLast+15;// XReal

bool XSpotLightProperties::SetProperty(XBase* node,const XU32 property_id,const XReal& val)
{
  DO_TYPE_CHECK(node,XSpotLight);
  XReal temps=0.0f;
  XReal tempe=0.0f;
  switch(property_id)
  {
  case XSpotLight::kConeAngle: object->SetConeAngle(val); return true;
  case XSpotLight::kPenumbraAngle: object->SetPenumbraAngle(val); return true;
  case XSpotLight::kDropOff: object->SetDropOff(val); return true;
  case XSpotLight::kLeftBarnDoorAngle: object->SetLeftBarnDoorAngle(val); return true;
  case XSpotLight::kRightBarnDoorAngle: object->SetRightBarnDoorAngle(val); return true;
  case XSpotLight::kTopBarnDoorAngle: object->SetTopBarnDoorAngle(val); return true;
  case XSpotLight::kBottomBarnDoorAngle: object->SetBottomBarnDoorAngle(val); return true;

  case XSpotLight::kFirstDecayRegionStart: 
    {
      object->GetFirstDecayRegion(temps,tempe);
      object->SetFirstDecayRegion(val,tempe);
    }
    return true;

  case XSpotLight::kFirstDecayRegionEnd: 
    {
      object->GetFirstDecayRegion(temps,tempe);
      object->SetFirstDecayRegion(temps,val);
    }
    return true;

  case XSpotLight::kSecondDecayRegionStart: 
    {
      object->GetSecondDecayRegion(temps,tempe);
      object->SetSecondDecayRegion(val,tempe);
    }
    return true;

  case XSpotLight::kSecondDecayRegionEnd: 
    {
      object->GetSecondDecayRegion(temps,tempe);
      object->SetSecondDecayRegion(temps,val);
    }
    return true;

  case XSpotLight::kThirdDecayRegionStart: 
    {
      object->GetThirdDecayRegion(temps,tempe);
      object->SetThirdDecayRegion(val,tempe);
    }
    return true;

  case XSpotLight::kThirdDecayRegionEnd: 
    {
      object->GetThirdDecayRegion(temps,tempe);
      object->SetThirdDecayRegion(temps,val);
    }
    return true;

  default: break;
  }
  return XExtendedLightProperties::SetProperty(node,property_id,val);
}

bool XSpotLightProperties::GetProperty(XBase* node,const XU32 property_id,XReal& val)
{
  DO_TYPE_CHECK(node,XSpotLight);
  XReal temp=0.0f;
  switch(property_id)
  {
  case XSpotLight::kConeAngle: val = object->GetConeAngle(); return true;
  case XSpotLight::kPenumbraAngle: val = object->GetPenumbraAngle(); return true;
  case XSpotLight::kDropOff: val = object->GetDropOff(); return true;
  case XSpotLight::kLeftBarnDoorAngle: val = object->GetLeftBarnDoorAngle(); return true;
  case XSpotLight::kRightBarnDoorAngle: val = object->GetRightBarnDoorAngle(); return true;
  case XSpotLight::kTopBarnDoorAngle: val = object->GetTopBarnDoorAngle(); return true;
  case XSpotLight::kBottomBarnDoorAngle: val = object->GetBottomBarnDoorAngle(); return true;

  case XSpotLight::kFirstDecayRegionStart: object->GetFirstDecayRegion(val,temp); return true;
  case XSpotLight::kFirstDecayRegionEnd: object->GetFirstDecayRegion(temp,val); return true;
  case XSpotLight::kSecondDecayRegionStart: object->GetSecondDecayRegion(val,temp); return true;
  case XSpotLight::kSecondDecayRegionEnd: object->GetSecondDecayRegion(temp,val); return true;
  case XSpotLight::kThirdDecayRegionStart: object->GetThirdDecayRegion(val,temp); return true;
  case XSpotLight::kThirdDecayRegionEnd: object->GetThirdDecayRegion(temp,val); return true;

  default: break;
  }
  return XExtendedLightProperties::GetProperty(node,property_id,val);
}

//----------------------------------------------------------------------------------------------------------------------
class XVolumeLightProperties : public XExtendedLightProperties
{
  static const XU32 kNumProperties=2;
  static XPropertyInfo m_Properties[kNumProperties];
public:
  PROPERTY_COUNT(XVolumeLightProperties,XExtendedLightProperties);
  PROPERTY_TYPE(XReal);
};

XPropertyInfo XVolumeLightProperties::m_Properties[2] = 
{
  {XPropertyType::kReal,"arc",XVolumeLight::kArc,XPropertyFlags::kAnimatable},
  {XPropertyType::kReal,"cone_end_radius",XVolumeLight::kConeEndRadius,XPropertyFlags::kAnimatable}
};

DECLARE_NODE_PROPERTIES(XVolumeLight)
IMPLEMENT_PROPERTY_COMMON(XVolumeLightProperties,XExtendedLightProperties);

bool XVolumeLightProperties::SetProperty(XBase* node,const XU32 property_id,const XReal& val)
{
  DO_TYPE_CHECK(node,XVolumeLight);
  switch(property_id)
  {
  case XVolumeLight::kArc: object->SetArc(val); return true;
  case XVolumeLight::kConeEndRadius: object->SetConeEndRadius(val); return true;
  default: break;
  }
  return XExtendedLightProperties::SetProperty(node,property_id,val);
}

bool XVolumeLightProperties::GetProperty(XBase* node,const XU32 property_id,XReal& val)
{
  DO_TYPE_CHECK(node,XVolumeLight);
  switch(property_id)
  {
  case XVolumeLight::kArc: val = object->GetArc(); return true;
  case XVolumeLight::kConeEndRadius: val = object->GetConeEndRadius(); return true;
  default: break;
  }
  return XExtendedLightProperties::GetProperty(node,property_id,val);
}

//----------------------------------------------------------------------------------------------------------------------
class XVolumeObjectProperties : public XShapeProperties
{
  static XPropertyInfo* m_Properties;
  static const XU32 kNumProperties=0;
public:
  PROPERTY_COUNT(XVolumeObjectProperties,XShapeProperties);
};
XPropertyInfo* XVolumeObjectProperties::m_Properties=0;
DECLARE_NODE_PROPERTIES(XVolumeObject)
IMPLEMENT_PROPERTY_COMMON(XVolumeObjectProperties,XShapeProperties);

//----------------------------------------------------------------------------------------------------------------------
class XVolumeBoxProperties : public XVolumeObjectProperties
{
  static const XU32 kNumProperties=3;
  static XPropertyInfo m_Properties[kNumProperties];
public:
  PROPERTY_COUNT(XVolumeBoxProperties,XVolumeObjectProperties);
  PROPERTY_TYPE(XReal);
};

XPropertyInfo XVolumeBoxProperties::m_Properties[3] = 
{
  {XPropertyType::kReal,"x_size",XVolumeBox::kSizeX,XPropertyFlags::kAnimatable},
  {XPropertyType::kReal,"y_size",XVolumeBox::kSizeY,XPropertyFlags::kAnimatable},
  {XPropertyType::kReal,"z_size",XVolumeBox::kSizeZ,XPropertyFlags::kAnimatable}
};

DECLARE_NODE_PROPERTIES(XVolumeBox)
IMPLEMENT_PROPERTY_COMMON(XVolumeBoxProperties,XVolumeObjectProperties);

bool XVolumeBoxProperties::SetProperty(XBase* node,const XU32 property_id,const XReal& val)
{
  DO_TYPE_CHECK(node,XVolumeBox);
  switch(property_id)
  {
  case XVolumeBox::kSizeX: object->SetX(val); return true;
  case XVolumeBox::kSizeY: object->SetY(val); return true;
  case XVolumeBox::kSizeZ: object->SetZ(val); return true;
  default: break;
  }
  return XVolumeObjectProperties::SetProperty(node,property_id,val);
}

bool XVolumeBoxProperties::GetProperty(XBase* node,const XU32 property_id,XReal& val)
{
  DO_TYPE_CHECK(node,XVolumeBox);
  switch(property_id)
  {
  case XVolumeBox::kSizeX: val = object->GetX(); return true;
  case XVolumeBox::kSizeY: val = object->GetY(); return true;
  case XVolumeBox::kSizeZ: val = object->GetZ(); return true;
  default: break;
  }
  return XVolumeObjectProperties::GetProperty(node,property_id,val);
}

//----------------------------------------------------------------------------------------------------------------------
class XVolumeConeProperties : public XVolumeObjectProperties
{
  static const XU32 kNumProperties=2;
  static XPropertyInfo m_Properties[kNumProperties];
public:
  PROPERTY_COUNT(XVolumeConeProperties,XVolumeObjectProperties);
  PROPERTY_TYPE(XReal);
};

XPropertyInfo XVolumeConeProperties::m_Properties[2] = 
{
  {XPropertyType::kReal,"angle",XVolumeCone::kAngle,XPropertyFlags::kAnimatable},
  {XPropertyType::kReal,"cap",XVolumeCone::kCap,XPropertyFlags::kAnimatable}
};

DECLARE_NODE_PROPERTIES(XVolumeCone)
IMPLEMENT_PROPERTY_COMMON(XVolumeConeProperties,XVolumeObjectProperties);

bool XVolumeConeProperties::SetProperty(XBase* node,const XU32 property_id,const XReal& val)
{
  DO_TYPE_CHECK(node,XVolumeCone);
  switch(property_id)
  {
  case XVolumeCone::kAngle: object->SetAngle(val); return true;
  case XVolumeCone::kCap: object->SetCap(val); return true;
  default: break;
  }
  return XVolumeObjectProperties::SetProperty(node,property_id,val);
}

bool XVolumeConeProperties::GetProperty(XBase* node,const XU32 property_id,XReal& val)
{
  DO_TYPE_CHECK(node,XVolumeCone);
  switch(property_id)
  {
  case XVolumeCone::kAngle: val = object->GetAngle(); return true;
  case XVolumeCone::kCap: val = object->GetCap(); return true;
  default: break;
  }
  return XVolumeObjectProperties::GetProperty(node,property_id,val);
}

//----------------------------------------------------------------------------------------------------------------------
class XVolumeCylinderProperties : public XVolumeObjectProperties
{
  static const XU32 kNumProperties=2;
  static XPropertyInfo m_Properties[kNumProperties];
public:
  PROPERTY_COUNT(XVolumeCylinderProperties,XVolumeObjectProperties);
  PROPERTY_TYPE(XReal);
};

XPropertyInfo XVolumeCylinderProperties::m_Properties[2] = 
{
  {XPropertyType::kReal,"radius",XVolumeCylinder::kRadius,XPropertyFlags::kAnimatable},
  {XPropertyType::kReal,"height",XVolumeCylinder::kHeight,XPropertyFlags::kAnimatable}
};

DECLARE_NODE_PROPERTIES(XVolumeCylinder)
IMPLEMENT_PROPERTY_COMMON(XVolumeCylinderProperties,XVolumeObjectProperties);

bool XVolumeCylinderProperties::SetProperty(XBase* node,const XU32 property_id,const XReal& val)
{
  DO_TYPE_CHECK(node,XVolumeCylinder);
  switch(property_id)
  {
  case XVolumeCylinder::kRadius: object->SetRadius(val); return true;
  case XVolumeCylinder::kHeight: object->SetHeight(val); return true;
  default: break;
  }
  return XVolumeObjectProperties::SetProperty(node,property_id,val);
}

bool XVolumeCylinderProperties::GetProperty(XBase* node,const XU32 property_id,XReal& val)
{
  DO_TYPE_CHECK(node,XVolumeCylinder);
  switch(property_id)
  {
  case XVolumeCylinder::kRadius: val = object->GetRadius(); return true;
  case XVolumeCylinder::kHeight: val = object->GetHeight(); return true;
  default: break;
  }
  return XVolumeObjectProperties::GetProperty(node,property_id,val);
}

//----------------------------------------------------------------------------------------------------------------------
class XVolumeSphereProperties : public XVolumeObjectProperties
{
  static const XU32 kNumProperties=1;
  static XPropertyInfo m_Properties[kNumProperties];
public:
  PROPERTY_COUNT(XVolumeSphereProperties,XVolumeObjectProperties);
  PROPERTY_TYPE(XReal);
};

XPropertyInfo XVolumeSphereProperties::m_Properties[1] = 
{
  {XPropertyType::kReal,"radius",XVolumeSphere::kRadius,XPropertyFlags::kAnimatable}
};

DECLARE_NODE_PROPERTIES(XVolumeSphere)
IMPLEMENT_PROPERTY_COMMON(XVolumeSphereProperties,XVolumeObjectProperties);

bool XVolumeSphereProperties::SetProperty(XBase* node,const XU32 property_id,const XReal& val)
{
  DO_TYPE_CHECK(node,XVolumeSphere);
  switch(property_id)
  {
  case XVolumeSphere::kRadius: object->SetRadius(val); return true;
  default: break;
  }
  return XVolumeObjectProperties::SetProperty(node,property_id,val);
}

bool XVolumeSphereProperties::GetProperty(XBase* node,const XU32 property_id,XReal& val)
{
  DO_TYPE_CHECK(node,XVolumeSphere);
  switch(property_id)
  {
  case XVolumeSphere::kRadius: val = object->GetRadius(); return true;
  default: break;
  }
  return XVolumeObjectProperties::GetProperty(node,property_id,val);
}

//----------------------------------------------------------------------------------------------------------------------
class XTexPlacement2DProperties : public XBaseProperties
{
  static const XU32 kNumProperties=20;
  static XPropertyInfo m_Properties[kNumProperties];
public:
  PROPERTY_COUNT(XTexPlacement2DProperties,XBaseProperties);
  PROPERTY_TYPE(XReal);
  PROPERTY_TYPE(bool);
  PROPERTY_TYPE(XU32);
  PROPERTY_TYPE(XString);
};

XPropertyInfo XTexPlacement2DProperties::m_Properties[20] = 
{
  {XPropertyType::kReal,"translate_frame_u",XTexPlacement2D::kTranslateFrameU,XPropertyFlags::kAnimatable},
  {XPropertyType::kReal,"translate_frame_v",XTexPlacement2D::kTranslateFrameV,XPropertyFlags::kAnimatable},
  {XPropertyType::kReal,"rotate_frame",XTexPlacement2D::kRotateFrame,XPropertyFlags::kAnimatable},
  {XPropertyType::kReal,"repeat_u",XTexPlacement2D::kRepeatU,XPropertyFlags::kAnimatable},
  {XPropertyType::kReal,"repeat_v",XTexPlacement2D::kRepeatV,XPropertyFlags::kAnimatable},
  {XPropertyType::kReal,"offset_u",XTexPlacement2D::kOffsetU,XPropertyFlags::kAnimatable},
  {XPropertyType::kReal,"offset_v",XTexPlacement2D::kOffsetV,XPropertyFlags::kAnimatable},
  {XPropertyType::kReal,"rotate_uv",XTexPlacement2D::kRotateUV,XPropertyFlags::kAnimatable},
  {XPropertyType::kReal,"noise_u",XTexPlacement2D::kNoiseU,XPropertyFlags::kAnimatable},
  {XPropertyType::kReal,"noise_v",XTexPlacement2D::kNoiseV,XPropertyFlags::kAnimatable},
  {XPropertyType::kString,"file_texture_name",XTexPlacement2D::kFileName,0},
  {XPropertyType::kUnsigned,"filter_type",XTexPlacement2D::kFilterType,0},
  {XPropertyType::kReal,"filter_amount",XTexPlacement2D::kFilterAmount,0},
  {XPropertyType::kBool,"pre_filter",XTexPlacement2D::kPreFilter,0},
  {XPropertyType::kReal,"pre_filter_radius",XTexPlacement2D::kPreFilterRadius,0},
  {XPropertyType::kBool,"is_mirrored_u",XTexPlacement2D::kIsMirroredU,0},
  {XPropertyType::kBool,"is_mirrored_v",XTexPlacement2D::kIsMirroredV,0},
  {XPropertyType::kBool,"is_staggered",XTexPlacement2D::kIsStaggered,0},
  {XPropertyType::kBool,"is_wrapped_u",XTexPlacement2D::kIsWrappedU,0},
  {XPropertyType::kBool,"is_wrapped_v",XTexPlacement2D::kIsWrappedV,0}
};

DECLARE_NODE_PROPERTIES(XTexPlacement2D)
IMPLEMENT_PROPERTY_COMMON(XTexPlacement2DProperties,XBaseProperties);


bool XTexPlacement2DProperties::SetProperty(XBase* node,const XU32 property_id,const XString& val)
{
  DO_TYPE_CHECK(node,XTexPlacement2D);
  switch(property_id)
  {
  case XTexPlacement2D::kFileName: object->SetFileTextureName(val.c_str()); return true;
  default: break;
  }
  return XBaseProperties::SetProperty(node,property_id,val);
}

bool XTexPlacement2DProperties::GetProperty(XBase* node,const XU32 property_id,XString& val)
{
  DO_TYPE_CHECK(node,XTexPlacement2D);
  switch(property_id)
  {
  case XTexPlacement2D::kFileName: val = object->GetFileTextureName(); return true;
  default: break;
  }
  return XBaseProperties::GetProperty(node,property_id,val);
}

bool XTexPlacement2DProperties::SetProperty(XBase* node,const XU32 property_id,const XU32& val)
{
  DO_TYPE_CHECK(node,XTexPlacement2D);
  switch(property_id)
  {
  case XTexPlacement2D::kFilterType: object->SetFilterType((XTexPlacement2D::FilterType)val); return true;
  default: break;
  }
  return XBaseProperties::SetProperty(node,property_id,val);
}

bool XTexPlacement2DProperties::GetProperty(XBase* node,const XU32 property_id,XU32& val)
{
  DO_TYPE_CHECK(node,XTexPlacement2D);
  switch(property_id)
  {
  case XTexPlacement2D::kFilterType: val = (XU32)object->GetFilterType(); return true;
  default: break;
  }
  return XBaseProperties::GetProperty(node,property_id,val);
}

bool XTexPlacement2DProperties::SetProperty(XBase* node,const XU32 property_id,const bool& val)
{
  DO_TYPE_CHECK(node,XTexPlacement2D);
  switch(property_id)
  {
  case XTexPlacement2D::kPreFilter: object->SetPreFilter(val); return true;
  case XTexPlacement2D::kIsMirroredU: object->SetIsMirroredU(val); return true;
  case XTexPlacement2D::kIsMirroredV: object->SetIsMirroredV(val); return true;
  case XTexPlacement2D::kIsStaggered: object->SetIsStaggered(val); return true;
  case XTexPlacement2D::kIsWrappedU: object->SetIsWrappedU(val); return true;
  case XTexPlacement2D::kIsWrappedV: object->SetIsWrappedV(val); return true;
  default: break;
  }
  return XBaseProperties::SetProperty(node,property_id,val);
}

bool XTexPlacement2DProperties::GetProperty(XBase* node,const XU32 property_id,bool& val)
{
  DO_TYPE_CHECK(node,XTexPlacement2D);
  switch(property_id)
  {
  case XTexPlacement2D::kPreFilter: val = object->GetPreFilter(); return true;
  case XTexPlacement2D::kIsMirroredU: val = object->GetIsMirroredU(); return true;
  case XTexPlacement2D::kIsMirroredV: val = object->GetIsMirroredV(); return true;
  case XTexPlacement2D::kIsStaggered: val = object->GetIsStaggered(); return true;
  case XTexPlacement2D::kIsWrappedU: val = object->GetIsWrappedU(); return true;
  case XTexPlacement2D::kIsWrappedV: val = object->GetIsWrappedV(); return true;
  default: break;
  }
  return XBaseProperties::GetProperty(node,property_id,val);
}

bool XTexPlacement2DProperties::SetProperty(XBase* node,const XU32 property_id,const XReal& val)
{
  DO_TYPE_CHECK(node,XTexPlacement2D);
  switch(property_id)
  {
  case XTexPlacement2D::kTranslateFrameU: object->SetTranslateFrameU(val); return true;
  case XTexPlacement2D::kTranslateFrameV: object->SetTranslateFrameV(val); return true;
  case XTexPlacement2D::kRotateFrame: object->SetRotateFrame(val); return true;
  case XTexPlacement2D::kRepeatU: object->SetRepeatU(val); return true;
  case XTexPlacement2D::kRepeatV: object->SetRepeatV(val); return true;
  case XTexPlacement2D::kOffsetU: object->SetOffsetU(val); return true;
  case XTexPlacement2D::kOffsetV: object->SetOffsetV(val); return true;
  case XTexPlacement2D::kRotateUV: object->SetRotateUV(val); return true;
  case XTexPlacement2D::kNoiseU: object->SetNoiseU(val); return true;
  case XTexPlacement2D::kNoiseV: object->SetNoiseV(val); return true;
  case XTexPlacement2D::kFilterAmount: object->SetFilterAmount(val); return true;
  case XTexPlacement2D::kPreFilterRadius: object->SetPreFilterRadius(val); return true;
  default: break;
  }
  return XBaseProperties::SetProperty(node,property_id,val);
}

bool XTexPlacement2DProperties::GetProperty(XBase* node,const XU32 property_id,XReal& val)
{
  DO_TYPE_CHECK(node,XTexPlacement2D);
  switch(property_id)
  {
  case XTexPlacement2D::kTranslateFrameU: val = object->GetTranslateFrameU(); return true;
  case XTexPlacement2D::kTranslateFrameV: val = object->GetTranslateFrameV(); return true;
  case XTexPlacement2D::kRotateFrame: val = object->GetRotateFrame(); return true;
  case XTexPlacement2D::kRepeatU: val = object->GetRepeatU(); return true;
  case XTexPlacement2D::kRepeatV: val = object->GetRepeatV(); return true;
  case XTexPlacement2D::kOffsetU: val = object->GetOffsetU(); return true;
  case XTexPlacement2D::kOffsetV: val = object->GetOffsetV(); return true;
  case XTexPlacement2D::kRotateUV: val = object->GetRotateUV(); return true;
  case XTexPlacement2D::kNoiseU: val = object->GetNoiseU(); return true;
  case XTexPlacement2D::kNoiseV: val = object->GetNoiseV(); return true;
  case XTexPlacement2D::kFilterAmount: val = object->GetFilterAmount(); return true;
  case XTexPlacement2D::kPreFilterRadius: val = object->GetPreFilterRadius(); return true;
  default: break;
  }
  return XBaseProperties::GetProperty(node,property_id,val);
}

//----------------------------------------------------------------------------------------------------------------------
class XTextureProperties : public XBaseProperties
{
  static XPropertyInfo* m_Properties;
  static const XU32 kNumProperties=0;
public:
  static const XU32 kLast = XBaseProperties::kLast;
  PROPERTY_COUNT(XTextureProperties,XBaseProperties);
};

DECLARE_NODE_PROPERTIES(XTexture)
IMPLEMENT_PROPERTY_COMMON(XTextureProperties,XBaseProperties);
XPropertyInfo* XTextureProperties::m_Properties=0;

//----------------------------------------------------------------------------------------------------------------------
class XBumpMapProperties : public XTextureProperties
{
  static const XU32 kNumProperties=2;
  static XPropertyInfo m_Properties[kNumProperties];
public:
  PROPERTY_COUNT(XBumpMapProperties,XTextureProperties);
  PROPERTY_TYPE(XReal);
  PROPERTY_TYPE(XBase*);
};

DECLARE_NODE_PROPERTIES(XBumpMap)
IMPLEMENT_PROPERTY_COMMON(XBumpMapProperties,XTextureProperties);

XPropertyInfo XBumpMapProperties::m_Properties[2] = 
{
  {XPropertyType::kReal,"bump_depth",XBumpTexture::kBumpDepth,XPropertyFlags::kAnimatable},
  {XPropertyType::kReal,"placement",XBumpTexture::kPlacement,0}
};

bool XBumpMapProperties::SetProperty(XBase* node,const XU32 property_id,const XReal& val)
{
  DO_TYPE_CHECK(node,XBumpMap);
  switch(property_id)
  {
  case XBumpMap::kBumpDepth: object->SetBumpDepth(val); return true;
  default: break;
  }
  return XBaseProperties::SetProperty(node,property_id,val);
}

bool XBumpMapProperties::GetProperty(XBase* node,const XU32 property_id,XReal& val)
{
  DO_TYPE_CHECK(node,XBumpMap);
  switch(property_id)
  {
  case XBumpMap::kBumpDepth: val = object->GetBumpDepth(); return true;
  default: break;
  }
  return XBaseProperties::GetProperty(node,property_id,val);
}

bool XBumpMapProperties::SetProperty(XBase* node,const XU32 property_id,const XBase*& val)
{
  DO_TYPE_CHECK(node,XBumpMap);
  switch(property_id)
  {
  case XBumpTexture::kPlacement: object->SetPlacement(val); return true;
  default: break;
  }
  return XBaseProperties::SetProperty(node,property_id,val);
}

bool XBumpMapProperties::GetProperty(XBase* node,const XU32 property_id,XBase*& val)
{
  DO_TYPE_CHECK(node,XBumpMap);
  switch(property_id)
  {
  case XBumpTexture::kPlacement: val = object->GetPlacement(); return true;
  default: break;
  }
  return XBaseProperties::GetProperty(node,property_id,val);
}

//----------------------------------------------------------------------------------------------------------------------
class XEnvmapBaseTextureProperties : public XTextureProperties
{
  static const XU32 kNumProperties=1;
  static XPropertyInfo m_Properties[kNumProperties];
public:
  PROPERTY_COUNT(XEnvmapBaseTextureProperties,XTextureProperties);
  PROPERTY_TYPE(XBase*);
};

DECLARE_NODE_PROPERTIES(XEnvmapBaseTexture)
IMPLEMENT_PROPERTY_COMMON(XEnvmapBaseTextureProperties,XTextureProperties);

XPropertyInfo XEnvmapBaseTextureProperties::m_Properties[1] = 
{
  {XPropertyType::kNode,"placement",XEnvmapBaseTexture::k3dPlacement,0,XFn::TexturePlacement3D}
};

bool XEnvmapBaseTextureProperties::SetProperty(XBase* node,const XU32 property_id,const XBase*& val)
{
  DO_TYPE_CHECK(node,XEnvmapBaseTexture);
  switch(property_id)
  {
  case XEnvmapBaseTexture::k3dPlacement: object->SetPlacement3D(val); return true;
  default: break;
  }
  return XTextureProperties::SetProperty(node,property_id,val);
}

bool XEnvmapBaseTextureProperties::GetProperty(XBase* node,const XU32 property_id,XBase*& val)
{
  DO_TYPE_CHECK(node,XEnvmapBaseTexture);
  switch(property_id)
  {
  case XEnvmapBaseTexture::k3dPlacement: val = object->GetPlacement3D(); return true;
  default: break;
  }
  return XTextureProperties::GetProperty(node,property_id,val);
}

//----------------------------------------------------------------------------------------------------------------------
class XCubeMapProperties : public XEnvmapBaseTextureProperties
{
  static const XU32 kNumProperties=7;
  static XPropertyInfo m_Properties[kNumProperties];
public:
  PROPERTY_COUNT(XCubeMapProperties,XEnvmapBaseTextureProperties);
  PROPERTY_TYPE(bool);
  PROPERTY_TYPE(XBase*);
};

DECLARE_NODE_PROPERTIES(XCubeMap)
IMPLEMENT_PROPERTY_COMMON(XCubeMapProperties,XEnvmapBaseTextureProperties);

XPropertyInfo XCubeMapProperties::m_Properties[7] = 
{
  {XPropertyType::kBool,"dynamic",XCubeMap::kDynamic,0},
  {XPropertyType::kNode,"left",XCubeMap::kLeft,0,XFn::TexturePlacement2D},
  {XPropertyType::kNode,"right",XCubeMap::kRight,0,XFn::TexturePlacement2D},
  {XPropertyType::kNode,"top",XCubeMap::kTop,0,XFn::TexturePlacement2D},
  {XPropertyType::kNode,"bottom",XCubeMap::kBottom,0,XFn::TexturePlacement2D},
  {XPropertyType::kNode,"front",XCubeMap::kFront,0,XFn::TexturePlacement2D},
  {XPropertyType::kNode,"back",XCubeMap::kBack,0,XFn::TexturePlacement2D}
};

bool XCubeMapProperties::SetProperty(XBase* node,const XU32 property_id,const bool& val)
{
  DO_TYPE_CHECK(node,XCubeMap);
  switch(property_id)
  {
  case XCubeMap::kDynamic: object->SetIsDynamic(val); return true;
  default: break;
  }
  return XBaseProperties::SetProperty(node,property_id,val);
}

bool XCubeMapProperties::GetProperty(XBase* node,const XU32 property_id,bool& val)
{
  DO_TYPE_CHECK(node,XCubeMap);
  switch(property_id)
  {
  case XCubeMap::kDynamic: val = object->GetIsDynamic(); return true;
  default: break;
  }
  return XBaseProperties::GetProperty(node,property_id,val);
}

bool XCubeMapProperties::SetProperty(XBase* node,const XU32 property_id,const XBase*& val)
{
  DO_TYPE_CHECK(node,XCubeMap);
  const XTexPlacement2D* placement = (val)->HasFn<XTexPlacement2D>();
  switch(property_id)
  {
  case XCubeMap::kLeft:   object->SetLeftPlacement(placement);   return true;
  case XCubeMap::kRight:  object->SetRightPlacement(placement);  return true;
  case XCubeMap::kTop:    object->SetTopPlacement(placement);    return true;
  case XCubeMap::kBottom: object->SetBottomPlacement(placement); return true;
  case XCubeMap::kFront:  object->SetFrontPlacement(placement);  return true;
  case XCubeMap::kBack:   object->SetBackPlacement(placement);   return true;
  default: break;
  }
  return XEnvmapBaseTextureProperties::SetProperty(node,property_id,val);
}

bool XCubeMapProperties::GetProperty(XBase* node,const XU32 property_id,XBase*& val)
{
  DO_TYPE_CHECK(node,XCubeMap);
  switch(property_id)
  {
  case XCubeMap::kLeft:   val = object->GetLeftPlacement();   return true;
  case XCubeMap::kRight:  val = object->GetRightPlacement();  return true;
  case XCubeMap::kTop:    val = object->GetTopPlacement();    return true;
  case XCubeMap::kBottom: val = object->GetBottomPlacement(); return true;
  case XCubeMap::kFront:  val = object->GetFrontPlacement();  return true;
  case XCubeMap::kBack:   val = object->GetBackPlacement();   return true;
  default: break;
  }
  return XEnvmapBaseTextureProperties::GetProperty(node,property_id,val);
}

//----------------------------------------------------------------------------------------------------------------------
class XSphereMapProperties : public XEnvmapBaseTextureProperties
{
  static const XU32 kNumProperties=1;
  static XPropertyInfo m_Properties[kNumProperties];
public:
  PROPERTY_COUNT(XSphereMapProperties,XEnvmapBaseTextureProperties);
  PROPERTY_TYPE(XBase*);
};

DECLARE_NODE_PROPERTIES(XSphereMap)
IMPLEMENT_PROPERTY_COMMON(XSphereMapProperties,XEnvmapBaseTextureProperties);

XPropertyInfo XSphereMapProperties::m_Properties[1] = 
{
  {XPropertyType::kNode,"image",XSphereMap::kPlacement,0,XFn::TexturePlacement2D}
};

bool XSphereMapProperties::SetProperty(XBase* node,const XU32 property_id,const XBase*& val)
{
  DO_TYPE_CHECK(node,XSphereMap);
  switch(property_id)
  {
  case XSphereMap::kPlacement: return object->SetPlacement(val); 
  default: break;
  }
  return XEnvmapBaseTextureProperties::SetProperty(node,property_id,val);
}

bool XSphereMapProperties::GetProperty(XBase* node,const XU32 property_id,XBase*& val)
{
  DO_TYPE_CHECK(node,XSphereMap);
  switch(property_id)
  {
  case XSphereMap::kPlacement: val = object->GetPlacement(); return true;
  default: break;
  }
  return XEnvmapBaseTextureProperties::GetProperty(node,property_id,val);
}
}
