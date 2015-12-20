//----------------------------------------------------------------------------------------------------------------------
/// \file   VertexArray.h
/// \author Rob Bateman [http://robthebloke.org]
/// \brief  defines a class to convert an XMD::XMesh to a vertex array
/// \note   (C) Copyright 2003-2005 Robert Bateman. All rights reserved.
//----------------------------------------------------------------------------------------------------------------------
#pragma once
//----------------------------------------------------------------------------------------------------------------------
#include "XMD/XFn.h"
#include "XM2/RealArray.h"
#include "XM2/Vector3Array.h"
#include "XM2/vector.h"
//----------------------------------------------------------------------------------------------------------------------
#ifndef DOXYGEN_PROCESSING
namespace XMD
{
  class XMD_EXPORT XBase;
  class XMD_EXPORT XMesh;
}
#endif
//----------------------------------------------------------------------------------------------------------------------
namespace XMU  
{
//----------------------------------------------------------------------------------------------------------------------
/// \brief  This utility class converts the XMesh descriptions (where 
///         each vtx-face contains separate indices 
//----------------------------------------------------------------------------------------------------------------------
class XMD_EXPORT XVertexArray 
{
public:

  /// \brief  ctor
  /// \param  ptr  -  the mesh handle
  /// \note   generates all of the vertex array info for the mesh. This will assert if the object is not a mesh...
  XVertexArray(XMD::XBase* ptr);

  /// \brief  ctor
  /// \param  ptr - the mesh handle
  /// \param  world - the world space transform matrix to apply to the mesh
  /// \note   generates all of the vertex array info for the mesh, the difference being that the data will 
  ///         be in world space as specified by the instances transform
  XVertexArray(XMD::XBase* ptr,const XM2::XMatrix& world);

  /// \brief  returns the number of vertex array positions
  /// \return num of vertex array elements
  XM2::XU32 GetNumVertices() const;

  /// \brief  returns the number of indices
  /// \return num of vertex array indices
  XM2::XU32 GetNumIndices() const;

  /// \brief  returns the number of uv sets used by the model
  /// \return num of uv sets in the vertex array
  XM2::XU32 GetNumUvSets() const;

  /// \brief  returns the vertex data
  /// \return a constant reference to the internal vertex array
  const XM2::XRealArray& Vertices() const;

  /// \brief  returns the normal data
  /// \return a constant reference to the internal normals array
  const XM2::XRealArray& Normals() const;

  /// \brief  returns the bi-normal data
  /// \return a constant reference to the internal bi-normals array
  const XM2::XRealArray& BiNormals() const;

  /// \brief  returns the tangent data
  /// \return a constant reference to the internal tangent array
  const XM2::XRealArray& Tangents() const;

  /// \brief  returns the first set of UV's
  /// \return a constant reference to the internal tex coordinate array
  const XM2::XRealArray& Uvs0() const;

  /// \brief  returns the second set of UV's
  /// \return a constant reference to the internal tex coordinate array
  /// 
  const XM2::XRealArray& Uvs1() const;

  /// \brief  returns the third set of UV's
  /// \return a constant reference to the internal tex coordinate array
  const XM2::XRealArray& Uvs2() const;

  /// \brief  returns the fourth set of UV's
  /// \return a constant reference to the internal tex coordinate array
  const XM2::XRealArray& Uvs3() const;

  /// \brief  returns the UV set indexed by the parameter uv_set
  /// \return a const pointer to the internal tex coordinates array index by uv_set
  const XM2::XRealArray* GetUvs(const XMD::XU32 uv_set) const;

  /// \brief  returns the triangle indices
  /// \return a constant reference to the internal face index array
  const XMD::XU32Array& Indices() const;

  /// \brief  returns the array of mappings from the vertex array indices back to it's original point indices. 
  ///         ie, Mappings()[10] would return a mapping for the 11th vertex array element back to the original 
  ///         point in the XMesh node. 
  /// \return a constant reference to the internal normal mappings array
  const XMD::XU32Array& Mappings() const;

