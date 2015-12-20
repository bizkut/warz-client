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

#ifndef RENDERER_H
#define RENDERER_H

#include <RendererConfig.h>

#include <RendererMaterial.h>
#include <RendererWindow.h>
#include <RendererColor.h>
#include <SampleUserInput.h>

#include <vector>
#include <string>
#include <map>
#include <queue>

#include "foundation/PxVec2.h"

namespace physx {
	class PxProfileEventSender;
	class PxProfileZone;
}

namespace SampleRenderer
{

	class RendererDesc;
	class RendererWindow;
	class RendererVertexBuffer;
	class RendererVertexBufferDesc;
	class RendererIndexBuffer;
	class RendererIndexBufferDesc;
	class RendererSurfaceBuffer;
	class RendererSurfaceBufferDesc;
	class RendererInstanceBuffer;
	class RendererInstanceBufferDesc;
	class RendererTexture;
	class RendererTextureDesc;
	class RendererTarget;
	class RendererTargetDesc;
	class RendererMaterial;
	class RendererMaterialDesc;
	class RendererMesh;
	class RendererMeshDesc;
	class RendererMeshContext;
	class RendererLight;
	class RendererLightDesc;

	class RendererColor;
	class RendererProjection;

	class ScreenQuad
	{
		public:
						ScreenQuad();

		RendererColor	mLeftUpColor;		//!< Color for left-up vertex
		RendererColor	mLeftDownColor;		//!< Color for left-down vertex
		RendererColor	mRightUpColor;		//!< Color for right-up vertex
		RendererColor	mRightDownColor;	//!< Color for right-down vertex
		PxReal			mAlpha;				//!< Alpha value
		PxReal			mX0, mY0;			//!< Up-left coordinates
		PxReal			mX1, mY1;			//!< Bottom-right coordinates
	};

	class Renderer
	{
	public:
		struct TextVertex
		{
			PxVec3	p;
			PxReal	rhw;
			PxU32	color;
			PxReal	u,v;
		};

		struct TessellationParams
		{
			TessellationParams();

			void setDefault(); 
			std::string toString();

			PxVec4 tessFactor;
			PxReal tessMinMaxDistance[2];
			PxReal tessHeightScaleAndBias[2];
			PxReal tessUVScale[2];
		};

		typedef enum DriverType
		{
			DRIVER_OPENGL   = 0,	// Supports Windows, Linux, MacOSX and PS3.
			DRIVER_GLES2,			// Supports Android (> 2.2) and iPhone.
			DRIVER_DIRECT3D9,		// Supports Windows, XBOX360.
			DRIVER_LIBGCM,			// Supports PS3.
			DRIVER_LIBGXM,			// Supports PSP2
			DRIVER_DIRECT3D11,		// Supports Windows 7/Vista.
		} DriverType;

	public:
		// assetDir should point to the directory where shaders can be found in the assetDir/shaders/ subdirectory and textures are in assetDir/textures
		// enableMaterialCaching will try to compile as few shaders as possible
		static Renderer *createRenderer(const RendererDesc &desc, const char* assetDir, bool enableMaterialCaching = true);

		static const char *getDriverTypeName(DriverType type);

	protected:
		Renderer(DriverType driver, const char* shaderDir);
		virtual ~Renderer(void);

	public:
		void release(void);

		// get the driver type for this renderer.
		DriverType getDriverType(void) const;

		// get the offset to the center of a pixel relative to the size of a pixel (so either 0 or 0.5).
		PxF32 getPixelCenterOffset(void) const;

		// get the name of the hardware device.
		const char *getDeviceName(void) const;

		// adds a mesh to the render queue.
		void queueMeshForRender(RendererMeshContext &mesh);
		void removeMeshFromRenderQueue(RendererMesh& mesh);

		// adds a light to the render queue.
		void queueLightForRender(RendererLight &light);
		void removeLightFromRenderQueue(RendererLight &light);

		// renders the current scene to the offscreen buffers. empties the render queue when done.
		void render(const physx::PxMat44 &eye, const RendererProjection &proj, RendererTarget *target=0, bool depthOnly=false);

