/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// primary contact: JUBA - secondary contact: LAPA

#ifndef Navigation_NavFloorToNavGraphLinks_H
#define Navigation_NavFloorToNavGraphLinks_H

#include "gwnavruntime/navgraph/navgraphlink.h"
#include "gwnavruntime/containers/bitfieldutils.h"

namespace Kaim
{

class NavGraphLink;
class NavGraphVertexRawPtr;

typedef TrackedCollection<NavGraphLink*, MemStat_NavData> NavGraphLinkCollection;

class NavFloorToNavGraphLinks
{
	KY_DEFINE_NEW_DELETE_OPERATORS(Stat_Default_Mem)

public:
	static KY_INLINE KyUInt32 GetNumberOfWordsForTriangleLinkStatus(KyUInt32 numberOfTriangle) { return BitFieldUtils::GetWordsCount(numberOfTriangle); }

public:
	NavFloorToNavGraphLinks();
	~NavFloorToNavGraphLinks();

	void Clear();
	KyUInt32 GetCount() const;
	NavGraphLink* GetNavGraphLink(KyUInt32 index);
	const NavGraphLink* GetNavGraphLink(KyUInt32 index) const;

	void AddNavGraphLink(NavGraphLink* navGraphlinkToAdd);
	void RemoveNavGraphLink(NavGraphLink* navGraphlinkToRemove);

	bool IsTriangleLinkedToGraph(NavTriangleIdx navTriangleIdx) const;
	void SetTriangleAsLinked(NavTriangleIdx navTriangleIdx);
	void SetTriangleAsNotLinked(NavTriangleIdx navTriangleIdx);
	void SetAllTriangleAsNotLinked(KyUInt32 triangleCount);

	void SetOffsetForTriangleLinkStatus(char* memoryStartForStatus, KyUInt32 triangleCount);

	void OnNavFloorDeActivate(KyUInt32 navFloorTriangleCount);
private:
	KyUInt32* GetBitFieldWords();
	const KyUInt32* GetBitFieldWords() const;
public:
	KyInt32 m_offsetToBitField; // KyUInt32* bitfiled = (KyUInt32*)this + m_offsetToBitField
	NavGraphLinkCollection m_navGraphLinks;
};

KY_INLINE NavFloorToNavGraphLinks::NavFloorToNavGraphLinks() : m_offsetToBitField(KyInt32MAXVAL) {}
KY_INLINE NavFloorToNavGraphLinks::~NavFloorToNavGraphLinks() { Clear(); }

KY_INLINE void NavFloorToNavGraphLinks::Clear() { m_navGraphLinks.Clear(); }
KY_INLINE KyUInt32 NavFloorToNavGraphLinks::GetCount() const { return m_navGraphLinks.GetCount(); }
KY_INLINE NavGraphLink* NavFloorToNavGraphLinks::GetNavGraphLink(KyUInt32 index) { return m_navGraphLinks[index]; }
KY_INLINE const NavGraphLink* NavFloorToNavGraphLinks::GetNavGraphLink(KyUInt32 index) const { return m_navGraphLinks[index]; }

KY_INLINE void NavFloorToNavGraphLinks::AddNavGraphLink(NavGraphLink* navGraphlinkToAdd)
{
	KY_DEBUG_ERRORN_IF(m_navGraphLinks.DoesContain(navGraphlinkToAdd),            ("this link already exists"));
	KY_LOG_ERROR_IF(navGraphlinkToAdd->m_navTriangleRawPtr.IsValid() == false,    ("this link is not valid"));
	KY_LOG_ERROR_IF(navGraphlinkToAdd->m_navGraphVertexRawPtr.IsValid() == false, ("this link is not valid"));
	m_navGraphLinks.PushBack(navGraphlinkToAdd);
	SetTriangleAsLinked(navGraphlinkToAdd->m_navTriangleRawPtr.GetTriangleIdx());
}

KY_INLINE bool NavFloorToNavGraphLinks::IsTriangleLinkedToGraph(NavTriangleIdx navTriangleIdx) const { return BitFieldUtils::IsBitSet(GetBitFieldWords(), navTriangleIdx); }
KY_INLINE void NavFloorToNavGraphLinks::SetTriangleAsLinked(NavTriangleIdx navTriangleIdx) { BitFieldUtils::SetBit(GetBitFieldWords(), navTriangleIdx); }
KY_INLINE void NavFloorToNavGraphLinks::SetTriangleAsNotLinked(NavTriangleIdx navTriangleIdx) { BitFieldUtils::UnsetBit(GetBitFieldWords(), navTriangleIdx); }
KY_INLINE void NavFloorToNavGraphLinks::SetAllTriangleAsNotLinked(KyUInt32 triangleCount)
{
	memset(GetBitFieldWords(), 0, sizeof(KyUInt32) * GetNumberOfWordsForTriangleLinkStatus(triangleCount));
}

KY_INLINE void NavFloorToNavGraphLinks::SetOffsetForTriangleLinkStatus(char* memoryStartForStatus, KyUInt32 triangleCount)
{
	m_offsetToBitField = (KyInt32)(memoryStartForStatus - (char*)this);
	SetAllTriangleAsNotLinked(triangleCount);
}

KY_INLINE KyUInt32* NavFloorToNavGraphLinks::GetBitFieldWords() { return (KyUInt32*)((char*)this + m_offsetToBitField); }
KY_INLINE const KyUInt32* NavFloorToNavGraphLinks::GetBitFieldWords() const { return (const KyUInt32*)((const char*)this + m_offsetToBitField); }

}

#endif // Navigation_NavFloorToNavGraphLinks_H
