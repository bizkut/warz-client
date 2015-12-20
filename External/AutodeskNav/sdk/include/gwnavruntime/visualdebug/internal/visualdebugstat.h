/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// primary contact: BRGR - secondary contact: NOBODY
#ifndef Navigation_VisualDebugStat_H
#define Navigation_VisualDebugStat_H


#include "gwnavruntime/blob/blobfieldarray.h"
#include "gwnavruntime/blob/blobfieldstring.h"
#include "gwnavruntime/blob/blobfield32.h"
#include "gwnavruntime/base/types.h"
#include "gwnavruntime/math/fastmath.h"


namespace Kaim
{

class VisualDebugStats
{
public:
	VisualDebugStats(const char* name, KyFloat32 current, KyFloat32 average, KyFloat32 max, KyFloat32 min)
		: m_name(name), m_current(current), m_average(average), m_max(max), m_min(min) {}

public:
	String    m_name;
	KyFloat32 m_current;
	KyFloat32 m_average;
	KyFloat32 m_max;
	KyFloat32 m_min;
};

class VisualDebugSingleUInt32
{
public:
	VisualDebugSingleUInt32(const char* name, KyUInt32 value)
		: m_name(name), m_value(value)
	{}
	
public:
	String m_name;
	KyUInt32 m_value;
};

class VisualDebugSingleInt32
{
public:
	VisualDebugSingleInt32(const char* name, KyInt32 value)
		: m_name(name), m_value(value) {}

public:
	String m_name;
	KyInt32 m_value;
};

class VisualDebugSingleFloat32
{
public:
	VisualDebugSingleFloat32(const char* name, KyFloat32 value)
		: m_name(name), m_value(value) {}
	
public:
	String m_name;
	KyFloat32 m_value;
};


class VisualDebugSingleString
{
public:
	VisualDebugSingleString(const char* name, const char* value)
		: m_name(name), m_value(value) {}

	String m_name;
	String m_value;
};

class VisualDebugStatGroup
{
	KY_DEFINE_NEW_DELETE_OPERATORS(MemStat_VisualDebug)
public:
	VisualDebugStatGroup() : m_name("") {}

	VisualDebugStatGroup(const char* name) : m_name(name) {}

	void AddUInt32(const char* statName, KyUInt32 value)
	{ m_singleUInt32.PushBack(VisualDebugSingleUInt32(statName, value)); }

	void AddInt32(const char* statName, KyInt32 value)
	{ m_singleInt32.PushBack(VisualDebugSingleInt32(statName, value)); }
	
	void AddFloat32(const char* statName, KyFloat32 value)
	{ m_singleFloat32.PushBack(VisualDebugSingleFloat32(statName, value)); }

	void AddString(const char* statName, String value)
	{ m_singleString.PushBack(VisualDebugSingleString(statName, value)); }

	void AddStats(const char* statName, const KyFloat32 value, const KyFloat32 average, const KyFloat32 max, const KyFloat32 min)
	{ m_stats.PushBack(VisualDebugStats(statName, value, average, max, min)); }

	void Map(BlobFieldsMapping& mapping);

public:
	String m_name;
	KyArray<VisualDebugStats> m_stats;
	KyArray<VisualDebugSingleUInt32> m_singleUInt32;
	KyArray<VisualDebugSingleInt32> m_singleInt32;
	KyArray<VisualDebugSingleFloat32> m_singleFloat32;
	KyArray<VisualDebugSingleString> m_singleString;
};


class SentBlobTypeStats
{
	KY_DEFINE_NEW_DELETE_OPERATORS(MemStat_VisualDebug)
public:
	SentBlobTypeStats(const char* name);

	void Clear();

	void OnSend(KyUInt32 sendSize);

	void SetMinMax( KyUInt32 sendSize );

	KyUInt32 m_sentSizeInOctets;
	KyUInt32 m_sentCount;
	KyUInt32 m_averageSize;
	KyUInt32 m_minSize;
	KyUInt32 m_maxSize;
	BlobMultiField32Mapping m_statMapping;
};


}

#endif
