/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/

#ifndef Navigation_BubbleArray_H
#define Navigation_BubbleArray_H


#include "gwnavruntime/channel/bubble.h"
#include "gwnavruntime/containers/kyarray.h"
#include "gwnavruntime/visualsystem/visualshapecolor.h"


namespace Kaim
{

class World;

class BubbleArrayPersistentDisplayListIds
{
public:
	BubbleArrayPersistentDisplayListIds()
		: m_world(KY_NULL)
		, m_singleFrame(true)
		, m_displayListId_Bubbles(KyUInt32MAXVAL)
		, m_displayListId_BubbleIdx(KyUInt32MAXVAL)
		, m_displayListId_RefPos(KyUInt32MAXVAL)
	{}

	~BubbleArrayPersistentDisplayListIds() {}

	void Initialize(World* world);
	void Release();

public:
	World* m_world;
	bool m_singleFrame;

	KyUInt32 m_displayListId_Bubbles;
	KyUInt32 m_displayListId_BubbleIdx;
	KyUInt32 m_displayListId_RefPos;
};

class BubbleArrayDisplayConfig
{
public:
	BubbleArrayDisplayConfig() { SetDefaults(); }

	void SetDefaults();

	VisualShapeColor m_bubbleColor_CCW;
	VisualShapeColor m_bubbleColor_CW;
	VisualColor m_bubbleIdxColor;

	KyUInt32 m_diskSubdivision;
	KyFloat32 m_verticalOffset;

	BubbleArrayPersistentDisplayListIds m_persistentDisplayListIds;
};


/// This class encapsulate an array of Bubbles.
class BubbleArray
{
public:
	BubbleArray() {}
	~BubbleArray() { Clear(); }

	//-------------------- Array wrapping methods --------------------

	void Clear()                        { m_bubbleArray.Clear();              }
	void Reserve(KyUInt32 bubbleCount)  { m_bubbleArray.Reserve(bubbleCount); }
	void PushBack(const Bubble& bubble) { m_bubbleArray.PushBack(bubble);     }

	bool     IsEmpty()             const { return m_bubbleArray.IsEmpty();  }
	KyUInt32 GetCount()            const { return m_bubbleArray.GetCount(); }

	Bubble&       At(KyUInt32 nodeIdx)               { return m_bubbleArray.At(nodeIdx); }
	const Bubble& At(KyUInt32 nodeIdx)         const { return m_bubbleArray.At(nodeIdx); }

	Bubble&       operator[](KyUInt32 nodeIdx)       { return m_bubbleArray[nodeIdx]; }
	const Bubble& operator[](KyUInt32 nodeIdx) const { return m_bubbleArray[nodeIdx]; }


	//-------------------- Specific methods --------------------

	void SendVisualDebug(World* world, const BubbleArrayDisplayConfig& displayConfig, const char* listBaseName = "", const char* groupName = "BubbleArray", KyUInt32 visualDebugId = KyUInt32MAXVAL) const;

private:
	KyArray<Bubble> m_bubbleArray;
};

} // namespace Kaim

#endif // Navigation_BubbleArray_H
