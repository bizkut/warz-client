/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// primary contact: LAPA - secondary contact: NOBODY
#ifndef Navigation_TagVolumeDisplayListBuilder_H
#define Navigation_TagVolumeDisplayListBuilder_H

#include "gwnavruntime/visualsystem/idisplaylistbuilder.h"
#include "gwnavruntime/visualsystem/visualcolor.h"
#include "tagvolume.h"


namespace Kaim
{

class Database;
class TagVolumeBlob;
class TagVolumeContextBlob;


class TagVolumeDisplayListBuilder
{
public:
	TagVolumeDisplayListBuilder()
	{
		m_aabbColor_Projected = VisualColor::Lime;
		m_aabbColor_NotProjected  = VisualColor::Red;
		m_contourColor = VisualColor::Cyan;

		m_colorNotIntegrated = VisualColor::Red;
		m_colorToBeIntegrated = VisualColor::Orange;
		m_colorIntegrationInProcess = VisualColor::Orange;
		m_colorIntegrated = VisualColor::LimeGreen;

		m_displayWorldStatus = true;
		m_displayIntegrationStatus = true;
	}

	void DisplayTagVolumeContour(ScopedDisplayList* displayList, const TagVolumeBlob* tagVolumeBlob, Database* database);
	void DisplayTagVolumeAABB(ScopedDisplayList* displayList, const TagVolumeBlob* tagVolumeBlob);
	void DisplayTagVolumeObjectType(ScopedDisplayList* displayList, const TagVolumeBlob* tagVolumeBlob);
	void DisplayTagVolumeStatus(ScopedDisplayList* displayList, const TagVolumeBlob* tagVolumeBlob, const TagVolumeContextBlob* tagVolumeContextBlob);

	VisualColor GetIntegrationStatusColor(const TagVolume::IntegrationStatus tagVolumeIntegrationStatus) const;
	KyUInt8 GetAlphaForObstacleFromIntegrationStatus(const TagVolume::IntegrationStatus tagVolumeIntegrationStatus) const;

public:
	VisualColor m_aabbColor_Projected;
	VisualColor m_aabbColor_NotProjected;
	VisualColor m_contourColor;

	VisualColor m_colorNotIntegrated;
	VisualColor m_colorToBeIntegrated;
	VisualColor m_colorIntegrationInProcess;
	VisualColor m_colorIntegrated;

	bool m_displayWorldStatus;
	bool m_displayIntegrationStatus;
};

} // namespace Kaim

#endif // Navigation_TagVolumeDisplayListBuilder_H
