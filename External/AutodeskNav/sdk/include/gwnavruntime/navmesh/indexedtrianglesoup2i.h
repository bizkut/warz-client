/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// Primary contact: GUAL - secondary contact: NOBODY
#ifndef GwNavGen_IndexedTriangleSoup2i_H
#define GwNavGen_IndexedTriangleSoup2i_H


#include "gwnavruntime/navmesh/navmeshtypes.h"
#include "gwnavruntime/containers/kyarray.h"


namespace Kaim
{

class IndexedTriangleSoup2i
{
	KY_DEFINE_NEW_DELETE_OPERATORS(Kaim::Stat_Default_Mem)
public:
	IndexedTriangleSoup2i(MemoryHeap* heap)
		: m_vertices(heap)
		, m_verticesAltitudes(heap)
		, m_triangles(heap)
		, m_verticesTags(heap)
	{}

	void Clear()
	{
		m_vertices.Clear();
		m_verticesAltitudes.Clear();
		m_triangles.Clear();
		m_verticesTags.Clear();
	}

public:
	KyArrayDH<Vec2i> m_vertices;
	KyArrayDH_POD<KyFloat32> m_verticesAltitudes;
	KyArrayDH_POD<KyUInt32> m_triangles;
	KyArrayDH_POD<KyUInt32> m_verticesTags;
};

}


#endif
