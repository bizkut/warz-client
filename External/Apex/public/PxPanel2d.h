/*
 * Copyright 2009-2011 NVIDIA Corporation.  All rights reserved.
 *
 * NOTICE TO USER:
 *
 * This source code is subject to NVIDIA ownership rights under U.S. and
 * international Copyright laws.  Users and possessors of this source code
 * are hereby granted a nonexclusive, royalty-free license to use this code
 * in individual and commercial software.
 *
 * NVIDIA MAKES NO REPRESENTATION ABOUT THE SUITABILITY OF THIS SOURCE
 * CODE FOR ANY PURPOSE.  IT IS PROVIDED "AS IS" WITHOUT EXPRESS OR
 * IMPLIED WARRANTY OF ANY KIND.  NVIDIA DISCLAIMS ALL WARRANTIES WITH
 * REGARD TO THIS SOURCE CODE, INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY, NONINFRINGEMENT, AND FITNESS FOR A PARTICULAR PURPOSE.
 * IN NO EVENT SHALL NVIDIA BE LIABLE FOR ANY SPECIAL, INDIRECT, INCIDENTAL,
 * OR CONSEQUENTIAL DAMAGES, OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS
 * OF USE, DATA OR PROFITS,  WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE
 * OR OTHER TORTIOUS ACTION,  ARISING OUT OF OR IN CONNECTION WITH THE USE
 * OR PERFORMANCE OF THIS SOURCE CODE.
 *
 * U.S. Government End Users.   This source code is a "commercial item" as
 * that term is defined at  48 C.F.R. 2.101 (OCT 1995), consisting  of
 * "commercial computer  software"  and "commercial computer software
 * documentation" as such terms are  used in 48 C.F.R. 12.212 (SEPT 1995)
 * and is provided to the U.S. Government only as a commercial end item.
 * Consistent with 48 C.F.R.12.212 and 48 C.F.R. 227.7202-1 through
 * 227.7202-4 (JUNE 1995), all U.S. Government End Users acquire the
 * source code with only those rights set forth herein.
 *
 * Any use of this source code in individual and commercial software must
 * include, in the user documentation and internal comments to the code,
 * the above Disclaimer and U.S. Government End Users Notice.
 */

#ifndef PX_PANEL2D_H
#define PX_PANEL2D_H

#include "PxRenderDebug.h"

// This is a helper class to do 2d debug rendering in screenspace.
namespace physx
{
	namespace general_renderdebug4
	{


class PxPanel2D
{
public:
	PxPanel2D(RenderDebug *debugRender,PxU32 pwid,PxU32 phit)
	{
		mRenderDebug = debugRender;
		mWidth		= pwid;
		mHeight		= phit;
		mRecipX		= 1.0f / (pwid/2);
		mRecipY		= 1.0f / (phit/2);
		mRenderDebug->pushRenderState();
		mRenderDebug->addToCurrentState(DebugRenderState::ScreenSpace);
	}

	~PxPanel2D(void)
	{
		mRenderDebug->popRenderState();
	}


	void setFontSize(PxU32 fontSize)
	{
		const PxF32 fontPixel = 0.009f;
		mFontSize = (PxF32)(fontSize*fontPixel);
		mFontOffset = mFontSize*0.3f;
		mRenderDebug->setCurrentTextScale(mFontSize);
	}

	void printText(PxU32 x,PxU32 y,const char *text)
	{
		PxVec3 p;
		getPos(x,y,p);
		p.y-=mFontOffset;
		mRenderDebug->debugText(p,"%s",text);
	}

	void getPos(PxU32 x,PxU32 y,PxVec3 &p)
	{
		p.z = 0;
		p.x = (PxF32)x*mRecipX-1;
		p.y = 1-(PxF32)y*mRecipY;
	}

	void drawLine(PxU32 x1,PxU32 y1,PxU32 x2,PxU32 y2)
	{
		PxVec3 p1,p2;
		getPos(x1,y1,p1);
		getPos(x2,y2,p2);
		mRenderDebug->debugLine(p1,p2);
	}
private:
	RenderDebug	*mRenderDebug;

	PxF32		mFontSize;
	PxF32		mFontOffset;
	PxF32		mRecipX;
	PxF32		mRecipY;
	PxU32		mWidth;
	PxU32		mHeight;
};

	}; // end namespace general_renderdebug4
}; // end namespace physx

#endif
