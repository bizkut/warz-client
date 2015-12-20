//
// Copyright 2009 Autodesk, Inc.  All rights reserved. 
//
// This computer source code and related instructions and comments are the
// unpublished confidential and proprietary information of Autodesk, Inc. and
// are protected under applicable copyright and trade secret law.  They may
// not be disclosed to, copied or used by any third party without the prior
// written consent of Autodesk, Inc.
//
//
#pragma once
#include <WTypes.h>

/*! \defgroup patchDisplayFlags Patch Display Flags
For processWireFaces and general mesh class use */
//@{
#define DISP_VERTTICKS		(1<<0)	//!< Display vertices as tick marks
#define DISP_SELVERTS		(1<<10)	//!< Display selected vertices.
#define DISP_SELFACES		(1<<11)	//!< Display selected faces.
#define DISP_SELEDGES		(1<<12)	//!< Display selected edges.
#define DISP_SELPOLYS		(1<<13)	//!< Display selected polygons.
#define DISP_OBJSELECTED	(1<<8)	//!< Mimics COMP_OBJSELECTED in mesh.h
//@}

// driver types for getDriver() method
#define GW_DRV_RENDERER		0
#define GW_DRV_DEVICE		1

// for possible future implementation
#define GW_HEIDI			0
#define GW_OPENGL			1
#define GW_DIRECT3D			2
#define GW_HEIDI3D			3
#define GW_NULL			4
#define GW_CUSTOM			5

// Region types (for built-in hit-testing)
#define POINT_RGN	0x0001
#define	RECT_RGN	0x0002
#define CIRCLE_RGN	0x0004
#define FENCE_RGN	0x0008

// region directions (left or right)
#define RGN_DIR_UNDEF	-1
#define RGN_DIR_RIGHT	0	
#define RGN_DIR_LEFT	1

const double pi        = 3.141592653589793;
const double piOver180 = 3.141592653589793 / 180.0;

// right->left => crossing (AutoCAD compatible)
#define AC_DIR_RL_CROSS		0
// left->right => crossing (ACAD incompatible)
#define AC_DIR_LR_CROSS		1


// General definitions
#define WM_SHUTDOWN			(WM_USER+2001)
#define WM_INIT_COMPLETE	(WM_USER+2002)

#define GW_MAX_FILE_LEN		128
#define GW_MAX_CAPTION_LEN	128

#define GW_MAX_VERTS		32
#define GFX_MAX_STRIP		100
#define GFX_MAX_TEXTURES	8

// Rendering modes
#define GW_NO_ATTS				0x0000000
#define GW_WIREFRAME			0x0000001
#define GW_ILLUM				0x0000002
#define GW_FLAT					0x0000004
#define GW_SPECULAR				0x0000008
#define GW_TEXTURE				0x0000010
#define GW_Z_BUFFER				0x0000020
#define GW_PERSP_CORRECT		0x0000040
#define GW_POLY_EDGES			0x0000080
#define GW_BACKCULL				0x0000100
#define GW_TWO_SIDED			0x0000200
#define GW_COLOR_VERTS			0x0000400
#define GW_SHADE_CVERTS			0x0000800
#define GW_PICK					0x0001000
#define GW_BOX_MODE				0x0002000
#define GW_ALL_EDGES			0x0004000
#define GW_VERT_TICKS			0x0008000
#define GW_SHADE_SEL_FACES		0x0010000
#define GW_TRANSPARENCY			0x0020000
#define GW_TRANSPARENT_PASS		0x0040000
#define GW_EMISSIVE_VERTS		0x0080000
#define GW_ALL_OPAQUE			0x0100000
#define GW_EDGES_ONLY			0x0200000
#define GW_CONSTANT				0x0400000
#define GW_HIDDENLINE			0x0800000  //this is the same as constant but the shade color will the color of the background
#define GW_BLENDING				0x1000000
#define GW_DEPTHWRITE_DISABLE	0x2000000	//disable writing into the depth buffer


#define GW_LIGHTING			(GW_ILLUM | GW_SPECULAR)


// spotlight shapes
#define GW_SHAPE_RECT		0
#define GW_SHAPE_CIRCULAR	1

// texture tiling
#define GW_TEX_NO_TILING	0
#define GW_TEX_REPEAT		1
#define GW_TEX_MIRROR		2