  /// \brief  returns  an array of indices and counts of normals that are shared and may need merging. 
  /// \return a constant reference to the internal face index array
  const XMD::XU32Array& NormalFixes() const;

  /// \brief  returns a list of vertex ID's that correspond to a list of face ID's provided.
  /// \note   Get the face indices AFTER the mesh has been triangulated!
  void FacesToIndices(const XMD::XIndexList& FaceIds, XMD::XIndexList& VertexIds) const;

  /// \brief  allows you to get the indices for the triangles in the vertex array data
  /// \param  indices - the returned indices
  void GetIndices(XMD::XU32Array& indices) const;

  /// \brief  allows you to get the indices for the triangles in the vertex array data
  /// \param  indices - the returned indices
  void GetIndices(XMD::XU16Array& indices) const;

  /// \brief  returns the array of mappings from the vertex array indices back to it's original point 
  ///         indices. ie, Mappings()[10] would return a mapping for the 11th vertex array element back to
  ///         the original point in the XMesh node. 
  /// \param  indices - the returned indices
  void GetMappings2(XM2::XU32* indices) const;

  /// \brief  returns the array of mappings from the vertex array indices back to it's original point indices. 
  ///         ie, Mappings()[10] would return a mapping for the 11th vertex array element back to
  ///         the original point in the XMesh node. 
  /// \param  indices - the returned indices
  /// 
  void GetMappings2(XM2::XU16* indices) const;

  /// \brief  If you need to re-calculate the surface normals and tangents for a vertex array, you need to be 
  ///         able to correctly merge normals at UV borders. To do this, this function generates a byte stream of 
  ///         normal fixes. Basically, a byte will indicate the number of indices that follow. Each set of indices 
  ///         refer to a set of normals that should be averaged because they will not calculate properly.
  /// \param  ppdata - a pointer to the returned byte stream
  /// \param  nBytes - the actual number of bytes within the stream
  /// \param  is32bit - a flag that will indicate if the contents of the stream are in fact 32 bit or 16 bit.
  void GetNormalFixesByteStream(XM2::XU8** ppdata, XM2::XU32& nBytes, bool& is32bit) const;

  /// \brief  Automatically calculates the normals on this set of vertex array data
  void GenerateNormals();

  /// \brief  generates the Tangents, Bi-normals & Normals for the polygon data (if UV's are present, else just the normals)
  void GenerateTBN();

  /// \brief  Sets the internal vertex data. 
  /// \param  vertexData - the new vertex data
  /// \note   This probably isn't all that useful
  void SetVertexData(const XM2::XRealArray& vertexData);

private:
#ifndef DOXYGEN_PROCESSING
  bool GenerateNormalFixes(XMD::XMesh* pm);

  /// \brief  During the construction of the vertex array, we want to re-use previous vertex array positions. 
  ///         This basically checks to see if its been used before
  XMD::XU32 GetElem(XM2::XU32, XM2::XReal* v, XM2::XReal* n, XM2::XReal* t = 0, XM2::XReal* b = 0, XM2::XReal* t0 = 0,
                    XM2::XReal* t1 = 0, XM2::XReal* t2 = 0, XM2::XReal* t3 = 0);

  /// the vertex array data
  XM2::XRealArray m_VertexData;
  XM2::XRealArray m_NormalData;
  XM2::XRealArray m_TangentData;
  XM2::XRealArray m_BiNormalData;
  XM2::XRealArray m_UvData0;
  XM2::XRealArray m_UvData1;
  XM2::XRealArray m_UvData2;
  XM2::XRealArray m_UvData3;
  XMD::XU32Array m_Indices;
  XMD::XU32Array m_Mappings;
  XMD::XU32Array m_NormalFixes;

  // temp. used during creation
  XM2::vector<XMD::XIndexList> mInternalMap;
#endif
};
};
