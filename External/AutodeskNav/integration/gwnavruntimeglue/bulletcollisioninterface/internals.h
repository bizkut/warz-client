/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/

#ifndef KyRuntimeGlue_Internals_H
#define KyRuntimeGlue_Internals_H

KY_INLINE Kaim::Vec3f btVectorToVec3f(const btVector3& btVec)
{
	return Kaim::Vec3f(btVec[0], btVec[1], btVec[2]);
}

KY_INLINE btVector3 Vec3fTobtVector(const Kaim::Vec3f& vec3f)
{
	return btVector3(vec3f[0], vec3f[1], vec3f[2]);
}

Kaim::VisualColor btColorToVisualColor(const btVector3& btColor)
{
	return Kaim::VisualColor((KyUInt8)btColor[0], (KyUInt8)btColor[1], (KyUInt8)btColor[2]);
}

class CollisionInterfaceDebugDraw : public btIDebugDraw
{
public:

	CollisionInterfaceDebugDraw() : m_displayList(KY_NULL), m_debugMode(0) {}
	virtual ~CollisionInterfaceDebugDraw() {}

	virtual void	drawLine(const btVector3& from, const btVector3& to, const btVector3& color)
	{
		const Kaim::Vec3f a = btVectorToVec3f(from);
		const Kaim::Vec3f b = btVectorToVec3f(to);
		const Kaim::VisualColor c = btColorToVisualColor(color);
		m_displayList->PushLine(a, b, c);
	}

	virtual void	drawLine(const btVector3& from, const btVector3& to, const btVector3& fromColor, const btVector3& /*toColor*/)
	{
		drawLine(from, to, fromColor);
	}

	virtual void	drawTriangle(const btVector3& a, const btVector3& b, const btVector3& c, const btVector3& color, btScalar alpha)
	{
		const Kaim::VisualColor visCol = btColorToVisualColor(color);
		Kaim::VisualShapeColor shapeCol;
		shapeCol.SetOnlyTriangleColor(visCol);
		m_displayList->PushTriangle(btVectorToVec3f(a), btVectorToVec3f(b), btVectorToVec3f(c), shapeCol);
	}

	virtual	void	drawTriangle(const btVector3& v0,const btVector3& v1,const btVector3& v2,const btVector3& /*n0*/,const btVector3& /*n1*/,const btVector3& /*n2*/,const btVector3& color, btScalar alpha)
	{
		drawTriangle(v0,v1,v2,color,alpha);
	}

	virtual void	drawContactPoint(const btVector3& PointOnB,const btVector3& normalOnB,btScalar distance,int lifeTime,const btVector3& color) {}

	virtual void	reportErrorWarning(const char* warningString) {}

	virtual void	draw3dText(const btVector3& location,const char* textString)
	{
		const Kaim::Vec3f loc = btVectorToVec3f(location);
		m_displayList->PushText(loc, Kaim::VisualColor::Coral, textString);
	}

	virtual void	setDebugMode(int debugMode)
	{
		m_debugMode = debugMode;
	}

	virtual int		getDebugMode() const
	{
		return btIDebugDraw::DBG_DrawWireframe;
	}

	void BeginFrame(Kaim::ScopedDisplayList* displayList)
	{
		m_displayList = displayList;
	}

	void EndFrame()
	{
		m_displayList = KY_NULL;
	}

private:
	Kaim::ScopedDisplayList* m_displayList;
	int m_debugMode;
};

#endif //KyRuntimeGlue_Internals_H