		// sets fog
		void setFog(const RendererColor &fogColor, float fogDistance);

		// sets the ambient lighting color.
		void setAmbientColor(const RendererColor &ambientColor);
		// get the ambient lighting color.
		RendererColor getAmbientColor();

		// sets the clear color.
		void setClearColor(const RendererColor &clearColor);
		RendererColor& getClearColor() { return m_clearColor; }

		// sets whether tessellation is enabled.  must be supported by the underlying renderer implementation
		void setEnableTessellation(bool enable)   { m_enableTessellation = enable && isTessellationSupported(); }
		bool getEnableTessellation() const { return m_enableTessellation; }
		virtual bool isTessellationSupported(void) const { return false; }

		// sets appropriate tessellation parameters
		void setTessellationParams(const TessellationParams&);
		TessellationParams& getTessellationParams() { return m_tessellationParams; }

		// sets whether wireframe mode is enabled
		void toggleWireframe() { m_enableWireframe = !m_enableWireframe; }
		bool wireframeEnabled() const { return m_enableWireframe; }

		// sets whether to override the blending of individual meshes
		void setEnableBlendingOverride(bool enable) { m_enableBlendingOverride = enable; }
		bool blendingOverrideEnabled() const { return m_enableBlendingOverride; }

		// sets whether to use culling on meshes with blending
		void setEnableBlendingCull(bool enable) { m_enableBlendingCull = enable; }
		bool blendingCull() const { return m_enableBlendingCull; }

		// get and set the output message stream
		void setErrorCallback(PxErrorCallback* errc) { m_errorCallback = errc; }
		PxErrorCallback* getErrorCallback() { return m_errorCallback; }

		// clears the offscreen buffers.
		virtual void clearBuffers(void) = 0;

		// presents the current color buffer to the screen.
		// returns true on device reset and if buffers need to be rewritten.
		virtual bool swapBuffers(void) = 0;
        
		// get the device pointer (void * abstraction)
		virtual void *getDevice() = 0;
		
		// save the screen data to disk
		virtual bool captureScreen(const char* filename);

		// gets a handle to the current frame's data, in bitmap format
		//    note: subsequent calls will invalidate any previously returned data
		virtual bool captureScreen(PxU32 &width, PxU32& height, PxU32& sizeInBytes, const void*& screenshotData) = 0;

		virtual void getWindowSize(PxU32 &width, PxU32 &height) const = 0;

		virtual PxU32 convertColor(const RendererColor& color) const = 0;
		virtual void finishRendering() {}

		virtual RendererVertexBuffer   *createVertexBuffer(  const RendererVertexBufferDesc   &desc) = 0;
		virtual RendererIndexBuffer    *createIndexBuffer(   const RendererIndexBufferDesc    &desc) = 0;
//		virtual RendererSurfaceBuffer  *createSurfaceBuffer( const RendererSurfaceBufferDesc  &desc) = 0;
		virtual RendererInstanceBuffer *createInstanceBuffer(const RendererInstanceBufferDesc &desc) = 0;
		virtual RendererTexture        *createTexture(       const RendererTextureDesc        &desc);
		virtual RendererTexture2D      *createTexture2D(     const RendererTexture2DDesc      &desc) = 0;
		virtual RendererTexture3D      *createTexture3D(     const RendererTexture3DDesc      &desc) = 0;
		virtual RendererTarget         *createTarget(        const RendererTargetDesc         &desc) = 0;
		virtual RendererMaterial       *createMaterial(      const RendererMaterialDesc       &desc) = 0;
		virtual RendererMesh           *createMesh(          const RendererMeshDesc           &desc) = 0;
		virtual RendererLight          *createLight(         const RendererLightDesc          &desc) = 0;

		// These two methods are only necessary for internal caching of compiled materials
		bool                            getEnableMaterialCaching() { return mEnableMaterialCaching; }

		virtual void                    setVsync(bool on) = 0;
	protected:
		void                            setEnableMaterialCaching(bool enable) { mEnableMaterialCaching = enable; }
		RendererMaterial               *hasMaterialAlready(  const RendererMaterialDesc& desc);
		void                            registerMaterial(    const RendererMaterialDesc& desc, RendererMaterial* mat);
		void                            releaseAllMaterials();

