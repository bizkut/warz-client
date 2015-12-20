//----------------------------------------------------------------------------------------------------------------------
/// \file   PolygonMesh.h
/// \author Rob Bateman [http://robthebloke.org]
/// \brief  This is the replacement class for the deprecated XMesh class.The old
///         XMesh class was somewhat limited to the data that it could store. To
///         counter this problem, this class holds any number of index and vertex
///         sets for polygonal data
/// \note   (C) Copyright 2003-2005 Robert Bateman. All rights reserved. 
//----------------------------------------------------------------------------------------------------------------------
#pragma once
//----------------------------------------------------------------------------------------------------------------------
#include "XMD/Geometry.h"
#include "XM2/Colour.h"
#include "XM2/Vector2.h"
#include "XM2/Vector3.h"
#include "XM2/Vector4.h"
#include "XMD/TexPlacement2D.h"
//----------------------------------------------------------------------------------------------------------------------
#ifndef DOXYGEN_PROCESSING
namespace XMU
{
  class XMD_EXPORT XVertexArray;
}
#endif
namespace XMD
{
class XMD_EXPORT XPolygonMesh;;
struct XMD_EXPORT XIndexSet;
struct XMD_EXPORT XVertexSet;

//----------------------------------------------------------------------------------------------------------------------
/// \class XMD::XIndexSet
/// \brief This class hold the index data for one or more XVertexSet's. An
///        XPolygonMesh node makes use of this class to store indices for
///        separate data sets (i.e. vertices, normals, uvs etc).
//----------------------------------------------------------------------------------------------------------------------
struct XMD_EXPORT XIndexSet
{
  /// \name   Index Set methods
  //@{

  /// \brief  returns the name of the index set.
  /// \return the index set name
  const XChar* GetName() const;

  /// \brief  sets the name for the index set
  /// \param  name - the new name for the index set
  /// \return true if the name is OK. False if another index set already has this name
  bool SetName(const XChar* name);

  /// \brief  array operator to access elements of the index set
  /// \param  index - the index to return
  /// \return the index value
  XS32& operator[](XU32 index);

  /// \brief  array operator to access elements of the index set
  /// \param  index - the index to return
  /// \return the index value
  XS32 operator[](XU32 index) const;

  /// \brief  returns the number of indices in the index set
  /// \return the index array size
  XU32 GetSize() const;

  /// \brief  returns the number of XVertexSets that use these indices
  /// \return the number of vertex sets that use this index set
  XU32 GetNumVertexSets() const;

  /// \brief  returns the i'th XVertexSet that uses this index set
  /// \param  i - the index of the XVertexSet that uses this data. This
  ///         value must be between 0 and (GetNumVertexSets()-1).
  /// \return the i'th XVertexSet that uses this XIndexSet to provide
  ///         it's vertex indices.
  XVertexSet* GetVertexSet(XU32 i);

  /// \brief  returns the i'th XVertexSet that uses this index set
  /// \param  i - the index of the XVertexSet that uses this data. This
  ///         value must be between 0 and (GetNumVertexSets()-1).
  /// \return the i'th XVertexSet that uses this XIndexSet to provide
  ///         it's vertex indices.
  const XVertexSet* GetVertexSet(XU32 i) const;

  /// \brief  array operator to access elements of the index set
  /// \param  index - the index to return
  /// \return the index value
  XS32& GetIndex(XU32 index);

  /// \brief  array operator to access elements of the index set
  /// \param  index - the index to return
  /// \return the index value
  XS32 GetIndex(XU32 index) const;

  /// \brief  sets the index data 
  /// \param  array - the index data to set
  /// \return true if ok, false if the index count is not valid
  bool SetData(const XS32Array& array);

  //@}

private:
  bool DoData(XFileIO& io);
  XU32 GetDataSize() const;
  friend class XMD_EXPORT XPolygonMesh;
  XIndexSet(XPolygonMesh* mesh,const XChar* name,const XU32 size);
  XIndexSet(XPolygonMesh* mesh,const XU32 size);
  /// dtor. only mesh can access this
  ~XIndexSet() {}
  /// name of the index set
  XString m_Name;
  /// the indices in the index set
  XS32Array m_Indices;
  /// the mesh this set belongs to
  XPolygonMesh* m_Mesh;
};
typedef XM2::pod_vector<XIndexSet*> XIndexSetArray;

//----------------------------------------------------------------------------------------------------------------------
/// \class XMD::XVertexSet
/// \brief this class holds a vertex data element for a mesh (i.e. array of normals/verts etc)
//----------------------------------------------------------------------------------------------------------------------
struct XMD_EXPORT XVertexSet
{
  //--------------------------------------------------------------------------------------------------------------------
  /// \name   Vertex Data usage Semantic function
  //@{

