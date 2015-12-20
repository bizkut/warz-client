/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


#ifndef LabEngine_TextualCommandBlob_H
#define LabEngine_TextualCommandBlob_H


#include "gwnavruntime/visualdebug/visualdebugblobcategory.h"
#include "gwnavruntime/blob/baseblobbuilder.h"
#include "gwnavruntime/blob/blobtypes.h"
#include "gwnavruntime/kernel/SF_String.h"

namespace Kaim
{

class TextualCommandBlob
{
	KY_DEFINE_NEW_DELETE_OPERATORS(MemStat_VisualDebug)
	KY_ROOT_BLOB_CLASS(VisualDebugMessage, TextualCommandBlob, 0)

public:
	TextualCommandBlob()
		: m_command()
	{}

	~TextualCommandBlob() {}

public:
	Kaim::BlobArray<char> m_command;
};

inline void SwapEndianness(Kaim::Endianness::Target e, TextualCommandBlob& self)
{
	SwapEndianness(e, self.m_command);
}


class TextualCommandBlobBuilder : public Kaim::BaseBlobBuilder<TextualCommandBlob>
{
	KY_CLASS_WITHOUT_COPY(TextualCommandBlobBuilder)
public:
	TextualCommandBlobBuilder(Kaim::String& command) : m_command(command)
	{}
	  
	~TextualCommandBlobBuilder() {}

	virtual void DoBuild()
	{
		BLOB_STRING(m_blob->m_command, m_command.ToCStr());
	}

	const Kaim::String m_command;
};


}

#endif // LabEngine_TextualCommandBlob_H
