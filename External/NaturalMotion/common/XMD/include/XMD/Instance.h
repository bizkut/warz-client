//----------------------------------------------------------------------------------------------------------------------
/// \file   Instance.h
/// \author Rob Bateman [http://robthebloke.org]
/// \brief  
/// \note   (C) Copyright 2003-2005 Robert Bateman. All rights reserved.
//----------------------------------------------------------------------------------------------------------------------
#pragma once
//----------------------------------------------------------------------------------------------------------------------
#include "XMD/Base.h"
//----------------------------------------------------------------------------------------------------------------------
namespace XMD 
{
#ifndef DOXYGEN_PROCESSING
class XMD_EXPORT XBone;
class XMD_EXPORT XGeometry;
class XMD_EXPORT XShape;
class XMD_EXPORT XMaterial;
class XMD_EXPORT XModel;
class XMD_EXPORT XTexture;
#endif

#ifndef DOXYGEN_PROCESSING
//----------------------------------------------------------------------------------------------------------------------
/// \class XMD::XMaterialGroup
/// \brief Holds a material and a set of faces that have been assigned that material
//----------------------------------------------------------------------------------------------------------------------
struct XMD_EXPORT XMaterialGroup 
{
  /// the ID of the material applied to these faces
  XU32 m_MaterialID;

  /// the indices of the faces that use this material
  XU32Array m_FaceIndices;

  /// \brief  This function writes the data for the chunk. The header,
  ///         name, extra attributes and surrounding braces will have
  ///         been written already.
  /// \param  io  -  the output file stream
  /// \return true if OK
  bool DoData(XFileIO& io);

  /// \brief  returns the number of bytes required to store this object on disk.
  /// \return the data size of the node in bytes
  XU32 GetDataSize() const;
};
typedef XM2::pod_vector<XMaterialGroup*> XMaterialGroupList;
#endif


//----------------------------------------------------------------------------------------------------------------------
/// \class XMD::XInstance
/// \brief Holds an instance to a geometry object
//---------------------------------------------------------------------------------------------------------------------- 
class XMD_EXPORT XInstance
{
#ifndef DOXYGEN_PROCESSING
  friend class XMD_EXPORT XModel;
  friend class XMD_EXPORT XGeometry;
  friend class XMD_EXPORT XShape;
  friend class XMD_EXPORT XBone;
  friend std::ostream& operator << (std::ostream&,const XInstance&);
  friend std::istream& operator >> (std::istream&, XInstance&);
protected:

  XInstance(XModel*);

public:

  /// dtor
  ~XInstance();

#endif

  //--------------------------------------------------------------------------------------------------------------------
  /// \name   Instance Naming
  /// \brief  Set or Get the instance name
  //@{

  /// \brief  returns the name of this instance
  /// \return the instance name
  const XChar* GetName() const;

  /// \brief  allows you to set the name of the instance
  /// \param  n  -  the new name for the object instance
  void SetName(const XChar* n);

  /// \brief  returns the id number of this instance
  inline XId GetID() const
  {
    return m_InstanceID;
  }

  //@}

  //--------------------------------------------------------------------------------------------------------------------
  /// \name   Basic Material Assignment
  /// \brief  Handles geometry instances that have only one material assigned
  //@{

  /// \brief  returns true if this object has a material assigned
  bool HasMaterial() const;

  /// \brief  returns a pointer to the material applied to this geometry object.
  /// \return returns a pointer of type XMaterial
  XMaterial* GetMaterial() const;

  //@}

  //--------------------------------------------------------------------------------------------------------------------
  /// \name   Multiple Material Assignment
  /// \brief  Functions used when there is more than one material 
  ///         assigned to a specific mesh. These allow you
  ///         to determine which face (indices) are assigned to which
  ///         materials.
  //@{

  /// \brief  returns true if this instance is actually a mesh with
  ///         a number of different materials applied to different faces
  /// \return true or false
  bool HasMaterialGroups() const;

  /// \brief  returns a pointer to the material applied to this geometry object.
  /// \return returns a pointer of type XMaterial
  XMaterial* GetMaterial(XU32 group) const;

  /// \brief  Sets the material applied to the surface
  /// \param  material  - pointer to the material to apply
  /// \return true if OK
  bool SetMaterial(const XMaterial* material);

