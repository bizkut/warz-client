/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/

// Primary contact: MAMU - secondary contact: NOBODY
#ifndef Navigation_NavGraph_LinkInfo_Blob_H
#define Navigation_NavGraph_LinkInfo_Blob_H

#include "gwnavruntime/blob/blobref.h"
#include "gwnavruntime/blob/blobarray.h"
#include "gwnavruntime/math/vec3f.h"
#include "gwnavruntime/math/triangle3f.h"
#include "gwnavruntime/blob/blobtypes.h"
#include "gwnavruntime/navdata/navdatablobcategory.h"

namespace Kaim
{

class NavGraphLinkInfoBlob;
class NavGraphVertexLinkInfoBlob;
class NavGraphVertexSpatializationBlob;
class NavData;

enum NavGraphVertexLinkStatus
{
	NAVGRAPHVERTEX_NOTLINKABLE,
	NAVGRAPHVERTEX_NOTLINKED,
	NAVGRAPHVERTEX_LINKED
};

class NavGraphLinkInfoBlob
{
	KY_CLASS_WITHOUT_COPY(NavGraphLinkInfoBlob)

public:
	NavGraphLinkInfoBlob() {}

public:
	KyUInt32 m_graphIndex; // The graph index in the navdata
	BlobArray<NavGraphVertexLinkInfoBlob > m_verticesLinkInfo;
};
inline void SwapEndianness(Endianness::Target e, NavGraphLinkInfoBlob& self)
{
	SwapEndianness(e, self.m_graphIndex);
	SwapEndianness(e, self.m_verticesLinkInfo);
}


class NavGraphVertexLinkInfoBlob
{
	KY_CLASS_WITHOUT_COPY(NavGraphVertexLinkInfoBlob)

public:
	NavGraphVertexLinkInfoBlob() {}

public:
	KyUInt32 m_vertexStatus; // Cast this to NavGraphVertexLinkStatus
	Vec3f m_vertexPosition;
	BlobRef<NavGraphVertexSpatializationBlob> m_spatialization;
};
inline void SwapEndianness(Endianness::Target e, NavGraphVertexLinkInfoBlob& self)
{
	SwapEndianness(e, self.m_vertexStatus);
	SwapEndianness(e, self.m_vertexPosition);
	SwapEndianness(e, self.m_spatialization);
}


class NavGraphVertexSpatializationBlob
{
	KY_CLASS_WITHOUT_COPY(NavGraphVertexSpatializationBlob)

public:
	NavGraphVertexSpatializationBlob() {}

public:
	Triangle3f m_projectedTriangle;    // The graph index in the navdata
	Vec3f m_attachPoint;               // When the vertex is outside, this is the point where the vertex is attached to the navmesh
};
inline void SwapEndianness(Endianness::Target e, NavGraphVertexSpatializationBlob& self)
{
	SwapEndianness(e, self.m_projectedTriangle);
	SwapEndianness(e, self.m_attachPoint);
}


class NavGraphArrayLinkInfoBlob
{
	KY_ROOT_BLOB_CLASS(NavData, NavGraphArrayLinkInfoBlob, 0)
	KY_CLASS_WITHOUT_COPY(NavGraphArrayLinkInfoBlob)

public:
	NavGraphArrayLinkInfoBlob() {}

public:
	KyUInt32 m_visualDebugId;
	BlobArray<NavGraphLinkInfoBlob > m_graphArray;
};
inline void SwapEndianness(Endianness::Target e, NavGraphArrayLinkInfoBlob& self)
{
	SwapEndianness(e, self.m_visualDebugId);
	SwapEndianness(e, self.m_graphArray);
}

}

#endif