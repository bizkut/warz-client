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
  extern FbxManager* g_manager;
  typedef std::map<XBone*, FbxNode*> BoneDict;

  //----------------------------------------------------------------------------------------------------------------------
  void FBXFileTranslator::SetExportOptions(const FBXIOOptions& i_options)
  {
    m_exportOptions = i_options;
  }

  //----------------------------------------------------------------------------------------------------------------------
  void CreateSkeleton(BoneDict &nodeMap, bool isRoot, FbxNode* parentNode, XBone* bone)
  {
    FBXSDK_NAMESPACE::FbxScene* scene = parentNode->GetScene();

    FbxSkeleton* lSkeletonNodeAttribute = FbxSkeleton::Create(scene, bone->GetName());
    if (isRoot)
    {
      lSkeletonNodeAttribute->SetSkeletonType(FbxSkeleton::eRoot);
    }
    else
    {
      lSkeletonNodeAttribute->SetSkeletonType(FbxSkeleton::eLimbNode);
    }
    

    FbxNode* node = FbxNode::Create(scene, bone->GetName());
    node->SetNodeAttribute(lSkeletonNodeAttribute);

    node->RotationActive = true;
    node->TranslationActive = true;

    node->UpdatePivotsAndLimitsFromProperties();

    XMD::XBone::RotationOrder rotaionOrder = bone->GetRotationOrder();
    node->SetPivotState(FbxNode::eSourcePivot, FbxNode::ePivotActive);

    switch (rotaionOrder)
    {
    case XMD::XBone::kXYZ:
      node->SetRotationOrder(FbxNode::eDestinationPivot, FBXSDK_NAMESPACE::eEulerXYZ);
      break;
    case XMD::XBone::kXZY:
      node->SetRotationOrder(FbxNode::eDestinationPivot, FBXSDK_NAMESPACE::eEulerXZY);
      break;
    case XMD::XBone::kYXZ:
      node->SetRotationOrder(FbxNode::eDestinationPivot, FBXSDK_NAMESPACE::eEulerYXZ);
      break;
    case XMD::XBone::kYZX:
      node->SetRotationOrder(FbxNode::eDestinationPivot, FBXSDK_NAMESPACE::eEulerYZX);
      break;
    case XMD::XBone::kZXY:
      node->SetRotationOrder(FbxNode::eDestinationPivot, FBXSDK_NAMESPACE::eEulerZXY);
      break;
    case XMD::XBone::kZYX:
      node->SetRotationOrder(FbxNode::eDestinationPivot, FBXSDK_NAMESPACE::eEulerZYX);
      break;
    }
    const XMD::XQuaternion preRotation = bone->GetJointOrient();
    const XMD::XQuaternion postRotation = bone->GetRotationOrient();

    XM2::XReal x,y,z = 0;

    preRotation.toEulerAnglesD(x,y,z);
    node->SetPreRotation(FbxNode::eSourcePivot, FbxVector4(x, y, z));

    postRotation.toEulerAnglesD(x,y,z);
    node->SetPostRotation(FbxNode::eSourcePivot, FbxVector4(x, y, z));

    const XM2::XVector3 rotatePivot = bone->GetRotatePivot();
    node->SetRotationPivot(FbxNode::eSourcePivot, FbxVector4(rotatePivot.x, rotatePivot.y, rotatePivot.z));

    const XM2::XVector3 rotatePivotTranslate = bone->GetRotatePivotTranslate();
    node->SetRotationOffset(FbxNode::eSourcePivot, FbxVector4(rotatePivotTranslate.x, rotatePivotTranslate.y, rotatePivotTranslate.z));

    const XM2::XVector3 translation = bone->GetTranslation();
    node->LclTranslation.Set(FbxDouble3(translation.x, translation.y,translation.z));

    const XM2::XQuaternion rotation = bone->GetRotation();
    node->LclRotation.Set(FbxVector4(rotation.x, rotation.y, rotation.z, rotation.w));

    parentNode->AddChild(node);

    nodeMap[bone] = node;

    XMD::XBoneList bonelist;
    bone->GetChildren(bonelist);

    for (size_t i = 0, childBoneCount = bonelist.size(); i < childBoneCount; i++)
    {
      CreateSkeleton(nodeMap, false, node, bonelist[i]);
    }
  }

  //----------------------------------------------------------------------------------------------------------------------
  void CreateAnimation(BoneDict &nodeMap, XBone* bone,  const XMD::XAnimCycle* animCycle, FbxAnimLayer* animLayer)
  {
    XMD::XBoneList bonelist;
    bone->GetChildren(bonelist);

    FbxNode* node = nodeMap[bone];

    const XMD::XSampledKeys* sampledKeys = animCycle->GetBoneKeys(bone);

    const XMD::XVector3Array& scaleKeys = sampledKeys->ScaleKeys();
    const XMD::XVector3Array& translationKeys = sampledKeys->TranslationKeys();
    const XMD::XQuaternionArray& rotationKeys = sampledKeys->RotationKeys();

    FbxAnimCurve* tXCurve = node->LclTranslation.GetCurve(animLayer,  FBXSDK_CURVENODE_COMPONENT_X, true);
    FbxAnimCurve* tYCurve = node->LclTranslation.GetCurve(animLayer, FBXSDK_CURVENODE_COMPONENT_Y, true);
    FbxAnimCurve* tZCurve = node->LclTranslation.GetCurve(animLayer, FBXSDK_CURVENODE_COMPONENT_Z, true);

    FbxAnimCurve* rXCurve = node->LclRotation.GetCurve(animLayer, FBXSDK_CURVENODE_COMPONENT_X, true);
    FbxAnimCurve* rYCurve = node->LclRotation.GetCurve(animLayer, FBXSDK_CURVENODE_COMPONENT_Y, true);
    FbxAnimCurve* rZCurve = node->LclRotation.GetCurve(animLayer, FBXSDK_CURVENODE_COMPONENT_Z, true);

    FbxAnimCurve* sXCurve = node->LclScaling.GetCurve(animLayer, FBXSDK_CURVENODE_COMPONENT_X, true);
    FbxAnimCurve* sYCurve = node->LclScaling.GetCurve(animLayer, FBXSDK_CURVENODE_COMPONENT_Y, true);
    FbxAnimCurve* sZCurve = node->LclScaling.GetCurve(animLayer, FBXSDK_CURVENODE_COMPONENT_Z, true);

    XM2::XReal startTime = animCycle->GetStartTime();
    XM2::XReal fps = animCycle->GetFramesPerSecond();
    FbxTime lTime;
    int lKeyIndex = 0;

    //Translation
    tXCurve->KeyModifyBegin();
    tYCurve->KeyModifyBegin();
    tZCurve->KeyModifyBegin();

    for (size_t i = 0, count = translationKeys.size(); i < count; i++)
    {
      lTime.SetSecondDouble(startTime + (i / fps));
      lKeyIndex = tXCurve->KeyAdd(lTime);
      tXCurve->KeySetValue(lKeyIndex, translationKeys[i].x);
      tXCurve->KeySetInterpolation(lKeyIndex, FbxAnimCurveDef::eInterpolationLinear);

      lKeyIndex = tYCurve->KeyAdd(lTime);
      tYCurve->KeySetValue(lKeyIndex, translationKeys[i].y);
      tYCurve->KeySetInterpolation(lKeyIndex, FbxAnimCurveDef::eInterpolationLinear);

      lKeyIndex = tZCurve->KeyAdd(lTime);
      tZCurve->KeySetValue(lKeyIndex, translationKeys[i].z);
      tZCurve->KeySetInterpolation(lKeyIndex, FbxAnimCurveDef::eInterpolationLinear);
    }

    tXCurve->KeyModifyEnd();
    tYCurve->KeyModifyEnd();
    tZCurve->KeyModifyEnd();

    //rotation
    rXCurve->KeyModifyBegin();
    rYCurve->KeyModifyBegin();
    rZCurve->KeyModifyBegin();

    for (size_t i = 0, count = rotationKeys.size(); i < count; i++)
    {
       XMD::XQuaternion rotation = rotationKeys[i];

       XM2::XReal x,y,z;
       rotation.toEulerAnglesD(x, y, z);
      
      lTime.SetSecondDouble(startTime + (i / fps));
      lKeyIndex = rXCurve->KeyAdd(lTime);
      rXCurve->KeySetValue(lKeyIndex, x);
      rXCurve->KeySetInterpolation(lKeyIndex, FbxAnimCurveDef::eInterpolationLinear);

      lKeyIndex = rYCurve->KeyAdd(lTime);
      rYCurve->KeySetValue(lKeyIndex, y);
      rYCurve->KeySetInterpolation(lKeyIndex, FbxAnimCurveDef::eInterpolationLinear);

      lKeyIndex = rZCurve->KeyAdd(lTime);
      rZCurve->KeySetValue(lKeyIndex, z);
      rZCurve->KeySetInterpolation(lKeyIndex, FbxAnimCurveDef::eInterpolationLinear);

    }

    rXCurve->KeyModifyEnd();
    rYCurve->KeyModifyEnd();
    rZCurve->KeyModifyEnd();

    FbxAnimCurveFilterGimbleKiller gimblerFilter;
    FbxAnimCurve* rotationCurves[] = {rXCurve, rYCurve, rZCurve};
    gimblerFilter.Apply(rotationCurves, 3);

    //scale
    sXCurve->KeyModifyBegin();
    sYCurve->KeyModifyBegin();
    sZCurve->KeyModifyBegin();

    for (size_t i = 0, count = scaleKeys.size(); i < count; i++)
    {
      lTime.SetSecondDouble(startTime + (i / fps));
      lKeyIndex = sXCurve->KeyAdd(lTime);
      sXCurve->KeySetValue(lKeyIndex, scaleKeys[i].x);
      sXCurve->KeySetInterpolation(lKeyIndex, FbxAnimCurveDef::eInterpolationLinear);

      lKeyIndex = sYCurve->KeyAdd(lTime);
      sYCurve->KeySetValue(lKeyIndex, scaleKeys[i].y);
      sYCurve->KeySetInterpolation(lKeyIndex, FbxAnimCurveDef::eInterpolationLinear);

      lKeyIndex = sZCurve->KeyAdd(lTime);
      sZCurve->KeySetValue(lKeyIndex, scaleKeys[i].z);
      sZCurve->KeySetInterpolation(lKeyIndex, FbxAnimCurveDef::eInterpolationLinear);
    }

    sXCurve->KeyModifyEnd();
    sYCurve->KeyModifyEnd();
    sZCurve->KeyModifyEnd();

    FBXSDK_NAMESPACE::FbxScene* scene = node->GetScene();

    for (size_t i = 0, childBoneCount = bonelist.size(); i < childBoneCount; i++)
    {
      CreateAnimation(nodeMap, bonelist[i], animCycle, animLayer);
    }
  }

  //----------------------------------------------------------------------------------------------------------------------
  bool FBXFileTranslator::Export(const XMD::XChar* filename, XMD::XModel& model) 
  { 
    bool result = false;
#if FBX_PLUGIN_ENABLE_PROFILING != 0
    NMP::Timer profileTimer;
#endif

    std::string sfilename = filename;

#ifdef _DEBUG
    XBasicLogger* oldLogger = XGlobal::GetLogger();
    XFileLogger* fileLogger = new XFileLogger("FBXFileTranslator_Log.txt");
    XGlobal::SetLogger(fileLogger);
#endif//_DEBUG

    m_model = &model;

    FBX_PLUGIN_PROFILE_TIMER_START(profileTimer);
    FbxExporter* exporter = FbxExporter::Create(g_manager, "");
    FBX_PLUGIN_PROFILE_TIMER_STOP(profileTimer, "FbxExporter::Create");

    FBX_PLUGIN_PROFILE_TIMER_START(profileTimer);
    m_scene = FbxScene::Create(g_manager, "");
    FBX_PLUGIN_PROFILE_TIMER_STOP(profileTimer, "FbxScene::Create");

    FBX_PLUGIN_PROFILE_TIMER_START(profileTimer);
    bool initialised = exporter->Initialize(sfilename.c_str());
    FBX_PLUGIN_PROFILE_TIMER_STOP(profileTimer, "FbxExporter::Initialize");

    if (initialised)
    {
      FBX_PLUGIN_PROFILE_TIMER_START(profileTimer);
      FbxIOSettings* ios = FbxIOSettings::Create(g_manager, IOSROOT);
      FBX_PLUGIN_PROFILE_TIMER_STOP(profileTimer, "FbxIOSettings::Create");

      FBX_PLUGIN_PROFILE_TIMER_START(profileTimer);
      ios->SetBoolProp(IMP_FBX_MATERIAL, m_exportOptions.m_materials);
      ios->SetBoolProp(IMP_FBX_TEXTURE, m_exportOptions.m_textures);
      ios->SetBoolProp(IMP_FBX_LINK, m_exportOptions.m_links);
      ios->SetBoolProp(IMP_FBX_SHAPE, m_exportOptions.m_shapes);
      ios->SetBoolProp(IMP_FBX_MODEL_COUNT, m_exportOptions.m_geometry);
      ios->SetBoolProp(IMP_FBX_GOBO, m_exportOptions.m_gobos);
      ios->SetBoolProp(IMP_FBX_ANIMATION, m_exportOptions.m_animations);
      ios->SetBoolProp(IMP_FBX_GLOBAL_SETTINGS, m_exportOptions.m_globalSettings);
      g_manager->SetIOSettings(ios);
      FBX_PLUGIN_PROFILE_TIMER_STOP(profileTimer, "configure IO Settings");

      FBX_PLUGIN_PROFILE_TIMER_START(profileTimer);

      BoneDict nodeMap;

      XMD::XBoneList bonelist;
      m_model->GetRootBones(bonelist);

      XMD::XU32 numAnimCycle = model.NumAnimCycles();

      for (size_t i = 0, boneCount = bonelist.size(); i < boneCount; i++)
      {
        CreateSkeleton(nodeMap, true, m_scene->GetRootNode(), bonelist[i]);
      }

      // Extract all animation takes
      FbxAnimEvaluator* evaluator = m_scene->GetEvaluator();

      FbxGlobalSettings& globalSettings = m_scene->GetGlobalSettings();
      switch (model.GetInfo()->GetUpAxis())
      {
      case XMD::XMD_YUp:
        globalSettings.SetOriginalUpAxis(FbxAxisSystem::MayaYUp);
        globalSettings.SetAxisSystem(FbxAxisSystem::MayaYUp);
        break;
      case XMD::XMD_ZUp:
        globalSettings.SetOriginalUpAxis(FbxAxisSystem::Max);
        globalSettings.SetAxisSystem(FbxAxisSystem::Max);
        break;
      }
      
      for (XMD::XU32 i = 0; i < numAnimCycle; i++ )
      {
        const XMD::XAnimCycle* animCycle = model.GetAnimCycle(i);

        FbxAnimStack* animStack = FbxAnimStack::Create(m_scene, animCycle->GetName());
        FbxAnimLayer* animLayer = FbxAnimLayer::Create(m_scene, "Base Layer");
        animStack->AddMember(animLayer);

        for (size_t n = 0, boneCount = bonelist.size(); n < boneCount; n++)
        {
          CreateAnimation(nodeMap, bonelist[n], animCycle, animLayer);
        }
        if (animCycle->GetNumFrames() > 0 )
        {
          FbxTimeSpan timeSpan;
          FbxTime endTime;
          endTime.SetFrame(0);
          timeSpan.SetStart(endTime);

          endTime.SetFrame(animCycle->GetNumFrames() - 1);
          timeSpan.SetStop(endTime);
          
          animStack->SetLocalTimeSpan(timeSpan);
        }
      }

      FBX_PLUGIN_PROFILE_TIMER_STOP(profileTimer, "Converting scene");

      FBX_PLUGIN_PROFILE_TIMER_START(profileTimer);
      result = exporter->Export(m_scene);
      FBX_PLUGIN_PROFILE_TIMER_STOP(profileTimer, "FbxExporter::Export");
    }

    FBX_PLUGIN_PROFILE_TIMER_START(profileTimer);
    m_scene->Destroy();
    FBX_PLUGIN_PROFILE_TIMER_STOP(profileTimer, "FbxScene::Destroy");
    m_scene = 0;
    FBX_PLUGIN_PROFILE_TIMER_START(profileTimer);
    exporter->Destroy();
    FBX_PLUGIN_PROFILE_TIMER_STOP(profileTimer, "FbxImporter::Destroy");

#ifdef _DEBUG
    // restore the old logger and delete the file logger
    XGlobal::SetLogger(oldLogger);
    delete fileLogger;
#endif//_DEBUG

#if FBX_PLUGIN_ENABLE_PROFILING != 0
    profileTimer.printTotalSectionTime("FBXFileTranslator::Export");
#endif
    return result;
  }

}//namespace FBX_XMD_PLUGIN