/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// primary contact: GUAL - secondary contact: NOBODY

#ifndef Navigation_Path_H
#define Navigation_Path_H

#include "gwnavruntime/containers/kyarray.h"
#include "gwnavruntime/navmesh/identifiers/navtriangleptr.h"
#include "gwnavruntime/navgraph/identifiers/navgraphvertexptr.h"
#include "gwnavruntime/navgraph/identifiers/navgraphedgeptr.h"
#include "gwnavruntime/navmesh/worldintegerpos.h"


namespace Kaim
{
class DatabaseGenMetrics;
class ChannelArray;

/// Defines the different kind of PathEdge within a Path.
enum PathEdgeType
{
	PathEdgeType_Undefined            , ///< The PathEdgeType has not yet been set for the corresponding PathEdge.
	PathEdgeType_OnNavMesh            , ///< The PathEdge has been computed on the NavMesh.
	PathEdgeType_OnNavGraph           , ///< The PathEdge corresponds to a NavGraph edge.
	PathEdgeType_FromOutsideToNavMesh , ///< The PathEdge is coming from outside to the NavMesh.
	PathEdgeType_FromNavMeshToOutside , ///< The PathEdge is going from the NavMesh to outside.
	PathEdgeType_FromOutsideToNavGraph, ///< The PathEdge is coming from outside to the NavMesh.
	PathEdgeType_FromNavGraphToOutside, ///< The PathEdge is going from the NavMesh to outside.
	PathEdgeType_OutsideAlongGraphEdge, ///< The PathEdge is going along an a NavGraphEdge but not starting nor ending at one of the NavGrapHVertex
	PathEdgeType_AbstractEdge,			///< The PathEdge is fro an abstract Path.
	PathEdgeType_MaxCount,
};


/// The class representing a path.
class Path
{
	KY_DEFINE_NEW_DELETE_OPERATORS(MemStat_Path)
	KY_CLASS_WITHOUT_COPY(Path)
	Path();
	~Path();

public:
	/// A class that configures the creation of a Path.
	class CreateConfig
	{
	public:
		// ---------------------------------- Public Members ----------------------------------
		CreateConfig(KyUInt32 nodesCount = 0) : m_nodesCount(nodesCount) {}

		KyUInt32 m_nodesCount;
	};


	// ---------------------------------- Path Creation ----------------------------------

	/// Creates a Path instance and allocates necessary memory accordingly to
	/// the information provided through createConfig.
	static Ptr<Path> CreatePath(const CreateConfig& createConfig);

	void     SetNavigationProfileId(KyUInt32 navigationProfileId);
	KyUInt32 GetNavigationProfileId() const;

	// ---------------------------------- Get Counts ----------------------------------

	KyUInt32 GetNodeCount() const;
	KyUInt32 GetEdgeCount() const;

	// ---------------------------------- Get Cost and Distance ----------------------------------

	/// Retrieves the total sum of the distance covered by each segment of the path.
	/// Note that this value is calculated when the path is created, and is not updated dynamically.
	KyFloat32 GetPathDistance() const;

	/// Retrieves the total sum of the cost calculated for each segment of the path by a RayCanGoQuery. 
	/// Note that this value is calculated when the path is created, and is not updated dynamically.
	KyFloat32 GetPathCost()     const;

	// ---------------------------------- Get Node Data ----------------------------------

	const Vec3f& GetPathStartPosition() const;
	const Vec3f& GetPathEndPosition() const;

	const Vec3f* GetNodePositionBuffer() const;
	const Vec3f& GetNodePosition(KyUInt32 nodeIdx) const;
	      Vec3f& GetNodePosition(KyUInt32 nodeIdx);

	const WorldIntegerPos* GetNodeIntegerPositionBuffer() const;
	const WorldIntegerPos& GetNodeIntegerPosition(KyUInt32 nodeIdx) const;
	      WorldIntegerPos& GetNodeIntegerPosition(KyUInt32 nodeIdx);

	const NavTrianglePtr& GetNodeNavTrianglePtr(KyUInt32 nodeIdx) const;
	      NavTrianglePtr& GetNodeNavTrianglePtr(KyUInt32 nodeIdx);

	const NavGraphVertexPtr& GetNodeNavGraphVertexPtr(KyUInt32 nodeIdx) const;
	      NavGraphVertexPtr& GetNodeNavGraphVertexPtr(KyUInt32 nodeIdx);

