/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/

// primary contact: LASI - secondary contact: GUAL
#ifndef GwNavGen_GeneratorConfig_H
#define GwNavGen_GeneratorConfig_H

#include "gwnavgeneration/generator/generatorsectorlist.h"
#include "gwnavgeneration/generator/generatorparameters.h"
#include "gwnavgeneration/generator/generatorpostprocessparameters.h"
#include "gwnavgeneration/generator/generatorrunoptions.h"
#include "gwnavgeneration/generator/generatorguidcompound.h"
#include "gwnavruntime/abstractgraph/blobs/abstractdataindex.h"
#include "gwnavruntime/collision/collisiondataindex.h"
#include "gwnavruntime/base/endianness.h"
#include "gwnavruntime/base/types.h"
#include "gwnavruntime/base/memory.h"
#include "gwnavruntime/kernel/SF_String.h"
#include "gwnavruntime/base/fileopener.h"
#include "gwnavruntime/navdata/navdataindex.h"
#include "gwnavruntime/navdata/databasedescriptor.h"
#include "gwnavruntime/basesystem/coordsystem.h"



namespace Kaim
{

/// The GeneratorInputOutput class is a central element of the generation.
/// Its purpose is to setup a generation as well as store the data that have been produced.
/// GeneratorInputOutput contains 3 types of information:
//  - the Sectors: description of all sectors in the database (typically change not very often)
/// - the Actions: what to generate in the next call to Generator::Generate(). By default, everything is generated from scratch, but it's possible to setup iterative generations.
/// - the Outputs: GeneratorInputOutput gives access to generated NavData
/// GeneratorInputOutput can be saved, loaded and modified to enable iterative generations.
class GeneratorInputOutput : public RefCountBaseNTS<GeneratorInputOutput, MemStat_NavDataGen>
{
	KY_CLASS_WITHOUT_COPY(GeneratorInputOutput)

public:
	GeneratorInputOutput(); ///< constructor
	void ShallowCopy(const GeneratorInputOutput& other);
	void DeepCopy(const GeneratorInputOutput& other, GeneratorSector::ClearPointedDataMode clearPointedDataMode);
	void Clear(); ///< Reset members as if the object was freshly constructed

	// ---------------------------------- Save, Load, Copy, Clear Functions ----------------------------------

	/// Save the GeneratorInputOutput in a file.
	/// \param fullFileName    The filename where to save the GeneratorInputOutput.
	/// \param fileOpener      A custom file opener to access files. By default, if no fileOpener is set, the DefaultFileOpener will be used.
	KyResult Save(const char* fullFileName, FileOpenerBase* fileOpener = KY_NULL) const;
	
	/// Load the GeneratorInputOutput from a file.
	/// \param fullFileName    The filename from where to load the GeneratorInputOutput.
	/// \param fileOpener      A custom file opener to access files. By default, if no fileOpener is set, the DefaultFileOpener will be used.
	/// Note: after a call to Load(), the GeneratorSector m_userData are KY_NULL. you can set it with GetSectorWithGuid()->SetUserData(userData).
	KyResult Load(const char* fullFileName, FileOpenerBase* fileOpener = KY_NULL);

	// ---------------------------------- Add/Remove/Set Functions ----------------------------------

	/// Set sectorChange for all sectors (regular and patches)
	void SetSectorChangeForAllSectors(GenFlags::SectorChange sectorChange);

	/// Set inputSource for all sectors (regular and patches)
	void SetInputSourceForAllSectors(GenFlags::SectorInputSource inputSource);

	/// Use this function to Add new GeneratorSector to the list stored in the GeneratorInputOutput.
	/// This function returns KY_ERROR if a sector with the same name or GUID is already in the sector list.
	KyResult AddSector(Ptr<GeneratorSector> sector);

	/// Specifies the GUIDs of sectors that cover the same areas of terrain, but that will never be loaded into the same Database
	/// at the same time. This prevents the NavData generation system from generating alternative data in the area of overlap.
	/// This is primarily intended to support swappable sectors, which reflect multiple versions of the terrain in a single area --
	/// for example, one version of a sector that contains a building, and another version of a sector in which the building has
	/// been destroyed.
	/// You can still use swappable sectors without calling this method. However, as the alternative data generated in the area
	/// of overlap between the sectors will never be used, you can reduce the size of your NavData by preventing the NavData generation
	/// system from generating the data in the first place.
	/// \param guids         An array of GUIDs that identify the sectors that will never be loaded into the same Database at the
	///                      same time.
	/// \param guidsCount    The number of elements in the guids array. 
	void AddExclusiveGuids(const KyGuid* guids, KyUInt32 guidsCount);
	void AddExclusiveGuids(const GeneratorGuidCompound& generatorGuidCompound);

	// ---------------------------------- Accessors ----------------------------------
	KyUInt32 GetSectorCount(); ///< Returns the sector count

	Ptr<GeneratorSector> GetSectorWithIndex(KyUInt32 index);     ///< Returns the Sector at index if it exists, non const version.
	Ptr<GeneratorSector> GetSectorWithName(const char* name);    ///< Returns the Sector with name if it exists, non const version.
	Ptr<GeneratorSector> GetSectorWithGuid(const KyGuid& guid);  ///< Returns the Sector with guid if it exists, non const version.

	const Ptr<GeneratorSector> GetSectorWithIndex(KyUInt32 index) const;     ///< Returns the Sector at index if it exists, const version.
	const Ptr<GeneratorSector> GetSectorWithName(const char* name) const;    ///< Returns the Sector with name if it exists, const version.
	const Ptr<GeneratorSector> GetSectorWithGuid(const KyGuid& guid) const;  ///< Returns the Sector with guid if it exists, const version.

private:
	void CommonCopy(const GeneratorInputOutput& other);

public:
	GeneratorParameters m_params; //In
	GeneratorAbstractGraphParameters m_abstractGraphParams; //In
	GeneratorRunOptions m_runOptions; //In
	KyArray<GeneratorGuidCompound> m_exclusiveGuids; //In
	GeneratorSectorList m_sectors; // In/Out
	
	// Out. The info contained in m_navDataIndexHandler is redundant with m_sectors
	// but having a NavDataIndex Blob available allows to load them in a pure runtime environment
	// that does NOT link with gwnavgeneration.lib
	Ptr< BlobHandler<NavDataIndex> > m_navDataIndexHandler;

	// Out. The info contained in m_abstractDataIndexHandler is redundant with m_sectors
	// but having a AbstractDataIndex Blob available allows to load them in a pure runtime environment
	// that does NOT link with gwnavgeneration.lib
	Ptr< BlobHandler<AbstractDataIndex> > m_abstractDataIndexHandler;

	// Out. The info contained in m_colDataIndexHandler is redundant with m_sectors
	// but having a ColDataIndex Blob in GenIO files allows to load them in a pure runtime environment
	// that does not link with gwnavgeneration.lib
	Ptr< BlobHandler<ColDataIndex> > m_colDataIndexHandler;

	/// Out  The database description
	DatabaseDescriptor m_databaseDescriptor;
};


}


#endif
