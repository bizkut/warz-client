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
#ifndef GXM_RENDERER_H
#define GXM_RENDERER_H

#include <RendererConfig.h>

#if defined(RENDERER_ENABLE_LIBGXM)

class SceGxmContext;
class SceGxmRenderTarget;

#include <Renderer.h>
#include <psp2/PSP2GraphicsUtility.h>
#include <vectormath.h>

namespace SampleFramework {
	class SamplePlatform;
}

namespace SampleRenderer
{
	class GXMRendererMaterial;
	class GXMRenderer;
	class GXMRendererVertexBuffer;
	class GXMRendererInstanceBuffer;
	class GXMRendererIndexBuffer;

	typedef struct DisplayData{
		void *			m_address;
		GXMRenderer*	m_renderer;
	};


	class GXMRenderer : public Renderer
	{
	public:
		GXMRenderer(const RendererDesc &desc, const char* assetDir);
		virtual ~GXMRenderer(void);



		const PxMat44 &getViewMatrix(void) const { return m_viewMatrix; }

		void                       setCurrentMaterial(const GXMRendererMaterial *cm) { m_currMaterial = cm;   }
		const GXMRendererMaterial *getCurrentMaterial(void)                          { return m_currMaterial; }

		class ShaderEnvironment
		{
		public:
			sce::Vectormath::Scalar::Aos::Matrix4   mModelMatrix;
			sce::Vectormath::Scalar::Aos::Matrix4   mViewMatrix;
			sce::Vectormath::Scalar::Aos::Matrix4   mProjMatrix;
			sce::Vectormath::Scalar::Aos::Matrix4   mModelViewMatrix;
			sce::Vectormath::Scalar::Aos::Matrix4   mModelViewProjMatrix;

			sce::Vectormath::Scalar::Aos::Matrix4	mModelViewProjMatrixUniform;

			float									mFogColorAndDistance;

			sce::Vectormath::Scalar::Aos::Vector4	mLightColor;
			float									mLightIntensity;
			float									mLightDirection[4];
			float									mLightPosition[4];
			float									mLightInnerRadius;
			float									mLightOuterRadius;
			float									mLightInnerCone;
			float									mLightOuterCone;

			float									mEyePosition[4];
			float									mEyeDirection[4];

			sce::Vectormath::Scalar::Aos::Vector4   mAmbientColor;

			SceGxmPrimitiveType						mFillMode;

			const GXMRendererVertexBuffer*			mVertexBuffer;
			const GXMRendererInstanceBuffer*		mInstanceBuffer;
			const GXMRendererIndexBuffer*			mIndexBufffer;
			PxU32									mNumIndices;
			PxU32									mNumVertices;
			PxU32									mNumInstances;

			bool									mValid;
			
		public:
			ShaderEnvironment(void);
		};

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
		virtual void *getDevice(); 

		virtual void finishRendering();

		// get the window size
		void getWindowSize(PxU32 &width, PxU32 &height) const;

		virtual RendererVertexBuffer   *createVertexBuffer(  const RendererVertexBufferDesc   &desc);
		virtual RendererIndexBuffer    *createIndexBuffer(   const RendererIndexBufferDesc    &desc);
		virtual RendererInstanceBuffer *createInstanceBuffer(const RendererInstanceBufferDesc &desc);
		virtual RendererTexture2D      *createTexture2D(     const RendererTexture2DDesc      &desc);
		virtual RendererTarget         *createTarget(        const RendererTargetDesc         &desc);
		virtual RendererMaterial       *createMaterial(      const RendererMaterialDesc       &desc);
		virtual RendererMesh           *createMesh(          const RendererMeshDesc           &desc);
		virtual RendererLight          *createLight(         const RendererLightDesc          &desc);

		virtual bool captureScreen(const char* filename);

		void renderDebugText( const DisplayData* displayData );

		static void *graphicsAlloc(SceKernelMemBlockType type, uint32_t size, uint32_t alignment, uint32_t attribs, SceUID *uid);
		static void graphicsFree(SceUID uid);

		SceGxmShaderPatcher* getShaderPatcher() const { return m_shaderPatcher; }
		SceGxmContext*	getContext() const { return m_graphicsData.pContext; }

		const ShaderEnvironment& getShaderEnvironment() const { return m_environment; }
		ShaderEnvironment& getShaderEnvironment() { return m_environment; }

	private:		
		virtual void bindViewProj(const physx::PxMat44 &eye, const RendererProjection &proj);
		virtual void bindAmbientState(const RendererColor &ambientColor);
		virtual void bindFogState(const RendererColor &fogColor, float fogDistance);
		virtual void bindDeferredState(void);
		virtual void bindMeshContext(const RendererMeshContext &context);
		virtual void beginMultiPass(void);
		virtual void endMultiPass(void);
		virtual void renderDeferredLight(const RendererLight &light);
		virtual PxU32 convertColor(const RendererColor& color) const;

		virtual bool isOk(void) const;

		virtual	void setupTextRenderStates();
		virtual	void resetTextRenderStates();
		virtual	void renderTextBuffer(const void* vertices, PxU32 nbVerts, const PxU16* indices, PxU32 nbIndices, RendererMaterial* material);
		virtual void renderLines2D(const void* vertices, PxU32 nbVerts, RendererMaterial* material);  
		virtual	void setupScreenquadRenderStates();
		virtual	void resetScreenquadRenderStates();
		virtual bool initTexter();

		// GXM implementation
		void initGXM();		

	private:
		SampleFramework::SamplePlatform*			   m_platform;


		GraphicsUtilContextData		m_graphicsData;
		SceGxmRenderTarget *		m_renderTarget;
		DisplayData					m_displayData;
		SceGxmShaderPatcher	*		m_shaderPatcher;

		const GXMRendererMaterial *m_currMaterial;

		PxU32                      m_displayWidth;
		PxU32                      m_displayHeight;

		void*						m_TextRendererVertexBuffer;
		void*						m_TextRendererIndexBuffer;

		PxU32						m_CurrentTextRendererVertexBufferSize;
		PxU32						m_CurrentTextRendererIndexBufferSize;
		PxU32						m_TextMaxVerts;		
		PxU32						m_TextMaxIndices;
		SceUID						m_TextRendererVertexBufferUid;
		SceUID						m_TextRendererIndexBufferUid;

		ShaderEnvironment              m_environment;

		PxMat44 m_viewMatrix;
	};

} // namespace SampleRenderer

#endif // #if defined(RENDERER_ENABLE_LIBGXM)
#endif
