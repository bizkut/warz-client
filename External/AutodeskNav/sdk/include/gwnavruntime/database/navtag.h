/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/



// primary contact: GUAL - secondary contact: LASI
#ifndef Navigation_NavTag_H
#define Navigation_NavTag_H

#include "gwnavruntime/blob/baseblobbuilder.h"
#include "gwnavruntime/navmesh/navmeshtypes.h"
#include "gwnavruntime/containers/kyarray.h"
#include "gwnavruntime/navdata/navdatablobcategory.h"
#include "gwnavruntime/visualsystem/visualcolor.h"
#include "gwnavruntime/basesystem/logger.h"

namespace Kaim
{

typedef KyUInt32 NavTagIdx;
static const NavTagIdx NavTagIdx_Invalid = KyUInt32MAXVAL;

class NavTag;

enum NavTagBitFlag
{
	NAVTAG_IS_EXCLUSIVE_BIT      = 0,
	NAVTAG_IS_ON_NAVGRAPH_BIT    = 1,
	NAVTAG_IS_FROM_GENERATION    = 2,
	NAVTAG_IS_GENERATION_DEFAULT = 3, // only relevant if from generation
};

class DynamicNavTag
{
	KY_DEFINE_NEW_DELETE_OPERATORS(Stat_Default_Mem)
public:
	static VisualColor GetDefaultVisualColor()    { return VisualColor::DarkOrange; }
	static VisualColor GetExclusiveVisualColor() { return VisualColor::Red;        }

	DynamicNavTag();

	void Clear();

	void Init(const KyUInt32* blindDataBuffer, KyUInt32 blindDataCount);
	void InitFromBlob(const NavTag& navTag);

	bool operator <(const DynamicNavTag& other) const;
	bool operator!=(const DynamicNavTag& other) const;
	bool operator==(const DynamicNavTag& other) const;// NavTags are compared using the values in the blind array
	
	bool operator <(const NavTag& other) const;
	bool operator!=(const NavTag& other) const;
	bool operator==(const NavTag& other) const;
	
	bool IsEqualToNavTag(const NavTag& other) const;

	bool IsEmpty()             const;
	bool IsExclusive()         const;
	bool IsWalkable()          const;
	bool IsOnNavGraph()        const;
	bool IsOnNavMesh()         const;
	bool IsFromGeneration()    const;
	bool IsFromRuntime()       const;
	bool IsGenerationDefault() const;

	VisualColor GetVisualColor() const;

	void SetAsExclusive();
	void SetAsWalkable();
	void SetColor(VisualColor color);
	void SetColor(KyUInt8 r, KyUInt8 g, KyUInt8 b);

public: //Internal
	void SetOnNavGraph();
	void SetOnNavMesh();
	void SetFromGeneration();
	void SetAsGenerationDefault();

public:
	KyArrayPOD<KyUInt32> m_blindDataArray;
	KyUInt8 m_flags; // tells if the navTag is exclusive, if it is on the navMesh, or on the navGrah ...
	KyUInt8 m_colorR;
	KyUInt8 m_colorG;
	KyUInt8 m_colorB;
};

class NavTag
{
	KY_DEFINE_NEW_DELETE_OPERATORS(Stat_Default_Mem)
	KY_ROOT_BLOB_CLASS(NavData, NavTag, 1)
	KY_CLASS_WITHOUT_COPY(NavTag)
public:
	static VisualColor GetDefaultVisualColor()    { return DynamicNavTag::GetDefaultVisualColor();    }
	static VisualColor GetExclusivetVisualColor() { return DynamicNavTag::GetExclusiveVisualColor(); }

	NavTag() {}

	bool operator!=(const NavTag& other) const;
	bool operator==(const NavTag& other) const;
	bool operator<(const NavTag& other)  const;

	bool operator <(const DynamicNavTag& other) const;
	bool operator!=(const DynamicNavTag& other) const;
	bool operator==(const DynamicNavTag& other) const;

	KyUInt32  GetWordCount()            const;
	const KyUInt32* GetWords()          const;
	KyUInt32  GetWord(KyUInt32 wordIdx) const;
	KyUInt32 GetByteSize()              const;

	bool IsWalkable()          const;
	bool IsOnNavMesh()         const;
	bool IsExclusive()         const;
	bool IsOnNavGraph()        const;
	bool IsFromGeneration()    const;
	bool IsFromRuntime()       const;
	bool IsGenerationDefault() const;

	VisualColor GetVisualColor() const;


public:
	BlobArray<KyUInt32> m_blindData;
	KyUInt8 m_flags; // tells if the navTag is exclusive, if it is on the navMesh, or on the navGrah
	KyUInt8 m_colorR;
	KyUInt8 m_colorG;
	KyUInt8 m_colorB;
};


inline void SwapEndianness(Endianness::Target e, NavTag& self)
{
	SwapEndianness(e, self.m_blindData);
	SwapEndianness(e, self.m_flags);
	SwapEndianness(e, self.m_colorR);
	SwapEndianness(e, self.m_colorG);
	SwapEndianness(e, self.m_colorB);
}

class NavTagBlobBuilder : public BaseBlobBuilder<NavTag>
{
public:
	NavTagBlobBuilder(const DynamicNavTag& navTag) : m_navTag(&navTag) {}
private:
	void DoBuild();

	const DynamicNavTag* m_navTag;
};

class NavTagCopier : public BaseBlobBuilder<NavTag>
{
public:
	NavTagCopier(const NavTag& navTag) : m_navTag(&navTag) {}
private:
	void DoBuild();
	const NavTag* m_navTag;
};

}

#include "gwnavruntime/database/navtag.inl"

#endif

