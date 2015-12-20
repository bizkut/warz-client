//----------------------------------------------------------------------------------------------------------------------
/// \file   Mesh.h
/// \author Rob Bateman [http://robthebloke.org]
/// \brief  defines the geometry class to handle a mesh.
/// \note   (C) Copyright 2003-2005 Robert Bateman. All rights reserved.
//----------------------------------------------------------------------------------------------------------------------
#pragma once
//----------------------------------------------------------------------------------------------------------------------
#include "XMD/Geometry.h"
#include "XM2/Colour.h"
#include "XM2/Vector3Array.h"
#include "XMD/TexPlacement2D.h"
//----------------------------------------------------------------------------------------------------------------------
#ifndef DOXYGEN_PROCESSING
// forward declarations
namespace XMU 
{
  class XMD_EXPORT XVertexArray;
}
#endif
namespace XMD
{
#ifndef DOXYGEN_PROCESSING
class XMD_EXPORT XMeshVertexColours;
class XMD_EXPORT XBasicLogger;
#endif

//----------------------------------------------------------------------------------------------------------------------
/// \class XMD::XTexCoord
/// \brief a class to hold a uv texture coord
//----------------------------------------------------------------------------------------------------------------------
class XMD_EXPORT XTexCoord
{
  friend std::istream &operator>>(std::istream&,XTexCoord&);

public:

  /// \brief  ctor
  XTexCoord();

  /// \brief  ctor
  /// \param  a - the u texture co-ordinate
  /// \param  b - the v texture co-ordinate
  XTexCoord(XReal a, XReal b);

  /// \brief  dtor
  ~XTexCoord();

  /// \brief  copy ctor
  XTexCoord(const XTexCoord& v_);

  /// \brief  assignment operator
  /// \param  tc - the texture coordinate to copy
  /// \return a const reference to this
  const XTexCoord& operator = (const XTexCoord& tc);

  /// \brief  equivalence operator
  /// \param  tc - the texture co-ordinate to compare to this
  /// \return true if they are the same
  bool operator == (const XTexCoord& tc) const;

  union
  {
    struct 
    {
      XReal u;
      XReal v;
    };
    XReal data[2];

  };
};
/// \brief An array of 2D texture coordinates
typedef XM2::pod_vector<XTexCoord> XTexCoordList;

//----------------------------------------------------------------------------------------------------------------------
/// \class XMD::XPointIndex
/// \brief this internal structure is used to package all indices into 12bytes. 
//----------------------------------------------------------------------------------------------------------------------
struct XMD_EXPORT XPointIndex
{
  /// \brief  stream extraction operator
  /// \param  ifs - the input stream
  /// \param  pi - the point index to read
  /// \return the input stream
  friend std::istream &operator>>(std::istream& ifs, XPointIndex& pi);

  /// \brief  stream insertion operator
  /// \param  ofs - the output stream
  /// \param  pi - the point index to read
  /// \return the output stream
  friend std::ostream &operator<<(std::ostream& ofs, XPointIndex& pi);

  /// \brief  ctor 
  XPointIndex();

  /// \brief  copy ctor
  /// \param  v_ - the point index to copy
  XPointIndex(const XPointIndex& v_);

  /// \brief  assignment operator
  /// \param  rhs - the point index to copy
  /// \return a const reference to this
  const XPointIndex& operator = (const XPointIndex& rhs);

  /// the vertex index
  XU32 v;

  /// the normal index
  XU32 n;

  /// the texture coordinate index
  XS32Array t;
#ifndef DOXYGEN_PROCESSING
  bool DoData(XFileIO& io);
#endif
};
/// \brief  An array of vertex normals
typedef XM2::XVector3Array XNormalList;

//----------------------------------------------------------------------------------------------------------------------
/// \class XMD::XFace
/// \brief a class to hold a face
//----------------------------------------------------------------------------------------------------------------------
class XMD_EXPORT XFace
{
  /// the mesh class is a friend...
  friend class XMesh;
public:

  /// ctor - sets the pointer to NULL for that is good...
  XFace();

  /// dtor
  ~XFace();

  /// ctor - sets the pointer to NULL for that is good...
  XFace(const XFace& f);

  /// \brief  this function sets the number of indices in the face
  /// \param  num_indices  -  number of indices in the face
  /// \param  num_uv_sets  -  number of uv sets used on the model
  void SetSize(XU32 num_indices, XU32 num_uv_sets = 1);

  /// \brief  This function returns the number of points making up the polygon face.
  /// \return the number of points making up the polygon
  XU32 NumPoints() const;

  /// \brief  This function returns the vertex index of the specified point on the polygon.
  /// \param  ind  -  the face point index
  /// \return the vertex index
  XU32 GetVertexIndex(XU32 ind) const;

