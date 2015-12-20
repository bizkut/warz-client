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
#ifndef OGL_RENDERER_H
#define OGL_RENDERER_H

#include <RendererConfig.h>

#if defined(RENDERER_ENABLE_OPENGL)
#include <GLIncludes.h>

#if defined(RENDERER_ENABLE_CG)
	#include <Cg/cg.h>
	#include <Cg/cgGL.h>
#endif

#include <Renderer.h>

namespace SampleFramework {
	class SamplePlatform;
}

namespace SampleRenderer
{

#if defined(RENDERER_ENABLE_CG)
	void setColorParameter(CGparameter param, const RendererColor &color);
	void setFloat4Parameter(CGparameter param, float* values);
#endif

	class OGLRendererMaterial;


	void PxToGL(GLfloat *gl44, const physx::PxMat44 &mat);
	void RenToGL(GLfloat *gl44, const RendererProjection &proj);

	class OGLRenderer : public Renderer
	{
	public:
		OGLRenderer(const RendererDesc &desc, const char* assetDir);
		virtual ~OGLRenderer(void);

#if defined(RENDERER_ENABLE_CG)
		class CGEnvironment
		{
		public:
			CGparameter modelMatrix;
			CGparameter viewMatrix;
			CGparameter projMatrix;
			CGparameter modelViewMatrix;
			CGparameter modelViewProjMatrix;

			CGparameter boneMatrices;

			CGparameter fogColorAndDistance;

			CGparameter eyePosition;
			CGparameter eyeDirection;

			CGparameter ambientColor;

			CGparameter lightColor;
			CGparameter lightIntensity;
			CGparameter lightDirection;
			CGparameter lightPosition;
			CGparameter lightInnerRadius;
			CGparameter lightOuterRadius;
			CGparameter lightInnerCone;
			CGparameter lightOuterCone;

		public:
			CGEnvironment(void);
			CGEnvironment(CGcontext cgContext);
		};

		CGcontext            getCGContext(void)           { return m_cgContext; }
		CGEnvironment       &getCGEnvironment(void)       { return m_cgEnv; }
		const CGEnvironment &getCGEnvironment(void) const { return m_cgEnv; }
#endif

		const physx::PxMat44      &getViewMatrix(void) const { return m_viewMatrix; }

		void                       setCurrentMaterial(const OGLRendererMaterial *cm) { m_currMaterial = cm;   }
		const OGLRendererMaterial *getCurrentMaterial(void)                          { return m_currMaterial; }

	private:
		bool begin(void);
		void end(void);
		void checkResize(void);

	public:
		// clears the offscreen buffers.
		virtual void clearBuffers(void);

		// presents the current color buffer to the screen.
		// returns true on device reset and if buffers need to be rewritten.
		virtual bool swapBuffers(void);

		// get the device pointer (void * abstraction)
		virtual void *getDevice()                         { return static_cast<void*>(getCGContext()); }

		// gets a handle to the current frame's data, in bitmap format
		//    note: subsequent calls will invalidate any previously returned data
		//    return true on successful screenshot capture
		virtual bool captureScreen(PxU32 &width, PxU32& height, PxU32& sizeInBytes, const void*& screenshotData);

		// get the window size
		void getWindowSize(PxU32 &width, PxU32 &height) const;

		virtual RendererVertexBuffer   *createVertexBuffer(  const RendererVertexBufferDesc   &desc);
		virtual RendererIndexBuffer    *createIndexBuffer(   const RendererIndexBufferDesc    &desc);
		virtual RendererInstanceBuffer *createInstanceBuffer(const RendererInstanceBufferDesc &desc);
		virtual RendererTexture2D      *createTexture2D(     const RendererTexture2DDesc      &desc);
		virtual RendererTexture3D      *createTexture3D(     const RendererTexture3DDesc      &desc);
		virtual RendererTarget         *createTarget(        const RendererTargetDesc         &desc);
		virtual RendererMaterial       *createMaterial(      const RendererMaterialDesc       &desc);
		virtual RendererMesh           *createMesh(          const RendererMeshDesc           &desc);
		virtual RendererLight          *createLight(         const RendererLightDesc          &desc);

		virtual void                    setVsync(bool on);

	private:
		virtual void bindViewProj(const physx::PxMat44 &inveye, const RendererProjection &proj);
		virtual void bindAmbientState(const RendererColor &ambientColor);
		virtual void bindFogState(const RendererColor &fogColor, float fogDistance);
		virtual void bindDeferredState(void);
		virtual void bindMeshContext(const RendererMeshContext &context);
		virtual void beginMultiPass(void);
		virtual void endMultiPass(void);
		virtual void beginTransparentMultiPass(void);
		virtual void endTransparentMultiPass(void);
		virtual void renderDeferredLight(const RendererLight &light);
		virtual PxU32 convertColor(const RendererColor& color) const;

		virtual bool isOk(void) const;

		virtual	void setupTextRenderStates();
		virtual	void resetTextRenderStates();
		virtual	void renderTextBuffer(const void* vertices, PxU32 nbVerts, const PxU16* indices, PxU32 nbIndices, RendererMaterial* material);
		virtual	void renderLines2D(const void* vertices, PxU32 nbVerts);
		virtual	void setupScreenquadRenderStates();
		virtual	void resetScreenquadRenderStates();

	private:
		SampleFramework::SamplePlatform*			   m_platform;
#if defined(RENDERER_ENABLE_CG)
		CGcontext                  m_cgContext;
		CGEnvironment              m_cgEnv;
#endif

		const OGLRendererMaterial *m_currMaterial;

		PxU32                      m_displayWidth;
		PxU32                      m_displayHeight;

		std::vector<GLubyte>       m_displayData;

		physx::PxMat44             m_viewMatrix;
	};

} // namespace SampleRenderer

#endif // #if defined(RENDERER_ENABLE_OPENGL)
#endif
