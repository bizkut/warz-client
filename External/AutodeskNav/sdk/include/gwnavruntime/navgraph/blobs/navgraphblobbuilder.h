/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/

// Primary contact: LASI - secondary contact: NONE
#ifndef Navigation_NavGraphBlobBuilder_H
#define Navigation_NavGraphBlobBuilder_H

#include "gwnavruntime/blob/baseblobbuilder.h"
#include "gwnavruntime/navgraph/blobs/navgraphblobbuildertypes.h"
#include "gwnavruntime/kernel/SF_String.h"


namespace Kaim
{

class NavGraphBlob;

/// This class allows you to build a NavGraph, which you can then add to the NavData for a sector.
/// This class is typically used in a custom post-processing phase after generating the NavMesh for
/// a terrain.
class NavGraphBlobBuilder : public BaseBlobBuilder<NavGraphBlob>
{
public:
	NavGraphBlobBuilder();

	KyUInt32 GetEdgeCount()   const;
	KyUInt32 GetVertexCount() const;
	KyUInt32 GetNavTagCount() const;

	/// Adds a NavTag that will be associated to edge and/or vertices.

	/// \param dynamicNavTag	A pointer to the associated dynamicNavTag that will be added to the NavGraph.
	/// \return the index of the NavTag that should pass as parameters to associate this NavTag to vertices and/or edges.
	KyUInt32 AddNavTag(const DynamicNavTag* dynamicNavTag);


	// ---- Add vertices ----

	/// Adds a new vertex to the Graph with no associated NavTag.
	/// \param position			The position of the vertex in 3D space, in the Gameware Navigation coordinate system.
	/// \param vertexType		A tag provides the way the vertex will be connected or not at run-time.
	/// \return the index of the newly created vertex. 
	KyUInt32 AddVertexWithoutNavTag(const Vec3f& position, NavGraphVertexLinkType vertexType = NavGraphVertexLinkType_LinkToNavMesh);

	/// Adds a new vertex to the Graph with an associated NavTag that is not supposed to be associated to other vertices or edges.
	/// \param position			The position of the vertex in 3D space, in the Gameware Navigation coordinate system.
	/// \param dynamicNavTag	A pointer to the associated dynamicNavTag that will be added to the NavGraph.
	/// \param vertexType		A tag providing the way the vertex will be connected at run-time (if conntected).
	/// \return the index of the newly created vertex. 
	KyUInt32 AddVertexWithExclusiveNavTag(const Vec3f& position, DynamicNavTag* dynamicNavTag, NavGraphVertexLinkType vertexType = NavGraphVertexLinkType_LinkToNavMesh);

	/// Adds a new vertex to the Graph with an associated NavTag that has already been added to the graph.
	/// \param position		The position of the vertex in 3D space, in the Gameware Navigation coordinate system.
	/// \param navTagIdx	The associated navTag index that has already been added to Graph .
	/// \param vertexType	A tag provides the way the vertex will be connected or not at run-time.
	/// \return the index of the newly created vertex. 
	KyUInt32 AddVertexWithNavTag(const Vec3f& position, KyUInt32 navTagIdx, NavGraphVertexLinkType vertexType = NavGraphVertexLinkType_LinkToNavMesh);

	// ---- Add Monodirectional edges --- 

	/// Adds a new edge that connects the specified vertices. NOTE: the edge is mono-directional, from
	/// the startVertex to the endVertex. If you want your characters to be able to plan paths that go
	/// between the two vertices in either direction, you must add a second edge from the endVertex to the
	///startVertex, or use AddBidirectionalEdgeWithoutNavTag() instead.
	/// No navTag will be associated to this edge.
	/// \param startVertexIdx	The index of the starting vertex for the edge, which was returned by AddVertex().
	/// \param endVertexIdx		The index of the ending vertex for the edge, which was returned by AddVertex().
	/// \return the index of the newly created edge. 
	KyUInt32 AddMonodirectionalEdgeWithoutNavTag(KyUInt32 startVertexIdx, KyUInt32 endVertexIdx);

	/// Adds a new edge that connects the specified vertices. NOTE: the edge is mono-directional, from
	/// the startVertex to the endVertex. If you want your characters to be able to plan paths that go
	/// between the two vertices in either direction, you must add a second edge from the endVertex to the
	///startVertex, or use AddBidirectionalEdgeWithoutNavTag() instead.
	/// The edge will be associated to the NavTag that is not supposed to be associated to other vertices or edges
	/// \param startVertexIdx	The index of the starting vertex for the edge, which was returned by AddVertex().
	/// \param endVertexIdx		The index of the ending vertex for the edge, which was returned by AddVertex().
	/// \param dynamicNavTag	A pointer to the associated dynamicNavTag that will be added to the NavGraph
	/// \return the index of the newly created edge. 
	KyUInt32 AddMonodirectionalEdgeWithExclusiveNavTag(KyUInt32 startVertexIdx, KyUInt32 endVertexIdx, DynamicNavTag* dynamicNavTag);

