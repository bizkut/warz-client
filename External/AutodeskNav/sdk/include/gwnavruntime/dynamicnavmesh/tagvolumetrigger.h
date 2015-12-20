/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/



// primary contact: LAPA - secondary contact: NOBODY
#ifndef Navigation_TagVolumeTrigger_H
#define Navigation_TagVolumeTrigger_H


#include "gwnavruntime/world/tagvolume.h"


namespace Kaim
{

/// TagVolumeTrigger is dedicated to TagVolume creation / removal management.
/// It is typically used in BoxObstacle and CylinderObstacle classes to delegate
/// TagVolume-related information.
template<class ObstacleClass>
class TagVolumeTrigger
{
	KY_DEFINE_NEW_DELETE_OPERATORS(MemStat_TagVolume)
	KY_CLASS_WITHOUT_COPY(TagVolumeTrigger)

public:
	// ---------------------------------- Public Member Functions ----------------------------------

	TagVolumeTrigger()
		: m_obstacleRawPtr(KY_NULL)
	{}

	~TagVolumeTrigger() {}

	void Init(ObstacleClass* obstacle);
	void Clear();

	void Update(bool doTriggerTagVolume);
	void CreateTagVolume(TagVolumeInitConfig& tagVolumeInitConfig);

	void OnRemoveFromWorld();

	const TagVolume* GetTagVolume() const;
	TagVolume* GetTagVolume();
	TagVolume::IntegrationStatus GetIntegrationStatus() const;


private:
	Ptr<TagVolume> m_tagVolume;
	ObstacleClass* m_obstacleRawPtr;
};

} // Kaim


#include "gwnavruntime/dynamicnavmesh/tagvolumetrigger.inl"


#endif // Navigation_TagVolumeTrigger_H
