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

// Face Flags: (moved from mesh.h)
// 		3 LSBs hold the edge visibility flags
// 		Bit 3 indicates the presence of texture verticies

// if bit is 1, edge is visible
#define EDGE_VIS			1
#define EDGE_INVIS			0

// first edge-visibility bit field
#define VIS_BIT			0x0001
#define VIS_MASK			0x0007

#define EDGE_A		(1<<0)
#define EDGE_B		(1<<1)
#define EDGE_C		(1<<2)
#define EDGE_ALL	(EDGE_A|EDGE_B|EDGE_C)

#define FACE_HIDDEN	(1<<3)
#define HAS_TVERTS	(1<<4)	// DO NOT USE: This flag is obselete.
#define FACE_WORK	(1<<5) // used in various algorithms
#define FACE_STRIP	(1<<6)

// flags to indicate that face normal is used because no smoothing group
// normal is found
#define FACE_NORM_A	(1<<8)
#define FACE_NORM_B	(1<<9)
#define FACE_NORM_C	(1<<10)
#define FACE_NORM_MASK	0x0700

#define FACE_INFOREGROUND	(1<<11)  //watje this is used to track faces that are in the foreground we should not draw thes but they are hitested against
#define FACE_BACKFACING		(1<<12)  //watje this is used to track which faces are back faacing this is a temporary flag 


// The mat ID is stored in the HIWORD of the face flags
#define FACE_MATID_SHIFT	16
#define FACE_MATID_MASK		0xFFFF