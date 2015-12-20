/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


#ifndef Navigation_TimeSlicedQueryInfoBlob_H
#define Navigation_TimeSlicedQueryInfoBlob_H

#include "gwnavruntime/queries/blobs/queryblobcategory.h"
#include "gwnavruntime/querysystem/iquery.h"
#include "gwnavruntime/blob/baseblobbuilder.h"

namespace Kaim
{

class TimeSlicedQueryInfoBlob
{
	KY_DEFINE_NEW_DELETE_OPERATORS(Stat_Default_Mem)
public:
	TimeSlicedQueryInfoBlob() : 
		m_advanceCount(0),
		m_lastAdvanceFrameIdx(0)
	{}

	KyUInt32  m_advanceCount;
	KyUInt32  m_lastAdvanceFrameIdx;
};
inline void SwapEndianness(Endianness::Target e, TimeSlicedQueryInfoBlob& self)
{
	SwapEndianness(e, self.m_advanceCount         );
	SwapEndianness(e, self.m_lastAdvanceFrameIdx  );
}

class TimeSlicedQueryBlobBuilder : public BaseBlobBuilder<TimeSlicedQueryInfoBlob>
{
public:
	TimeSlicedQueryBlobBuilder(ITimeSlicedQuery* query) { m_query = query; }
	virtual void DoBuild()
	{
		BLOB_SET(m_blob->m_advanceCount         , m_query->m_advanceCount         );
		BLOB_SET(m_blob->m_lastAdvanceFrameIdx  , m_query->m_lastAdvanceFrameIdx  );
	}

private:
	ITimeSlicedQuery* m_query;
};
}

#endif // Navigation_TimeSlicedQueryInfoBlob_H
