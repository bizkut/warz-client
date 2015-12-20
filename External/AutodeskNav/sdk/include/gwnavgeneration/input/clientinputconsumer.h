/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// primary contact: GUAL - secondary contact: LASI
#ifndef GwNavGen_ClientInputConsumer_H
#define GwNavGen_ClientInputConsumer_H

#include "gwnavgeneration/input/taggedtriangle3i.h"
#include "gwnavruntime/math/box3f.h"
#include "gwnavgeneration/input/clientinput.h"
#include "gwnavgeneration/input/clientinputchunk.h"
#include "gwnavgeneration/input/clientinputtagvolume.h"
#include "gwnavruntime/collision/indexedmesh.h"
#include "gwnavruntime/collision/heightfield.h"
#include "gwnavruntime/world/tagvolume.h"

namespace Kaim
{

class GeneratorSystem;
class SpatializedSectorInput;
class SectorInputData;
class GeneratorSector;
class GeneratorInputProducer;


// interesting statistics, can be used to check if there was a OneMeter problem
class ClientInputConsumerStatistics
{
	KY_DEFINE_NEW_DELETE_OPERATORS(Stat_Default_Mem)
public:
	ClientInputConsumerStatistics() { Clear(); }

	void Clear()
	{
		m_nbConsumedTriangles = 0;
		m_consumedSurface2d = 0;
		m_nbConsumedVolumes = 0;
	}

	KyUInt32 GetNbConsumedTriangles()               { return m_nbConsumedTriangles; }
	KyFloat32 GetConsumedSurface2d()                { return m_consumedSurface2d; }
	KyFloat32 GetAverageConsumedTriangleSurface2d() { return m_consumedSurface2d / (KyFloat32)m_nbConsumedTriangles; }
	void GetConsumedTrianglesBoundingBox(Box3f& box) { box = m_consumedTrianglesBoundingBox; }

public:
	KyInt32 m_nbConsumedTriangles;
	KyFloat32 m_consumedSurface2d;
	Box3f m_consumedTrianglesBoundingBox;
	KyUInt32 m_nbConsumedVolumes;
};

/// The ClientInputConsumer class is one of the primary components of the NavData generation system. Its role
/// is to accept data that describes your terrain from within Kaim::GeneratorInputProducer::Produce(const GeneratorSector& sector, ClientInputConsumer& consumer).
/// This data is then used as input for the NavData generation process. This data includes:
/// -	The triangles that make up the terrain mesh, along with an optional color for each that defines its
/// -	material: the type of terrain it represents.
/// -	Optional 3D Tag volumes that tag all triangles within their volumes with a particular NavTag.
/// -	Optional seed points that identify walkable areas.
class ClientInputConsumer
{
	KY_DEFINE_NEW_DELETE_OPERATORS(MemStat_NavData)
	friend class GeneratorSectorBuilder;
	friend class SectorInputData;

public:
	// ClientInputConsumer will initialize
	// seedPoints       with what is passed to ConsumeSeedPoint()
	// inputMeshBuilder with what is passed to ConsumeTriangle...()
	// indexedMeshes    with what is passed to ConsumeIndexedMesh()
	// heightfield      with what is passed to ConsumeHeightfield()
	ClientInputConsumer(GeneratorSystem* sys, Ptr<GeneratorSector> sector, KyArray<Vec3f>* seedPoints, KyArray<TagVolumeInitConfig>* tagVolumeConfigs,
		IndexedMeshBlobBuilder* inputMeshBuilder, KyArray<Ptr<IndexedMesh> >* indexedMeshes, Ptr<HeightField>* heightfield);

	~ClientInputConsumer();

public:
	// ---------------------------------- Consumption ----------------------------------

	/// A, B and C are in Gameware Navigation coordinate system. m_defaultNavTag is used.
	KyResult ConsumeTriangle(const Vec3f& A, const Vec3f& B, const Vec3f& C);
	
	/// A, B and Care in client coordinate system. m_defaultNavTag is used.
	KyResult ConsumeTriangle(const Vec3f& client_A, const Vec3f& client_B, const Vec3f& client_C, const CoordSystem& clientCoordSystem);

	/// A, B and C are in Gameware Navigation coordinate system.
	KyResult ConsumeTriangle(const Vec3f& A, const Vec3f& B, const Vec3f& C, const DynamicNavTag& navTag);
	
	/// A, B and C are in client coordinate system.
	KyResult ConsumeTriangle(
		const Vec3f& client_A, const Vec3f& client_B, const Vec3f& client_C, const DynamicNavTag& navTag, const CoordSystem& clientCoordSystem);

