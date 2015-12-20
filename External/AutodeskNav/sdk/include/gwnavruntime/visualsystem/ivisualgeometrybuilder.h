/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/




// primary contact: GUAL - secondary contact: MAMU
#ifndef Navigation_IVisualGeometryBuilder_H
#define Navigation_IVisualGeometryBuilder_H

#include "gwnavruntime/visualsystem/visualgeometrysetupconfig.h"
#include "gwnavruntime/visualsystem/visualshapecolor.h"
#include "gwnavruntime/basesystem/coordsystem.h"
#include "gwnavruntime/math/triangle3f.h"
#include "gwnavruntime/kernel/SF_RefCount.h"
#include "gwnavruntime/basesystem/logger.h"


namespace Kaim
{

class OrientedBox2d;
class Box3f;
class Transform;
class IVisualGeometry;
class VisualBoxVertices;

/// Enumerates the possible levels of detail that can be set for a VisualRepresentation. Not all classes of
/// VisualRepresentation support different levels of detail; whether or not this is supported and how it is
/// supported depends on the implementation of VisualRepresentation::BuildGeometry() in each derived class.
enum VisualGeometryDetailLevel
{
	KY_VISUAL_DETAIL_LOW  = 0, ///< Identifies a lower level of detail. 
	KY_VISUAL_DETAIL_HIGH = 1, ///< Identifies a higher level of detail. 
	KY_VISUAL_DETAIL_MAX  = 2  ///< Identifies a higher level of detail. 
};

class VisualGeometryBuildConfig
{
public:
	VisualGeometryBuildConfig() : m_altitudeOffset(0.f), m_detailLevel(KY_VISUAL_DETAIL_LOW) {}
	KyFloat32 m_altitudeOffset;
	VisualGeometryDetailLevel m_detailLevel;
};

class IVisualGeometryBuilder
{
	KY_DEFINE_NEW_DELETE_OPERATORS(MemStat_VisualSystem)
public:
	IVisualGeometryBuilder(IVisualGeometry* visualGeometry = KY_NULL);
	virtual ~IVisualGeometryBuilder();

	void SetConfig(const VisualGeometryBuildConfig& config);
	void SetVisualGeometry(IVisualGeometry* visualGeometry);

	bool IsInFillMode()  const;
	bool IsInCountMode() const;

	IVisualGeometry* GetVisualGeometry() const;
	VisualGeometryBuildConfig& GetBuildConfig();


	void BuildVisualGeometry();


protected :
	virtual void DoBuild() = 0;

	void BuildSubVisualGeometry(IVisualGeometryBuilder& subGeometryBuilder);

	// By default, normal generation is Disabled. Once it has been enabled, it cannot be disabled anymore
	// which means : In visualGeometry, either all the normals of triangles are computed, or none of them are computed
	void EnableNormalGeneration();


	//--------------- Shapes -------------

	/// Pushes a text.
	void PushTextVarg( const Vec3f& pos, const VisualColor& color, const char* textFmt, ...);
	void PushText(const Vec3f& pos, const VisualColor& color, const char* text, KyUInt32 textLength = 0);

	/// Pushes a line.
	void PushLine(const Vec3f& P, const Vec3f& Q, const VisualColor& color, KyFloat32 width = 1.f);

	/// Pushes a single triangle. 
	void PushTriangle(const Vec3f& A, const Vec3f& B, const Vec3f& C, const VisualShapeColor& color);

	/// Pushes a single triangle. 
	void PushTriangle(const Triangle3f& triangle, const VisualShapeColor& color);

	/// Pushes a cylinder whose base is centered on P. 
	void PushVerticalCylinder(const Vec3f& P, KyFloat32 radius, KyFloat32 height, KyUInt32 subdivisions, const VisualShapeColor& color);

	/// Pushes a segment. P and Q are the centers of the two opposite ends, and halfWidth is half the width of the segment.
	void PushSegment(const Vec3f& A, const Vec3f& B, KyFloat32 halfWidth, const VisualShapeColor& color);

	/// Pushes a flat Quad
	void PushQuad(const Vec3f& A, const Vec3f& B, const Vec3f& C, const Vec3f& D, const VisualShapeColor& color);

	/// Pushes a flat Quad
	void PushQuad(const Vec3f& A, const Vec3f& B, KyFloat32 radius, const VisualShapeColor& color);

