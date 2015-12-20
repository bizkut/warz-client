/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// primary contact: MAMU - secondary contact: NOBODY

#ifndef Navigation_ChannelArray_Blob_H
#define Navigation_ChannelArray_Blob_H

#include "gwnavruntime/world/runtimeblobcategory.h"
#include "gwnavruntime/blob/baseblobbuilder.h"
#include "gwnavruntime/math/vec3f.h"

namespace Kaim
{

class Gate;
class Channel;
class ChannelArray;

class GateBlob
{
public:
	GateBlob() {}

	KyUInt32 m_type; // cast this as GateType
	Vec3f m_leftPos;
	Vec3f m_pathPos;
	Vec3f m_rightPos;
};

inline void SwapEndianness(Endianness::Target e, GateBlob& self)
{
	SwapEndianness(e, self.m_type);
	SwapEndianness(e, self.m_leftPos);
	SwapEndianness(e, self.m_pathPos);
	SwapEndianness(e, self.m_rightPos);
}

class GateBlobBuilder : public BaseBlobBuilder<GateBlob>
{
	KY_CLASS_WITHOUT_COPY(GateBlobBuilder)

public:
	GateBlobBuilder(const Gate* gate) : m_gate(gate) {}

	virtual void DoBuild();

private:
	const Gate* m_gate;
};

// ----------------------------- Channel blob -------------------------------

class ChannelBlob
{
	KY_ROOT_BLOB_CLASS(Runtime, ChannelBlob, 0)
	
public:
	ChannelBlob() {}

public:
	BlobArray<GateBlob> m_gates;
	BlobArray<Vec3f> m_preChannelPolyline;
	BlobArray<Vec3f> m_postChannelPolyline;
};

inline void SwapEndianness(Endianness::Target e, ChannelBlob& self)
{
	SwapEndianness(e, self.m_gates);
	SwapEndianness(e, self.m_preChannelPolyline);
	SwapEndianness(e, self.m_postChannelPolyline);
}

class ChannelBlobBuilder : public BaseBlobBuilder<ChannelBlob>
{
	KY_CLASS_WITHOUT_COPY(ChannelBlobBuilder)

public:
	ChannelBlobBuilder(const Channel* channel) : m_channel(channel) {}

	virtual void DoBuild();

private:
	const Channel* m_channel;
};


// ---------------------------------- Channel array blob ----------------------------------

class ChannelArrayBlob
{
	KY_ROOT_BLOB_CLASS(Runtime, ChannelArrayBlob, 0)

public:
	ChannelArrayBlob() {}

public:
	KyUInt32 m_visualDebugId;
	BlobArray<ChannelBlob> m_channelArray;
};

inline void SwapEndianness(Endianness::Target e, ChannelArrayBlob& self)
{
	SwapEndianness(e, self.m_visualDebugId);
	SwapEndianness(e, self.m_channelArray);
}

class ChannelArrayBlobBuilder : public BaseBlobBuilder<ChannelArrayBlob>
{
	KY_CLASS_WITHOUT_COPY(ChannelArrayBlobBuilder)

public:
	ChannelArrayBlobBuilder(const ChannelArray* channelArray, KyUInt32 visualDebugId) : m_channelArray(channelArray), m_visualDebugId(visualDebugId) {}

	virtual void DoBuild();

private:
	const ChannelArray* m_channelArray;
	KyUInt32 m_visualDebugId;
};


} // namespace Kaim

#endif
