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
#include "XM2/Colour.h"
//----------------------------------------------------------------------------------------------------------------------
namespace XMD
{
class XMD_EXPORT XPolygonMesh;
class XMD_EXPORT XColour3;
struct XMD_EXPORT XVertexSet;
struct XMD_EXPORT XIndexSet;

//----------------------------------------------------------------------------------------------------------------------
/// \brief  this class provides a slightly simpler way to iterate over
///         the mesh polygons, and provides easier access to the polygonal
///         data than by using the XPolygonMesh interface alone.
//----------------------------------------------------------------------------------------------------------------------
class XMD_EXPORT XItPolygon
{
public:
#ifndef DOXYGEN_PROCESSING
friend class XMD_EXPORT XPolygonMesh;
#endif

  /// \brief  ctor. initializes the iterator to the specified mesh.
  /// \param  poly_mesh - the mesh to iterate over
  XItPolygon(XPolygonMesh* poly_mesh);

  //--------------------------------------------------------------------------------------------------------------------
  /// \name   Iteration methods
  //@{

  /// \brief returns true if this iterator is referencing a valid XPolygonMesh node.
  bool IsValid() const;

  /// \brief  moves the iterator to the next polygon in the mesh
  void Next();

  /// \brief  moves the iterator to the previous polygon in the mesh  
  void Previous();

  /// \brief  resets the iterator to the first face in the XPolygonMesh 
  void Reset();

  /// \brief  returns true if the iterator has iterated through all polygons
  /// \return true if iteration is done.
  bool IsDone() const;

  /// \brief  returns the current face index within the polygon mesh
  /// \return true if iteration is done.
  XU32 GetFaceIndex() const;

  /// \brief  sets the iterator to the specified face index.
  /// \param  face_index - the face index to set the face to
  bool SetToFace(XU32 face_index);
  //@}

  //--------------------------------------------------------------------------------------------------------------------
  /// \name   Polygonal Face information
  //@{

  /// \brief  returns the area of this polygon.
  /// \return the polygon's area
  XReal GetArea() const;
  /// \brief  returns the number of points in the mesh
  /// \return the number of points.
  XU32 GetNumPoints() const;
  /// \brief  returns true if this face is planar
  /// \return true if all vertices lie on the same plane, false otherwise
  bool IsPlanar(XReal tolerance=0.00001f) const;
  //@}

  //--------------------------------------------------------------------------------------------------------------------
  /// \name   Vertex Data access
  //@{

  /// \brief  returns the vertex value at the specified face index
  /// \param  point - the returned vertex
  /// \param  face_vertex_index - the vertex index of vertex to query
  ///         in the range 0 to (GetNumPoints()-1)
  /// \return true if the data was retrieved correctly, false otherwise.
  bool GetVertex(XVector3& point, XU32 face_vertex_index) const;

  /// \brief  returns the vertex normal value at the specified vertex index
  /// \param  normal - the returned vertex normal
  /// \param  face_vertex_index - the vertex index of vertex to query
  ///         in the range 0 to (GetNumPoints()-1)
  /// \return true if the data was retrieved correctly, false otherwise.
  bool GetNormal(XVector3& normal, XU32 face_vertex_index) const;

  /// \brief  returns the vertex tangent value at the specified vertex index
  /// \param  normal - the returned vertex normal
  /// \param  face_vertex_index - the vertex index of vertex to query
  ///         in the range 0 to (GetNumPoints()-1)
  /// \return true if the data was retrieved correctly, false otherwise.
  bool GetTangent(XVector3& tangent, XU32 face_vertex_index) const;

  /// \brief  returns the vertex bi-normal value at the specified vertex index
  /// \param  normal - the returned vertex normal
  /// \param  face_vertex_index - the vertex index of vertex to query
  ///         in the range 0 to (GetNumPoints()-1)
  /// \return true if the data was retrieved correctly, false otherwise.
  bool GetBiNormal(XVector3& binormal, XU32 face_vertex_index) const;

  /// \brief  returns the vertex colour value at the specified vertex
  ///         face index as an RGBA colour
  /// \param  colour - the returned vertex colour
  /// \param  face_vertex_index - the vertex index of vertex to query
  ///         in the range 0 to (GetNumPoints()-1)
  /// \return true if the data was retrieved correctly, false otherwise.
  bool GetColour(XColour& colour, XU32 face_vertex_index) const;