	/// Pushes an axis-aligned box centered on P. Or, a cross made of lines.
	void PushPoint(const Vec3f& P, const VisualShapeColor& color) { PushPoint(P, 0.1f, color); }

	/// Pushes an axis-aligned box centered on P. Or, a cross made of lines.
	void PushPoint(const Vec3f& P, KyFloat32 radius, const VisualShapeColor& color);

	/// Push a pyramid composed of a base and four sides. P the is center of the base, Q is the peak. 
	void PushPyramid(const Vec3f& P, const Vec3f& Q, KyFloat32 halfWidth, const VisualShapeColor& color);

	/// Pushes a hollow tube with a square cross-section. P and Q are at the center of two opposite faces, and the width of the
	/// tube is equal to twicehalfWidth. 
	void PushSquareTubeSegment(const Vec3f& P, const Vec3f& Q, KyFloat32 halfWidth, const VisualShapeColor& color);

	/// Pushes an arrow from P to Q.
	/// \param P                 Starting point of the arrow.
	/// \param Q                 Ending point of the arrow.
	/// \param bodyHalfWidth     Half of the width of the body or trunk of the arrow.
	/// \param color             Color of the arrow.
	/// \param headWidthRatio    The ratio between the width of the head and the width of the body. In other words, the head or point
	///                           of the arrow will be this many times wider than the body or trunk of the arrow.
	///                           \defaultvalue   3.0f.
	/// \param headLengthRatio   The ratio between the length of the head and the length of the body. In other words, the head or point
	///                           of the arrow will be this many times longer than the body or trunk of the arrow.
	///                           \defaultvalue  0.33f.
	void PushArrow(const Vec3f& P, const Vec3f& Q, KyFloat32 bodyHalfWidth, const VisualShapeColor& color, KyFloat32 headWidthRatio = 3.0f, KyFloat32 headLengthRatio = 0.33f);

	/// Pushes a bezier arrow from P to Q.
	/// \param P                  Starting point of the arrow.
	/// \param Q                  Ending point of the arrow.
	/// \param bodyHalfWidth      Half of the width of the body or trunk of the arrow.
	/// \param color              Color of the arrow.
	/// \param headWidthRatio     The ratio between the width of the head and the width of the body. In other words, the head or point
	///                            of the arrow will be this many times wider than the body or trunk of the arrow.
	///                            \defaultvalue   3.0f
	/// \param directionnality    If 0, has no arrow. If 1, has an arrow at the end. If 2, has an arrow at start and end.
	///                            \defaultvalue   0 
	void PushBezierArrow(const Vec3f& P, const Vec3f& Q, KyFloat32 bodyHalfWidth, const VisualShapeColor& color, KyFloat32 headWidthRatio = 3.0f, KyUInt32 directionnality = 0);

	/// Pushes a Christmas tree: two orthogonal triangles with the same central axes. P is the center of the base of
	/// both triangles, and Q is at the top of both triangles. Both triangles extend a distance of bodyHalfWidth
	/// from the central axes.
	void PushChristmasTree(const Vec3f& P, const Vec3f& Q, KyFloat32 bodyHalfWidth, const VisualShapeColor& color);

	/// Pushes a silex from P to Q with the specified bodyHalfWidth. 
	void PushSilex(const Vec3f& P, const Vec3f& Q, KyFloat32 bodyHalfWidth, const VisualShapeColor& color);

	/// Pushes a double silex from P to Q with the specified bodyHalfWidth. Equivalent to a silex from P to the midpoint
	/// between P and Q, and a silex from Q to the midpoint between P and Q. 
	void PushDoubleSilex(const Vec3f& P, const Vec3f& Q, KyFloat32 bodyHalfWidth, const VisualShapeColor& color);

	/// Pushes two orthogonal rectangles with the same central axes. P is the center of one end of both rectangles, and Q 
	/// is at the center of the opposite end of both rectangles. Both rectangles extend a distance of bodyHalfWidth
	/// from the central axes. 
	void PushCrossedRectangles(const Vec3f& P, const Vec3f& Q, KyFloat32 bodyHalfWidth, const VisualShapeColor& color);

	/// Pushes an axis-aligned box whose base is centered on P. 
	void PushColumn(const Vec3f& P, KyFloat32 halfWidth, KyFloat32 height, const VisualShapeColor& color);

