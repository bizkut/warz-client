//----------------------------------------------------------------------------------------------------------------------
/// \file   Upgrading.cpp
/// \date   11-9-2004
/// \author Rob Bateman [http://robthebloke.org]
/// \note   (C) Copyright 2003-2005 Robert Bateman. All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#include "XMD/Model.h"
#include "XMD/Mesh.h"
#include "XMD/PolygonMesh.h"
#include "XMD/Material.h"
#include "XMD/Deformer.h"
#include "XMD/Instance.h"
#include "XMD/Bone.h"
#include "XMD/AnimCycle.h"
#include "XMD/AnimationTake.h"
#include "XMD/PoseStream.h"
#include "XMD/SampledStream.h"
#include "XMD/FCurveStream.h"
#include "XMD/MeshVertexColours.h"
#include "nmtl/pod_vector.h"

using namespace XM2;
using namespace XMD;
namespace XMU  
{
//----------------------------------------------------------------------------------------------------------------------
XMD_EXPORT bool XMUUpdateMeshData(XModel& model)
{
  XList meshes;
  model.List(meshes,XFn::Mesh);

  // convert each mesh found
  for (XList::iterator it = meshes.begin();it != meshes.end(); ++it)
  {
    XVertexSet* pNormals = 0;
    XVertexSet* pUvSets[4] = {0};
    XVertexSet* pColourSets[4] = {0};

    // grab old mesh
    XMesh* mesh = (*it)->HasFn<XMesh>();
    if(!mesh)
      continue;

    // create a new one
    XPolygonMesh* poly_mesh = model.CreateNode(XFn::PolygonMesh)->HasFn<XPolygonMesh>();
    
    // assign verts
    XVector3Array vertices;
    mesh->GetPoints(vertices);
    poly_mesh->SetPoints(vertices);

    // assign normals
    if(mesh->GetNormals().size())
    {
      pNormals = poly_mesh->CreateVertexSet("normals",(XU32)mesh->GetNormals().size(),3,XVertexSet::kNormal);
      pNormals->SetData(mesh->GetNormals());
    }


    // if colours present, copy the colour data
    if (mesh->GetVertexColours())
    {
      XMeshVertexColours* vc = mesh->GetVertexColours();
      for(XU32 i=0;i<vc->GetNumberOfColourSets();++i)
      {
        const XColourArray& colour_data = vc->GetColourArray(i);
        XVertexSet* vset = poly_mesh->CreateVertexSet(vc->GetSetName(i),(XU32)colour_data.size(),4,XVertexSet::kColour);
        vset->SetData(colour_data);
        pColourSets[i] = vset;
      }
    }
    
    // assign UV coords
    if (mesh->GetNumUvSets())
    {
      for(XU32 i=0;i<mesh->GetNumUvSets();++i)
      {
        const XTexCoordList& uv_data = mesh->GetTexCoords(i);
        XVertexSet* vset = poly_mesh->CreateVertexSet(mesh->GetUvSetName(i),(XU32)uv_data.size(),4,XVertexSet::kColour);
        for(size_t j=0;j<uv_data.size();++j)
        {
          vset->SetElement(XVector2(uv_data[j].u,uv_data[j].v),(XU32)j);
        }
        pUvSets[i] = vset;
      }
    }
    XU32 mNumVertexIndices=0;

    // generate poly counts
    {
      XU32Array poly_counts;
      XS32Array indices;
      for (XFaceList::const_iterator itf = mesh->GetFaces().begin();itf != mesh->GetFaces().end();++itf)
      {
        poly_counts.push_back(itf->NumPoints());
        for (XU32 j=0;j<itf->NumPoints();++j)
        {
          indices.push_back((XS32)itf->GetVertexIndex(j));
        }
      }
      poly_mesh->SetPolyCounts(poly_counts,indices);

      mNumVertexIndices = (XU32)indices.size();
    }

    // assign remaining face indices
    {
      XS32Array norm_indices;
      XS32Array colour_indices[4];
      XS32Array uv_indices[4];

      if(pNormals)
        norm_indices.reserve(mNumVertexIndices);

      for(int i=0;i<4;++i)
      {
        if(pUvSets[i])
          uv_indices[i].reserve(mNumVertexIndices);
        if(pColourSets[i])
          colour_indices[i].reserve(mNumVertexIndices);
      }

      for (XFaceList::const_iterator itf = mesh->GetFaces().begin();itf != mesh->GetFaces().end();++itf)
      {
        for (XU32 j=0;j<itf->NumPoints();++j)
        {
          if(pNormals) norm_indices.push_back(itf->GetNormalIndex(j));

          if(pUvSets[0]) uv_indices[0].push_back(itf->GetNormalIndex(j));
          if(pUvSets[1]) uv_indices[1].push_back(itf->GetNormalIndex(j));
          if(pUvSets[2]) uv_indices[2].push_back(itf->GetNormalIndex(j));
          if(pUvSets[3]) uv_indices[3].push_back(itf->GetNormalIndex(j));

          if(pColourSets[0]) colour_indices[0].push_back(itf->GetUvIndex(j,0));
          if(pColourSets[1]) colour_indices[1].push_back(itf->GetUvIndex(j,1));
          if(pColourSets[2]) colour_indices[2].push_back(itf->GetUvIndex(j,2));
          if(pColourSets[3]) colour_indices[3].push_back(itf->GetUvIndex(j,3));
        }
      }

      if(pNormals)
      {
        XIndexSet* iset = poly_mesh->CreateIndexSet("normals");
        iset->SetData(norm_indices);
        pNormals->SetIndexSet(iset);
      }

      if(pUvSets[0]) 
      {
        XIndexSet* iset = poly_mesh->CreateIndexSet("uv_set0");
        iset->SetData(uv_indices[0]);
        pUvSets[0]->SetIndexSet(iset);
      }
      if(pUvSets[1]) 
      {
        XIndexSet* iset = poly_mesh->CreateIndexSet("uv_set1");
        iset->SetData(uv_indices[1]);
        pUvSets[1]->SetIndexSet(iset);
      }
      if(pUvSets[2]) 
      {
        XIndexSet* iset = poly_mesh->CreateIndexSet("uv_set2");
        iset->SetData(uv_indices[2]);
        pUvSets[2]->SetIndexSet(iset);
      }
      if(pUvSets[3]) 
      {
        XIndexSet* iset = poly_mesh->CreateIndexSet("uv_set3");
        iset->SetData(uv_indices[3]);
        pUvSets[3]->SetIndexSet(iset);
      }

      if(pColourSets[0]) 
      {
        XIndexSet* iset = poly_mesh->CreateIndexSet("colour_set0");
        iset->SetData(colour_indices[0]);
        pColourSets[0]->SetIndexSet(iset);
      }
      if(pColourSets[1]) 
      {
        XIndexSet* iset = poly_mesh->CreateIndexSet("colour_set1");
        iset->SetData(colour_indices[1]);
        pColourSets[1]->SetIndexSet(iset);
      }
      if(pColourSets[2]) 
      {
        XIndexSet* iset = poly_mesh->CreateIndexSet("colour_set2");
        iset->SetData(colour_indices[2]);
        pColourSets[2]->SetIndexSet(iset);
      }
      if(pColourSets[3]) 
      {
        XIndexSet* iset = poly_mesh->CreateIndexSet("colour_set3");
        iset->SetData(colour_indices[3]);
        pColourSets[3]->SetIndexSet(iset);
      }
    }

    // disconnect deformer queue from the old node
    XDeformerList deformers;
    mesh->GetDeformerQueue(deformers);
    mesh->SetDeformerQueue(XDeformerList());

    // assign the deformer queue to the new mesh
    poly_mesh->SetDeformerQueue(deformers);

    // duplicate all instances for the new node
    XInstanceList instances;
    mesh->GetObjectInstances(instances);
    for (XInstanceList::iterator it = instances.begin();it != instances.end();++it)
    {
      XInstance* old_instance = *it;
      XInstance* new_instance = model.CreateGeometryInstance(old_instance->GetParent(),poly_mesh);
      if(old_instance->HasMaterialGroups())
      {
        new_instance->SetNumMaterialGroups(old_instance->GetNumMaterialGroups());
        for(XU32 i=0;i<old_instance->GetNumMaterialGroups();++i)
        {
          new_instance->SetMaterial(old_instance->GetMaterial(i),i);
          XIndexList faces;
          old_instance->GetFaces(i,faces);
          new_instance->SetFaces(i,faces);
        }
      }
      else
      {
        new_instance->SetMaterial(old_instance->GetMaterial());
      }
      // delete old instance
      model.DeleteInstance(old_instance->GetID());
    }    

    // delete old mesh
    model.DeleteNode(mesh);
  }  
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
template<typename T>
bool areTheSame(const XM2::pod_vector<T>& test,XReal tolerance)
{
  for (XM2::pod_vector<T>::const_iterator it = test.begin()+1;it < test.end();++it)
  {
    if ( !(*it).equal(*(it-1),tolerance) )
    {
      return false;
    }    
  }
  return true;  
}
//----------------------------------------------------------------------------------------------------------------------
XMD_EXPORT bool XMUUpdateAnimationData(XModel& model,const XReal tolerance)
{
  for (XU32 i=0;i<model.NumAnimCycles();++i)
  {
    // grab anim cycle
    XAnimCycle* cycle = model.GetAnimCycle(i);

    // create a new one
    XAnimationTake* anim = model.CreateNode(XFn::AnimationTake)->HasFn<XAnimationTake>();

    // copy over the basic anim info
    anim->SetStartTime(cycle->GetStartTime());
    anim->SetTime(cycle->GetStartTime());
    anim->SetEndTime(cycle->GetEndTime());
    anim->SetFramesPerSecond(cycle->GetFramesPerSecond());
    anim->SetNumFrames(cycle->GetNumFrames());

    // Now duplicate the sampled keys
    XIndexList node_ids;
    cycle->GetSampledNodes(node_ids);


    for (XIndexList::iterator it = node_ids.begin();it != node_ids.end();++it)
    {
      XBase* node = model.FindNode(*it);
      const XSampledKeys* keys = cycle->GetBoneKeys(*it);
      if (node&&keys)
      {
        XAnimatedNode* anim_node = anim->CreateNodeAnimation(node);

        // create scale keys
        XAnimatedAttribute* scale     = anim_node->CreateAnimatedAttribute(XBone::kScale);
        if (areTheSame(keys->ScaleKeys(),tolerance))
        {
          XPoseStream* scale_keys = scale->CreatePoseStream();
          scale_keys->Set(keys->ScaleKeys()[0]);
        }
        else
        {
          XSampledStream* scale_keys = scale->CreateSampledStream();
          for (size_t s = 0;s!=keys->ScaleKeys().size();++s)
          {
            scale_keys->SetSample((XU32)s,keys->ScaleKeys()[s]);
          }
        }

        // create rotation keys
        XAnimatedAttribute* rotation  = anim_node->CreateAnimatedAttribute(XBone::kRotation);
        if (areTheSame(keys->RotationKeys(),tolerance))
        {
          XPoseStream* rotate_keys = rotation->CreatePoseStream();
          rotate_keys->Set(keys->RotationKeys()[0]);
        } 
        else
        {
          XSampledStream* rotate_keys = rotation->CreateSampledStream();
          for (size_t s = 0;s!=keys->RotationKeys().size();++s)
          {
            rotate_keys->SetSample((XU32)s,keys->RotationKeys()[s]);
          }
        }

        // create translate keys
        XAnimatedAttribute* translate = anim_node->CreateAnimatedAttribute(XBone::kTranslation);
        if (areTheSame(keys->TranslationKeys(),tolerance))
        {
          XPoseStream* translate_keys = translate->CreatePoseStream();
          translate_keys->Set(keys->TranslationKeys()[0]);
        }
        else
        {
          XSampledStream* translate_keys = translate->CreateSampledStream();
          for (size_t s = 0;s!=keys->TranslationKeys().size();++s)
          {
            translate_keys->SetSample((XU32)s,keys->TranslationKeys()[s]);
          }
        }
      }      
    }

    // grab copy of cycle name
    XString name = cycle->GetName();

    // delete old anim cycle
    model.DeleteNode(cycle);

    // rename the new node
    anim->SetName(name.c_str());
  }  
  return true;
}
}
