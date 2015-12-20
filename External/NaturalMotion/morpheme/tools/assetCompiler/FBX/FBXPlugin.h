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
#ifdef _MSC_VER
  #pragma once
#endif
#ifndef ASSET_COMPILER_FBX_PLUGIN_H
#define ASSET_COMPILER_FBX_PLUGIN_H
//----------------------------------------------------------------------------------------------------------------------
#include "XMD/FileTranslator.h"
#include <map>
#include <vector>

#ifndef FBXSDK_NAMESPACE
  #define FBXSDK_NAMESPACE fbxsdk_2014_1
#endif

#ifdef XMD_PLUGIN_FBX_STATIC
  #define FBX_PLUGIN_PUBLIC
#else
  #ifdef XMDPLUGIN_FBX_EXPORTS
    #define FBX_PLUGIN_PUBLIC __declspec(dllexport)
  #else
    #define FBX_PLUGIN_PUBLIC __declspec(dllimport)
  #endif
#endif

/// \brief Will output profile timings to stdout when importing fbx files.
#define FBX_PLUGIN_ENABLE_PROFILING 0

/// \brief Due to bad performance of FbxImporter::Initialize, for large fbx files it is much quicker to
/// ignore the frame rate stored in the file and just use the default global setting. This avoids the need
/// to call FbxImporter::Initialize at all when generating anim info for a file.
#define FBX_PLUGIN_ANIMINFO_DISABLE_IMPORT_FRAMERATE 1

#if FBX_PLUGIN_ENABLE_PROFILING != 0
# define FBX_PLUGIN_PROFILE_TIMER_START(_timer) _timer.start()
# define FBX_PLUGIN_PROFILE_TIMER_STOP(_timer, _message) _timer.stop(_message)
#else // defined(FBX_PLUGIN_ENABLE_PROFILING)
# define FBX_PLUGIN_PROFILE_TIMER_START(_timer)
# define FBX_PLUGIN_PROFILE_TIMER_STOP(_timer, _message)
#endif // defined(FBX_PLUGIN_ENABLE_PROFILING)

//----------------------------------------------------------------------------------------------------------------------
// forward declarations
namespace FBXSDK_NAMESPACE
{
class FbxNode;
class FbxTakeInfo;
class FbxAnimEvaluator;
class FbxScene;
class FbxManager;
class FbxSurfaceMaterial;
class FbxScene;
class FbxCluster;
class FbxPose;
}

namespace XMD
{
class XModel;
class XBone;
class XMaterial;
class XAnimCycle;
class XInstance;
class XGeometry;
class XJoint;
}

namespace ME
{
class AnimInfo;
}

namespace FBX_XMD_PLUGIN
{

enum FBXIOFlags
{
  MATERIAL = 0,
  TEXTURE,
  LINK,
  SHAPE,
  GOBO,
  ANIMATION,
  GLOBAL_SETTINGS
};

struct FBXIOOptions
{
  bool m_materials;
  bool m_textures;
  bool m_links;
  bool m_shapes;
  bool m_gobos;
  bool m_animations;
  bool m_globalSettings;
  bool m_geometry;
};

//----------------------------------------------------------------------------------------------------------------------
/// \brief Plugin for XMD, translates a FBX file to XMD
class FBX_PLUGIN_PUBLIC FBXFileTranslator : public XMD::XFileTranslator
{
public:
  FBXFileTranslator();
  virtual ~FBXFileTranslator();

  //----------------------------------------------------------------------------
  /// \brief Sets a scaling value to be applied on all bone translations
  virtual void SetScale(float sc) { m_scale = sc; }

  //----------------------------------------------------------------------------
  /// \brief Imports a FBX file into XMD
  virtual bool Import(const XMD::XChar* filename, XMD::XModel& model);

  //----------------------------------------------------------------------------
  /// \brief Imports just the animations from a FBX file into XMD
  virtual bool ImportAnim(const XMD::XChar* filename, XMD::XModel& model);

  //----------------------------------------------------------------------------
  /// \brief Exports an XMD to FBX
  virtual bool Export(const XMD::XChar* filename, XMD::XModel& model);

  //----------------------------------------------------------------------------
  /// \brief As name, is not currently supported
  virtual bool HasExport() const { return true; }

  //----------------------------------------------------------------------------
  /// \brief As name, is currently supported
  virtual bool HasImport() const { return true; }

  //----------------------------------------------------------------------------
  /// \brief As name, is currently supported
  virtual bool HasImportAnim() const { return true; }

  //----------------------------------------------------------------------------------------------------------------------
  /// \brief Set the options used when importing FBX files
  void SetImportOptions(const FBXIOOptions& i_options);

