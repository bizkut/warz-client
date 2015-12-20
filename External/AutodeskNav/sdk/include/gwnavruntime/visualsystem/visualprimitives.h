/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// primary contact: GUAL - secondary contact: MAMU
#ifndef Navigation_VisualTriangle_H
#define Navigation_VisualTriangle_H


#include "gwnavruntime/math/vec3f.h"
#include "gwnavruntime/visualsystem/visualcolor.h"


namespace Kaim
{

/// Represents a single triangle that can be rendered by a class that derives from IVisualGeometry.
class VisualTriangle
{
public:
	VisualTriangle() {}
	VisualTriangle(const Vec3f& defA, const Vec3f& defB, const Vec3f& defC, const Vec3f& normal, const VisualColor& color)
	: A(defA), B(defB), C(defC), m_normal(normal), m_color(color) {}

	Vec3f A; ///< The first point in the triangle. 
	Vec3f B; ///< The second point in the triangle. 
	Vec3f C; ///< The third point in the triangle. 
	Vec3f m_normal; ///< The normal of the front face of the triangle. 
	VisualColor m_color; ///< The color that should be used to render the triangle. 
};

class VisualLine
{
public:
	VisualLine() : m_width(1.f) {}
	VisualLine(const Vec3f& defA, const Vec3f& defB,const VisualColor& color, KyFloat32 width = 1.f)
		: A(defA), B(defB), m_color(color), m_width(width) {}

	Vec3f A; ///< The first point in the line. 
	Vec3f B; ///< The second point in the line. 
	VisualColor m_color; ///< The color that should be used to render the triangle. 
	KyFloat32 m_width; //  width of rasterized lines, default is 1.f
};

class VisualText
{
public:
	VisualText() : m_text(KY_NULL), m_length(0) {}
	VisualText(const char* text, KyUInt32 length, const Vec3f& defA, const VisualColor& color)
		: m_text(text), m_pos(defA), m_length(length), m_color(color) {}

	const char* m_text;
	Vec3f m_pos;
	KyUInt32 m_length;
	VisualColor m_color; ///< The color that should be used to render the triangle. 
};

}


#endif