		void formatScreenshot(PxU32 width, PxU32 height, PxU32 sizeInBytes, int rPosition, int gPosition, int bPosition, bool bFlipY, void* screenshotData);
	public:

		// Text rendering
		virtual	bool					initTexter();
		virtual	void					closeTexter();
		void							print(PxU32 x, PxU32 y, const char* text, PxReal scale=0.5f, PxReal shadowOffset=6.0f, RendererColor textColor = RendererColor(255, 255, 255, 255), bool forceFixWidthNumbers = false);
		void							print(PxU32* x, PxU32* y, const char** text, PxU32 textCount, PxReal scale=0.5f, PxReal shadowOffset=6.0f, RendererColor* textColors = NULL, bool forceFixWidthNumbers = false);
		const char*						getAssetDir();
		
		// assetDir should point to the directory where shaders can be found in the assetDir/shaders/ subdirectory and textures are in assetDir/textures
		void							setAssetDir( const char * assetDir );

		// On-screen controls/sticks for tablets
#if defined(RENDERER_TABLET)
		struct TabletButton
		{
			PxVec2						leftBottom;
			PxVec2						rightTop;
			physx::PxU8					pressedCount;
			physx::PxU16				emulatedKeyCode;
			std::string					text;
			RendererMesh*				mesh;
			RendererMaterial*			material;
			RendererMaterialInstance*	materialInstance;
			
			PxVec4						defaultColor, pressedColor;
			
			void						(*callback)();
			
			TabletButton();
			void 						setPressedCount(physx::PxU8);
			void 						incPressed();
			void 						decPressed();
		};
		
		virtual bool					initControls(RendererMaterial* controlMaterial, RendererMaterialInstance* controlMaterialInstance);
		void							setControlPosition(int ctrl_idx, const PxVec2&);
		void							setControlDefaultPosition(int ctrl_idx);
		PxBounds3						getControlBounds(int ctrl_idx);
		PxBounds3						getCenteredControlBounds(int ctrl_idx);
		PxVec2							getControlPosition(int ctrl_idx);
		PxVec2							getCenteredControlPosition(int ctrl_idx);
		// On-screen buttons for tablets
		void							addButton(const PxVec2& leftBottom, 
												const PxVec2& rightTop, 
												void (*func_ptr)(), 
												RendererMaterial* controlMaterial, 
												RendererMaterialInstance* controlMaterialInstance);
		void							releaseAllButtons();
		void							bindButtonToUserInput(size_t buttonIndex, physx::PxU16 userInputId, const char* buttonName);
		std::vector<TabletButton>& 		screenButtons();
#endif

		// Screenquad
		virtual	bool					initScreenquad();
		virtual	void					closeScreenquad();
				bool					drawScreenQuad(const ScreenQuad& screenQuad);
				bool					drawLines2D(PxU32 nbVerts, const PxReal* vertices, const RendererColor& color);
				bool					drawLines2D(PxU32 nbVerts, const PxReal* vertices, const RendererColor* colors);

		static void setProfileZone( physx::PxProfileZone* inSDK ) { mProfileZone = inSDK; }
		static physx::PxProfileEventSender*	getEventBuffer();

	private:
		void renderMeshes(std::vector<RendererMeshContext>& meshes, RendererMaterial::Pass pass);
		void renderDeferredLights(void);
		void sortMeshes(const physx::PxMat44& eye);

	private:		
		RendererMesh*				initControl(PxReal* vertices, PxReal* texcoords, PxU32 verticesCount);