	/// Provides a single tag volume to the ClientInputConsumer.
	/// They will be transformed automatically according to the CoordSystem set up for the Generator. 
	KyResult ConsumeTagVolume(const ClientInputTagVolume& inputTagVolume);

	/// Provides a seed point that identifies a walkable area of the terrain. If you provide a seed point, any areas of NavData
	/// that are not reachable from that seed point are automatically discarded during a post-processing phase. You can provide as
	/// many seed points as necessary for each sector.
	/// All coordinates and altitudes must be expressed in Gameware Navigation coordinate system.
	/// \param position		The position of the seed point
	KyResult ConsumeSeedPoint(const Vec3f& position);

	/// Identical to previous function but uses vertices in client coordinate system that must be specified in order to convert them in Navigation CoordSystem
	KyResult ConsumeSeedPointInClientCoordinates(const Vec3f& client_position, const CoordSystem& clientCoordSystem) { return ConsumeSeedPoint(clientCoordSystem.ClientToNavigation_Pos(client_position)); }

	KyResult ConsumeHeightField(Ptr<HeightField> heightfield);
	KyResult ConsumeHeightFieldFile(const String& heightfieldFileName);

	KyResult ConsumeIndexedMesh(Ptr<IndexedMesh> indexedMesh);
	KyResult ConsumeIndexedMeshFile(const String& indexedMeshFileName);
	/// Retrieves information about the triangles consumed by this object. 
	ClientInputConsumerStatistics& GetStats() { return m_stats; }

	VisualColor GetNavTagVisualColor(const DynamicNavTag& dynamicNavTag) const;
private: 
	/// internal usage by GeneratorSectorBuilder
	void Flush();
	KyResult SaveClientInput(const String& fullFileName);
	KyResult FlushClientInput();
	void TestWarningOnLowMemoryModeNbTriangles();
	bool IsTriangleSlopeWalkable(const Vec3f& A, const Vec3f& B, const Vec3f& C);

public:
	ClientInputConsumerStatistics m_stats; ///< Maintains statistics about the data added to this object. Do not modify. 
	GeneratorSystem* m_sys; ///< Fast access to the Generator-related information
	
private: // ---------------------------------- Internal ----------------------------------
	Ptr<GeneratorSector> m_sector;  ///< Internal member: The sector being processed

	// computed (or read) directly from m_sys->m_genIO->m_params
	DynamicNavTag m_defaultNavTag;    ///< shortcut for m_sys m_genParams.m_defaultNavTag;
	bool m_backFaceTrianglesWalkable; ///< Internal member: Accessed using ToggleBackfaceTriangleFiltering
	KyFloat32 m_cosSlopeMax;          ///< Internal member: cosinus of maximum slope

	DynamicClientInputChunk m_dynamicClientInputChunk; ///< Internal member: Original input chunk
	KyUInt32 m_clientInputFlushCount;                  ///< Internal member: Count how many times the ClientInput was flushed

	KyUInt32 m_maxTriangleCountBeforeFlush; ///< Internal member: For limited memory mode. 1024*1024 by default
	KyUInt32 m_warnOnLowMemoryModeNbTriangles;

	SpatializedSectorInput* m_spatializedSectorInput; ///< Internal member: Spatialized input in Gameware Navigation coordinates
	KyArray<Vec3f>* m_seedPoints;                     ///< Internal member: Consumed seedpoints in Gameware Navigation coordinates
	KyArray<TagVolumeInitConfig>* m_tagVolumeConfigs;

	KyArray<Vec3f> m_inputTriangles;    ///< Temporary buffer for triangles - in Gameware Navigation coordinates
	KyArray<Vec3f> m_childrenTriangles; ///< Temporary buffer for triangles - in Gameware Navigation coordinates

	IndexedMeshBlobBuilder* m_inputMeshBuilder;
	KyArray<Ptr<IndexedMesh> >* m_indexedMeshes; ///< Internal member: Consumed IndexedMesh in Gameware Navigation coordinates
	Ptr<HeightField>*           m_heightfield;   ///< Internal member: Consumed heightField in Gameware Navigation coordinates
};



inline KyResult ClientInputConsumer::ConsumeTriangle(const Vec3f& A, const Vec3f& B, const Vec3f& C)
{
	return ConsumeTriangle(A, B, C, m_defaultNavTag);
}

inline KyResult ClientInputConsumer::ConsumeTriangle(const Vec3f& client_A, const Vec3f& client_B, const Vec3f& client_C, const CoordSystem& clientCoordSystem)
{
	return ConsumeTriangle(client_A, client_B, client_C, m_defaultNavTag, clientCoordSystem);
}


}


#endif

