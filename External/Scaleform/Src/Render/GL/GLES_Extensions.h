/**********************************************************************

Filename    :   GLES_Extensions.h
Content     :   GL ES extension header.
Created     :   Automatically generated.
Authors     :   Extensions.pl

Copyright   :   (c) 2001-2011 Scaleform Corp. All Rights Reserved.

Licensees may use this file in accordance with the valid Scaleform
Commercial License Agreement provided with the software.

This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING 
THE WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR ANY PURPOSE.

***********************************************************************/

#ifndef INC_SF_Render_GLES_Extensions_H
#define INC_SF_Render_GLES_Extensions_H

#include "Kernel/SF_Debug.h"
#include "Render/GL/GL_Common.h"

namespace Scaleform { namespace Render { namespace GL {

class Extensions
{
    PFNGLUNMAPBUFFEROESPROC p_glUnmapBufferOES;
    PFNGLMAPBUFFEROESPROC   p_glMapBufferOES;
public:
    bool Init();
    
    class ScopedGLErrorCheck
    {
    public:
        ScopedGLErrorCheck(const char* func) : FuncName(func)
        {
        #ifdef SF_BUILD_DEBUG
            GLenum error = glGetError(); SF_UNUSED(error);
            SF_DEBUG_ASSERT2(!error, "GL error before function %s (error code = %d)\n", FuncName, error );
        #endif
        }
        ~ScopedGLErrorCheck()
        {
        #ifdef SF_BUILD_DEBUG
            GLenum error = glGetError(); SF_UNUSED(error);
            SF_DEBUG_ASSERT2(!error, "GL error after function %s (error code = %d)\n", FuncName, error );
        #endif
        }
    private:
        const char * FuncName;
    };
    GLboolean glUnmapBufferOES(GLenum a0)
    {
        ScopedGLErrorCheck check(__FUNCTION__);
        return p_glUnmapBufferOES(a0);
    }

    void* glMapBufferOES(GLenum a0, GLenum a1)
    {
        ScopedGLErrorCheck check(__FUNCTION__);
        return p_glMapBufferOES(a0, a1);
    }

};

}}}
#endif
