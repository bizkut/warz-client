// Copyright (c) 2010 NaturalMotion.  All Rights Reserved.
// Not to be copied, adapted, modified, used, distributed, sold,
// licensed or commercially exploited in any manner without the
// written consent of NaturalMotion.
//
// All non public elements of this software are the confidential
// information of NaturalMotion and may not be disclosed to any
// person nor used for any purpose not expressly approved by
// NaturalMotion in writing.

#include "FBXPlugin.h"
#include <math.h>

#include <fbxsdk.h>

#include "XM2/EulerAngles.h"
#include "XMU/FileSystem.h"
#include "XMD/Model.h"

namespace FBX_XMD_PLUGIN
{

//----------------------------------------------------------------------------------------------------------------------
//
//  Simply builds a map of the fbx nodes to the current XModels bones. Then
//  recursively sets the animation data on each bone.
//
bool FBXFileTranslator::ImportAnim(const XMD::XChar* filename, XMD::XModel& m)
{
  // store the old options
  FBXIOOptions oldOptions = m_importOptions;
  FBXIOOptions newOptions;
  newOptions.m_animations = true;
  newOptions.m_globalSettings = true;
  newOptions.m_gobos = false;
  newOptions.m_links = false;
  newOptions.m_materials = false;
  newOptions.m_shapes = false;
  newOptions.m_textures = false;
  newOptions.m_geometry = false;

  m_importOptions = newOptions;

  bool result = Import(filename, m);

  // restore old options;
  m_importOptions = oldOptions;

  return result;
}

}//namespace FBX_XMD_PLUGIN