  /// \brief  returns the UV texture coordinate at the specified vertex
  ///         face index as a 1D uv coordinate
  /// \paran  uv - the returned uv coordinate
  /// \param  face_vertex_index - the vertex index of vertex to query
  ///         in the range 0 to (GetNumPoints()-1)
  /// \return true if the data was retrieved correctly, false otherwise.
  bool GetUvCoord(XReal& uv, XU32 face_vertex_index) const;

  /// \brief  returns the UV texture coordinate at the specified vertex
  ///         face index as a 2D uv coordinate
  /// \paran  uv - the returned uv coordinate
  /// \param  face_vertex_index - the vertex index of vertex to query
  ///         in the range 0 to (GetNumPoints()-1)
  /// \return true if the data was retrieved correctly, false otherwise.
  bool GetUvCoord(XVector2& uv, XU32 face_vertex_index) const;

  /// \brief  returns the UV texture coordinate at the specified vertex
  ///         face index as a 3D uv coordinate
  /// \paran  uv - the returned uv coordinate
  /// \param  face_vertex_index - the vertex index of vertex to query
  ///         in the range 0 to (GetNumPoints()-1)
  /// \return true if the data was retrieved correctly, false otherwise.
  bool GetUvCoord(XVector3& uv, XU32 face_vertex_index) const;

  /// \brief  returns the UV texture coordinate at the specified vertex
  ///         face index as a 4D uv coordinate
  /// \paran  uv - the returned uv coordinate
  /// \param  face_vertex_index - the vertex index of vertex to query
  ///         in the range 0 to (GetNumPoints()-1)
  /// \return true if the data was retrieved correctly, false otherwise.
  bool GetUvCoord(XVector4& uv, XU32 face_vertex_index) const;

  /// \brief  returns the vertex colour value at the specified vertex
  ///         face index as an RGBA colour
  /// \paran  colour - the returned vertex colour
  /// \param  face_vertex_index - the vertex index of vertex to query
  ///         in the range 0 to (GetNumPoints()-1)
  /// \param  colour_set_index - the index of the colour set to get the
  ///         colour from.
  /// \return true if the data was retrieved correctly, false otherwise.
  bool GetColour(XColour& colour, XU32 face_vertex_index, XU32 colour_set_index) const;

  /// \brief  returns the UV texture coordinate at the specified vertex
  ///         face index as a 1D uv coordinate
  /// \paran  uv - the returned uv coordinate
  /// \param  face_vertex_index - the vertex index of vertex to query
  ///         in the range 0 to (GetNumPoints()-1)
  /// \param  uv_set_index - the index of the uv set to get the
  ///         texture coordinate from.
  /// \return true if the data was retrieved correctly, false otherwise.
  bool GetUvCoord(XReal& uv, XU32 face_vertex_index, XU32 uv_set_index) const;

  /// \brief  returns the UV texture coordinate at the specified vertex
  ///         face index as a 2D uv coordinate
  /// \paran  uv - the returned uv coordinate
  /// \param  face_vertex_index - the vertex index of vertex to query
  ///         in the range 0 to (GetNumPoints()-1)
  /// \param  uv_set_index - the index of the uv set to get the
  ///         texture coordinate from.
  /// \return true if the data was retrieved correctly, false otherwise.
  bool GetUvCoord(XVector2& uv, XU32 face_vertex_index, XU32 uv_set_index) const;

  /// \brief  returns the UV texture coordinate at the specified vertex
  ///         face index as a 3D uv coordinate
  /// \paran  uv - the returned uv coordinate
  /// \param  face_vertex_index - the vertex index of vertex to query
  ///         in the range 0 to (GetNumPoints()-1)
  /// \param  uv_set_index - the index of the uv set to get the
  ///         texture coordinate from.
  /// \return true if the data was retrieved correctly, false otherwise.
  bool GetUvCoord(XVector3& uv, XU32 face_vertex_index, XU32 uv_set_index) const;

