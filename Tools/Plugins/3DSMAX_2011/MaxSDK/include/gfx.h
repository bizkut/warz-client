//**************************************************************************/
// Copyright (c) 1998-2006 Autodesk, Inc.
// All rights reserved.
// 
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information written by Autodesk, Inc., and are
// protected by Federal copyright law. They may not be disclosed to third
// parties or copied or duplicated in any form, in whole or in part, without
// the prior written consent of Autodesk, Inc.
//**************************************************************************/
// FILE:        gfx.h
// DESCRIPTION: main graphics system include file.
// AUTHOR:      Don Brittain
// HISTORY:     
//**************************************************************************/
#pragma once

#include "maxheap.h"
#include "geomlib.h"
#include "export.h"
#include "tab.h"
#include "mtl.h"
#include "BaseInterface.h"
#include "HWMesh.h"
#include "MeshFaceFlagConstants.h"
#include "GraphicsConstants.h"
#include "GraphicsTypes.h"
#include "GraphicsViewPortColorConstants.h"
#include "maxtypes.h"
#include "TabTypes.h"
#include "LightTypeEnums.h"
#include "Strip.h"
#include "GraphicsWindow.h"

class VertexBuffer: public MaxHeapOperators {
public:
	Point3	position;
	DWORD		color;
};

class LineBuffer: public MaxHeapOperators {
public:
	Point3	position;
	DWORD		color;
};

class GWFace: public MaxHeapOperators {
public:
	DWORD	v[3];	// indexed references to the vertex array
	DWORD	flags;	// see face flags description above
};

// General 3D light structure
/*! \sa  <a href="ms-its:3dsmaxsdk.chm::/start_data_types.html">Data
Types</a>.\n\n
\par Description:
This class describes the lights used in the interactive renderer. All methods
of this class are implemented by the system.  */
class Light : public BaseInterfaceServer {
public:
    /*! \remarks Class constructor. Sets the default values of the light.\n\n
    <b>type = OMNI_LGT;</b>\n\n
    <b>attenType = NO_ATTEN;</b>\n\n
    <b>atten = (float)0.0;</b>\n\n
    <b>intensity = (float)1.0;</b>\n\n
    <b>angle = (float)30.0;</b>\n\n
    <b>color[0] = (float)1.0;</b>\n\n
    <b>color[1] = (float)1.0;</b>\n\n
    <b>color[2] = (float)1.0;</b>
    \par Data Members:
    <b>LightType type;</b>\n\n
    One of the following values:\n\n
    <b>OMNI_LGT</b>\n\n
    Omni-directional.\n\n
    <b>SPOT_LGT</b>\n\n
    Spot light.\n\n
    <b>DIRECT_LGT</b>\n\n
    Directional light.\n\n
    <b>AMBIENT_LGT</b>\n\n
    Ambient light - global illumination.\n\n
    <b>Point3 color;</b>\n\n
    The color of the light. Individual values are from 0.0 to 1.0 with 1.0 as
    white.\n\n
    <b>Attenuation attenType;</b>\n\n
    Attenuation is not currently implemented. A developer should pass
    <b>NO_ATTEN</b>.\n\n
    <b>float atten;</b>\n\n
    Note: Attenuation is not currently implemented.\n\n
    Light attenuation factor.\n\n
    <b>float intensity;</b>\n\n
    Light multiplier factor.\n\n
    <b>float angle;</b>\n\n
    Angle of cone for spot and cone lights in degrees.\n\n
    <b>int shape;</b>\n\n
    One of the following values:\n\n
    <b>GW_SHAPE_RECT</b> - Rectangular spotlights.\n\n
    <b>GW_SHAPE_CIRCULAR</b> - Circular spotlights.\n\n
    <b>float aspect;</b>\n\n
    The aspect ratio of the light.\n\n
    <b>int overshoot;</b>\n\n
    Nonzero indicates the light supports overshoot; otherwise 0.\n\n
    <b>BOOL affectDiffuse;</b>\n\n
    This data member is available in release 2.0 and later only.\n\n
    This defaults to TRUE, but if the user set it to FALSE in the light
    modifier panel, then the\n\n
    light is not supposed to illuminate the diffuse component of an object's
    material.\n\n
    <b>BOOL affectSpecular;</b>\n\n
    This data member is available in release 2.0 and later only.\n\n
    This defaults to TRUE, but if the user set it to FALSE in the light
    modifier panel, then the\n\n
    light is not supposed to illuminate the specular component of an object's
    material. */
    DllExport Light();
    LightType		type;
    Point3			color;
    int				attenType;
    float			attenStart;
	float			attenEnd;
    float			intensity;
    float			hotSpotAngle;
	float			fallOffAngle;
	int				shape;
	float			aspect;
	int				overshoot;
	BOOL 			affectDiffuse;
	BOOL 			affectSpecular;
};

