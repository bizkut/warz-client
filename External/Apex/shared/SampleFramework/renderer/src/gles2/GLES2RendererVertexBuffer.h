
#ifndef GLES2_RENDERER_VERTEXBUFFER_H
#define GLES2_RENDERER_VERTEXBUFFER_H

#include <RendererConfig.h>

#if defined(RENDERER_ENABLE_GLES2)

#include <RendererVertexBuffer.h>
#include "GLES2Renderer.h"

namespace SampleRenderer
{

class GLES2RendererVertexBuffer : public RendererVertexBuffer
{
	public:
		GLES2RendererVertexBuffer(const RendererVertexBufferDesc &desc);
		virtual ~GLES2RendererVertexBuffer(void);
		static	PxU32 convertColor(const RendererColor& color);	
		
	protected:
		virtual void  swizzleColor(void *colors, PxU32 stride, PxU32 numColors, RendererVertexBuffer::Format inFormat);
		virtual void *lock(void);
		virtual void  unlock(void);
		
		virtual void  bind(PxU32 streamID, PxU32 firstVertex);
		virtual void  unbind(PxU32 streamID);

        virtual void bindGL(GLint position, const SemanticDesc &sm, PxU8 *buffer);	
        virtual void unbindGL(GLint attr);
	private:
        void  *m_data;
		GLuint m_vbo;
		PxU32  m_bufferSize;
};

}

#endif // #if defined(RENDERER_ENABLE_GLES2)
#endif