  /// \brief  returns the UV texture coordinate at the specified vertex
  ///         face index as a 4D uv coordinate
  /// \paran  uv - the returned uv coordinate
  /// \param  face_vertex_index - the vertex index of vertex to query
  ///         in the range 0 to (GetNumPoints()-1)
  /// \param  uv_set_index - the index of the uv set to get the
  ///         texture coordinate from.
  /// \return true if the data was retrieved correctly, false otherwise.
  bool GetUvCoord(XVector4& uv, XU32 face_vertex_index, XU32 uv_set_index) const;

  /// \brief  returns the vertex colour value at the specified vertex
  ///         face index as an RGBA colour
  /// \paran  colour - the returned vertex colour
  /// \param  face_vertex_index - the vertex index of vertex to query
  ///         in the range 0 to (GetNumPoints()-1)
  /// \param  colour_set_name - the name of the colour set to get the
  ///         colour from.
  /// \return true if the data was retrieved correctly, false otherwise.
  bool GetColour(XColour& colour, XU32 face_vertex_index, const XString& colour_set_name) const;

  /// \brief  returns the UV texture coordinate at the specified vertex
  ///         face index as a 1D uv coordinate
  /// \paran  uv - the returned uv coordinate
  /// \param  face_vertex_index - the vertex index of vertex to query
  ///         in the range 0 to (GetNumPoints()-1)
  /// \param  uv_set_name - the name of the uv set to get the
  ///         texture coordinate from.
  /// \return true if the data was retrieved correctly, false otherwise.
  bool GetUvCoord(XReal& uv, XU32 face_vertex_index, const XString& uv_set_name) const;

  /// \brief  returns the UV texture coordinate at the specified vertex
  ///         face index as a 2D uv coordinate
  /// \paran  uv - the returned uv coordinate
  /// \param  face_vertex_index - the vertex index of vertex to query
  ///         in the range 0 to (GetNumPoints()-1)
  /// \param  uv_set_name - the name of the uv set to get the
  ///         texture coordinate from.
  /// \return true if the data was retrieved correctly, false otherwise.
  bool GetUvCoord(XVector2& uv, XU32 face_vertex_index, const XString& uv_set_name) const;

  /// \brief  returns the UV texture coordinate at the specified vertex
  ///         face index as a 3D uv coordinate
  /// \paran  uv - the returned uv coordinate
  /// \param  face_vertex_index - the vertex index of vertex to query
  ///         in the range 0 to (GetNumPoints()-1)
  /// \param  uv_set_name - the name of the uv set to get the
  ///         texture coordinate from.
  /// \return true if the data was retrieved correctly, false otherwise.
  bool GetUvCoord(XVector3& uv, XU32 face_vertex_index, const XString& uv_set_name) const;

  /// \brief  returns the UV texture coordinate at the specified vertex
  ///         face index as a 4D uv coordinate
  /// \paran  uv - the returned uv coordinate
  /// \param  face_vertex_index - the vertex index of vertex to query
  ///         in the range 0 to (GetNumPoints()-1)
  /// \param  uv_set_name - the name of the uv set to get the
  ///         texture coordinate from.
  /// \return true if the data was retrieved correctly, false otherwise.
  bool GetUvCoord(XVector4& uv, XU32 face_vertex_index, const XString& uv_set_name) const;

  //@}
  //-------------------------------------------------------------------------------------------------------------------- 
  /// \name   Vertex Data Index access  
  //@{
  /// \brief  returns the vertex index for the vertex face specified
  /// \param  face_vertex_index - index of the vertex within this face,
  ///         i.e. zero to (GetNumPoints()-1).
  /// \return the vertex index
  XS32 GetVertexIndex(XU32 face_vertex_index) const;

  /// \brief  returns the normal index for the vertex face specified
  /// \param  face_vertex_index - index of the vertex within this face,
  ///         i.e. zero to (GetNumPoints()-1).
  /// \return the normal index
  XS32 GetNormalIndex(XU32 face_vertex_index) const;

  /// \brief  returns the binormal index for the vertex face specified
  /// \param  face_vertex_index - index of the vertex within this face,
  ///         i.e. zero to (GetNumPoints()-1).
  /// \return the binormal index
  XS32 GetBiNormalIndex(XU32 face_vertex_index) const;

