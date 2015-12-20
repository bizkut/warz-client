/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/

// primary contact: GUAL - secondary contact: LASI
#ifndef GwNavGen_GeneratorSectorList_H
#define GwNavGen_GeneratorSectorList_H


#include "gwnavgeneration/generator/generatorsector.h"


namespace Kaim
{

class GeneratorSectorList
{
	KY_DEFINE_NEW_DELETE_OPERATORS(Stat_Default_Mem)
	KY_CLASS_WITHOUT_COPY(GeneratorSectorList)

public:
	GeneratorSectorList(); /// constructor
	void ShallowCopy(const GeneratorSectorList& other); // copy and original point to the same GeneratorSectors

	/// new GeneratorSectors are instantiated BUT each GeneratorSector copy is SHALLOW
	// You can either Clear 
	void DeepCopy(const GeneratorSectorList& other, GeneratorSector::ClearPointedDataMode clearPointedDataMode);

	void Clear();

	// returns KY_ERROR if a sector with same name or Guid is already in the list
	KyResult AddSector(Ptr<GeneratorSector> sector);

	// returns KY_ERROR if sector can't be found
	KyResult RemoveSectorWithName(char* name);
	KyResult RemoveSectorWithGuid(const KyGuid& guid);
	KyResult RemoveSectorAtIdx(KyUInt32 idx);
	void RemoveAllSectors(); 

	// returns KyUInt32MaxVal if not found
	KyUInt32 GetSectorIdxFromName(const char* name);
	KyUInt32 GetSectorIdxFromGuid(const KyGuid& guid);

	// returns KY_NULL if not found
	Ptr<GeneratorSector> GetSectorWithName(const char* name);
	Ptr<GeneratorSector> GetSectorWithGuid(const KyGuid& guid);
	Ptr<GeneratorSector> GetSector(KyUInt32 index) { return index < m_sectors.GetCount() ? m_sectors[index] : KY_NULL; }

	const Ptr<GeneratorSector> GetSectorWithName(const char* name) const;
	const Ptr<GeneratorSector> GetSectorWithGuid(const KyGuid& guid) const ;
	const Ptr<GeneratorSector> GetSector(KyUInt32 index) const { return index < m_sectors.GetCount() ? m_sectors[index] : KY_NULL; }

	const KyArray< Ptr<GeneratorSector> >& GetSectors() const { return m_sectors; }
	KyArray< Ptr<GeneratorSector> >& GetSectors() { return m_sectors; }
	KyUInt32 GetSectorCount() const { return m_sectors.GetCount(); }

public:
	KyArray<Ptr<GeneratorSector> > m_sectors;
};


}


#endif

