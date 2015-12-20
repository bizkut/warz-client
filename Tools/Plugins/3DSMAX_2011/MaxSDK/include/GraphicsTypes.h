//
// Copyright 2009 Autodesk, Inc.  All rights reserved. 
//
// This computer source code and related instructions and comments are the
// unpublished confidential and proprietary information of Autodesk, Inc. and
// are protected under applicable copyright and trade secret law.  They may
// not be disclosed to, copied or used by any third party without the prior
// written consent of Autodesk, Inc.
#pragma once

#include <WTypes.h>

typedef BOOL	(*HitFunc)(int, int, void *);
typedef void (*GFX_ESCAPE_FN)(void *);

enum CameraType
{
	PERSP_CAM,
	ORTHO_CAM
};

//! \brief Color types (used by setColor)
enum ColorType
{
	LINE_COLOR,
	FILL_COLOR,
	TEXT_COLOR,
	CLEAR_COLOR
};

//! \brief Marker types
enum MarkerType
{
	POINT_MRKR,
	HOLLOW_BOX_MRKR,
	PLUS_SIGN_MRKR, 
	ASTERISK_MRKR,
	X_MRKR,
	BIG_BOX_MRKR, 
	CIRCLE_MRKR,
	TRIANGLE_MRKR,
	DIAMOND_MRKR,
	SM_HOLLOW_BOX_MRKR,
	SM_CIRCLE_MRKR, 
	SM_TRIANGLE_MRKR,
	SM_DIAMOND_MRKR,
	DOT_MRKR,
	SM_DOT_MRKR,
	BOX2_MRKR,
	BOX3_MRKR,
	BOX4_MRKR,
	BOX5_MRKR,
	BOX6_MRKR,
	BOX7_MRKR,
	DOT2_MRKR,
	DOT3_MRKR,
	DOT4_MRKR,
	DOT5_MRKR,
	DOT6_MRKR,
	DOT7_MRKR
};

//! \brief Facing type
enum FacingType
{
	kFrontFacing,
	kSideFacing,
	kBackFacing
};