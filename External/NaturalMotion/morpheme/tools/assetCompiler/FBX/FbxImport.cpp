// Copyright (c) 2010 NaturalMotion.  All Rights Reserved.
// Not to be copied, adapted, modified, used, distributed, sold,
// licensed or commercially exploited in any manner without the
// written consent of NaturalMotion.
//
// All non public elements of this software are the confidential
// information of NaturalMotion and may not be disclosed to any
// person nor used for any purpose not expressly approved by
// NaturalMotion in writing.

//----------------------------------------------------------------------------------------------------------------------
#include "FBXPlugin.h"
#include "FBXUtil.h"
#include <map>
#include "XM2/EulerAngles.h"
#include "XMD/PolygonMesh.h"
#include "XMD/NurbsSurface.h"
#include "XMD/Light.h"
#include "XMD/Locator.h"
#include "XMD/Phong.h"
#include "XMD/TexPlacement2D.h"
#include "XMD/AmbientLight.h"
#include "XMD/DirectionalLight.h"
#include "XMD/PointLight.h"
#include "XMD/SpotLight.h"
#include "XMD/Camera.h"
#include "XMD/SkinCluster.h"
#include "XMD/Model.h"
#include "XMD/Anisotropic.h"
#include <math.h>
#include "NMPlatform/NMPlatform.h"
#include "NMPlatform/NMTimer.h"

