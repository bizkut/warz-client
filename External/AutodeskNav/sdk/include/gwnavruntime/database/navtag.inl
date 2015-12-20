/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// Primary contact: JUBA - secondary contact: NOBODY

namespace Kaim
{



KY_INLINE DynamicNavTag::DynamicNavTag() { Clear(); }

KY_INLINE void DynamicNavTag::Clear()
{
	m_flags = 0;
	m_blindDataArray.Clear();
	SetColor(GetDefaultVisualColor());
}

KY_INLINE void DynamicNavTag::SetAsExclusive()
{
	SetBitInMask<NAVTAG_IS_EXCLUSIVE_BIT>(m_flags);
	m_blindDataArray.Clear();
	KY_LOG_WARNING_IF(GetVisualColor() != GetDefaultVisualColor() && GetVisualColor() != GetExclusiveVisualColor(),
		("When setting a TagVolume as exclusive, its color is overwritten to be Red (%d,%d,%d)", 
		GetExclusiveVisualColor().m_r, GetExclusiveVisualColor().m_g, GetExclusiveVisualColor().m_b));
	SetColor(GetExclusiveVisualColor());
}
KY_INLINE void DynamicNavTag::SetAsWalkable()
{
	UnSetBitInMask<     NAVTAG_IS_EXCLUSIVE_BIT>(m_flags);
	if (GetVisualColor() == GetExclusiveVisualColor())
		SetColor(GetDefaultVisualColor());
}

KY_INLINE void DynamicNavTag::SetOnNavGraph()          {   SetBitInMask<   NAVTAG_IS_ON_NAVGRAPH_BIT>(m_flags); }
KY_INLINE void DynamicNavTag::SetOnNavMesh()           { UnSetBitInMask<   NAVTAG_IS_ON_NAVGRAPH_BIT>(m_flags); }
KY_INLINE void DynamicNavTag::SetFromGeneration()      {   SetBitInMask<   NAVTAG_IS_FROM_GENERATION>(m_flags); }
KY_INLINE void DynamicNavTag::SetAsGenerationDefault() {   SetBitInMask<NAVTAG_IS_GENERATION_DEFAULT>(m_flags); }

KY_INLINE bool DynamicNavTag::operator!=(const DynamicNavTag& other) const { return !operator==(other); }

KY_INLINE bool DynamicNavTag::operator==(const NavTag& other) const { return IsEqualToNavTag(other); }
KY_INLINE bool DynamicNavTag::operator!=(const NavTag& other) const { return !operator==(other); }

KY_INLINE bool DynamicNavTag::IsEmpty()             const { return IsExclusive() == false && m_blindDataArray.IsEmpty();  }
KY_INLINE bool DynamicNavTag::IsExclusive()         const { return IsBitSetInMask<NAVTAG_IS_EXCLUSIVE_BIT>(m_flags);      }
KY_INLINE bool DynamicNavTag::IsWalkable()          const { return IsExclusive() == false;                                }
KY_INLINE bool DynamicNavTag::IsOnNavGraph()        const { return IsBitSetInMask<NAVTAG_IS_ON_NAVGRAPH_BIT>(m_flags);    }
KY_INLINE bool DynamicNavTag::IsOnNavMesh()         const { return IsOnNavMesh() == false;                                }
KY_INLINE bool DynamicNavTag::IsFromGeneration()    const { return IsBitSetInMask<NAVTAG_IS_FROM_GENERATION>(m_flags);    }
KY_INLINE bool DynamicNavTag::IsFromRuntime()       const { return IsFromGeneration() == false;                           }
KY_INLINE bool DynamicNavTag::IsGenerationDefault() const { return IsBitSetInMask<NAVTAG_IS_GENERATION_DEFAULT>(m_flags); }

KY_INLINE VisualColor DynamicNavTag::GetVisualColor() const { return VisualColor(m_colorR, m_colorG, m_colorB); }

KY_INLINE void DynamicNavTag::SetColor(VisualColor color) { SetColor(color.m_r, color.m_g, color.m_b); }
KY_INLINE void DynamicNavTag::SetColor(KyUInt8 r, KyUInt8 g, KyUInt8 b)
{
	KY_LOG_WARNING_IF(IsExclusive() && VisualColor(r, g, b) != GetExclusiveVisualColor(),
		("you are modifying the color of an exclusive TagVolume which are supposed to be Red (%d,%d,%d)", 
		GetExclusiveVisualColor().m_r, GetExclusiveVisualColor().m_g, GetExclusiveVisualColor().m_b));
	m_colorR = r;
	m_colorG = g;
	m_colorB = b;
}
KY_INLINE void DynamicNavTag::InitFromBlob(const NavTag& navTag)
{
	Init(navTag.GetWords(), navTag.GetWordCount());
	m_flags  = navTag.m_flags;
	SetColor(navTag.m_colorR, navTag.m_colorG, navTag.m_colorB);
}


KY_INLINE bool NavTag::operator!=(const NavTag& other) const { return !operator==(other); }
KY_INLINE bool NavTag::operator==(const DynamicNavTag& other) const { return other == *this; }
KY_INLINE bool NavTag::operator!=(const DynamicNavTag& other) const { return !operator==(other); }

KY_INLINE bool NavTag::IsExclusive()         const { return IsBitSetInMask<NAVTAG_IS_EXCLUSIVE_BIT>(m_flags);      }
KY_INLINE bool NavTag::IsWalkable()          const { return IsExclusive() == false;                                }
KY_INLINE bool NavTag::IsOnNavGraph()        const { return IsBitSetInMask<NAVTAG_IS_ON_NAVGRAPH_BIT>(m_flags);    }
KY_INLINE bool NavTag::IsOnNavMesh()         const { return IsOnNavGraph() == false;                               }
KY_INLINE bool NavTag::IsFromGeneration()    const { return IsBitSetInMask<NAVTAG_IS_FROM_GENERATION>(m_flags);    }
KY_INLINE bool NavTag::IsFromRuntime()       const { return IsFromGeneration() == false;                           }
KY_INLINE bool NavTag::IsGenerationDefault() const { return IsBitSetInMask<NAVTAG_IS_GENERATION_DEFAULT>(m_flags); }

KY_INLINE KyUInt32        NavTag::GetByteSize()    const { return KyUInt32((char*)(m_blindData.GetValues() + m_blindData.GetCount()) - (char*)this); }
KY_INLINE VisualColor     NavTag::GetVisualColor() const { return VisualColor(m_colorR, m_colorG, m_colorB); }
KY_INLINE       KyUInt32  NavTag::GetWordCount()   const { return m_blindData.GetCount();  }
KY_INLINE const KyUInt32* NavTag::GetWords()       const { return m_blindData.GetValues(); }

KY_INLINE KyUInt32  NavTag::GetWord(KyUInt32 wordIdx) const
{
	KY_DEBUG_ASSERTN(wordIdx < GetWordCount(), ("Wrong index pass to this function. input is %u, it must be lower than %u", wordIdx, GetWordCount()));
	return GetWords()[wordIdx];
}

inline void NavTagCopier::DoBuild()
{
	KyUInt32* blobArray = BLOB_ARRAY(m_blob->m_blindData, m_navTag->GetWordCount());
	if (this->IsWriteMode())
	{
		m_blob->m_flags  = m_navTag->m_flags;
		m_blob->m_colorR = m_navTag->m_colorR;
		m_blob->m_colorG = m_navTag->m_colorG;
		m_blob->m_colorB = m_navTag->m_colorB;

		for (KyUInt32 i = 0; i < m_navTag->GetWordCount(); ++i)
			blobArray[i] = m_navTag->GetWord(i);
	}
}

inline void NavTagBlobBuilder::DoBuild()
{
	KyUInt32* blobArray = BLOB_ARRAY(m_blob->m_blindData, m_navTag->m_blindDataArray.GetCount());
	if (IsWriteMode())
	{
		m_blob->m_flags  = m_navTag->m_flags;
		m_blob->m_colorR = m_navTag->m_colorR;
		m_blob->m_colorG = m_navTag->m_colorG;
		m_blob->m_colorB = m_navTag->m_colorB;

		for (KyUInt32 i = 0; i < m_navTag->m_blindDataArray.GetCount(); ++i)
			blobArray[i] = m_navTag->m_blindDataArray[i];
	}
}

}