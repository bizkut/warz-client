/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/

// primary contact: GUAL - secondary contact: LASI
#ifndef GwNavGen_GeneratorSector_H
#define GwNavGen_GeneratorSector_H

#include "gwnavruntime/base/kyguid.h"
#include "gwnavgeneration/common/generatorfilesystem.h"
#include "gwnavruntime/navdata/navdata.h"
#include "gwnavruntime/base/endianness.h"
#include "gwnavruntime/basesystem/coordsystem.h"
#include "gwnavgeneration/input/clientinputtagvolume.h"
#include "gwnavgeneration/generator/genflags.h"
#include "gwnavruntime/collision/collisiondata.h"

namespace Kaim
{

class GeneratorSectorBlob;
class GeneratorFileSystem;
class BlobFieldsMapping;


/// Base class for the optional m_refCountedUserData that allows to refCount your user data.
class GeneratorSectorRefCountedUserData : public Kaim::RefCountBase<GeneratorSectorRefCountedUserData, Kaim::MemStat_NavDataGen>
{
public:
	virtual const char* GetType() const = 0;
	
	bool Isa(const char* type) const { return SFstrcmp(GetType(), type) == 0; }

	virtual ~GeneratorSectorRefCountedUserData() {}
};

/// The GeneratorSector class provides a definition for a single sector to be treated by the Generator.
/// It contains the sector configuration and the generated NavData.
/// Typically a GeneratorSector can be obtained from a GeneratorInputOutput instance using AddSector() or 
/// one of the GetSector() method. 
class GeneratorSector : public RefCountBaseNTS<GeneratorSector, MemStat_NavDataGen>
{
	KY_DEFINE_NEW_DELETE_OPERATORS(Stat_Default_Mem)
	
friend class Generator;
friend class GeneratorSectorBuilder;
friend class GeneratorSectorBlobBuilder;
friend class GeneratorSectorListBlob;

public:
	GeneratorSector(); /// constructor
	GeneratorSector(const KyGuid& sectorGuid, const char* sectorName);
	void Clear(); /// Reset the GeneratorSector as if is was just default constructed

	/// NOTE: Copy Constructor and Copy Assignment Operator are those generated automatically. They perform SHALLOW Copy.
	/// original and copy will point to the same instances of: void* m_userData; Ptr<> m_refCountedUserData; Ptr<> m_navData; Ptr<> m_navDataPatch;
	/// To make sure those members are NULL, call ClearPointedData() after Copy Constructor or Copy Assignment Operator
	enum ClearPointedDataMode { CLEAR_NONE = 0, CLEAR_NAVDATA = 1, CLEAR_USERDATA = 2, CLEAR_ALL_POINTED_DATA = 3 };
	void ClearPointedData(ClearPointedDataMode clearPointedDataMode);

public:
	const char* GetName() const { return m_sectorName.ToCStr(); } ///< Retrieves the name set for this sector as char*.
	const String& GetNameString() const { return m_sectorName; }  ///< Retrieves the name set for this sector as String.

	const KyGuid& GetGuid() const { return m_sectorGuid; }; ///< Retrieves the GUID set for this sector.
	String GetGuidString() const; ///< Retrieves a string representing the GUID set for this sector.

	void SetSectorChange(GenFlags::SectorChange sectorChange); ///< Write accessor for #m_sectorChange.
	GenFlags::SectorChange GetSectorChange() const;            ///< Read accessor for #m_sectorChange.

	void SetColDataBuildMode(GenFlags::SectorColDataBuildMode colDataBuildMode);
	GenFlags::SectorColDataBuildMode GetColDataBuildMode() const;

	void AddInputFileName(const char* inputFileName) { m_inputFileNames.PushBack(String(inputFileName)); }
	
	void AddInputSeedPoint(const Vec3f& position)    { m_inputSeedPoints.PushBack(position); }
	void AddInputSeedPointInClientCoordinates(const Vec3f& client_position, const CoordSystem& coordSystem) { AddInputSeedPoint(coordSystem.ClientToNavigation_Pos(client_position)); }

	void AddInputTagVolume(const ClientInputTagVolume& clientInputTagVolume) { m_inputTagVolumes.PushBack(clientInputTagVolume); }

	bool IsNavDataCellBoxSet() const; ///< Determines whether or not a cellBox was set for this sector

	String GetAbsoluteOutputFileName(const GeneratorFileSystem& fileSystem, const String& extension) const;
	String GetRelativeOutputFileName(const GeneratorFileSystem& fileSystem, const String& extension) const;

	/// Retrieve a pointer to the NavData that was generated for this sector, or #KY_NULL if the navdata is not available
	/// or has not been generated yet.
	/// The NavData provided is not bound to any Kaim::Database or Kaim::World and needs to be added manually in order to have access 
	/// to the stitched runtime data
	Ptr<NavData> GetNavData() { return m_navData; }

	/// Retrieve a pointer to the NavDataPatch that was generated for this sector, or #KY_NULL if the navDataPath is not available
	/// or has not been generated yet.
	Ptr<NavData> GetNavDataPatch() { return m_navDataPatch; }