	void PushOrientedBox(const Transform& transform, const Box3f& extents, const VisualShapeColor& color);
	void PushOrientedBox2d(const OrientedBox2d& orientedBox2d, const VisualShapeColor& color);

	/// Pushes an axis-aligned tetrahedron centered on P and pointing down. 
	void PushTetrahedron(const Vec3f& P, KyFloat32 halfWidth, const VisualShapeColor& color);

	/// Pushes a 2D flat disk centered on P. 
	void PushDisk(const Vec3f& P, KyFloat32 radius, KyUInt32 subdivisions, const VisualShapeColor& color);

	/// Pushes a 2D flat corona centered on P. 
	void PushCorona(const Vec3f& P, KyFloat32 innerRadius, KyFloat32 outerRadius, KyUInt32 subdivisions, const VisualShapeColor& color);

	void PushStadium(const Vec3f& P, const Vec3f& Q, KyFloat32 radius, KyUInt32 subdivisions, const VisualShapeColor& color);

	/// Pushes an axis-aligned box. 
	void PushBox(const Box3f& box, const VisualShapeColor& color);

	/// Pushes a hollow tube with a triangular cross-section. P and Q are at the center of two opposite faces, and the width of the
	/// tube is equal to twicehalfWidth. 
	void PushTriangleTubeSegment(const Vec3f& P, const Vec3f& Q, KyFloat32 halfWidth, const VisualShapeColor& color);

	/// Pushes a three-dimensional oriented box.
	/// \param P             A point at one end of the box.
	/// \param Q             A point at the opposite end of the box.
	/// \param upHeight      The distance of the top of the box above P and Q.
	/// \param downHeight    The distance of the bottom of the box below P and Q.
	/// \param halfWidth     The distance of the sides of the box on either side of P and Q. 
	/// \param color         Color of the wall.
	void PushWall(const Vec3f& P, const Vec3f& Q, KyFloat32 upHeight, KyFloat32 downHeight, KyFloat32 halfWidth, const VisualShapeColor& color);

	void PushLadder(const Transform& transform, const Box3f& entrances, KyFloat32 ladderWidth, KyFloat32 rungsThickness, KyFloat32 rungsGapSize, const VisualShapeColor& shapeColor);

	void PushParabola(const Kaim::Vec3f& P, const Kaim::Vec3f& Q, KyFloat32 heightBias, KyUInt32 subdivisions, const VisualShapeColor& color);

	void PushFlag(const Vec3f& P, KyFloat32 height, KyFloat32 radius, const VisualShapeColor& color);

	// ...
private:
	void IncrementTextCount(KyUInt32 increment);
	void IncrementTriangleCount(KyUInt32 increment, const VisualColor& color);
	void IncrementLineCount(KyUInt32 increment, const VisualColor& lineColor);

