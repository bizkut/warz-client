//----------------------------------------------------------------------------------------------------------------------
/// \file Bone.cpp
/// \note (C) Copyright 2003-2005 Robert Bateman. All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#include "XMD/FileIO.h"
#include "XMD/Bone.h"
#include "XMD/Model.h"
#include "XMD/FileIO.h"

namespace XMD 
{
NMTL_POD_VECTOR_EXPORT(XBone*,XMD_EXPORT,XM2);
//----------------------------------------------------------------------------------------------------------------------
XJoint::XJoint(XModel* mod) : XBone(mod) ,m_SegmentScaleCompensate(true)
{
}
//----------------------------------------------------------------------------------------------------------------------
XJoint::~XJoint()
{
}
//----------------------------------------------------------------------------------------------------------------------
bool XJoint::GetSegmentScaleCompensate() const
{
  return m_SegmentScaleCompensate;
}

//----------------------------------------------------------------------------------------------------------------------
void XJoint::SetSegmentScaleCompensate(const bool val) 
{
  m_SegmentScaleCompensate = val;
}

//----------------------------------------------------------------------------------------------------------------------
bool XJoint::WriteChunk(std::ostream& ofs)
{
  if(!XBone::WriteChunk(ofs))
    return false;
  ofs << "\tsegment_scale_compensate " << m_SegmentScaleCompensate << "\n";
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool XJoint::ReadChunk(std::istream& ifs)
{
  if(!XBone::ReadChunk(ifs))
    return false;
  if(GetModel()->GetInputVersion()>=4)
  {
    READ_CHECK("segment_scale_compensate",ifs);
    ifs >> m_SegmentScaleCompensate;
  }
  return ifs.good();
}

//----------------------------------------------------------------------------------------------------------------------
bool XJoint::DoData(XFileIO& io)
{
  if(!XBone::DoData(io))
    return false;

  if(io.IsWriting() || GetModel()->GetInputVersion()>=4)
  {
    IO_CHECK( io.DoData(&m_SegmentScaleCompensate) );
  }
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool XJoint::NodeDeath(XId id)
{
  return XBone::NodeDeath(id);
}

//----------------------------------------------------------------------------------------------------------------------
void XJoint::PreDelete(XIdSet& extra_nodes)
{
  XBone::PreDelete(extra_nodes);
}

//----------------------------------------------------------------------------------------------------------------------
XU32 XJoint::GetDataSize() const
{
  return sizeof(bool) + XBone::GetDataSize();
}

//----------------------------------------------------------------------------------------------------------------------
void XJoint::DoCopy(const XBase* node)
{
  const XJoint* jnt = node->HasFn<XJoint>();
  m_SegmentScaleCompensate = jnt->m_SegmentScaleCompensate;
  XBone::DoCopy(node);
}

//----------------------------------------------------------------------------------------------------------------------
XFn::Type XJoint::GetApiType() const 
{
  return XFn::Joint;
}

//----------------------------------------------------------------------------------------------------------------------
XBase* XJoint::GetFn(XFn::Type type) 
{
  if (XFn::Joint == type)
  {
    return (XJoint*)this;
  }
  return XBone::GetFn(type);
}

//----------------------------------------------------------------------------------------------------------------------
const XBase* XJoint::GetFn(XFn::Type type) const
{
  if(XFn::Joint==type)
    return (const XJoint*)this;
  return XBone::GetFn(type);
}

#define SET_MIN(X) *((XU32*)((void*)&(X))) = 0xffffffff
#define SET_MAX(X) *((XU32*)((void*)&(X))) = 0x7fffffff

//----------------------------------------------------------------------------------------------------------------------
XBone::XBone(XModel* pmodel)
  : XBase(pmodel),
  m_Parent(0),
  m_ChildBones(),
  m_Instances(),
  m_BoundingBoxMin(),
  m_BoundingBoxMax(),
  m_ScalePivot(),
  m_RotatePivot(),
  m_ScalePivotTranslate(),
  m_RotatePivotTranslate(),
  m_Shearing(0,0,0),
  m_RotationOrder(kXYZ),
  m_JointOrient(),
  m_RotationOrient(),
  m_Transform(),
  m_BindTransform(),
  m_World(),
  m_Local(),
  m_Inverse(),
  m_InverseTranspose(),
  m_BindWorld(),
  m_BindLocal(),
  m_BindInverse(),
  m_BindInverseTranspose(),
  m_Visible(true),
  m_InheritsTransform(true)
{
  SET_MIN(m_TranslateLimits[0]);
  SET_MIN(m_TranslateLimits[2]);
  SET_MIN(m_TranslateLimits[4]);

  SET_MIN(m_RotateLimits[0]);
  SET_MIN(m_RotateLimits[2]);
  SET_MIN(m_RotateLimits[4]);

  SET_MIN(m_ScaleLimits[0]);
  SET_MIN(m_ScaleLimits[2]);
  SET_MIN(m_ScaleLimits[4]);

  SET_MAX(m_TranslateLimits[1]);
  SET_MAX(m_TranslateLimits[3]);
  SET_MAX(m_TranslateLimits[5]);

  SET_MAX(m_RotateLimits[1]);
  SET_MAX(m_RotateLimits[3]);
  SET_MAX(m_RotateLimits[5]);

  SET_MAX(m_ScaleLimits[1]);
  SET_MAX(m_ScaleLimits[3]);
  SET_MAX(m_ScaleLimits[5]);
}

#undef SET_MIN
#undef SET_MAX

//----------------------------------------------------------------------------------------------------------------------
XBone::~XBone()
{
}

//----------------------------------------------------------------------------------------------------------------------
bool XBone::IsVisible() const 
{
  return m_Visible; 
}

//----------------------------------------------------------------------------------------------------------------------
void XBone::SetVisible(bool v)  
{
  m_Visible=v; 
}

//----------------------------------------------------------------------------------------------------------------------
XFn::Type XBone::GetApiType() const
{
  return XFn::Bone;
}

//----------------------------------------------------------------------------------------------------------------------
void XBone::BuildBindMatrices()
{
  DUMPER(XBone__BuildBindMatrices);
  m_BindLocal = m_Local;
  m_BindWorld = m_World;// RecursiveBindBuild(this);
  m_BindInverse = m_Inverse;
  m_BindInverseTranspose = m_InverseTranspose;
}

//----------------------------------------------------------------------------------------------------------------------
void XBone::BuildMatrices()
{
  DUMPER(XBone__BuildMatrices);

  m_World = RecursiveBuild(this);
  m_Inverse.invert(m_World);
  m_InverseTranspose.transpose(m_Inverse);
}

//----------------------------------------------------------------------------------------------------------------------
bool XBone::DeleteInstance(XId instance) 
{
  DUMPER(XBone__DeleteInstance);
  XIndexList::iterator it = m_Instances.begin();
  for( ; it != m_Instances.end(); ++it )
  {
    if (instance == *it)
    {
      m_Instances.erase(it);
      return true;
    }
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool XBone::DoData(XFileIO& io) 
{
  DUMPER(XBone);

  CHECK_IO( XBase::DoData(io) );

  CHECK_IO( io.DoData(&m_BindTransform) );
  //    DPARAM(m_BindTransform);

  CHECK_IO( io.DoData(&m_Visible) );
  DPARAM(m_Visible);

  CHECK_IO( io.DoData(&m_ScalePivot) );
  DPARAM(m_ScalePivot);

  CHECK_IO( io.DoData(&m_RotatePivot) );
  DPARAM(m_RotatePivot);

  CHECK_IO( io.DoData(&m_BoundingBoxMin) );
  DPARAM(m_BoundingBoxMin);

  CHECK_IO( io.DoData(&m_BoundingBoxMax) );
  DPARAM(m_BoundingBoxMax);

  CHECK_IO( io.DoData(&m_RotationOrient) );
  DPARAM(m_RotationOrient);

  CHECK_IO( io.DoData(&m_JointOrient) );
  DPARAM(m_JointOrient);

  CHECK_IO( io.DoData(m_BindLocal.data,16) );
  DPARAM(m_BindLocal);

  if(GetModel()->GetInputVersion()>=4)
  {
    CHECK_IO( io.DoData(&m_RotationOrder) );
    DPARAM(m_RotationOrder);

    CHECK_IO( io.DoData(&m_InheritsTransform) );
    DPARAM(m_InheritsTransform);

    CHECK_IO( io.DoData(&m_Shearing) );
    DPARAM(m_Shearing);

    CHECK_IO( io.DoData(&m_ScalePivotTranslate) );
    DPARAM(m_ScalePivotTranslate);

    CHECK_IO( io.DoData(&m_RotatePivotTranslate) );
    DPARAM(m_RotatePivotTranslate);

    CHECK_IO( io.DoData(m_TranslateLimits,6) );
    CHECK_IO( io.DoData(m_RotateLimits,6) );
    CHECK_IO( io.DoData(m_ScaleLimits,6) );
  }

  CHECK_IO( io.DoData(&m_Parent) );
  DPARAM(m_Parent);

  CHECK_IO( io.DoDataVector(m_ChildBones) );
  DAPARAM(m_ChildBones);

  XInstanceList instances;
  GetObjectInstances(instances);
  if(io.IsWriting())
  {
    if(GetModel()->GetExportPolicy().IsEnabled(XExportPolicy::WriteShapes))
    {
      {
        XU32 num_instances=static_cast<XU32>(instances.size());
        CHECK_IO( io.DoData(&num_instances) );
        DPARAM(num_instances);
        if(num_instances)
        {
          instances.resize(num_instances);
          for (XU32 i=0;i!=num_instances;++i) 
          {
            if(io.IsReading())
              instances[i] = new XInstance(m_pModel);
            CHECK_IO( instances[i]->DoData(io) );
          }
        }
      }
    }
    else
    {
      XU32 no_instances=0;
      CHECK_IO( io.DoData(&no_instances) );
    }
  }
  else
  {
    {
      XU32 num_instances=static_cast<XU32>(instances.size());
      CHECK_IO( io.DoData(&num_instances) );
      DPARAM(num_instances);
      if(num_instances)
      {
        instances.resize(num_instances);
        for (XU32 i=0;i!=num_instances;++i) 
        {
          if(io.IsReading())
            instances[i] = new XInstance(m_pModel);
          CHECK_IO( instances[i]->DoData(io) );
        }
      }
    }
  }

  //  if reading, make sure the parents pointers of the instances are set to this.
  if(io.IsReading()) 
  {      
    XInstanceList::iterator it = instances.begin();
    for( ; it != instances.end(); ++it ) 
    {
      (*it)->m_Parent = this->GetID();
      m_Instances.push_back( (*it)->GetID() );
    }
  }

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
void XBone::DoCopy(const XBase* rhs)
{
  const XBone* cb = rhs->HasFn<XBone>();
  XMD_ASSERT(cb);
  m_BindTransform = cb->m_BindTransform;
  m_Transform = cb->m_Transform;
  m_Visible = cb->m_Visible;
  m_ScalePivot = cb->m_ScalePivot;
  m_RotatePivot = cb->m_RotatePivot;
  m_BoundingBoxMin = cb->m_BoundingBoxMin;
  m_BoundingBoxMax = cb->m_BoundingBoxMax;
  m_RotationOrient = cb->m_RotationOrient;
  m_JointOrient = cb->m_JointOrient;
  m_BindWorld = cb->m_BindWorld;
  m_BindLocal = cb->m_BindLocal;
  m_BindInverse = cb->m_BindInverse;
  m_BindInverseTranspose = cb->m_BindInverseTranspose;
  m_Local = cb->m_Local;
  m_World = cb->m_World;

  m_RotationOrder = cb->m_RotationOrder;
  m_InheritsTransform = cb->m_InheritsTransform;
  m_Shearing = cb->m_Shearing;

  memcpy(m_TranslateLimits,cb->m_TranslateLimits,6*sizeof(XReal));
  memcpy(m_RotateLimits,cb->m_RotateLimits,6*sizeof(XReal));
  memcpy(m_ScaleLimits,cb->m_ScaleLimits,6*sizeof(XReal));

  XClonePolicy& cp = GetModel()->GetClonePolicy();

  bool LeaveParent = cp.IsEnabled(XClonePolicy::LeaveCurrentParent);

  // can't do a straight copy, need to add to the parent bone
  m_Parent = 0;
  XBone* p = cb->GetParent();
  if(p && LeaveParent)
  {
    p->AddChild(this);
    m_Parent = p->GetID();
  }

  // if duplicating child bones
  if(cp.IsEnabled(XClonePolicy::DuplicateAllChildren))
  {
    XBoneList bones;
    cb->GetChildren(bones);

    // disable this whilst creating children
    cp.Disable(XClonePolicy::LeaveCurrentParent);

    // now duplicate each child and parent to this bone
    for (XBoneList::iterator it = bones.begin();it != bones.end(); ++it)
    {
      XBone* kid = (*it)->Clone()->HasFn<XBone>();
      AddChild(kid);
    }

      // enable again if needed
    if(LeaveParent)
      cp.Enable(XClonePolicy::LeaveCurrentParent);
  }

  // if we are duplicating child shape instances as well... 
  if(cp.IsEnabled(XClonePolicy::DuplicateAllInstances))
  {
    // if we are copying the shape data.... 
    if(cp.IsEnabled(XClonePolicy::DuplicateInstanceShapes))
    {
      m_Instances = cb->m_Instances;
    }
    else
    // if we are instancing the shape data again... 
    {
      XInstanceList instances;
      cb->GetObjectInstances(instances);
    }
  }
  XBase::DoCopy(cb);
}

//----------------------------------------------------------------------------------------------------------------------
void XJoint::Evaluate(const XMatrix& ParentX,bool recurse)
{
  // get scale and translation matrices
  XMatrix S;
  XMatrix T;
  S.scale(m_Transform.scale);
  T.translate(m_Transform.translate);

  // XMatrix _R = (GetRotationOrient() * GetRotation() * GetJointOrient()).AsMatrix();

  XMatrix RO,R,JO;
  RO.fromQuaternion(GetRotationOrient());
  R.fromQuaternion(GetRotation());
  JO.fromQuaternion(GetJointOrient());

  // compute inverse scale component of parent transform
  // (if segment scale compensate is turned on).
  XMatrix IS;
  if (m_SegmentScaleCompensate)
  {
    if (GetParent())
    {
      IS.scale(GetParent()->GetScale(false));
      IS.invert();
    }
  }

  // compute local space matrix
  //m_Local = T * IS * JO * R * RO * S;
  m_Local.quickMult(T,IS);
  XMatrix temp1,temp2,temp3;
  temp1.quickMult(T,IS);
  temp2.quickMult(JO,R);
  temp3.quickMult(RO,S);

  m_Local.quickMult(temp1,temp2);
  temp1.quickMult(m_Local,temp3);
  m_Local = temp1;

  // compute world space TM
  if(m_InheritsTransform)
  {
    m_World.quickMult(ParentX,m_Local);
  }
  else
  {
    m_World = m_Local;
  }

  // compute WS inverse
  m_Inverse.invert(m_World);

  // compute world space transpose
  m_InverseTranspose.transpose(m_Inverse);

  if(recurse)
  {
    // compute matrices of child transforms
    XIndexList::iterator it = m_ChildBones.begin();
    for( ; it != m_ChildBones.end(); ++it )
    {
      XBase* ptr = m_pModel->FindNode(*it);
      if(ptr)
      {
        XBone* pb = ptr->HasFn<XBone>();
        if(pb)
        {
          pb->Evaluate(m_World);
        }
      }
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
void XBone::Evaluate(const XMatrix& ParentX,bool recurse)
{
  DUMPER(XBone__Evaluate);
  //      -1                                 -1
  //  [Sp]  x [S] x [Sh] x [Sp] x [St] x [Rp]  x [Ro] x [R] x [Rp] x [Rt] x [T]
  //

  XMatrix T;    ///< translation                x
  XMatrix Rp;   ///< rotate pivot               x
  XMatrix R;    ///< rotation                   x
  XMatrix Ro;   ///< rotate axis                x
  XMatrix Rpi;  ///< inverse rotate pivot       x
  XMatrix Sp;   ///< scale pivot                x
  XMatrix Sh;   ///< Shearing                   x
  XMatrix S;    ///< Scaling                    x
  XMatrix Spi;  ///< inverse scale pivot        x
  XMatrix Rt;   ///< rotate pivot translation   x
  XMatrix St;   ///< Scale pivot translation    x

  T.translate( GetTranslation(false) );
  S.scale( GetScale(false) );
  Sp.translate( GetScalePivot() );
  Rp.translate( GetRotatePivot() );
  St.translate( GetScalePivotTranslate() );
  Rt.translate( GetRotatePivotTranslate() );
  Spi.invert(Sp);
  Rpi.invert(Rp);
  R.fromQuaternion(GetRotation(false));
  Ro.fromQuaternion(GetRotationOrient());

  Sh.m10 = m_Shearing.x;
  Sh.m20 = m_Shearing.y;
  Sh.m21 = m_Shearing.z;

  // compute local space matrix
  m_Local = T * Rt * Rp * R * Ro * Rpi * St * Sp * Sh * S * Spi;
  
  // compute world space TM
  if(m_InheritsTransform)
  {
    m_World.quickMult(ParentX,m_Local);
  }
  else
  {
    m_World = m_Local;
  }

  // compute WS inverse
  m_Inverse.invert(m_World);

  // compute world space transpose
  m_InverseTranspose.transpose(m_Inverse);

  if(recurse)
  {
    // compute matrices of child transforms
    XIndexList::iterator it = m_ChildBones.begin();
    for( ; it != m_ChildBones.end(); ++it )
    {
      XBase* ptr = m_pModel->FindNode(*it);
      if(ptr)
      {
        XBone* pb = ptr->HasFn<XBone>();
        if(pb)
        {
          pb->Evaluate(m_World);
        }
      }
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
bool XBone::GetChildren(XBoneList& bones) const
{
  DUMPER(XBone__GetChildren);
  bones.resize( m_ChildBones.size() );

  XIndexList::const_iterator it = m_ChildBones.begin();
  XBoneList::iterator it_bone = bones.begin();
  for(XU32 idx = 0; it != m_ChildBones.end(); ++it )
  {
    // make sure the bone index is not zero. zero indicates invalid
    XMD_ASSERT( (*it) );

    if(!IsValidObj((*it)))
    {
      bones.erase( it_bone );
      it_bone= bones.begin() + idx;
      continue;
    }

    XBase* b = m_pModel->FindNode( *it );

    // asserted because the child is not found in this XModel
    XMD_ASSERT(b);
    XBone* pBone = b->HasFn<XBone>();

    // asserted because the child is not actually a bone!
    XMD_ASSERT(pBone);
    *it_bone = pBone;
    ++it_bone;
    ++idx;
  }
  return bones.size() != 0;
}

//----------------------------------------------------------------------------------------------------------------------
XU32 XBone::GetDataSize() const 
{
  XU32 size = XBase::GetDataSize();
  size += sizeof(XMatrix);
  size += sizeof(bool)*2;
  size += sizeof(XQuaternion) * 2;
  size += 3*sizeof(XReal) * 7;
  size += sizeof(XQuaternion) + 6*sizeof(XReal);
  size += sizeof(XId);
  size += static_cast<XU32>(sizeof(XId) * m_ChildBones.size() + 4);

  size += sizeof(XReal)*18;
  size += sizeof(RotationOrder);

  XInstanceList instances;
  GetObjectInstances(instances);

  size += sizeof(XU32);
  for(XU32 i=0;i!= instances.size(); ++i)
    size += instances[i]->GetDataSize();

  return size;
}

//----------------------------------------------------------------------------------------------------------------------
const XMatrix& XBone::GetInverse(const bool bind_pose) const
{
  if(bind_pose)return m_BindInverse;
  return m_Inverse;
}

//----------------------------------------------------------------------------------------------------------------------
const XMatrix& XBone::GetInverseTranspose(const bool bind_pose) const
{
  if(bind_pose)return m_BindInverseTranspose;
  return m_InverseTranspose;
}

//----------------------------------------------------------------------------------------------------------------------
const XQuaternion& XBone::GetJointOrient() const
{
  return m_JointOrient;
}

//----------------------------------------------------------------------------------------------------------------------
const XVector3& XBone::GetScalePivot() const
{
  return m_ScalePivot;
}

//----------------------------------------------------------------------------------------------------------------------
const XVector3& XBone::GetScalePivotTranslate() const
{
  return m_ScalePivotTranslate;
}

//----------------------------------------------------------------------------------------------------------------------
bool XBone::GetInheritsTransform() const
{
  return m_InheritsTransform;
}

//----------------------------------------------------------------------------------------------------------------------
const XVector3& XBone::GetRotatePivot() const
{
  return m_RotatePivot;
}

//----------------------------------------------------------------------------------------------------------------------
const XVector3& XBone::GetRotatePivotTranslate() const
{
  return m_RotatePivotTranslate;
}

//----------------------------------------------------------------------------------------------------------------------
const XMatrix& XBone::GetLocal(const bool bind_pose) const
{
  if(bind_pose)return m_BindLocal;
  return m_Local;
}

//----------------------------------------------------------------------------------------------------------------------
bool XBone::GetObjectInstances(XInstanceList& ins) const
{
  DUMPER(XBone__GetObjectInstances);
  ins.clear();


  XInstanceList list_;
  m_pModel->GetObjectInstances(list_);

  if (list_.size()==0)
  {
    return false;
  }

  XIndexList::const_iterator it = m_Instances.begin();
  for( ; it != m_Instances.end(); ++it )
  {
    XInstanceList::iterator it_ = list_.begin();
    for( ; it_ != list_.end(); ++it_ )
    {
      if (*it == (*it_)->GetID()) 
      {
        ins.push_back(*it_);
        break;
      }
    }
  }
  return ins.size() != 0;
}

//----------------------------------------------------------------------------------------------------------------------
XBone* XBone::GetParent() const
{
  DUMPER(XBone__GetParent);
  XBase* b = m_pModel->FindNode( m_Parent );
  if (b) 
  {
    XBone* pBone = b->HasFn<XBone>();
    return pBone;
  }
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
const XVector3& XBone::GetBoundingBoxMin(const bool bind_pose) const
{
  (void)bind_pose;
  return m_BoundingBoxMin;
}

//----------------------------------------------------------------------------------------------------------------------
const XVector3& XBone::GetBoundingBoxMax(const bool bind_pose) const
{
  (void)bind_pose;
  return m_BoundingBoxMax;
}

//----------------------------------------------------------------------------------------------------------------------
const XQuaternion& XBone::GetRotation(const bool bind_pose) const
{
  if(bind_pose)return m_BindTransform.rotate;
  return m_Transform.rotate;
}

//----------------------------------------------------------------------------------------------------------------------
const XQuaternion& XBone::GetRotationOrient() const
{
  return m_RotationOrient;
}

//----------------------------------------------------------------------------------------------------------------------
const XVector3& XBone::GetScale(const bool bind_pose) const
{
  if(bind_pose)return m_BindTransform.scale;
  return m_Transform.scale;
}

//----------------------------------------------------------------------------------------------------------------------
const XVector3& XBone::GetShearing() const
{
  return m_Shearing;
}

//----------------------------------------------------------------------------------------------------------------------
const XVector3& XBone::GetTranslation(const bool bind_pose) const
{
  if(bind_pose)return m_BindTransform.translate;
  return m_Transform.translate;
}

//----------------------------------------------------------------------------------------------------------------------
const XMatrix& XBone::GetWorld(const bool bind_pose) const
{
  if(bind_pose)return m_BindWorld;
  return m_World;
}

//----------------------------------------------------------------------------------------------------------------------
XBase* XBone::GetFn(XFn::Type type)
{
  if (XFn::Bone == type)
  {
    return (XBone*)this;
  }
  return XBase::GetFn(type);
}

//----------------------------------------------------------------------------------------------------------------------
const XBase* XBone::GetFn(XFn::Type type) const
{
  if(XFn::Bone==type)
    return (const XBone*)this;
  return XBase::GetFn(type);
}

//----------------------------------------------------------------------------------------------------------------------
void removeScale(XMatrix& m)
{
  XVector3 scale;
  m.getScale(scale);
  m.xAxis().normalise();
  m.yAxis().normalise();
  m.zAxis().normalise();
  m.wAxis().x *= scale.x;
  m.wAxis().y *= scale.y;
  m.wAxis().z *= scale.z;
}

//----------------------------------------------------------------------------------------------------------------------
bool hasNegativeScale(const XMatrix& M)
{
  // see what X cross Y gives us
  XVector3 TestZ;
  TestZ.cross(M.xAxis(),M.yAxis());

  // if Z and our TestZ face in opposite directions, we have negative scale. 
  return (TestZ.dot(M.zAxis())) < 0;
}

//----------------------------------------------------------------------------------------------------------------------
void XBone::KillScale()
{
  // if the bone has a parent, modify the translation by the 
  XBone* parent = GetParent();
  if(parent)
  {
    m_Transform.translate *= parent->GetScale(false);
  }

  XBoneList bones;
  GetChildren(bones);
  for (XBoneList::iterator it = bones.begin(); it != bones.end(); ++it)
  {
    (*it)->KillScale();
  }
  
  m_Transform.scale.set(1.0f,1.0f,1.0f);
}

//----------------------------------------------------------------------------------------------------------------------
void XBone::FreezeTransforms(bool local_is_LH)
{
  XBone* parent = GetParent();

  XMatrix WS = GetWorld(false);
  XMatrix LS = GetLocal(false);

  bool WSneg = hasNegativeScale(WS);
  bool LSneg = hasNegativeScale(LS);
  bool must_flip_geometry_in_x=false;

  if(WSneg) 
  {
    WS.xAxis().negate();
    must_flip_geometry_in_x = true;
  }

  // create a new local space TM
  XMatrix new_LS;
  if(parent)
    new_LS = WS * parent->GetInverse(false);
  else
    new_LS = WS;

  m_Transform.translate.set(new_LS.wAxis().data);
  m_Transform.rotate.fromMatrix(new_LS);
  m_Transform.scale.set(new_LS.xAxis().length(),
                        new_LS.yAxis().length(),
                        new_LS.zAxis().length());

  m_Local = new_LS;

  // finally, if parent is scaled negatively, then mirror translate 
  if(parent)
  {
    if(local_is_LH)
    {
      m_Transform.translate.x = -m_Transform.translate.x;
    }
    Evaluate(parent->GetWorld(false),false);
  }
  else
    Evaluate(XMatrix(),false);

  // modify children
  XBoneList bones;
  GetChildren(bones);
  for (XBoneList::iterator it = bones.begin(); it != bones.end(); ++it)
  {
    (*it)->FreezeTransforms(LSneg);
  }

  // add a temp attr to the model so that we can determine the modifications needed 
  // on the geometry later. 
  if (must_flip_geometry_in_x && !GetAttribute("must_scale_geom_by_minus_1_in_x"))
  {
    XAttribute* attr = CreateAttribute("must_scale_geom_by_minus_1_in_x");
    attr->SetType(XFn::BoolAttribute);
    attr->Set(true);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void XBone::KillOrients()
{
  m_JointOrient.set(0,0,0,1.0f);
  m_RotationOrient.set(0,0,0,1.0f);
}

//----------------------------------------------------------------------------------------------------------------------
void XBone::RemoveOrients()
{
  XMatrix pre;
  pre.fromQuaternion(m_JointOrient);
  XMatrix post;
  post.fromQuaternion(m_RotationOrient);
  XMatrix rot,combined;
  XModel* model = m_pModel;

  // first manipulate all of the anim cycle keys for this bone
  //
  for (XU32 i=0;i!=model->NumAnimCycles();++i) 
  {
    XAnimCycle* cycle = model->GetAnimCycle(i);
    XSampledKeys* bone_keys = cycle->GetBoneKeys(this);
    if (bone_keys) 
    {
      XQuaternionArray::iterator it  = bone_keys->RotationKeys().begin();
      XQuaternionArray::iterator end = bone_keys->RotationKeys().end();
      for ( ; it != end; ++it ) 
      {
        // check I have these the right way around
        XQuaternion q;
        q.mul(m_JointOrient,*it);
        it->mul(q,m_RotationOrient);
        /*
        rot.fromQuaternion(*it);
        combined.quickMult(pre,rot);
        combined.quickMult(post);
        it->fromMatrix(combined)
        it->normalise();
        */
      }
    }
  }

  m_Transform.rotate.toMatrix(rot);
  combined = pre;
  combined *= rot;
  combined *= post;
  combined.toQuaternion(m_Transform.rotate);
  m_Transform.rotate.normalise();

  m_BindTransform.rotate.toMatrix(rot);
  combined = pre;
  combined *= rot;
  combined *= post;
  combined.toQuaternion(m_BindTransform.rotate);
  m_BindTransform.rotate.normalise();

  m_JointOrient.identity();
  m_RotationOrient.identity();

  m_BindTransform.toMatrix(m_BindLocal);
  m_Transform.toMatrix(m_Local);
}

//----------------------------------------------------------------------------------------------------------------------
void XBone::PreDelete(XIdSet& extra_nodes)
{
  DUMPER(XBone__PreDelete);
  const XDeletePolicy& dp = GetModel()->GetDeletePolicy();

  if ( dp.IsEnabled(XDeletePolicy::RelatedTransforms) ) 
  {
    XIndexList::iterator it = m_ChildBones.begin();
    for( ; it != m_ChildBones.end(); ++it )
    {
      extra_nodes.insert( *it );
    }
  }

  if ( dp.IsEnabled(XDeletePolicy::RelatedChildren) ) 
  {
    XInstanceList instances;
    if(GetObjectInstances(instances))
    {
      XInstanceList::iterator it = instances.begin();
      for( ; it != instances.end(); ++it )
      {
        XInstance* ptr = *it;
        XShape* pgeom = ptr->GetShape();
        if( pgeom )
        {
          extra_nodes.insert( pgeom->GetID() );
        }
      }
    }
  }

  XBase::PreDelete(extra_nodes);
}

//----------------------------------------------------------------------------------------------------------------------
bool XBone::NodeDeath(XId id)
{
  DUMPER(XBone__NodeDeath);
  XInstanceList instances;
  if(GetObjectInstances(instances))
  {
    XInstanceList::iterator it = instances.begin();
    for( ; it != instances.end(); ++it )
    {
      XInstance* ptr = *it;
      XShape* pgeom = ptr->GetShape();
      if( pgeom && pgeom->GetID() == id )
      {
        // delete the instance from the model
        m_pModel->DeleteInstance( ptr->GetID() );
        break;
      }

      if (ptr->HasMaterialGroups()) 
      {
        for (XU32 i=0;i!=ptr->GetNumMaterialGroups();++i) 
        {
          XMaterial* material = ptr->GetMaterial(i);
          if (material && material->GetID()==id) 
          {
            ptr->SetMaterial(0,i);
            break;
          }
        }
      }
      else
      {
        XMaterial* material = ptr->GetMaterial();
        if (material && material->GetID()==id) 
        {
          ptr->SetMaterial(0);
        }
      }

    }
  }

  XIndexList::iterator it = m_ChildBones.begin();
  for( ; it != m_ChildBones.end(); ++it )
  {
    if( *it == id )
    {
      m_ChildBones.erase( it );
      break;
    }
  }

  if (id == m_Parent) 
  {
    m_Parent=0;
  }

  return XBase::NodeDeath(id);
}

//----------------------------------------------------------------------------------------------------------------------
bool XBone::IsChildNode(const XBone* kid) const
{
  if (!kid || !IsValidObj(kid))
    return false;

  XIndexList::const_iterator it = m_ChildBones.begin();
  for (; it != m_ChildBones.end(); ++it)
  {
    if (*it  == kid->GetID())
    {
      return true;
    }
    const XBase* base = GetModel()->FindNode(*it);
    if(base)
    {
      const XBone* bone = base->HasFn<XBone>();
      if(bone->IsChildNode(kid))
        return true;
    }
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool XBone::AddChild(XBone* kid)
{
  DUMPER(XBone__AddChild);
  if(!kid) 
    return false;

  // do not allow to parent to self
  if (kid == this)
  {
    return false;
  }

  bool is_valid = IsValidObj(kid);

  // if you assert here, then the child you are attempting to add as 
  // a child is not a member of the same XModel as this node.
  XMD_ASSERT(is_valid);

  if(!is_valid)
    return false;

  bool is_already_a_child = IsChildNode(kid);
  if (is_already_a_child)
  {
    XGlobal::GetLogger()->Logf(XBasicLogger::kError,
      "Cannot add \"%s\" as a child, since it is already a child of \"%s\" or one of it's children....",
       kid->GetName(), GetName());
    return false;
  }

  is_already_a_child = kid->IsChildNode(this);
  if (is_already_a_child)
  {
    XGlobal::GetLogger()->Logf(XBasicLogger::kError,
      "Cannot add \"%s\" as a child, since it is a parent of this node \"%s\" or one of it's children....",
      kid->GetName(), GetName());
    return false;
  }

  m_ChildBones.push_back(kid->GetID());
  kid->m_Parent = this->GetID();
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool XBone::RemoveChild(XBone* kid)
{
  DUMPER(XBone__RemoveChild);
  if(!kid) return false;
  XIndexList::iterator it = m_ChildBones.begin();
  for (; it != m_ChildBones.end(); ++it)
  {
    if (*it  == kid->GetID())
    {
      m_ChildBones.erase( it );
      kid->m_Parent = 0;
      return true;
    }
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
void XBone::AddInstance(const XInstance* kid)
{
  DUMPER(XBone__AddInstance);
  if(!kid) 
    return;

  if (kid->GetParent() != this)
  {
    XGlobal::GetLogger()->Logf(XBasicLogger::kError, "cannot add instance to %s because it's for the wrong bone", GetName());
    return;
  }

  // MORPH-21317: IsValidInstance check required.

  XInstanceList instances;
  GetObjectInstances(instances);
  for (XInstanceList::iterator it = instances.begin(); it != instances.end(); ++it)
  {
    XInstance* inst = *it;
    if (inst->GetShape() == kid->GetShape())
    {
      // already an instance...
      return;
    }
  }
  m_Instances.push_back(kid->GetID());
}

//----------------------------------------------------------------------------------------------------------------------
void XBone::AddParent(const XBone* ptr)
{
  DUMPER(XBone__AddParent);
  if(!ptr) return;
  m_Parent = ptr->GetID();
}

//----------------------------------------------------------------------------------------------------------------------
XReal min_limit_value(std::istream& ifs)
{
  XString minl;
  ifs >> minl;
  XReal f;
  *((XU32*)((void*)&f)) = 0xffffffff;
  if (minl == "-INF")
    return f;
  #if _MSC_VER >= 1400
  sscanf_s(minl.c_str(),"%f",&f);
  #else
  sscanf(minl.c_str(),"%f",&f);
  #endif
  return f;
}

//----------------------------------------------------------------------------------------------------------------------
XReal max_limit_value(std::istream& ifs)
{
  XString minl;
  ifs >> minl;
  XReal f;
  *((XU32*)((void*)&f)) = 0x7fffffff;
  if (minl == "+INF")
    return f;
  #if _MSC_VER >= 1400
  sscanf_s(minl.c_str(),"%f",&f);
  #else
  sscanf(minl.c_str(),"%f",&f);
  #endif
  return f;
}

//----------------------------------------------------------------------------------------------------------------------
bool XBone::ReadChunk(std::istream& ifs)
{
  DUMPER(XBone__ReadChunk);
  READ_CHECK("visible",ifs);
  ifs >> m_Visible;

  READ_CHECK("scale_pivot",ifs);
  ifs >> m_ScalePivot.x
      >> m_ScalePivot.y
      >> m_ScalePivot.z;

  READ_CHECK("rotate_pivot",ifs);
  ifs >> m_RotatePivot.x
      >> m_RotatePivot.y
      >> m_RotatePivot.z;

  READ_CHECK("min_bound",ifs);
  ifs >> m_BoundingBoxMin.x
      >> m_BoundingBoxMin.y
      >> m_BoundingBoxMin.z;

  READ_CHECK("max_bound",ifs);
  ifs >> m_BoundingBoxMax.x
      >> m_BoundingBoxMax.y
      >> m_BoundingBoxMax.z;

  // read scale
  READ_CHECK("scale",ifs);
  ifs >> m_BindTransform.scale.x
      >> m_BindTransform.scale.y
      >> m_BindTransform.scale.z;

  // read translate
  READ_CHECK("translate",ifs);
  ifs >> m_BindTransform.translate.x
      >> m_BindTransform.translate.y
      >> m_BindTransform.translate.z;

  // read rotation_orient
  READ_CHECK("rotation_orient",ifs);
  ifs >> m_RotationOrient.x
      >> m_RotationOrient.y
      >> m_RotationOrient.z
      >> m_RotationOrient.w;

  // read rotation_orient
  READ_CHECK("rotation",ifs);
  ifs >> m_BindTransform.rotate.x
      >> m_BindTransform.rotate.y
      >> m_BindTransform.rotate.z
      >> m_BindTransform.rotate.w;
  m_Transform = m_BindTransform;

  // read joint_orient
  READ_CHECK("joint_orient",ifs);
  ifs >> m_JointOrient.x
      >> m_JointOrient.y
      >> m_JointOrient.z
      >> m_JointOrient.w;

  // read bind_pose_matrix
  READ_CHECK("bind_pose_matrix",ifs);
  ifs >> m_BindLocal.data[ 0] >> m_BindLocal.data[ 1] >> m_BindLocal.data[ 2] >> m_BindLocal.data[ 3]
      >> m_BindLocal.data[ 4] >> m_BindLocal.data[ 5] >> m_BindLocal.data[ 6] >> m_BindLocal.data[ 7]
      >> m_BindLocal.data[ 8] >> m_BindLocal.data[ 9] >> m_BindLocal.data[10] >> m_BindLocal.data[11]
      >> m_BindLocal.data[12] >> m_BindLocal.data[13] >> m_BindLocal.data[14] >> m_BindLocal.data[15] ;

  if(GetModel()->GetInputVersion()>=4)
  {
    READ_CHECK("rotation_order",ifs);
    {
      XString temp;
      ifs >> temp;
      switch(temp[0])
      {
      case 'x':case 'X':
        m_RotationOrder = ((temp[1]=='y')|(temp[1]=='Y')) ? kXYZ : kXZY;
        break;
      case 'y':case 'Y':
        m_RotationOrder = ((temp[1]=='x')|(temp[1]=='X')) ? kYXZ : kYZX;
        break;
      case 'z':case 'Z':
        m_RotationOrder = ((temp[1]=='y')|(temp[1]=='Y')) ? kZYX : kZXY;
        break;
      default:
        XMD_ASSERT(0 && "Invalid rotation order in file");
        break;
      }
    }
    READ_CHECK("inherits_transform",ifs);
    ifs >> m_InheritsTransform;

    READ_CHECK("shearing",ifs);
    ifs >> m_Shearing.x 
        >> m_Shearing.y 
        >> m_Shearing.z;

    READ_CHECK("rotate_pivot_translate",ifs);
    ifs >> m_RotatePivotTranslate.x
        >> m_RotatePivotTranslate.y
        >> m_RotatePivotTranslate.z;

    READ_CHECK("scale_pivot_translate",ifs);
    ifs >> m_ScalePivotTranslate.x
        >> m_ScalePivotTranslate.y
        >> m_ScalePivotTranslate.z;

    READ_CHECK("translate_limit_x",ifs);
    m_TranslateLimits[0] = min_limit_value(ifs);
    m_TranslateLimits[1] = max_limit_value(ifs);
    
    READ_CHECK("translate_limit_y",ifs);
    m_TranslateLimits[2] = min_limit_value(ifs);
    m_TranslateLimits[3] = max_limit_value(ifs);

    READ_CHECK("translate_limit_z",ifs);
    m_TranslateLimits[4] = min_limit_value(ifs);
    m_TranslateLimits[5] = max_limit_value(ifs);

    READ_CHECK("rotate_limit_x",ifs);
    m_RotateLimits[0] = min_limit_value(ifs);
    m_RotateLimits[1] = max_limit_value(ifs);

    READ_CHECK("rotate_limit_y",ifs);
    m_RotateLimits[2] = min_limit_value(ifs);
    m_RotateLimits[3] = max_limit_value(ifs);

    READ_CHECK("rotate_limit_z",ifs);
    m_RotateLimits[4] = min_limit_value(ifs);
    m_RotateLimits[5] = max_limit_value(ifs);

    READ_CHECK("scale_limit_x",ifs);
    m_ScaleLimits[0] = min_limit_value(ifs);
    m_ScaleLimits[1] = max_limit_value(ifs);

    READ_CHECK("scale_limit_y",ifs);
    m_ScaleLimits[2] = min_limit_value(ifs);
    m_ScaleLimits[3] = max_limit_value(ifs);

    READ_CHECK("scale_limit_z",ifs);
    m_ScaleLimits[4] = min_limit_value(ifs);
    m_ScaleLimits[5] = max_limit_value(ifs);
  }

  // read parent ID
  READ_CHECK("parent",ifs);
  ifs >> m_Parent;

  // read child ID's
  READ_CHECK("child_joints",ifs);

  XU32 num;
  ifs >> num;
  m_ChildBones.resize(num);

  XIndexList::iterator it = m_ChildBones.begin();
  for( ; it != m_ChildBones.end(); ++it )
  {
    ifs >> *it;
  }

  // read object instaces
  READ_CHECK("instance_list",ifs);

  // read {
  READ_CHECK("{",ifs);

  XS32 bc = 1;

  while(bc) 
  {
    XString buffer;
    ifs >> buffer;

    if(strcmp("}",buffer.c_str())==0)
      --bc;
    else
    if(strcmp("{",buffer.c_str())==0)
      ++bc;
    else
    if(strcmp("INSTANCE",buffer.c_str())==0)
    {
      XString instanceName;
      XU32 ID,nUvSets;

      // read name of instance
      ifs >> instanceName >> buffer;

      // read the ID of the instanced object

      READ_CHECK("object_id",ifs);
      ifs >> ID;

      XStringList uv_set_names;
      XIndexList assigned_textures;

      if(GetModel()->GetInputVersion()>=4)
      {
        READ_CHECK("num_uv_sets",ifs);
        ifs >> nUvSets;

        uv_set_names.resize(nUvSets);
        assigned_textures.resize(nUvSets);

        for (XU32 i=0;i!=nUvSets;++i)
        {
          ifs >> assigned_textures[i] >> uv_set_names[i];
        }
      }

      // retrieve a pointer for the geometry
      XShape* pObj=0;
      {
        XBase* pBase = m_pModel->FindNode(ID);
        if(!pBase)
        {
          XGlobal::GetLogger()->Logf(XBasicLogger::kWarning, "Shape ID %d : name %s not found.", ID, instanceName.c_str());
        }
        else
        {
          pObj = pBase->HasFn<XShape>();
          if(!pObj)
          {
            XGlobal::GetLogger()->Logf(XBasicLogger::kWarning, "Shape ID %d : name %s is not a shape.", ID, instanceName.c_str());
          }
        }
      }

      // if invalid instance found, skip stuff till the end of the instance block. 
      if (!pObj)
      {
        ++bc;
        while(bc) 
        {
          XString buffer;
          ifs >> buffer;

          if(strcmp("}",buffer.c_str())==0)
            --bc;
          else
          if(strcmp("{",buffer.c_str())==0)
            ++bc;
        }
      }
      else
      {
        // create a new instance
        XInstance* pInst = m_pModel->CreateGeometryInstance(this,pObj);

        for(XU32 ii=0;ii!=uv_set_names.size();++ii)
        {
          XBase* b = GetModel()->FindNode(assigned_textures[ii]);
          if(!b)
            continue;
          XTexture* tex = b->HasFn<XTexture>();
          if(tex)
          {
            pInst->AssignTextureAndUvSet(tex,uv_set_names[ii].c_str());
          }
        }

        if(!pInst)
          return false;

        // set the instance name
        pInst->SetName(instanceName.c_str());

        // read material(s) assigned to the geometry object.
        bool done=false;
        while(!done) 
        {
          ifs >> buffer;

          if(strcmp(buffer.c_str(),"material")==0) 
          {
            XU32 mID = 0;
            ifs >> mID;
            XBase* pBase = m_pModel->FindNode(mID);
            if(pBase)
            {
              XMaterial* pMat = pBase->HasFn<XMaterial>();
              if(!pMat)
                return false;

              if(!pInst->SetMaterial(pMat))
                return false;
            }
          }
          else
          if(strcmp(buffer.c_str(),"MATERIAL_GROUP")==0) 
          {
            READ_CHECK("{",ifs);

            // read material node ID
            READ_CHECK("material_id",ifs);
            XU32 matID;
            ifs >> matID;

            // get material node. This might end up being NULL
            XBase* pBase = m_pModel->FindNode(matID);

            // get material interface
            XMaterial* pMat = pBase ? pBase->HasFn<XMaterial>() : 0;
    

            READ_CHECK("num_face_indices",ifs);
            XU32 numFaces;
            ifs >> numFaces;

            // read faces
            XIndexList FaceIds;
            FaceIds.resize(numFaces);
            XIndexList::iterator iti = FaceIds.begin();
            for( ; iti != FaceIds.end(); ++iti ) 
            {
              ifs >> *iti;
            }

            // terminate group
            READ_CHECK("}",ifs);

            // set material group
            XU32 grp = pInst->GetNumMaterialGroups();
            pInst->SetMaterial(pMat,grp);
            pInst->SetFaces(grp,FaceIds);
          }
          else
          if(strcmp(buffer.c_str(),"}")==0) 
          {
            done = true;
          }
          else 
          {
            XGlobal::GetLogger()->Log(XBasicLogger::kError,"Unexpected error when reading instance list");
            return false;
          }
        }
      }
    }
    else
    {
      XGlobal::GetLogger()->Log(XBasicLogger::kError,"Unexpected error when reading instance list");
      return false;
    }
  }

  return ifs.good();
}

//----------------------------------------------------------------------------------------------------------------------
XMatrix XBone::RecursiveBindBuild(XBone* pb)
{
  DUMPER(XBone__RecursiveBindBuild);

  // asserted because the parent bone is NULL or not from this XModel.
  // this really shouldn't happen
  XMD_ASSERT( (pb) && IsValidObj(pb) );
  XBone* p = pb->GetParent();
  XMatrix world ;
  if(p)
  {
    world = RecursiveBindBuild(p);
  }
  world *= m_BindLocal;
  return world;
}

//----------------------------------------------------------------------------------------------------------------------
XMatrix XBone::RecursiveBuild(XBone* pb)
{
  DUMPER(XBone__RecursiveBuild);

  // asserted because the parent bone is NULL or not from this XModel.
  // this really shouldn't happen
  XMD_ASSERT( (pb) && IsValidObj(pb) );
  XBone* p = pb->GetParent();
  XMatrix world;
  #if 0
  world = m_Local;
  if(p) 
  {
    world *= RecursiveBuild(p);
  }
  #else
  if(p) 
  {
    world = RecursiveBuild(p);
  }
  world *= m_Local;
  #endif
  return world;
}

//----------------------------------------------------------------------------------------------------------------------
bool XBone::SetChildren(XBoneList& children)
{
  XIndexList indices;
  {
    XBoneList::const_iterator it = children.begin();
    for (;it != children.end();++it)
    {
      if (*it == 0)
        return false;
      if (!IsValidObj(*it))
        return false;
      if (IsChildNode(*it))
        return false;
      indices.push_back( (*it)->GetID() );
    }
  }
  m_ChildBones = indices;

  XBoneList::iterator it = children.begin();
  for (;it != children.end();++it)
  {
    (*it)->m_Parent = GetID();
  }
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
void XBone::SetInheritsTransform(const bool value) 
{
  m_InheritsTransform = value;
}

//----------------------------------------------------------------------------------------------------------------------
void XBone::SetJointOrient(const XQuaternion& r)
{
  m_JointOrient=r;
}

//----------------------------------------------------------------------------------------------------------------------
void XBone::SetIdentity() 
{
  DUMPER(XBone__SetIdentity);
  m_World.identity();
  m_Local.identity();
  m_Inverse.identity();
  m_InverseTranspose.identity();
  m_BindWorld.identity();
  m_BindLocal.identity();
  m_BindInverse.identity();
  m_BindInverseTranspose.identity();
  m_JointOrient.set(0,0,0,1);
  m_RotationOrient.set(0,0,0,1);
  m_Transform.rotate.set(0,0,0,1.0f);
  m_BindTransform.rotate.set(0,0,0,1.0f);
  m_Transform.scale.set(1.0f,1.0f,1.0f);
  m_BindTransform.scale.set(1.0f,1.0f,1.0f);
  m_Transform.translate.set(0,0,0);
  m_BindTransform.translate.set(0,0,0);
  m_RotatePivot.set(0,0,0);
  m_ScalePivot.set(0,0,0);
  m_RotatePivotTranslate.set(0,0,0);
  m_ScalePivotTranslate.set(0,0,0);
}

//----------------------------------------------------------------------------------------------------------------------
void XBone::SetShearing(const XVector3& shear)
{
  m_Shearing = shear;
}

//----------------------------------------------------------------------------------------------------------------------
void XBone::SetShearing(const XReal x,const XReal y,const XReal z)
{
  m_Shearing.set(x,y,z);
}

//----------------------------------------------------------------------------------------------------------------------
void  XBone::SetRotatePivot(const XVector3& v)
{
  m_RotatePivot = v;
}

//----------------------------------------------------------------------------------------------------------------------
void  XBone::SetScalePivot(const XVector3& v)
{
  m_ScalePivot = v;
}

//----------------------------------------------------------------------------------------------------------------------
void  XBone::SetRotatePivotTranslate(const XVector3& v)
{
  m_RotatePivotTranslate = v;
}

//----------------------------------------------------------------------------------------------------------------------
void  XBone::SetScalePivotTranslate(const XVector3& v)
{
  m_ScalePivotTranslate = v;
}

//----------------------------------------------------------------------------------------------------------------------
void XBone::SetRotationOrient(const XQuaternion& r)
{
  m_RotationOrient=r;
}

//----------------------------------------------------------------------------------------------------------------------
void XBone::SetRotation(const XQuaternion& r)
{
  m_Transform.rotate = r;
}

//----------------------------------------------------------------------------------------------------------------------
void XBone::SetTranslation(const XVector3& v)
{
  m_Transform.translate =v;
}

//----------------------------------------------------------------------------------------------------------------------
void XBone::SetScale(const XVector3& v)
{
  m_Transform.scale = v;
}

//----------------------------------------------------------------------------------------------------------------------
void XBone::SetWorld(const XMatrix& mat) 
{
  m_World = mat;
}

//----------------------------------------------------------------------------------------------------------------------
void XBone::SetToBindPose()
{
  DUMPER(XBone__SetToBindPose);
  m_Transform.scale = m_BindTransform.scale;
  m_Transform.rotate = m_BindTransform.rotate;
  m_Transform.translate = m_BindTransform.translate;
}

//----------------------------------------------------------------------------------------------------------------------
void XBone::UpdateBindPose()
{
  DUMPER(XBone__SetToBindPose);
  m_BindTransform.scale = m_Transform.scale;
  m_BindTransform.rotate = m_Transform.rotate;
  m_BindTransform.translate = m_Transform.translate;
  m_BindTransform.toMatrix(m_Local);
  m_BindTransform.toMatrix(m_BindLocal);
}

//----------------------------------------------------------------------------------------------------------------------
bool XBone::UnParent() 
{
  DUMPER(XBone__UnParent);
  XBone* p = GetParent();
  m_Parent=0;
  if(p)
  {
    XIndexList::iterator it = p->m_ChildBones.begin();
    for( ; it != p->m_ChildBones.end(); ++it )
    {
      if( *it == GetID() )
      {
        p->m_ChildBones.erase(it);
        return true;
      }
    }
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
std::ostream& write_limit_value(std::ostream& ofs,XReal value)
{
  union {
    XReal r;
    XU32 t;
  } temp;
  temp.r = value;
  switch(temp.t)
  {
  case 0x7fffffff:
    ofs << "+INF";
    break;
  case 0xffffffff:
    ofs << "-INF";
    break;
  default:
    ofs << value;
    break;
  }
  return ofs;
}

//----------------------------------------------------------------------------------------------------------------------
bool XBone::WriteChunk(std::ostream& ofs)
{
  DUMPER(XBone__WriteChunk);
  ofs << "\tvisible\t"
      << m_Visible << "\n";

  ofs << "\tscale_pivot\t"
      << m_ScalePivot.x << " "
      << m_ScalePivot.y << " "
      << m_ScalePivot.z << "\n";

  ofs << "\trotate_pivot\t"
      << m_RotatePivot.x << " "
      << m_RotatePivot.y << " "
      << m_RotatePivot.z << "\n";

  ofs << "\tmin_bound\t"
      << m_BoundingBoxMin.x << " "
      << m_BoundingBoxMin.y << " "
      << m_BoundingBoxMin.z << "\n";

  ofs << "\tmax_bound\t"
      << m_BoundingBoxMax.x << " "
      << m_BoundingBoxMax.y << " "
      << m_BoundingBoxMax.z << "\n";

  ofs << "\tscale\t"
      << m_BindTransform.scale.x << " "
      << m_BindTransform.scale.y << " "
      << m_BindTransform.scale.z << "\n";

  ofs << "\ttranslate\t"
      << m_BindTransform.translate.x << " "
      << m_BindTransform.translate.y << " "
      << m_BindTransform.translate.z << "\n";

  ofs << "\trotation_orient\t"
      << m_RotationOrient.x << " "
      << m_RotationOrient.y << " "
      << m_RotationOrient.z << " "
      << m_RotationOrient.w << "\n";

  ofs << "\trotation\t"
      << m_BindTransform.rotate.x << " "
      << m_BindTransform.rotate.y << " "
      << m_BindTransform.rotate.z << " "
      << m_BindTransform.rotate.w << "\n";

  ofs << "\tjoint_orient\t"
      << m_JointOrient.x << " "
      << m_JointOrient.y << " "
      << m_JointOrient.z << " "
      << m_JointOrient.w << "\n";

  ofs << "\tbind_pose_matrix\n"
      << "\t\t" << m_BindLocal.data[ 0] << " " << m_BindLocal.data[ 1] << " " << m_BindLocal.data[ 2] << " " << m_BindLocal.data[ 3] << "\n"
      << "\t\t" << m_BindLocal.data[ 4] << " " << m_BindLocal.data[ 5] << " " << m_BindLocal.data[ 6] << " " << m_BindLocal.data[ 7] << "\n"
      << "\t\t" << m_BindLocal.data[ 8] << " " << m_BindLocal.data[ 9] << " " << m_BindLocal.data[10] << " " << m_BindLocal.data[11] << "\n"
      << "\t\t" << m_BindLocal.data[12] << " " << m_BindLocal.data[13] << " " << m_BindLocal.data[14] << " " << m_BindLocal.data[15] << "\n";

  ofs << "\trotation_order ";
  switch(m_RotationOrder)
  {
  case kXYZ: ofs << "xyz\n"; break;
  case kXZY: ofs << "xzy\n"; break;
  case kYXZ: ofs << "yxz\n"; break;
  case kYZX: ofs << "yzx\n"; break;
  case kZXY: ofs << "zxy\n"; break;
  case kZYX: ofs << "zyx\n"; break;
  }

  ofs << "\tinherits_transform " << m_InheritsTransform << "\n";

  ofs << "\tshearing " 
      << m_Shearing.x << " "
      << m_Shearing.y << " "
      << m_Shearing.z << std::endl;

  ofs << "\trotate_pivot_translate\t"
      << m_RotatePivotTranslate.x << " "
      << m_RotatePivotTranslate.y << " "
      << m_RotatePivotTranslate.z << "\n";

  ofs << "\tscale_pivot_translate\t"
      << m_ScalePivotTranslate.x << " "
      << m_ScalePivotTranslate.y << " "
      << m_ScalePivotTranslate.z << "\n";

  ofs << "\ttranslate_limit_x ";
  write_limit_value(ofs,m_TranslateLimits[0]);
  ofs << " ";
  write_limit_value(ofs,m_TranslateLimits[1]);
  ofs << std::endl;

  ofs << "\ttranslate_limit_y ";
  write_limit_value(ofs,m_TranslateLimits[2]);
  ofs << " ";
  write_limit_value(ofs,m_TranslateLimits[3]);
  ofs << std::endl;

  ofs << "\ttranslate_limit_z ";
  write_limit_value(ofs,m_TranslateLimits[4]);
  ofs << " ";
  write_limit_value(ofs,m_TranslateLimits[5]);
  ofs << std::endl;


  ofs << "\trotate_limit_x ";
  write_limit_value(ofs,m_RotateLimits[0]);
  ofs << " ";
  write_limit_value(ofs,m_RotateLimits[1]);
  ofs << std::endl;

  ofs << "\trotate_limit_y ";
  write_limit_value(ofs,m_RotateLimits[2]);
  ofs << " ";
  write_limit_value(ofs,m_RotateLimits[3]);
  ofs << std::endl;

  ofs << "\trotate_limit_z ";
  write_limit_value(ofs,m_RotateLimits[4]);
  ofs << " ";
  write_limit_value(ofs,m_RotateLimits[5]);
  ofs << std::endl;


  ofs << "\tscale_limit_x ";
  write_limit_value(ofs,m_ScaleLimits[0]);
  ofs << " ";
  write_limit_value(ofs,m_ScaleLimits[1]);
  ofs << std::endl;

  ofs << "\tscale_limit_y ";
  write_limit_value(ofs,m_ScaleLimits[2]);
  ofs << " ";
  write_limit_value(ofs,m_ScaleLimits[3]);
  ofs << std::endl;

  ofs << "\tscale_limit_z ";
  write_limit_value(ofs,m_ScaleLimits[4]);
  ofs << " ";
  write_limit_value(ofs,m_ScaleLimits[5]);
  ofs << std::endl;

  ofs << "\tparent " << m_Parent << "\n";
  ofs << "\tchild_joints " << static_cast<XU32>(m_ChildBones.size()) << "\n";

  if(  m_ChildBones.size() )
  {
    ofs << "\t\t";
    XIndexList::iterator it = m_ChildBones.begin();
    for( ; it != m_ChildBones.end(); ++it )
    {
      ofs << *it << " ";
    }
    ofs << "\n";
  }
  ofs << "\tINSTANCE_LIST\n\t{\n";

  if(GetModel()->GetExportPolicy().IsEnabled(XExportPolicy::WriteShapes))
  {
    XInstanceList instances;
    if(GetObjectInstances(instances))
    {
      XInstanceList::iterator it = instances.begin();
      for( ; it != instances.end(); ++it )
      {
        ofs << *(*it);
      }
    }
  }
  ofs << "\t}\n";

  return ofs.good();
}

//----------------------------------------------------------------------------------------------------------------------
bool XBone::GetParents( XBoneList& bones ) const
{
  bones.clear();
  if(GetParent())
    bones.push_back(GetParent());
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool XBone::HasLimit(const Limit limit_type) const
{
  union 
  {
    XReal r;
    XU32 b;
  } 
  temp;
  temp.b=0;
  switch(limit_type)
  {
  case kScaleMaxX:
    temp.r = m_ScaleLimits[1];
    break;
  case kScaleMinX:
    temp.r = m_ScaleLimits[0];
    break;
  case kScaleMaxY:
    temp.r = m_ScaleLimits[3];
    break;
  case kScaleMinY:
    temp.r = m_ScaleLimits[2];
    break;
  case kScaleMaxZ:
    temp.r = m_ScaleLimits[5];
    break;
  case kScaleMinZ:
    temp.r = m_ScaleLimits[4];
    break;
  case kRotateMaxX:
    temp.r = m_ScaleLimits[1];
    break;
  case kRotateMinX:
    temp.r = m_RotateLimits[0];
    break;
  case kRotateMaxY:
    temp.r = m_RotateLimits[3];
    break;
  case kRotateMinY:
    temp.r = m_RotateLimits[2];
    break;
  case kRotateMaxZ:
    temp.r = m_RotateLimits[5];
    break;
  case kRotateMinZ:
    temp.r = m_RotateLimits[4];
    break;
  case kTranslateMaxX:
    temp.r = m_TranslateLimits[1];
    break;
  case kTranslateMinX:
    temp.r = m_TranslateLimits[0];
    break;
  case kTranslateMaxY:
    temp.r = m_TranslateLimits[3];
    break;
  case kTranslateMinY:
    temp.r = m_TranslateLimits[2];
    break;
  case kTranslateMaxZ:
    temp.r = m_TranslateLimits[5];
    break;
  case kTranslateMinZ:
    temp.r = m_TranslateLimits[4];
    break;
  default:
    XMD_ASSERT(0 && "Should never get here");
    break;
  }
  return (temp.b != 0x7fffffff) && (temp.b != 0xffffffff);
}

//----------------------------------------------------------------------------------------------------------------------
XReal XBone::GetLimitValue(const Limit limit_type) const
{
  XReal temp=0;
  switch(limit_type)
  {
  case kScaleMaxX:
    temp = m_ScaleLimits[1];
    break;
  case kScaleMinX:
    temp = m_ScaleLimits[0];
    break;
  case kScaleMaxY:
    temp = m_ScaleLimits[3];
    break;
  case kScaleMinY:
    temp = m_ScaleLimits[2];
    break;
  case kScaleMaxZ:
    temp = m_ScaleLimits[5];
    break;
  case kScaleMinZ:
    temp = m_ScaleLimits[4];
    break;
  case kRotateMaxX:
    temp = m_ScaleLimits[1];
    break;
  case kRotateMinX:
    temp = m_RotateLimits[0];
    break;
  case kRotateMaxY:
    temp = m_RotateLimits[3];
    break;
  case kRotateMinY:
    temp = m_RotateLimits[2];
    break;
  case kRotateMaxZ:
    temp = m_RotateLimits[5];
    break;
  case kRotateMinZ:
    temp = m_RotateLimits[4];
    break;
  case kTranslateMaxX:
    temp = m_TranslateLimits[1];
    break;
  case kTranslateMinX:
    temp = m_TranslateLimits[0];
    break;
  case kTranslateMaxY:
    temp = m_TranslateLimits[3];
    break;
  case kTranslateMinY:
    temp = m_TranslateLimits[2];
    break;
  case kTranslateMaxZ:
    temp = m_TranslateLimits[5];
    break;
  case kTranslateMinZ:
    temp = m_TranslateLimits[4];
    break;
  default:
    XMD_ASSERT(0 && "Should never get here");
    break;
  }
  return temp;
}

//----------------------------------------------------------------------------------------------------------------------
void XBone::SetLimit(const Limit limit_type,const XReal limit)
{
  switch(limit_type)
  {
  case kScaleMaxX:
    m_ScaleLimits[1] = limit;
    break;
  case kScaleMinX:
    m_ScaleLimits[0] = limit;
    break;
  case kScaleMaxY:
    m_ScaleLimits[3] = limit;
    break;
  case kScaleMinY:
    m_ScaleLimits[2] = limit;
    break;
  case kScaleMaxZ:
    m_ScaleLimits[5] = limit;
    break;
  case kScaleMinZ:
    m_ScaleLimits[4] = limit;
    break;
  case kRotateMaxX:
    m_ScaleLimits[1] = limit;
    break;
  case kRotateMinX:
    m_RotateLimits[0] = limit;
    break;
  case kRotateMaxY:
    m_RotateLimits[3] = limit;
    break;
  case kRotateMinY:
    m_RotateLimits[2] = limit;
    break;
  case kRotateMaxZ:
    m_RotateLimits[5] = limit;
    break;
  case kRotateMinZ:
    m_RotateLimits[4] = limit;
    break;
  case kTranslateMaxX:
    m_TranslateLimits[1] = limit;
    break;
  case kTranslateMinX:
    m_TranslateLimits[0] = limit;
    break;
  case kTranslateMaxY:
    m_TranslateLimits[3] = limit;
    break;
  case kTranslateMinY:
    m_TranslateLimits[2] = limit;
    break;
  case kTranslateMaxZ:
    m_TranslateLimits[5] = limit;
    break;
  case kTranslateMinZ:
    m_TranslateLimits[4] = limit;
    break;
  default:
    XMD_ASSERT(0 && "Should never get here");
    break;
  }
}

//----------------------------------------------------------------------------------------------------------------------
void XBone::DisableLimit(const Limit limit_type)
{
  union {
    XReal value;
    XU32 limit;
  } MinLimit;
  union {
    XReal value;
    XU32 limit;
  } MaxLimit;
  MinLimit.limit = 0xffffffff;
  MaxLimit.limit = 0x7fffffff;

  switch(limit_type)
  {
  case kScaleMaxX:
    m_ScaleLimits[1] = MaxLimit.value;
    break;
  case kScaleMinX:
    m_ScaleLimits[0] = MinLimit.value;
    break;
  case kScaleMaxY:
    m_ScaleLimits[3] = MaxLimit.value;
    break;
  case kScaleMinY:
    m_ScaleLimits[2] = MinLimit.value;
    break;
  case kScaleMaxZ:
    m_ScaleLimits[5] = MaxLimit.value;
    break;
  case kScaleMinZ:
    m_ScaleLimits[4] = MinLimit.value;
    break;
  case kRotateMaxX:
    m_ScaleLimits[1] = MaxLimit.value;
    break;
  case kRotateMinX:
    m_RotateLimits[0] = MinLimit.value;
    break;
  case kRotateMaxY:
    m_RotateLimits[3] = MaxLimit.value;
    break;
  case kRotateMinY:
    m_RotateLimits[2] = MinLimit.value;
    break;
  case kRotateMaxZ:
    m_RotateLimits[5] = MaxLimit.value;
    break;
  case kRotateMinZ:
    m_RotateLimits[4] = MinLimit.value;
    break;
  case kTranslateMaxX:
    m_TranslateLimits[1] = MaxLimit.value;
    break;
  case kTranslateMinX:
    m_TranslateLimits[0] = MinLimit.value;
    break;
  case kTranslateMaxY:
    m_TranslateLimits[3] = MaxLimit.value;
    break;
  case kTranslateMinY:
    m_TranslateLimits[2] = MinLimit.value;
    break;
  case kTranslateMaxZ:
    m_TranslateLimits[5] = MaxLimit.value;
    break;
  case kTranslateMinZ:
    m_TranslateLimits[4] = MinLimit.value;
    break;
  default:
    XMD_ASSERT(0 && "Should never get here");
    break;
  }
}

//----------------------------------------------------------------------------------------------------------------------
XBone::RotationOrder XBone::GetRotationOrder() const
{
  return m_RotationOrder;
}

//----------------------------------------------------------------------------------------------------------------------
void XBone::SetRotationOrder(const RotationOrder ro)
{
  m_RotationOrder = ro;
}
}
