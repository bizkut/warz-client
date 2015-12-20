/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// primary contact: MAMU - secondary contact: NOBODY
#ifndef Navigation_DatabaseBindingBlob_H
#define Navigation_DatabaseBindingBlob_H

#include "gwnavruntime/world/runtimeblobcategory.h"
#include "gwnavruntime/world/databasebinding.h"
#include "gwnavruntime/database/database.h"


namespace Kaim
{

class DatabaseBindingBlob
{
	KY_CLASS_WITHOUT_COPY(DatabaseBindingBlob)
	KY_ROOT_BLOB_CLASS(Runtime, DatabaseBindingBlob, 105)

public:
	DatabaseBindingBlob() {}

	bool IsBoundToDatabase(KyUInt32 databaseIndex) const
	{
		bool isBoundToDebuggedDatabase = false;
		if (m_boundDatabaseIndices.GetCount() == 0) // no binding means, bound to all databases
			isBoundToDebuggedDatabase = true;

		const KyUInt32* boundDatabaseIndices = m_boundDatabaseIndices.GetValues();
		for (KyUInt32 databaseBindingIdx = 0; databaseBindingIdx < m_boundDatabaseIndices.GetCount(); ++databaseBindingIdx)
		{
			if (boundDatabaseIndices[databaseBindingIdx] == databaseIndex)
				return true;
		}

		return isBoundToDebuggedDatabase;
	}

public:
	BlobArray<KyUInt32> m_boundDatabaseIndices;
};

inline void SwapEndianness(Endianness::Target e, DatabaseBindingBlob& self)
{
	SwapEndianness(e, self.m_boundDatabaseIndices);
}

class DatabaseBindingBlobBuilder : public BaseBlobBuilder<DatabaseBindingBlob>
{
public:
	DatabaseBindingBlobBuilder(const DatabaseBinding* databaseBinding) : m_databaseBinding(databaseBinding) {}

private:
	void DoBuild()
	{
		if (m_databaseBinding)
		{
			KyUInt32* databaseBindingIndices = BLOB_ARRAY(m_blob->m_boundDatabaseIndices, m_databaseBinding->GetBoundDatabaseCount());
			if (this->IsWriteMode())
			{
				for (KyUInt32 boundDatabaseBindingIdx = 0; boundDatabaseBindingIdx < m_databaseBinding->GetBoundDatabaseCount(); ++boundDatabaseBindingIdx)
				{
					Database* database = m_databaseBinding->GetBoundDatabase(boundDatabaseBindingIdx);
					if (database != KY_NULL)
						databaseBindingIndices[boundDatabaseBindingIdx] = database->GetDatabaseIndex();
				}
			}
		}
	}

	const DatabaseBinding* m_databaseBinding;
};

} // namespace Kaim

#endif // Navigation_TagVolumeBlob_H
