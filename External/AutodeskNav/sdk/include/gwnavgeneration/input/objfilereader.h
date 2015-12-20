/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// primary contact: JAPA - secondary contact: GUAL
#ifndef GwNavGen_ObjFileReader_H
#define GwNavGen_ObjFileReader_H


#include "gwnavruntime/math/vec3f.h"
#include "gwnavruntime/containers/kyarray.h"
#include "gwnavruntime/kernel/SF_String.h"
#include "gwnavruntime/math/box3f.h"
#include "gwnavruntime/kernel/SF_File.h"

namespace Kaim
{

class FileOpenerBase;
class CoordSystem;

/// A basic parser of .obj files in text format able to read triangles.
class ObjFileReader
{
	KY_DEFINE_NEW_DELETE_OPERATORS(Stat_Default_Mem)
	KY_CLASS_WITHOUT_COPY(ObjFileReader)
public:
	ObjFileReader();

	~ObjFileReader();

	KyResult ReadObjFile(Ptr<File> objFileToRead);

	// ---------------------------------- Parsed data ----------------------------------
	KyUInt32 VerticesCount() const { return m_vertices.GetCount();   }
	const Vec3f* Vertices()  const { return m_vertices.GetDataPtr(); }

	KyUInt32 IndicesCount()   const { return m_indices.GetCount();   }
	const KyUInt32* Indices() const { return m_indices.GetDataPtr(); }

	KyUInt32 GetFacesCount() const { return m_facesCount; }
	KyUInt32 GetFaceMinVertexCount() const { return m_faceMinVertexCount; }
	KyUInt32 GetFaceMaxVertexCount() const { return m_faceMaxVertexCount; }

	// ---------------------------------- Misc ----------------------------------
	void Clear();

	/// Will browse all the vertices extracted from file and compute a bounding box from them
	Box3f ComputeBoundingBox() const;
	Box3f ComputeBoundingBox(const CoordSystem& coordSystem) const;

	const char* GetFileName() const { return m_file != KY_NULL ? m_file->GetFilePath() : KY_NULL; }
	
private:
	KyResult ReadPosition(const char* str, Vec3f& pos);
	KyResult ReadFace(const char* str);
	KyResult ReadFaceFast(const char* str);

	KyResult ReadContent();

	KyResult ReadNextChunk();
	char* ReadLine();

private:
	char*        m_line;
	char*        m_fileIOBuffer;
	const char*  m_endOfReadFileChunk;
	char*        m_currentPosInBuffer;
	Ptr<File>    m_file;

	const KyUInt32 m_maxLineSize;
	const KyUInt32 m_maxBufferSize;

	KyUInt32 m_facesCount;
	KyUInt32 m_faceMinVertexCount;
	KyUInt32 m_faceMaxVertexCount;

	// current file data
	KyArray<Vec3f>       m_vertices;
	KyArrayPOD<KyUInt32> m_indices;
	KyArrayPOD<KyUInt32> m_faceIndices;
};


}


#endif