	/// Adds a new edge that connects the specified vertices. Note that the edge is mono-directional, from
	/// the startVertex to the endVertex. If you want your characters to be able to plan paths that go
	/// between the two vertices in either direction, you must add a second edge from the endVertex to the
	///startVertex, or use AddBidirectionalEdgeWithoutNavTag() instead.
	/// the edge will be associated to the NavTag that is not supposed to be associated to other vertices or edges
	/// \param startVertexIdx		The index of the starting vertex for the edge, which was returned by AddVertex().
	/// \param endVertexIdx			The index of the ending vertex for the edge, which was returned by AddVertex().
	/// \param navTagIdx			The associated navTag index that has already been added to Graph 
	/// \return the index of the newly created edge. 
	KyUInt32 AddMonodirectionalEdgeWithNavTag(KyUInt32 startVertexIdx, KyUInt32 endVertexIdx, KyUInt32 navTagIdx);

	// ---- Add Bidirectional edges --- 

	/// Adds a bidirectional edge that connects the specified vertices.
	/// NOTE: this function will create two mono-directional edges. One from startVertexIdx to endVertexIdx and
	/// One from endVertexIdx to startVertexIdx.
	/// No navTag will be associated to this edge.
	/// \param startVertexIdx		The index of the starting vertex for the edge, which was returned by AddVertex().
	/// \param endVertexIdx			The index of the ending vertex for the edge, which was returned by AddVertex().
	void AddBidirectionalEdgeWithoutNavTag(KyUInt32 startVertexIdx, KyUInt32 endVertexIdx);

	/// Adds a bidirectional edge that connects the specified vertices.
	/// NOTE: this function will create two mono-directional edges. One from startVertexIdx to endVertexIdx and
	/// one from endVertexIdx to startVertexIdx.
	/// The edge will be associated to the NavTag that is not supposed to be associated to other vertices or edges
	/// \param startVertexIdx	The index of the starting vertex for the edge, which was returned by AddVertex().
	/// \param endVertexIdx		The index of the ending vertex for the edge, which was returned by AddVertex().
	/// \param dynamicNavTag	A pointer to the associated dynamicNavTag that will be added to the NavGraph
	void AddBidirectionalEdgeWithExclusiveNavTag(KyUInt32 startVertexIdx, KyUInt32 endVertexIdx, DynamicNavTag* dynamicNavTag);

	/// Adds a bidirectional edge that connects the specified vertices.
	/// NOTE: this function will create two mono-directional edges. One from startVertexIdx to endVertexIdx and
	/// One from endVertexIdx to startVertexIdx
	/// the edge will be associated to the NavTag that is not supposed to be associated to other vertices or edges
	/// \param startVertexIdx		The index of the starting vertex for the edge, which was returned by AddVertex().
	/// \param endVertexIdx			The index of the ending vertex for the edge, which was returned by AddVertex().
	/// \param navTagIdx			The associated navTag index that has already been added to Graph 
	void AddBidirectionalEdgeWithNavTag(KyUInt32 startVertexIdx, KyUInt32 endVertexIdx, KyUInt32 navTagIdx);

	void SetName(const char* graphName); ///< // optional, only for Visual Debug
private:
	virtual void DoBuild();

public: // internal
	KyUInt32 m_visualDebugId; // unused since NavGraphBlobs were sent into NavGraphArray blob from a NavData
	String m_name; // optional, only for Visual Debug
	KyArrayPOD<const DynamicNavTag*> m_navTags;
	KyArray<NavGraphBlobBuilder_Vertex> m_vertices; //< Graph Vertices.
	KyArray<NavGraphBlobBuilder_Edge> m_edges; //< Graph Edges.
};


KY_INLINE KyUInt32 NavGraphBlobBuilder::GetEdgeCount()   const { return m_edges.GetCount();    }
KY_INLINE KyUInt32 NavGraphBlobBuilder::GetVertexCount() const { return m_vertices.GetCount(); }
KY_INLINE KyUInt32 NavGraphBlobBuilder::GetNavTagCount() const { return m_navTags.GetCount();  }

}

#endif // Navigation_NavGraphBlobBuilder_H