namespace FBX_XMD_PLUGIN
{

using XM2::XEulerAngles;
using XM2::XQuaternion;
using XM2::XVector3;
using XM2::XReal;
using XM2::XVector3Array;
using XM2::XRealArray;
using XM2::XColour;
using XM2::XQuaternionArray;
using XM2::XVector2Array;
using XM2::XRotationOrder;

using XMD::XBone;
using XMD::XBase;
using XMD::XJoint;
using XMD::XFn;
using XMD::XUserData;
using XMD::XString;
using XMD::XModel;
using XMD::XMaterial;
using XMD::XBasicLogger;
using XMD::XFileLogger;
using XMD::XAnimCycle;
using XMD::XGlobal;
using XMD::XLocator;
using XMD::XInstance;
using XMD::XNurbsSurface;
using XMD::XCamera;
using XMD::XPointLight;
using XMD::XDirectionalLight;
using XMD::XSpotLight;
using XMD::XSkinCluster;
using XMD::XGeometry;
using XMD::XDeformerList;
using XMD::XIndexList;
using XMD::XSampledKeys;
using XMD::XPolygonMesh;
using XMD::XU32Array;
using XMD::XVertexSet;
using XMD::XIndexSet;

//----------------------------------------------------------------------------------------------------------------------
FBXSDK_NAMESPACE::FbxManager* g_manager = 0;

//----------------------------------------------------------------------------------------------------------------------
inline float toRad(const float degrees)
{
  return (degrees / 180.0f) * XM2::XM2_PI;
}

//----------------------------------------------------------------------------------------------------------------------
std::string FixupName(const char* n)
{
  std::string s;
  while (*n)
  {
    if (*n == ':')
    {
      s.clear();
      s = "";
    }
    else if (*n == ' ' || *n == '\t')
    {
      s += "_";
    }
    else s += *n;
    ++n;
  }
  return s;
}

//----------------------------------------------------------------------------------------------------------------------
struct XMatInfo : public XUserData {
  XMatInfo(XBase* tpr, const XString& data_id) : XUserData(tpr, data_id) {
    mMat = 0;
  }
  virtual void Release() { delete this; }
  FbxSurfaceMaterial* mMat;
};

//----------------------------------------------------------------------------------------------------------------------
static bool generateAnimation(
  FBXSDK_NAMESPACE::FbxAnimEvaluator& evaluator,
  std::map<FBXSDK_NAMESPACE::FbxNode*, XMD::XBone*>& nmap,
  XMD::XAnimCycle& cycle, 
  float scale);

//----------------------------------------------------------------------------------------------------------------------
FBXFileTranslator::FBXFileTranslator() :
  XMD::XFileTranslator(),
  m_scale(1.0f)
{
  // import and export everything by default
  m_importOptions.m_materials = true;
  m_importOptions.m_textures = true;
  m_importOptions.m_links = true;
  m_importOptions.m_shapes = true;
  m_importOptions.m_gobos = true;
  m_importOptions.m_animations = true;
  m_importOptions.m_geometry = true;
  m_importOptions.m_globalSettings = true;

  m_exportOptions = m_importOptions;
}

//----------------------------------------------------------------------------------------------------------------------
FBXFileTranslator::~FBXFileTranslator()
{
}

//----------------------------------------------------------------------------------------------------------------------
bool FBXFileTranslator::Import(const XMD::XChar* filename, XModel& m)
{
#if FBX_PLUGIN_ENABLE_PROFILING != 0
  NMP::Timer profileTimer;
#endif

  std::string sfilename = filename;

#ifdef _DEBUG
  XBasicLogger* oldLogger = XGlobal::GetLogger();
  XFileLogger* fileLogger = new XFileLogger("FBXFileTranslator_Log.txt");
  XGlobal::SetLogger(fileLogger);
#endif//_DEBUG

  m_model = &m;

  std::vector< XMaterial* > materials;

  FBX_PLUGIN_PROFILE_TIMER_START(profileTimer);
  FbxImporter* importer = FbxImporter::Create(g_manager, "");
  FBX_PLUGIN_PROFILE_TIMER_STOP(profileTimer, "FbxImporter::Create");

  FBX_PLUGIN_PROFILE_TIMER_START(profileTimer);
  m_scene = FbxScene::Create(g_manager, "");
  FBX_PLUGIN_PROFILE_TIMER_STOP(profileTimer, "FbxScene::Create");

  bool retValue = false;

  FBX_PLUGIN_PROFILE_TIMER_START(profileTimer);
  bool initialised = importer->Initialize(sfilename.c_str());
  FBX_PLUGIN_PROFILE_TIMER_STOP(profileTimer, "FbxImporter::Initialize");

  if (initialised)
  {
    FBX_PLUGIN_PROFILE_TIMER_START(profileTimer);
    FbxIOSettings* ios = FbxIOSettings::Create(g_manager, IOSROOT);
    FBX_PLUGIN_PROFILE_TIMER_STOP(profileTimer, "FbxIOSettings::Create");

    FBX_PLUGIN_PROFILE_TIMER_START(profileTimer);
    ios->SetBoolProp(IMP_FBX_MATERIAL, m_importOptions.m_materials);
    ios->SetBoolProp(IMP_FBX_TEXTURE, m_importOptions.m_textures);
    ios->SetBoolProp(IMP_FBX_LINK, m_importOptions.m_links);
    ios->SetBoolProp(IMP_FBX_SHAPE, m_importOptions.m_shapes);
    ios->SetBoolProp(IMP_FBX_MODEL_COUNT, m_importOptions.m_geometry);
    ios->SetBoolProp(IMP_FBX_GOBO, m_importOptions.m_gobos);
    ios->SetBoolProp(IMP_FBX_ANIMATION, m_importOptions.m_animations);
    ios->SetBoolProp(IMP_FBX_GLOBAL_SETTINGS, m_importOptions.m_globalSettings);

    g_manager->SetIOSettings(ios);
    FBX_PLUGIN_PROFILE_TIMER_STOP(profileTimer, "configure IO Settings");

    FBX_PLUGIN_PROFILE_TIMER_START(profileTimer);
    bool imported = importer->Import(m_scene);
    FBX_PLUGIN_PROFILE_TIMER_STOP(profileTimer, "FbxImporter::Import");

    if (imported)
    {
      // used to create a mapping between the incoming FBX bones, and the XMD bones for future lookup
      std::map<FbxNode*, XBone*> nodeMap;

      FBX_PLUGIN_PROFILE_TIMER_START(profileTimer);
      // Traverse the scene importing all bones
      // don't process the root node, only its children
      FbxNode* root = m_scene->GetRootNode();
      for (int32_t childIndex = 0, childCount =  root->GetChildCount(false); childIndex < childCount; ++childIndex)
      {
        generateBones(nodeMap, m_scene->GetEvaluator(), root->GetChild(childIndex), materials);
      }
      FBX_PLUGIN_PROFILE_TIMER_STOP(profileTimer, "generateBones");

      FBX_PLUGIN_PROFILE_TIMER_START(profileTimer);
      std::map<FbxCluster*, XJoint*> jointMap;
      generateDeformers(m_scene->GetRootNode(), &jointMap);
      FBX_PLUGIN_PROFILE_TIMER_STOP(profileTimer, "generateDeformers");

      FBX_PLUGIN_PROFILE_TIMER_START(profileTimer);
      SetToBindPose(jointMap);
      FBX_PLUGIN_PROFILE_TIMER_STOP(profileTimer, "SetToBindPose");

      // Extract all animation takes
      FbxAnimEvaluator* evaluator = m_scene->GetEvaluator();

      FBX_PLUGIN_PROFILE_TIMER_START(profileTimer);
      FbxGlobalSettings& globalSettings = m_scene->GetGlobalSettings();
      FbxTime::EMode globalTimeMode = globalSettings.GetTimeMode();

      FbxTime fbxFramerate;
      double framerate = FbxTime::GetFrameRate(globalTimeMode);
      fbxFramerate.SetSecondDouble(framerate);

      FbxTime bakePeriod;
      bakePeriod.SetSecondDouble(1.0 / framerate);
      FBX_PLUGIN_PROFILE_TIMER_STOP(profileTimer, "GetFrameRate");

      FBX_PLUGIN_PROFILE_TIMER_START(profileTimer);
      FbxArray<FbxString*> takeNameArray;
      m_scene->FillAnimStackNameArray(takeNameArray);
      FBX_PLUGIN_PROFILE_TIMER_STOP(profileTimer, "FbxScene::FillAnimStackNameArray");

      for (int32_t stackIndex = 0, stackCount = m_scene->GetSrcObjectCount<FbxAnimStack>();
           stackIndex < stackCount;
           ++stackIndex)
      {
        FBX_PLUGIN_PROFILE_TIMER_START(profileTimer);

        FbxAnimStack* animStack = FbxCast<FbxAnimStack>(m_scene->GetSrcObject<FbxAnimStack>(stackIndex));
        

        FbxTimeSpan timeSpan = animStack->GetLocalTimeSpan();
        FbxTime lclStart = timeSpan.GetStart();
        FbxTime lclStop  = timeSpan.GetStop();

        // baking the stack first makes querying the scene using the evaluator much quicker
        animStack->BakeLayers(evaluator, lclStart, lclStop, bakePeriod);
        
        evaluator->SetContext(animStack);

        double startTime = lclStart.GetSecondDouble() * framerate;
        double stopTime  = lclStop.GetSecondDouble() * framerate;

        const uint32_t numFrames = static_cast<uint32_t>(floor(stopTime - startTime)) + 1;

        XAnimCycle* animCycle = m_model->CreateNode(XFn::AnimCycle)->HasFn<XAnimCycle>();
        animCycle->SetFrameTimes(static_cast<XReal>(startTime), static_cast<XReal>(stopTime), numFrames);
        animCycle->SetFrameRate(static_cast<XReal>(framerate));

        const char* stackName = stackIndex < takeNameArray.GetCount() ? takeNameArray.GetAt(stackIndex)->Buffer() : "Take";

        animCycle->SetName(FixupName(stackName).c_str());

        generateAnimation(*evaluator, nodeMap, *animCycle, m_scale);

        FBX_PLUGIN_PROFILE_TIMER_STOP(profileTimer, "generateAnimation");
      }
      retValue = true;
    }
  }

  FBX_PLUGIN_PROFILE_TIMER_START(profileTimer);
  m_scene->Destroy();
  FBX_PLUGIN_PROFILE_TIMER_STOP(profileTimer, "FbxScene::Destroy");
  m_scene = 0;
  FBX_PLUGIN_PROFILE_TIMER_START(profileTimer);
  importer->Destroy();
  FBX_PLUGIN_PROFILE_TIMER_STOP(profileTimer, "FbxImporter::Destroy");

#ifdef _DEBUG
  // restore the old logger and delete the file logger
  XGlobal::SetLogger(oldLogger);
  delete fileLogger;
#endif//_DEBUG

#if FBX_PLUGIN_ENABLE_PROFILING != 0
  profileTimer.printTotalSectionTime("FBXFileTranslator::Import");
#endif

  return retValue;
}

//----------------------------------------------------------------------------------------------------------------------
void FBXFileTranslator::SetImportOptions(const FBXIOOptions& i_options)
{
  m_importOptions = i_options;
}

//----------------------------------------------------------------------------------------------------------------------
//
// First pass, generate the bones from the FBX, and any geometry or other shapes
// that may be parented under a given xform.
//
//
bool FBXFileTranslator::generateBones(
  std::map<FbxNode*, XBone*>& mNodeMap, 
  FbxAnimEvaluator* evaluator,
  FbxNode* node, 
  std::vector<XMaterial*>& materials, 
  XBone* parent)
{
  XBone* joint;

  // check to see if this node has a skeleton chunk. If so, create an IkJoint. If
  // not, create a transform node. The difference here is that a joint uses the
  // orients in it's matrix calc, transforms use pivot info instead...

  FbxNodeAttribute* attr = node->GetNodeAttribute();
  if (attr && (attr->GetAttributeType() == FbxNodeAttribute::eSkeleton))
  {
    XJoint* skelJoint = m_model->CreateNode(XFn::Joint)->HasFn<XJoint>();
    joint = skelJoint;

    // set the segment scale flag appropriately
    FbxTransform::EInheritType transformType;
    node->GetTransformationInheritType(transformType);
    if (transformType != FbxTransform::eInheritRrSs)
    {
      skelJoint->SetSegmentScaleCompensate(false);
    }
  }
  else
  {
    joint = m_model->CreateNode(XFn::Bone)->HasFn<XBone>();
  }

  joint->SetName(FixupName(node->GetName()).c_str());

  // store map between FBX and XMD bone for later
  mNodeMap.insert(std::make_pair(node, joint));

  // grab local TM from transform
  const FbxMatrix& wsm = evaluator->GetNodeGlobalTransform(node);
  
  XM2::XMatrix matrix( (float)wsm.Get(0,0), (float)wsm.Get(0,1), (float)wsm.Get(0,2), (float)wsm.Get(0,3), 
                       (float)wsm.Get(1,0), (float)wsm.Get(1,1), (float)wsm.Get(1,2), (float)wsm.Get(1,3), 
                       (float)wsm.Get(2,0), (float)wsm.Get(2,1), (float)wsm.Get(2,2), (float)wsm.Get(2,3), 
                       (float)wsm.Get(3,0), (float)wsm.Get(3,1), (float)wsm.Get(3,2), (float)wsm.Get(3,3) );

  FbxNode* parent_tm = node->GetParent();

  const FbxMatrix& wsm2 = evaluator->GetNodeGlobalTransform(parent_tm);
  XM2::XMatrix matrix_parent( (float)wsm2.Get(0,0), (float)wsm2.Get(0,1), (float)wsm2.Get(0,2), (float)wsm2.Get(0,3), 
                              (float)wsm2.Get(1,0), (float)wsm2.Get(1,1), (float)wsm2.Get(1,2), (float)wsm2.Get(1,3), 
                              (float)wsm2.Get(2,0), (float)wsm2.Get(2,1), (float)wsm2.Get(2,2), (float)wsm2.Get(2,3), 
                              (float)wsm2.Get(3,0), (float)wsm2.Get(3,1), (float)wsm2.Get(3,2), (float)wsm2.Get(3,3) );
  
  matrix_parent.invert();
  matrix.xAxis().normalise();
  matrix.yAxis().normalise();
  matrix.zAxis().normalise();
  XM2::XMatrix temp(matrix);
  matrix.mul(matrix_parent, temp);

  matrix.xAxis().normalise();
  matrix.yAxis().normalise();
  matrix.zAxis().normalise();

  FbxVector4 lcl_scale     = evaluator->GetNodeLocalScaling(node);
  FbxVector4 jointOrient   = node->GetPreRotation(FbxNode::eSourcePivot);
  FbxVector4 rotAxes       = node->GetPostRotation(FbxNode::eSourcePivot);

  // lets see if this works then...  (god knows what those Euler values are supposed to be)
  FbxQuaternion q_joint_orient;
  FbxQuaternion q_rot_axes;
  q_joint_orient.ComposeSphericalXYZ(jointOrient);
  q_rot_axes.ComposeSphericalXYZ(rotAxes);

  // set the pre/post rotations in XMD
  XM2::XQuaternion xmd_jo((float)q_joint_orient[0], (float)q_joint_orient[1], (float)q_joint_orient[2], (float)q_joint_orient[3]);
  XM2::XQuaternion xmd_ro((float)q_rot_axes[0], (float)q_rot_axes[1], (float)q_rot_axes[2], (float)q_rot_axes[3]);
  joint->SetJointOrient(xmd_jo);
  joint->SetRotationOrient(xmd_ro);

  // includes the joint orient and rotation axes
  XM2::XQuaternion xmd_full_rotation(matrix);

  // now we need to remove the effect of the pre/post rotations
  xmd_jo.invert();
  xmd_ro.invert();
  XM2::XQuaternion rotation_only;
  rotation_only.mul(xmd_ro, xmd_full_rotation);
  rotation_only.mul(xmd_jo);
  joint->SetRotation(rotation_only);

  // set translation and scale
  joint->SetTranslation( XM2::XVector3(matrix.wAxis().x * m_scale, matrix.wAxis().y * m_scale, matrix.wAxis().z * m_scale) );
  joint->SetScale( XM2::XVector3((float)lcl_scale[0], (float)lcl_scale[1], (float)lcl_scale[2]) );

  // update the bind pose
  joint->UpdateBindPose();

  if (parent)
  {
    parent->AddChild(joint);
  }

  unsigned nKids = node->GetChildCount();
  for (unsigned i = 0; i != nKids; ++i)
  {
    if (!generateBones(mNodeMap, evaluator, node->GetChild(i), materials, joint))
      return false;
  }
  return generateGeometry(node, joint, materials);
}

//----------------------------------------------------------------------------------------------------------------------
bool FBXFileTranslator::generateGeometry(FbxNode* node, XBone* parent, std::vector<XMaterial*>& materials)
{
  if (node && node->GetNodeAttribute())
  {
    const FbxNodeAttribute::EType nodeAttributeType = node->GetNodeAttribute()->GetAttributeType();
    switch (nodeAttributeType)
    {
      //
    case FbxNodeAttribute::eNull:
    {
      FbxNull* null = node->GetNull();
      XLocator* xlocator = m_model->CreateNode(XFn::Locator)->HasFn<XLocator>();
      xlocator->SetName((FixupName(node->GetName()) + "Shape").c_str());
      XInstance* instance = m_model->CreateGeometryInstance(parent, xlocator);
    }
    break;

    // this is a bone
    case FbxNodeAttribute::eSkeleton:
    {
      FbxSkeleton* skelly = node->GetSkeleton();
    }
    break;

    // create a mesh
    case FbxNodeAttribute::eMesh:
    {
      CreateMesh(node, parent, materials);
    }
    break;

    // create a nurbs surface
    case FbxNodeAttribute::eNurbs:
    {
      FbxNurbs* nurb = node->GetNurbs();

      unsigned int nCvsU = nurb->GetVCount();
      unsigned int nCvsV = nurb->GetUCount();
      unsigned int nActualCvsU = nurb->GetVCount();
      unsigned int nActualCvsV = nurb->GetUCount();

      bool isup = false;
      bool isvp = false;

      if (nurb->GetNurbsVType() == FbxNurbs::ePeriodic)
      {
        isup = true;
        nActualCvsU += nurb->GetVOrder() - 1;
      }

      if (nurb->GetNurbsUType() == FbxNurbs::ePeriodic)
      {
        isvp = true;
        nActualCvsV += nurb->GetUOrder() - 1;
      }

      XVector3Array points;
      points.resize(nActualCvsU * nActualCvsV);
      FbxVector4* pnts = nurb->GetControlPoints();

      XVector3Array::iterator itp = points.begin();

      {
        for (unsigned ii = 0; ii != nActualCvsU; ++ii)
        {
          unsigned iindex = ii % nCvsU;
          for (unsigned jj = 0; jj != nActualCvsV; ++jj, ++itp)
          {
            unsigned jindex = jj % nCvsV;

            FbxVector4* vert = pnts + iindex * nCvsV + jindex ;
            itp->set((XReal)(*vert)[0]*m_scale, (XReal)(*vert)[1]*m_scale, (XReal)(*vert)[2]*m_scale);
          }
        }
      }

      XNurbsSurface* xnurb = m_model->CreateNode(XFn::NurbsSurface)->HasFn<XNurbsSurface>();
      xnurb->SetName((FixupName(node->GetName()) + "Shape").c_str());
      xnurb->SetPoints(points);

      XRealArray knotsU;
      XRealArray knotsV;

      for (unsigned i = 0; i != nurb->GetUKnotCount(); ++i)
      {
        knotsU.push_back((XReal)nurb->GetUKnotVector()[i]);
      }

      for (unsigned i = 0; i != nurb->GetVKnotCount(); ++i)
      {
        knotsV.push_back((XReal)nurb->GetVKnotVector()[i]);
      }

      xnurb->SetDegreeU(nurb->GetVOrder() - 1);
      xnurb->SetDegreeV(nurb->GetUOrder() - 1);
      xnurb->SetKnotsV(knotsU);
      xnurb->SetKnotsU(knotsV);

      unsigned ii = 0;
      switch (nurb->GetNurbsUType())
      {
      case FbxNurbs::ePeriodic:
        ii = 2;
        break;
      case FbxNurbs::eClosed:
        ii = 1;
        break;
      default:
        break;
      }
      xnurb->SetTypeInV(ii);

      ii = 0;
      switch (nurb->GetNurbsVType())
      {
      case FbxNurbs::ePeriodic:
        ii = 2;
        break;
      case FbxNurbs::eClosed:
        ii = 1;
        break;
      default:
        break;
      }
      xnurb->SetTypeInU(ii);

      xnurb->SetNumCvsInV(nurb->GetUCount() + (isvp ? nurb->GetUOrder() - 1 : 0));
      xnurb->SetNumCvsInU(nurb->GetVCount() + (isup ? nurb->GetVOrder() - 1 : 0));

      XInstance* instance = m_model->CreateGeometryInstance(parent, xnurb);

      FbxLayer* layer = nurb->GetLayer(0);
      if (layer)
      {
        FbxLayerElementMaterial* mats =  layer->GetMaterials();
      }
      NMP_VERIFY_FAIL("Error generating nurbs geometry");
      parent->AddInstance(instance);
    }
    break;
    case FbxNodeAttribute::ePatch:
    {
      FbxPatch* patch = node->GetPatch();
    }
    break;

    // create a camera
    case FbxNodeAttribute::eCamera:
    {
      FbxCamera* camera = node->GetCamera();

      XCamera* cam = m_model->CreateNode(XFn::Camera)->HasFn<XCamera>();
      cam->SetName((FixupName(node->GetName()) + "Shape").c_str());

      cam->SetVerticalAspect((XReal)camera->AspectWidth.Get());
      cam->SetHorizontalAspect((XReal)camera->AspectHeight.Get());
      cam->SetFar((XReal)camera->GetFarPlane());
      cam->SetNear((XReal)camera->GetNearPlane());

      XInstance* instance = m_model->CreateGeometryInstance(parent, cam);
      parent->AddInstance(instance);
    }
    break;

    // erm. dunno what to do with this. I might add this into XMD some when.
    case FbxNodeAttribute::eCameraSwitcher:
    {
      FbxCameraSwitcher* cswitch = node->GetCameraSwitcher();
    }
    break;

    // create a light node
    case FbxNodeAttribute::eLight:
    {
      FbxLight* light = node->GetLight();

      FbxDouble3 color;
      color = light->Color.Get();
      float intensity = (XReal)light->Intensity.Get();
      float cangle = (XReal)light->OuterAngle.Get();
      float fog = (XReal)light->Fog.Get();

      switch (light->LightType.Get())
      {
      case FbxLight::ePoint :
      {
        XPointLight* pl = m_model->CreateNode(XFn::PointLight)->HasFn<XPointLight>();

        pl->SetName((FixupName(node->GetName()) + "Shape").c_str());
        pl->SetIntensity((XReal)intensity * 0.01f);
        pl->SetColour(XColour((XReal)color[0], (XReal)color[1], (XReal)color[2]));

        XInstance* instance = m_model->CreateGeometryInstance(parent, pl);
        parent->AddInstance(instance);
      }
      break;
      case FbxLight::eDirectional :
      {
        XDirectionalLight* pl = m_model->CreateNode(XFn::DirectionalLight)->HasFn<XDirectionalLight>();

        pl->SetName((FixupName(node->GetName()) + "Shape").c_str());
        pl->SetIntensity((XReal)intensity * 0.01f);
        pl->SetColour(XColour((XReal)color[0], (XReal)color[1], (XReal)color[2]));

        XInstance* instance = m_model->CreateGeometryInstance(parent, pl);
        parent->AddInstance(instance);

      }
      break;
      case FbxLight::eSpot :
      {
        XSpotLight* pl = m_model->CreateNode(XFn::SpotLight)->HasFn<XSpotLight>();

        pl->SetName((FixupName(node->GetName()) + "Shape").c_str());
        pl->SetIntensity((XReal)intensity * 0.01f);
        pl->SetColour(XColour((XReal)color[0], (XReal)color[1], (XReal)color[2]));
        pl->SetConeAngle((XReal)cangle*(3.14159f / 180.0f));

        XInstance* instance = m_model->CreateGeometryInstance(parent, pl);
        parent->AddInstance(instance);
      }
      break;
      default:
        break;
      }
    }
    break;

    // ?? should this be a locater node?
    case FbxNodeAttribute::eOpticalReference:
    {
      FbxOpticalReference* oref = node->GetOpticalReference();
    }
    break;

    // ?? should this be a locater node?
    case FbxNodeAttribute::eOpticalMarker:
      break;

    default:
      break;
    }
    return true;
  }
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool FBXFileTranslator::generateDeformers(FbxNode* node, std::map<FbxCluster*, XJoint*>* jointMap)
{
  FbxGeometry* geom = node->GetGeometry();
  if (geom)
  {
    // check to see if the FBX node has one or more links. A link in FBX
    // speak is basically a joint cluster in my terms. This is basically
    // a structure that holds the joint,  and a set of weights for vertices
    // influenced by those bones.

    // anyhow, if we have a link, create a skin cluster node. I'm going to
    // import this as a smooth skin, rather than using joint clusters....
    //
    // It might also be wise to add an additional check here to see if the
    // link count == 1. If it does, then basically we have a cluster.
    //
    FbxSkin* fbxSkin = (FbxSkin*)geom->GetDeformer(0, FbxDeformer::eSkin);
    if (fbxSkin)
    {
      // make sure we can find the geometry shape first...
      XBase* base = m_model->FindNode(FixupName((XString(node->GetName()) + "Shape").c_str()));
      if (!base)
        return false;

      // create the skin
      XSkinCluster* skin = m_model->CreateNode(XFn::SkinCluster)->HasFn<XSkinCluster>();
      XGeometry* xgeom = base->HasFn<XGeometry>();

      skin->SetAffected(xgeom);

      XDeformerList defs;
      defs.push_back(skin);
      xgeom->SetDeformerQueue(defs);

      // now basically iterate through each link, and add it's weights
      // to the skin cluster.
      //
      const int clusterCount = fbxSkin->GetClusterCount();
      for (unsigned int clusterIndex = 0; clusterIndex != clusterCount; ++clusterIndex)
      {
        FbxCluster* link = fbxSkin->GetCluster(clusterIndex);
        if (link)
        {
          FbxNode* lBone = link->GetLink();
          if(!lBone)
            continue;

          XBase* infBone = m_model->FindNode(FixupName(lBone->GetName()).c_str());
          if(!infBone)
            continue;

          if (infBone->HasFn<XJoint>())
          {
            jointMap->insert(std::make_pair(link, (XJoint*)infBone));
          }

          unsigned infID = skin->AddInfluence(infBone->HasFn<XBone>());
          int* inds = link->GetControlPointIndices ();
          double* ws = link->GetControlPointWeights ();
          unsigned nVerts = link->GetControlPointIndicesCount ();

          // set the weight of this bone on each influenced vertex
          for (unsigned vertIndex = 0; vertIndex != nVerts; ++vertIndex)
          {
            skin->AddWeight(inds[vertIndex], infID, (XReal)ws[vertIndex]);
          }
        }
      }

      // make sure the weights are valid
      XIndexList list;
      bool valid = skin->AreWeightsValid(list);
      if (!valid)
      {
        skin->NormaliseWeights(list);
      }
    }
  }

  unsigned nKids = node->GetChildCount();
  for (unsigned i = 0; i != nKids; ++i)
  {
    if (!generateDeformers(node->GetChild(i), jointMap))
      return false;
  }

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool FBXFileTranslator::SetToBindPose(const std::map<FbxCluster*, XJoint*>& jointMap)
{
  // going to update the matrices. 
  m_model->BuildMatrices();

  // no joint/cluster mappings to process?
  if (!m_importOptions.m_geometry || !jointMap.size())
  {
    return false;
  }

  std::vector<FbxAMatrix> worldJointMatrixVec;
  worldJointMatrixVec.resize(jointMap.size());

  // retrieve the world bind pose matrix of each joint
  size_t matrixIndex = 0;
  for (std::map<FbxCluster*, XJoint*>::const_iterator itMappings = jointMap.begin(); itMappings != jointMap.end(); ++itMappings, ++matrixIndex)
  {
    FbxCluster& fbxCluster = *(*itMappings).first;
    fbxCluster.GetTransformLinkMatrix(worldJointMatrixVec[matrixIndex]);
  }

  // create the inverse of all matrices in preparation for moving children to local space
  std::vector<FbxAMatrix> inverseWorldJointMatrixVec;
  inverseWorldJointMatrixVec.resize(jointMap.size());
  for (matrixIndex = 0; matrixIndex < inverseWorldJointMatrixVec.size(); ++matrixIndex)
  {
    inverseWorldJointMatrixVec[matrixIndex] = worldJointMatrixVec[matrixIndex].Inverse();
  }

  // turn the global matrices into local matrices by multiplying by the inverse parent
  matrixIndex = 0;
  std::vector<FbxAMatrix> localJointMatrixVec;
  localJointMatrixVec.resize(jointMap.size());
  for (std::map<FbxCluster*, XJoint*>::const_iterator itMappings = jointMap.begin(); itMappings != jointMap.end(); ++itMappings, ++matrixIndex)
  {
    XBase* parentBase = (*itMappings).second->GetParent();
    XJoint* parentJoint = 0;

    // set the local matrix as its world matrix in case no parent matrix is found
    localJointMatrixVec[matrixIndex] = worldJointMatrixVec[matrixIndex];

    // if no parent exists then it must be the root bone and so its local matrix is its world matrix
    if (!parentBase || (parentJoint = parentBase->HasFn<XJoint>()) == 0)
    {
      continue;
    }

    int parentMatrixIndex = 0;
    // find the parent
    for (std::map<FbxCluster*, XJoint*>::const_iterator itMappings = jointMap.begin(); itMappings != jointMap.end(); ++itMappings, ++parentMatrixIndex)
    {
      if ((*itMappings).second == parentJoint)
      {
        localJointMatrixVec[matrixIndex] = inverseWorldJointMatrixVec[parentMatrixIndex] * worldJointMatrixVec[matrixIndex];
        break;
      }
    }

    // if we didn't find the parent TM
    if (parentMatrixIndex == (int)jointMap.size())
    {
      const XM2::XMatrix& wsm = parentJoint->GetWorld();
      FbxAMatrix wmatrix;
      for (uint32_t i=0; i<4; ++i)
      {
        for (uint32_t j=0; j<4; ++j)
        {
          uint32_t index = (i<<2) | j;
          wmatrix.mData[i].mData[j] = wsm.data[index];
        }
      }
      wmatrix = wmatrix.Inverse();
      localJointMatrixVec[matrixIndex] = wmatrix * worldJointMatrixVec[matrixIndex];
    }
  }

  // update the transforms for each joint
  matrixIndex = 0;
  for (std::map<FbxCluster*, XJoint*>::const_iterator itMappings = jointMap.begin(); itMappings != jointMap.end(); ++itMappings, ++matrixIndex)
  {
    FbxAMatrix& localMatrix = localJointMatrixVec[matrixIndex];

    // convert the fbx transform values to XMD equivalents
    XVector3 scale;
    FbxVector4 fbxScale = localMatrix.GetS();
    ConvertVector<FbxVector4, XVector3, 3>(fbxScale, scale);

    XVector3 translate;
    FbxVector4 fbxTranslate = localMatrix.GetT();
    ConvertVector<FbxVector4, XVector3, 3>(fbxTranslate, translate);

    XQuaternion rot;
    FbxQuaternion fbxRot = localMatrix.GetQ();
    ConvertVector<FbxQuaternion, XQuaternion, 4>(fbxRot, rot);

    // extract the original rotation quaternion from the equation below as the rotation,
    // we received was combined with the pre and post rotation
    // XMatrix _R = (GetRotationOrient() * GetRotation() * GetJointOrient()).AsMatrix();

    // can't successfully extract local rotation so instead remove the orients as the local,
    // rotation now contains these. Note this code doesn't get called for animations because,
    // of the check at the start of the function
    XJoint* xJoint = (*itMappings).second;

    XM2::XQuaternion xmd_jo = xJoint->GetJointOrient();
    XM2::XQuaternion xmd_ro = xJoint->GetRotationOrient();

    // now we need to remove the effect of the pre/post rotations
    xmd_jo.invert();
    xmd_ro.invert();
    XM2::XQuaternion rotation_only;
    rotation_only.mul(xmd_ro, rot);
    rotation_only.mul(xmd_jo);
    xJoint->SetRotation(rotation_only);

    // update the joint with the bind pose transforms
    xJoint->SetTranslation(translate);
    xJoint->SetScale(scale);
    xJoint->UpdateBindPose();
  }

  return true;
}

#ifdef _DEBUG
//----------------------------------------------------------------------------------------------------------------------
void FBXDumpNodeProperties(FbxObject* nodePtr)
{
  std::string info;
  info = "NodeName: ";
  info += std::string(nodePtr->GetName());
  info += "\n";
  info += "\n";

#if 0
  // dump all node attributes
  for (int nodeAttributeCount = 0; nodeAttributeCount < nodePtr->GetNodeAttributeCount(); ++nodeAttributeCount)
  {
    info += std::string("NodeAttribute: ");
    FbxNodeAttribute* attributePtr = nodePtr->GetNodeAttributeByIndex(nodeAttributeCount);
    FbxNodeAttribute::EAttributeType type = attributePtr->GetAttributeType();
    switch (type)
    {
    case FbxNodeAttribute::eUNIDENTIFIED: info += std::string("eUNIDENTIFIED"); break;
    case FbxNodeAttribute::eNULL: info += std::string("eNULL"); break;
    case FbxNodeAttribute::eMARKER: info += std::string("eMARKER"); break;
    case FbxNodeAttribute::eSKELETON: info += std::string("eSKELETON"); break;
    case FbxNodeAttribute::eMESH: info += std::string("eMESH"); break;
    case FbxNodeAttribute::eNURB: info += std::string("eNURB"); break;
    case FbxNodeAttribute::ePATCH: info += std::string("ePATCH"); break;
    case FbxNodeAttribute::eCAMERA: info += std::string("eCAMERA"); break;
    case FbxNodeAttribute::eCAMERA_SWITCHER: info += std::string("eCAMERA_SWITCHER"); break;
    case FbxNodeAttribute::eLIGHT: info += std::string("eLIGHT"); break;
    case FbxNodeAttribute::eOPTICAL_REFERENCE: info += std::string("eOPTICAL_REFERENCE"); break;
    case FbxNodeAttribute::eOPTICAL_MARKER: info += std::string("eOPTICAL_MARKER"); break;
    case FbxNodeAttribute::eNURBS_CURVE: info += std::string("eNURBS_CURVE"); break;
    case FbxNodeAttribute::eTRIM_NURBS_SURFACE: info += std::string("eTRIM_NURBS_SURFACE"); break;
    case FbxNodeAttribute::eBOUNDARY: info += std::string("eBOUNDARY"); break;
    case FbxNodeAttribute::eNURBS_SURFACE: info += std::string("eNURBS_SURFACE"); break;
    case FbxNodeAttribute::eSHAPE: info += std::string("eSHAPE"); break;
    case FbxNodeAttribute::eLODGROUP: info += std::string("eLODGROUP"); break;
    case FbxNodeAttribute::eSUBDIV: info += std::string("eSUBDIV"); break;
    default: info += std::string("unknown type!!"); break;
    }

    info += std::string("\n");
  }
#endif

  // dump all node properties
  FbxProperty firstProperty = nodePtr->GetFirstProperty();
  FbxProperty currentProperty = firstProperty;
  do
  {
    info += std::string("Property Name: ");
    info += std::string(currentProperty.GetName().Buffer());
    FbxDataType dataType = currentProperty.GetPropertyDataType();
    info += std::string(", Type: ");
    info += std::string(dataType.GetName());

    info += std::string(", FBX Type: ");

    switch (dataType.GetType())
    {
    case eFbxUndefined: info += std::string("eUNIDENTIFIED"); break;
    case eFbxChar: info += std::string("eBYTE1"); break;
    case eFbxUChar: info += std::string("eUBYTE1"); break;
    case eFbxShort: info += std::string("eSHORT1"); break;
    case eFbxUShort: info += std::string("eUSHORT1"); break;
    case eFbxUInt: info += std::string("eUINTEGER1"); break;
    case eFbxLongLong: info += std::string("eLONGLONG1"); break;
    case eFbxULongLong: info += std::string("eULONGLONG1"); break;
    case eFbxHalfFloat: info += std::string("eHALFFLOAT"); break;
    case eFbxBool: info += std::string("eBOOL1"); break;
    case eFbxInt: info += std::string("eINTEGER1"); break;
    case eFbxFloat: info += std::string("eFLOAT1"); break;
    case eFbxDouble: info += std::string("eDOUBLE1"); break;
    case eFbxDouble2: info += std::string("eDOUBLE2"); break;
    case eFbxDouble3: info += std::string("eDOUBLE3"); break;
    case eFbxDouble4: info += std::string("eDOUBLE4"); break;
    case eFbxDouble4x4: info += std::string("eDOUBLE44"); break;
    case eFbxEnum: info += std::string("eENUM"); break;
    case eFbxString: info += std::string("eSTRING"); break;
    case eFbxTime: info += std::string("eTIME"); break;
    case eFbxReference: info += std::string("eREFERENCE"); break;
    case eFbxBlob: info += std::string("eBLOB"); break;
    case eFbxDistance: info += std::string("eDISTANCE"); break;
    case eFbxDateTime: info += std::string("eDATETIME"); break;
    case eFbxTypeCount: info += std::string("eMAX_TYPES"); break;
    default: info += std::string("unknown type!!"); break;
    }

    info += std::string("\n");
  }
  while ((currentProperty = nodePtr->GetNextProperty(currentProperty)).IsValid());

  XMD::XGlobal::GetLogger()->Log(XBasicLogger::kInfo, info.c_str());
}
  #define FBX_DUMP_NODE_ATTRIBUTES(nodePtr)  FBXDumpNodeProperties(nodePtr)
#else//_DEBUG
  #define FBX_DUMP_NODE_ATTRIBUTES()
#endif//_DEBUG

//----------------------------------------------------------------------------------------------------------------------
bool FBXFileTranslator::generateMaterials(FbxScene* scene, std::vector<XMaterial*>& ms)
{
  NMP_VERIFY_FAIL("FBXFileTranslator::generateMaterials is not currently supported");

  return false;
}

template<typename T> bool doDiffCheck(T& x, const T& tl, float am) {

  bool b = false;
  if (tl > x)
  {
    while ((tl - am * 0.5f) > x)
    {
      x += am;
      b = true;
    }
  }
  else if (tl < x)
  {
    while ((tl + am * 0.5f) < x)
    {
      x -= am;
      b = true;
    }
  }
  return b;
}

//----------------------------------------------------------------------------------------------------------------------
bool generateAnimation(FbxAnimEvaluator& evaluator, std::map<FbxNode*, XBone*>& nmap, XAnimCycle& cycle, float scale)
{
  // Looping frames then bones is much faster than looping bones then frames.
  // Do not change the order of these loops without profiling with large fbx files with multiple takes.

  XM2::XU32 numFrames = cycle.GetNumFrames();

  // set up all the anim cycles
  for (std::map<FbxNode*, XBone*>::iterator it = nmap.begin(), end = nmap.end(); it != end; ++it)
  {
    FbxNode* pNode = it->first;
    XBone* pBone = it->second;

    XSampledKeys* bone_anim = cycle.AddSampledKeys(pBone->GetID());
    bone_anim->SetSize(numFrames);
  }

  // now fill them with data
  for (uint32_t frameIndex = 0; frameIndex < numFrames; ++frameIndex)
  {
    FbxTime time;
    double currentTime = (frameIndex + cycle.GetStartTime()) * cycle.GetTimeInc();
    time.SetSecondDouble(currentTime);
    
    for (std::map<FbxNode*, XBone*>::iterator it = nmap.begin(), end = nmap.end(); it != end; ++it)
    {
      FbxNode* pNode = it->first;
      
      const FbxAMatrix& localMatrix =  evaluator.GetNodeLocalTransform(pNode, time);

      // convert the fbx transform values to XMD equivalents
      XVector3 scale;
      FbxVector4 fbxScale = localMatrix.GetS();
      ConvertVector<FbxVector4, XVector3, 3>(fbxScale, scale);

      XVector3 translate;
      FbxVector4 fbxTranslate = localMatrix.GetT();
      ConvertVector<FbxVector4, XVector3, 3>(fbxTranslate, translate);

      XQuaternion rot;
      FbxQuaternion fbxRot = localMatrix.GetQ();
      ConvertVector<FbxQuaternion, XQuaternion, 4>(fbxRot, rot);
      

      XBone* pBone = it->second;

      XM2::XQuaternion xmd_jo = pBone->GetJointOrient();
      XM2::XQuaternion xmd_ro = pBone->GetRotationOrient();

      // now we need to remove the effect of the pre/post rotations
      xmd_jo.invert();
      xmd_ro.invert();
      XM2::XQuaternion rotation_only;
      rotation_only.mul(xmd_ro, rot);
      rotation_only.mul(xmd_jo);

      XSampledKeys* bone_anim = cycle.GetBoneKeys(pBone->GetID());

      XQuaternionArray& rotationKeys = bone_anim->RotationKeys();
      rotationKeys[frameIndex] = rotation_only;

      XVector3Array& scaleKeys = bone_anim->ScaleKeys();
      scaleKeys[frameIndex] = scale;

      XVector3Array& translationKeys = bone_anim->TranslationKeys();
      translationKeys[frameIndex] = translate;
    }
  }

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool FBXFileTranslator::remapBones(std::map<FbxNode*, XBone*>& mNodeMap, FbxNode* node)
{
  XBone* bone = m_model->GuessBone(FixupName(node->GetName()));
  if (bone)
  {
    mNodeMap.insert(std::make_pair(node, bone));
  }

  unsigned nKids = node->GetChildCount();
  for (unsigned i = 0; i != nKids; ++i)
  {
    if (!remapBones(mNodeMap, node->GetChild(i)))
      return false;
  }
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool FBXFileTranslator::CreateMesh(FbxNode* node, XMD::XBone* parent, std::vector<XMD::XMaterial*>& materials)
{
  const FbxMesh& fbxMesh = *node->GetMesh();

  if (fbxMesh.GetPolygonCount() == 0)
    return true;

  // create the XMD mesh
  XPolygonMesh* xMesh = m_model->CreateNode(XFn::PolygonMesh)->HasFn<XPolygonMesh>();
  xMesh->SetName((FixupName(node->GetName()) + "Shape").c_str());

  // get the poly sizes
  XU32Array polyCountArray;
  const unsigned int polygonCount = fbxMesh.GetPolygonCount();
  polyCountArray.resize(polygonCount);
  for (unsigned int polyIndex = 0; polyIndex < polygonCount; ++polyIndex)
  {
    polyCountArray[polyIndex] = fbxMesh.GetPolygonSize(polyIndex);
  }

  // get the poly indices
  XMD::XS32Array indexArray;
  indexArray.resize(fbxMesh.GetPolygonVertexCount());
  int* polyVerts = fbxMesh.GetPolygonVertices();
  const size_t copySize = sizeof(int) * indexArray.size();
  memcpy(&indexArray[0], polyVerts, copySize);

  // apply the polys to the mesh
  xMesh->SetPolyCounts(polyCountArray, indexArray);

  // create the control point containers
  XVector3Array points;
  const unsigned int controlPointCount = fbxMesh.GetControlPointsCount();
  points.resize(controlPointCount);
  FbxVector4* pnts = fbxMesh.GetControlPoints();

  // read in all the control points (vertex positions)
  for (XVector3Array::iterator it = points.begin(); it != points.end(); ++it, ++pnts)
  {
    it->x = (XReal)(*pnts)[0] * m_scale;
    it->y = (XReal)(*pnts)[1] * m_scale;
    it->z = (XReal)(*pnts)[2] * m_scale;
  }
  xMesh->SetPoints(points);

  // create the mesh normals
  XVector3Array normals;
  xMesh->CalculateNormals();

  // read in the mesh uvs
  const FbxLayer* uvLayer = fbxMesh.GetLayer(0, FbxLayerElement::eTextureDiffuse);
  if (uvLayer)
  {
    if ( uvLayer->GetUVs())
    {
      // create the UV coords
      int uvIndex = 0;
      const FbxLayerElementArrayTemplate<FbxVector2>& uvArray = uvLayer->GetUVs()->GetDirectArray();
      XVector2Array uvs;
      XVertexSet* uvSet = xMesh->CreateVertexSet("UVs", uvArray.GetCount(), 2, XVertexSet::kTexCoord);
      uvSet->SetData(uvs);
      for (int uvIndex = 0; uvIndex < uvArray.GetCount(); ++uvIndex)
      {
        uvs[uvIndex].x = (XReal)uvArray[uvIndex][0];
        uvs[uvIndex].y = (XReal)uvArray[uvIndex][1];
      }
  
      // create the index buffer
      XIndexSet* indexSet = uvSet->CreateIndexSet();
      FbxLayerElementArrayTemplate<int>& intArray = uvLayer->GetUVs()->GetIndexArray();
      for (int indece = 0; indece < intArray.GetCount(); ++indece)
      {
        (*indexSet)[indece] = intArray.GetAt(indece);
      }
    }
  }

  XInstance* instance = m_model->CreateGeometryInstance(parent, xMesh);
  parent->AddInstance(instance);

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool init()
{
  if (g_manager)
  {
    return false;
  }

  g_manager = FbxManager::Create();

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool term()
{
  if (g_manager)
  {
    g_manager->Destroy();
    g_manager = 0;

    return true;
  }

  return false;
}

} //namespace FBX_XMD_PLUGIN
