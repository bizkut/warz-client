/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// primary contact: LASI - secondary contact: GUAL
#ifndef Navigation_NavGraphArray_H
#define Navigation_NavGraphArray_H

#include "gwnavruntime/navgraph/blobs/navgraphblob.h"

namespace Kaim
{

class NavGraphArray
{
	KY_ROOT_BLOB_CLASS(NavData, NavGraphArray, 0)
	KY_CLASS_WITHOUT_COPY(NavGraphArray)

public: 
	NavGraphArray() {}

	KyUInt32 ComputeVertexCount() const
	{
		KyUInt32 count = 0;
		const BlobRef<NavGraphBlob>* navGraphs = m_navGraphs.GetValues();
		for (KyUInt32 graphIdx = 0; graphIdx < m_navGraphs.GetCount(); ++graphIdx)
		{
			count += navGraphs[graphIdx].Ptr()->GetVertexCount();
		}
		return count;
	}

	KyUInt32 ComputeEdgeCount() const
	{
		KyUInt32 count = 0;
		const BlobRef<NavGraphBlob>* navGraphs = m_navGraphs.GetValues();
		for (KyUInt32 graphIdx = 0; graphIdx < m_navGraphs.GetCount(); ++graphIdx)
		{
			count += navGraphs[graphIdx].Ptr()->GetEdgeCount();
		}
		return count;
	}

	KyUInt32 ComputeNavTagCount() const
	{
		KyUInt32 count = 0;
		const BlobRef<NavGraphBlob>* navGraphs = m_navGraphs.GetValues();
		for (KyUInt32 graphIdx = 0; graphIdx < m_navGraphs.GetCount(); ++graphIdx)
		{
			count += navGraphs[graphIdx].Ptr()->GetNavTagCount();
		}
		return count;
	}

	KyUInt32 ComputeVertexToConnectCount() const
	{
		KyUInt32 count = 0;
		const BlobRef<NavGraphBlob>* navGraphs = m_navGraphs.GetValues();
		for (KyUInt32 graphIdx = 0; graphIdx < m_navGraphs.GetCount(); ++graphIdx)
		{
			count += navGraphs[graphIdx].Ptr()->GetVertexToConnectCount();
		}
		return count;
	}

public:
	KyUInt32 m_visualDebugId;
	KyUInt32 m_databaseIndex;
	BlobArray<BlobRef<NavGraphBlob> > m_navGraphs;
};
inline void SwapEndianness(Endianness::Target e, NavGraphArray& self)
{
	SwapEndianness(e, self.m_visualDebugId);
	SwapEndianness(e, self.m_databaseIndex);
	SwapEndianness(e, self.m_navGraphs);
}

} // namespace Kaim


#endif // Navigation_NavGraphArray_H

