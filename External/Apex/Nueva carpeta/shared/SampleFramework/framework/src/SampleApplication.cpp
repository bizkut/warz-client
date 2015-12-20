/*
 * Copyright 2008-2012 NVIDIA Corporation.  All rights reserved.
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
#include <PsUtilities.h>
#include <SampleApplication.h>
#include <SampleCommandLine.h>
#include <SampleAssetManager.h>
#include <Renderer.h>
#include <RendererMemoryMacros.h>
#include <SamplePlatform.h>
#include "SampleFrameworkInputEventIds.h"
// for PsString.h
namespace physx
{
	namespace string {}
}
#include <PsString.h>
#include "PsFoundation.h"
#include "PsUtilities.h"

//#define RENDERER_USE_OPENGL_ON_WINDONWS	1

#ifdef PHYSX_PROFILE_SDK
#include "SamplePxProfileZone.h"
#include "PxProfileZone.h"
#include "PxProfileZoneManager.h"
extern SampleProfileEventNameProvider gProfileNameProvider;
#endif //PHYSX_PROFILE_SDK

#ifdef PX_ANDROID
#define LOGI(...) LOG_INFO("SampleApplication", __VA_ARGS__)
#define printf LOGI
#endif

#if defined(SMOOTH_CAM)
const float g_smoothCamBaseVel  = 6.0f;
const float g_smoothCamFastMul = 4.0f;
const float g_smoothCamPosLerp  = 0.4f;

const float g_smoothCamRotSpeed = 0.005f;
const float g_smoothCamRotLerp  = 0.4f;
#endif

#include "FrameworkFoundation.h"

using namespace SampleFramework;

static PxMat44 EulerToMat33(const PxVec3& e)
{
	const float c1 = cosf(e.z);
	const float s1 = sinf(e.z);
	const float c2 = cosf(e.y);
	const float s2 = sinf(e.y);
	const float c3 = cosf(e.x);
	const float s3 = sinf(e.x);
	PxMat44 m;
	m.column0 = PxVec4(c1*c2,              -s1*c2,             s2,	0.0f);
	m.column1 = PxVec4((s1*c3)+(c1*s2*s3), (c1*c3)-(s1*s2*s3),-c2*s3, 0.0f);
	m.column2 = PxVec4((s1*s3)-(c1*s2*c3), (c1*s3)+(s1*s2*c3), c2*c3, 0.0f);
	m.column3 = PxVec4(0,0,0,1);
	return m;
}

static PxVec3 Mat33ToEuler(const PxMat44& m)
{
	const PxF32 epsilon = 0.99999f;
	PxVec3 e, x, y, z;

	x = PxVec3(m.column0.x, m.column1.x, m.column2.x);
	y = PxVec3(m.column0.y, m.column1.y, m.column2.y);
	z = PxVec3(m.column0.z, m.column1.z, m.column2.z);

	if(x.z > epsilon)
	{
		e.x = PxAtan2(z.y, y.y);
		e.y = PxPi * 0.5f;
		e.z = 0;
	}
	else if(x.z < -epsilon)
	{
		e.x = PxAtan2(z.y, y.y);
		e.y = -PxPi * 0.5f;
		e.z = 0;
	}
	else
	{
		e.x = PxAtan2(-y.z, z.z);
		e.y = PxAsin(x.z);
		e.z = PxAtan2(-x.y, x.x);
	}
	return e;
}

SampleApplication::SampleApplication(const SampleCommandLine &cmdline, const char *assetPathPrefix, PxI32 camMoveButton) :
	m_cmdline(cmdline)
,   m_disableRendering(false)
,	m_rotationSpeedScale(200.0f)
,	m_moveSpeedScale(40.0f)
,	m_rightStickRotate(false)
,	m_rewriteBuffers(false)
{
	m_platform->setCWDToEXE();

	if (assetPathPrefix)
	{
		if (!searchForPath(assetPathPrefix, m_assetPathPrefix, PX_ARRAY_SIZE(m_assetPathPrefix), true, 20))
		{
			RENDERER_ASSERT(false, "assetPathPrefix could not be found in any of the parent directories!");
			m_assetPathPrefix[0] = 0;
		}
	}
	else
	{
		RENDERER_ASSERT(assetPathPrefix, "assetPathPrefix must not be NULL (try \"media\" instead)");
#if defined(RENDERER_ANDROID)
		strcpy(m_assetPathPrefix, "/sdcard/media/");
#else
		m_assetPathPrefix[0] = 0;
#endif			
	}

	m_renderer     = 0;
	m_sceneSize    = 1.0f;
	m_assetManager = 0;
	m_timeCounter	= 0;
	m_camMoveButton = camMoveButton;

	mProfileZone = NULL;
}

SampleApplication::~SampleApplication(void)
{
	RENDERER_ASSERT(!m_renderer, "Renderer was not released prior to window closure.");
	RENDERER_ASSERT(!m_assetManager, "Asset Manager was not released prior to window closure.");

	DELETESINGLE(m_platform);

	clearSearchPaths();
}

void SampleApplication::setEyeTransform(const PxMat44& eyeTransform)
{
	m_worldToView.setInverseTransform(eyeTransform);
	m_eyeRot = Mat33ToEuler(eyeTransform);
#if defined(SMOOTH_CAM)
	m_targetEyePos = m_worldToView.getInverseTransform().getPosition();
	m_targetEyeRot = m_eyeRot;
#endif
}

void SampleApplication::setEyeTransform(const PxVec3& pos, const PxVec3& rot)
{
	PxMat44 eye;
	m_eyeRot = rot;
	eye = EulerToMat33(m_eyeRot);
	eye.setPosition(pos);
#if defined(SMOOTH_CAM)
	m_targetEyePos = pos;
	m_targetEyeRot = m_eyeRot;
#endif

	m_worldToView.setInverseTransform(eye);
}

void SampleApplication::setViewTransform(const PxMat44 &viewTransform)
{
	m_worldToView.setForwardTransform(viewTransform);
	m_eyeRot = Mat33ToEuler( m_worldToView.getInverseTransform() );
#if defined(SMOOTH_CAM)
	m_targetEyePos = m_worldToView.getInverseTransform().getPosition();
	m_targetEyeRot = m_eyeRot;
#endif
}

const PxMat44& SampleApplication::getViewTransform() const
{
	return m_worldToView.getForwardTransform();
}

void SampleApplication::onOpen(void)
{
	m_platform->preRendererSetup();

	char assetDir[1024];
	physx::string::strcpy_s(assetDir, sizeof(assetDir), m_assetPathPrefix);
	physx::string::strcat_s(assetDir, sizeof(assetDir), ASSET_DIR);

	m_eyeRot = PxVec3(0,0,0);
	PxMat44 eye = PxMat44::createIdentity();
	const PxVec3 pos = PxVec3(0.0f, 2.0f, 16.0f);
	eye.setPosition(pos);
	m_worldToView.setInverseTransform(eye);
#if defined(SMOOTH_CAM)
	m_targetEyePos = pos;
	m_targetEyeRot = m_eyeRot;
#endif

	// default renderer drivers for various platforms...
	setupRendererDescription(m_renDesc);
#if defined RENDERER_USE_OPENGL_ON_WINDONWS
	m_renDesc.driver = SampleRenderer::Renderer::DRIVER_OPENGL;
#endif

	// check to see if the user wants to override the renderer driver...
#if defined(RENDERER_ENABLE_OPENGL)
	if(m_cmdline.hasSwitch("ogl"))   m_renDesc.driver = SampleRenderer::Renderer::DRIVER_OPENGL;
#endif
#if defined(RENDERER_ENABLE_GLES2)
	if(m_cmdline.hasSwitch("gles2")) m_renDesc.driver = SampleRenderer::Renderer::DRIVER_GLES2;
#endif
#if defined(RENDERER_ENABLE_DIRECT3D9)
	if(m_cmdline.hasSwitch("d3d9"))  m_renDesc.driver = SampleRenderer::Renderer::DRIVER_DIRECT3D9;
#endif
#if defined(RENDERER_ENABLE_DIRECT3D11)
	if(m_cmdline.hasSwitch("d3d11")) m_renDesc.driver = SampleRenderer::Renderer::DRIVER_DIRECT3D11;
#endif

#if defined(RENDERER_ANDROID)
	m_renDesc.windowHandle = (physx::PxU64)android_window_ptr;
#endif

	// UGLY!  We set the path here temporarily to the base media path.  The onInit() call 
	// should call m_renderer->setAssetDir() with the full SampleRenderer media path
	m_renderer = SampleRenderer::Renderer::createRenderer(m_renDesc, m_assetPathPrefix);
	m_platform->postRendererSetup(m_renderer);

	m_timeCounter = m_time.getCurrentCounterValue();

	m_assetManager = new SampleAssetManager(*m_renderer);
	addSearchPath(m_assetPathPrefix);
	//addSearchPath(assetDir);
	onInit();

	// make sure the resize method is called once
	if (m_renderer)
	{
		PxU32 width,height;
		m_renderer->getWindowSize(width, height);
		onResize(width, height);
		if ( mProfileZone != NULL )
			m_renderer->setProfileZone( mProfileZone );
	}
}

bool SampleApplication::onClose(void)
{
	onShutdown();
	DELETESINGLE(m_assetManager);

	SAFE_RELEASE(m_renderer);
	m_platform->postRendererRelease();

	return true;
}

inline float SmoothStepPolynomial( float s )
{
	if( s <= 0 ) return 0;
	if( s >= 1 ) return 1;
	return s*s*(3-2*s);
}

template <typename T>
T SmoothStep( const T& start, const T& end, float s )
{
	float ss = SmoothStepPolynomial( s );
	return ss * (end - start) + start;
}

void SampleApplication::onDraw(void)
{
	if (!getRenderer())
	{
		return;
	}

	physx::PxU64 qpc = m_time.getCurrentCounterValue();
	static float sToSeconds = float(m_time.getBootCounterFrequency().mNumerator) / float(m_time.getBootCounterFrequency().mDenominator * m_time.sNumTensOfNanoSecondsInASecond);
	float dtime = float(qpc - m_timeCounter) * sToSeconds;
	m_lastDTime = dtime;
	m_timeCounter = qpc;
	PX_ASSERT(dtime > 0);
	if(dtime > 0)
	{
		dtime = tweakElapsedTime(dtime);

		{
			onTickPreRender(dtime);
		}
		if(m_renderer)
		{
			if(!m_disableRendering)
			{
				onRender();
			}
		}
		{
			onTickPostRender(dtime);
		}

		//return;

		if(m_renderer && !m_disableRendering)
		{
			m_rewriteBuffers = m_renderer->swapBuffers();
		}

		// update scene...

		PxMat44 tmp = m_worldToView.getInverseTransform();
		PxMat44 eye = EulerToMat33(m_eyeRot);
		eye.column3 = tmp.column3;

		doInput();

		PxVec3* targetParam;
#if defined(SMOOTH_CAM)
		const float eyeSpeed = m_sceneSize * g_smoothCamBaseVel * dtime * (getPlatform()->getSampleUserInput()->getDigitalInputEventState(CAMERA_SHIFT_SPEED) ? g_smoothCamFastMul : 1.0f);
		targetParam = &m_targetEyePos;
#else
		const float eyeSpeed = m_sceneSize * 4.0f * dtime * (getPlatform()->getSampleUserInput()->getDigitalInputEventState(CAMERA_SHIFT_SPEED) ? 4.0f : 1.0f);
		targetParam = &eye.t;
#endif

		const PxVec3 column2 = eye.getBasis(2);
		const PxVec3 column0 = eye.getBasis(0);
		const PxVec3 column1 = eye.getBasis(1);

		if(getPlatform()->getSampleUserInput()->getDigitalInputEventState(CAMERA_MOVE_FORWARD))
			*targetParam -= column2 * eyeSpeed;
		if(getPlatform()->getSampleUserInput()->getDigitalInputEventState(CAMERA_MOVE_LEFT))
			*targetParam -= column0 * eyeSpeed;
		if(getPlatform()->getSampleUserInput()->getDigitalInputEventState(CAMERA_MOVE_BACKWARD))
			*targetParam += column2 * eyeSpeed;
		if(getPlatform()->getSampleUserInput()->getDigitalInputEventState(CAMERA_MOVE_RIGHT))
			*targetParam += column0 * eyeSpeed;
		if(getPlatform()->getSampleUserInput()->getDigitalInputEventState(CAMERA_MOVE_UP))
			*targetParam += column1 * eyeSpeed;
		if(getPlatform()->getSampleUserInput()->getDigitalInputEventState(CAMERA_MOVE_DOWN))
			*targetParam -= column1 * eyeSpeed;

		// move forward from gamepad
		*targetParam -= column2 * eyeSpeed *  getPlatform()->getSampleUserInput()->getAnalogInputEventState(CAMERA_GAMEPAD_MOVE_FORWARD_BACK) * m_moveSpeedScale * dtime;
		// strafe from gamepad
		*targetParam += column0 * eyeSpeed * getPlatform()->getSampleUserInput()->getAnalogInputEventState(CAMERA_GAMEPAD_MOVE_LEFT_RIGHT) * m_moveSpeedScale* dtime;

#if defined(SMOOTH_CAM)
		PxVec3 eye_t = eye.getPosition();
		eye_t = eye_t + (m_targetEyePos - eye_t) * g_smoothCamPosLerp;
		eye.setPosition(eye_t);
#endif

		// rotate from gamepad
		{
			const PxF32 rotationSpeed = m_rotationSpeedScale * dtime;
			PxF32 dx = getPlatform()->getSampleUserInput()->getAnalogInputEventState(CAMERA_GAMEPAD_ROTATE_LEFT_RIGHT) * rotationSpeed;
			PxF32 dy = getPlatform()->getSampleUserInput()->getAnalogInputEventState(CAMERA_GAMEPAD_ROTATE_UP_DOWN) * rotationSpeed;
			rotateCamera(dx, dy);
		}

		m_worldToView.setInverseTransform(eye);
	}
}

// When holding down the mouse button and dragging across the edge of the window,
//   the input provider will spit out nonsensical delta values... so threshold appropriately
void thresholdCameraRotate(physx::PxReal& dx, physx::PxReal& dy) 
{
	static const physx::PxReal invalidDelta = 1000.f;
	if ( physx::PxAbs(dx) > invalidDelta )
		dx = physx::PxSign(dx);
	if ( physx::PxAbs(dy) > invalidDelta )
		dy = physx::PxSign(dy);
}

void SampleApplication::onPointerInputEvent(const InputEvent& ie, physx::PxU32 x, physx::PxU32 y, physx::PxReal dx, physx::PxReal dy)
{
	switch (ie.m_Id)
	{
	case CAMERA_MOUSE_LOOK:
		{	
			if(getPlatform()->getSampleUserInput()->getDigitalInputEventState(CAMERA_MOVE_BUTTON))
			{
				thresholdCameraRotate(dx, dy);
				rotateCamera(dx, dy);

			}
		}
		break;
	}
}

bool SampleApplication::onDigitalInputEvent(const InputEvent& , bool val)
{
	return true;
}

void SampleApplication::moveCamera(PxF32 dx, PxF32 dy)
{
	PxMat44 tmp = m_worldToView.getInverseTransform();
	PxMat44 eye = EulerToMat33(m_eyeRot);
	eye.column3 = tmp.column3;

	PxVec3* targetParam;
#if defined(SMOOTH_CAM)
	const float eyeSpeed = m_sceneSize * g_smoothCamBaseVel * m_lastDTime * (getPlatform()->getSampleUserInput()->getDigitalInputEventState(CAMERA_SHIFT_SPEED) ? g_smoothCamFastMul : 1.0f);
	targetParam = &m_targetEyePos;
#else
	const float eyeSpeed = m_sceneSize * 4.0f * m_lastDTime * (getPlatform()->getSampleUserInput()->getDigitalInputEventState(CAMERA_SHIFT_SPEED) ? 4.0f : 1.0f);
	targetParam = &eye.t;
#endif

	const PxVec3 column2 = eye.getBasis(2);
	const PxVec3 column0 = eye.getBasis(0);
	const PxVec3 column1 = eye.getBasis(1);

	// strafe from gamepad
	*targetParam += column0 * eyeSpeed * dx * m_moveSpeedScale* m_lastDTime;
	// move forward from gamepad
	*targetParam -= column2 * eyeSpeed *  dy * m_moveSpeedScale * m_lastDTime;

#if defined(SMOOTH_CAM)
	PxVec3 eye_t = eye.getPosition();
	eye_t = eye_t + (m_targetEyePos - eye_t) * g_smoothCamPosLerp;
	eye.setPosition(eye_t);
#endif

	m_worldToView.setInverseTransform(eye);	
}

void SampleApplication::onAnalogInputEvent(const InputEvent& ie, float val)
{
	switch (ie.m_Id)
	{
	case CAMERA_GAMEPAD_ROTATE_LEFT_RIGHT:
		{	
			rotateCamera((PxF32)val, (PxF32)0.0);
		}
		break;
	case CAMERA_GAMEPAD_ROTATE_UP_DOWN:
		{	
			rotateCamera((PxF32)0.0, (PxF32)val);
		}
		break;
	case CAMERA_GAMEPAD_MOVE_LEFT_RIGHT:
		{	
			moveCamera((PxF32)val, (PxF32)0.0);
		}
		break;
	case CAMERA_GAMEPAD_MOVE_FORWARD_BACK:
		{	
			moveCamera((PxF32)0.0, (PxF32)val);
		}
		break;
	}
}

void SampleApplication::rotateCamera(PxF32 dx, PxF32 dy)
{
	const float eyeCap      = 1.5f;
#if defined(SMOOTH_CAM)
	m_targetEyeRot.x -= dy * g_smoothCamRotSpeed;
	m_targetEyeRot.y += dx * g_smoothCamRotSpeed;
	if(m_targetEyeRot.x >  eyeCap) m_targetEyeRot.x =  eyeCap;
	if(m_targetEyeRot.x < -eyeCap) m_targetEyeRot.x = -eyeCap;

	m_eyeRot= m_eyeRot + (m_targetEyeRot - m_eyeRot) * g_smoothCamRotLerp;
#else
	const float eyeRotSpeed = 0.005f;
	m_eyeRot.x -= dy * eyeRotSpeed;
	m_eyeRot.y += dx * eyeRotSpeed;
	if(m_eyeRot.x >  eyeCap) m_eyeRot.x =  eyeCap;
	if(m_eyeRot.x < -eyeCap) m_eyeRot.x = -eyeCap;
#endif
}

void SampleApplication::fatalError(const char * msg)
{
printf("Fatal Error in SampleApplication: %s\n", msg);
close();
exit(1);
}

void SampleApplication::doInput()
{
	m_platform->doInput();
}

void SampleApplication::setupRendererDescription(SampleRenderer::RendererDesc& renDesc)
{
	m_platform->setupRendererDescription(renDesc);
}