	void FillText(const Vec3f& pos, const VisualColor& color, const char* text, KyUInt32 textLength);
	void FillLine(const Vec3f& P, const Vec3f& Q, const VisualColor& color, KyFloat32 width = 1.f);
	void FillTriangle(const Vec3f& A, const Vec3f& B, const Vec3f& C, const VisualShapeColor& color);
	void FillTriangle(const Triangle3f& triangle, const VisualShapeColor& color);
	void FillVerticalCylinder(const Vec3f& P, KyFloat32 radius, KyFloat32 height, KyUInt32 subdivisions,
		const VisualShapeColor& color);
	void FillSegment(const Vec3f& A, const Vec3f& B, KyFloat32 halfWidth, const VisualShapeColor& color);
	void FillQuad(const Vec3f& A, const Vec3f& B, KyFloat32 radius, const VisualShapeColor& color);
	void FillQuad(const Vec3f& A, const Vec3f& B, const Vec3f& C, const Vec3f& D, const VisualShapeColor& color);
	void FillPoint(const Vec3f& P, const VisualShapeColor& color) { PushPoint(P, 0.1f, color); }
	void FillPoint(const Vec3f& P, KyFloat32 radius, const VisualShapeColor& color);
	void FillPyramid(const Vec3f& P, const Vec3f& Q, KyFloat32 halfWidth, const VisualShapeColor& color);
	void FillSquareTubeSegment(const Vec3f& P, const Vec3f& Q, KyFloat32 halfWidth, const VisualShapeColor& color);
	void FillArrow(const Vec3f& P, const Vec3f& Q, KyFloat32 bodyHalfWidth, const VisualShapeColor& color,
		KyFloat32 headWidthRatio, KyFloat32 headLengthRatio);
	void FillBezierArrow(const Vec3f& P, const Vec3f& Q, KyFloat32 bodyHalfWidth, const VisualShapeColor& color,
		KyFloat32 headWidthRatio, KyUInt32 directionnality);
	void FillChristmasTree(const Vec3f& P, const Vec3f& Q, KyFloat32 bodyHalfWidth, const VisualShapeColor& color);
	void FillSilex(const Vec3f& P, const Vec3f& Q, KyFloat32 bodyHalfWidth, const VisualShapeColor& color);
	void FillDoubleSilex(const Vec3f& P, const Vec3f& Q, KyFloat32 bodyHalfWidth, const VisualShapeColor& color);
	void FillCrossedRectangles(const Vec3f& P, const Vec3f& Q, KyFloat32 bodyHalfWidth, const VisualShapeColor& color);
	void FillColumn(const Vec3f& P, KyFloat32 halfWidth, KyFloat32 height, const VisualShapeColor& color);
	void FillOrientedBox(const Transform& transform, const Box3f& extents, const VisualShapeColor& color);
	void FillOrientedBox2d(const OrientedBox2d& orientedBox2d, const VisualShapeColor& color);
	void FillTetrahedron(const Vec3f& P, KyFloat32 halfWidth, const VisualShapeColor& color);
	void FillDisk(const Vec3f& P, KyFloat32 radius, KyUInt32 subdivisions, const VisualShapeColor& color);
	void FillCorona(const Vec3f& P, KyFloat32 innerRadius, KyFloat32 outerRadius, KyUInt32 subdivisions, const VisualShapeColor& color);
	void FillStadium(const Vec3f& P, const Vec3f& Q, KyFloat32 radius, KyUInt32 subdivisions, const VisualShapeColor& color);
	void FillBox(const Box3f& box, const VisualShapeColor& color);
	void FillTriangleTubeSegment(const Vec3f& P, const Vec3f& Q, KyFloat32 halfWidth, const VisualShapeColor& color);
	void FillWall(const Vec3f& P, const Vec3f& Q, KyFloat32 upHeight, KyFloat32 downHeight, KyFloat32 halfWidth, const VisualShapeColor& color);
	void FillLadder(const Transform& transform, const Box3f& entrances, KyFloat32 ladderWidth, KyFloat32 rungsThickness,
		KyFloat32 rungsGapSize, const VisualShapeColor& shapeColor);
	void FillParabola(const Kaim::Vec3f& P, const Kaim::Vec3f& Q, KyFloat32 heightBias, KyUInt32 subdivisions, const VisualShapeColor& color);
	void FillFlag(const Vec3f& P, KyFloat32 height, KyFloat32 radius, const VisualShapeColor& color);

	void FillFromVisualBoxVertices(VisualBoxVertices& box, const VisualShapeColor& shapeColor);

	void SetCountMode();
	void BeginFillMode();
	void EndFillMode();

	void DebugIncrementTextCount(KyUInt32 increment);
	void DebugIncrementTriangleCount(KyUInt32 increment, const VisualColor& color);
	void DebugIncrementLineCount(KyUInt32 increment, const VisualColor& lineColor);
public:
	void FillTriangleWithFinalPos(const Vec3f& A, const Vec3f& B, const Vec3f& C, VisualColor shapeColor);

	static bool CalculateFrontUpRight(const Vec3f& P, const Vec3f& Q, Vec3f& front, Vec3f& up, Vec3f& right);

public:
	enum BuildMode { BuildMode_COUNT, BuildMode_FILL };
	BuildMode m_buildMode;

	VisualGeometryBuildConfig m_buildConfig;

	VisualGeometrySetupConfig m_setupConfig;
	Ptr<IVisualGeometry> m_visualGeometry;

	// debug
#ifndef KY_BUILD_SHIPPING
	VisualGeometrySetupConfig m_debugSetupConfig;
#endif
};



}

#include "gwnavruntime/visualsystem/ivisualgeometrybuilder.inl"

#endif //Navigation_IVisualGeometryBuilder_H
