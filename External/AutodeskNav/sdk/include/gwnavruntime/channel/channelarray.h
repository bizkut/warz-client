/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// primary contact: LAPA - secondary contact: NOBODY

#ifndef Navigation_ChannelArray_H
#define Navigation_ChannelArray_H

#include "gwnavruntime/channel/channel.h"
#include "gwnavruntime/channel/diagonalstrip.h"
#include "gwnavruntime/channel/stringpuller.h"

#include "gwnavruntime/kernel/SF_RefCount.h"


namespace Kaim
{

class Path;
class World;
class ChannelSectionPtr;
class Database;


/// This class aggregates Channels along a Path. There is one Channel for each NavMesh section
/// of the Path. NavGraph sections have no Channel representation.
class ChannelArray: public RefCountBase<ChannelArray, MemStat_Channel>
{
public:
	ChannelArray() {}
	virtual ~ChannelArray();

	void Clear();

	KyUInt32       GetChannelCount()          const { return m_channelArray.GetCount(); }
	Channel* GetChannel(KyUInt32 index) const { return ((index < GetChannelCount()) ? m_channelArray[index] : KY_NULL); }
	KyUInt32 GetChannelIdxFromPathNodeIdx(KyUInt32 pathNodeIdx) const;

	void SendVisualDebug(World* world) const;
	void SendVisualDebug(World* world, KyUInt32 visualDebugId) const;


public: // internal
	KyArray<Ptr<Channel> > m_channelArray;
};

} // namespace Kaim

#endif // Navigation_ChannelArray_H
