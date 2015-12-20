/**********************************************************************

Filename    :   GL_Extensions.cpp
Content     :   GL  extension implementation.
Created     :   Automatically generated.
Authors     :   Extensions.pl

Copyright   :   (c) 2001-2011 Scaleform Corp. All Rights Reserved.

Licensees may use this file in accordance with the valid Scaleform
Commercial License Agreement provided with the software.

This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING 
THE WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR ANY PURPOSE.

***********************************************************************/

#include "Render/GL/GL_Common.h"

#ifdef SF_GL_RUNTIME_LINK

namespace Scaleform { namespace Render { namespace GL {

bool Extensions::Init()
{
    bool result = 1;
    p_glIsRenderbufferEXT = (PFNGLISRENDERBUFFEREXTPROC) SF_GL_RUNTIME_LINK("glIsRenderbufferEXT");

    p_glGenerateMipmapEXT = (PFNGLGENERATEMIPMAPEXTPROC) SF_GL_RUNTIME_LINK("glGenerateMipmapEXT");

    p_glFramebufferTexture2DEXT = (PFNGLFRAMEBUFFERTEXTURE2DEXTPROC) SF_GL_RUNTIME_LINK("glFramebufferTexture2DEXT");

    p_glDrawElementsInstanced = (PFNGLDRAWELEMENTSINSTANCEDPROC) SF_GL_RUNTIME_LINK("glDrawElementsInstanced");
        if (!p_glDrawElementsInstanced)
        {
             p_glDrawElementsInstanced = (PFNGLDRAWELEMENTSINSTANCEDPROC) SF_GL_RUNTIME_LINK("glDrawElementsInstancedARB");
             if (!p_glDrawElementsInstanced)
             {
                  p_glDrawElementsInstanced = (PFNGLDRAWELEMENTSINSTANCEDPROC) SF_GL_RUNTIME_LINK("glDrawElementsInstancedEXT");
                  if (!p_glDrawElementsInstanced && false)
                      result = 0;
             }
        }

    p_glMapBuffer = (PFNGLMAPBUFFERPROC) SF_GL_RUNTIME_LINK("glMapBuffer");
        if (!p_glMapBuffer)
        {
             p_glMapBuffer = (PFNGLMAPBUFFERPROC) SF_GL_RUNTIME_LINK("glMapBufferARB");
             if (!p_glMapBuffer)
             {
                  p_glMapBuffer = (PFNGLMAPBUFFERPROC) SF_GL_RUNTIME_LINK("glMapBufferEXT");
                  if (!p_glMapBuffer && true)
                      result = 0;
             }
        }

    p_glGenBuffers = (PFNGLGENBUFFERSPROC) SF_GL_RUNTIME_LINK("glGenBuffers");
        if (!p_glGenBuffers)
        {
             p_glGenBuffers = (PFNGLGENBUFFERSPROC) SF_GL_RUNTIME_LINK("glGenBuffersARB");
             if (!p_glGenBuffers)
             {
                  p_glGenBuffers = (PFNGLGENBUFFERSPROC) SF_GL_RUNTIME_LINK("glGenBuffersEXT");
                  if (!p_glGenBuffers && true)
                      result = 0;
             }
        }

    p_glCreateShader = (PFNGLCREATESHADERPROC) SF_GL_RUNTIME_LINK("glCreateShader");
        if (!p_glCreateShader)
        {
             p_glCreateShader = (PFNGLCREATESHADERPROC) SF_GL_RUNTIME_LINK("glCreateShaderARB");
             if (!p_glCreateShader)
             {
                  p_glCreateShader = (PFNGLCREATESHADERPROC) SF_GL_RUNTIME_LINK("glCreateShaderEXT");
                  if (!p_glCreateShader && true)
                      result = 0;
             }
        }

    p_glBufferData = (PFNGLBUFFERDATAPROC) SF_GL_RUNTIME_LINK("glBufferData");
        if (!p_glBufferData)
        {
             p_glBufferData = (PFNGLBUFFERDATAPROC) SF_GL_RUNTIME_LINK("glBufferDataARB");
             if (!p_glBufferData)
             {
                  p_glBufferData = (PFNGLBUFFERDATAPROC) SF_GL_RUNTIME_LINK("glBufferDataEXT");
                  if (!p_glBufferData && true)
                      result = 0;
             }
        }

    p_glCompressedTexImage2D = (PFNGLCOMPRESSEDTEXIMAGE2DPROC) SF_GL_RUNTIME_LINK("glCompressedTexImage2D");
        if (!p_glCompressedTexImage2D)
        {
             p_glCompressedTexImage2D = (PFNGLCOMPRESSEDTEXIMAGE2DPROC) SF_GL_RUNTIME_LINK("glCompressedTexImage2DARB");
             if (!p_glCompressedTexImage2D)
             {
                  p_glCompressedTexImage2D = (PFNGLCOMPRESSEDTEXIMAGE2DPROC) SF_GL_RUNTIME_LINK("glCompressedTexImage2DEXT");
                  if (!p_glCompressedTexImage2D && true)
                      result = 0;
             }
        }

    p_glLinkProgram = (PFNGLLINKPROGRAMPROC) SF_GL_RUNTIME_LINK("glLinkProgram");
        if (!p_glLinkProgram)
        {
             p_glLinkProgram = (PFNGLLINKPROGRAMPROC) SF_GL_RUNTIME_LINK("glLinkProgramARB");
             if (!p_glLinkProgram)
             {
                  p_glLinkProgram = (PFNGLLINKPROGRAMPROC) SF_GL_RUNTIME_LINK("glLinkProgramEXT");
                  if (!p_glLinkProgram && true)
                      result = 0;
             }
        }

    p_glActiveTexture = (PFNGLACTIVETEXTUREPROC) SF_GL_RUNTIME_LINK("glActiveTexture");
        if (!p_glActiveTexture)
        {
             p_glActiveTexture = (PFNGLACTIVETEXTUREPROC) SF_GL_RUNTIME_LINK("glActiveTextureARB");
             if (!p_glActiveTexture)
             {
                  p_glActiveTexture = (PFNGLACTIVETEXTUREPROC) SF_GL_RUNTIME_LINK("glActiveTextureEXT");
                  if (!p_glActiveTexture && true)
                      result = 0;
             }
        }

    p_glGetRenderbufferParameterivEXT = (PFNGLGETRENDERBUFFERPARAMETERIVEXTPROC) SF_GL_RUNTIME_LINK("glGetRenderbufferParameterivEXT");

    p_glGetProgramiv = (PFNGLGETPROGRAMIVPROC) SF_GL_RUNTIME_LINK("glGetProgramiv");
        if (!p_glGetProgramiv)
        {
             p_glGetProgramiv = (PFNGLGETPROGRAMIVPROC) SF_GL_RUNTIME_LINK("glGetProgramivARB");
             if (!p_glGetProgramiv)
             {
                  p_glGetProgramiv = (PFNGLGETPROGRAMIVPROC) SF_GL_RUNTIME_LINK("glGetProgramivEXT");
                  if (!p_glGetProgramiv && true)
                      result = 0;
             }
        }

    p_glCheckFramebufferStatusEXT = (PFNGLCHECKFRAMEBUFFERSTATUSEXTPROC) SF_GL_RUNTIME_LINK("glCheckFramebufferStatusEXT");

    p_glUniform1fv = (PFNGLUNIFORM1FVPROC) SF_GL_RUNTIME_LINK("glUniform1fv");
        if (!p_glUniform1fv)
        {
             p_glUniform1fv = (PFNGLUNIFORM1FVPROC) SF_GL_RUNTIME_LINK("glUniform1fvARB");
             if (!p_glUniform1fv)
             {
                  p_glUniform1fv = (PFNGLUNIFORM1FVPROC) SF_GL_RUNTIME_LINK("glUniform1fvEXT");
                  if (!p_glUniform1fv && true)
                      result = 0;
             }
        }

    p_glBlendEquation = (PFNGLBLENDEQUATIONPROC) SF_GL_RUNTIME_LINK("glBlendEquation");
        if (!p_glBlendEquation)
        {
             p_glBlendEquation = (PFNGLBLENDEQUATIONPROC) SF_GL_RUNTIME_LINK("glBlendEquationARB");
             if (!p_glBlendEquation)
             {
                  p_glBlendEquation = (PFNGLBLENDEQUATIONPROC) SF_GL_RUNTIME_LINK("glBlendEquationEXT");
                  if (!p_glBlendEquation && true)
                      result = 0;
             }
        }

    p_glUniform1iv = (PFNGLUNIFORM1IVPROC) SF_GL_RUNTIME_LINK("glUniform1iv");
        if (!p_glUniform1iv)
        {
             p_glUniform1iv = (PFNGLUNIFORM1IVPROC) SF_GL_RUNTIME_LINK("glUniform1ivARB");
             if (!p_glUniform1iv)
             {
                  p_glUniform1iv = (PFNGLUNIFORM1IVPROC) SF_GL_RUNTIME_LINK("glUniform1ivEXT");
                  if (!p_glUniform1iv && true)
                      result = 0;
             }
        }

    p_glDisableVertexAttribArray = (PFNGLDISABLEVERTEXATTRIBARRAYPROC) SF_GL_RUNTIME_LINK("glDisableVertexAttribArray");
        if (!p_glDisableVertexAttribArray)
        {
             p_glDisableVertexAttribArray = (PFNGLDISABLEVERTEXATTRIBARRAYPROC) SF_GL_RUNTIME_LINK("glDisableVertexAttribArrayARB");
             if (!p_glDisableVertexAttribArray)
             {
                  p_glDisableVertexAttribArray = (PFNGLDISABLEVERTEXATTRIBARRAYPROC) SF_GL_RUNTIME_LINK("glDisableVertexAttribArrayEXT");
                  if (!p_glDisableVertexAttribArray && true)
                      result = 0;
             }
        }

    p_glBindAttribLocation = (PFNGLBINDATTRIBLOCATIONPROC) SF_GL_RUNTIME_LINK("glBindAttribLocation");
        if (!p_glBindAttribLocation)
        {
             p_glBindAttribLocation = (PFNGLBINDATTRIBLOCATIONPROC) SF_GL_RUNTIME_LINK("glBindAttribLocationARB");
             if (!p_glBindAttribLocation)
             {
                  p_glBindAttribLocation = (PFNGLBINDATTRIBLOCATIONPROC) SF_GL_RUNTIME_LINK("glBindAttribLocationEXT");
                  if (!p_glBindAttribLocation && true)
                      result = 0;
             }
        }

    p_glCompileShader = (PFNGLCOMPILESHADERPROC) SF_GL_RUNTIME_LINK("glCompileShader");
        if (!p_glCompileShader)
        {
             p_glCompileShader = (PFNGLCOMPILESHADERPROC) SF_GL_RUNTIME_LINK("glCompileShaderARB");
             if (!p_glCompileShader)
             {
                  p_glCompileShader = (PFNGLCOMPILESHADERPROC) SF_GL_RUNTIME_LINK("glCompileShaderEXT");
                  if (!p_glCompileShader && true)
                      result = 0;
             }
        }

    p_glAttachShader = (PFNGLATTACHSHADERPROC) SF_GL_RUNTIME_LINK("glAttachShader");
        if (!p_glAttachShader)
        {
             p_glAttachShader = (PFNGLATTACHSHADERPROC) SF_GL_RUNTIME_LINK("glAttachShaderARB");
             if (!p_glAttachShader)
             {
                  p_glAttachShader = (PFNGLATTACHSHADERPROC) SF_GL_RUNTIME_LINK("glAttachShaderEXT");
                  if (!p_glAttachShader && true)
                      result = 0;
             }
        }

    p_glGetAttribLocation = (PFNGLGETATTRIBLOCATIONPROC) SF_GL_RUNTIME_LINK("glGetAttribLocation");
        if (!p_glGetAttribLocation)
        {
             p_glGetAttribLocation = (PFNGLGETATTRIBLOCATIONPROC) SF_GL_RUNTIME_LINK("glGetAttribLocationARB");
             if (!p_glGetAttribLocation)
             {
                  p_glGetAttribLocation = (PFNGLGETATTRIBLOCATIONPROC) SF_GL_RUNTIME_LINK("glGetAttribLocationEXT");
                  if (!p_glGetAttribLocation && true)
                      result = 0;
             }
        }

    p_glBlendFuncSeparate = (PFNGLBLENDFUNCSEPARATEPROC) SF_GL_RUNTIME_LINK("glBlendFuncSeparate");
        if (!p_glBlendFuncSeparate)
        {
             p_glBlendFuncSeparate = (PFNGLBLENDFUNCSEPARATEPROC) SF_GL_RUNTIME_LINK("glBlendFuncSeparateARB");
             if (!p_glBlendFuncSeparate)
             {
                  p_glBlendFuncSeparate = (PFNGLBLENDFUNCSEPARATEPROC) SF_GL_RUNTIME_LINK("glBlendFuncSeparateEXT");
                  if (!p_glBlendFuncSeparate && true)
                      result = 0;
             }
        }

    p_glGenFramebuffersEXT = (PFNGLGENFRAMEBUFFERSEXTPROC) SF_GL_RUNTIME_LINK("glGenFramebuffersEXT");

    p_glUniform1i = (PFNGLUNIFORM1IPROC) SF_GL_RUNTIME_LINK("glUniform1i");
        if (!p_glUniform1i)
        {
             p_glUniform1i = (PFNGLUNIFORM1IPROC) SF_GL_RUNTIME_LINK("glUniform1iARB");
             if (!p_glUniform1i)
             {
                  p_glUniform1i = (PFNGLUNIFORM1IPROC) SF_GL_RUNTIME_LINK("glUniform1iEXT");
                  if (!p_glUniform1i && true)
                      result = 0;
             }
        }

    p_glDeleteRenderbuffersEXT = (PFNGLDELETERENDERBUFFERSEXTPROC) SF_GL_RUNTIME_LINK("glDeleteRenderbuffersEXT");

    p_glGetShaderiv = (PFNGLGETSHADERIVPROC) SF_GL_RUNTIME_LINK("glGetShaderiv");
        if (!p_glGetShaderiv)
        {
             p_glGetShaderiv = (PFNGLGETSHADERIVPROC) SF_GL_RUNTIME_LINK("glGetShaderivARB");
             if (!p_glGetShaderiv)
             {
                  p_glGetShaderiv = (PFNGLGETSHADERIVPROC) SF_GL_RUNTIME_LINK("glGetShaderivEXT");
                  if (!p_glGetShaderiv && true)
                      result = 0;
             }
        }

    p_glBufferSubData = (PFNGLBUFFERSUBDATAPROC) SF_GL_RUNTIME_LINK("glBufferSubData");
        if (!p_glBufferSubData)
        {
             p_glBufferSubData = (PFNGLBUFFERSUBDATAPROC) SF_GL_RUNTIME_LINK("glBufferSubDataARB");
             if (!p_glBufferSubData)
             {
                  p_glBufferSubData = (PFNGLBUFFERSUBDATAPROC) SF_GL_RUNTIME_LINK("glBufferSubDataEXT");
                  if (!p_glBufferSubData && true)
                      result = 0;
             }
        }

    p_glDeleteProgram = (PFNGLDELETEPROGRAMPROC) SF_GL_RUNTIME_LINK("glDeleteProgram");
        if (!p_glDeleteProgram)
        {
             p_glDeleteProgram = (PFNGLDELETEPROGRAMPROC) SF_GL_RUNTIME_LINK("glDeleteProgramARB");
             if (!p_glDeleteProgram)
             {
                  p_glDeleteProgram = (PFNGLDELETEPROGRAMPROC) SF_GL_RUNTIME_LINK("glDeleteProgramEXT");
                  if (!p_glDeleteProgram && true)
                      result = 0;
             }
        }

    p_glStringMarkerGREMEDY = (PFNGLSTRINGMARKERGREMEDYPROC) SF_GL_RUNTIME_LINK("glStringMarkerGREMEDY");
        if (!p_glStringMarkerGREMEDY)
        {
             p_glStringMarkerGREMEDY = (PFNGLSTRINGMARKERGREMEDYPROC) SF_GL_RUNTIME_LINK("glStringMarkerGREMEDYARB");
             if (!p_glStringMarkerGREMEDY)
             {
                  p_glStringMarkerGREMEDY = (PFNGLSTRINGMARKERGREMEDYPROC) SF_GL_RUNTIME_LINK("glStringMarkerGREMEDYEXT");
                  if (!p_glStringMarkerGREMEDY && false)
                      result = 0;
             }
        }

    p_glDeleteFramebuffersEXT = (PFNGLDELETEFRAMEBUFFERSEXTPROC) SF_GL_RUNTIME_LINK("glDeleteFramebuffersEXT");

    p_glBindFramebufferEXT = (PFNGLBINDFRAMEBUFFEREXTPROC) SF_GL_RUNTIME_LINK("glBindFramebufferEXT");

    p_glCreateProgram = (PFNGLCREATEPROGRAMPROC) SF_GL_RUNTIME_LINK("glCreateProgram");
        if (!p_glCreateProgram)
        {
             p_glCreateProgram = (PFNGLCREATEPROGRAMPROC) SF_GL_RUNTIME_LINK("glCreateProgramARB");
             if (!p_glCreateProgram)
             {
                  p_glCreateProgram = (PFNGLCREATEPROGRAMPROC) SF_GL_RUNTIME_LINK("glCreateProgramEXT");
                  if (!p_glCreateProgram && true)
                      result = 0;
             }
        }

    p_glUseProgram = (PFNGLUSEPROGRAMPROC) SF_GL_RUNTIME_LINK("glUseProgram");
        if (!p_glUseProgram)
        {
             p_glUseProgram = (PFNGLUSEPROGRAMPROC) SF_GL_RUNTIME_LINK("glUseProgramARB");
             if (!p_glUseProgram)
             {
                  p_glUseProgram = (PFNGLUSEPROGRAMPROC) SF_GL_RUNTIME_LINK("glUseProgramEXT");
                  if (!p_glUseProgram && true)
                      result = 0;
             }
        }

    p_glUniform1f = (PFNGLUNIFORM1FPROC) SF_GL_RUNTIME_LINK("glUniform1f");
        if (!p_glUniform1f)
        {
             p_glUniform1f = (PFNGLUNIFORM1FPROC) SF_GL_RUNTIME_LINK("glUniform1fARB");
             if (!p_glUniform1f)
             {
                  p_glUniform1f = (PFNGLUNIFORM1FPROC) SF_GL_RUNTIME_LINK("glUniform1fEXT");
                  if (!p_glUniform1f && true)
                      result = 0;
             }
        }

    p_glBindRenderbufferEXT = (PFNGLBINDRENDERBUFFEREXTPROC) SF_GL_RUNTIME_LINK("glBindRenderbufferEXT");

    p_glDeleteBuffers = (PFNGLDELETEBUFFERSPROC) SF_GL_RUNTIME_LINK("glDeleteBuffers");
        if (!p_glDeleteBuffers)
        {
             p_glDeleteBuffers = (PFNGLDELETEBUFFERSPROC) SF_GL_RUNTIME_LINK("glDeleteBuffersARB");
             if (!p_glDeleteBuffers)
             {
                  p_glDeleteBuffers = (PFNGLDELETEBUFFERSPROC) SF_GL_RUNTIME_LINK("glDeleteBuffersEXT");
                  if (!p_glDeleteBuffers && true)
                      result = 0;
             }
        }

    p_glDeleteShader = (PFNGLDELETESHADERPROC) SF_GL_RUNTIME_LINK("glDeleteShader");
        if (!p_glDeleteShader)
        {
             p_glDeleteShader = (PFNGLDELETESHADERPROC) SF_GL_RUNTIME_LINK("glDeleteShaderARB");
             if (!p_glDeleteShader)
             {
                  p_glDeleteShader = (PFNGLDELETESHADERPROC) SF_GL_RUNTIME_LINK("glDeleteShaderEXT");
                  if (!p_glDeleteShader && true)
                      result = 0;
             }
        }

    p_glGetProgramInfoLog = (PFNGLGETPROGRAMINFOLOGPROC) SF_GL_RUNTIME_LINK("glGetProgramInfoLog");
        if (!p_glGetProgramInfoLog)
        {
             p_glGetProgramInfoLog = (PFNGLGETPROGRAMINFOLOGPROC) SF_GL_RUNTIME_LINK("glGetProgramInfoLogARB");
             if (!p_glGetProgramInfoLog)
             {
                  p_glGetProgramInfoLog = (PFNGLGETPROGRAMINFOLOGPROC) SF_GL_RUNTIME_LINK("glGetProgramInfoLogEXT");
                  if (!p_glGetProgramInfoLog && true)
                      result = 0;
             }
        }

    p_glGenRenderbuffersEXT = (PFNGLGENRENDERBUFFERSEXTPROC) SF_GL_RUNTIME_LINK("glGenRenderbuffersEXT");

    p_glRenderbufferStorageEXT = (PFNGLRENDERBUFFERSTORAGEEXTPROC) SF_GL_RUNTIME_LINK("glRenderbufferStorageEXT");

    p_glGetFramebufferAttachmentParameterivEXT = (PFNGLGETFRAMEBUFFERATTACHMENTPARAMETERIVEXTPROC) SF_GL_RUNTIME_LINK("glGetFramebufferAttachmentParameterivEXT");

    p_glBindBuffer = (PFNGLBINDBUFFERPROC) SF_GL_RUNTIME_LINK("glBindBuffer");
        if (!p_glBindBuffer)
        {
             p_glBindBuffer = (PFNGLBINDBUFFERPROC) SF_GL_RUNTIME_LINK("glBindBufferARB");
             if (!p_glBindBuffer)
             {
                  p_glBindBuffer = (PFNGLBINDBUFFERPROC) SF_GL_RUNTIME_LINK("glBindBufferEXT");
                  if (!p_glBindBuffer && true)
                      result = 0;
             }
        }

    p_glShaderSource = (PFNGLSHADERSOURCEPROC) SF_GL_RUNTIME_LINK("glShaderSource");
        if (!p_glShaderSource)
        {
             p_glShaderSource = (PFNGLSHADERSOURCEPROC) SF_GL_RUNTIME_LINK("glShaderSourceARB");
             if (!p_glShaderSource)
             {
                  p_glShaderSource = (PFNGLSHADERSOURCEPROC) SF_GL_RUNTIME_LINK("glShaderSourceEXT");
                  if (!p_glShaderSource && true)
                      result = 0;
             }
        }

    p_glGetShaderInfoLog = (PFNGLGETSHADERINFOLOGPROC) SF_GL_RUNTIME_LINK("glGetShaderInfoLog");
        if (!p_glGetShaderInfoLog)
        {
             p_glGetShaderInfoLog = (PFNGLGETSHADERINFOLOGPROC) SF_GL_RUNTIME_LINK("glGetShaderInfoLogARB");
             if (!p_glGetShaderInfoLog)
             {
                  p_glGetShaderInfoLog = (PFNGLGETSHADERINFOLOGPROC) SF_GL_RUNTIME_LINK("glGetShaderInfoLogEXT");
                  if (!p_glGetShaderInfoLog && true)
                      result = 0;
             }
        }

    p_glUniform4fv = (PFNGLUNIFORM4FVPROC) SF_GL_RUNTIME_LINK("glUniform4fv");
        if (!p_glUniform4fv)
        {
             p_glUniform4fv = (PFNGLUNIFORM4FVPROC) SF_GL_RUNTIME_LINK("glUniform4fvARB");
             if (!p_glUniform4fv)
             {
                  p_glUniform4fv = (PFNGLUNIFORM4FVPROC) SF_GL_RUNTIME_LINK("glUniform4fvEXT");
                  if (!p_glUniform4fv && true)
                      result = 0;
             }
        }

    p_glEnableVertexAttribArray = (PFNGLENABLEVERTEXATTRIBARRAYPROC) SF_GL_RUNTIME_LINK("glEnableVertexAttribArray");
        if (!p_glEnableVertexAttribArray)
        {
             p_glEnableVertexAttribArray = (PFNGLENABLEVERTEXATTRIBARRAYPROC) SF_GL_RUNTIME_LINK("glEnableVertexAttribArrayARB");
             if (!p_glEnableVertexAttribArray)
             {
                  p_glEnableVertexAttribArray = (PFNGLENABLEVERTEXATTRIBARRAYPROC) SF_GL_RUNTIME_LINK("glEnableVertexAttribArrayEXT");
                  if (!p_glEnableVertexAttribArray && true)
                      result = 0;
             }
        }

    p_glUniformMatrix4fv = (PFNGLUNIFORMMATRIX4FVPROC) SF_GL_RUNTIME_LINK("glUniformMatrix4fv");
        if (!p_glUniformMatrix4fv)
        {
             p_glUniformMatrix4fv = (PFNGLUNIFORMMATRIX4FVPROC) SF_GL_RUNTIME_LINK("glUniformMatrix4fvARB");
             if (!p_glUniformMatrix4fv)
             {
                  p_glUniformMatrix4fv = (PFNGLUNIFORMMATRIX4FVPROC) SF_GL_RUNTIME_LINK("glUniformMatrix4fvEXT");
                  if (!p_glUniformMatrix4fv && true)
                      result = 0;
             }
        }

    p_glUnmapBuffer = (PFNGLUNMAPBUFFERPROC) SF_GL_RUNTIME_LINK("glUnmapBuffer");
        if (!p_glUnmapBuffer)
        {
             p_glUnmapBuffer = (PFNGLUNMAPBUFFERPROC) SF_GL_RUNTIME_LINK("glUnmapBufferARB");
             if (!p_glUnmapBuffer)
             {
                  p_glUnmapBuffer = (PFNGLUNMAPBUFFERPROC) SF_GL_RUNTIME_LINK("glUnmapBufferEXT");
                  if (!p_glUnmapBuffer && true)
                      result = 0;
             }
        }

    p_glFramebufferRenderbufferEXT = (PFNGLFRAMEBUFFERRENDERBUFFEREXTPROC) SF_GL_RUNTIME_LINK("glFramebufferRenderbufferEXT");

    p_glVertexAttribPointer = (PFNGLVERTEXATTRIBPOINTERPROC) SF_GL_RUNTIME_LINK("glVertexAttribPointer");
        if (!p_glVertexAttribPointer)
        {
             p_glVertexAttribPointer = (PFNGLVERTEXATTRIBPOINTERPROC) SF_GL_RUNTIME_LINK("glVertexAttribPointerARB");
             if (!p_glVertexAttribPointer)
             {
                  p_glVertexAttribPointer = (PFNGLVERTEXATTRIBPOINTERPROC) SF_GL_RUNTIME_LINK("glVertexAttribPointerEXT");
                  if (!p_glVertexAttribPointer && true)
                      result = 0;
             }
        }

    p_glUniform2f = (PFNGLUNIFORM2FPROC) SF_GL_RUNTIME_LINK("glUniform2f");
        if (!p_glUniform2f)
        {
             p_glUniform2f = (PFNGLUNIFORM2FPROC) SF_GL_RUNTIME_LINK("glUniform2fARB");
             if (!p_glUniform2f)
             {
                  p_glUniform2f = (PFNGLUNIFORM2FPROC) SF_GL_RUNTIME_LINK("glUniform2fEXT");
                  if (!p_glUniform2f && true)
                      result = 0;
             }
        }

    p_glGetUniformLocation = (PFNGLGETUNIFORMLOCATIONPROC) SF_GL_RUNTIME_LINK("glGetUniformLocation");
        if (!p_glGetUniformLocation)
        {
             p_glGetUniformLocation = (PFNGLGETUNIFORMLOCATIONPROC) SF_GL_RUNTIME_LINK("glGetUniformLocationARB");
             if (!p_glGetUniformLocation)
             {
                  p_glGetUniformLocation = (PFNGLGETUNIFORMLOCATIONPROC) SF_GL_RUNTIME_LINK("glGetUniformLocationEXT");
                  if (!p_glGetUniformLocation && true)
                      result = 0;
             }
        }

    p_glIsFramebufferEXT = (PFNGLISFRAMEBUFFEREXTPROC) SF_GL_RUNTIME_LINK("glIsFramebufferEXT");

    p_glUniform2fv = (PFNGLUNIFORM2FVPROC) SF_GL_RUNTIME_LINK("glUniform2fv");
        if (!p_glUniform2fv)
        {
             p_glUniform2fv = (PFNGLUNIFORM2FVPROC) SF_GL_RUNTIME_LINK("glUniform2fvARB");
             if (!p_glUniform2fv)
             {
                  p_glUniform2fv = (PFNGLUNIFORM2FVPROC) SF_GL_RUNTIME_LINK("glUniform2fvEXT");
                  if (!p_glUniform2fv && true)
                      result = 0;
             }
        }

    p_glUniform3fv = (PFNGLUNIFORM3FVPROC) SF_GL_RUNTIME_LINK("glUniform3fv");
        if (!p_glUniform3fv)
        {
             p_glUniform3fv = (PFNGLUNIFORM3FVPROC) SF_GL_RUNTIME_LINK("glUniform3fvARB");
             if (!p_glUniform3fv)
             {
                  p_glUniform3fv = (PFNGLUNIFORM3FVPROC) SF_GL_RUNTIME_LINK("glUniform3fvEXT");
                  if (!p_glUniform3fv && true)
                      result = 0;
             }
        }

    return result;
}

}}}

#endif