  /// \brief  returns the tangent index for the vertex face specified
  /// \param  face_vertex_index - index of the vertex within this face,
  ///         i.e. zero to (GetNumPoints()-1).
  /// \return the tangent index
  XS32 GetTangentIndex(XU32 face_vertex_index) const;

  /// \brief  returns the colour index for the vertex face specified
  /// \param  face_vertex_index - index of the vertex within this face,
  ///         i.e. zero to (GetNumPoints()-1).
  /// \return the colour index
  /// \note   This will use the first colour set found on the mesh. It
  ///         is basically the same as calling:
  ///
  ///             GetColourIndex(face_vertex_index,0)
  XS32 GetColourIndex(XU32 face_vertex_index) const;

  /// \brief  returns the colour index for the vertex face specified
  /// \param  face_vertex_index - index of the vertex within this face,
  ///         i.e. zero to (GetNumPoints()-1).
  /// \param  colour_set_index - the zero based index of the colour set
  ///         to query
  /// \return the colour index
  XS32 GetColourIndex(XU32 face_vertex_index, XU32 colour_set_index) const;

  /// \brief  returns the colour index for the vertex face specified
  /// \param  face_vertex_index - index of the vertex within this face,
  ///         i.e. zero to (GetNumPoints()-1).
  /// \param  colour_set_name - the name of the colour set to query the
  ///         data from
  /// \return the colour index
  XS32 GetColourIndex(XU32 face_vertex_index, const XString& colour_set_name) const;

  /// \brief  returns the texture coordinate index for the vertex face
  ///         specified
  /// \param  face_vertex_index - index of the vertex within this face,
  ///         i.e. zero to (GetNumPoints()-1).
  /// \return the texture coordinate index
  /// \note   This will use the first uv set found on the mesh. It
  ///         is basically the same as calling:
  ///
  ///             GetUvCoordIndex(face_vertex_index,0)
  XS32 GetUvCoordIndex(XU32 face_vertex_index) const;

  /// \brief  returns the texture coordinate index for the vertex face
  ///         specified
  /// \param  face_vertex_index - index of the vertex within this face,
  ///         i.e. zero to (GetNumPoints()-1).
  /// \param  uv_set_index - the index of the texture coordinate data set
  ///         to pull the data from.
  /// \return the texture coordinate index
  XS32 GetUvCoordIndex(XU32 face_vertex_index, XU32 uv_set_index) const;

  /// \brief  returns the texture coordinate index for the vertex face
  ///         specified
  /// \param  face_vertex_index - index of the vertex within this face,
  ///         i.e. zero to (GetNumPoints()-1).
  /// \param  uv_set_name - the name of the texture coordinate data set
  ///         to pull the data from.
  /// \return the texture coordinate index
  XS32 GetUvCoordIndex(XU32 face_vertex_index, const XString& uv_set_name) const;

  //@}
  //--------------------------------------------------------------------------------------------------------------------  
  /// \name   Raw Data access  
  /// \brief  These methods provide a way of accessing the polygonal 
  ///         directly from the XVertexSet and XIndexSet classes. These 
  ///         methods are required to access vertex data that is described 
  ///         by the semantic XVertexData::kNone; or you simply want to 
  ///         handle all mesh data as generic sets.  
  //@{

  /// \brief  returns the vertex data element for the specified vertex
  ///         on this face.
  /// \param  face_vertex_index - index of the vertex on this face for
  ///         which you want the data. zero to GetNumPoints()-1
  /// \param  vertex_set_index - the index of the vertex set to retrieve
  ///         the data from
  /// \param  data - the returned data
  /// \return true if OK.
  bool GetVertexData(XU32 face_vertex_index, XU32 vertex_set_index, XReal& data) const;

  /// \brief  returns the vertex data element for the specified vertex
  ///         on this face.
  /// \param  face_vertex_index - index of the vertex on this face for
  ///         which you want the data. zero to GetNumPoints()-1
  /// \param  vertex_set_index - the index of the vertex set to retrieve
  ///         the data from
  /// \param  data - the returned data
  /// \return true if OK.
  bool GetVertexData(XU32 face_vertex_index, XU32 vertex_set_index, XVector2& data) const;

