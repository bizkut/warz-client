/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// primary contact: MAMU - secondary contact: 

#ifndef Navigation_ActiveDataBlob_H
#define Navigation_ActiveDataBlob_H

#include "gwnavruntime/blob/blobtypes.h"
#include "gwnavruntime/database/database.h"
#include "gwnavruntime/database/navmeshelementmanager.h"
#include "gwnavruntime/navdata/navdata.h"

namespace Kaim
{

enum ActiveSectorType
{
	ActiveSectorType_NavMesh       = 0,
	ActiveSectorType_NavGraph      = 1,
	ActiveSectorType_AbstractGraph = 2,
	
	ActiveSectorType_Undefined     = KyUInt8MAXVAL,
};

class ActiveSectorBlob
{
public:
	ActiveSectorBlob() {}

	KyGuid m_guid;
	BlobArray<char> m_sectorName;
	BlobArray<char> m_sectorRelativeDir;
	KyUInt8 m_activeSectorType; // cf. ActiveSectorType
};

inline void SwapEndianness(Endianness::Target e, ActiveSectorBlob& self)
{
	SwapEndianness(e, self.m_guid);
	SwapEndianness(e, self.m_sectorName);
	SwapEndianness(e, self.m_sectorRelativeDir);
	SwapEndianness(e, self.m_activeSectorType);
}

class ActiveSectorBlobBuilder : public BaseBlobBuilder<ActiveSectorBlob>
{
public:
	static ActiveSectorType GetActiveSectorType(const NavData* navData)
	{
		if (navData->GetNavMeshElementBlobCollection().GetCount() != 0)
			return ActiveSectorType_NavMesh;
		else if (navData->GetNavGraphBlobCollection().GetCount() != 0)
			return ActiveSectorType_NavGraph;
		else if (navData->GetAbstractGraphBlobCollection().GetCount() != 0)
			return ActiveSectorType_AbstractGraph;
		return ActiveSectorType_Undefined;
	}

public:
	ActiveSectorBlobBuilder(const KyGuid& guid, const String& sectorName, const String& sectorRelativeDir, ActiveSectorType activeSectorType)
		: m_guid(guid), m_sectorName(sectorName), m_sectorRelativeDir(sectorRelativeDir), m_activeSectorType(activeSectorType) {}

	void DoBuild()
	{
		BLOB_SET(m_blob->m_guid, m_guid);
		BLOB_STRING(m_blob->m_sectorName, m_sectorName.ToCStr());
		BLOB_STRING(m_blob->m_sectorRelativeDir, m_sectorRelativeDir.ToCStr());
		BLOB_SET(m_blob->m_activeSectorType, ((KyUInt8) m_activeSectorType));
	}

	KyGuid m_guid;
	String m_sectorName;
	String m_sectorRelativeDir;
	ActiveSectorType m_activeSectorType;
};

class ActiveDataBlob
{
	KY_ROOT_BLOB_CLASS(NavData, ActiveDataBlob, 0)
	KY_CLASS_WITHOUT_COPY(ActiveDataBlob)

public:
	ActiveDataBlob() : m_databaseIndex(KyUInt32MAXVAL) {}

	KyUInt32 m_databaseIndex;
	BlobArray<ActiveSectorBlob> m_activeSectors;
};

inline void SwapEndianness(Endianness::Target e, ActiveDataBlob& self)
{
	SwapEndianness(e, self.m_databaseIndex);
	SwapEndianness(e, self.m_activeSectors);
}

class ActiveDataBlobBuilder : public BaseBlobBuilder<ActiveDataBlob>
{
public:
	ActiveDataBlobBuilder(const Database* db) : m_db(db) {}

	void DoBuild()
	{
		if (m_db == NULL)
			return;
		
		BLOB_SET(m_blob->m_databaseIndex, m_db->GetDatabaseIndex());

		KyArray<Kaim::NavData*> navDataToBuild;
		for (KyUInt32 i = 0; i < m_db->m_navDatas.GetCount(); ++i)
		{
			Kaim::NavData* navData = m_db->m_navDatas[i];
			if (navData->GetSectorName() != "")
			{
				navDataToBuild.PushBack(navData);
			}
		}

		ActiveSectorBlob* activeSectors = BLOB_ARRAY(m_blob->m_activeSectors, navDataToBuild.GetCount());
		for (KyUInt32 i = 0; i < navDataToBuild.GetCount(); ++i)
		{
			NavData* navData = navDataToBuild[i];
			KY_DEBUG_ERRORN_IF(navData->GetSectorName() == "", ("Error in ActiveDataBlobBuilder, we should not build ActiveDataBlob for this navData"));
				
			const KyGuid* guid = navData->GetMainGuid();
			if (guid == KY_NULL)
				guid = &KyGuid::GetInvalidGuid();

			ActiveSectorType sectorType = ActiveSectorBlobBuilder::GetActiveSectorType(navData);
			BLOB_BUILD(activeSectors[i], ActiveSectorBlobBuilder(*guid, navData->m_sectorDescriptor.m_sectorName, navData->m_sectorDescriptor.m_generatorRelativeOutputDirectory, sectorType));
		}
	}

	const Database* m_db;
};

// Used to be sent from the NavigationLab
// Not used anymore since supporting identification of Graph-only NavData based on NavData.m_sectorDescriptor.m_sectorName 
// since such NavData has no guid (since there's no navmesh)
class ActiveGuidsBlob
{
	KY_ROOT_BLOB_CLASS(NavData, ActiveGuidsBlob, 0)
	KY_CLASS_WITHOUT_COPY(ActiveGuidsBlob)

public:
	ActiveGuidsBlob() {}

	BlobArray<KyGuid> m_guids;
};

inline void SwapEndianness(Endianness::Target e, ActiveGuidsBlob& self)
{
	SwapEndianness(e, self.m_guids);
}

class ActiveGuidsBlobBuilder : public BaseBlobBuilder<ActiveGuidsBlob>
{

public:
	ActiveGuidsBlobBuilder(KyArray<KyGuid, MemStat_NavData>* activeGuids) : m_activeGuids(activeGuids) {}

	void DoBuild()
	{
		if (m_activeGuids == NULL)
			return;

		BLOB_ARRAY_COPY_2(m_blob->m_guids, (*m_activeGuids));
	}

	KyArray<KyGuid, MemStat_NavData>* m_activeGuids;
};



}

#endif