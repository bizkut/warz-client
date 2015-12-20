/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


#ifndef Navigation_CollisionRayCastQueryBlob_H
#define Navigation_CollisionRayCastQueryBlob_H

#include "gwnavruntime/queries/blobs/queryblobcategory.h"
#include "gwnavruntime/queries/blobs/querydynamicoutputblob.h"
#include "gwnavruntime/collision/collisionraycastquery.h"

namespace Kaim
{

class CollisionRayCastQueryOutputBlob
{
	KY_DEFINE_NEW_DELETE_OPERATORS(Stat_Default_Mem)
public:
	CollisionRayCastQueryOutputBlob() : m_result(0) {}

	CollisionRayCastResult GetResult() const { return (CollisionRayCastResult)m_result; }
public:
	KyUInt32 m_result;
};
inline void SwapEndianness(Endianness::Target e, CollisionRayCastQueryOutputBlob& self)
{
	SwapEndianness(e, self.m_result                   );
}


class CollisionRayCastQueryOutputBlobBuilder : public BaseBlobBuilder<CollisionRayCastQueryOutputBlob>
{
public:
	CollisionRayCastQueryOutputBlobBuilder(CollisionRayCastQuery* query) : m_query(query) {}
	virtual void DoBuild()
	{
		BLOB_SET(m_blob->m_result, (KyUInt32)m_query->GetResult());
	}

private:
	CollisionRayCastQuery* m_query;
};

class CollisionRayCastQueryBlob
{
	KY_DEFINE_NEW_DELETE_OPERATORS(Stat_Default_Mem)
	KY_ROOT_BLOB_CLASS(Query, CollisionRayCastQueryBlob, 0)
public:
	CollisionRayCastQueryBlob() : m_queryInfoId(KyUInt32MAXVAL), m_databaseIdx(KyUInt32MAXVAL) {}

public:
	KyUInt32 m_queryInfoId;

	KyUInt32 m_databaseIdx;
	Vec3f m_startPos3f;
	Vec3f m_endPos3f;

	BlobRef<CollisionRayCastQueryOutputBlob> m_queryOutput;
};
inline void SwapEndianness(Endianness::Target e, CollisionRayCastQueryBlob& self)
{
	SwapEndianness(e, self.m_queryInfoId);
	SwapEndianness(e, self.m_databaseIdx);
	SwapEndianness(e, self.m_startPos3f);
	SwapEndianness(e, self.m_endPos3f);

	SwapEndianness(e, self.m_queryOutput);
}



class CollisionRayCastQueryBlobBuilder : public BaseBlobBuilder<CollisionRayCastQueryBlob>
{
public:
	CollisionRayCastQueryBlobBuilder(CollisionRayCastQuery* query) : m_query(query) {}
	virtual void DoBuild()
	{
		BLOB_SET(m_blob->m_queryInfoId, m_query->m_queryInfoId);

		if (m_query->m_database != KY_NULL)
		{
			BLOB_SET(m_blob->m_databaseIdx                , m_query->m_database->GetDatabaseIndex());
			BLOB_SET(m_blob->m_startPos3f                 , m_query->GetStartPos());
			BLOB_SET(m_blob->m_endPos3f                   , m_query->GetDestPos());

			switch (m_query->GetResult())
			{
			case RayDidNotHit :
			case RayHit :
				BUILD_REFERENCED_BLOB(m_blob->m_queryOutput, CollisionRayCastQueryOutputBlobBuilder(m_query));
				break;
			default:
				break;
			}
		}
	}

private:
	CollisionRayCastQuery* m_query;
};
}

#endif // Navigation_CollisionRayCastQueryBlob_H