// General camera structure
class Camera : public BaseInterfaceServer {
public:
	DllExport Camera();
	void			setPersp(float f, float asp)
						{ type = PERSP_CAM; persp.fov = f; 
						  persp.aspect = asp; makeMatrix(); }
	void			setOrtho(float l, float t, float r, float b)
						{ type = ORTHO_CAM; ortho.left = l; ortho.top = t; 
						  ortho.right = r; ortho.bottom = b; makeMatrix(); }
	void			setClip(float h, float y) 
						{ hither = h; yon = y; makeMatrix(); }
	CameraType		getType(void)	{ return type; }
	float			getHither(void) { return hither; }
	float			getYon(void)	{ return yon; }
	DllExport void	reset();
	DllExport void	getProj(float mat[4][4]);
private:
	DllExport void	makeMatrix();
	float			proj[4][4];
	CameraType		type;
	union {
	    struct : public MaxHeapOperators {
            float	fov;
            float	aspect;
		} persp;
		struct : public MaxHeapOperators {
		    float	left;
		    float	right;
		    float	bottom;
		    float	top;
		} ortho;
	};
	float			hither;
	float			yon;
};



DllExport void	setAutoCross(int onOff);
DllExport int	getAutoCross();
DllExport void	setAutoCrossDir(int dir);
DllExport int	getAutoCrossDir();

struct CIRCLE: public MaxHeapOperators
{
    LONG  x;
    LONG  y;
	LONG  r;
};

/*! \sa  <a href="ms-its:3dsmaxsdk.chm::/start_data_types.html">Data
Types</a>.\n\n
\par Description:
This class describes the properties of a region used for built-in hit testing
of items in the interactive renderer.
\par Data Members:
<b>int type;</b>\n\n
The region type. One of the following values:\n\n
<b>POINT_RGN</b>\n\n
A single point.\n\n
<b>RECT_RGN</b>\n\n
A rectangular region.\n\n
<b>CIRCLE_RGN</b>\n\n
A circular region.\n\n
<b>FENCE_RGN</b>\n\n
An arbitrary multi-point polygon region.\n\n
<b>int crossing;</b>\n\n
If nonzero, elements that are contained within <b>or cross</b> the region
boundary are hit. If zero, only those elements <b>entirely within</b> the
boundary are hit. This is not used for point hit testing.\n\n
<b>int epsilon;</b>\n\n
Specifies the distance in pixels outside the pick point within which elements
may be and still be hit. This is not used for rect or circle testing, is sometimes used with fence hit
testing, where it doubles the size of the region, especially when selecting subobject edges or vertices
and is always used for point hit testing.\n\n
<b>union {</b>\n\n
<b>POINT pt;</b>\n\n
<b>RECT rect;</b>\n\n
<b>CIRCLE circle;</b>\n\n
<b>POINT *pts;</b>\n\n
<b>};</b>\n\n
The storage for the region. */
class HitRegion: public MaxHeapOperators {
	DWORD size;
public:
	int				type;
	int				dir;		// region direction
	int				crossing;	// not used for point
	int				epsilon;	// not used for rect or circle
	union {
		POINT		pt;
		RECT		rect;
		CIRCLE		circle;
		POINT *		pts;
	};
	HitRegion()		{ dir = RGN_DIR_UNDEF; size = sizeof(HitRegion);}
};
#ifndef ABS
inline int ABS(const int x) { return (x > 0) ? x : -x; }
#endif


