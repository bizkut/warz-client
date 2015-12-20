/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// Primary contact: GUAL - secondary contact: NOBODY

namespace Kaim
{

KY_INLINE void IVisualGeometryBuilder::EnableNormalGeneration()
{
	if (m_setupConfig.m_generateNormals == false)
	{
		KY_LOG_ERROR_IF(IsInFillMode(), ("Error, the generation of triangles normals has not been enabled during count mode"));
		m_setupConfig.m_generateNormals = true;
	}
}

KY_INLINE bool IVisualGeometryBuilder::IsInFillMode()  const { return m_buildMode == BuildMode_FILL;  }
KY_INLINE bool IVisualGeometryBuilder::IsInCountMode() const { return m_buildMode == BuildMode_COUNT; }

KY_INLINE IVisualGeometry*           IVisualGeometryBuilder::GetVisualGeometry() const { return m_visualGeometry; }
KY_INLINE VisualGeometryBuildConfig& IVisualGeometryBuilder::GetBuildConfig()          { return m_buildConfig;    }
KY_INLINE void IVisualGeometryBuilder::IncrementTextCount(KyUInt32 increment)
{
	m_setupConfig.m_textCount += increment;
}
KY_INLINE void IVisualGeometryBuilder::IncrementTriangleCount(KyUInt32 increment, const VisualColor& color)
{
	if (color == VisualShapeColor::Inactive())
		return;
	
	if (color.m_a == 255)
		m_setupConfig.m_opaqueTriangleCount += increment;
	else
		m_setupConfig.m_transparentTriangleCount += increment;
}

KY_INLINE void IVisualGeometryBuilder::IncrementLineCount(KyUInt32 increment, const VisualColor& lineColor)
{
	if (lineColor == VisualShapeColor::Inactive())
		return;

	m_setupConfig.m_lineCount += increment;
}

KY_INLINE void IVisualGeometryBuilder::PushLine(const Vec3f& P, const Vec3f& Q, const VisualColor& shapeColor, KyFloat32 width)
{
	if (IsInCountMode())
		IncrementLineCount(1, shapeColor);
	else
		FillLine(P, Q, shapeColor, width);
}

KY_INLINE void IVisualGeometryBuilder::PushTriangle(const Vec3f& A, const Vec3f& B, const Vec3f& C, const VisualShapeColor& shapeColor)
{
	if (IsInCountMode())
	{
		IncrementLineCount(3, shapeColor.m_lineColor);
		IncrementTriangleCount(1, shapeColor.m_triangleColor);
	}
	else
	{
		FillTriangle(A, B, C, shapeColor);
	}
}

KY_INLINE void IVisualGeometryBuilder::PushTriangle(const Triangle3f& triangle, const VisualShapeColor& shapeColor)
{
	PushTriangle(triangle.A, triangle.B, triangle.C, shapeColor);
}

KY_INLINE void IVisualGeometryBuilder::PushVerticalCylinder(const Vec3f& P, KyFloat32 radius, KyFloat32 height,
	KyUInt32 subdivisionCount, const VisualShapeColor& shapeColor)
{
	if (IsInCountMode())
	{
		IncrementLineCount(subdivisionCount * 3, shapeColor.m_lineColor);
		IncrementTriangleCount(subdivisionCount * 4, shapeColor.m_triangleColor);
	}
	else
	{
		FillVerticalCylinder(P, radius, height, subdivisionCount, shapeColor);
	}
}

KY_INLINE void IVisualGeometryBuilder::PushSegment(const Vec3f& A, const Vec3f& B, KyFloat32 halfWidth, const VisualShapeColor& shapeColor)
{
	if (IsInCountMode())
	{
		IncrementLineCount(12, shapeColor.m_lineColor);
		IncrementTriangleCount(12, shapeColor.m_triangleColor);
	}
	else
	{
		FillSegment(A, B, halfWidth, shapeColor);
	}
}

KY_INLINE void IVisualGeometryBuilder::PushQuad(const Vec3f& A, const Vec3f& B, const Vec3f& C, const Vec3f& D, const VisualShapeColor& shapeColor)
{
	if (IsInCountMode())
	{
		IncrementLineCount(4, shapeColor.m_lineColor);
		IncrementTriangleCount(2, shapeColor.m_triangleColor);
	}
	else
	{
		FillQuad(A, B, C, D, shapeColor);
	}
}

KY_INLINE void IVisualGeometryBuilder::PushQuad(const Vec3f& A, const Vec3f& B, KyFloat32 radius, const VisualShapeColor& shapeColor)
{
	if (IsInCountMode())
	{
		IncrementLineCount(4, shapeColor.m_lineColor);
		IncrementTriangleCount(2, shapeColor.m_triangleColor);
	}
	else
	{
		FillQuad(A, B, radius, shapeColor);
	}
}


KY_INLINE void IVisualGeometryBuilder::PushPoint(const Vec3f& P, KyFloat32 radius, const VisualShapeColor& shapeColor)
{
	if (IsInCountMode())
	{
		IncrementLineCount(2, shapeColor.m_lineColor);
		IncrementTriangleCount(12, shapeColor.m_triangleColor);
	}
	else
	{
		FillPoint(P, radius, shapeColor);
	}
}

KY_INLINE void IVisualGeometryBuilder::PushOrientedBox(const Transform& transform, const Box3f& extents, const VisualShapeColor& shapeColor)
{
	if (IsInCountMode())
	{
		IncrementLineCount(12, shapeColor.m_lineColor);
		IncrementTriangleCount(12, shapeColor.m_triangleColor);
	}
	else
	{
		FillOrientedBox(transform, extents, shapeColor);
	}
}

KY_INLINE void IVisualGeometryBuilder::PushBox(const Box3f& box, const VisualShapeColor& shapeColor)
{
	if (IsInCountMode())
	{
		IncrementLineCount(12, shapeColor.m_lineColor);
		IncrementTriangleCount(12, shapeColor.m_triangleColor);
	}
	else
	{
		FillBox(box, shapeColor);
	}
}

KY_INLINE void IVisualGeometryBuilder::PushDisk(const Vec3f& P, KyFloat32 radius, KyUInt32 subdivisionCount, const VisualShapeColor& shapeColor)
{
	if (IsInCountMode())
	{
		IncrementLineCount(subdivisionCount, shapeColor.m_lineColor);
		IncrementTriangleCount(subdivisionCount, shapeColor.m_triangleColor);
	}
	else
	{
		FillDisk(P, radius, subdivisionCount, shapeColor);
	}
}

KY_INLINE void IVisualGeometryBuilder::PushOrientedBox2d(const OrientedBox2d& orientedBox2d, const VisualShapeColor& shapeColor)
{
	if (IsInCountMode())
	{
		IncrementLineCount(12, shapeColor.m_lineColor);
		IncrementTriangleCount(12, shapeColor.m_triangleColor);
	}
	else
	{
		FillOrientedBox2d(orientedBox2d, shapeColor);
	}
}

KY_INLINE void IVisualGeometryBuilder::PushPyramid(const Vec3f& P, const Vec3f& Q, KyFloat32 halfWidth, const VisualShapeColor& shapeColor)
{
	if (IsInCountMode())
	{
		IncrementLineCount(8, shapeColor.m_lineColor);
		IncrementTriangleCount(6, shapeColor.m_triangleColor);
	}
	else
	{
		FillPyramid(P, Q, halfWidth, shapeColor);
	}
}

KY_INLINE void IVisualGeometryBuilder::PushSquareTubeSegment(const Vec3f& P, const Vec3f& Q, KyFloat32 halfWidth, const VisualShapeColor& shapeColor)
{
	if (IsInCountMode())
	{
		IncrementLineCount(4, shapeColor.m_lineColor);
		IncrementTriangleCount(8, shapeColor.m_triangleColor);
	}
	else
	{
		FillSquareTubeSegment(P, Q, halfWidth, shapeColor);
	}
}

KY_INLINE void IVisualGeometryBuilder::PushTriangleTubeSegment(const Vec3f& P, const Vec3f& Q, KyFloat32 halfWidth, const VisualShapeColor& shapeColor)
{
	if (IsInCountMode())
	{
		IncrementLineCount(3, shapeColor.m_lineColor);
		IncrementTriangleCount(6, shapeColor.m_triangleColor);
	}
	else
	{
		FillTriangleTubeSegment(P, Q, halfWidth, shapeColor);
	}
}

KY_INLINE void IVisualGeometryBuilder::PushArrow(const Vec3f& P, const Vec3f& Q, KyFloat32 bodyHalfWidth, const VisualShapeColor& shapeColor,
	KyFloat32 headWidthRatio, KyFloat32 headLengthRatio)
{
	if (IsInCountMode())
	{
		IncrementTriangleCount(18, shapeColor.m_triangleColor);
		IncrementLineCount(20, shapeColor.m_lineColor);
	}
	else
	{
		FillArrow(P, Q, bodyHalfWidth, shapeColor, headWidthRatio, headLengthRatio);
	}
}

KY_INLINE void IVisualGeometryBuilder::PushParabola(const Kaim::Vec3f& P, const Kaim::Vec3f& Q, KyFloat32 heightBias, KyUInt32 subdivisionCount, const VisualShapeColor& shapeColor)
{
	if (IsInCountMode())
	{
		IncrementLineCount(subdivisionCount, shapeColor.m_lineColor);
	}
	else
	{
		FillParabola(P, Q, heightBias, subdivisionCount, shapeColor);
	}
}

KY_INLINE void IVisualGeometryBuilder::PushStadium(const Vec3f& P, const Vec3f& Q, KyFloat32 radius, KyUInt32 subdivisionCount, const VisualShapeColor& shapeColor)
{
	if (IsInCountMode())
	{
		IncrementLineCount(2 + subdivisionCount*2, shapeColor.m_lineColor);         // 2 disks + 2 lines
		IncrementTriangleCount(2 + subdivisionCount*2, shapeColor.m_triangleColor); // 2 disks + 2 triangles
	}
	else
	{
		FillStadium(P, Q, radius, subdivisionCount, shapeColor);
	}
}

KY_INLINE void IVisualGeometryBuilder::PushFlag(const Vec3f& P, KyFloat32 height, KyFloat32 radius, const VisualShapeColor& shapeColor)
{
	if (IsInCountMode())
	{
		IncrementLineCount(5, shapeColor.m_lineColor);
	}
	else
	{
		FillFlag(P, height, radius, shapeColor);
	}
}


KY_INLINE void IVisualGeometryBuilder::PushCorona(const Vec3f& P, KyFloat32 innerRadius, KyFloat32 outerRadius, KyUInt32 subdivisionCount, const VisualShapeColor& shapeColor)
{
	if (IsInCountMode())
	{
		IncrementLineCount(2 * subdivisionCount, shapeColor.m_lineColor);
		IncrementTriangleCount(2 * subdivisionCount, shapeColor.m_triangleColor);
	}
	else
	{
		FillCorona(P, innerRadius, outerRadius, subdivisionCount, shapeColor);
	}
}

KY_INLINE void IVisualGeometryBuilder::PushBezierArrow(const Vec3f& P, const Vec3f& Q, KyFloat32 bodyHalfWidth, const VisualShapeColor& shapeColor, KyFloat32 headWidthRatio, KyUInt32 directionnality)
{
	if (IsInCountMode())
	{
		KyUInt32 graphCount = 16;
		KyUInt32 triangleCount = (graphCount - 2 * (KyUInt32)directionnality) * 18 + 6*(KyUInt32)directionnality;
		IncrementTriangleCount(triangleCount, shapeColor.m_triangleColor);

		KyUInt32 lineCount = (graphCount - 2 * (KyUInt32)directionnality) * 9 + 8*(KyUInt32)directionnality;
		IncrementLineCount(lineCount, shapeColor.m_lineColor);
	}
	else
	{
		FillBezierArrow(P, Q, bodyHalfWidth, shapeColor, headWidthRatio, (KyUInt32)directionnality);
	}
}

KY_INLINE void IVisualGeometryBuilder::PushChristmasTree(const Vec3f& P, const Vec3f& Q, KyFloat32 bodyHalfWidth, const VisualShapeColor& shapeColor)
{
	if (IsInCountMode())
	{
		IncrementTriangleCount(2, shapeColor.m_triangleColor);
		IncrementLineCount(2*3, shapeColor.m_lineColor);
	}
	else
	{
		FillChristmasTree(P, Q, bodyHalfWidth, shapeColor);
	}
}

KY_INLINE void IVisualGeometryBuilder::PushSilex(const Vec3f& P, const Vec3f& Q, KyFloat32 bodyHalfWidth, const VisualShapeColor& shapeColor)
{
	if (IsInCountMode())
	{
		IncrementTriangleCount(4, shapeColor.m_triangleColor);
		IncrementLineCount(4*3, shapeColor.m_lineColor);
	}
	else
	{
		FillSilex(P, Q, bodyHalfWidth, shapeColor);
	}
}

KY_INLINE void IVisualGeometryBuilder::PushDoubleSilex(const Vec3f& P, const Vec3f& Q, KyFloat32 bodyHalfWidth, const VisualShapeColor& shapeColor)
{
	if (IsInCountMode())
	{
		IncrementTriangleCount(6, shapeColor.m_triangleColor);
		IncrementLineCount(6*3, shapeColor.m_lineColor);
	}
	else
	{
		FillDoubleSilex(P, Q, bodyHalfWidth, shapeColor);
	}
}

KY_INLINE void IVisualGeometryBuilder::PushCrossedRectangles(const Vec3f& P, const Vec3f& Q, KyFloat32 bodyHalfWidth, const VisualShapeColor& shapeColor)
{
	if (IsInCountMode())
	{
		IncrementTriangleCount(4, shapeColor.m_triangleColor);
		IncrementLineCount(8, shapeColor.m_lineColor);
	}
	else
	{
		FillCrossedRectangles(P, Q, bodyHalfWidth, shapeColor);
	}
}

KY_INLINE void IVisualGeometryBuilder::PushColumn(const Vec3f& P, KyFloat32 halfWidth, KyFloat32 height, const VisualShapeColor& shapeColor)
{
	if (IsInCountMode())
	{
		IncrementTriangleCount(12, shapeColor.m_triangleColor);
		IncrementLineCount(12, shapeColor.m_lineColor);
	}
	else
	{
		FillColumn(P, halfWidth, height, shapeColor);
	}
}

KY_INLINE void IVisualGeometryBuilder::PushTetrahedron(const Vec3f& P, KyFloat32 halfWidth, const VisualShapeColor& shapeColor)
{
	if (IsInCountMode())
	{
		IncrementTriangleCount(4, shapeColor.m_triangleColor);
		IncrementLineCount(4*3, shapeColor.m_lineColor);
	}
	else
	{
		FillTetrahedron(P, halfWidth, shapeColor);
	}
}

KY_INLINE void IVisualGeometryBuilder::PushWall(const Vec3f& P, const Vec3f& Q, KyFloat32 upHeight, KyFloat32 downHeight, KyFloat32 halfWidth, const VisualShapeColor& shapeColor)
{
	if (IsInCountMode())
	{
		IncrementLineCount(12, shapeColor.m_lineColor);
		IncrementTriangleCount(12, shapeColor.m_triangleColor);
	}
	else
	{
		FillWall(P, Q, upHeight, downHeight, halfWidth, shapeColor);
	}
}

KY_INLINE void IVisualGeometryBuilder::PushLadder(const Transform& transform, const Box3f& entrances, KyFloat32 ladderWidth, KyFloat32 rungsThickness, KyFloat32 rungsGapSize, const VisualShapeColor& shapeColor)
{
	if (IsInCountMode())
	{
		KyFloat32 height = fabsf(entrances.m_max.z - entrances.m_min.z);
		KyUInt32 rungsCount = (KyUInt32) (height/rungsGapSize);
		IncrementLineCount(24+ rungsCount*4, shapeColor.m_lineColor);
		IncrementTriangleCount(24+ rungsCount*8, shapeColor.m_triangleColor);
	}
	else
	{
		FillLadder(transform, entrances, ladderWidth, rungsThickness, rungsGapSize, shapeColor);
	}
}

KY_INLINE void IVisualGeometryBuilder::PushText(const Vec3f& pos, const VisualColor& shapeColor, const char* text, KyUInt32 textLength)
{
	if (IsInCountMode())
		IncrementTextCount(1);
	else
		FillText(pos, shapeColor, text, textLength);
}

#ifndef KY_BUILD_SHIPPING
KY_INLINE void IVisualGeometryBuilder::DebugIncrementTextCount(KyUInt32 increment)
{
	m_debugSetupConfig.m_textCount += increment;
}
KY_INLINE void IVisualGeometryBuilder::DebugIncrementTriangleCount(KyUInt32 increment, const VisualColor& color)
{
	KY_LOG_ERROR_IF(color == VisualShapeColor::Inactive(), ("we should not push invisible triangles"));
	if (color.m_a == 255)
		m_debugSetupConfig.m_opaqueTriangleCount += increment;
	else
		m_debugSetupConfig.m_transparentTriangleCount += increment;
}

KY_INLINE void IVisualGeometryBuilder::DebugIncrementLineCount(KyUInt32 increment, const VisualColor& lineColor)
{
	KY_LOG_ERROR_IF(lineColor == VisualShapeColor::Inactive(), ("we should not push invisible lines"));
	m_debugSetupConfig.m_lineCount += increment;
}
#else
KY_INLINE void IVisualGeometryBuilder::DebugIncrementTextCount(KyUInt32 /*increment*/) {}
KY_INLINE void IVisualGeometryBuilder::DebugIncrementTriangleCount(KyUInt32 /*increment*/, const VisualColor& /*color*/) {}
KY_INLINE void IVisualGeometryBuilder::DebugIncrementLineCount(KyUInt32 /*increment*/, const VisualColor& /*lineColor*/) {}
#endif
}