  /// \brief  the semantic is used to mark vertex data sets with a
  ///         specific usage. i.e. is it a normal data set, or uv set etc.
  enum VertexSemantic
  {
    kNone,                ///< Data does not map to a semantic - user specified
    kVertex,              ///< Data is a vertex set. NOTE: you cannot create a vertex data set on an XPolygonMesh
    kFogCoord,            ///< Data used as a fog coordinate
    kTexCoord,            ///< Data used as texture coordinate
    kColour,              ///< Data used as vertex colour
    kNormal,              ///< Data used as vertex normal
    kBiNormal,            ///< Data used as a bi-normal
    kBiTangent=kBiNormal, ///< Same as bi normal.
    kTangent,             ///< Data used as a tangent
    kTBN                  ///< Data is a TBN matrix consisting of a Tangent, Bi-normal and normal
  };

  /// \brief  sets the usage semantic of this vertex set.
  /// \param  semantic - the vertex data usage semantic.
  void SetUsage(VertexSemantic semantic);

  /// \brief  returns the usage semantic of this vertex set.
  /// \return the vertex data usage semantic.
  VertexSemantic GetUsage() const;

  //@}

  //--------------------------------------------------------------------------------------------------------------------
  /// \name   Vertex Data set name
  //@{

  /// \brief  returns the name of the vertex set
  const XString& GetName() const;

  /// \brief  sets the name of the vertex data set.
  bool SetName(const XString& name);

  //@}

  //--------------------------------------------------------------------------------------------------------------------
  /// \name   Vertex Data size
  //@{

  /// \brief  returns the total number of elements stored in the vertex set
  /// \return the number of elements in the vertex array
  XU32 GetNumElements() const;

  /// \brief  returns the number of floats required for each vertex. For example,
  ///         normal vectors would be 3, TBN matrices would be 9, uv sets 2 etc.
  /// \return number of floats needed for each element
  XU32 GetElementSize() const;

  //@}

  //--------------------------------------------------------------------------------------------------------------------
  /// \name   Vertex Indicies manipulation
  //@{

  /// \brief  creates a new XIndexSet for this vertex set data. This is
  ///         basically the same as doing
  /// \code
  ///     XIndexSet* indices = XPolygonMesh::CreateIndexSet();
  ///     some_vertex_set->SetIndexSet(indices);
  /// \endcode
  /// \return the newly created index set
  XIndexSet* CreateIndexSet();

  /// \brief  assigns an index set to the vertex data
  /// \param  indices - the indices to set on the vertex data
  /// \return true if OK.
  bool SetIndexSet(XIndexSet* indices);

  /// \brief  returns the index set used by this vertex set or NULL
  ///         if this vertex set does not use indices.
  XIndexSet* GetIndexSet();

  /// \brief  returns the index set used by this vertex set or NULL
  ///         if this vertex set does not use indices.
  const XIndexSet* GetIndexSet() const;

  //@}

  //--------------------------------------------------------------------------------------------------------------------
  /// \name   Data set methods
  //@{

  /// \brief  fills the data stored in this vertex set from the
  ///         specified data array
  /// \param  data - the data to store
  /// \param  num_elements - num elements in the raw data array
  /// \param  element_size - the element size (i.e. num floats per data
  ///         element).
  /// \return true if OK.
  bool SetData(const XReal* data, XU32 num_elements, XU32 element_size);

  /// \brief  fills the data stored in this vertex set from the
  ///         specified data array
  /// \param  data - the data to store
  /// \return true if OK.
  bool SetData(const XRealArray& data);

  /// \brief  fills the data stored in this vertex set from the
  ///         specified data array
  /// \param  data - the data to store
  /// \return true if OK.
  bool SetData(const XVector2Array& data);

  /// \brief  fills the data stored in this vertex set from the
  ///         specified data array
  /// \param  data - the data to store
  /// \return true if OK.
  bool SetData(const XVector3Array& data);

  /// \brief  fills the data stored in this vertex set from the
  ///         specified data array
  /// \param  data - the data to store
  /// \return true if OK.
  bool SetData(const XVector4Array& data);

