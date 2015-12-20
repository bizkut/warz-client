/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// primary contact: BRGR - secondary contact: NOBODY
#ifndef Navigation_FrameDumpBlob_H
#define Navigation_FrameDumpBlob_H

#include "gwnavruntime/blob/blobarray.h"
#include "gwnavruntime/blob/baseblobbuilder.h"

namespace Kaim
{

class FrameDumpBlob
{
	KY_ROOT_BLOB_CLASS(VisualDebugMessage, FrameDumpBlob, 0)
public:
	FrameDumpBlob() : m_text() {}
	BlobArray<char> m_text;
};

inline void SwapEndianness(Endianness::Target e, FrameDumpBlob& self)
{
	SwapEndianness(e, self.m_text);
}

class FrameDumpBlobBuilder : public BaseBlobBuilder<FrameDumpBlob>
{
	KY_CLASS_WITHOUT_COPY(FrameDumpBlobBuilder)
public:
	FrameDumpBlobBuilder(const char* text, KyUInt32 textSize)
		: m_text(text)
		, m_textSize(textSize)
	{}

private:
	virtual void DoBuild()
	{
		BLOB_STRING(m_blob->m_text, m_text);
	}

	const char* m_text;
	KyUInt32 m_textSize;
};

} // namespace Kaim

#endif // Navigation_FrameDumpBlob_H