// graphics window setup structure
/*! \sa  Class GraphicsWindow.\n\n
\par Description:
This class is the graphics window setup structure. An instance of this class is
passed to the function <b>createGW()</b> to create a new graphics window.\n\n
Note: This is no longer available for use by plug-ins in 3ds Max 2.0 and later.
*/
class GWinSetup: public MaxHeapOperators {
public:
    DllExport GWinSetup();
    MCHAR		caption[GW_MAX_CAPTION_LEN];
	MCHAR		renderer[GW_MAX_FILE_LEN];
	MCHAR		device[GW_MAX_FILE_LEN];
	DWORD		winStyle;
	POINT		size;
	POINT		place;
	INT_PTR		id;
	int			type;
	bool		quietMode;
};


// for Windows int coords with origin at upper-left
inline int wIsFacingBack(const IPoint3 &v0, const IPoint3 &v1, const IPoint3 &v2, int flip=0 )
{
	int s = ( (v0[0]-v1[0])*(v2[1]-v1[1]) - (v2[0]-v1[0])*(v0[1]-v1[1]) ) < 0;
	return flip ? !s : s;
}

// for HEIDI int coords with origin at lower-left
inline int hIsFacingBack(const IPoint3 &v0, const IPoint3 &v1, const IPoint3 &v2, int flip=0 )
{
	int s = ( (v0[0]-v1[0])*(v2[1]-v1[1]) - (v2[0]-v1[0])*(v0[1]-v1[1]) );
	return flip ? s < 0 : s > 0;
}

//! \brief Returns the facing of a given triangle relative to the screen.
/*! \remarks Returns whether a given triangle is front-facing, 
 side-facing, or back-facing relative to the screen. The triangle is passed as three points in screen space. 
 This function is used for "w" format device coordinates.
\param v0 The 1st triangle vertex
\param v1 The 2nd triangle vertex
\param v2 The 3rd triangle vertex 
\param flip If true, flip the triangle (so backfacing would return frontfacing) */
inline FacingType wFacingType(const IPoint3 &v0, const IPoint3 &v1, const IPoint3 &v2, int flip=0 )
{
	int s = ( (v0[0]-v1[0])*(v2[1]-v1[1]) - (v2[0]-v1[0])*(v0[1]-v1[1]) );
	return (s == 0) ? kSideFacing : ((flip ? s > 0 : s < 0) ? kBackFacing : kFrontFacing);
}

//! \brief Returns the facing of a given triangle relative to the screen.
/*! \remarks The methods wFacingType() and hFacingType() will return whether a given triangle is front-facing, 
side-facing, or back-facing relative to the screen. The triangle is passed as three points in screen space. 
This function is used for "h" format device coordinates. 
\param v0 The 1st triangle vertex
\param v1 The 2nd triangle vertex
\param v2 The 3rd triangle vertex
\param flip If true, flip the triangle (so backfacing would return frontfacing) */
inline FacingType hFacingType(const IPoint3 &v0, const IPoint3 &v1, const IPoint3 &v2, int flip=0 )
{
	int s = ( (v0[0]-v1[0])*(v2[1]-v1[1]) - (v2[0]-v1[0])*(v0[1]-v1[1]) );
	return (s == 0) ? kSideFacing : ((flip ? s < 0 : s > 0) ? kBackFacing : kFrontFacing);
}

/*! \remarks This function is available in release 2.0 and later only.\n\n
This function is not supported for use in the SDK. */
DllExport HINSTANCE GetGraphicsLibHandle(MCHAR *driverLibName);
/*! \remarks This function is available in release 2.0 and later only.\n\n
This function is not supported for use in the SDK. */
DllExport BOOL GraphicsSystemIsAvailable(HINSTANCE drv);
/*! \remarks This function is available in release 2.0 and later only.\n\n
This function is not supported for use in the SDK. */
DllExport BOOL GraphicsSystemCanConfigure(HINSTANCE drv);
/*! \remarks This function is available in release 2.0 and later only.\n\n
This function is not supported for use in the SDK. */
DllExport BOOL GraphicsSystemConfigure(HWND hWnd, HINSTANCE drv);
/*! \remarks This function is available in release 2.0 and later only.\n\n
This function is not supported for use in the SDK. */
DllExport void FreeGraphicsLibHandle(HINSTANCE drv);