	/// Retrieve a pointer to the ColData that was generated for this sector, or #KY_NULL if the ColData is not available
	/// or has not been generated yet.
	/// The ColData provided is not bound to any Kaim::World and needs to be added manually in order to use it
	Ptr<CollisionData> GetColData() { return m_colData; }

	/// If NavData was generated, save it to the specified location
	KyResult SaveNavData(const char* filename, FileOpenerBase* fileOpener = KY_NULL, Endianness::Type endianness = Endianness::BigEndian);

private:
	void InitBlobFieldsMapping(BlobFieldsMapping& mapping);
	void InitFromBlob(const GeneratorSectorBlob& blob);

public:
	// ---------- High level description of the sector, once these parameters has been set, they must not change. ----------
	KyGuid m_sectorGuid; ///< The sector GUID.
	String m_sectorName; ///< The sector Name.
	CellBox m_navDataCellBox; ///< Optional, limits the NavData generation to this CellBox. Relevant when pipeline uses tiles as sectors.
	bool m_isPatch;         ///< Indicates if the Sector is a Patch.
	
	// ---------- How to produce the sector generation inputs. These parameters are mostly used in your implementation of GeneratorInputProducer::ProduceSectorInputs() ----------
	// How to produce the inputs. Your derivation of GeneratorInputProducer passed to the Generator constructor has the responsibility to produce inputs.
	// But the following parameters affect the process.
	KyUInt32 m_inputSource; ///< Indicates whether GeneratorInputProducer::ProduceSectorInputs() is called -or- the sector ClientInput file is loaded.
	KyArray<String> m_inputFileNames; ///< Optional facility. To be used in GeneratorInputProducer::ProduceSectorInputs().
	KyArray<Vec3f> m_inputSeedPoints; ///< Optional facility. Will be fed automatically to the Sector. Sometimes easier than producing seedPoints in GeneratorInputProducer::ProduceSectorInputs().
	KyArray<ClientInputTagVolume> m_inputTagVolumes; ///< Optional facility. Will be fed automatically to the Sector. Sometimes easier than producing TagVolumes in GeneratorInputProducer::ProduceSectorInputs().
	void* m_userData; ///< Allows to point to your own sector data. Typically to be used in GeneratorInputProducer::ProduceSectorInputs().
	Ptr<GeneratorSectorRefCountedUserData> m_refCountedUserData; ///< Allows to point to your own sector data. Typically to be used in GeneratorInputProducer::ProduceSectorInputs().

	// ---------- What has changed in the sector since previous generation ----------
	KyUInt32 m_sectorChange; ///< What has changed in the sector since previous generation

	// ---------- Debug/Iterative file setup ----------
	bool m_isClientInputSaveEnabled; ///< Indicates whether to save ClientInput file
	KyUInt32 m_colDataBuildMode;     ///< takes values from GenerationFlags::SectorColDataSaveMode

	// ---------- Generation Outputs ----------
	String m_clientInputAbsoluteFileName;  ///< Indicates Set by the Generator when the ClientInput is saved.
	String m_navDataAbsoluteFileName;      ///< Set by the Generator when the NavData is saved. Relevant when isPatch is false.
	String m_navDataPatchAbsoluteFileName; ///< Set by the Generator when the NavDataPatch is saved.
	String m_colDataAbsoluteFileName;      ///< Set by the Generator when the ColData is saved
	Ptr<NavData> m_navData;                ///< Generated NavData. Relevant when m_isPatch=false and m_buildMode=SECTOR_CHANGE.
	Ptr<NavData> m_navDataPatch;           ///< Generated NavData. Relevant when m_isPatch=true  and m_patchOperation!=PATCH_NO_CHANGE.
	Ptr<CollisionData> m_colData;               ///< Will be accessible when generation is done through GetColData(). #KY_NULL until then.

	// ---------- Previous generation outputs, which are useful for iterative generations ----------
	KyArray<CellPos> m_previousPatchCellPosList; ///< From previous generation, the list of CellPos that were impacted by the Patch inputs. Empty if m_isPatch is false or if the Patch is new.
	CellBox          m_previousInputCellBox;     ///< From previous generation, the AABB of CellPos that were impacted by the Sector or Patch Inputs.
	KyArray<Vec3f>   m_previousSeedPoints;       ///< From previous generation, the sector SeedPoints.
};


inline String GeneratorSector::GetGuidString() const
{
	char guidChars[37];
	m_sectorGuid.ToString(guidChars);
	return String(guidChars);
}

inline void GeneratorSector::SetSectorChange(GenFlags::SectorChange sectorChange) { m_sectorChange = sectorChange; }
inline GenFlags::SectorChange GeneratorSector::GetSectorChange() const            { return (GenFlags::SectorChange)m_sectorChange; }
inline bool GeneratorSector::IsNavDataCellBoxSet() const { return m_navDataCellBox.IsValid(); }

inline void GeneratorSector::SetColDataBuildMode(GenFlags::SectorColDataBuildMode colDataBuildMode) { m_colDataBuildMode = colDataBuildMode; }
inline GenFlags::SectorColDataBuildMode GeneratorSector::GetColDataBuildMode() const                      { return (GenFlags::SectorColDataBuildMode)m_colDataBuildMode; }


}


#endif