  /// \brief  This function returns the normal index of the specified point on the polygon.
  /// \param  ind  -  the face point index
  /// \return the normal index
  XU32 GetNormalIndex(XU32 ind) const;

  /// \brief  This function returns the uv index of the specified
  ///         point on the polygon, for the specified uv set
  /// \param  ind  -  the face point index
  /// \param  Set  -  the uv set index
  /// \return the normal index
  XS32 GetUvIndex(XU32 ind, XU32 Set = 0) const;

  /// \brief  This function sets the vertex index of the specified
  ///         point on the polygon.
  /// \param  ind  -  the face point index
  /// \param  idx - the vertex index
  void SetVertexIndex(XU32 ind, XU32 idx);

  /// \brief  This function sets the normal index of the specified
  ///         point on the polygon.
  /// \param  ind  -  the face point index
  /// \param  idx - the vertex index
  void SetNormalIndex(XU32 ind, XU32 idx);

  /// \brief  This function sets the uv index of the specified point on 
  ///         the polygon, for the specified uv set
  /// \param  ind  -  the face point index
  /// \param  idx - the vertex index
  /// \param  Set  -  the uv set index
  void SetUvIndex(XU32 ind, XS32 idx, XU32 Set = 0);

  /// \brief  reverses the polygon windings 
  void ReverseWinding();

protected:
  
  /// \brief  used by XMesh to delete the specified UV set info on this face
  /// \param  idx - the UV set to delete.
  void DeleteUVSet(XU32 idx);
#ifndef DOXYGEN_PROCESSING

  bool DoData(XFileIO&);
  bool Read(std::istream&,XU32);
  void Write(std::ostream&) const;

