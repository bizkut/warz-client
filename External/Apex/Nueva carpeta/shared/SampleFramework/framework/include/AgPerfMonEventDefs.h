/*
 * Copyright 2008-2012 NVIDIA Corporation.  All rights reserved.
 * Copyright 2007-2008 Ageia Technologies.  All rights reserved.
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

// This file is used to define a list of AgPerfMon events.
//
// This file is included exclusively by AgPerfMonEventSrcAPI.h
// and by AgPerfMonEventSrcAPI.cpp, for the purpose of building
// an enumeration (enum xx) and an array of strings ()
// that contain the list of events.
//
// This file should only contain event definitions, using the
// DEFINE_EVENT macro.  E.g.:
//
//     DEFINE_EVENT(sample_name_1)
//     DEFINE_EVENT(sample_name_2)
//     DEFINE_EVENT(sample_name_3)


// Statistics from the fluid mesh packet cooker
DEFINE_EVENT(renderFunction)
DEFINE_EVENT(SampleRendererVBwriteBuffer)
DEFINE_EVENT(SampleOnTickPreRender)
DEFINE_EVENT(SampleOnTickPostRender)
DEFINE_EVENT(SampleOnRender)
DEFINE_EVENT(SampleOnDraw)
DEFINE_EVENT(D3D9Renderer_createVertexBuffer)
DEFINE_EVENT(Renderer_render)
DEFINE_EVENT(Renderer_render_depthOnly)
DEFINE_EVENT(Renderer_render_deferred)
DEFINE_EVENT(Renderer_render_lit)
DEFINE_EVENT(Renderer_render_unlit)
DEFINE_EVENT(Renderer_renderMeshes)
DEFINE_EVENT(Renderer_renderDeferredLights)
DEFINE_EVENT(D3D9RendererMesh_renderIndices)
DEFINE_EVENT(D3D9RendererMesh_renderVertices)
DEFINE_EVENT(D3D9Renderer_createIndexBuffer)
DEFINE_EVENT(D3D9RendererMesh_renderVerticesInstanced)
DEFINE_EVENT(D3D9Renderer_createInstanceBuffer)
DEFINE_EVENT(D3D9Renderer_createTexture2D)
DEFINE_EVENT(D3D9Renderer_createTarget)
DEFINE_EVENT(D3D9Renderer_createMaterial)
DEFINE_EVENT(D3D9Renderer_createMesh)
DEFINE_EVENT(D3D9Renderer_createLight)
DEFINE_EVENT(D3D9RendererMesh_renderIndicesInstanced)
DEFINE_EVENT(OGLRenderer_createVertexBuffer)
DEFINE_EVENT(OGLRenderer_createIndexBuffer)
DEFINE_EVENT(OGLRenderer_createInstanceBuffer)
DEFINE_EVENT(OGLRenderer_createTexture2D)
DEFINE_EVENT(OGLRenderer_createTarget)
DEFINE_EVENT(OGLRenderer_createMaterial)
DEFINE_EVENT(OGLRenderer_createMesh)
DEFINE_EVENT(OGLRenderer_createLight)
DEFINE_EVENT(OGLRendererMaterial_compile_vertexProgram)
DEFINE_EVENT(OGLRendererMaterial_load_vertexProgram)
DEFINE_EVENT(OGLRendererMaterial_compile_fragmentProgram)
DEFINE_EVENT(OGLRendererMaterial_load_fragmentProgram)
DEFINE_EVENT(OGLRendererVertexBufferBind)
DEFINE_EVENT(OGLRendererSwapBuffers)
DEFINE_EVENT(writeBufferSemanticStride)
DEFINE_EVENT(writeBufferfixUV)
DEFINE_EVENT(writeBufferConvertFromApex)
DEFINE_EVENT(writeBufferGetFormatSemantic)
DEFINE_EVENT(writeBufferlockSemantic)
DEFINE_EVENT(OGLRendererVertexBufferLock)
DEFINE_EVENT(Renderer_meshRenderLast)
DEFINE_EVENT(Renderer_atEnd)
DEFINE_EVENT(renderMeshesBindMeshContext)
DEFINE_EVENT(renderMeshesFirstIf)
DEFINE_EVENT(renderMeshesSecondIf)
DEFINE_EVENT(renderMeshesThirdIf)
DEFINE_EVENT(renderMeshesForthIf)
DEFINE_EVENT(OGLRendererBindMeshContext)
DEFINE_EVENT(OGLRendererBindMeshcg)
DEFINE_EVENT(cgGLSetMatrixParameter)
DEFINE_EVENT(D3D9RenderVBlock)
DEFINE_EVENT(D3D9RenderVBunlock)
DEFINE_EVENT(D3D9RenderIBlock)
DEFINE_EVENT(D3D9RenderIBunlock)