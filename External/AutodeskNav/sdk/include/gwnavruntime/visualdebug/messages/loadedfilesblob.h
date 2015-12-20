/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/

#ifndef LabEngine_LoadedFilesBlob_H
#define LabEngine_LoadedFilesBlob_H


#include "gwnavruntime/visualdebug/visualdebugblobcategory.h"
#include "gwnavruntime/blob/baseblobbuilder.h"
#include "gwnavruntime/blob/blobtypes.h"
#include "gwnavruntime/containers/kyarray.h"
#include "gwnavruntime/kernel/SF_String.h"

namespace Kaim
{

class LoadedFilesBlob
{
	KY_ROOT_BLOB_CLASS(VisualDebugMessage, LoadedFilesBlob, 1)

public:
	LoadedFilesBlob() {}

	BlobArray<BlobArray<char> > m_fileNames;
};

inline void SwapEndianness(Endianness::Target e, LoadedFilesBlob& self)
{
	SwapEndianness(e, self.m_fileNames);
}

class LoadedFilesBlobBuilder : public BaseBlobBuilder<LoadedFilesBlob>
{
	KY_CLASS_WITHOUT_COPY(LoadedFilesBlobBuilder)
public:
	LoadedFilesBlobBuilder(const KyArray<String>& fileNames) : m_fileNames(fileNames)
	{}

	~LoadedFilesBlobBuilder() {}

	virtual void DoBuild()
	{
		KyUInt32 filenamesCount = m_fileNames.GetCount();
		BlobArray<char>* filenames = BLOB_ARRAY(m_blob->m_fileNames, filenamesCount);
		for (KyUInt32 filenameIdx = 0; filenameIdx < filenamesCount; ++filenameIdx)
		{
			BLOB_STRING(filenames[filenameIdx], m_fileNames[filenameIdx].ToCStr());
		}
	}

	const KyArray<String> m_fileNames;
};

}

#endif