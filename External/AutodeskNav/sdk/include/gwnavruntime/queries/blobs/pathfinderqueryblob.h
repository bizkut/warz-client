/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


#ifndef Navigation_PathFinderQueryBlob_H
#define Navigation_PathFinderQueryBlob_H

#include "gwnavruntime/queries/blobs/queryblobcategory.h"
#include "gwnavruntime/queries/ipathfinderquery.h"
namespace Kaim
{

class PathFinderQueryBlob
{
	KY_ROOT_BLOB_CLASS(Query, PathFinderQueryBlob, 0)
	
public:
	PathFinderQueryBlob() : m_queryType(KyUInt32MAXVAL), m_result((KyUInt32)IPathFinderQuery::PathFinderNotInitialized), m_isPathfinderWithDest(0) {}

	Vec3f m_startPos3f;
	Vec3f m_destPos3f;
	KyUInt32 m_queryType;
	KyUInt32 m_result;
	BlobArray<char> m_errorMessage;
	KyUInt8 m_isPathfinderWithDest; // 0(false) or 1(true)
};
inline void SwapEndianness(Endianness::Target e, PathFinderQueryBlob& self)
{
	SwapEndianness(e, self.m_startPos3f);
	SwapEndianness(e, self.m_destPos3f);
	SwapEndianness(e, self.m_queryType);
	SwapEndianness(e, self.m_result);
	SwapEndianness(e, self.m_errorMessage);
	SwapEndianness(e, self.m_isPathfinderWithDest);
}

class PathFinderInfoBlobBuilder: public BaseBlobBuilder<PathFinderQueryBlob>
{
	KY_CLASS_WITHOUT_COPY(PathFinderInfoBlobBuilder)

public:
	PathFinderInfoBlobBuilder(const IPathFinderQuery* pathFinderQuery): m_pathFinderQuery(pathFinderQuery) {}
	~PathFinderInfoBlobBuilder() {}

private:
	virtual void DoBuild()
	{
		if (m_pathFinderQuery != NULL)
		{
			BLOB_SET(m_blob->m_startPos3f, m_pathFinderQuery->m_startPos3f);
			const Vec3f* dest = m_pathFinderQuery->GetDestination();
			if (dest == KY_NULL)
			{
				BLOB_SET(m_blob->m_isPathfinderWithDest, 1);
				BLOB_SET(m_blob->m_destPos3f, Vec3f(KyFloat32MAXVAL, KyFloat32MAXVAL, KyFloat32MAXVAL));
			}
			else
			{
				BLOB_SET(m_blob->m_isPathfinderWithDest, 1);
				BLOB_SET(m_blob->m_destPos3f, *dest);
			}

			BLOB_SET(m_blob->m_queryType, (KyUInt32)m_pathFinderQuery->GetType());
			BLOB_SET(m_blob->m_result, (KyUInt32)m_pathFinderQuery->GetPathFinderResult());
			String text;
			m_pathFinderQuery->GetPathFinderTextResult(text);
			BLOB_STRING(m_blob->m_errorMessage, text.ToCStr());
		}
	}

private:
	const IPathFinderQuery* m_pathFinderQuery;
};


}

#endif // Kaim_PathFinderQueryInputBlob_H
