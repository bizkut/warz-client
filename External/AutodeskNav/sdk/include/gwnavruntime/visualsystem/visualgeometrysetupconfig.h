/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/




// primary contact: GUAL - secondary contact: MAMU
#ifndef Navigation_VisualGeometrySetupConfig_H
#define Navigation_VisualGeometrySetupConfig_H

#include "gwnavruntime/base/types.h"


namespace Kaim
{

/// The VisualGeometrySetupConfig class is used to configure an object that derives from IVisualGeometry. 
class VisualGeometrySetupConfig
{
public:
	VisualGeometrySetupConfig();
	void ResetAllButNormaleGeneration();
	void Reset();
public: // Internal
	KyUInt32 m_opaqueTriangleCount;
	KyUInt32 m_transparentTriangleCount;
	KyUInt32 m_lineCount;
	KyUInt32 m_textCount;
	bool m_generateNormals; ///< Indicates whether or not the IVisualGeometry has computed the normals ot the triangles.
};


KY_INLINE VisualGeometrySetupConfig::VisualGeometrySetupConfig() { Reset(); }
KY_INLINE void VisualGeometrySetupConfig::Reset()
{
	ResetAllButNormaleGeneration();
	m_generateNormals = false;
}
KY_INLINE void VisualGeometrySetupConfig::ResetAllButNormaleGeneration()
{
	m_opaqueTriangleCount = 0;
	m_transparentTriangleCount = 0;
	m_lineCount = 0;
	m_textCount  = 0;
}

}


#endif //Navigation_VisualGeometrySetupConfig_H
