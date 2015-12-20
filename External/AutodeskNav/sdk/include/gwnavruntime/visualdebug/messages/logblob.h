/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// primary contact: BRGR - secondary contact: NOBODY
#ifndef Navigation_LogBlob_H
#define Navigation_LogBlob_H

#include "gwnavruntime/blob/blobarray.h"
#include "gwnavruntime/blob/baseblobbuilder.h"

#include "gwnavruntime/kernel/SF_Log.h"
#include "gwnavruntime/visualdebug/visualdebugblobcategory.h"

namespace Kaim
{

class LogBlob
{
	KY_ROOT_BLOB_CLASS(VisualDebugMessage, LogBlob, 0)
public:
	LogBlob() 
		: m_text()
		, m_logMsgId(0) 
	{}

	BlobArray<char> m_text;
	KyUInt32 m_logMsgId;
};

inline void SwapEndianness(Endianness::Target e, LogBlob& self)
{
	SwapEndianness(e, self.m_text);
	SwapEndianness(e, self.m_logMsgId);
}

class LogBlobBuilder : public BaseBlobBuilder< LogBlob >
{
	KY_CLASS_WITHOUT_COPY(LogBlobBuilder)
public:
	LogBlobBuilder(const char* text, KyUInt32 textSize, LogMessageId logMsgId)
		: m_text(text)
		, m_textSize(textSize)
		, m_logMsgId(logMsgId)
	{}

private:
	virtual void DoBuild()
	{
		BLOB_STRING(m_blob->m_text, m_text);
		BLOB_SET(m_blob->m_logMsgId, (KyUInt32)m_logMsgId);
	}

	const char* m_text;
	KyUInt32 m_textSize;
	LogMessageId m_logMsgId;
};

} // namespace Kaim

#endif // Navigation_LogBlob_H