/*! \remarks This function is used internally to create a new graphics window.
Use of this method is not supported for plug-ins. */
DllExport GraphicsWindow* createGW(HWND hWnd, GWinSetup &gws);

/*! \remarks Returns a bounding rectangle that encloses the entire hit region.
For example if the hit regions was a fence region, this method would return the
smallest rectangle that included the entire set of fence region points.
\par Parameters:
<b>HitRegion *hr</b>\n\n
The hit region to check.\n\n
<b>RECT *rect</b>\n\n
The returned bounding rectangle. */
DllExport void getRegionRect(HitRegion *hr, RECT *rect);
/*! \remarks Returns TRUE if the specified point is inside the region
<b>hr</b>; otherwise FALSE. */
DllExport BOOL pointInRegion(int x, int y, HitRegion *hr);

/*! \remarks Returns the signed distance from <b>x,y</b> to the line defined
by <b>p1-\>p2</b>. */
DllExport int distToLine(int x, int y, int *p1, int *p2);
DllExport int zDepthToLine(int x, int y, int *p1, int *p2);

/*! \remarks Returns nonzero if the line defined by <b>p1-\>p2</b> crosses
into the RECT and 0 otherwise. */
DllExport int lineCrossesRect(RECT *rc, int *p1, int *p2);
DllExport int segCrossesRect(RECT *rc, int *p1, int *p2);
/*! \remarks Returns nonzero if the line defined by <b>p1-\>p2</b> crosses the
circle center at (<b>cx, cy</b>) with a radius of <b>r</b> 0 otherwise. */
DllExport int segCrossesCircle(int cx, int cy, int r, int *p1, int *p2);
/*! \remarks Returns TRUE if the point passed is inside the specified
triangle.
\par Parameters:
<b>IPoint3 \&p0</b>\n\n
The first point of the triangle.\n\n
<b>IPoint3 \&p1</b>\n\n
The second point of the triangle.\n\n
<b>IPoint3 \&p2</b>\n\n
The third point of the triangle.\n\n
<b>IPoint3 \&q</b>\n\n
The point to check.
\return  Returns TRUE if the point passed is inside the specified triangle;
otherwise FALSE. */
DllExport BOOL insideTriangle(IPoint3 &p0, IPoint3 &p1, IPoint3 &p2, IPoint3 &q);
/*! \remarks Returns the z value of where the projected screen point <b>q</b>
would intersect the triangle defined by (<b>p0, p1, p2</b>).
\par Parameters:
<b>IPoint3 \&p0</b>\n\n
The first point of the triangle.\n\n
<b>IPoint3 \&p1</b>\n\n
The second point of the triangle.\n\n
<b>IPoint3 \&p2</b>\n\n
The third point of the triangle.\n\n
<b>IPoint3 \&q</b>\n\n
The screen point to check. */
DllExport int getZfromTriangle(IPoint3 &p0, IPoint3 &p1, IPoint3 &p2, IPoint3 &q);

DllExport int getClosestPowerOf2(int num);

/**! \name Access to customizable colors used to draw viewport, scene and user interface elements (not elements of different color schemes available in 3ds Max). 
* Clients of these methods should consider registering for NOTIFY_COLOR_CHANGE notification in order
* to ensure that they use the current custom colors.
*/
//@{
/** ! \brief Retrieves the current value of the specified custom color
* \param which The identifier of the custom color to retrive. See \ref viewportDrawingColors
* \return The current value of the color, as an RGB value
*/
DllExport Point3 GetUIColor(int which);
/** ! \brief Sets the current value of the specified custom color
* \param which The identifier of the custom color to retrive. See \ref viewportDrawingColors
* \param clr Pointer to the new color value object
*/
DllExport void SetUIColor(int which, Point3 *clr);
/** ! \brief Retrieves the default value of the specified custom color
* \param which The identifier of the custom color to retrive. See \ref viewportDrawingColors
* \return The default value of the color, as an RGB value
*/
DllExport Point3 GetDefaultUIColor(int which);
//@}

#define GetSelColor()		GetUIColor(COLOR_SELECTION)
/*! \remarks Returns the color used for sub-object selection. */
#define GetSubSelColor()	GetUIColor(COLOR_SUBSELECTION)
#define GetFreezeColor()	GetUIColor(COLOR_FREEZE)