  /// \brief  fills the data stored in this vertex set from the
  ///         specified data array
  /// \param  data - the data to store
  /// \return true if OK.
  bool SetData(const XColourArray& data);

  /// \brief  fills the data stored in this vertex set from the
  ///         specified data array
  /// \param  data - the data to store
  /// \return true if OK.
  bool SetElement(const XReal& data, XU32 index);

  /// \brief  fills the data stored in this vertex set from the
  ///         specified data array
  /// \param  data - the data to store
  /// \return true if OK.
  bool SetElement(const XVector2& data, XU32 index);

  /// \brief  fills the data stored in this vertex set from the
  ///         specified data array
  /// \param  data - the data to store
  /// \return true if OK.
  bool SetElement(const XVector3& data, XU32 index);

  /// \brief  fills the data stored in this vertex set from the
  ///         specified data array
  /// \param  data - the data to store
  /// \return true if OK.
  bool SetElement(const XVector4& data, XU32 index);

  /// \brief  fills the data stored in this vertex set from the
  ///         specified data array
  /// \param  data - the data to store
  /// \return true if OK.
  bool SetElement(const XColour& data, XU32 index);

  //@}

  //--------------------------------------------------------------------------------------------------------------------
  /// \name   Data get methods
  //@{

  /// \brief  fills the data stored in this vertex set from the
  ///         specified data array
  /// \param  data - the data to store
  /// \return true if OK.
  bool GetData(XReal* data) const;

  /// \brief  fills the data stored in this vertex set from the
  ///         specified data array
  /// \param  data - the data to store
  /// \return true if OK.
  bool GetData(XRealArray& data) const;

  /// \brief  fills the data stored in this vertex set from the
  ///         specified data array
  /// \param  data - the data to store
  /// \return true if OK.
  bool GetData(XVector2Array& data) const;

  /// \brief  fills the data stored in this vertex set from the
  ///         specified data array
  /// \param  data - the data to store
  /// \return true if OK.
  bool GetData(XVector3Array& data) const;

  /// \brief  fills the data stored in this vertex set from the
  ///         specified data array
  /// \param  data - the data to store
  /// \return true if OK.
  bool GetData(XVector4Array& data) const;

  /// \brief  fills the data stored in this vertex set from the
  ///         specified data array
  /// \param  data - the data to store
  /// \return true if OK.
  bool GetData(XColourArray& data) const;

  /// \brief  fills the data stored in this vertex set from the
  ///         specified data array
  /// \param  data - the data to store
  /// \return true if OK.
  bool GetElement(XReal& data, XU32 index) const;

  /// \brief  fills the data stored in this vertex set from the
  ///         specified data array
  /// \param  data - the data to store
  /// \return true if OK.
  bool GetElement(XVector2& data, XU32 index) const;

  /// \brief  fills the data stored in this vertex set from the
  ///         specified data array
  /// \param  data - the data to store
  /// \return true if OK.
  bool GetElement(XVector3& data, XU32 index) const;

  /// \brief  fills the data stored in this vertex set from the
  ///         specified data array
  /// \param  data - the data to store
  /// \return true if OK.
  bool GetElement(XVector4& data, XU32 index) const;

  /// \brief  fills the data stored in this vertex set from the
  ///         specified data array
  /// \param  data - the data to store
  /// \return true if OK.
  bool GetElement(XColour& data, XU32 index) const;