  //----------------------------------------------------------------------------------------------------------------------
  /// \brief Set the export used when importing FBX files
  void SetExportOptions(const FBXIOOptions& i_options);

private:
  std::string convertString(const XMD::XChar* str);
  bool remapBones(std::map<FBXSDK_NAMESPACE::FbxNode*, XMD::XBone*>& mNodeMap, FBXSDK_NAMESPACE::FbxNode* node);
  bool generateBones(
    std::map<FBXSDK_NAMESPACE::FbxNode*,
    XMD::XBone*>& nmap,
    FBXSDK_NAMESPACE::FbxAnimEvaluator* evaluator,
    FBXSDK_NAMESPACE::FbxNode* node,
    std::vector<XMD::XMaterial*>& materials,
    XMD::XBone* parent = 0);
  bool generateGeometry(FBXSDK_NAMESPACE::FbxNode* node, XMD::XBone*, std::vector<XMD::XMaterial*>& materials);
  bool generateDeformers(FBXSDK_NAMESPACE::FbxNode* node, std::map<FBXSDK_NAMESPACE::FbxCluster*, XMD::XJoint*>* jointMap);
  bool SetToBindPose(const std::map<FBXSDK_NAMESPACE::FbxCluster*, XMD::XJoint*>& jointMap);
  bool generateMaterials(FBXSDK_NAMESPACE::FbxScene* node, std::vector<XMD::XMaterial*>& materials);
  bool generateTextures(FBXSDK_NAMESPACE::FbxScene* node);

  bool addBones(FBXSDK_NAMESPACE::FbxNode* node, XMD::XBone* parent, std::map<XMD::XBone*, FBXSDK_NAMESPACE::FbxNode*>& nmap, FBXSDK_NAMESPACE::FbxManager* fbxMan, bool root) ;
  bool addGeometry(XMD::XGeometry* geom, std::map<XMD::XBone*, FBXSDK_NAMESPACE::FbxNode*>& nmap, FBXSDK_NAMESPACE::FbxManager* fbxMan, std::map<XMD::XId, FBXSDK_NAMESPACE::FbxSurfaceMaterial*>& fbxmaterials) ;
  bool addDeformers(FBXSDK_NAMESPACE::FbxNode* node);
  bool addMaterials(FBXSDK_NAMESPACE::FbxManager* fbxMan, std::map<XMD::XId, FBXSDK_NAMESPACE::FbxSurfaceMaterial*>& fbxmaterials) ;
  bool addTextures(FBXSDK_NAMESPACE::FbxNode* node);
  bool addAnimation(XMD::XAnimCycle* cycle, std::map<XMD::XBone*, FBXSDK_NAMESPACE::FbxNode*>& nmap);

  bool makeCluster(FBXSDK_NAMESPACE::FbxNode* fbxBone, XMD::XBone* mesh);
  bool makeMesh(XMD::XInstance* inst, FBXSDK_NAMESPACE::FbxNode* fbxBone, FBXSDK_NAMESPACE::FbxManager* fbxMan, std::map<XMD::XId, FBXSDK_NAMESPACE::FbxSurfaceMaterial*>& fbxmaterials);
  bool makeNurbsSurface(XMD::XInstance* inst, FBXSDK_NAMESPACE::FbxNode* fbxBone, FBXSDK_NAMESPACE::FbxManager* fbxMan, std::map<XMD::XId, FBXSDK_NAMESPACE::FbxSurfaceMaterial*>& fbxmaterials);
  bool makePatch(XMD::XInstance* inst, FBXSDK_NAMESPACE::FbxNode* fbxBone, FBXSDK_NAMESPACE::FbxManager* fbxMan, std::map<XMD::XId, FBXSDK_NAMESPACE::FbxSurfaceMaterial*>& fbxmaterials);
  bool makeCamera(XMD::XInstance* inst, FBXSDK_NAMESPACE::FbxNode* fbxBone, FBXSDK_NAMESPACE::FbxManager* fbxMan);
  bool makeDefaultLight(XMD::XInstance* inst, FBXSDK_NAMESPACE::FbxNode* fbxBone, FBXSDK_NAMESPACE::FbxManager* fbxMan);
  bool makeDirectionalLight(XMD::XInstance* inst, FBXSDK_NAMESPACE::FbxNode* fbxBone, FBXSDK_NAMESPACE::FbxManager* fbxMan);
  bool makeAmbientLight(XMD::XInstance* inst, FBXSDK_NAMESPACE::FbxNode* fbxBone, FBXSDK_NAMESPACE::FbxManager* fbxMan);
  bool makePointLight(XMD::XInstance* inst, FBXSDK_NAMESPACE::FbxNode* fbxBone, FBXSDK_NAMESPACE::FbxManager* fbxMan);
  bool makeSpotLight(XMD::XInstance* inst, FBXSDK_NAMESPACE::FbxNode* fbxBone, FBXSDK_NAMESPACE::FbxManager* fbxMan);

  bool CreateMesh(FBXSDK_NAMESPACE::FbxNode* node, XMD::XBone*, std::vector<XMD::XMaterial*>& materials);

private:
  float        m_scale;
  XMD::XModel* m_model;

  FBXIOOptions                          m_importOptions;
  FBXIOOptions                          m_exportOptions;
  FBXSDK_NAMESPACE::FbxScene*      m_scene;
};

/// \brief Initialize the fbx plugin.
FBX_PLUGIN_PUBLIC bool init();

/// \brief Shutdown the fbx plugin.
FBX_PLUGIN_PUBLIC bool term();

/// \brief Quick check to see if a file is a binary fbx file.
/// \note Does not use any fbx sdk functions because FbxImporter::Initialize is slow for ascii files.
FBX_PLUGIN_PUBLIC bool isBinaryFBX(const char* animFileName);

/// \brief Updates the anim info for an fbx file.
FBX_PLUGIN_PUBLIC bool updateAnimInfoWithFBX(ME::AnimInfo* animInfo, const char* animFileName, time_t animUpdateTime);

} // namespace FBX_XMD_PLUGIN

#endif // ASSET_COMPILER_FBX_PLUGIN_H
