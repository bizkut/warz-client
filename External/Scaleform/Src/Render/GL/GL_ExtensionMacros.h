/**********************************************************************

Filename    :   GL_ExtensionMacros.h
Content     :   GL  extension macros header.
Created     :   Automatically generated.
Authors     :   Extensions.pl

Copyright   :   (c) 2001-2011 Scaleform Corp. All Rights Reserved.

Licensees may use this file in accordance with the valid Scaleform
Commercial License Agreement provided with the software.

This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING 
THE WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR ANY PURPOSE.

***********************************************************************/

#ifndef INC_SF_Render_GL_ExtensionMacros_H
#define INC_SF_Render_GL_ExtensionMacros_H
#include "Render/GL/GL_HAL.h"

#ifdef SF_GL_RUNTIME_LINK

#define glIsRenderbufferEXT GetHAL()->glIsRenderbufferEXT
#define glGenerateMipmapEXT GetHAL()->glGenerateMipmapEXT
#define glFramebufferTexture2DEXT GetHAL()->glFramebufferTexture2DEXT
#define glDrawElementsInstanced GetHAL()->glDrawElementsInstanced
#define glMapBuffer GetHAL()->glMapBuffer
#define glGenBuffers GetHAL()->glGenBuffers
#define glCreateShader GetHAL()->glCreateShader
#define glBufferData GetHAL()->glBufferData
#define glCompressedTexImage2D GetHAL()->glCompressedTexImage2D
#define glLinkProgram GetHAL()->glLinkProgram
#define glActiveTexture GetHAL()->glActiveTexture
#define glGetRenderbufferParameterivEXT GetHAL()->glGetRenderbufferParameterivEXT
#define glGetProgramiv GetHAL()->glGetProgramiv
#define glCheckFramebufferStatusEXT GetHAL()->glCheckFramebufferStatusEXT
#define glUniform1fv GetHAL()->glUniform1fv
#define glBlendEquation GetHAL()->glBlendEquation
#define glUniform1iv GetHAL()->glUniform1iv
#define glDisableVertexAttribArray GetHAL()->glDisableVertexAttribArray
#define glBindAttribLocation GetHAL()->glBindAttribLocation
#define glCompileShader GetHAL()->glCompileShader
#define glAttachShader GetHAL()->glAttachShader
#define glGetAttribLocation GetHAL()->glGetAttribLocation
#define glBlendFuncSeparate GetHAL()->glBlendFuncSeparate
#define glGenFramebuffersEXT GetHAL()->glGenFramebuffersEXT
#define glUniform1i GetHAL()->glUniform1i
#define glDeleteRenderbuffersEXT GetHAL()->glDeleteRenderbuffersEXT
#define glGetShaderiv GetHAL()->glGetShaderiv
#define glBufferSubData GetHAL()->glBufferSubData
#define glDeleteProgram GetHAL()->glDeleteProgram
#define glStringMarkerGREMEDY GetHAL()->glStringMarkerGREMEDY
#define glDeleteFramebuffersEXT GetHAL()->glDeleteFramebuffersEXT
#define glBindFramebufferEXT GetHAL()->glBindFramebufferEXT
#define glCreateProgram GetHAL()->glCreateProgram
#define glUseProgram GetHAL()->glUseProgram
#define glUniform1f GetHAL()->glUniform1f
#define glBindRenderbufferEXT GetHAL()->glBindRenderbufferEXT
#define glDeleteBuffers GetHAL()->glDeleteBuffers
#define glDeleteShader GetHAL()->glDeleteShader
#define glGetProgramInfoLog GetHAL()->glGetProgramInfoLog
#define glGenRenderbuffersEXT GetHAL()->glGenRenderbuffersEXT
#define glRenderbufferStorageEXT GetHAL()->glRenderbufferStorageEXT
#define glGetFramebufferAttachmentParameterivEXT GetHAL()->glGetFramebufferAttachmentParameterivEXT
#define glBindBuffer GetHAL()->glBindBuffer
#define glShaderSource GetHAL()->glShaderSource
#define glGetShaderInfoLog GetHAL()->glGetShaderInfoLog
#define glUniform4fv GetHAL()->glUniform4fv
#define glEnableVertexAttribArray GetHAL()->glEnableVertexAttribArray
#define glUniformMatrix4fv GetHAL()->glUniformMatrix4fv
#define glUnmapBuffer GetHAL()->glUnmapBuffer
#define glFramebufferRenderbufferEXT GetHAL()->glFramebufferRenderbufferEXT
#define glVertexAttribPointer GetHAL()->glVertexAttribPointer
#define glUniform2f GetHAL()->glUniform2f
#define glGetUniformLocation GetHAL()->glGetUniformLocation
#define glIsFramebufferEXT GetHAL()->glIsFramebufferEXT
#define glUniform2fv GetHAL()->glUniform2fv
#define glUniform3fv GetHAL()->glUniform3fv

#endif
#endif
