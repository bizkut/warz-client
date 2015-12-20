/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/



// primary contact: LAPA - secondary contact: NOBODY

namespace Kaim
{

template<class ObstacleClass>
KY_INLINE void TagVolumeTrigger<ObstacleClass>::Init(ObstacleClass* obstacle)
{
	m_obstacleRawPtr = obstacle;
}

template<class ObstacleClass>
KY_INLINE void TagVolumeTrigger<ObstacleClass>::Clear()
{
	m_tagVolume = KY_NULL;
	m_obstacleRawPtr = KY_NULL;
}

template<class ObstacleClass>
KY_INLINE void TagVolumeTrigger<ObstacleClass>::Update(bool doTriggerTagVolume)
{
	if (doTriggerTagVolume)
	{
		if (m_tagVolume == KY_NULL)
		{
			TagVolumeInitConfig tagVolumeInitConfig;
			m_obstacleRawPtr->SetupTagVolumeInitConfig(tagVolumeInitConfig);
			CreateTagVolume(tagVolumeInitConfig);
			KY_ASSERT(m_tagVolume != KY_NULL);
		}
	}
	else
	{
		if ((m_tagVolume != KY_NULL) && (m_tagVolume->IsAddedToWorld()))
		{
			m_tagVolume->ResetObstacle();
			m_tagVolume->RemoveFromWorld();
			m_tagVolume = KY_NULL;
		}
	}
}

template<class ObstacleClass>
KY_INLINE void TagVolumeTrigger<ObstacleClass>::CreateTagVolume(TagVolumeInitConfig& tagVolumeInitConfig)
{
	KY_ASSERT(m_tagVolume == KY_NULL);
	m_tagVolume = *KY_NEW TagVolume;

	// Setup TagVolume with new contour and add it to the world
	m_tagVolume->Init(tagVolumeInitConfig);
	m_tagVolume->SetObstacle(m_obstacleRawPtr);
	m_tagVolume->AddToWorld();
}

template<class ObstacleClass>
KY_INLINE void TagVolumeTrigger<ObstacleClass>::OnRemoveFromWorld()
{
	if (m_tagVolume)
	{
		m_tagVolume->ResetObstacle();

		if (m_tagVolume->IsAddedToWorld())
			m_tagVolume->RemoveFromWorld();

		m_tagVolume = KY_NULL;
	}
}

template<class ObstacleClass>
KY_INLINE const TagVolume*       TagVolumeTrigger<ObstacleClass>::GetTagVolume() const { return m_tagVolume; }

template<class ObstacleClass>
KY_INLINE TagVolume*             TagVolumeTrigger<ObstacleClass>::GetTagVolume()       { return m_tagVolume; }

template<class ObstacleClass>
KY_INLINE TagVolume::IntegrationStatus TagVolumeTrigger<ObstacleClass>::GetIntegrationStatus() const
{
	return ((m_tagVolume == KY_NULL) ? TagVolume::IntegrationStatus_NotIntegrated : m_tagVolume->GetIntegrationStatus());
}

} // Kaim