  /// \brief  returns the vertex data element for the specified vertex
  ///         on this face.
  /// \param  face_vertex_index - index of the vertex on this face for
  ///         which you want the data. zero to GetNumPoints()-1
  /// \param  vertex_set_index - the index of the vertex set to retrieve
  ///         the data from
  /// \param  data - the returned data
  /// \return true if OK.
  bool GetVertexData(XU32 face_vertex_index, XU32 vertex_set_index, XVector3& data) const;

  /// \brief  returns the vertex data element for the specified vertex
  ///         on this face.
  /// \param  face_vertex_index - index of the vertex on this face for
  ///         which you want the data. zero to GetNumPoints()-1
  /// \param  vertex_set_index - the index of the vertex set to retrieve
  ///         the data from
  /// \param  data - the returned data
  /// \return true if OK.
  bool GetVertexData(XU32 face_vertex_index, XU32 vertex_set_index, XVector4& data) const;

  /// \brief  returns the vertex data element for the specified vertex
  ///         on this face.
  /// \param  face_vertex_index - index of the vertex on this face for
  ///         which you want the data. zero to GetNumPoints()-1
  /// \param  vertex_set_index - the index of the vertex set to retrieve
  ///         the data from
  /// \param  data - the returned data
  /// \return true if OK.
  bool GetVertexData(XU32 face_vertex_index, XU32 vertex_set_index, XColour3& data) const;

  /// \brief  returns the vertex data element for the specified vertex
  ///         on this face.
  /// \param  face_vertex_index - index of the vertex on this face for
  ///         which you want the data. zero to GetNumPoints()-1
  /// \param  vertex_set_index - the index of the vertex set to retrieve
  ///         the data from
  /// \param  data - the returned data
  /// \return true if OK.
  bool GetVertexData(XU32 face_vertex_index, XU32 vertex_set_index, XColour& data) const;

  /// \brief  returns the vertex data element for the specified vertex
  ///         on this face.
  /// \param  face_vertex_index - index of the vertex on this face for
  ///         which you want the data. zero to GetNumPoints()-1
  /// \param  vertex_set_name - the name of the vertex set to retrieve
  ///         the data from
  /// \param  data - the returned data
  /// \return true if OK.
  bool GetVertexData(XU32 face_vertex_index, const XString& vertex_set_name, XReal& data) const;

  /// \brief  returns the vertex data element for the specified vertex
  ///         on this face.
  /// \param  face_vertex_index - index of the vertex on this face for
  ///         which you want the data. zero to GetNumPoints()-1
  /// \param  vertex_set_name - the name of the vertex set to retrieve
  ///         the data from
  /// \param  data - the returned data
  /// \return true if OK.
  bool GetVertexData(XU32 face_vertex_index, const XString& vertex_set_name, XVector2& data) const;

  /// \brief  returns the vertex data element for the specified vertex
  ///         on this face.
  /// \param  face_vertex_index - index of the vertex on this face for
  ///         which you want the data. zero to GetNumPoints()-1
  /// \param  vertex_set_name - the name of the vertex set to retrieve
  ///         the data from
  /// \param  data - the returned data
  /// \return true if OK.
  bool GetVertexData(XU32 face_vertex_index, const XString& vertex_set_name, XVector3& data) const;

  /// \brief  returns the vertex data element for the specified vertex
  ///         on this face.
  /// \param  face_vertex_index - index of the vertex on this face for
  ///         which you want the data. zero to GetNumPoints()-1
  /// \param  vertex_set_name - the name of the vertex set to retrieve
  ///         the data from
  /// \param  data - the returned data
  /// \return true if OK.
  bool GetVertexData(XU32 face_vertex_index, const XString& vertex_set_name, XVector4& data) const;

  /// \brief  returns the vertex data element for the specified vertex
  ///         on this face.
  /// \param  face_vertex_index - index of the vertex on this face for
  ///         which you want the data. zero to GetNumPoints()-1
  /// \param  vertex_set_name - the name of the vertex set to retrieve
  ///         the data from
  /// \param  data - the returned data
  /// \return true if OK.
  bool GetVertexData(XU32 face_vertex_index, const XString& vertex_set_name, XColour3& data) const;

