//----------------------------------------------------------------------------------------------------------------------
/// \file Model.cpp
/// \note (C) Copyright 2003-2005 Robert Bateman. All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#include <time.h>
#if defined(WIN32) || defined(WIN64)
#include <windows.h>
#endif
#include "XMD/Model.h"
#include "XMD/FileIO.h"
#include "XMD/AnimCycle.h"
#include "XMD/Bone.h"
#include "XMD/Deformer.h"
#include "XMD/Factory.h"
#include "XMD/Info.h"
#include "XMD/ExtraAttributes.h"
#include "XMD/Geometry.h"
#include "XMD/Instance.h"
#include "XMD/ShadingNode.h"
#include "XMD/Material.h"
#include "XMD/Texture.h"

#include "XMD/Air.h"
#include "XMD/AmbientLight.h"
#include "XMD/Anisotropic.h"
#include "XMD/AnimationTake.h"
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
#include "XMD/LayeredTexture.h"
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
#include "XMD/PolygonMesh.h"
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

#include "nmtl/algorithm.h"

#include "XMU/FileSystem.h"
#ifdef WIN32
#include <windows.h>
#else
#endif

#include <string>
#include <sstream>


#define REGISTER_CHUNK_TYPE( CLASS )                    \
 {                                                    \
   IXCreateBase* pinfo = new IXCreateBase_##CLASS;    \
   if( !XGlobal::RegisterTypeInfo( pinfo ) )          \
   {                                                  \
     XGlobal::GetLogger()->Logf(XBasicLogger::kError, \
       "Registering failed %s",                       \
        pinfo->GetTypeString().c_str());                      \
     delete pinfo;                                    \
   }                                                  \
 }

#if _DEBUG
#define CHECK_RCHUNK_BEGIN(IO) XU32 __temp__i = (IO).CurrPos() - 8;
#define CHECK_RCHUNK_END(IO,CHUNK,SZ)                             \
  if( (__temp__i + (SZ)) != ((IO).CurrPos()) )                  \
  {                                                              \
    XGlobal::GetLogger()->Logf(XBasicLogger::kError,            \
        "Chunk size != data written with "                      \
        "type: %d id: %d name: %s DS: %d"                       \
        "[START_POS=%d][END_POS=%d]",                           \
          ((CHUNK).GetApiType()),                               \
          ((CHUNK).GetID()),                                     \
          ((CHUNK).GetName()),                                  \
          (SZ),__temp__i,(IO).CurrPos());                       \
    XMD_ASSERT(0 && "ERROR! Chunk size != data written" );          \
  }

#define CHECK_CHUNK_BEGIN(IO) XU32 __temp__i = (IO).CurrPos();
#define CHECK_CHUNK_END(IO,CHUNK)                                 \
  if( (__temp__i + (CHUNK).GetDataSize()) != ((IO).CurrPos()) ) \
  {                                                              \
    XGlobal::GetLogger()->Logf(XBasicLogger::kError,            \
        "Chunk size != data written with "                      \
        "type: %d id: %d name: %s DS: %d"                       \
        "[START_POS=%d][END_POS=%d]",                           \
          ((CHUNK).GetApiType()),                               \
          ((CHUNK).GetID()),                                     \
          ((CHUNK).GetName()),                                  \
          ((CHUNK).GetDataSize()),__temp__i,(IO).CurrPos());    \
    XMD_ASSERT(0 && "ERROR! Chunk size != data written" );          \
  }

#else
#define CHECK_RCHUNK_BEGIN(IO) ;
#define CHECK_RCHUNK_END(IO,CHUNK,SZ) ;
#define CHECK_CHUNK_BEGIN(IO) ;
#define CHECK_CHUNK_END(IO,CHUNK) ;
#endif

