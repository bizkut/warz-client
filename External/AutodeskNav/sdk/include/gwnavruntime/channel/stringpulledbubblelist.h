/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/

#ifndef Navigation_StringPulledBubbleList_H
#define Navigation_StringPulledBubbleList_H

#include "gwnavruntime/channel/bubblearray.h"

#include "gwnavruntime/visualsystem/displaylistblob.h"
#include "gwnavruntime/kernel/SF_String.h"


namespace Kaim
{

class World;


class StringPullerPersistentDisplayListIds
{
public:
	StringPullerPersistentDisplayListIds()
		: m_world(KY_NULL)
		, m_singleFrame(true)
		, m_displayListId_Edges(KyUInt32MAXVAL)
	{}

	~StringPullerPersistentDisplayListIds() {}

	void Initialize(World* world);
	void Release();

public:
	World* m_world;
	bool m_singleFrame;

	BubbleArrayPersistentDisplayListIds m_bubbleArrayPersistentDisplayListIds;
	KyUInt32 m_displayListId_Edges;
};

class StringPulledBubbleListDisplayConfig
{
public:
	StringPulledBubbleListDisplayConfig() { SetDefaults(); }

	void SetDefaults();

	VisualShapeColor m_edgeColor;
	KyFloat32 m_arrowWidth; // if 0.0f, render just a line

	BubbleArrayDisplayConfig m_bubbleArrayDisplayConfig;
	StringPullerPersistentDisplayListIds m_persistentDisplayListIds;
};



class StringPulledEdge
{
public:
	StringPulledEdge() {}

	Vec3f m_start;
	Vec3f m_end;
	Vec2f m_normalizedPerpCCW;
};

class StringPulledBubbleList
{
public:
	StringPulledBubbleList() {}

	void Clear();
	void Reserve(KyUInt32 bubbleCount);
	void PushBubble(const Bubble& bubble) { m_bubbleArray.PushBack(bubble); }
	KyResult Finalize();

	bool                        IsEmpty()                   const { return m_bubbleArray.IsEmpty();  }
	KyUInt32                    GetBubbleCount()            const { return m_bubbleArray.GetCount(); }
	const Bubble&               GetBubble(KyUInt32 nodeIdx) const { return m_bubbleArray[nodeIdx];   }
	KyUInt32                    GetEdgeCount()              const { return m_edges.GetCount();   }
	const StringPulledEdge&     GetEdge(KyUInt32 edgeIdx)   const { return m_edges[edgeIdx];     }

	void SendVisualDebug(World* world, const StringPulledBubbleListDisplayConfig& displayConfig, const char* listBaseName = "", const char* groupName = "StringPuller", KyUInt32 visualDebugId = KyUInt32MAXVAL) const;

	Bubble& GetBubble(KyUInt32 nodeIdx) { return m_bubbleArray[nodeIdx]; }

	KyResult ComputeEdge(KyUInt32 edgeIdx);
	void SetEdge(KyUInt32 edgeIdx, const Vec3f& start, const Vec3f& end);

private:
	KyResult ComputeEdges();
	void ComputeEdgeNormal(StringPulledEdge& edge);

	BubbleArray m_bubbleArray;
	KyArray<StringPulledEdge> m_edges;
};

} // namespace Kaim

#endif // Navigation_StringPulledBubbleList_H