	// ---------------------------------- Get Edge Data ----------------------------------

	const Vec3f& GetPathEdgeStartPosition(KyUInt32 edgeIdx) const;
	const Vec3f& GetPathEdgeEndPosition  (KyUInt32 edgeIdx) const;

	const NavTrianglePtr& GetPathEdgeStartNavTrianglePtr(KyUInt32 edgeIdx) const;
	const NavTrianglePtr& GetPathEdgeEndNavTrianglePtr  (KyUInt32 edgeIdx) const;

	const NavGraphEdgePtr& GetEdgeNavGraphEdgePtr(KyUInt32 edgeIdx) const;
	      NavGraphEdgePtr& GetEdgeNavGraphEdgePtr(KyUInt32 edgeIdx);

	PathEdgeType GetPathEdgeType(KyUInt32 edgeIdx) const;

	// ---------------------------------- Set Node Data ----------------------------------

	void SetNodePosition         (KyUInt32 nodeIdx, const Vec3f& position                     );
	void SetNodeIntegerPosition  (KyUInt32 nodeIdx, const WorldIntegerPos& integerPos         );
	void SetNodeNavTrianglePtr   (KyUInt32 nodeIdx, const NavTrianglePtr& navTrianglePtr      );
	void SetNodeNavGraphVertexPtr(KyUInt32 nodeIdx, const NavGraphVertexPtr& navGraphVertexPtr);

	void SetNodePosition3fAndInteger(KyUInt32 nodeIdx, const Vec3f& position, const WorldIntegerPos& integerPos);
	// ---------------------------------- Set Edge Data ----------------------------------

	void SetEdgeNavGraphEdgePtr(KyUInt32 edgeIdx, const NavGraphEdgePtr& navGraphEdgePtr);
	void SetPathEdgeType       (KyUInt32 edgeIdx, PathEdgeType edgeType                 );

	void SetPathCostAndDistance(KyFloat32 pathCost, KyFloat32 pathDistance);

	const CellBox& GetPathCellBox() const;
	CellBox& GetPathCellBox();
	void SetPathCellBox(const CellBox& cellBox);

	void ComputeAllNodeIntegerPositionAndPathCellBox(const DatabaseGenMetrics& genMetrics);

	KyUInt32 GetByteSize() const;
private :
	static KyUInt32 ComputeByteSize(const CreateConfig& createConfig);

	void InitBuffers();
	void ClearAllBeforeDestruction();

	template <class T>
	void ClearBufferCPP(T*& buffer, KyUInt32 count);
	template <class T>
	void ClearBufferPOD(T*& buffer, KyUInt32 count);
	template <class T>
	void InitBufferCPP(T*& buffer, KyUInt32 count, char*& memory);
	template <class T>
	void InitBufferPOD(T*& buffer, KyUInt32 count, char*& memory);


public:
	// ---------------------------------- Public Data Members ----------------------------------

	/// The Database on which the Path has been computed. If you created the Path
	/// by yourself and want a Bot follow it you must set #m_database to the Database
	/// on which the Bot will move.
	Database* m_database;

	/// The ChannelArray maintaining Channels around the path sections laying on the NavMesh.
	Ptr<ChannelArray> m_channelArray;


private:
	KyUInt32 m_navigationProfileId;

	KyUInt32 m_byteSize;

	KyUInt32 m_nodesCount;
	KyUInt32 m_edgesCount;

	KyFloat32 m_pathDistance;
	KyFloat32 m_pathCost;

	CellBox m_pathCellBox;

	Vec3f*             m_nodePositions;
	WorldIntegerPos*   m_nodeIntegerPositions;
	NavTrianglePtr*    m_nodeTrianglePtrs;
	NavGraphVertexPtr* m_nodeGraphVertexPtrs;

	NavGraphEdgePtr* m_edgeNavGraphEdgePtr;
	KyUInt8*         m_edgeTypes;

	KyUInt32 RefCount; // consider thread-safetyness here with mutable AtomicInt<int> RefCount;

public: // internal
	// Lifetime - separate implementation due to Create function
	void AddRef();
	void Release();
	// Debug method only.
	int GetRefCount() const { return RefCount; }
};


}

#include "gwnavruntime/path/path.inl"



#endif
