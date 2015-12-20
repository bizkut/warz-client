//----------------------------------------------------------------------------------------------------------------------
/// \file   MeshVertexColours.h
/// \author Rob Bateman [http://robthebloke.org]
/// \brief  defines the XMeshVertexColours class which can contain a set of mesh normals
//----------------------------------------------------------------------------------------------------------------------
#pragma once
//----------------------------------------------------------------------------------------------------------------------
#include "XMD/Base.h"
#include "XM2/ColourArray.h"
//----------------------------------------------------------------------------------------------------------------------
namespace XMD
{
#ifndef DOXYGEN_PROCESSING
class XMD_EXPORT XMesh;
#endif

//----------------------------------------------------------------------------------------------------------------------
/// \class XMD::XMeshVertexColours
/// \brief This class defines an additional set of vertex colours that are assigned to a mesh.
//----------------------------------------------------------------------------------------------------------------------
class XMD_EXPORT XMeshVertexColours
  : public XBase
{
#ifndef DOXYGEN_PROCESSING
  typedef XM2::vector< XS32Array > XColorFaceIndices;

  /// internal storage for a colour set
  struct XColourSet
  {
    /// name of the set
    XString m_ColourSetName;
    /// the colours contained in the set
    XColourArray m_Colours;
    /// the face indices in the set
    XColorFaceIndices m_FaceIndices;

    /// \brief  returns the number of bytes required to store this object on disk.
    /// \return the data size of the node in bytes
    XU32 GetDataSize() const;
  };

  XMD_CHUNK(XMeshVertexColours);
#endif
public:

  //--------------------------------------------------------------------------------------------------------------------
  /// \name   Mesh Assignment Information
  /// \brief  Sets/Gets the mesh that this vertex colour data applies to.
  //@{

  /// \brief  sets the mesh on which this colour applies.
  /// \param  mesh   - the mesh to assign this set to
  /// \return this will modify the stored
  void SetMesh(const XBase* mesh);

  /// \brief  returns the mesh to which this colour set applies.
  /// \return the mesh
  XMesh* GetMesh() const;

  //@}

  //--------------------------------------------------------------------------------------------------------------------
  /// \name   Colour Set Data
  /// \brief  Sets/Gets the vertex colours
  //@{

  /// \brief  returns the number of colour sets on the mesh
  /// \return the number of colour sets on the mesh
  XU32 GetNumberOfColourSets() const;

  /// \brief  sets the number of colour sets on the mesh
  /// \param  num - the number of colour sets on the mesh
  /// \return true if OK.
  bool SetNumberOfColourSets(XU32 num);

  /// \brief  returns the name assigned to the colour set
  /// \param  set_index   - the index to query
  /// \return returns the name of the specified colour set
  const XChar* GetSetName(XU32 set_index) const;

  /// \brief  returns the name assigned to the colour set
  /// \param  set_index   - the index to query
  /// \return returns the name of the specified colour set
  bool SetSetName(XU32 set_index, const XChar* name);

  /// \brief  returns the array of colours specified on the colour set
  /// \param  set_index   - the index to query
  /// \return an array of colours for the set
  const XColourArray& GetColourArray(XU32 set_index) const;

  /// \brief  returns the name assigned to the colour set
  /// \param  set_index   - the index to query
  /// \return returns the name of the specified colour set
  bool SetColourArray(XU32 set_index, const XColourArray& colours);

  /// \brief  returns the colour indices for the specified face
  /// \param  set_index   - the index to query
  /// \param  face_index - the index of the face to query the colours on
  /// \return the set of indices for the requested face
  const XS32Array& GetIndexArray(XU32 set_index, XU32 face_index) const;

  /// \brief  returns the colour indices for the specified face
  /// \param  set_index   - the index to query
  /// \param  face_index - the index of the face to query the colours on
  /// \return the set of indices for the requested face
  bool SetIndexArray(XU32 set_index, XU32 face_index, const XS32Array& indices);

  /// \brief  returns the colour indices for the specified face
  /// \param  set_index   - the index to query
  /// \param  face_index - the index of the face to query the colours on
  /// \return the set of indices for the requested face
  bool GetIndexArray(XU32 set_index, XS32Array& indices) const;

  /// \brief  returns the colour indices for the specified face
  /// \param  set_index   - the index to query
  /// \param  face_index - the index of the face to query the colours on
  /// \return the set of indices for the requested face
  bool SetIndexArray(XU32 set_index, const XS32Array& indices);

  //@}

private:
#ifndef DOXYGEN_PROCESSING
  /// the ID of the mesh this chunk is associated with
  XId m_Mesh;

  /// the vertex colour sets applied to the mesh.
  XM2::pod_vector<XColourSet*> m_ColourSets;
#endif
};
#ifndef DOXYGEN_PROCESSING
DECLARE_CHUNK_TYPE(XFn::MeshVertexColours, XMeshVertexColours, "mv", "MESH_VTX_COLOURS");
#endif
}