  /// the point indices for the face
  XM2::vector<XPointIndex> m_Points;
#endif
};
/// \brief An array of polygonal faces
typedef XM2::vector<XFace> XFaceList;

//----------------------------------------------------------------------------------------------------------------------
/// \class XMD::XMesh
/// \brief This class defines the main mesh class. This class is responsible
///        for rendering, loading and saving of mesh data
//----------------------------------------------------------------------------------------------------------------------
class XMD_EXPORT XMesh
  : public XGeometry
{
  friend class XMU::XVertexArray;
  XMD_CHUNK(XMesh);
public:

  //--------------------------------------------------------------------------------------------------------------------
  // property ID's
  static const XU32 kLast = XGeometry::kLast;
  //--------------------------------------------------------------------------------------------------------------------

  //--------------------------------------------------------------------------------------------------------------------
  /// \name   Face Data
  //@{

  /// \brief  This function returns to you the number of faces in the mesh
  /// \return The number of faces
  XU32 GetNumFaces() const;

  /// \brief This function returns to you the number of faces in the mesh
  /// \param num_faces - the new number of faces in the mesh
  void SetNumFaces(XU32 num_faces);

  /// \brief   Returns the internal face array for this mesh 
  /// \return  reference to the face data
  const XFaceList& GetFaces() const;

  /// \brief   Returns the internal face array for this mesh 
  /// \return  reference to the face data
  XFaceList& GetFaces();

  /// \brief  Returns the points for this mesh so they can be deformed
  /// \return reference to the face data
  void SetFaces(const XFaceList& faces);

  /// \brief  This function triangulates the mesh
  void Triangulate();

  /// \brief  reverses the polygon windings for the mesh
  void ReversePolyWindings();

  //@}

  //--------------------------------------------------------------------------------------------------------------------
  /// \name   Vertex Normal Data
  //@{

  /// \brief  This function returns to you the num normals in the mesh
  /// \return The number of normals
  XU32 GetNumNormals() const;

  /// \brief  This function returns to you the normals for the mesh
  /// \return The normals as an indexed array.
  const XNormalList& GetNormals() const;

  /// \brief  This function returns to you the normals for the mesh
  /// \return The normals as an indexed array.
  const XNormalList& GetBiNormals() const;

  /// \brief  This function returns to you the normals for the mesh
  /// \return The normals as an indexed array.
  const XNormalList& GetTangents() const;

  /// \brief  This function returns to you normals in world space for
  ///         with respect to the specific instance & bone
  /// \param  norms - the returned array of normals
  /// \param  bone  - the bone to make the normals relative to
  /// \param  instance  - the ID of the instance to get the normals for.
  /// \return The normals as an indexed array.
  void GetNormals(XNormalList &norms, const XBone* bone, const XInstance* instance) const;

  /// \brief  sets the current normal array
  /// \param  n  -  the normals
  void SetNormals(const XNormalList& n);

  /// \brief  Returns the tex coord's for this mesh so they can be read
  /// \param  keep_indices  -  if true the original indices are used,
  ///                if false, new indices are generated
  /// \param  uv_set      -  the UV set index to use for the
  ///                generation of bi-normals & tangents
  void CalculateNormals(bool keep_indices, XU32 uv_set = 0);

  //@}

  //--------------------------------------------------------------------------------------------------------------------
  /// \name   Texture Coordinate Data
  //@{

  /// \brief  checks to see if the specified UV set actually exists.
  /// \param  uv_set_name - the name of the uv set to look for.
  /// \return true if the UV set is found
  bool HasUvSet(const XChar* uv_set_name) const;

  /// \brief  This function returns the num of texture coord's in the
  ///         specified UV set.
  /// \param  uv_set  - the uv set from which to query the number of UVs.
  /// \return The number of texture coord's in the specified uv set
  XU32 GetNumTexCoords(XU32 uv_set = 0) const;

  /// \brief  Returns the tex coord's for this mesh so they can be read
  /// \return reference to the tex coordinate data
  const XTexCoordList& GetTexCoords(XU32 uv_set) const;

  /// \brief  Returns the name of the specified UV set
  /// \return the requested uv set name
  const XChar* GetUvSetName(XU32 uv_set) const;

  /// \brief  Returns the tex coord's for this mesh so they can be read
  /// \return reference to the tex coordinate data
  void SetTexCoords(XU32 uv_set, const XTexCoordList& uvs);

  /// \brief  Sets the current number of UV sets on the mesh. It's a
  ///      pretty good idea to Then set the UV's for the newly added
  ///      set. If you are removing a set, bear in mind that it will
  ///      be the last set you remove.
  ///
  ///      To say, remove the first UV set , i.e. set 0, you can copy
  ///      the UV set with index (NumUvSets*()-1) over set 0
  ///      using the CopyUvSet command.
  /// \param  count  -  the number of uv sets.
  void SetNumUvSets(XU32 count);

  /// \brief  Returns the number of uv sets available on this mesh
  /// \return the num of UV sets
  XU32 GetNumUvSets() const;

  /// \brief  Retrieves the index of the UV set associated with the given texture placement
  /// \return a bool, true if a uv set exists for the texture placement
  bool GetUvSetIndex(const XTexPlacement2D &placement, XS32 &uv_set_index) const;

  /// \brief  Returns the number of uv sets available on this mesh
  /// \return the num of UV sets
  bool CopyUvSet(XId original_set, XId to_replace_index);

  /// \brief  This function copies the specified UV set into a new UV
  ///      set - i.e., the call creates a new uv set which is placed as
  ///      the last UV set on the mesh. The specified UV set is then
  ///      copied into the newly created set.
  /// \param  original_set  -  uv set to copy
  /// \return  true if OK
  bool CopyUvSet(XId original_set);

  /// \brief  deletes the specified UV set
  /// \param  set_to_delete  -  speaks for itself.
  /// \return true if OK
  bool DeleteUvSet(XId set_to_delete);

  //@}

  //--------------------------------------------------------------------------------------------------------------------
  /// \name   Vertex Colour Data
  //@{

  /// \brief  returns the vertex colour data associated with this mesh 
  ///           - if it has colour data present.
  /// \return  a pointer to the colour data chunk
  XMeshVertexColours* GetVertexColours() const;

  /// \brief  returns the number of colour sets on the mesh 
  /// \return the number of colour sets contained within this class
  /// \note   It's far more efficient to call GetVertexColours() than 
  ///         query the vertex colours directly from this class. There 
  ///         is no direct reference stored in XMesh to the colour data, 
  ///         so be warned that this func will have to search through all
  ///         of the chunks in the XModel before it can find the colours. 
  ///         It's therefore better to do it once !
  XU32 GetNumberOfColourSets() const;

  /// \brief  returns the name assigned to the colour set
  /// \param  set_index   - the index to query
  /// \return returns the name of the specified colour set
  /// \note   It's far more efficient to call GetVertexColours() than 
  ///         query the vertex colours directly from this class. There 
  ///         is no direct reference stored in XMesh to the colour data, 
  ///         so be warned that this func will have to search through all
  ///         of the chunks in the XModel before it can find the colours. 
  ///         It's therefore better to do it once !
  const XChar* GetColourSetName(XU32 set_index) const;

  /// \brief  returns the array of colours specified on the colour set
  /// \param  set_index   - the index to query
  /// \return an array of colours for the set
  /// \note   It's far more efficient to call GetVertexColours() than 
  ///         query the vertex colours directly from this class. There 
  ///         is no direct reference stored in XMesh to the colour data, 
  ///         so be warned that this func will have to search through all
  ///         of the chunks in the XModel before it can find the colours. 
  ///         It's therefore better to do it once !
  const XColourArray& GetColourArray(XU32 set_index) const;

  /// \brief  returns the colour indices for the specified face
  /// \param  set_index   - the index to query
  /// \param  face_index   - the index to query
  /// \return the set of indices for the requested face
  /// \note   It's far more efficient to call GetVertexColours() than 
  ///         query the vertex colours directly from this class. There 
  ///         is no direct reference stored in XMesh to the colour data, 
  ///         so be warned that this func will have to search through all
  ///         of the chunks in the XModel before it can find the colours. 
  ///         It's therefore better to do it once !
  const XS32Array& GetColourIndexArray(XU32 set_index, XU32 face_index) const;

  //@}

  //--------------------------------------------------------------------------------------------------------------------
  /// \name   Flattened Face Index Information
  /// \brief  Some packages, e.g. Maya and dotXSI, require data as 
  ///         flat lists of indices. These are some simple utilities
  ///         to perform that conversion for you. 
  //@{

  /// \brief  Returns an array containing a list of int's for each face.
  ///         Each int stores the number of points used by the respective face
  /// \param  face_counts  -  the returned array of vertex counts
  template< typename TVector >
  void GetFaceCounts(TVector& face_counts) const;

  /// \brief  Returns the vertex indices as a single vector
  /// \param  point_inds  -  the point indices
  template< typename TVector >
  void GetPointIndices(TVector& point_inds) const;

  /// \brief  Returns the normal indices as a single vector
  /// \param  normal_inds  -  the normal indices
  template< typename TVector >
  void GetNormalIndices(TVector& normal_inds) const;

  /// \brief  Returns the UV indices as a single vector
  /// \param  uv_inds  -  the UV indices
  /// \param  set_  -  the UV set to query
  /// \return true if OK
  template< typename TVector >
  bool GetUvIndices(TVector& uv_inds, XU32 set_ = 0) const;

  //@}

private:
#ifndef DOXYGEN_PROCESSING
  /// an array of faces
  XFaceList m_Faces;
  /// the uv sets for the model
  XM2::vector< XTexCoordList >  m_TexCoords;
  /// the names of the UV sets
  XStringList m_UvSetNames;
  /// an array of normals
  XNormalList m_Normals;
  /// an array of tangents
  XNormalList m_Tangents;
  /// an array of bi-normals
  XNormalList m_BiNormals;
#endif
};
#ifndef DOXYGEN_PROCESSING
DECLARE_CHUNK_TYPE(XFn::Mesh,XMesh,"mesh","MESH");

//----------------------------------------------------------------------------------------------------------------------
template< typename TVector >
void XMesh::GetPointIndices(TVector& point_inds) const 
{
  XFaceList::const_iterator it = m_Faces.begin();
  size_t npoints = 0;
  for( ;it != m_Faces.end(); ++it ) {
    npoints += it->NumPoints();
  }
  point_inds.resize(npoints);
  it = m_Faces.begin();
  typename TVector::iterator itb = point_inds.begin();
  for( ;it != m_Faces.end(); ++it ) 
  {
    for(XU32 i=0;i<it->NumPoints();++i,++itb)
    {
      *itb = it->GetVertexIndex(i);
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
template< typename TVector >
void XMesh::GetNormalIndices(TVector& point_inds) const
{
  XFaceList::const_iterator it = m_Faces.begin();
  size_t npoints = 0;
  for( ;it != m_Faces.end(); ++it ) {
    npoints += it->NumPoints();
  }
  point_inds.resize(npoints);
  it = m_Faces.begin();
  typename TVector::iterator itb = point_inds.begin();
  for( ;it != m_Faces.end(); ++it ) 
  {
    for(XU32 i=0;i<it->NumPoints();++i,++itb)
    {
      *itb = it->GetNormalIndex(i);
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
template< typename TVector >
bool XMesh::GetUvIndices(TVector& point_inds, XU32 set_) const
{
  if(m_UvSetNames.size()<=set_)
    return false;

  XFaceList::const_iterator it = m_Faces.begin();
  size_t npoints = 0;
  for( ;it != m_Faces.end(); ++it ) {
    npoints += it->NumPoints();
  }
  point_inds.resize(npoints);

  it = m_Faces.begin();
  typename TVector::iterator itb = point_inds.begin();
  for( ;it != m_Faces.end(); ++it ) 
  {
    for(XU32 i=0;i<it->NumPoints();++i,++itb)
    {
      *itb = it->GetUvIndex(i,set_);
    }
  }
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
template< typename TVector >
void XMesh::GetFaceCounts(TVector& face_counts) const
{
  face_counts.resize( m_Faces.size() );
  XFaceList::const_iterator it = m_Faces.begin();
  typename TVector::iterator itb = face_counts.begin();
  for( ;it != m_Faces.end(); ++it,++itb ) 
  {
    *itb = static_cast<typename TVector::value_type>(it->NumPoints());
  }
}
#endif
}