  //@}

private:
  bool DoData(XFileIO& io);
  XU32 GetDataSize() const;
  friend class XMD_EXPORT XMU::XVertexArray;
  friend class XMD_EXPORT XPolygonMesh;
  XVertexSet(XPolygonMesh* mesh, const XChar* name, XU32 elem_size, XU32 num_elems);
  XVertexSet(XPolygonMesh* mesh);
  /// name of the index set
  XString m_Name;
  /// name of the index set
  XString m_IndexSetName;
  /// the indices in the index set
  XRealArray m_VertexData;
  /// the usage of this vertex set
  VertexSemantic m_Usage;
  /// the number of floats contained in each element
  XU32 m_ElementSize;
  /// the number of vertex data items contained in this array
  XU32 m_NumItems;
  /// the mesh this set belongs to
  XPolygonMesh* m_Mesh;
};
typedef XM2::pod_vector<XVertexSet*> XVertexSetArray;

//----------------------------------------------------------------------------------------------------------------------
/// \class XMD::XPolygonMesh
/// \brief defines a polygonal mesh
//----------------------------------------------------------------------------------------------------------------------
class XMD_EXPORT XPolygonMesh
  : public XGeometry
{
  friend class XMD_EXPORT XItPolygon;
  XMD_CHUNK(XPolygonMesh);
public:

  /// \brief  deletes all of the internal data sets
  void Clear();

  /// \brief  sets both the number of polygons within the mesh, and the
  ///         number of vertices each of those polygons have. For example,
  ///         a poly_counts array such as
  ///
  ///             { 3, 3, 4, 4 }
  ///
  ///         would mean that there would be 4 polygonal faces. The first
  ///         two will be triangles, the second two will be quads.
  ///           The vertex_indices array would then define all of the vertex
  ///         indices for all faces in one go. In the previous example
  ///         the number of required vertex indices are
  ///
  ///               3 + 3 + 4 + 4 == 14
  ///
  /// \param  poly_counts - the poly counts array
  /// \param  vertex_indices - the array of vertex indices for the mesh
  /// \return true if OK
  /// \note   This method is intended to be used as a construction method
  ///         only. If it is used when data is present on the node, then
  ///         all of that data will be deleted.
  bool SetPolyCounts(const XU32Array& poly_counts, const XS32Array& vertex_indices);

  /// \brief  gets the number of vertices for every poly in the mesh.
  /// \param  poly_counts - an array of vertex counts one element for
  ///         each poly
  void GetPolyCounts(XU32Array& poly_counts);

  /// \brief  returns the number of polygon faces in this mesh
  /// \return the number of faces in the mesh
  XU32 GetNumFaces() const;

  /// \brief  reverses the polygon windings for the mesh
  void ReversePolyWindings();

  //--------------------------------------------------------------------------------------------------------------------
  /// \name   Vertex Set methods
  //@{

  /// \brief  creates a vertex set.
  /// \param  name - the name of the set
  /// \param  num_vertices - the number of array elements the set should
  ///         contain.
  /// \param  num_elements - the number of floats that each data element
  ///         requires.
  /// \param  usage - an optional usage parameter to indicate the usage
  ///         of the data set (i.e. normals, uvs etc).
  /// \return the newly created data set
  XVertexSet* CreateVertexSet(const XString& name,
                              XU32 num_vertices,
                              XU32 num_elements,
                              const XVertexSet::VertexSemantic usage = XVertexSet::kNone);

  /// \brief  a quick utility function to determine whether the specified
  ///         vertex set actually belongs to this mesh.
  /// \param  vset - the vertex set to test
  /// \return true if the vertex data set is owned by this mesh
  bool IsSetValid(const XVertexSet* vset);

  /// \brief  returns the requested vertex set by its usage. This will
  ///         return the first data set found that matches the usage
  ///         semantic. Be aware that data sets such as uv sets and
  ///         vertex colour's may have more than just one set, so you
  ///         may find that this is not the appropriate GetVertexSet
  ///         implementation in those cases.
  /// \param  usage - the usage of the vertex set to return.
  /// \return the requested vertex data set
  XVertexSet* GetVertexSet(const XVertexSet::VertexSemantic& usage);

  /// \brief  returns the requested vertex set by its usage. This will
  ///         return the first data set found that matches the usage
  ///         semantic. Be aware that data sets such as uv sets and
  ///         vertex colour's may have more than just one set, so you
  ///         may find that this is not the appropriate GetVertexSet
  ///         implementation in those cases.
  /// \param  usage - the index of the set to return
  /// \return the requested vertex data set
  const XVertexSet* GetVertexSet(const XVertexSet::VertexSemantic& usage) const;

  /// \brief  returns the number of vertex sets contained within the
  ///         polygonal mesh
  /// \return the number of vertex data sets on the mesh
  XU32 GetNumVertexSets() const;

  /// \brief  returns the requested vertex set
  /// \param  index - the index of the set to return
  /// \return the requested vertex data set
  XVertexSet* GetVertexSet(XU32 index);

  /// \brief  returns the requested vertex set
  /// \param  index - the index of the set to return
  /// \return the requested vertex data set
  const XVertexSet* GetVertexSet(XU32 index) const;

  /// \brief  returns the requested vertex set
  /// \param  name - the name of the set to return
  /// \return the requested vertex data set
  XVertexSet* GetVertexSet(const XString& name);

  /// \brief  returns the requested vertex set
  /// \param  name - the name of the set to return
  /// \return the requested vertex data set
  const XVertexSet* GetVertexSet(const XString& name) const;

  /// \brief  removes the requested vertex set
  /// \param  set - the vertex set to remove
  /// \note   set will be set to null if successful
  /// \return true if removed
  bool RemoveVertexSet(XVertexSet* set);

  /// \brief  removes the requested vertex set by index
  /// \param  index - the index of the set to remove
  /// \return true if removed
  bool RemoveVertexSet(XU32 index);

  /// \brief  removes the requested vertex set by name
  /// \param  name - the name of the set to remove
  /// \return true if removed
  bool RemoveVertexSet(const XString& name);

  /// \brief  returns the number of vertex sets contained within the
  ///         polygonal mesh that are flagged with the
  ///         XVertexSet::kTexCoord usage flag
  /// \return the number of vertex data sets on the mesh used for UV coordinates
  XU32 GetNumUvSet() const;

  /// \brief  returns the requested vertex set
  /// \param  name - the name of the set to return
  /// \return the requested vertex data set
  XVertexSet* GetUvSet(XU32 index);

  /// \brief  returns the requested vertex set
  /// \param  name - the name of the set to return
  /// \return the requested vertex data set
  const XVertexSet* GetUvSet(XU32 index) const;

  /// \brief  returns the requested vertex set
  /// \param  name - the name of the set to return
  /// \return the requested vertex data set
  XVertexSet* GetUvSet(const XString& name);

  /// \brief  returns the requested vertex set
  /// \param  name - the name of the set to return
  /// \return the requested vertex data set
  const XVertexSet* GetUvSet(const XString& name) const;

  /// \brief  checks to see if the specified UV set actually exists.
  /// \param  uv_set_name - the name of the uv set to look for.
  /// \return true if the UV set is found
  bool HasUvSet(const XChar* uv_set_name) const;

  /// \brief  returns the number of vertex sets contained within the
  ///         polygonal mesh that are flagged with the
  ///         XVertexSet::kColour usage flag
  /// \return the number of vertex data sets on the mesh used for vertex colours
  XU32 GetNumColourSet() const;

  /// \brief  returns the requested vertex set
  /// \param  name - the name of the set to return
  /// \return the requested vertex data set
  XVertexSet* GetColourSet(XU32 index);

  /// \brief  returns the requested vertex set
  /// \param  name - the name of the set to return
  /// \return the requested vertex data set
  const XVertexSet* GetColourSet(XU32 index) const;

  /// \brief  returns the requested vertex set
  /// \param  name - the name of the set to return
  /// \return the requested vertex data set
  XVertexSet* GetColourSet(const XString& name);

  /// \brief  returns the requested vertex set
  /// \param  name - the name of the set to return
  /// \return the requested vertex data set
  const XVertexSet* GetColourSet(const XString& name) const;

  //@}

  //--------------------------------------------------------------------------------------------------------------------
  /// \name   Normal methods
  /// \brief  A set of funcs to ease access to normals, bi-normals and
  ///         tangents.
  //@{

  /// \brief  returns the requested vertex set
  /// \param  name - the name of the set to return
  /// \return the requested vertex data set
  XVertexSet* GetNormals();

  /// \brief  returns the requested vertex set
  /// \param  name - the name of the set to return
  /// \return the requested vertex data set
  const XVertexSet* GetNormals() const;

  /// \brief  returns the requested vertex set
  /// \param  name - the name of the set to return
  /// \return the requested vertex data set
  XVertexSet* GetTangents();

  /// \brief  returns the requested vertex set
  /// \param  name - the name of the set to return
  /// \return the requested vertex data set
  const XVertexSet* GetTangents() const;

  /// \brief  returns the requested vertex set
  /// \param  name - the name of the set to return
  /// \return the requested vertex data set
  XVertexSet* GetBiNormals();

  /// \brief  returns the requested vertex set
  /// \param  name - the name of the set to return
  /// \return the requested vertex data set
  const XVertexSet* GetBiNormals() const;

  //@}

  //--------------------------------------------------------------------------------------------------------------------
  /// \name   Index Set methods
  //@{

  /// \brief  returns the number of index sets on the mesh
  /// \return the num of index sets.
  XU32 GetNumIndexSets() const;

  /// \brief  creates an index set using the name provided as the name
  ///         for the index set.
  /// \param  name - the new name of the set
  /// \return the newly created index set.
  /// \note   the memory in the set will have been pre-allocated, so
  ///         you just need to set the data via the [] operators.
  XIndexSet* CreateIndexSet(const XString& name);

  /// \brief  a quick utility function to determine whether the specified
  ///         index set actually belongs to this mesh.
  /// \param  vset - the vertex set to test
  /// \return true if the vertex data set is owned by this mesh
  bool IsSetValid(const XIndexSet* vset) const;

  /// \brief  returns the points index set
  XIndexSet* GetPointsIndexSet();

  /// \brief  returns the points index set
  const XIndexSet* GetPointsIndexSet() const;

  /// \brief  returns the requested index set
  /// \param  index - the index of the set to return
  XIndexSet* GetIndexSet(XU32 index);

  /// \brief  returns the requested index set
  /// \param  index - the index of the set to return
  const XIndexSet* GetIndexSet(XU32 index) const;

  /// \brief  returns the requested index set
  /// \param  name - the name of the set to return
  XIndexSet* GetIndexSet(const XString& name);

  /// \brief  returns the requested index set
  /// \param  name - the name of the set to return
  const XIndexSet* GetIndexSet(const XString& name) const;

  /// \brief  removes the requested index set
  /// \param  set - the index set to remove
  /// \note   set will be set to null if successful
  /// \return true if removed
  bool RemoveIndexSet(XIndexSet* set);

  /// \brief  removes the requested index set by index
  /// \param  index - the index of the set to remove
  /// \return true if removed
  bool RemoveIndexSet(XU32 index);

  /// \brief  removes the requested index set by name
  /// \param  name - the name of the set to remove
  /// \return true if removed
  bool RemoveIndexSet(const XString& name);

  //@}

  //--------------------------------------------------------------------------------------------------------------------
  /// \name   Polygon utilities
  //@{

  /// \brief  triangulates the mesh data
  /// \return true if OK
  bool Triangulate();

  /// \brief  collapses data to separate index sets for all vertex sets.
  ///         If it is found that there are 2 or more sets that share the
  ///         same indices, then a single index set will be used for those.
  /// \note   When compressing points deformers are updated but not tested
  ///         during compression so may result in a loss of data.
  /// \return true if OK.
  bool Compress();

  /// \brief  expands the data to a vertex array format. If use_indices is
  ///         true, then a single index set will be used for all vertex sets.
  ///         If use_indices is false, then the data will be expanded out
  ///         and will contain NO index data at all. i.e. you can assume
  ///         that there will be 3 * num_tris number of points.
  /// \param  use_indices - true to generate a single set of indices,
  ///         false to completely flatten data. true is generally recommended.
  /// \return true if OK
  bool ExpandToVertexArray(bool use_indices = true);

  /// \brief  removes all degenerate polygonal faces (those with no area).
  /// \param  tolerance - the size of a zero error triangle.
  void DeleteDegenerateTriangles(XReal tolerance = 0.00001f);

  /// \brief  a function to calculate normals for the geometry mesh. This
  ///         can also optionally calculate tangents and bi-normals for you.
  ///         If no XVertexSets exist that are flagged with the semantic
  ///         kNormal, kBiNormal, or kTangent, then those sets will be created
  /// \param  uvset_name - optional name of uvset to use when calcuating
  ///         tangents and binormals, if none is specified or the set cannot
  ///         be found it will default to the first XVertexSet found with
  ///         kTexCoord VertexSemantic.
  void CalculateNormals(bool calculate_tangents = false,
                        bool calculate_bi_normals = false,
                        XString* uvset_name = 0);
  //@}

private:
#ifndef DOXYGEN_PROCESSING
  friend class XMD_EXPORT XMU::XVertexArray;
  /// the internal array identifying how many vertices there are in each face
  XU32Array m_PolyCounts;
  /// the number of required indices as determined by the poly counts array
  XU32 m_NumRequiredIndices;
  /// the vertex set data
  XVertexSetArray m_VertexSets;
  /// the index set data
  XIndexSetArray m_IndexSets;
  /// the indices used for the point data
  XIndexSet* m_PointSet;
#endif
};
#ifndef DOXYGEN_PROCESSING
DECLARE_CHUNK_TYPE(XFn::PolygonMesh, XPolygonMesh, "polygonMesh", "POLYGON_MESH");
#endif
}