// texture operations
#define GW_TEX_LEAVE				0	// Use the source pixel value
#define GW_TEX_REPLACE				1	// Use the texture pixel value
#define GW_TEX_MODULATE				2	// Multiply the source with the texture
#define GW_TEX_ADD					3	// Add the source and texture
#define GW_TEX_ADD_SIGNED			4	// Add the source and texture with an 0.5 subtraction
#define GW_TEX_SUBTRACT				5	// Subtract the source from the texture
#define GW_TEX_ADD_SMOOTH			6	// Add the source and the texture then subtract their product
#define GW_TEX_ALPHA_BLEND			7	// Alpha blend the texture with the source
#define GW_TEX_PREMULT_ALPHA_BLEND	8	// Alpha blend the the source with a premultiplied alpha texture
#define GW_TEX_ALPHA_BLEND2			9	// Alpha blend the the source with a premultiplied alpha texture with GL_MODULATE as the tex env instead of GL_DECAL.

// texture scale factors
#define GW_TEX_SCALE_1X		0	// Multiply the tex op result by 1
#define GW_TEX_SCALE_2X		1	// Multiply the tex op result by 2
#define GW_TEX_SCALE_4X		2	// Multiply the tex op result by 4

// texture alpha sources
#define GW_TEX_ZERO			0	// Use no alpha value
#define GW_TEX_SOURCE		1	// Use the source alpha
#define GW_TEX_TEXTURE		2	// Use the texture alpha
#define GW_TEX_CONSTANT		3	// Use a constant BGRA color as an alpha
#define GW_TEX_PREVIOUS		4	// Use the previous texture stage alpha

// View volume clip flags
#define GW_LEFT_PLANE		0x0100
#define GW_RIGHT_PLANE		0x0200
#define GW_BOTTOM_PLANE		0x0400
#define GW_TOP_PLANE		0x0800
#define GW_FRONT_PLANE		0x1000
#define GW_BACK_PLANE		0x2000
#define GW_PLANE_MASK		0x3f00

// edge styles
#define GW_EDGE_SKIP		0
#define GW_EDGE_VIS			1
#define GW_EDGE_INVIS		2

// buffer types (for dual-plane stuff)
#define BUF_F_BUFFER		0
#define BUF_Z_BUFFER		1

// support method return values
#define GW_DOES_SUPPORT			TRUE
#define GW_DOES_NOT_SUPPORT		FALSE

// support queries
#define GW_SPT_TXT_CORRECT		0	// allow persp correction to be toggled?
#define GW_SPT_GEOM_ACCEL		1	// do 3D xforms, clipping, lighting thru driver?
#define GW_SPT_TRI_STRIPS		2	// send down strips instead of individual triangles?
#define GW_SPT_DUAL_PLANES		3	// allow dual planes to be used?
#define GW_SPT_SWAP_MODEL		4	// update viewports with complete redraw on WM_PAINT?
#define GW_SPT_INCR_UPDATE		5	// redraw only damaged areas on object move?
#define GW_SPT_1_PASS_DECAL		6	// do decaling with only one pass?
#define GW_SPT_DRIVER_CONFIG	7	// allow driver config dialog box?
#define GW_SPT_TEXTURED_BKG		8	// is viewport background a texture?
#define GW_SPT_VIRTUAL_VPTS		9	// are viewports bigger than the window allowed?
#define GW_SPT_PAINT_DOES_BLIT	10	// does WM_PAINT cause a backbuffer blit?
#define GW_SPT_WIREFRAME_STRIPS	11	// if true, wireframe objects are sent as tristrips
#define GW_SPT_ORG_UPPER_LEFT	12	// true if device origin is at upper left, false o/w
#define GW_SPT_ARRAY_PROCESSING	13	// true if the driver can handle vertex array data
#define GW_SPT_NUM_LIGHTS		14	// number of lights supported
#define GW_SPT_NUM_TEXTURES		15	// number of multitexture stages supported
#define GW_SPT_WIRE_FACES		16	// support for wireframe faces with visibility flags
#define GW_SPT_TOTAL			17	// always the max number of spt queries

// display state of the graphics window
#define GW_DISPLAY_MAXIMIZED	1
#define GW_DISPLAY_WINDOWED		2
#define GW_DISPLAY_INVISIBLE	3

// multi-pass rendering
#define GW_PASS_ONE				0
#define GW_PASS_TWO				1

// Light attenuation types -- not fully implemented
#define GW_ATTEN_NONE		0x0000
#define GW_ATTEN_START		0x0001
#define GW_ATTEN_END		0x0002
#define GW_ATTEN_LINEAR		0x0010
#define GW_ATTEN_QUAD		0x0020

//! Internal Use only
//! NH Flags used to tell Max graphics drivers that the user has returned from various system events
//! Internal Use only - resolves issues with DirectX - the flags are used with GraphicsWindow::setFlags
#define SYSTEM_LOCK_RETURN 0x001
#define SYSTEM_REMOTE_RETURN 0x002