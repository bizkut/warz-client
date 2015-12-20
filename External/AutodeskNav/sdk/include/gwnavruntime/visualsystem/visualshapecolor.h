/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// primary contact: GUAL - secondary contact: MAMU
#ifndef Navigation_VisualShapeColor_H
#define Navigation_VisualShapeColor_H

#include "gwnavruntime/visualsystem/visualcolor.h"

namespace Kaim
{

enum VisualShapePrimitive
{
	VisualShapePrimitive_Triangle,
	VisualShapePrimitive_Line,
};

// Define the aspect of a shape: its color and the primitives used to render a shape
class VisualShapeColor
{
public:
	KY_INLINE static VisualColor Inactive() { return VisualColor::Inactive(); }

	VisualShapeColor();
	VisualShapeColor(const VisualColor& triangleColor, const VisualColor& lineColor);
	VisualShapeColor(const VisualColor& color, const VisualShapePrimitive& primitive);

	~VisualShapeColor();

	void SetOnlyTriangleColor(const VisualColor& triangleColor);
	void SetOnlyLineColor(const VisualColor& lineColor);

	// DisplayShapeColor must have at least one valid color for triangles or lines
	bool IsValid() const;

public:
	VisualColor m_triangleColor; // color for triangles, assign Inactive() is you don't want triangles
	VisualColor m_lineColor;  // color for lines, assign Inactive() is you don't want triangles
};
inline void SwapEndianness(Endianness::Target e, VisualShapeColor& self)
{
	SwapEndianness(e, self.m_triangleColor);
	SwapEndianness(e, self.m_lineColor);
}

KY_INLINE VisualShapeColor::VisualShapeColor()
	: m_triangleColor(Inactive())
	, m_lineColor(Inactive())
{}

KY_INLINE VisualShapeColor::VisualShapeColor(const VisualColor& triangleColor, const VisualColor& lineColor)
	: m_triangleColor(triangleColor)
	, m_lineColor(lineColor)
{}

KY_INLINE VisualShapeColor::VisualShapeColor(const VisualColor& color, const VisualShapePrimitive& primitive)
	: m_triangleColor(Inactive())
	, m_lineColor(Inactive())
{
	if (primitive == VisualShapePrimitive_Triangle)
		m_triangleColor = color;
	else
		m_lineColor = color;
}

KY_INLINE VisualShapeColor::~VisualShapeColor() {}

KY_INLINE void VisualShapeColor::SetOnlyTriangleColor(const VisualColor& triangleColor)
{
	m_triangleColor = triangleColor;
	m_lineColor = Inactive();
}

KY_INLINE void VisualShapeColor::SetOnlyLineColor(const VisualColor& lineColor)
{
	m_lineColor = lineColor;
	m_triangleColor = Inactive();
}

KY_INLINE bool VisualShapeColor::IsValid() const { return m_triangleColor != Inactive() || m_lineColor != Inactive(); }

}


#endif //Navigation_VisualShapeColor_H