NMTL_POD_VECTOR_EXPORT(XMD::XBase*,XMD_EXPORT,XM2);
namespace XMD
{

/// the default stdout logger
static XStdOutLogger g_DefaultLogger;

//----------------------------------------------------------------------------------------------------------------------
XMD_EXPORT bool XMDInit(XBasicLogger* logger)
{
  DUMPER(XMDInit);
  XGlobal::Init(logger);

  // register abstract types
  REGISTER_CHUNK_TYPE( XBase );
  REGISTER_CHUNK_TYPE( XConstraint );
  REGISTER_CHUNK_TYPE( XField );
  REGISTER_CHUNK_TYPE( XDeformer);
  REGISTER_CHUNK_TYPE( XNonLinearDeformer );
  REGISTER_CHUNK_TYPE( XShadingNode );
  REGISTER_CHUNK_TYPE( XMaterial );
  REGISTER_CHUNK_TYPE( XShape );
  REGISTER_CHUNK_TYPE( XCollisionObject );
  REGISTER_CHUNK_TYPE( XGeometry );
  REGISTER_CHUNK_TYPE( XParametricBase );
  REGISTER_CHUNK_TYPE( XParametricCurve );
  REGISTER_CHUNK_TYPE( XParametricSurface );
  REGISTER_CHUNK_TYPE( XLight );
  REGISTER_CHUNK_TYPE( XExtendedLight );
  REGISTER_CHUNK_TYPE( XVolumeObject );
  REGISTER_CHUNK_TYPE( XTexture );
  REGISTER_CHUNK_TYPE( XEnvmapBaseTexture );
  REGISTER_CHUNK_TYPE( XNodeCollection );

  // register concrete types
  REGISTER_CHUNK_TYPE( XAimConstraint );
  REGISTER_CHUNK_TYPE( XAmbientLight );
  REGISTER_CHUNK_TYPE( XAnisotropic );
  REGISTER_CHUNK_TYPE( XAnimCycle );
  REGISTER_CHUNK_TYPE( XAnimationTake );
  REGISTER_CHUNK_TYPE( XAir );
  REGISTER_CHUNK_TYPE( XAreaLight );
  REGISTER_CHUNK_TYPE( XBaseLattice );
  REGISTER_CHUNK_TYPE( XBone );
  REGISTER_CHUNK_TYPE( XBezierCurve );
  REGISTER_CHUNK_TYPE( XBezierPatch );
  REGISTER_CHUNK_TYPE( XBlinn );
  REGISTER_CHUNK_TYPE( XBlendShape );
  REGISTER_CHUNK_TYPE( XBumpMap );
  REGISTER_CHUNK_TYPE( XCamera );
  REGISTER_CHUNK_TYPE( XCluster );
  REGISTER_CHUNK_TYPE( XCollisionBox );
  REGISTER_CHUNK_TYPE( XCollisionCone );
  REGISTER_CHUNK_TYPE( XCollisionCapsule );
  REGISTER_CHUNK_TYPE( XCollisionCylinder );
  REGISTER_CHUNK_TYPE( XCollisionMesh );
  REGISTER_CHUNK_TYPE( XCollisionSphere );
  REGISTER_CHUNK_TYPE( XCubeMap );
  REGISTER_CHUNK_TYPE( XDirectionalLight );
  REGISTER_CHUNK_TYPE( XDisplayLayer );
  REGISTER_CHUNK_TYPE( XDrag );
  REGISTER_CHUNK_TYPE( XEmitter );
  REGISTER_CHUNK_TYPE( XFFD );
  REGISTER_CHUNK_TYPE( XGeometryConstraint );
  REGISTER_CHUNK_TYPE( XGravity );
  REGISTER_CHUNK_TYPE( XHermiteCurve );
  REGISTER_CHUNK_TYPE( XHermitePatch );
  REGISTER_CHUNK_TYPE( XHwShaderNode );
  REGISTER_CHUNK_TYPE( XIkChain );
  REGISTER_CHUNK_TYPE( XJoint );
  REGISTER_CHUNK_TYPE( XJiggleDeformer );
  REGISTER_CHUNK_TYPE( XJointCluster );
  REGISTER_CHUNK_TYPE( XLambert );
  REGISTER_CHUNK_TYPE( XLattice );
  REGISTER_CHUNK_TYPE( XLayeredShader );
  REGISTER_CHUNK_TYPE( XLayeredTexture );
  REGISTER_CHUNK_TYPE( XLocator );
  REGISTER_CHUNK_TYPE( XLookAtConstraint );
  REGISTER_CHUNK_TYPE( XMesh );
  REGISTER_CHUNK_TYPE( XMeshVertexColours );
  REGISTER_CHUNK_TYPE( XNewton );
  REGISTER_CHUNK_TYPE( XNonLinearBend );
  REGISTER_CHUNK_TYPE( XNonLinearFlare );
  REGISTER_CHUNK_TYPE( XNonLinearSine );
  REGISTER_CHUNK_TYPE( XNonLinearSquash );
  REGISTER_CHUNK_TYPE( XNonLinearTwist );
  REGISTER_CHUNK_TYPE( XNonLinearWave );
  REGISTER_CHUNK_TYPE( XNormalConstraint );
  REGISTER_CHUNK_TYPE( XNurbsCurve );
  REGISTER_CHUNK_TYPE( XNurbsSurface );
  REGISTER_CHUNK_TYPE( XObjectSet );
  REGISTER_CHUNK_TYPE( XOrientConstraint );
  REGISTER_CHUNK_TYPE( XParentConstraint );
  REGISTER_CHUNK_TYPE( XParticleShape );
  REGISTER_CHUNK_TYPE( XPhong );
  REGISTER_CHUNK_TYPE( XPhongE );
  REGISTER_CHUNK_TYPE( XPointConstraint );
  REGISTER_CHUNK_TYPE( XPointLight );
  REGISTER_CHUNK_TYPE( XPoleVectorConstraint );
  REGISTER_CHUNK_TYPE( XPolygonMesh );
  REGISTER_CHUNK_TYPE( XRampShader );
  REGISTER_CHUNK_TYPE( XRadial );
  REGISTER_CHUNK_TYPE( XRenderLayer );
  REGISTER_CHUNK_TYPE( XScaleConstraint );
  REGISTER_CHUNK_TYPE( XSculptDeformer );
  REGISTER_CHUNK_TYPE( XSkinCluster );
  REGISTER_CHUNK_TYPE( XSpecular );
  REGISTER_CHUNK_TYPE( XSphereMap );
  REGISTER_CHUNK_TYPE( XSpotLight );
  REGISTER_CHUNK_TYPE( XSurfaceShader );
  REGISTER_CHUNK_TYPE( XTangentConstraint );
  REGISTER_CHUNK_TYPE( XTexPlacement2D );
  REGISTER_CHUNK_TYPE( XTexPlacement3D );
  REGISTER_CHUNK_TYPE( XTurbulence );
  REGISTER_CHUNK_TYPE( XUniform );
  REGISTER_CHUNK_TYPE( XUseBackground );
  REGISTER_CHUNK_TYPE( XVolumeAxis );
  REGISTER_CHUNK_TYPE( XVolumeBox );
  REGISTER_CHUNK_TYPE( XVolumeCone );
  REGISTER_CHUNK_TYPE( XVolumeCylinder );
  REGISTER_CHUNK_TYPE( XVolumeSphere );
  REGISTER_CHUNK_TYPE( XVolumeLight );
  REGISTER_CHUNK_TYPE( XVortex );
  REGISTER_CHUNK_TYPE( XWireDeformer );
  REGISTER_CHUNK_TYPE( XWrapDeformer );

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
XMD_EXPORT bool XMDCleanup()
{
  DUMPER(XMDCleanup);
  XGlobal::Cleanup();
  // would like to call this here, but might be a better idea to use an allocator for XMD.
#if 0
  nmtl::NMTLDeleteDefaultAllocator();
#endif
  return true;
}

bool operator==(const XFn::Type& t,const XU16& et)
{
  return et == (XU16)(t);
}
bool operator==(const XU16& et,const XFn::Type& t)
{
  return et == (XU16)(t);
}
bool operator!=(const XFn::Type& t,const XU16& et)
{
  return et != (XU16)(t);
}
bool operator!=(const XU16& et,const XFn::Type& t)
{
  return et != (XU16)(t);
}
bool operator<(const XFn::Type& t,const XU16& et)
{
  return et < (XU16)(t);
}
bool operator<(const XU16& et,const XFn::Type& t)
{
  return et < (XU16)(t);
}
bool operator>(const XFn::Type& t,const XU16& et)
{
  return et > (XU16)(t);
}
bool operator>(const XU16& et,const XFn::Type& t)
{
  return et > (XU16)(t);
}

//----------------------------------------------------------------------------------------------------------------------
XModel::XModel()
  : m_InputVersion(XMD_VERSION_NUMBER),
    m_pFileInfo(0),
    m_Images(),
    m_Instances(),
    m_AnimCycles(),
    m_Nodes(),
    m_NodeLookup(),
    m_InstanceIDGen(0),
    m_Inherited(),
    m_NewNodes(),
    m_IdGen(0),
    m_ClonePolicy(),
    m_CreatePolicy(),
    m_DeletePolicy(),
    m_ExportPolicy(),
    m_ImportPolicy()
{
  DUMPER(XModel__ctor);
}

//----------------------------------------------------------------------------------------------------------------------
XClonePolicy& XModel::GetClonePolicy()
{
  return m_ClonePolicy;
}

//----------------------------------------------------------------------------------------------------------------------
const XClonePolicy& XModel::GetClonePolicy() const
{
  return m_ClonePolicy;
}

//----------------------------------------------------------------------------------------------------------------------
XExportPolicy& XModel::GetExportPolicy()
{
  return m_ExportPolicy;
}

//----------------------------------------------------------------------------------------------------------------------
const XExportPolicy& XModel::GetExportPolicy() const
{
  return m_ExportPolicy;
}

//----------------------------------------------------------------------------------------------------------------------
XImportPolicy& XModel::GetImportPolicy()
{
  return m_ImportPolicy;
}

//----------------------------------------------------------------------------------------------------------------------
const XImportPolicy& XModel::GetImportPolicy() const
{
  return m_ImportPolicy;
}

//----------------------------------------------------------------------------------------------------------------------
XCreatePolicy& XModel::GetCreatePolicy()
{
  return m_CreatePolicy;
}

//----------------------------------------------------------------------------------------------------------------------
const XCreatePolicy& XModel::GetCreatePolicy() const
{
  return m_CreatePolicy;
}

//----------------------------------------------------------------------------------------------------------------------
XDeletePolicy& XModel::GetDeletePolicy()
{
  return m_DeletePolicy;
}

//----------------------------------------------------------------------------------------------------------------------
const XDeletePolicy& XModel::GetDeletePolicy() const
{
  return m_DeletePolicy;
}

//----------------------------------------------------------------------------------------------------------------------
void XModel::Release()
{
  DUMPER(XModel__Release);
  {
    XInstanceList::iterator it = m_Instances.begin();
    for( ; it != m_Instances.end(); ++it )
    {
      delete *it;
    }
  }
  {
    XMap::iterator it = m_Nodes.begin();
    for( ; it != m_Nodes.end(); ++it )
    {
      delete it->second;
    }
  }
  {
    /// all of the images loaded in the scene
    XImageList::iterator it = m_Images.begin();
    for( ; it != m_Images.end(); ++it )
    {
      delete *it;
    }
  }
  {
    /// all of the images loaded in the scene
    XAnimList::iterator it = m_AnimCycles.begin();
    for( ; it != m_AnimCycles.end(); ++it )
    {
      delete *it;
    }
  }
  {
    XInheritedNodeList::iterator it = m_Inherited.begin();
    for( ; it != m_Inherited.end(); ++it )
    {
      delete *it;
    }
  }
  {
    XNewNodeInfoList::iterator it = m_NewNodes.begin();
    for (;it != m_NewNodes.end();++it)
    {
      delete it->second;
    }
  }

  m_Instances.clear();
  m_Images.clear();
  m_Nodes.clear();
  m_AnimCycles.clear();
  m_Inherited.clear();
  m_NodeLookup.clear();
  m_NewNodes.clear();
  delete m_pFileInfo;
  m_pFileInfo=0;
  m_InstanceIDGen=0;
  m_IdGen=0;
}

//----------------------------------------------------------------------------------------------------------------------
XModel::~XModel(void)
{
  DUMPER(XModel_dtor);
  Release();
}

//----------------------------------------------------------------------------------------------------------------------
XBase* XModel::CreateNode(const XFn::Type type)
{
  DUMPER(XModel_CreateNode);
  XBase* ptr = XFactory::CreateNode(this,type);
  if (!ptr)
  {
    XGlobal::GetLogger()->Logf(XBasicLogger::kError,
      "Could not create type %s. This is either because the type is abstract, or invalid...",
       XGlobal::ApiTypeStrFromNodeType(type).c_str());
    return 0;
  }
  ptr->m_ThisID = ++m_IdGen;

  XString name;
  XS32 i=1;
  bool done =false;
  while(!done)
  {
    std::stringstream buffer;
    buffer << "node" << i;
    std::string temp_name = buffer.str();
    if (!FindNode(temp_name.c_str()))
    {
      name = temp_name;
      break;
    }
    ++i;
  }

  if (ptr->HasFn<XAnimCycle>())
  {
    m_AnimCycles.push_back(ptr->HasFn<XAnimCycle>());
  }
  else
  {
    ptr->m_Name = (name.c_str());
    m_Nodes.insert( std::make_pair( name, ptr ) );
    m_NodeLookup.insert( std::make_pair( m_IdGen, ptr ) );
  }

  return ptr;
}

//----------------------------------------------------------------------------------------------------------------------
XInstance* XModel::CreateGeometryInstance(XBone* bone,XShape* object)
{
  DUMPER(XModel_CreateGeometryInstance);

  // first run a check to make sure instance is not already created...
  XInstanceList instances;
  bone->GetObjectInstances(instances);
  for (XInstanceList::iterator it = instances.begin(); it != instances.end(); ++it)
  {
    XInstance* inst = *it;
    if (inst->GetShape() == object)
    {
      return inst;
    }
  }

  // create the instance
  XInstance* ptr = new XInstance(this,object,bone);
  ptr->m_InstanceID = ++m_InstanceIDGen;
  ptr->m_Name=object->GetName();
  m_Instances.push_back(ptr);

  // now add to the bone.
  bone->m_Instances.push_back(ptr->GetID());
  object->m_Instances.push_back(ptr->GetID());

  return ptr;
}

//----------------------------------------------------------------------------------------------------------------------
bool XModel::DeleteAllOfType(XFn::Type type)
{
  DUMPER(XModel__DeleteAllOfType);
  XList ls;

  // get list of all objects of the specified type
  List(ls,type);

  // iterate over and delete all of them....
  XList::iterator it = ls.begin();
  for( ; it != ls.end(); ++it )
  {
    if(*it && IsValidNode(*it) )
      DeleteNode(*it);
  }
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
void XModel::DoFinalDeleteNode(XBase* obj)
{
  DUMPER(XModel__DoFinalDeleteNode);

  // obj is NULL, or not from this XModel
  XMD_ASSERT( obj && IsValidNode(obj) );

  XString obj_name = obj->GetName();
  DPARAM( obj_name );
  XId obj_id = obj->GetID();
  DPARAM( obj_id );
  XMap2::iterator itl = m_NodeLookup.find(obj_id);

  if( itl != m_NodeLookup.end() )
  {
    m_NodeLookup.erase(itl);

    XMap::iterator it = m_Nodes.begin();
    for( ; it != m_Nodes.end(); ++it )
    {
      if( it->second == obj )
      {
        delete it->second;
        m_Nodes.erase(it);
        break;
      }
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
bool XModel::DeleteNode(XBase* obj)
{
  DUMPER(XModel__DeleteNode);

  // if you assert here, obj is NULL, or not from this XModel
  XMD_ASSERT( obj && IsValidNode(obj) );
  if(obj)
  {
    XString obj_name = obj->GetName();
    DPARAM( obj_name );
    XId obj_id = obj->GetID();
    DPARAM( obj_id );
    (void)obj_id;

    // determine the list of nodes to remove as a result of deleting this one
    XIdSet to_remove;
    DoDeleteNode(obj,to_remove);
    DoFinalDeleteNode(obj);

    // iterate over all node ID's to delete
    XIdSet::iterator itr = to_remove.begin();
    for( ; itr != to_remove.end(); ++itr )
    {
      if (IsValidNode(*itr))
      {
        DeleteNode( *itr );
      }
    }
    return true;
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool XModel::DeleteNode(const XId obj)
{
  DUMPER(XModel__DeleteNode);
  // if you assert here, obj is NULL, or not from this XModel
  XMD_ASSERT( obj && IsValidNode(obj) );
  XBase* pb = FindNode(obj);
  if(!pb)
    return true;
  return DeleteNode(pb);
}

//----------------------------------------------------------------------------------------------------------------------
bool XModel::DeleteInstance(const XId instance)
{
  DUMPER(XModel__DeleteInstance);
  XInstanceList::iterator it = m_Instances.begin();
  for( ; it != m_Instances.end(); ++it )
  {
    if ((*it)->GetID()==instance)
    {
      XBone* pBone = (*it)->GetParent();
      if (pBone)
      {
        pBone->DeleteInstance(instance);
      }
      delete *it;
      m_Instances.erase(it);
      return true;
    }
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool XModel::DoDeleteNode(XBase* obj,XIdSet& to_remove)
{
  DUMPER(XModel__DoDeleteNode);
  // if you assert here, obj is NULL, or not from this XModel
  XMD_ASSERT( obj && IsValidNode(obj) );
  XId id1 = obj->GetID();
  XString obj_name = obj->GetName();
  DPARAM( obj_name );
  XId obj_id = obj->GetID();
  DPARAM( obj_id );
  (void)obj_id;

  // notify the object of it's demise
  obj->PreDelete(to_remove);

//  to_remove.insert(id1);

  XMap::iterator it = m_Nodes.begin();
  for( ; it != m_Nodes.end(); ++it )
  {
    XId id2 = it->second->GetID();
    if( id1 != id2 )
    {
      it->second->NodeDeath(id1);
    }
  }
  XAnimList::iterator its = m_AnimCycles.begin();
  for( ; its != m_AnimCycles.end(); ++its )
  {
    (*its)->NodeDeath(id1);
  }
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
void XModel::EvaluateAt(XBase* anim,XReal sec)
{
  DUMPER(XModel__EvaluateAt);
  // if you assert here, you are attempting to evaluate an anim cycle that is either NULL,
  // or from another XModel.
  XMD_ASSERT( anim && IsValidNode(anim) );
  XAnimCycle* pc = anim->HasFn<XAnimCycle>();
  if(!pc)
    return;

  XList bones;
  List(bones,XFn::Bone);

  XList::iterator it = bones.begin();
  for( ; it != bones.end(); ++it )
  {
    XBone* b = (*it)->HasFn<XBone>();
    if( b )
    {
      if(!pc->EvaluateAt(b->GetID(),b->m_Transform,sec ) )
        b->m_Transform = b->m_BindTransform;
    }
  }
  BuildMatrices();
}

void XModel::BuildMatrices()
{
  XBoneList rbones;
  GetRootBones(rbones);
  {
    XBoneList::iterator it = rbones.begin();
    XBoneList::iterator end = rbones.end();
    for(;it != end;++it)
    {
      (*it)->Evaluate(XMatrix());
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
XBase* XModel::FindNode(const XString& name)
{
  DUMPER(XModel__FindNode);
  XMap::iterator it = m_Nodes.find(name);

  if( it != m_Nodes.end() )
  {
    return it->second;
  }
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
XBase* XModel::FindNode(const XId id_)
{
  DUMPER(XModel__FindNode);
  XMap2::iterator it = m_NodeLookup.find(id_);

      if( it != m_NodeLookup.end() )
  {
    return it->second;
  }
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
const XBase* XModel::FindNode(const XString& name) const
{
  DUMPER(XModel__FindNode);
  XMap::const_iterator it = m_Nodes.find(name);

  if( it != m_Nodes.end() )
  {
    return it->second;
  }
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
XList XModel::FindNodes(const XString& name) const
{
  XList items;
  XMap::const_iterator it = m_Nodes.find(name);
  XMap::const_iterator last = m_Nodes.upper_bound(name);

  for(;it!=last;++it)
  {
    items.push_back((XBase*)it->second);
  }
  return items;
}

//----------------------------------------------------------------------------------------------------------------------
const XBase* XModel::FindNode(const XId id_) const
{
  DUMPER(XModel__FindNode);
  XMap2::const_iterator it = m_NodeLookup.find(id_);

  if( it != m_NodeLookup.end() )
  {
    return it->second;
  }
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
XBone* XModel::GuessBone(const XString& name) const
{

  XBoneList bones;
  GetBones(bones);
  XBoneList::iterator it = bones.begin();
  XString name2=name;

  while (name2.size() && (*name2.begin()!=':') )
  {
    name2.erase(name2.begin());
  }

  // first check the name only
  for (; it != bones.end(); ++it )
  {
    if(name==(*it)->GetName())
      return *it;
  }

  // if name2 has something interesting in it
  if(name2.size())
  {
    // search for the smaller (-namespace) name
    it = bones.begin();
    for (; it != bones.end(); ++it )
    {
      if(name2==(*it)->GetName())
        return *it;
    }

    // ok, so now see if the bone names-namespace match any bone names
    it = bones.begin();
    for (; it != bones.end(); ++it )
    {
      XString name3=(*it)->GetName();
      while (name3.size() && (*name3.begin()!=':') )
      {
        name3.erase(name3.begin());
      }
      if(name3==name)
        return *it;
      if(name3==name2)
        return *it;
    }
  }
  else
  {
    it = bones.begin();
    for (; it != bones.end(); ++it )
    {
      XString name3=(*it)->GetName();
      while (name3.size() && (*name3.begin()!=':') )
      {
        name3.erase(name3.begin());
      }
      if ((!name3.empty()) && (*name3.begin()==':'))
      {
        name3.erase(name3.begin());
      }
      if(name3==name)
        return *it;
    }
  }
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
bool XModel::FreezeAllTransforms()
{
  DUMPER(XModel__FreezeAllTransforms);

  //
  // first go through all anim cycles, and for each key frame
  // freeze the anim cycle position and save the values into the
  // cycles as we go. Having done that, we then need to simply modify
  // the bind pose, and all geometry objects so that their matrices
  // become correct.
  //

  XBoneList roots;
  GetRootBones(roots);

  // in theory, this should remove all scale & orients from the animation cycle data.
  for (XU32 i=0; i!= NumAnimCycles(); ++i)
  {
    XAnimCycle* cycle = GetAnimCycle(i);
    XMD_ASSERT(cycle);

    for (XU32 j=0;j!=cycle->GetNumFrames();++j)
    {
      cycle->SetToFrame(j);
      for (XBoneList::iterator it = roots.begin(); it != roots.end(); ++it)
      {
        (*it)->FreezeTransforms();
        (*it)->KillScale();
      }
      cycle->UpdateFrame(j);
    }
  }

  // Now set the model back into the bind pose.
  SetToBindPose();

  // Now we can Modify all of the points in a geometry object so that
  // it can happily Do sensible things.
  XList geoms;
  List(geoms,XFn::Geometry);

  XList::iterator it = geoms.begin();
  for (;it != geoms.end(); ++it )
  {
    XGeometry* geom = (*it)->HasFn<XGeometry>();
    if (geom)
    {
      XInstanceList instances;
      geom->GetObjectInstances(instances);
      if (!instances.size())
        continue;

      // need to determine if the geometry object has both a negative and positive
      // scale.
      // If only a positive scale exists, we'll just modify the geom by the world TM
      // If only a negative scale exists, we'll just modify the geom by the world TM and invert it's x coord
      // If both exist, then we have to create a duplicate of the geometry object, and do both of the above to each,
      // geometry.
      //
      bool has_negx=false;
      bool has_posx=false;
      for (unsigned i=0; i!=instances.size();++i)
      {
        XBone* bone = instances[i]->GetParent();
        if (bone)
        {
          if(bone->GetAttribute("must_scale_geom_by_minus_1_in_x"))
            has_negx=true;
          else
            has_posx=true;
        }
      }

      /// \todo also need to check to see if the scales are actually different on the transforms, since that
      ///       would potentially require more than one modifcation to the geometry.
      XInstance* inst = instances[0];

      XMatrix mWorld = inst->GetParent()->GetWorld(false);
      XVector3 scale;
      mWorld.getScale(scale);

      if (has_negx&&has_posx)
      {
        XGeometry* cloned = geom->Clone()->HasFn<XGeometry>();

        XVector3Array points;
        geom->GetPoints(points);

        XVector3Array::iterator itp = points.begin();
        for (;itp != points.end(); ++itp)
        {
          itp->x *= scale.x;
          itp->y *= scale.y;
          itp->z *= scale.z;
        }
        geom->SetPoints(points);

        points.clear();
        cloned->GetPoints(points);

        itp = points.begin();
        for (;itp != points.end(); ++itp)
        {
          itp->x *= -scale.x;
          itp->y *= scale.y;
          itp->z *= scale.z;
        }
        cloned->SetPoints(points);

        // update the mesh normals if needed.
        XMesh* mesh = cloned->HasFn<XMesh>();
        if (mesh)
        {
          mesh->CalculateNormals(true);
          mesh->ReversePolyWindings();
        }

        XPolygonMesh* pmesh = cloned->HasFn<XPolygonMesh>();
        if (pmesh)
        {
          pmesh->CalculateNormals();
          pmesh->ReversePolyWindings();
        }

        mesh = geom->HasFn<XMesh>();
        if (mesh)
        {
          mesh->CalculateNormals(true);
        }

        pmesh = geom->HasFn<XPolygonMesh>();
        if (pmesh)
        {
          pmesh->CalculateNormals();
        }

        XIndexList ids;

        // now modify the instances that are negatively scaled
        for (unsigned i=0; i!=instances.size();++i)
        {
          XBone* bone = instances[i]->GetParent();
          if (bone)
          {
            if(bone->GetAttribute("must_scale_geom_by_minus_1_in_x"))
            {
              instances[i]->SetShape(cloned);

              // add instance into cloned geom
              cloned->m_Instances.push_back(instances[i]->GetID());

              // remove from old geom
              geom->m_Instances.erase(nmtl::find(geom->m_Instances.begin(),geom->m_Instances.end(), instances[i]->GetID() ));
            }
          }
        }
      }
      else
      if (has_posx)
      {
        XVector3Array points;
        geom->GetPoints(points);

        XVector3Array::iterator itp = points.begin();
        for (;itp != points.end(); ++itp)
        {
          itp->x *= scale.x;
          itp->y *= scale.y;
          itp->z *= scale.z;
        }
        geom->SetPoints(points);

        XMesh* mesh = geom->HasFn<XMesh>();
        if (mesh)
        {
          mesh->CalculateNormals(true);
        }

        XPolygonMesh* pmesh = geom->HasFn<XPolygonMesh>();
        if (pmesh)
        {
          pmesh->CalculateNormals();
        }
      }
      else
      if (has_negx)
      {
        XVector3Array points;
        geom->GetPoints(points);

        XVector3Array::iterator itp = points.begin();
        for (;itp != points.end(); ++itp)
        {
          itp->x *= -scale.x;
          itp->y *= scale.y;
          itp->z *= scale.z;
        }
        geom->SetPoints(points);

        XMesh* mesh = geom->HasFn<XMesh>();
        if (mesh)
        {
          mesh->ReversePolyWindings();
          mesh->CalculateNormals(true);
        }

        XPolygonMesh* pmesh = geom->HasFn<XPolygonMesh>();
        if (pmesh)
        {
          pmesh->ReversePolyWindings();
          pmesh->CalculateNormals();
        }
      }
    }
  }

  {
    // as a final step, freeze the bind pose
    for (XBoneList::iterator itb = roots.begin(); itb != roots.end(); ++itb)
    {
      (*itb)->FreezeTransforms();
    }
  }

  // and now copy the current pose over the bind pose info
  {
    XBoneList bones;
    GetBones(bones);
    // as a final step, freeze the bind pose
    for (XBoneList::iterator itb = bones.begin(); itb != bones.end(); ++itb)
    {
      (*itb)->KillScale();
      (*itb)->KillOrients();
      (*itb)->UpdateBindPose();
    }
  }
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
XAnimCycle* XModel::GetAnimCycle(const XU32 idx)
{
  DUMPER(XModel__GetAnimCycle);
  if(idx < NumAnimCycles() )
  {
    return m_AnimCycles[idx];
  }
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
const XAnimCycle* XModel::GetAnimCycle(const XU32 idx) const
{
  DUMPER(XModel__GetAnimCycle);
  if(idx < NumAnimCycles() )
  {
    return m_AnimCycles[idx];
  }
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
bool XModel::GetBones(XBoneList& bonelist)
{
  DUMPER(XModel__GetBones);
  XList bones;
  List(bones,XFn::Bone);
  bonelist.resize(bones.size());

  XList::iterator it = bones.begin();
  XBoneList::iterator  itb = bonelist.begin();

  for(; it != bones.end(); ++it, ++itb )
  {
    *itb = (*it)->HasFn<XBone>();
  }
  return bonelist.size() != 0;
}

//----------------------------------------------------------------------------------------------------------------------
bool XModel::GetBones(XBoneList& bonelist) const
{
  DUMPER(XModel__GetBones);
  XList bones;
  List(bones,XFn::Bone);
  bonelist.resize(bones.size());

  XList::iterator it = bones.begin();
  XBoneList::iterator  itb = bonelist.begin();

  for(; it != bones.end(); ++it, ++itb )
  {
    *itb = (*it)->HasFn<XBone>();
  }
  return bonelist.size() != 0;
}

//----------------------------------------------------------------------------------------------------------------------
bool XModel::GetRootBones(XBoneList& bonelist)
{
  DUMPER(XModel__GetBones);
  XList bones;
  List(bones,XFn::Bone);
  for(XU32 i=0;i!=bones.size();++i)
  {
    XBone* pbone = bones[i]->HasFn<XBone>();
    if(pbone && !pbone->GetParent())
      bonelist.push_back( pbone );
  }
  return bonelist.size() != 0;
}

//----------------------------------------------------------------------------------------------------------------------
bool XModel::GetRootBones(XBoneList& bonelist) const
{
  DUMPER(XModel__GetBones);
  XList bones;
  List(bones,XFn::Bone);
  for(XU32 i=0;i!=bones.size();++i)
  {
    XBone* pbone = bones[i]->HasFn<XBone>();
    if(pbone && !pbone->GetParent())
      bonelist.push_back( pbone );
  }
  return bonelist.size() != 0;
}

//----------------------------------------------------------------------------------------------------------------------
bool XModel::GetDeformers(XDeformerList& deflist) const
{
  DUMPER(XModel__GetDeformers);
  XList deformers;
  List(deformers,XFn::Deformer);
  deflist.resize(deformers.size());

  XList::iterator it = deformers.begin();
  XDeformerList::iterator  itb = deflist.begin();

  for(; it != deformers.end(); ++it, ++itb )
  {
    *itb = (*it)->HasFn<XDeformer>();
  }
  return deflist.size() != 0;
}

//----------------------------------------------------------------------------------------------------------------------
bool XModel::GetDeformers(XDeformerList& deflist,const XFn::Type type) const
{
  DUMPER(XModel__GetDeformers);
  XList deformers;
  List(deformers,type);
  deflist.resize(deformers.size());

  XList::iterator it = deformers.begin();
  XDeformerList::iterator  itb = deflist.begin();

  for(; it != deformers.end(); ++it, ++itb )
  {
    *itb = (*it)->HasFn<XDeformer>();
  }
  return deflist.size() != 0;
}

//----------------------------------------------------------------------------------------------------------------------
XImage* XModel::GetImage(XId index)
{
  DUMPER(XModel__GetImage);
  if(index<m_Images.size())
    return m_Images[index];
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
const XInfo* XModel::GetInfo() const
{
  DUMPER(XModel__GetInfo);
  return m_pFileInfo;
}

//----------------------------------------------------------------------------------------------------------------------
XInfo* XModel::GetInfo()
{
  DUMPER(XModel__GetInfo);
  return m_pFileInfo;
}

//----------------------------------------------------------------------------------------------------------------------
XInfo* XModel::CreateInfo()
{
  DUMPER(XModel__CreateInfo);
  if(m_pFileInfo)
    delete m_pFileInfo;
  m_pFileInfo = new XInfo;
  return m_pFileInfo;
}

//----------------------------------------------------------------------------------------------------------------------
XU32 XModel::GetNumImages()
{
  DUMPER(XModel__GetNumImages);
  return static_cast<XU32>(m_Images.size());
}

//----------------------------------------------------------------------------------------------------------------------
XInstance* XModel::GetInstance(const XId instance)
{
  DUMPER(XModel__GetInstance);
  XInstanceList::iterator it = m_Instances.begin();
  for( ; it != m_Instances.end(); ++it )
  {
    if ((*it)->GetID()==instance)
    {
      return *it;
    }
  }
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
const XInstance* XModel::GetInstance(const XId instance) const
{
  DUMPER(XModel__GetInstance);
  XInstanceList::const_iterator it = m_Instances.begin();
  for( ; it != m_Instances.end(); ++it )
  {
    if ((*it)->GetID()==instance)
    {
      return *it;
    }
  }
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
bool XModel::GetObjectInstances(XInstanceList& l)
{
  DUMPER(XModel__GetObjectInstances);
  l = m_Instances;
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool XModel::GetObjectInstances(XInstanceList& l,const XFn::Type type)
{
  DUMPER(XModel__GetObjectInstances);
  XInstanceList::iterator it = m_Instances.begin();
  for (;it != m_Instances.end();++it)
  {
    XInstance* inst = *it;
    XBase* b = inst->GetShape();
    if (b->HasFn(type))
    {
      l.push_back(inst);
    }
  }
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
/// \todo impliment me
bool XModel::GetUsedTextures(XTextureList& tl)
{
  DUMPER(XModel__GetUsedTextures);

  (void)tl;
#if 0
  XMaterialList ml;
  GetUsedMaterials(ml);

  XMaterialList::iterator it = ml.begin();
  for( ; it != ml.end(); ++it )
  {
    XEnvmapBaseTexture* pe = (*it)->GetEnvMap();
    XBumpTexture* pb = (*it)->GetBumpMap();
  /*  XTexture* pa = (*it)->GetTexture(XMaterial::AMBIENT_MAP);
    XTexture* pd = (*it)->GetTexture(XMaterial::DIFFUSE_MAP);
    XTexture* ps = (*it)->GetTexture(XMaterial::SPECULAR_MAP);
    XTexture* pi = (*it)->GetTexture(XMaterial::EMISSION_MAP);
    XTexture* pt = (*it)->GetTexture(XMaterial::TRANSPARENCY_MAP);*/

    if(pe) tl.push_back(pe);
    if(pb) tl.push_back(pb);
  /*  if(pa) tl.push_back(pa);
    if(pd) tl.push_back(pd);
    if(ps) tl.push_back(ps);
    if(pi) tl.push_back(pi);
    if(pt) tl.push_back(pt);*/
  }
#endif

  return true;
}


//----------------------------------------------------------------------------------------------------------------------
bool IsInList(XMaterialList& ml,XMaterial* m)
{
  for(XU32 i=0;i!=ml.size();++i)
  {
    if( ml[i] == m )
      return true;
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool XModel::GetUsedMaterials(XMaterialList& ml)
{
  DUMPER(XModel__GetUsedMaterials);
  XInstanceList il;
  GetObjectInstances(il);
  XInstanceList::iterator it = il.begin();
  for( ; it != il.end(); ++it )
  {
    XGeometry* pg = (*it)->GetGeometry();
    XBone* pb = (*it)->GetParent();
    if(pg && pb && (*it)->HasMaterial())
    {
      if( (*it)->HasMaterialGroups() )
      {
        for(XU32 i=0;i!=(*it)->GetNumMaterialGroups();++i)
        {
          XMaterial* pm = (*it)->GetMaterial(i);
          if(pm && !IsInList(ml,pm))
            ml.push_back(pm);
        }
      }
      else
      {
        XMaterial* pm = (*it)->GetMaterial();
        if(pm && !IsInList(ml,pm))
          ml.push_back(pm);
      }
    }
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool XModel::IsValidNode(const XString& name) const
{
  DUMPER(XModel__IsValidNode);
  return FindNode(name)!=0;
}

//----------------------------------------------------------------------------------------------------------------------
bool XModel::IsValidNode(const XId node) const
{
  DUMPER(XModel__IsValidNode);
  return FindNode(node)!=0;
}

//----------------------------------------------------------------------------------------------------------------------
bool XModel::IsValidNode(const XBase* node) const
{
  DUMPER(XModel__IsValidNode);
  XMap::const_iterator it = m_Nodes.begin();

  for( ; it != m_Nodes.end(); ++it )
  {
    if( it->second == node )
      return true;
  }
  if( ((XBase*)node)->HasFn<XAnimCycle>())
  {
    XAnimList::const_iterator ita = m_AnimCycles.begin();

    for( ; ita != m_AnimCycles.end(); ++ita )
    {
      if( *ita == node )
        return true;
    }
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool XModel::List(XList& items,const XFn::Type type, bool exactType)
{
  DUMPER(XModel__List);
  items.clear();
  XMap::iterator it = m_Nodes.begin();

  for( ; it != m_Nodes.end(); ++it )
  {
    if( it->second->HasFn(type) )
    {
      bool shouldAddToItems = exactType ? it->second->GetApiType() == type : true;
      if(shouldAddToItems)
      {
        items.push_back(it->second);
      }
    }
  }
  return items.size()!=0;
}

//----------------------------------------------------------------------------------------------------------------------
bool XModel::List(XList& items,const XStringList& must_have_attrs)
{
  DUMPER(XModel__List);
  // iterate over all nodes
  XMap::iterator it = m_Nodes.begin();
  for( ; it != m_Nodes.end(); ++it )
  {
    XBase* node = it->second;

    // check to see if node has all requiested attrs
    XStringList::const_iterator its =must_have_attrs.begin();
    for( ; its != must_have_attrs.end(); ++its )
    {
      if(!node->GetAttribute(its->c_str()))
        goto failed;
    }

    // node had all attrs
    items.push_back(node);

    failed: ;
  }

  return items.size() != 0;
}

//----------------------------------------------------------------------------------------------------------------------
bool XModel::List(XList& items,const XFn::Type type,const XStringList& must_have_attrs, bool exactType)
{
  DUMPER(XModel__List);
  // get the list of items of this type
  XList all_items;
  List(all_items,type);

  // iterate over list
  XList::iterator it = all_items.begin();
  for( ; it != all_items.end(); ++it )
  {
    XBase* node = *it;

    bool shouldAddToItems=false;

    // check to see if node has all requiested attrs
    XStringList::const_iterator its = must_have_attrs.begin();
    for( ; its != must_have_attrs.end(); ++its )
    {
      if(!node->GetAttribute(its->c_str()))
        goto failed;
    }

    // node had all attrs
    shouldAddToItems = exactType ? node->GetApiType() == type : true;
    if(shouldAddToItems)
    {
      items.push_back(node);
    }
    failed: ;
  }
  return items.size() != 0;
}

//----------------------------------------------------------------------------------------------------------------------
bool XModel::List(XList& items,const XFn::Type type, bool exactType) const
{
  DUMPER(XModel__List);
  items.clear();
  XMap::const_iterator it = m_Nodes.begin();

  for( ; it != m_Nodes.end(); ++it )
  {
    if( it->second->HasFn(type) )
    {
      bool shouldAddToItems = exactType ? it->second->GetApiType() == type : true;
      if(shouldAddToItems)
      {
        items.push_back((XBase*)it->second);
      }
    }
  }
  return items.size()!=0;
}

//----------------------------------------------------------------------------------------------------------------------
bool XModel::List(XList& items,const XStringList& must_have_attrs) const
{
  DUMPER(XModel__List);
  XList all_items;
  List(all_items,XFn::Base);

  // iterate over all nodes
  XList::iterator it = all_items.begin();
  for( ; it != all_items.end(); ++it )
  {
    const XBase* node = *it;

    // check to see if node has all requiested attrs
    XStringList::const_iterator its =must_have_attrs.begin();
    for( ; its != must_have_attrs.end(); ++its )
    {
      if(!node->GetAttribute(its->c_str()))
        goto failed;
    }

    // node had all attrs
    items.push_back((XBase*)node);

    failed: ;
  }

  return items.size() != 0;
}

//----------------------------------------------------------------------------------------------------------------------
bool XModel::List(XList& items,const XFn::Type type,const XStringList& must_have_attrs, bool exactType) const
{
  DUMPER(XModel__List);
  // get the list of items of this type
  XList all_items;
  List(all_items,type);

  // iterate over list
  XList::iterator it = all_items.begin();
  for( ; it != all_items.end(); ++it )
  {
    XBase* node = *it;

    bool shouldAddToItems = false;

    // check to see if node has all requiested attrs
    XStringList::const_iterator its = must_have_attrs.begin();
    for( ; its != must_have_attrs.end(); ++its )
    {
      if(!node->GetAttribute(its->c_str()))
        goto failed;
    }

    // node had all attrs
    shouldAddToItems = exactType ? node->GetApiType() == type : true;
    if(shouldAddToItems)
    {
      items.push_back((XBase*)node);
    }

    failed: ;
  }

  return items.size() != 0;
}

#define XIO_CHECK(X) if( !(X) ) return XFileError::FileCorrupt;

//----------------------------------------------------------------------------------------------------------------------
XFileError::eType XModel::LoadBin(const XString& filename,bool include_anim)
{
  DUMPER(XModel__LoadBin);
  XMU::FileInfo info( filename.c_str() );

  XU16 NumInheritedTypes=0;

  // a size check to ensure that we never go over the end of the file
  XS32 byte_size = info.GetSize();
  XFileIO io(filename.c_str());

  if(!io.Valid())
  {
    XGlobal::GetLogger()->Logf(XBasicLogger::kInfo, "file \"%s\" failed to open", filename.c_str());
    return XFileError::FailedToOpen;
  }

  XChar temp_buffer[24];
  io.DoData(temp_buffer,24);

  XFileError::eType ret = LoadTextures(io.GetPtr());
  if(ret != XFileError::Success)
  {
    XGlobal::GetLogger()->Log(XBasicLogger::kError, "could not load the textures");
  }
  byte_size -= io.CurrPos();

  while( byte_size > 0 )
  {
    XChunkHeader header;
    memset(&header,0,sizeof(XChunkHeader));

    XIO_CHECK( io.DoData( &header ) );
    if (!io.Valid() || header.Type==0/*XFn::Base*/)
    {
      break;
    }

    XString head_str = TypeToHeader( (XFn::Type)header.Type);

    if( (XFn::Type)header.Type == XFn::AnimCycle )
      head_str = "ANIM_CYCLE";
    else
    if( (XFn::Type)header.Type == XFn::Info )
      head_str = "INFO";
    else
    if( (XFn::Type)header.Type == XFn::InheritedFileFlag )
      head_str = "INHERITED";
    else
    if( (XFn::Type)header.Type == XFn::NewFileFlag )
      head_str = "NEW_NODE";

    // if recognised as type, read it in
    if(head_str!="")
    {
      if( (XFn::Type)header.Type == XFn::AnimCycle )
      {
        if(include_anim)
        {
          CHECK_RCHUNK_BEGIN( io );
          // create new anim cycle and read
          XAnimCycle* ptr = new XAnimCycle(this);
          if( ptr->DoData(io) )
            // add to cycle list
            m_AnimCycles.push_back( ptr );
          else
          {
            // oooh, dear. delete anim and skip to next chunk
            delete ptr;
            goto skippy_chunky;
          }
          CHECK_RCHUNK_END( io, (*ptr), header.ByteSize );
        }
        else
        {
          goto skippy_chunky;
        }
      }
      else
      if( (XFn::Type)header.Type == XFn::Info )
      {
        m_pFileInfo = new XInfo;
        if( !m_pFileInfo->DoData(io) )
        {
          delete m_pFileInfo;
          goto skippy_chunky;
        }
        else
        {
          if(! this->m_Images.size() &&
             m_pFileInfo->GetTextureFile().size() != 0 &&
             m_pFileInfo->GetTextureFile() != "none" )
          {
            if(!LoadTextures(m_pFileInfo->GetTextureFile().c_str()))
            {
              #ifndef WIN32
                // if failed first time, add '/' to the front for linux
                XString file__ = "/";
                file__ += m_pFileInfo->GetTextureFile().c_str();
                if(!LoadTextures(file__))
                {
                  XGlobal::GetLogger()->Log(XBasicLogger::kError,"could not find texture file\n");
                }
              #endif
            }
          }
        }
      }
      else
      if( (XFn::Type)header.Type == XFn::InheritedFileFlag )
      {

        XInheritedNode* info = new XInheritedNode(NumInheritedTypes++);
        if(!info->DoData(io))
        {
          return XFileError::FileCorrupt;
        }
        m_Inherited.push_back(info);
      }
      else
      if( (XFn::Type)header.Type == XFn::NewFileFlag )
      {
        PxNewNodeDescription* info = new PxNewNodeDescription;
        if(!info->DoData(io))
        {
          return XFileError::FileCorrupt;
        }
       // else
        //  goto skippy_chunky;
        m_NewNodes.insert( std::make_pair(info->GetChunkName(), info) );
      }
      else
      {
        CHECK_RCHUNK_BEGIN( io );
        XBase* pnew = XCreateNode(this,(XFn::Type)header.Type);
        if(pnew)
        {
          pnew->m_pAttributes = header.HasExtraAttrs ? new XExtraAttributes : 0;

          if( pnew->DoData(io) )
          {
            m_Nodes.insert( std::make_pair( pnew->GetName(), pnew ) );
            m_NodeLookup.insert( std::make_pair( pnew->GetID(), pnew ) );
          }
          else
          {
            delete pnew;
            goto skippy_chunky;
          }
        }
        else
        {
          XGlobal::GetLogger()->Log(XBasicLogger::kWarning, "XModel::LoadBin : unknown node type encountered");

          // chunk unknown
          goto skippy_chunky;
        }
        CHECK_RCHUNK_END( io, (*pnew), header.ByteSize );
      }
    }
    // else skip chunk
    else
    {
    skippy_chunky:

      if(header.ByteSize==0)
      {
        XGlobal::GetLogger()->Log(XBasicLogger::kError, "XModel::LoadBin : header byte size 0");
        return XFileError::FileCorrupt;
      }
      // skip remainder of chunk
      io.Skip( header.ByteSize - sizeof(XChunkHeader) );
    }

    byte_size -= header.ByteSize;
  }

  return XFileError::Success;
}

//----------------------------------------------------------------------------------------------------------------------
XFileError::eType XModel::Load(const XString& filename,bool include_anim)
{
  DUMPER(XModel__Load);
  Release();

  XFileError::eType status = XFileError::Success;
  XU32 file__name_len_ = static_cast<XU32>( filename.size() );
  XString EXT;
  EXT += (XChar)tolower(filename.c_str()[ file__name_len_ - 3 ] );
  EXT += (XChar)tolower(filename.c_str()[ file__name_len_ - 2 ] );
  EXT += (XChar)tolower(filename.c_str()[ file__name_len_ - 1 ] );

  XGlobal::GetLogger()->Logf(XBasicLogger::kInfo, "loading \"%s\"", filename.c_str());

  if (EXT=="xld"||EXT=="xmd")
  {
    // run quick test on ascii & make sure valid
    XChar header[25] = "|" XMD_VERSION_ASCII "#0000#00000000|";
    bool isAscii=true;

    {
      FILE* fp = fopen(filename.c_str(),"rb");
      if(!fp)
        return XFileError::FailedToOpen;
      XChar temp[25];
      fread(temp,25,1,fp);
      fclose(fp);

      if(strncmp(temp,header,5)==0)
      {
        m_InputVersion = (temp[5]-'0')*100 + (temp[6]-'0')*10 + (temp[7]-'0');
        if (m_InputVersion>XMD_VERSION_NUMBER)
        {
          XGlobal::GetLogger()->Logf(XBasicLogger::kError,
            "opening file: file version %d is newer than xmd version %d", m_InputVersion, XMD_VERSION_NUMBER);
          return XFileError::NewFormat;
        }
        else if(m_InputVersion<XMD_VERSION_NUMBER)
        {
          XGlobal::GetLogger()->Logf(XBasicLogger::kWarning,
            "File is from an older xmd version. If saved it will be unreadable by versions of xmd older than version %d", XMD_VERSION_NUMBER);
        }
        isAscii = (temp[8]=='A')?true:false;
      }
      else
      {
        // not an XMD file.
        XGlobal::GetLogger()->Logf(XBasicLogger::kError, "File is \"%s\" is not an xmd file", filename.c_str());
        return XFileError::InvalidFile;
      }
    }

    if(!isAscii)
    {
      status = LoadBin(filename,include_anim);
    }
    else
    {
      status = LoadAscii(filename,include_anim);
    }
    PostLoad();
  }
  else
  if( XGlobal::Import(filename.c_str(),*this) )
  {
    // this builds up any additional referenced nodes within the model
    PostLoad();
    status = XFileError::Success;
  }
  else
    status = XFileError::InvalidFile;


  // as a final step, make sure we update the node ID gen
  XList nodes;
  List(nodes,XFn::Base);
  m_IdGen=0;
  for (XList::iterator it = nodes.begin();it!=nodes.end();++it)
  {
    if (m_IdGen<(*it)->GetID())
    {
      m_IdGen = (*it)->GetID();
    }
  }

  return status;
}

//----------------------------------------------------------------------------------------------------------------------
XFileError::eType XModel::LoadAscii(const XString& filename,bool include_anim)
{
  // we want to make a local copy of the file content to prevent expensive block-reads from disk
  XU32 bufferSize = 0;
  char* bufferData = NULL;
  FILE *file = fopen(filename.c_str(), "rb");
  if(file != NULL) 
  {
    fseek(file, 0, SEEK_END);
    size_t fileSize = ftell(file);
    if(fileSize > 0)
    {
      bufferData = new char[fileSize];
      bufferSize = static_cast<XU32>(fileSize);
      fseek(file, 0, SEEK_SET);
      size_t bytesRead = fread(bufferData, 1, static_cast<size_t>(fileSize), file);
      bytesRead;
      assert(bytesRead == fileSize);
    }
    fclose(file);
  }

  if(bufferData == NULL)
  {
    XGlobal::GetLogger()->Logf(XBasicLogger::kInfo, "file \"%s\" failed to open", filename.c_str());
    return XFileError::FailedToOpen;
  }

  // now, construct a stringstream holding the data to get parsed inf LoadFromAsciiStream
  // this needs to be put in 4K chunks to prevent problems for larger files. This only
  // make sense because it is still faster than reading the data in small blocks from disk
  // (what would happen for filestream operations)
  std::stringstream iss;
  char* current = bufferData;
  while (bufferSize>0) 
  {
    size_t increment = (4096>bufferSize)?bufferSize:4096;
    iss<<std::string(current, increment);    
    current += increment;
    bufferSize -= static_cast<XU32>(increment);
  }
  delete[] bufferData;
  XFileError::eType error = LoadFromAsciiStream(iss,filename,include_anim);

  return error;
}

//----------------------------------------------------------------------------------------------------------------------
XFileError::eType XModel::LoadFromAsciiBuffer(const XChar* buffer,const XString& filename,bool include_anim)
{
  std::istringstream iss(buffer);
  return LoadFromAsciiStream(iss,filename,include_anim);
}

//----------------------------------------------------------------------------------------------------------------------
XFileError::eType XModel::LoadFromAsciiStream(std::istream& ifs,const XString& filename,bool include_anim)
{
  // variable used to kepp track of how many inherited node types we've actually loaded
  XU16 NumInheritedTypes=0;

  {
    XString temp;
    ifs >> temp;

    // run quick test on ascii & make sure valid
    XChar header[25] = "|" XMD_VERSION_ASCII "#0000#00000000|";

    if(strncmp(temp.c_str(),header,5)==0)
    {
      m_InputVersion = (temp[5]-'0')*100 + (temp[6]-'0')*10 + (temp[7]-'0');
      if (m_InputVersion>XMD_VERSION_NUMBER)
      {
        XGlobal::GetLogger()->Logf(XBasicLogger::kError,
          "opening file: file version %d is newer than xmd version %d", m_InputVersion, XMD_VERSION_NUMBER);
        return XFileError::NewFormat;
      }
      else if(m_InputVersion<XMD_VERSION_NUMBER)
      {
        XGlobal::GetLogger()->Logf(XBasicLogger::kWarning,
          "File is from an older xmd version. If saved it will be unreadable by versions of xmd older than version %d", XMD_VERSION_NUMBER);
      }

      // make sure is an ascii file
      if(temp[8]!='A')
      {
        XGlobal::GetLogger()->Logf(XBasicLogger::kError, "File is \"%s\" is not an ascii xmd file", filename.c_str());
        return XFileError::InvalidFile;
      }
    }
    else
    {
      // not an XMD file.
      XGlobal::GetLogger()->Logf(XBasicLogger::kError, "File is \"%s\" is not an xmd file", filename.c_str());
      return XFileError::InvalidFile;
    }
  }

  XString buffer;
  if( ifs.good() )
  {
    while( !ifs.eof() )
    {
      XString chunk_header,chunk_name;

      buffer="";

      // read the chunk header
      ifs >> buffer;
      if(ifs.eof())
        break;
      chunk_header = buffer;

      // read chunk name
      ifs >> buffer;
      if(ifs.eof())
        break;
      chunk_name = buffer;

      FormatName(chunk_name);

      XBase* pBase=0;

      XBase* pnode = 0;

      // debugging check - not currently needed
      #if 0
      pnode = FindNode(chunk_name);
      if(pnode)
      {
        XGlobal::GetLogger()->Logf(XBasicLogger::kWarning, "Node already exists \"%s\". Renaming", chunk_name);
      }
      #endif

      pnode = XGlobal::CreateFromChunkName(chunk_header.c_str(),this);
      if (include_anim &&
        chunk_header == "ANIM_CYCLE")
      {

        XAnimCycle* ptr = pnode->HasFn<XAnimCycle>();
        ifs >> buffer ;
        ptr->m_Name = chunk_name;
        if(ptr->ReadChunk(ifs))
          m_AnimCycles.push_back( ptr );
        else
        {
          delete ptr;
          return XFileError::FileCorrupt;
        }
        pBase = ptr;
      }
      else if(include_anim && chunk_header == "ANIMATION_TAKE")
      {
        pnode->m_Name = chunk_name;
        if(pnode->DoReadChunk(ifs))
        {
          m_Nodes.insert( std::make_pair( pnode->GetName(), pnode ) );
          m_NodeLookup.insert( std::make_pair( pnode->GetID(), pnode ) );
        }
        else
        {
          delete pnode;
          return XFileError::FileCorrupt;
        }
        pBase = pnode;
      }
      else
      if (pnode && chunk_header != "ANIM_CYCLE")
      {
        if(pnode->DoReadChunk(ifs))
        {
          m_Nodes.insert( std::make_pair( chunk_name, pnode ) );
          m_NodeLookup.insert( std::make_pair( pnode->GetID(), pnode ) );
        }
        else
        {
          delete pnode;
          return XFileError::FileCorrupt;
        }
        pBase=pnode;
      }
      else

      if (chunk_header == "INFO")
      {
        m_pFileInfo = new XInfo;
        if( !m_pFileInfo->Read(ifs) )
        {
          delete m_pFileInfo;
          m_pFileInfo=0;
          return XFileError::FileCorrupt;
        }
        else
        {
          if( m_pFileInfo->GetTextureFile() != "none" )
          {
            if(XFileError::Success != LoadTextures(m_pFileInfo->GetTextureFile().c_str()) && filename.size()>3)
            {
              XString path;

              XString orig_path = m_pFileInfo->GetTextureFile().c_str();
              path = filename;

              XS32 len =static_cast<XS32>(  path.size() );

              // under linux we can occasionally lose the '/' infront of full paths
              #ifndef WIN32
                XString file__ = "/";
                file__ += m_pFileInfo->GetTextureFile().c_str();
                if(LoadTextures(file__))
                {
                  goto sorted;
                }
              #endif

              //
              // try  filename, - "xmd", + "xtd"
              //
              // (note: only 'm' and 't' different between names)
              //
              path[len-2] = 't';

              if(XFileError::Success == LoadTextures(path.c_str()))
                goto sorted;


              //
              // try path, + texture fname
              //
              while (len > 0 && path[len - 1] != '\\' && path[len - 1] != '/')
              {
                --len;
              }
              if (len == 0)
              {
                goto try_file_only;
              }
              path.resize(len);

              len = static_cast<XS32>( orig_path.size() );

              while (len > 0 && orig_path[len - 1] != '\\' && orig_path[len - 1] != '/')
              {
                --len;
              }
              
              {
                XString::const_iterator its = orig_path.begin() + len;
                while (its != orig_path.end())
                {
                  path += *its;
                  ++its;
                }

                LoadTextures(path.c_str());
              }

              try_file_only:
              ;//LoadTextures(m_pFileInfo->GetTextureFile().c_str());

              sorted: ;
            }
          }
        }
      }
      else
      if (chunk_header == "NEW_NODE")
      {
        PxNewNodeDescription* ptr = new PxNewNodeDescription();
        if(ptr->Read(ifs))
          m_NewNodes.insert( std::make_pair( ptr->GetChunkName(), ptr ) );
        else
        {
          delete ptr;
          return XFileError::FileCorrupt;
        }
      }
      else
      if (chunk_header == "INHERITED_NODE")
      {
        XInheritedNode* info = new XInheritedNode(NumInheritedTypes++);
        if(!info->Read(ifs))
        {
          return XFileError::FileCorrupt;
        }
        m_Inherited.push_back(info);
      }
      else
      {
        // first check inherited nodes
        bool done = false;

        {
          XInheritedNodeList::iterator it = m_Inherited.begin();
          for( ; it != m_Inherited.end(); ++it )
          {
            if( (*it)->GetHeader() == chunk_header )
            {
              XBase* ptr = (*it)->Create(this);
              if(ptr->DoReadChunk(ifs))
              {
                m_Nodes.insert( std::make_pair( chunk_name, ptr ) );
                m_NodeLookup.insert( std::make_pair( ptr->GetID(), ptr ) );
              }
              else
              {
                delete ptr;
                return XFileError::FileCorrupt;
              }
              pBase = ptr;
              pBase->m_pInherited = *it;
              done = true;
              break;
            }
          }
        }

        // otherwise check any new nodes...
        if(!done)
        {
          XNewNodeInfoList::iterator it = m_NewNodes.find(chunk_header);
          if( it != m_NewNodes.end() )
          {
            XBase* ptr = it->second->Create(this);

            if(ptr->DoReadChunk(ifs))
            {
              m_Nodes.insert( std::make_pair( chunk_name, ptr ) );
              m_NodeLookup.insert( std::make_pair( ptr->GetID(), ptr ) );
            }
            else
            {
              delete ptr;
              return XFileError::FileCorrupt;
            }
            pBase = ptr;
            done = true;
          }
        }

        // dump it for we know not what it is....
        if(!done)
        {
          if(!ifs.eof())
          {
            bool safelyIgnored = !include_anim &&
              (chunk_header == "ANIM_CYCLE" ||
              chunk_header == "ANIMATION_TAKE");

            if (!safelyIgnored)
            {
              XGlobal::GetLogger()->Logf(XBasicLogger::kWarning,
                "Dumping Unknown Chunk: header: %s name: %s",
                  chunk_header.c_str(), chunk_name.c_str());
            }
          }

          // make sure we have read the first bracket.
          ifs >> buffer;
          if( buffer != "{" )
          {
            XGlobal::GetLogger()->Log(XBasicLogger::kError,"Missing Bracket '{' after chunk name");
            return XFileError::FileCorrupt;
          }

          // either we have an inherited or new node, or this is something
          // we don't understand so dump the chunk.
          XS32 bc = 1;
          while(bc && !ifs.eof())
          {
            ifs >> buffer;
            if(buffer[0] == '{')
            {
              ++bc;
            }
            else
            if(buffer[0] == '}')
            {
              --bc;
            }
          }
        }
      }

      if(pBase && !(pBase->HasFn<XAnimCycle>()) && !pBase->HasFn<XAnimationTake>())
        pBase->m_Name = chunk_name;
    }
    return XFileError::Success;
  }
  return XFileError::FailedToOpen;
}

//----------------------------------------------------------------------------------------------------------------------
XFileError::eType XModel::LoadAnim(const XString& filename)
{
  // if the file is not an XMD then call ImportAnim for any translators
  XU32 file__name_len_ = static_cast<XU32>( filename.size() );
  XString EXT;
  EXT += (XChar)tolower(filename.c_str()[ file__name_len_ - 3 ] );
  EXT += (XChar)tolower(filename.c_str()[ file__name_len_ - 2 ] );
  EXT += (XChar)tolower(filename.c_str()[ file__name_len_ - 1 ] );

  if (EXT!="xmd") 
  {
    if( XGlobal::ImportAnim(filename.c_str(),*this) )
    {
      return XFileError::Success;
    }

    return XFileError::FailedToOpen;
  }

  // LoadAnim doesn't currently work and has not been tested so use the regular Load() for now
  return Load(filename);

#if 0
  DUMPER(XModel__LoadAnim);

  XU32 file__name_len_ = static_cast<XU32>( filename.size() );
  XString EXT;
  EXT += (XChar)tolower(filename.c_str()[ file__name_len_ - 3 ] );
  EXT += (XChar)tolower(filename.c_str()[ file__name_len_ - 2 ] );
  EXT += (XChar)tolower(filename.c_str()[ file__name_len_ - 1 ] );

  if (EXT=="xld"||EXT=="xmd")
  {
    // run quick test on ascii & make sure valid
    XChar header[25] = "|" XMD_VERSION_ASCII "#0000#00000000|";
    bool isAscii=true;

    {
      FILE* fp = fopen(filename.c_str(),"rb");
      if(!fp)
        return XFileError::FailedToOpen;
      XChar temp[25];
      fread(temp,25,1,fp);
      fclose(fp);

      if(strncmp(temp,header,5)==0)
      {
        m_InputVersion = (temp[5]-'0')*100 + (temp[6]-'0')*10 + (temp[7]-'0');
        if (m_InputVersion>XMD_VERSION_NUMBER)
        {
          return XFileError::NewFormat;
        }
        isAscii = (temp[8]=='A')?true:false;
      }
      else
      {
        // not an XMD file.
        return XFileError::InvalidFile;
      }
    }

    if (!isAscii)
    {
      return LoadBinAnim(filename);
    }
  }
  else
  if (EXT!="xmd")
  {
    if( XGlobal::ImportAnim(filename.c_str(),*this) )
    {
      return XFileError::Success;
    }

    return XFileError::FailedToOpen;
  }


  // in file stream
  std::ifstream ifs;

  ifs.open(filename.c_str());

  if( ifs )
  {
    char temp[64]={0};
    ifs.getline(temp,64);

    XAnimCycle* aptr=0;

    std::map<XId,XId> nodeMap;

    while( !ifs.eof() )
    {
      XString chunk_header,chunk_name;
      XString buffer;

      // read the chunk header
      ifs >> buffer;
      if(ifs.eof())
        break;
      chunk_header = buffer;

      // read chunk name
      ifs >> buffer;
      if(ifs.eof())
        break;
      chunk_name = buffer;

      if (chunk_header == "ANIM_CYCLE")
      {
        aptr = new XAnimCycle(this);
        ifs >> buffer ;
        aptr->m_Name = chunk_name;
        if(!aptr->ReadChunk(ifs))
        {
          delete aptr;
          return XFileError::FileCorrupt;
        }

        // remap the node ID's
        aptr->RemapAnimNodes( nodeMap );

        // add the cycle to the model
        m_AnimCycles.push_back( aptr );
      }
      else
      if (chunk_header == "INFO" ||
        chunk_header == "INHERITED_NODE" ||
        chunk_header == "NEW_NODE")
      {
        XS32 bc=1;
        while(((XChar)ifs.get())!='{')
        {
        }
        while(bc)
        {
          XChar c = (XChar)ifs.get();
          if(c=='{')
            ++bc;
          if(c=='}')
            --bc;
        }
      }
      else
      {
        // skip "{", "id" and read ID
        XId id;
        ifs >> buffer >> buffer >> id;

        XBone* pNode = GuessBone(chunk_name);
        if(pNode)
        {
          nodeMap.insert( std::make_pair( id, pNode->GetID() ) );
        }

        // dump the rest of the chunk
        XS32 bc=1;
        size_t last = (size_t)ifs.tellg();
        while(bc)
        {
          ifs >> buffer;
          size_t pos = (size_t)ifs.tellg();
          last=pos;
          if(ifs.eof())
          {
            XGlobal::GetLogger()->Log(XBasicLogger::kError,"Unexpected end of file found");
            delete aptr;
            ifs.close();
            return XFileError::FileCorrupt;
          }
          if(strcmp(buffer.c_str(),"{")==0)
            ++bc;
          if(strcmp(buffer.c_str(),"}")==0)
            --bc;
        }
      }
    }

    // done with the file...
    ifs.close();
    return XFileError::Success;
  }
  return XFileError::FailedToOpen;
#endif//#if 0
}

extern void SkipTextures(FILE* fp) ;

//----------------------------------------------------------------------------------------------------------------------
XFileError::eType XModel::LoadBinAnim(const XString& filename)
{
  DUMPER(XModel__LoadBinAnim);
  XMU::FileInfo info( filename.c_str() );

  // a size check to ensure that we never go over the end of the file
  XS32 byte_size = info.GetSize();
  XFileIO io(filename.c_str());

  if(!io.Valid())
    return XFileError::FailedToOpen;

  SkipTextures(io.GetPtr());


  std::map<XId,XId> nodeMap;

  byte_size -= io.CurrPos();

  while( byte_size > 0 )
  {
    XChunkHeader header;

    XIO_CHECK( io.DoData( &header ) );

    XString head_str = TypeToHeader( (XFn::Type)header.Type);

    #ifdef _DEBUG
      XGlobal::GetLogger()->Logf(XBasicLogger::kInfo,"LOADED: %s", head_str.c_str());
    #endif
    XU32 file_position = io.CurrPos();
    XU32 end_position = file_position - header.ByteSize - sizeof(XChunkHeader);

    // if recognised as type, read it in
    if(head_str!="")
    {
      if( (XFn::Type)header.Type == XFn::AnimCycle )
      {
        // create new anim cycle and read
        XAnimCycle* aptr = new XAnimCycle(this);
        if( aptr->DoData(io) )
          // add to cycle list
          m_AnimCycles.push_back( aptr );
        else
        {
          // oooh, dear. delete anim and skip to next chunk
          delete aptr;
          goto skippy_chunky;
        }

        // remap the node ID's
        aptr->RemapAnimNodes( nodeMap );
      }
      else
      if( (XFn::Type)header.Type == XFn::Info )
      {
        goto skippy_chunky;
      }
      else
      if( (XFn::Type)header.Type == XFn::InheritedFileFlag )
      {
        goto skippy_chunky;
      }
      else
      if( (XFn::Type)header.Type == XFn::NewFileFlag )
      {
        goto skippy_chunky;
      }
      else
      {
        XId id;
        XString chunk_name;

        // read chunk name and ID
        XIO_CHECK( io.DoDataString(chunk_name) );
        XIO_CHECK( io.DoData(&id) );

        // try and get the chunk by it's name
        XBase* pNode = FindNode(chunk_name);
        if(pNode)
        {
          // place the ID's in a map to re-build the cycle later
          nodeMap.insert( std::make_pair( id, pNode->GetID() ) );
        }
      }
    }
    // else skip chunk
    else
    {
    skippy_chunky:

      // skip remainder of chunk
      io.Seek( end_position );
    }

    byte_size -= header.ByteSize;
  }

  return XFileError::Success;
}

//----------------------------------------------------------------------------------------------------------------------
XFileError::eType XModel::LoadTextures(FILE* fp)
{
  DUMPER(XModel__LoadTextures);
  XU16 num=0;
  fread(&num,1,2,fp);

  for(XU32 i=0;i<num;++i)
  {
    XImage* pImage = new XImage;
    XMD_ASSERT(pImage);

    if(!pImage->Read(fp))
    {
      delete pImage;
      break;
    }

    m_Images.push_back(pImage);
  }
  return XFileError::Success;
}

//----------------------------------------------------------------------------------------------------------------------
XFileError::eType XModel::LoadTextures(const XString& filename)
{
  DUMPER(XModel__LoadTextures);
  if(!filename.c_str()||filename=="")
    return XFileError::FailedToOpen;
  
  if (GetFileAttributesA(filename.c_str()) == INVALID_FILE_ATTRIBUTES)
    return XFileError::FailedToOpen;

  FILE* fp = fopen(filename.c_str(),"rb");
  if(!fp)
  {
    XGlobal::GetLogger()->Logf(XBasicLogger::kWarning,"texture file not found \"%s\"", filename.c_str());
    return XFileError::FileCorrupt;
  }

  XFileError::eType ret = LoadTextures(fp);

  fclose(fp);
  return ret;
}

//----------------------------------------------------------------------------------------------------------------------
XU32 XModel::NumAnimCycles() const
{
  return static_cast<XU32>(m_AnimCycles.size());
}

//----------------------------------------------------------------------------------------------------------------------
void XModel::PostLoad()
{
  DUMPER(XModel__PostLoad);

  XBoneList bones;
  GetRootBones(bones);
  {
    XMatrix identity;
    XBoneList::iterator it = bones.begin();
    for( ; it != bones.end(); ++it )
    {
      (*it)->Evaluate( identity );
    }
  }

  {
    /// an internal map of all nodes within the model
    XMap::iterator it = m_Nodes.begin();
    for( ; it != m_Nodes.end(); ++it )
    {
      it->second->PostLoad();
    }
  }
  SetToBindPose();
}

//----------------------------------------------------------------------------------------------------------------------
void XModel::BuildInternalArrays()
{
  DUMPER(XModel__BuildInternalArrays);

  /// an internal map of all nodes within the model
  XMap::iterator it = m_Nodes.begin();
  for( ; it != m_Nodes.end(); ++it )
  {
  }
}

//----------------------------------------------------------------------------------------------------------------------
bool XModel::ReassignIDs()
{
  DUMPER(XModel__ReassignIDs);

  return false;
}

//----------------------------------------------------------------------------------------------------------------------
XString XModel::RenameNode(const XString oldname,const XString newname)
{
  DUMPER(XModel__RenameNode);
  XBase* obj = FindNode(oldname);
  if(obj)
  {
    XS32 num=0;
    std::string TestName = newname;

    XS32 itcheck=0;
    (void)itcheck;
    while( FindNode(TestName) )
    {
      std::stringstream str;
      str << newname << (++num);
      TestName = str.str();

      // this might be an invalid assert. This is to check that XMD does not go spiralling
      // off into infinity when attempt to create a unique name for the node. It may be that
      // you do have  node1, node2, ..... node1000000  and want to create node1000001, in which
      // case change the assert.
      XMD_ASSERT(++itcheck < 1000000);
    }

    // rename the node with the new name
    obj->m_Name = TestName;

    if( obj->GetApiType() != XFn::AnimCycle )
    {
      XMap::iterator it = m_Nodes.find(oldname);
      XBase* ptr = it->second;
      m_Nodes.erase(it);
      m_Nodes.insert( std::make_pair(TestName,ptr) );
    }
    return TestName;
  }
  return "";
}

//----------------------------------------------------------------------------------------------------------------------
XString XModel::RenameNode(XBase* node,const XString newname)
{
  DUMPER(XModel__RenameNode);

  // cant rename this node because it is either NULL, or does not belong to this XModel
  XMD_ASSERT( node && IsValidNode(node) );

  XString n__ = newname;
  for(XU32 i=0;i<n__.size();++i)
  {
    if (n__[i] == ' ' || n__[i] == '\t')
    {
      n__[i] = '_';
    }
  }

  if(node)
  {
    XS32 num=0;
    XString TestName = n__;
    XS32 itcheck=0;
    (void)itcheck;
    while( FindNode(TestName) )
    {
      std::stringstream str;
      str << n__ << (++num);
      TestName = str.str();

      // this might be an invalid assert. This is to check that XMD does not go spiralling
      // off into infinity when attempt to create a unique name for the node. It may be that
      // you do have  node1, node2, ..... node1000000  and want to create node1000001, in which
      // case change the assert.
      XMD_ASSERT(++itcheck < 1000000);
    }

    XString oldname = node->GetName();

    // rename the node with the new name
    node->m_Name = TestName;

    if( node->GetApiType() != XFn::AnimCycle )
    {
      XMap::iterator it = m_Nodes.begin();

      for( ; it != m_Nodes.end(); ++it )
      {
        if( node->GetID() == it->second->GetID() )
          break;
      }
      XMD_ASSERT(it != m_Nodes.end());

      XBase* ptr = it->second;
      m_Nodes.erase(it);
      m_Nodes.insert( std::make_pair(TestName,ptr) );
    }

    return TestName;
  }
  return "";
}

//----------------------------------------------------------------------------------------------------------------------
XString XModel::RenameNode(const XId id,const XString newname)
{
  DUMPER(XModel__RenameNode);

  // If you assert here, this node is NULL or not fromn this XModel
  XMD_ASSERT( id && IsValidNode(id) );

  XBase* obj = FindNode(id);
  if(obj)
  {
    XS32 num=0;
    XString TestName = newname;
    XS32 itcheck=0;
    (void)itcheck;
    while( FindNode(TestName) )
    {
      std::stringstream str;
      str << newname << (++num);
      TestName = str.str();

      // this might be an invalid assert. This is to check that XMD does not go spiralling
      // off into infinity when attempt to create a unique name for the node. It may be that
      // you do have  node1, node2, ..... node1000000  and want to create node1000001, in which
      // case change the assert.
      XMD_ASSERT(++itcheck < 1000000);
    }
    XString oldname = obj->GetName();


    // rename the node with the new name
    obj->m_Name = TestName;

    if( obj->GetApiType() != XFn::AnimCycle )
    {
      XMap::iterator it = m_Nodes.begin();

      for( ; it != m_Nodes.end(); ++it )
      {
        if( obj->GetID() == it->second->GetID() )
          break;
      }
      XMD_ASSERT(it != m_Nodes.end());

      XBase* ptr = it->second;
      m_Nodes.erase(it);
      m_Nodes.insert( std::make_pair(TestName,ptr) );
    }

    return TestName;
  }
  return "";
}

//----------------------------------------------------------------------------------------------------------------------
void XModel::FormatName(XString& name)
{
  for(XU32 charIndex = 0; charIndex < name.size();) 
  {
    if(name[charIndex] == '"' || name[charIndex] == '/' || name[charIndex]== '\\') 
    {
      name.erase(name.begin() + charIndex);
    }
    else 
    {
      if(name[charIndex] == ' ')
      {
        name[charIndex] = '_';
      }
      ++charIndex;
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
XFileError::eType XModel::Export(const XString& filename)
{
  DUMPER(XModel__Export);
  return XGlobal::Export(filename.c_str(),*this) ? XFileError::Success : XFileError::FailedToOpen;
}

//----------------------------------------------------------------------------------------------------------------------
XFileError::eType XModel::Save(const XString& filename,bool ascii,bool include_anim)
{
  GetExportPolicy().Push();
  if(ascii)
    GetExportPolicy().Enable(XExportPolicy::WriteAscii);
  else
    GetExportPolicy().Disable(XExportPolicy::WriteAscii);
  if(include_anim)
    GetExportPolicy().Enable(XExportPolicy::WriteAnimations);
  else
    GetExportPolicy().Disable(XExportPolicy::WriteAnimations);
  XFileError::eType status = Save(filename);
  GetExportPolicy().Pop();
  return status;
}

//----------------------------------------------------------------------------------------------------------------------
XFileError::eType XModel::SaveToAsciiStream(std::ostream& ofs)
{
  XString header = "|" XMD_VERSION_ASCII "#";
  header += XGlobal::GetXmdHeader();
  ofs << header << "\n";

  if(m_pFileInfo)
  {
    m_pFileInfo->Write(ofs);
  }

  // output new node definitions
  {
    DUMPER(XNewNodeInfoList);
    XNewNodeInfoList::iterator it = m_NewNodes.begin();
    for( ; it != m_NewNodes.end(); ++it )
    {
      it->second->Write(ofs);
    }
  }

  // output inherited node definitions
  {
    DUMPER(XInheritedNodeList);
    XInheritedNodeList::iterator it = m_Inherited.begin();
    for( ; it != m_Inherited.end(); ++it )
    {
      (*it)->Write(ofs);
    }
  }

  // output texture data
  if(GetExportPolicy().IsEnabled(XExportPolicy::WriteTextures))
  {
    DUMPER(XTexturePlacement2DList);
    XList textures;
    List(textures,XFn::TexturePlacement2D);
    for(XU32 i=0;i!=textures.size();++i)
    {
      textures[i]->DoWriteChunk(ofs);
    }
  }

  // output env-map / bump-map data
  if(GetExportPolicy().IsEnabled(XExportPolicy::WriteTextures))
  {
    DUMPER(XTextureList);
    XList textures;
    List(textures,XFn::Texture);
    for(XU32 i=0;i!=textures.size();++i)
    {
      textures[i]->DoWriteChunk(ofs);
    }
  }

  // output materials
  if(GetExportPolicy().IsEnabled(XExportPolicy::WriteMaterials))
  {
    DUMPER(XMaterialList);
    XList mats;
    List(mats,XFn::Material);
    for(XU32 i=0;i!=mats.size();++i)
    {
      mats[i]->DoWriteChunk(ofs);
    }
  }

  // output surfaces
  if(GetExportPolicy().IsEnabled(XExportPolicy::WriteShapes))
  {
    DUMPER(XGeometryList);
    XList geoms;
    List(geoms,XFn::Shape);
    for(XU32 i=0;i!=geoms.size();++i)
    {
      geoms[i]->DoWriteChunk(ofs);
    }
  }

  // output transforms
  if(GetExportPolicy().IsEnabled(XExportPolicy::WriteBones))
  {
    DUMPER(XBoneList);
    XList bones;
    List(bones,XFn::Bone);
    for(XU32 i=0;i!=bones.size();++i)
    {
      bones[i]->DoWriteChunk(ofs);
    }
  }

  // output deformers
  if(GetExportPolicy().IsEnabled(XExportPolicy::WriteDeformers))
  {
    DUMPER(XDeformerList);
    XList deformers;
    List(deformers,XFn::Deformer);
    for(XU32 i=0;i!=deformers.size();++i)
    {
      deformers[i]->DoWriteChunk(ofs);
    }
  }

  // output others
  {
    DUMPER(XOtherList);
    XMap::iterator it = m_Nodes.begin();
    for( ; it != m_Nodes.end(); ++it )
    {
      XBase* b = it->second;
      if( !b->HasFn<XGeometry>() &&
        !b->HasFn<XShape>() &&
        !b->HasFn<XMaterial>() &&
        !b->HasFn<XTexture>() &&
        !b->HasFn<XTexPlacement2D>() &&
        !b->HasFn<XBone>() &&
        !b->HasFn<XDeformer>() )
      {
        b->DoWriteChunk(ofs);
      }
    }
  }

  // output animation cycles if requested
  if(GetExportPolicy().IsEnabled(XExportPolicy::WriteAnimations))
  {
    DUMPER(XAnimCycleList);
    XAnimList::iterator it = m_AnimCycles.begin();

    for( ; it != m_AnimCycles.end(); ++it )
    {
      (*it)->WriteChunk(ofs);
    }
  }

  return ofs.good() ? XFileError::Success : XFileError::FileCorrupt;
}

//----------------------------------------------------------------------------------------------------------------------
XFileError::eType XModel::SaveToBinaryStream(FILE* _fp)
{
  XFileIO* fp=0;

  fp = new XFileIO( _fp, false );
  XMD_ASSERT(fp);

  // write file header
  XString header = "|" XMD_VERSION_BINARY "#";
  header += XGlobal::GetXmdHeader();
  fp->DoData(&header[0],24);

  /*XFileError::eType ret = */
  SaveTextures(fp->GetPtr());

  // write file info
  if(m_pFileInfo)
  {
    if(!m_pFileInfo->DoData(*fp))
    {
      delete fp;
      return XFileError::FileCorrupt;
    }
  }

  // output new node definitions
  {
    DUMPER(XNewNodeInfoList);
    XNewNodeInfoList::iterator it = m_NewNodes.begin();
    for( ; it != m_NewNodes.end(); ++it )
    {
      if(!it->second->DoHeader(*fp))
      {
        delete fp;
        return XFileError::FileCorrupt;
      }
      if(!it->second->DoData(*fp))
        return XFileError::FileCorrupt;
    }
  }

  // output inherited node definitions
  {
    DUMPER(XInheritedNodeList);
    XInheritedNodeList::iterator it = m_Inherited.begin();
    for( ; it != m_Inherited.end(); ++it )
    {
      //  CHECK_CHUNK_BEGIN( *fp );
      if(!(*it)->DoHeader(*fp))
      {
        delete fp;
        return XFileError::FileCorrupt;
      }
      if(!(*it)->DoData(*fp))
      {
        delete fp;
        return XFileError::FileCorrupt;
      }
      //  CHECK_CHUNK_END( *fp, (*it) );
    }
  }

  // output texture data
  if(GetExportPolicy().IsEnabled(XExportPolicy::WriteTextures))
  {
    DUMPER(XTexturePlacement2DList);
    XList textures;
    List(textures,XFn::TexturePlacement2D);
    for(XU32 i=0;i!=textures.size();++i)
    {
      CHECK_CHUNK_BEGIN( *fp );
      if(!textures[i]->DoHeader(*fp))
      {
        delete fp;
        return XFileError::FileCorrupt;
      }
      if(!textures[i]->DoData(*fp))
      {
        delete fp;
        return XFileError::FileCorrupt;
      }
      CHECK_CHUNK_END( *fp, (*textures[i]) );
    }
  }

  // output env-map / bump-map data
  if(GetExportPolicy().IsEnabled(XExportPolicy::WriteTextures))
  {
    DUMPER(XTextureList);
    XList textures;
    List(textures,XFn::Texture);
    for(XU32 i=0;i!=textures.size();++i)
    {
      CHECK_CHUNK_BEGIN( *fp );
      if(!textures[i]->DoHeader(*fp))
      {
        delete fp;
        return XFileError::FileCorrupt;
      }
      if(!textures[i]->DoData(*fp))
      {
        delete fp;
        return XFileError::FileCorrupt;
      }
      CHECK_CHUNK_END( *fp, (*textures[i]) );
    }
  }

  // output materials
  if(GetExportPolicy().IsEnabled(XExportPolicy::WriteMaterials))
  {
    DUMPER(XMaterialList);
    XList mats;
    List(mats,XFn::Material);
    for(XU32 i=0;i!=mats.size();++i)
    {
      CHECK_CHUNK_BEGIN( *fp );
      if(!mats[i]->DoHeader(*fp))
      {
        delete fp;
        return XFileError::FileCorrupt;
      }
      if(!mats[i]->DoData(*fp))
      {
        delete fp;
        return XFileError::FileCorrupt;
      }
      CHECK_CHUNK_END( *fp, (*mats[i]) );
    }
  }

  // output surfaces
  if(GetExportPolicy().IsEnabled(XExportPolicy::WriteShapes))
  {
    DUMPER(XGeometryList);
    XList geoms;
    List(geoms,XFn::Shape);
    for(XU32 i=0;i!=geoms.size();++i)
    {
      CHECK_CHUNK_BEGIN( *fp );
      if(!geoms[i]->DoHeader(*fp))
      {
        delete fp;
        return XFileError::FileCorrupt;
      }
      if(!geoms[i]->DoData(*fp))
      {
        delete fp;
        return XFileError::FileCorrupt;
      }
      CHECK_CHUNK_END( *fp, (*geoms[i]) );
    }
  }

  // output transforms
  if(GetExportPolicy().IsEnabled(XExportPolicy::WriteBones))
  {
    DUMPER(XBoneList);
    XList bones;
    List(bones,XFn::Bone);
    for(XU32 i=0;i!=bones.size();++i)
    {
      CHECK_CHUNK_BEGIN( *fp );
      if(!bones[i]->DoHeader(*fp))
      {
        delete fp;
        return XFileError::FileCorrupt;
      }
      if(!bones[i]->DoData(*fp))
      {
        delete fp;
        return XFileError::FileCorrupt;
      }
      CHECK_CHUNK_END( *fp, (*bones[i]) );
    }
  }

  // output deformers
  if(GetExportPolicy().IsEnabled(XExportPolicy::WriteDeformers))
  {
    DUMPER(XDeformerList);
    XList deformers;
    List(deformers,XFn::Deformer);
    for(XU32 i=0;i!=deformers.size();++i)
    {
      CHECK_CHUNK_BEGIN( *fp );
      if(!deformers[i]->DoHeader(*fp))
      {
        delete fp;
        return XFileError::FileCorrupt;
      }
      if(!deformers[i]->DoData(*fp))
      {
        delete fp;
        return XFileError::FileCorrupt;
      }
      CHECK_CHUNK_END( *fp, (*deformers[i]) );
    }
  }

  // output others
  {
    DUMPER(XOtherList);
    XMap::iterator it = m_Nodes.begin();
    for( ; it != m_Nodes.end(); ++it )
    {
      XBase* b = it->second;
      if( !b->HasFn<XGeometry>() &&
        !b->HasFn<XShape>() &&
        !b->HasFn<XMaterial>() &&
        !b->HasFn<XTexture>() &&
        !b->HasFn<XTexPlacement2D>() &&
        !b->HasFn<XBone>() &&
        !b->HasFn<XDeformer>() )
      {
        CHECK_CHUNK_BEGIN( *fp );
        if(!b->DoHeader(*fp))
        {
          delete fp;
          return XFileError::FileCorrupt;
        }
        if(!b->DoData(*fp))
        {
          delete fp;
          return XFileError::FileCorrupt;
        }
        CHECK_CHUNK_END( *fp, (*b) );
      }
    }
  }


  // output animation cycles if requested
  if(GetExportPolicy().IsEnabled(XExportPolicy::WriteAnimations))
  {
    DUMPER(XAnimCycleList);
    XAnimList::iterator it = m_AnimCycles.begin();

    for( ; it != m_AnimCycles.end(); ++it )
    {
      CHECK_CHUNK_BEGIN( *fp );
      if(!(*it)->DoHeader(*fp))
      {
        delete fp;
        return XFileError::FileCorrupt;
      }
      if(!(*it)->DoData(*fp))
      {
        delete fp;
        return XFileError::FileCorrupt;
      }
      CHECK_CHUNK_END( *fp, (*(*it)) );
    }
  }

  delete fp;

  return XFileError::Success;
}

//----------------------------------------------------------------------------------------------------------------------
XFileError::eType XModel::Save(const XString& filename)
{
  DUMPER(XModel__Save);

  XFileError::eType ret = XFileError::Success;

  m_InputVersion = XMD_VERSION_NUMBER;

  bool ascii = GetExportPolicy().IsEnabled(XExportPolicy::WriteAscii);
  if(ascii)
  {
    std::ofstream ofs;
    ofs.open(filename.c_str());
    if(ofs)
    {
      ret = SaveToAsciiStream(ofs);
      ofs.close();
    }
    else
      ret = XFileError::FailedToOpen;
  }
  else
  {
    FILE* fp =fopen(filename.c_str(),"wb");
    if(fp)
    {
      ret = SaveToBinaryStream(fp);
      fclose(fp);
    }
    else
      ret = XFileError::FailedToOpen;
  }
  return ret;
}


//----------------------------------------------------------------------------------------------------------------------
XFileError::eType XModel::SaveAnims(const XString& filename,const XIndexList& cycle_ids)
{
  DUMPER(XModel__SaveAnims);
  (void)filename;
  (void)cycle_ids;

  return XFileError::FailedToOpen;
}

//----------------------------------------------------------------------------------------------------------------------
XFileError::eType XModel::SaveTextures(FILE* fp)
{
  DUMPER(XModel__SaveTextures);

  XU16 num=static_cast<XU16>(m_Images.size());
  fwrite(&num,1,2,fp);

  for(XU32 i=0;i<num;++i)
  {
    if(!m_Images[i]->Write(fp))
    {
      return XFileError::FailedToOpen;
    }
  }

  return XFileError::Success;
}

//----------------------------------------------------------------------------------------------------------------------
XFileError::eType XModel::SaveTextures(const XString& filename)
{
  DUMPER(XModel__SaveTextures);
  FILE* fp = fopen(filename.c_str(),"wb");
  if(!fp)
  {
    XGlobal::GetLogger()->Logf(XBasicLogger::kWarning,"file could not be opened for writing %s", filename.c_str());
    return XFileError::FailedToOpen;
  }
  XFileError::eType ret = SaveTextures(fp);

  fclose(fp);
  return ret;
}

//----------------------------------------------------------------------------------------------------------------------
void XModel::SetToBindPose()
{
  DUMPER(XModel__SetToBindPose);
  XList bones;
  List(bones,XFn::Bone);

  XList::iterator it = bones.begin();
  for( ; it != bones.end(); ++it )
  {
    XBone* b = (*it)->HasFn<XBone>();
    XMD_ASSERT(b);
    if( b )
    {
      b->SetToBindPose();
    }
  }
  it = bones.begin();
  for( ; it != bones.end(); ++it )
  {
    XBone* b = (*it)->HasFn<XBone>();
    XMD_ASSERT(b);
    if( b )
    {
      if(b->GetParent()==0)
      {
        b->Evaluate(XMatrix());
      }
    }
  }
  it = bones.begin();
  for( ; it != bones.end(); ++it )
  {
    XBone* b = (*it)->HasFn<XBone>();
    XMD_ASSERT(b);
    if( b )
    {
      b->BuildBindMatrices();
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
XU32 XModel::GetInputVersion() const
{
  return m_InputVersion;
}
}
