/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// primary contact: LAPA - secondary contact: NOBODY
#ifndef Navigation_TagVolumeSpatialization_H
#define Navigation_TagVolumeSpatialization_H


#include "gwnavruntime/world/databasebinding.h"
#include "gwnavruntime/navgraph/identifiers/navgraphedgerawptr.h"

namespace Kaim
{

class World;
class DatabaseBinding;
class Database;
class Box3f;
class TagVolume;


// INTERNAL CLASS: TagVolume spatialization in one Database: maintain the Database
// CellBox over which the TagVolume AABB extends.
class TagVolumeDbData
{
	KY_DEFINE_NEW_DELETE_OPERATORS(MemStat_Spatialization)
public:
	TagVolumeDbData() {}

	bool IsComputed() const { return m_cellBox != CellBox(); }
	void Compute(const Box3f& aabb, KyUInt32 obstacleTypeUInt, Database* database);
	void Clear() { m_extendedAabb.Clear(); m_cellBox.Clear(); }
public:
	Box3f m_extendedAabb;
	CellBox m_cellBox;
};


/// Each TagVolume instance aggregates one instance of this class to maintain its
/// spatialization information in all Database it is bound to.
class TagVolumeDatabaseData
{
	KY_DEFINE_NEW_DELETE_OPERATORS(MemStat_Spatialization)

public: // internal
	TagVolumeDatabaseData() : m_databaseBinding(KY_NULL), m_tagVolumeDbData(KY_NULL) {}
	~TagVolumeDatabaseData() { MakeZero(); }

	void Init(Ptr<DatabaseBinding> databaseBinding);

	KyUInt32 GetTagVolumeDbDataCount() const;

	      TagVolumeDbData& GetTagVolumeDbData(KyUInt32 boundDatabaseIdx);
	const TagVolumeDbData& GetTagVolumeDbData(KyUInt32 boundDatabaseIdx) const;

	void ComputeTagVolumeDbData(const Box3f& aabb, KyUInt32 tagVolumeExtendOptions);

private :
	void MakeZero();

	void CreateTagVolumeDbData();
	void DestroyTagVolumeDbData();

public: // internal
	Ptr<DatabaseBinding> m_databaseBinding;
	TagVolumeDbData* m_tagVolumeDbData; // array indexed like m_databaseBinding
};


KY_INLINE       KyUInt32         TagVolumeDatabaseData::GetTagVolumeDbDataCount()      const { return (m_databaseBinding ? KyUInt32(m_databaseBinding->GetBoundDatabaseCount()) : 0); }
KY_INLINE       TagVolumeDbData& TagVolumeDatabaseData::GetTagVolumeDbData(KyUInt32 i)       { return m_tagVolumeDbData[i]; }
KY_INLINE const TagVolumeDbData& TagVolumeDatabaseData::GetTagVolumeDbData(KyUInt32 i) const { return m_tagVolumeDbData[i]; }

} // namespace Kaim

#endif // Navigation_TagVolumeSpatialization_H