		class ScopedRender;
		virtual bool beginRender(void) { return true;}
		virtual void endRender(void) {}
		virtual void bindViewProj(const physx::PxMat44 &eye, const RendererProjection &proj)    = 0;
		virtual void bindFogState(const RendererColor &fogColor, float fogDistance)      = 0;
		virtual void bindAmbientState(const RendererColor &ambientColor)                 = 0;
		virtual void bindDeferredState(void)                                             = 0;
		virtual void bindMeshContext(const RendererMeshContext &context)                 = 0;
		virtual void beginMultiPass(void)                                                = 0;
		virtual void endMultiPass(void)                                                  = 0;
		virtual void beginTransparentMultiPass(void)                                     = 0;
		virtual void endTransparentMultiPass(void)                                       = 0;
		virtual void renderDeferredLight(const RendererLight &light)                     = 0;

		virtual bool isOk(void) const = 0;

	public:
		virtual	void setupTextRenderStates()		= 0;
		virtual	void resetTextRenderStates()		= 0;
		virtual	void renderTextBuffer(const void* vertices, PxU32 nbVerts, const PxU16* indices, PxU32 nbIndices, RendererMaterial* material) = 0;

#if defined(RENDERER_PSP2)
		virtual void renderLines2D(const void* vertices, PxU32 nbVerts, RendererMaterial* material)	= 0; 
#else
		virtual	void renderLines2D(const void* vertices, PxU32 nbVerts)	= 0;
#endif
		
#if defined(RENDERER_TABLET)
		virtual void renderControls(const ScreenQuad& screenQuad);
		virtual void renderButtons(const ScreenQuad& screenQuad);
#endif
		virtual	void setupScreenquadRenderStates()	= 0;
		virtual	void resetScreenquadRenderStates()	= 0;

	private:
		Renderer &operator=(const Renderer&) { return *this; }


		static physx::PxProfileZone*		  mProfileZone;

		const DriverType                  m_driver;
	
	protected:
		PxErrorCallback*                  m_errorCallback;
		// Texter data
		RendererMaterial*				m_textMaterial;
		RendererMaterialInstance*		m_textMaterialInstance;

	private:
		typedef std::vector<RendererMeshContext> MeshVector;
		MeshVector  m_visibleLitMeshes;
		MeshVector  m_visibleUnlitMeshes;
		MeshVector  m_screenSpaceMeshes;
		MeshVector  m_visibleLitTransparentMeshes;
		
		//std::priority_queue<RendererMeshContext, MeshVector, CompareMeshCameraDistance> m_visibleLitTransparentMeshes;
		std::vector<RendererLight*>       m_visibleLights;

		RendererColor					  m_fogColor;
		float							  m_fogDistance;

		RendererColor                     m_ambientColor;
		RendererColor                     m_clearColor;

		// Screenquad data
		RendererMaterial*				m_screenquadOpaqueMaterial;
		RendererMaterialInstance*		m_screenquadOpaqueMaterialInstance;
		RendererMaterial*				m_screenquadAlphaMaterial;
		RendererMaterialInstance*		m_screenquadAlphaMaterialInstance;

#if defined(RENDERER_TABLET)			
		RendererMaterial*				m_controlMaterial;
		RendererMaterialInstance*		m_controlMaterialInstance;
		RendererMesh*					m_controlMesh[2];
		PxVec2							m_controlHalfSize;
		PxVec2							m_controlCenteredPos[2];
		PxVec2							m_controlPos[2];
		std::vector<TabletButton>		m_buttons;
#endif		

	protected:
		PxF32                             m_pixelCenterOffset;
		char                              m_deviceName[256];
		bool                              m_useShadersForTextRendering;
		std::string                       m_assetDir;

		// Tessellation data
		TessellationParams                m_tessellationParams;
		bool                              m_enableTessellation;

		bool                              m_enableWireframe;
		bool                              m_enableBlendingOverride;
		bool                              m_enableBlendingCull;

		bool                              mEnableMaterialCaching;
		struct CompareRenderMaterialDesc
		{
			bool operator()(const RendererMaterialDesc& desc1, const RendererMaterialDesc& desc2) const;
		};
		typedef std::map<RendererMaterialDesc, RendererMaterial*, CompareRenderMaterialDesc> tMaterialCache;
		tMaterialCache                    m_materialCache;
	};

	inline physx::PxProfileEventSender*	getEventBuffer() { return Renderer::getEventBuffer(); }

} // namespace SampleRenderer

#endif