  /// \brief  returns the vertex data element for the specified vertex
  ///         on this face.
  /// \param  face_vertex_index - index of the vertex on this face for
  ///         which you want the data. zero to GetNumPoints()-1
  /// \param  vertex_set_name - the name of the vertex set to retrieve
  ///         the data from
  /// \param  data - the returned data
  /// \return true if OK.
  bool GetVertexData(XU32 face_vertex_index, const XString& vertex_set_name, XColour& data) const;

  /// \brief  returns the vertex data element for the specified vertex
  ///         on this face.
  /// \param  face_vertex_index - index of the vertex on this face for
  ///         which you want the data. zero to GetNumPoints()-1
  /// \param  vertex_set - the vertex set to retrieve the data from
  /// \param  data - the returned data
  /// \return true if OK.
  bool GetVertexData(XU32 face_vertex_index, const XVertexSet& vertex_set, XReal& data) const;

  /// \brief  returns the vertex data element for the specified vertex
  ///         on this face.
  /// \param  face_vertex_index - index of the vertex on this face for
  ///         which you want the data. zero to GetNumPoints()-1
  /// \param  vertex_set - the vertex set to retrieve the data from
  /// \param  data - the returned data
  /// \return true if OK.
  bool GetVertexData(XU32 face_vertex_index, const XVertexSet& vertex_set, XVector2& data) const;

  /// \brief  returns the vertex data element for the specified vertex
  ///         on this face.
  /// \param  face_vertex_index - index of the vertex on this face for
  ///         which you want the data. zero to GetNumPoints()-1
  /// \param  vertex_set - the vertex set to retrieve the data from
  /// \param  data - the returned data
  /// \return true if OK.
  bool GetVertexData(XU32 face_vertex_index, const XVertexSet& vertex_set, XVector3& data) const;

  /// \brief  returns the vertex data element for the specified vertex
  ///         on this face.
  /// \param  face_vertex_index - index of the vertex on this face for
  ///         which you want the data. zero to GetNumPoints()-1
  /// \param  vertex_set - the vertex set to retrieve the data from
  /// \param  data - the returned data
  /// \return true if OK.
  bool GetVertexData(XU32 face_vertex_index, const XVertexSet& vertex_set, XVector4& data) const;

  /// \brief  returns the vertex data element for the specified vertex
  ///         on this face.
  /// \param  face_vertex_index - index of the vertex on this face for
  ///         which you want the data. zero to GetNumPoints()-1
  /// \param  vertex_set - the vertex set to retrieve the data from
  /// \param  data - the returned data
  /// \return true if OK.
  bool GetVertexData(XU32 face_vertex_index, const XVertexSet& vertex_set, XColour3& data) const;

  /// \brief  returns the vertex data element for the specified vertex
  ///         on this face.
  /// \param  face_vertex_index - index of the vertex on this face for
  ///         which you want the data. zero to GetNumPoints()-1
  /// \param  vertex_set - the vertex set to retrieve the data from
  /// \param  data - the returned data
  /// \return true if OK.
  bool GetVertexData(XU32 face_vertex_index, const XVertexSet& vertex_set, XColour& data) const;

  /// \brief  returns the specified index for the face_vertex_index'th
  ///         index in this face from the provided index set
  /// \param  face_vertex_index - index of the vertex in the face to query
  /// \param  index_set - the index set to extract the data from
  /// \return the requested index
  XS32 GetIndex(XU32 face_vertex_index, const XIndexSet* index_set) const;

  /// \brief  returns the specified index for the face_vertex_index'th
  ///         index in this face from the provided vertex set
  /// \param  face_vertex_index - index of the vertex in the face to query
  /// \param  vertex_set - the vertex set to extract the index for
  /// \return the requested index
  XS32 GetIndex(XU32 face_vertex_index, const XVertexSet* vertex_set) const;
  //@}

private:
  /// the mesh this class is iterating over
  XPolygonMesh* m_Mesh;

  /// the current face index  
  XU32 m_CurrentFaceIndex;

  /// the current vertex face index.
  XU32 m_CurrentVertexIndex;

  /// the number of vertices in this current face
  XU32 m_NumVertices;
};
}