  /// \brief  Sets the material used in a material group applied to the surface
  /// \param  material - pointer to the material to apply
  /// \param  group - the index of the material group
  /// \return true if OK
  bool SetMaterial(const XMaterial* material, XU32 group);

  /// \brief  Returns the number of material groups in this instance
  /// \return the number of material groups
  XU32 GetNumMaterialGroups() const;

  /// \brief  Allows you to set the number of used material groups
  /// \param  num  -  the new number of material groups
  /// \return true if OK
  bool SetNumMaterialGroups(XU32 num);

  /// \brief  returns the face IDs contained within the specified material group.
  /// \param  group  -  the index of the material group
  /// \param  FaceIds  -  the returned array of face id 's
  /// \return true if OK
  bool GetFaces(XU32 group, XIndexList& FaceIds) const;

  /// \brief  Sets the number of faces in the material group
  /// \param  group  -  the index of the material group
  /// \param  FaceIds  -  the array of face id 's that use the material
  /// \return true if OK
  bool SetFaces(XU32 group, const XIndexList& FaceIds);

  //@}

  //--------------------------------------------------------------------------------------------------------------------
  /// \name   Basic Instance Information
  /// \brief  Returns the geometry and transforms used by this instance
  //@{

  /// \brief  returns a pointer to the geometry object
  XGeometry* GetGeometry() const;

  /// \brief  returns a pointer to the geometry object
  XShape* GetShape() const;

  /// \brief  returns a pointer to the geometry object
  XBone* GetParent() const;

  /// \brief  sets the shape to use on the instance. 
  /// \param  shape - the shape to set
  void SetShape(const XShape* shape);

  //@}

  //--------------------------------------------------------------------------------------------------------------------
  /// \name   UV set to texture map hookup
  /// \brief  provides functions to query which uv set is used by a given
  ///         XTexture node.
  //@{

  /// \brief  returns the name of the UV set which is used by the 
  ///         specified texture node. 
  /// \param  texture - the texture for which you want to find the UV set for. 
  /// \return the uv set name or "" if the texture is not used by the
  ///         geometry instance.
  const XChar* GetUvSetForTexture(const XTexture* texture) const;

  /// \brief  assigns a mapping between the specified texture node, and
  ///         the provided UV set. This function will fail if the uv_set
  ///         name does not exist on the instanced mesh, or if the instance
  ///         represents a shape that is not a mesh. 
  /// \param  texture - the texture node
  /// \param  uv_set - the name of the UV set that the texture uses.
  /// \return true if OK, false if the uv set name does not exist on the
  ///         instanced mesh, or the instance does not represent a mesh.
  bool AssignTextureAndUvSet(const XTexture* texture, const XChar* uv_set);

  //@}

#ifndef DOXYGEN_PROCESSING

  /// \brief  This function writes the data for the chunk. The header,
  ///      name, extra attributes and surrounding braces will have
  ///      been written already.
  /// \param  io  -  the output file stream
  /// \return  true if OK
  ///
  bool DoData(XFileIO& io);

  /// \brief  returns the number of bytes required to store this object on disk.
  /// \return  the data size of the node in bytes
  XU32 GetDataSize() const;

private:
  struct XUvLookup
  {
    /// the ID of the texture node that uses the UV set
    XId m_TextureNode;

    /// the name of the UV set used
    XString m_UvSet;
  };
  /// ctor
  XInstance(XModel*,const XShape*,const XBone*);
  /// a unique ID for this instance
  XId m_InstanceID;
  /// a single material is applied to this surface if this is not 0
  XU32 m_Material;
  /// the material groups that indicate what faces are assigned
  /// to a specific material. This only really applies to meshes
  /// If only one material is applied to an object, then the
  /// m_Material member will hold it's ID
  XMaterialGroupList m_Groups;
  /// the ID of the geometry object
  XId m_Geometry;
  /// the ID of the parent of this instance
  XId m_Parent;
  /// a mapping between the uv set and the texture that uses it
  XM2::pod_vector<XUvLookup*> m_UvSets;
  /// a pointer to the model used by this instance
  mutable XModel* m_pModel;
  /// the name of the instance
  XString m_Name;
#endif
};
/// an array of shape instances
typedef XM2::pod_vector<XInstance*> XInstanceList;
}
