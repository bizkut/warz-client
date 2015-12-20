/**************************************************************************

    PublicHeader:   Render
    Filename    :   GL_ShaderSource.cpp
    Content     :   GL Shader descriptors
    Created     :   6/27/2012
    Authors     :   Automatically generated.

    Copyright   :   Copyright 2012 Autodesk, Inc. All Rights reserved.

    Use of this software is subject to the terms of the Autodesk license
    agreement provided at the time of installation or download, or which
    otherwise accompanies this software in either electronic or hard copy form.

**************************************************************************/


namespace Scaleform { namespace Render { namespace GL {


extern const char* pSource_VTexTG;
const char* pSource_VTexTG = 
"uniform vec4 mvp[2];\n"
"uniform vec4 texgen[2];\n"
"attribute vec4 pos;\n"
"varying vec2 tc;\n"
"void main() {\n"
"gl_Position = vec4(0.0,0,0.0,1);\n"
"gl_Position.x = dot(pos, mvp[int(0.0)]);\n"
"gl_Position.y = dot(pos, mvp[int(1.0)]);\n"
"tc.x = dot(pos, texgen[int(0.0)]);\n"
"tc.y = dot(pos, texgen[int(1.0)]);\n"
"}\n";

extern const char* pSource_FTexTG;
const char* pSource_FTexTG = 
"uniform sampler2D tex;\n"
"varying vec2 tc;\n"
"void main() {\n"
"gl_FragColor = texture2D(tex,tc);\n"
"}\n";

extern const char* pSource_FTexTGMul;
const char* pSource_FTexTGMul = 
"uniform sampler2D tex;\n"
"varying vec2 tc;\n"
"void main() {\n"
"gl_FragColor = texture2D(tex,tc);\n"
"gl_FragColor = mix(vec4(1.0,1,1.0,1), gl_FragColor, gl_FragColor.a);\n"
"}\n";

extern const char* pSource_VTexTGEAlpha;
const char* pSource_VTexTGEAlpha = 
"uniform vec4 mvp[2];\n"
"uniform vec4 texgen[2];\n"
"attribute vec4 afactor;\n"
"attribute vec4 pos;\n"
"varying vec4 factor;\n"
"varying vec2 tc;\n"
"void main() {\n"
"gl_Position = vec4(0.0,0,0.0,1);\n"
"gl_Position.x = dot(pos, mvp[int(0.0)]);\n"
"gl_Position.y = dot(pos, mvp[int(1.0)]);\n"
"tc.x = dot(pos, texgen[int(0.0)]);\n"
"tc.y = dot(pos, texgen[int(1.0)]);\n"
"factor = afactor;\n"
"}\n";

extern const char* pSource_FTexTGEAlpha;
const char* pSource_FTexTGEAlpha = 
"uniform sampler2D tex;\n"
"varying vec4 factor;\n"
"varying vec2 tc;\n"
"void main() {\n"
"gl_FragColor = texture2D(tex,tc);\n"
"gl_FragColor.a *= factor.a;\n"
"}\n";

extern const char* pSource_FTexTGEAlphaMul;
const char* pSource_FTexTGEAlphaMul = 
"uniform sampler2D tex;\n"
"varying vec4 factor;\n"
"varying vec2 tc;\n"
"void main() {\n"
"gl_FragColor = texture2D(tex,tc);\n"
"gl_FragColor.a *= factor.a;\n"
"gl_FragColor = mix(vec4(1.0,1,1.0,1), gl_FragColor, gl_FragColor.a);\n"
"}\n";

extern const char* pSource_VTexTGCxform;
const char* pSource_VTexTGCxform = 
"uniform vec4 cxadd;\n"
"uniform vec4 cxmul;\n"
"uniform vec4 mvp[2];\n"
"uniform vec4 texgen[2];\n"
"attribute vec4 pos;\n"
"varying vec4 fucxadd;\n"
"varying vec4 fucxmul;\n"
"varying vec2 tc;\n"
"void main() {\n"
"gl_Position = vec4(0.0,0,0.0,1);\n"
"gl_Position.x = dot(pos, mvp[int(0.0)]);\n"
"gl_Position.y = dot(pos, mvp[int(1.0)]);\n"
"tc.x = dot(pos, texgen[int(0.0)]);\n"
"tc.y = dot(pos, texgen[int(1.0)]);\n"
"fucxadd = cxadd;\n"
"fucxmul = cxmul;\n"
"}\n";

extern const char* pSource_FTexTGCxform;
const char* pSource_FTexTGCxform = 
"uniform sampler2D tex;\n"
"varying vec4 fucxadd;\n"
"varying vec4 fucxmul;\n"
"varying vec2 tc;\n"
"void main() {\n"
"gl_FragColor = texture2D(tex,tc);\n"
"gl_FragColor = gl_FragColor * fucxmul + fucxadd;\n"
"}\n";

extern const char* pSource_FTexTGCxformMul;
const char* pSource_FTexTGCxformMul = 
"uniform sampler2D tex;\n"
"varying vec4 fucxadd;\n"
"varying vec4 fucxmul;\n"
"varying vec2 tc;\n"
"void main() {\n"
"gl_FragColor = texture2D(tex,tc);\n"
"gl_FragColor = gl_FragColor * fucxmul + fucxadd;\n"
"gl_FragColor = mix(vec4(1.0,1,1.0,1), gl_FragColor, gl_FragColor.a);\n"
"}\n";

extern const char* pSource_VTexTGCxformEAlpha;
const char* pSource_VTexTGCxformEAlpha = 
"uniform vec4 cxadd;\n"
"uniform vec4 cxmul;\n"
"uniform vec4 mvp[2];\n"
"uniform vec4 texgen[2];\n"
"attribute vec4 afactor;\n"
"attribute vec4 pos;\n"
"varying vec4 factor;\n"
"varying vec4 fucxadd;\n"
"varying vec4 fucxmul;\n"
"varying vec2 tc;\n"
"void main() {\n"
"gl_Position = vec4(0.0,0,0.0,1);\n"
"gl_Position.x = dot(pos, mvp[int(0.0)]);\n"
"gl_Position.y = dot(pos, mvp[int(1.0)]);\n"
"tc.x = dot(pos, texgen[int(0.0)]);\n"
"tc.y = dot(pos, texgen[int(1.0)]);\n"
"fucxadd = cxadd;\n"
"fucxmul = cxmul;\n"
"factor = afactor;\n"
"}\n";

extern const char* pSource_FTexTGCxformEAlpha;
const char* pSource_FTexTGCxformEAlpha = 
"uniform sampler2D tex;\n"
"varying vec4 factor;\n"
"varying vec4 fucxadd;\n"
"varying vec4 fucxmul;\n"
"varying vec2 tc;\n"
"void main() {\n"
"gl_FragColor = texture2D(tex,tc);\n"
"gl_FragColor = gl_FragColor * fucxmul + fucxadd;\n"
"gl_FragColor.a *= factor.a;\n"
"}\n";

extern const char* pSource_FTexTGCxformEAlphaMul;
const char* pSource_FTexTGCxformEAlphaMul = 
"uniform sampler2D tex;\n"
"varying vec4 factor;\n"
"varying vec4 fucxadd;\n"
"varying vec4 fucxmul;\n"
"varying vec2 tc;\n"
"void main() {\n"
"gl_FragColor = texture2D(tex,tc);\n"
"gl_FragColor = gl_FragColor * fucxmul + fucxadd;\n"
"gl_FragColor.a *= factor.a;\n"
"gl_FragColor = mix(vec4(1.0,1,1.0,1), gl_FragColor, gl_FragColor.a);\n"
"}\n";

extern const char* pSource_VVertex;
const char* pSource_VVertex = 
"uniform vec4 mvp[2];\n"
"attribute vec4 acolor;\n"
"attribute vec4 pos;\n"
"varying vec4 color;\n"
"void main() {\n"
"gl_Position = vec4(0.0,0,0.0,1);\n"
"gl_Position.x = dot(pos, mvp[int(0.0)]);\n"
"gl_Position.y = dot(pos, mvp[int(1.0)]);\n"
"color = acolor;\n"
"}\n";

extern const char* pSource_FVertex;
const char* pSource_FVertex = 
"varying vec4 color;\n"
"void main() {\n"
"gl_FragColor = color;\n"
"}\n";

extern const char* pSource_FVertexMul;
const char* pSource_FVertexMul = 
"varying vec4 color;\n"
"void main() {\n"
"gl_FragColor = color;\n"
"gl_FragColor = mix(vec4(1.0,1,1.0,1), gl_FragColor, gl_FragColor.a);\n"
"}\n";

extern const char* pSource_VVertexEAlpha;
const char* pSource_VVertexEAlpha = 
"uniform vec4 mvp[2];\n"
"attribute vec4 acolor;\n"
"attribute vec4 afactor;\n"
"attribute vec4 pos;\n"
"varying vec4 color;\n"
"varying vec4 factor;\n"
"void main() {\n"
"gl_Position = vec4(0.0,0,0.0,1);\n"
"gl_Position.x = dot(pos, mvp[int(0.0)]);\n"
"gl_Position.y = dot(pos, mvp[int(1.0)]);\n"
"color = acolor;\n"
"factor = afactor;\n"
"}\n";

extern const char* pSource_FVertexEAlpha;
const char* pSource_FVertexEAlpha = 
"varying vec4 color;\n"
"varying vec4 factor;\n"
"void main() {\n"
"gl_FragColor = color;\n"
"gl_FragColor.a *= factor.a;\n"
"}\n";

extern const char* pSource_FVertexEAlphaMul;
const char* pSource_FVertexEAlphaMul = 
"varying vec4 color;\n"
"varying vec4 factor;\n"
"void main() {\n"
"gl_FragColor = color;\n"
"gl_FragColor.a *= factor.a;\n"
"gl_FragColor = mix(vec4(1.0,1,1.0,1), gl_FragColor, gl_FragColor.a);\n"
"}\n";

extern const char* pSource_VVertexCxform;
const char* pSource_VVertexCxform = 
"uniform vec4 cxadd;\n"
"uniform vec4 cxmul;\n"
"uniform vec4 mvp[2];\n"
"attribute vec4 acolor;\n"
"attribute vec4 pos;\n"
"varying vec4 color;\n"
"varying vec4 fucxadd;\n"
"varying vec4 fucxmul;\n"
"void main() {\n"
"gl_Position = vec4(0.0,0,0.0,1);\n"
"gl_Position.x = dot(pos, mvp[int(0.0)]);\n"
"gl_Position.y = dot(pos, mvp[int(1.0)]);\n"
"color = acolor;\n"
"fucxadd = cxadd;\n"
"fucxmul = cxmul;\n"
"}\n";

extern const char* pSource_FVertexCxform;
const char* pSource_FVertexCxform = 
"varying vec4 color;\n"
"varying vec4 fucxadd;\n"
"varying vec4 fucxmul;\n"
"void main() {\n"
"gl_FragColor = color;\n"
"gl_FragColor = gl_FragColor * fucxmul + fucxadd;\n"
"}\n";

extern const char* pSource_FVertexCxformMul;
const char* pSource_FVertexCxformMul = 
"varying vec4 color;\n"
"varying vec4 fucxadd;\n"
"varying vec4 fucxmul;\n"
"void main() {\n"
"gl_FragColor = color;\n"
"gl_FragColor = gl_FragColor * fucxmul + fucxadd;\n"
"gl_FragColor = mix(vec4(1.0,1,1.0,1), gl_FragColor, gl_FragColor.a);\n"
"}\n";

extern const char* pSource_VVertexCxformEAlpha;
const char* pSource_VVertexCxformEAlpha = 
"uniform vec4 cxadd;\n"
"uniform vec4 cxmul;\n"
"uniform vec4 mvp[2];\n"
"attribute vec4 acolor;\n"
"attribute vec4 afactor;\n"
"attribute vec4 pos;\n"
"varying vec4 color;\n"
"varying vec4 factor;\n"
"varying vec4 fucxadd;\n"
"varying vec4 fucxmul;\n"
"void main() {\n"
"gl_Position = vec4(0.0,0,0.0,1);\n"
"gl_Position.x = dot(pos, mvp[int(0.0)]);\n"
"gl_Position.y = dot(pos, mvp[int(1.0)]);\n"
"color = acolor;\n"
"fucxadd = cxadd;\n"
"fucxmul = cxmul;\n"
"factor = afactor;\n"
"}\n";

extern const char* pSource_FVertexCxformEAlpha;
const char* pSource_FVertexCxformEAlpha = 
"varying vec4 color;\n"
"varying vec4 factor;\n"
"varying vec4 fucxadd;\n"
"varying vec4 fucxmul;\n"
"void main() {\n"
"gl_FragColor = color;\n"
"gl_FragColor = gl_FragColor * fucxmul + fucxadd;\n"
"gl_FragColor.a *= factor.a;\n"
"}\n";

extern const char* pSource_FVertexCxformEAlphaMul;
const char* pSource_FVertexCxformEAlphaMul = 
"varying vec4 color;\n"
"varying vec4 factor;\n"
"varying vec4 fucxadd;\n"
"varying vec4 fucxmul;\n"
"void main() {\n"
"gl_FragColor = color;\n"
"gl_FragColor = gl_FragColor * fucxmul + fucxadd;\n"
"gl_FragColor.a *= factor.a;\n"
"gl_FragColor = mix(vec4(1.0,1,1.0,1), gl_FragColor, gl_FragColor.a);\n"
"}\n";

extern const char* pSource_VTexTGTexTG;
const char* pSource_VTexTGTexTG = 
"uniform vec4 mvp[2];\n"
"uniform vec4 texgen[4];\n"
"attribute vec4 afactor;\n"
"attribute vec4 pos;\n"
"varying vec4 factor;\n"
"varying vec2 tc0;\n"
"varying vec2 tc1;\n"
"void main() {\n"
"gl_Position = vec4(0.0,0,0.0,1);\n"
"gl_Position.x = dot(pos, mvp[int(0.0)]);\n"
"gl_Position.y = dot(pos, mvp[int(1.0)]);\n"
"tc0.x = dot(pos, texgen[int(0.0)]);\n"
"tc0.y = dot(pos, texgen[int(1.0)]);\n"
"tc1.x = dot(pos, texgen[int(2.0)]);\n"
"tc1.y = dot(pos, texgen[int(3.0)]);\n"
"factor = afactor;\n"
"}\n";

extern const char* pSource_FTexTGTexTG;
const char* pSource_FTexTGTexTG = 
"uniform sampler2D tex[2];\n"
"varying vec4 factor;\n"
"varying vec2 tc0;\n"
"varying vec2 tc1;\n"
"void main() {\n"
"vec4 fcolor0 = texture2D(tex[int(0.0)], tc0);\n"
"vec4 fcolor1 = texture2D(tex[int(1.0)], tc1);\n"
"gl_FragColor = mix(fcolor1, fcolor0, factor.r);\n"
"}\n";

extern const char* pSource_FTexTGTexTGMul;
const char* pSource_FTexTGTexTGMul = 
"uniform sampler2D tex[2];\n"
"varying vec4 factor;\n"
"varying vec2 tc0;\n"
"varying vec2 tc1;\n"
"void main() {\n"
"vec4 fcolor0 = texture2D(tex[int(0.0)], tc0);\n"
"vec4 fcolor1 = texture2D(tex[int(1.0)], tc1);\n"
"gl_FragColor = mix(fcolor1, fcolor0, factor.r);\n"
"gl_FragColor = mix(vec4(1.0,1,1.0,1), gl_FragColor, gl_FragColor.a);\n"
"}\n";

extern const char* pSource_FTexTGTexTGEAlpha;
const char* pSource_FTexTGTexTGEAlpha = 
"uniform sampler2D tex[2];\n"
"varying vec4 factor;\n"
"varying vec2 tc0;\n"
"varying vec2 tc1;\n"
"void main() {\n"
"vec4 fcolor0 = texture2D(tex[int(0.0)], tc0);\n"
"vec4 fcolor1 = texture2D(tex[int(1.0)], tc1);\n"
"gl_FragColor = mix(fcolor1, fcolor0, factor.r);\n"
"gl_FragColor.a *= factor.a;\n"
"}\n";

extern const char* pSource_FTexTGTexTGEAlphaMul;
const char* pSource_FTexTGTexTGEAlphaMul = 
"uniform sampler2D tex[2];\n"
"varying vec4 factor;\n"
"varying vec2 tc0;\n"
"varying vec2 tc1;\n"
"void main() {\n"
"vec4 fcolor0 = texture2D(tex[int(0.0)], tc0);\n"
"vec4 fcolor1 = texture2D(tex[int(1.0)], tc1);\n"
"gl_FragColor = mix(fcolor1, fcolor0, factor.r);\n"
"gl_FragColor.a *= factor.a;\n"
"gl_FragColor = mix(vec4(1.0,1,1.0,1), gl_FragColor, gl_FragColor.a);\n"
"}\n";

extern const char* pSource_VTexTGTexTGCxform;
const char* pSource_VTexTGTexTGCxform = 
"uniform vec4 cxadd;\n"
"uniform vec4 cxmul;\n"
"uniform vec4 mvp[2];\n"
"uniform vec4 texgen[4];\n"
"attribute vec4 afactor;\n"
"attribute vec4 pos;\n"
"varying vec4 factor;\n"
"varying vec4 fucxadd;\n"
"varying vec4 fucxmul;\n"
"varying vec2 tc0;\n"
"varying vec2 tc1;\n"
"void main() {\n"
"gl_Position = vec4(0.0,0,0.0,1);\n"
"gl_Position.x = dot(pos, mvp[int(0.0)]);\n"
"gl_Position.y = dot(pos, mvp[int(1.0)]);\n"
"tc0.x = dot(pos, texgen[int(0.0)]);\n"
"tc0.y = dot(pos, texgen[int(1.0)]);\n"
"tc1.x = dot(pos, texgen[int(2.0)]);\n"
"tc1.y = dot(pos, texgen[int(3.0)]);\n"
"fucxadd = cxadd;\n"
"fucxmul = cxmul;\n"
"factor = afactor;\n"
"}\n";

extern const char* pSource_FTexTGTexTGCxform;
const char* pSource_FTexTGTexTGCxform = 
"uniform sampler2D tex[2];\n"
"varying vec4 factor;\n"
"varying vec4 fucxadd;\n"
"varying vec4 fucxmul;\n"
"varying vec2 tc0;\n"
"varying vec2 tc1;\n"
"void main() {\n"
"vec4 fcolor0 = texture2D(tex[int(0.0)], tc0);\n"
"vec4 fcolor1 = texture2D(tex[int(1.0)], tc1);\n"
"gl_FragColor = mix(fcolor1, fcolor0, factor.r);\n"
"gl_FragColor = gl_FragColor * fucxmul + fucxadd;\n"
"}\n";

extern const char* pSource_FTexTGTexTGCxformMul;
const char* pSource_FTexTGTexTGCxformMul = 
"uniform sampler2D tex[2];\n"
"varying vec4 factor;\n"
"varying vec4 fucxadd;\n"
"varying vec4 fucxmul;\n"
"varying vec2 tc0;\n"
"varying vec2 tc1;\n"
"void main() {\n"
"vec4 fcolor0 = texture2D(tex[int(0.0)], tc0);\n"
"vec4 fcolor1 = texture2D(tex[int(1.0)], tc1);\n"
"gl_FragColor = mix(fcolor1, fcolor0, factor.r);\n"
"gl_FragColor = gl_FragColor * fucxmul + fucxadd;\n"
"gl_FragColor = mix(vec4(1.0,1,1.0,1), gl_FragColor, gl_FragColor.a);\n"
"}\n";

extern const char* pSource_FTexTGTexTGCxformEAlpha;
const char* pSource_FTexTGTexTGCxformEAlpha = 
"uniform sampler2D tex[2];\n"
"varying vec4 factor;\n"
"varying vec4 fucxadd;\n"
"varying vec4 fucxmul;\n"
"varying vec2 tc0;\n"
"varying vec2 tc1;\n"
"void main() {\n"
"vec4 fcolor0 = texture2D(tex[int(0.0)], tc0);\n"
"vec4 fcolor1 = texture2D(tex[int(1.0)], tc1);\n"
"gl_FragColor = mix(fcolor1, fcolor0, factor.r);\n"
"gl_FragColor = gl_FragColor * fucxmul + fucxadd;\n"
"gl_FragColor.a *= factor.a;\n"
"}\n";

extern const char* pSource_FTexTGTexTGCxformEAlphaMul;
const char* pSource_FTexTGTexTGCxformEAlphaMul = 
"uniform sampler2D tex[2];\n"
"varying vec4 factor;\n"
"varying vec4 fucxadd;\n"
"varying vec4 fucxmul;\n"
"varying vec2 tc0;\n"
"varying vec2 tc1;\n"
"void main() {\n"
"vec4 fcolor0 = texture2D(tex[int(0.0)], tc0);\n"
"vec4 fcolor1 = texture2D(tex[int(1.0)], tc1);\n"
"gl_FragColor = mix(fcolor1, fcolor0, factor.r);\n"
"gl_FragColor = gl_FragColor * fucxmul + fucxadd;\n"
"gl_FragColor.a *= factor.a;\n"
"gl_FragColor = mix(vec4(1.0,1,1.0,1), gl_FragColor, gl_FragColor.a);\n"
"}\n";

extern const char* pSource_VTexTGVertex;
const char* pSource_VTexTGVertex = 
"uniform vec4 mvp[2];\n"
"uniform vec4 texgen[2];\n"
"attribute vec4 acolor;\n"
"attribute vec4 afactor;\n"
"attribute vec4 pos;\n"
"varying vec4 color;\n"
"varying vec4 factor;\n"
"varying vec2 tc;\n"
"void main() {\n"
"gl_Position = vec4(0.0,0,0.0,1);\n"
"gl_Position.x = dot(pos, mvp[int(0.0)]);\n"
"gl_Position.y = dot(pos, mvp[int(1.0)]);\n"
"color = acolor;\n"
"tc.x = dot(pos, texgen[int(0.0)]);\n"
"tc.y = dot(pos, texgen[int(1.0)]);\n"
"factor = afactor;\n"
"}\n";

extern const char* pSource_FTexTGVertex;
const char* pSource_FTexTGVertex = 
"uniform sampler2D tex;\n"
"varying vec4 color;\n"
"varying vec4 factor;\n"
"varying vec2 tc;\n"
"void main() {\n"
"vec4 fcolor0 = texture2D(tex,tc);\n"
"vec4 fcolor1 = color;\n"
"gl_FragColor = mix(fcolor1, fcolor0, factor.r);\n"
"}\n";

extern const char* pSource_FTexTGVertexMul;
const char* pSource_FTexTGVertexMul = 
"uniform sampler2D tex;\n"
"varying vec4 color;\n"
"varying vec4 factor;\n"
"varying vec2 tc;\n"
"void main() {\n"
"vec4 fcolor0 = texture2D(tex,tc);\n"
"vec4 fcolor1 = color;\n"
"gl_FragColor = mix(fcolor1, fcolor0, factor.r);\n"
"gl_FragColor = mix(vec4(1.0,1,1.0,1), gl_FragColor, gl_FragColor.a);\n"
"}\n";

extern const char* pSource_FTexTGVertexEAlpha;
const char* pSource_FTexTGVertexEAlpha = 
"uniform sampler2D tex;\n"
"varying vec4 color;\n"
"varying vec4 factor;\n"
"varying vec2 tc;\n"
"void main() {\n"
"vec4 fcolor0 = texture2D(tex,tc);\n"
"vec4 fcolor1 = color;\n"
"gl_FragColor = mix(fcolor1, fcolor0, factor.r);\n"
"gl_FragColor.a *= factor.a;\n"
"}\n";

extern const char* pSource_FTexTGVertexEAlphaMul;
const char* pSource_FTexTGVertexEAlphaMul = 
"uniform sampler2D tex;\n"
"varying vec4 color;\n"
"varying vec4 factor;\n"
"varying vec2 tc;\n"
"void main() {\n"
"vec4 fcolor0 = texture2D(tex,tc);\n"
"vec4 fcolor1 = color;\n"
"gl_FragColor = mix(fcolor1, fcolor0, factor.r);\n"
"gl_FragColor.a *= factor.a;\n"
"gl_FragColor = mix(vec4(1.0,1,1.0,1), gl_FragColor, gl_FragColor.a);\n"
"}\n";

extern const char* pSource_VTexTGVertexCxform;
const char* pSource_VTexTGVertexCxform = 
"uniform vec4 cxadd;\n"
"uniform vec4 cxmul;\n"
"uniform vec4 mvp[2];\n"
"uniform vec4 texgen[2];\n"
"attribute vec4 acolor;\n"
"attribute vec4 afactor;\n"
"attribute vec4 pos;\n"
"varying vec4 color;\n"
"varying vec4 factor;\n"
"varying vec4 fucxadd;\n"
"varying vec4 fucxmul;\n"
"varying vec2 tc;\n"
"void main() {\n"
"gl_Position = vec4(0.0,0,0.0,1);\n"
"gl_Position.x = dot(pos, mvp[int(0.0)]);\n"
"gl_Position.y = dot(pos, mvp[int(1.0)]);\n"
"color = acolor;\n"
"tc.x = dot(pos, texgen[int(0.0)]);\n"
"tc.y = dot(pos, texgen[int(1.0)]);\n"
"fucxadd = cxadd;\n"
"fucxmul = cxmul;\n"
"factor = afactor;\n"
"}\n";

extern const char* pSource_FTexTGVertexCxform;
const char* pSource_FTexTGVertexCxform = 
"uniform sampler2D tex;\n"
"varying vec4 color;\n"
"varying vec4 factor;\n"
"varying vec4 fucxadd;\n"
"varying vec4 fucxmul;\n"
"varying vec2 tc;\n"
"void main() {\n"
"vec4 fcolor0 = texture2D(tex,tc);\n"
"vec4 fcolor1 = color;\n"
"gl_FragColor = mix(fcolor1, fcolor0, factor.r);\n"
"gl_FragColor = gl_FragColor * fucxmul + fucxadd;\n"
"}\n";

extern const char* pSource_FTexTGVertexCxformMul;
const char* pSource_FTexTGVertexCxformMul = 
"uniform sampler2D tex;\n"
"varying vec4 color;\n"
"varying vec4 factor;\n"
"varying vec4 fucxadd;\n"
"varying vec4 fucxmul;\n"
"varying vec2 tc;\n"
"void main() {\n"
"vec4 fcolor0 = texture2D(tex,tc);\n"
"vec4 fcolor1 = color;\n"
"gl_FragColor = mix(fcolor1, fcolor0, factor.r);\n"
"gl_FragColor = gl_FragColor * fucxmul + fucxadd;\n"
"gl_FragColor = mix(vec4(1.0,1,1.0,1), gl_FragColor, gl_FragColor.a);\n"
"}\n";

extern const char* pSource_FTexTGVertexCxformEAlpha;
const char* pSource_FTexTGVertexCxformEAlpha = 
"uniform sampler2D tex;\n"
"varying vec4 color;\n"
"varying vec4 factor;\n"
"varying vec4 fucxadd;\n"
"varying vec4 fucxmul;\n"
"varying vec2 tc;\n"
"void main() {\n"
"vec4 fcolor0 = texture2D(tex,tc);\n"
"vec4 fcolor1 = color;\n"
"gl_FragColor = mix(fcolor1, fcolor0, factor.r);\n"
"gl_FragColor = gl_FragColor * fucxmul + fucxadd;\n"
"gl_FragColor.a *= factor.a;\n"
"}\n";

extern const char* pSource_FTexTGVertexCxformEAlphaMul;
const char* pSource_FTexTGVertexCxformEAlphaMul = 
"uniform sampler2D tex;\n"
"varying vec4 color;\n"
"varying vec4 factor;\n"
"varying vec4 fucxadd;\n"
"varying vec4 fucxmul;\n"
"varying vec2 tc;\n"
"void main() {\n"
"vec4 fcolor0 = texture2D(tex,tc);\n"
"vec4 fcolor1 = color;\n"
"gl_FragColor = mix(fcolor1, fcolor0, factor.r);\n"
"gl_FragColor = gl_FragColor * fucxmul + fucxadd;\n"
"gl_FragColor.a *= factor.a;\n"
"gl_FragColor = mix(vec4(1.0,1,1.0,1), gl_FragColor, gl_FragColor.a);\n"
"}\n";

extern const char* pSource_VPosition3dTexTG;
const char* pSource_VPosition3dTexTG = 
"uniform mat4 mvp;\n"
"uniform vec4 texgen[2];\n"
"attribute vec4 pos;\n"
"varying vec2 tc;\n"
"void main() {\n"
"gl_Position = (pos) * ( mvp);\n"
"tc.x = dot(pos, texgen[int(0.0)]);\n"
"tc.y = dot(pos, texgen[int(1.0)]);\n"
"}\n";

extern const char* pSource_VPosition3dTexTGEAlpha;
const char* pSource_VPosition3dTexTGEAlpha = 
"uniform mat4 mvp;\n"
"uniform vec4 texgen[2];\n"
"attribute vec4 afactor;\n"
"attribute vec4 pos;\n"
"varying vec4 factor;\n"
"varying vec2 tc;\n"
"void main() {\n"
"gl_Position = (pos) * ( mvp);\n"
"tc.x = dot(pos, texgen[int(0.0)]);\n"
"tc.y = dot(pos, texgen[int(1.0)]);\n"
"factor = afactor;\n"
"}\n";

extern const char* pSource_VPosition3dTexTGCxform;
const char* pSource_VPosition3dTexTGCxform = 
"uniform vec4 cxadd;\n"
"uniform vec4 cxmul;\n"
"uniform mat4 mvp;\n"
"uniform vec4 texgen[2];\n"
"attribute vec4 pos;\n"
"varying vec4 fucxadd;\n"
"varying vec4 fucxmul;\n"
"varying vec2 tc;\n"
"void main() {\n"
"gl_Position = (pos) * ( mvp);\n"
"tc.x = dot(pos, texgen[int(0.0)]);\n"
"tc.y = dot(pos, texgen[int(1.0)]);\n"
"fucxadd = cxadd;\n"
"fucxmul = cxmul;\n"
"}\n";

extern const char* pSource_VPosition3dTexTGCxformEAlpha;
const char* pSource_VPosition3dTexTGCxformEAlpha = 
"uniform vec4 cxadd;\n"
"uniform vec4 cxmul;\n"
"uniform mat4 mvp;\n"
"uniform vec4 texgen[2];\n"
"attribute vec4 afactor;\n"
"attribute vec4 pos;\n"
"varying vec4 factor;\n"
"varying vec4 fucxadd;\n"
"varying vec4 fucxmul;\n"
"varying vec2 tc;\n"
"void main() {\n"
"gl_Position = (pos) * ( mvp);\n"
"tc.x = dot(pos, texgen[int(0.0)]);\n"
"tc.y = dot(pos, texgen[int(1.0)]);\n"
"fucxadd = cxadd;\n"
"fucxmul = cxmul;\n"
"factor = afactor;\n"
"}\n";

extern const char* pSource_VPosition3dVertex;
const char* pSource_VPosition3dVertex = 
"uniform mat4 mvp;\n"
"attribute vec4 acolor;\n"
"attribute vec4 pos;\n"
"varying vec4 color;\n"
"void main() {\n"
"gl_Position = (pos) * ( mvp);\n"
"color = acolor;\n"
"}\n";

extern const char* pSource_VPosition3dVertexEAlpha;
const char* pSource_VPosition3dVertexEAlpha = 
"uniform mat4 mvp;\n"
"attribute vec4 acolor;\n"
"attribute vec4 afactor;\n"
"attribute vec4 pos;\n"
"varying vec4 color;\n"
"varying vec4 factor;\n"
"void main() {\n"
"gl_Position = (pos) * ( mvp);\n"
"color = acolor;\n"
"factor = afactor;\n"
"}\n";

extern const char* pSource_VPosition3dVertexCxform;
const char* pSource_VPosition3dVertexCxform = 
"uniform vec4 cxadd;\n"
"uniform vec4 cxmul;\n"
"uniform mat4 mvp;\n"
"attribute vec4 acolor;\n"
"attribute vec4 pos;\n"
"varying vec4 color;\n"
"varying vec4 fucxadd;\n"
"varying vec4 fucxmul;\n"
"void main() {\n"
"gl_Position = (pos) * ( mvp);\n"
"color = acolor;\n"
"fucxadd = cxadd;\n"
"fucxmul = cxmul;\n"
"}\n";

extern const char* pSource_VPosition3dVertexCxformEAlpha;
const char* pSource_VPosition3dVertexCxformEAlpha = 
"uniform vec4 cxadd;\n"
"uniform vec4 cxmul;\n"
"uniform mat4 mvp;\n"
"attribute vec4 acolor;\n"
"attribute vec4 afactor;\n"
"attribute vec4 pos;\n"
"varying vec4 color;\n"
"varying vec4 factor;\n"
"varying vec4 fucxadd;\n"
"varying vec4 fucxmul;\n"
"void main() {\n"
"gl_Position = (pos) * ( mvp);\n"
"color = acolor;\n"
"fucxadd = cxadd;\n"
"fucxmul = cxmul;\n"
"factor = afactor;\n"
"}\n";

extern const char* pSource_VPosition3dTexTGTexTG;
const char* pSource_VPosition3dTexTGTexTG = 
"uniform mat4 mvp;\n"
"uniform vec4 texgen[4];\n"
"attribute vec4 afactor;\n"
"attribute vec4 pos;\n"
"varying vec4 factor;\n"
"varying vec2 tc0;\n"
"varying vec2 tc1;\n"
"void main() {\n"
"gl_Position = (pos) * ( mvp);\n"
"tc0.x = dot(pos, texgen[int(0.0)]);\n"
"tc0.y = dot(pos, texgen[int(1.0)]);\n"
"tc1.x = dot(pos, texgen[int(2.0)]);\n"
"tc1.y = dot(pos, texgen[int(3.0)]);\n"
"factor = afactor;\n"
"}\n";

extern const char* pSource_VPosition3dTexTGTexTGCxform;
const char* pSource_VPosition3dTexTGTexTGCxform = 
"uniform vec4 cxadd;\n"
"uniform vec4 cxmul;\n"
"uniform mat4 mvp;\n"
"uniform vec4 texgen[4];\n"
"attribute vec4 afactor;\n"
"attribute vec4 pos;\n"
"varying vec4 factor;\n"
"varying vec4 fucxadd;\n"
"varying vec4 fucxmul;\n"
"varying vec2 tc0;\n"
"varying vec2 tc1;\n"
"void main() {\n"
"gl_Position = (pos) * ( mvp);\n"
"tc0.x = dot(pos, texgen[int(0.0)]);\n"
"tc0.y = dot(pos, texgen[int(1.0)]);\n"
"tc1.x = dot(pos, texgen[int(2.0)]);\n"
"tc1.y = dot(pos, texgen[int(3.0)]);\n"
"fucxadd = cxadd;\n"
"fucxmul = cxmul;\n"
"factor = afactor;\n"
"}\n";

extern const char* pSource_VPosition3dTexTGVertex;
const char* pSource_VPosition3dTexTGVertex = 
"uniform mat4 mvp;\n"
"uniform vec4 texgen[2];\n"
"attribute vec4 acolor;\n"
"attribute vec4 afactor;\n"
"attribute vec4 pos;\n"
"varying vec4 color;\n"
"varying vec4 factor;\n"
"varying vec2 tc;\n"
"void main() {\n"
"gl_Position = (pos) * ( mvp);\n"
"color = acolor;\n"
"tc.x = dot(pos, texgen[int(0.0)]);\n"
"tc.y = dot(pos, texgen[int(1.0)]);\n"
"factor = afactor;\n"
"}\n";

extern const char* pSource_VPosition3dTexTGVertexCxform;
const char* pSource_VPosition3dTexTGVertexCxform = 
"uniform vec4 cxadd;\n"
"uniform vec4 cxmul;\n"
"uniform mat4 mvp;\n"
"uniform vec4 texgen[2];\n"
"attribute vec4 acolor;\n"
"attribute vec4 afactor;\n"
"attribute vec4 pos;\n"
"varying vec4 color;\n"
"varying vec4 factor;\n"
"varying vec4 fucxadd;\n"
"varying vec4 fucxmul;\n"
"varying vec2 tc;\n"
"void main() {\n"
"gl_Position = (pos) * ( mvp);\n"
"color = acolor;\n"
"tc.x = dot(pos, texgen[int(0.0)]);\n"
"tc.y = dot(pos, texgen[int(1.0)]);\n"
"fucxadd = cxadd;\n"
"fucxmul = cxmul;\n"
"factor = afactor;\n"
"}\n";

extern const char* pSource_VBatchTexTG;
const char* pSource_VBatchTexTG = 
"uniform vec4 vfuniforms[96];\n"
"attribute vec4 pos;\n"
"attribute float vbatch;\n"
"varying vec2 tc;\n"
"void main() {\n"
"gl_Position = vec4(0.0,0,0.0,1);\n"
"gl_Position.x = dot(pos, vfuniforms[int(vbatch * 4.0 + 0.0+ 0.1 + 0.0)]);\n"
"gl_Position.y = dot(pos, vfuniforms[int(vbatch * 4.0 + 0.0+ 0.1 + 1.0)]);\n"
"tc.x = dot(pos, vfuniforms[int(vbatch * 4.0 + 2.0+ 0.1 + 0.0)]);\n"
"tc.y = dot(pos, vfuniforms[int(vbatch * 4.0 + 2.0+ 0.1 + 1.0)]);\n"
"}\n";

extern const char* pSource_VBatchTexTGEAlpha;
const char* pSource_VBatchTexTGEAlpha = 
"uniform vec4 vfuniforms[96];\n"
"attribute vec4 afactor;\n"
"attribute vec4 pos;\n"
"varying vec4 factor;\n"
"varying vec2 tc;\n"
"void main() {\n"
"gl_Position = vec4(0.0,0,0.0,1);\n"
"gl_Position.x = dot(pos, vfuniforms[int(afactor.b*255.01 * 4.0 + 0.0+ 0.1 + 0.0)]);\n"
"gl_Position.y = dot(pos, vfuniforms[int(afactor.b*255.01 * 4.0 + 0.0+ 0.1 + 1.0)]);\n"
"tc.x = dot(pos, vfuniforms[int(afactor.b*255.01 * 4.0 + 2.0+ 0.1 + 0.0)]);\n"
"tc.y = dot(pos, vfuniforms[int(afactor.b*255.01 * 4.0 + 2.0+ 0.1 + 1.0)]);\n"
"factor = afactor;\n"
"}\n";

extern const char* pSource_VBatchTexTGCxform;
const char* pSource_VBatchTexTGCxform = 
"uniform vec4 vfuniforms[144];\n"
"attribute vec4 pos;\n"
"attribute float vbatch;\n"
"varying vec4 fucxadd;\n"
"varying vec4 fucxmul;\n"
"varying vec2 tc;\n"
"void main() {\n"
"gl_Position = vec4(0.0,0,0.0,1);\n"
"gl_Position.x = dot(pos, vfuniforms[int(vbatch * 6.0 + 2.0+ 0.1 + 0.0)]);\n"
"gl_Position.y = dot(pos, vfuniforms[int(vbatch * 6.0 + 2.0+ 0.1 + 1.0)]);\n"
"tc.x = dot(pos, vfuniforms[int(vbatch * 6.0 + 4.0+ 0.1 + 0.0)]);\n"
"tc.y = dot(pos, vfuniforms[int(vbatch * 6.0 + 4.0+ 0.1 + 1.0)]);\n"
"fucxadd = vfuniforms[int(vbatch * 6.0 + 0.0+ 0.1)];\n"
"fucxmul = vfuniforms[int(vbatch * 6.0 + 1.0+ 0.1)];\n"
"}\n";

extern const char* pSource_VBatchTexTGCxformEAlpha;
const char* pSource_VBatchTexTGCxformEAlpha = 
"uniform vec4 vfuniforms[144];\n"
"attribute vec4 afactor;\n"
"attribute vec4 pos;\n"
"varying vec4 factor;\n"
"varying vec4 fucxadd;\n"
"varying vec4 fucxmul;\n"
"varying vec2 tc;\n"
"void main() {\n"
"gl_Position = vec4(0.0,0,0.0,1);\n"
"gl_Position.x = dot(pos, vfuniforms[int(afactor.b*255.01 * 6.0 + 2.0+ 0.1 + 0.0)]);\n"
"gl_Position.y = dot(pos, vfuniforms[int(afactor.b*255.01 * 6.0 + 2.0+ 0.1 + 1.0)]);\n"
"tc.x = dot(pos, vfuniforms[int(afactor.b*255.01 * 6.0 + 4.0+ 0.1 + 0.0)]);\n"
"tc.y = dot(pos, vfuniforms[int(afactor.b*255.01 * 6.0 + 4.0+ 0.1 + 1.0)]);\n"
"fucxadd = vfuniforms[int(afactor.b*255.01 * 6.0 + 0.0+ 0.1)];\n"
"fucxmul = vfuniforms[int(afactor.b*255.01 * 6.0 + 1.0+ 0.1)];\n"
"factor = afactor;\n"
"}\n";

extern const char* pSource_VBatchVertex;
const char* pSource_VBatchVertex = 
"uniform vec4 vfuniforms[48];\n"
"attribute vec4 acolor;\n"
"attribute vec4 pos;\n"
"attribute float vbatch;\n"
"varying vec4 color;\n"
"void main() {\n"
"gl_Position = vec4(0.0,0,0.0,1);\n"
"gl_Position.x = dot(pos, vfuniforms[int(vbatch * 2.0 + 0.0+ 0.1 + 0.0)]);\n"
"gl_Position.y = dot(pos, vfuniforms[int(vbatch * 2.0 + 0.0+ 0.1 + 1.0)]);\n"
"color = acolor;\n"
"}\n";

extern const char* pSource_VBatchVertexEAlpha;
const char* pSource_VBatchVertexEAlpha = 
"uniform vec4 vfuniforms[48];\n"
"attribute vec4 acolor;\n"
"attribute vec4 afactor;\n"
"attribute vec4 pos;\n"
"varying vec4 color;\n"
"varying vec4 factor;\n"
"void main() {\n"
"gl_Position = vec4(0.0,0,0.0,1);\n"
"gl_Position.x = dot(pos, vfuniforms[int(afactor.b*255.01 * 2.0 + 0.0+ 0.1 + 0.0)]);\n"
"gl_Position.y = dot(pos, vfuniforms[int(afactor.b*255.01 * 2.0 + 0.0+ 0.1 + 1.0)]);\n"
"color = acolor;\n"
"factor = afactor;\n"
"}\n";

extern const char* pSource_VBatchVertexCxform;
const char* pSource_VBatchVertexCxform = 
"uniform vec4 vfuniforms[96];\n"
"attribute vec4 acolor;\n"
"attribute vec4 pos;\n"
"attribute float vbatch;\n"
"varying vec4 color;\n"
"varying vec4 fucxadd;\n"
"varying vec4 fucxmul;\n"
"void main() {\n"
"gl_Position = vec4(0.0,0,0.0,1);\n"
"gl_Position.x = dot(pos, vfuniforms[int(vbatch * 4.0 + 2.0+ 0.1 + 0.0)]);\n"
"gl_Position.y = dot(pos, vfuniforms[int(vbatch * 4.0 + 2.0+ 0.1 + 1.0)]);\n"
"color = acolor;\n"
"fucxadd = vfuniforms[int(vbatch * 4.0 + 0.0+ 0.1)];\n"
"fucxmul = vfuniforms[int(vbatch * 4.0 + 1.0+ 0.1)];\n"
"}\n";

extern const char* pSource_VBatchVertexCxformEAlpha;
const char* pSource_VBatchVertexCxformEAlpha = 
"uniform vec4 vfuniforms[96];\n"
"attribute vec4 acolor;\n"
"attribute vec4 afactor;\n"
"attribute vec4 pos;\n"
"varying vec4 color;\n"
"varying vec4 factor;\n"
"varying vec4 fucxadd;\n"
"varying vec4 fucxmul;\n"
"void main() {\n"
"gl_Position = vec4(0.0,0,0.0,1);\n"
"gl_Position.x = dot(pos, vfuniforms[int(afactor.b*255.01 * 4.0 + 2.0+ 0.1 + 0.0)]);\n"
"gl_Position.y = dot(pos, vfuniforms[int(afactor.b*255.01 * 4.0 + 2.0+ 0.1 + 1.0)]);\n"
"color = acolor;\n"
"fucxadd = vfuniforms[int(afactor.b*255.01 * 4.0 + 0.0+ 0.1)];\n"
"fucxmul = vfuniforms[int(afactor.b*255.01 * 4.0 + 1.0+ 0.1)];\n"
"factor = afactor;\n"
"}\n";

extern const char* pSource_VBatchTexTGTexTG;
const char* pSource_VBatchTexTGTexTG = 
"uniform vec4 vfuniforms[144];\n"
"attribute vec4 afactor;\n"
"attribute vec4 pos;\n"
"varying vec4 factor;\n"
"varying vec2 tc0;\n"
"varying vec2 tc1;\n"
"void main() {\n"
"gl_Position = vec4(0.0,0,0.0,1);\n"
"gl_Position.x = dot(pos, vfuniforms[int(afactor.b*255.01 * 6.0 + 0.0+ 0.1 + 0.0)]);\n"
"gl_Position.y = dot(pos, vfuniforms[int(afactor.b*255.01 * 6.0 + 0.0+ 0.1 + 1.0)]);\n"
"tc0.x = dot(pos, vfuniforms[int(afactor.b*255.01 * 6.0 + 2.0+ 0.1 + 0.0)]);\n"
"tc0.y = dot(pos, vfuniforms[int(afactor.b*255.01 * 6.0 + 2.0+ 0.1 + 1.0)]);\n"
"tc1.x = dot(pos, vfuniforms[int(afactor.b*255.01 * 6.0 + 2.0+ 0.1 + 2.0)]);\n"
"tc1.y = dot(pos, vfuniforms[int(afactor.b*255.01 * 6.0 + 2.0+ 0.1 + 3.0)]);\n"
"factor = afactor;\n"
"}\n";

extern const char* pSource_VBatchTexTGTexTGCxform;
const char* pSource_VBatchTexTGTexTGCxform = 
"uniform vec4 vfuniforms[192];\n"
"attribute vec4 afactor;\n"
"attribute vec4 pos;\n"
"varying vec4 factor;\n"
"varying vec4 fucxadd;\n"
"varying vec4 fucxmul;\n"
"varying vec2 tc0;\n"
"varying vec2 tc1;\n"
"void main() {\n"
"gl_Position = vec4(0.0,0,0.0,1);\n"
"gl_Position.x = dot(pos, vfuniforms[int(afactor.b*255.01 * 8.0 + 2.0+ 0.1 + 0.0)]);\n"
"gl_Position.y = dot(pos, vfuniforms[int(afactor.b*255.01 * 8.0 + 2.0+ 0.1 + 1.0)]);\n"
"tc0.x = dot(pos, vfuniforms[int(afactor.b*255.01 * 8.0 + 4.0+ 0.1 + 0.0)]);\n"
"tc0.y = dot(pos, vfuniforms[int(afactor.b*255.01 * 8.0 + 4.0+ 0.1 + 1.0)]);\n"
"tc1.x = dot(pos, vfuniforms[int(afactor.b*255.01 * 8.0 + 4.0+ 0.1 + 2.0)]);\n"
"tc1.y = dot(pos, vfuniforms[int(afactor.b*255.01 * 8.0 + 4.0+ 0.1 + 3.0)]);\n"
"fucxadd = vfuniforms[int(afactor.b*255.01 * 8.0 + 0.0+ 0.1)];\n"
"fucxmul = vfuniforms[int(afactor.b*255.01 * 8.0 + 1.0+ 0.1)];\n"
"factor = afactor;\n"
"}\n";

extern const char* pSource_VBatchTexTGVertex;
const char* pSource_VBatchTexTGVertex = 
"uniform vec4 vfuniforms[96];\n"
"attribute vec4 acolor;\n"
"attribute vec4 afactor;\n"
"attribute vec4 pos;\n"
"varying vec4 color;\n"
"varying vec4 factor;\n"
"varying vec2 tc;\n"
"void main() {\n"
"gl_Position = vec4(0.0,0,0.0,1);\n"
"gl_Position.x = dot(pos, vfuniforms[int(afactor.b*255.01 * 4.0 + 0.0+ 0.1 + 0.0)]);\n"
"gl_Position.y = dot(pos, vfuniforms[int(afactor.b*255.01 * 4.0 + 0.0+ 0.1 + 1.0)]);\n"
"color = acolor;\n"
"tc.x = dot(pos, vfuniforms[int(afactor.b*255.01 * 4.0 + 2.0+ 0.1 + 0.0)]);\n"
"tc.y = dot(pos, vfuniforms[int(afactor.b*255.01 * 4.0 + 2.0+ 0.1 + 1.0)]);\n"
"factor = afactor;\n"
"}\n";

extern const char* pSource_VBatchTexTGVertexCxform;
const char* pSource_VBatchTexTGVertexCxform = 
"uniform vec4 vfuniforms[144];\n"
"attribute vec4 acolor;\n"
"attribute vec4 afactor;\n"
"attribute vec4 pos;\n"
"varying vec4 color;\n"
"varying vec4 factor;\n"
"varying vec4 fucxadd;\n"
"varying vec4 fucxmul;\n"
"varying vec2 tc;\n"
"void main() {\n"
"gl_Position = vec4(0.0,0,0.0,1);\n"
"gl_Position.x = dot(pos, vfuniforms[int(afactor.b*255.01 * 6.0 + 2.0+ 0.1 + 0.0)]);\n"
"gl_Position.y = dot(pos, vfuniforms[int(afactor.b*255.01 * 6.0 + 2.0+ 0.1 + 1.0)]);\n"
"color = acolor;\n"
"tc.x = dot(pos, vfuniforms[int(afactor.b*255.01 * 6.0 + 4.0+ 0.1 + 0.0)]);\n"
"tc.y = dot(pos, vfuniforms[int(afactor.b*255.01 * 6.0 + 4.0+ 0.1 + 1.0)]);\n"
"fucxadd = vfuniforms[int(afactor.b*255.01 * 6.0 + 0.0+ 0.1)];\n"
"fucxmul = vfuniforms[int(afactor.b*255.01 * 6.0 + 1.0+ 0.1)];\n"
"factor = afactor;\n"
"}\n";

extern const char* pSource_VBatchPosition3dTexTG;
const char* pSource_VBatchPosition3dTexTG = 
"uniform mat4 vfmuniforms[24];\n"
"uniform vec4 vfuniforms[48];\n"
"attribute vec4 pos;\n"
"attribute float vbatch;\n"
"varying vec2 tc;\n"
"void main() {\n"
"gl_Position = (pos) * ( vfmuniforms[int(vbatch * 1.0 + 0.0+ 0.1)]);\n"
"tc.x = dot(pos, vfuniforms[int(vbatch * 2.0 + 0.0+ 0.1 + 0.0)]);\n"
"tc.y = dot(pos, vfuniforms[int(vbatch * 2.0 + 0.0+ 0.1 + 1.0)]);\n"
"}\n";

extern const char* pSource_VBatchPosition3dTexTGEAlpha;
const char* pSource_VBatchPosition3dTexTGEAlpha = 
"uniform mat4 vfmuniforms[24];\n"
"uniform vec4 vfuniforms[48];\n"
"attribute vec4 afactor;\n"
"attribute vec4 pos;\n"
"varying vec4 factor;\n"
"varying vec2 tc;\n"
"void main() {\n"
"gl_Position = (pos) * ( vfmuniforms[int(afactor.b*255.01 * 1.0 + 0.0+ 0.1)]);\n"
"tc.x = dot(pos, vfuniforms[int(afactor.b*255.01 * 2.0 + 0.0+ 0.1 + 0.0)]);\n"
"tc.y = dot(pos, vfuniforms[int(afactor.b*255.01 * 2.0 + 0.0+ 0.1 + 1.0)]);\n"
"factor = afactor;\n"
"}\n";

extern const char* pSource_VBatchPosition3dTexTGCxform;
const char* pSource_VBatchPosition3dTexTGCxform = 
"uniform mat4 vfmuniforms[24];\n"
"uniform vec4 vfuniforms[96];\n"
"attribute vec4 pos;\n"
"attribute float vbatch;\n"
"varying vec4 fucxadd;\n"
"varying vec4 fucxmul;\n"
"varying vec2 tc;\n"
"void main() {\n"
"gl_Position = (pos) * ( vfmuniforms[int(vbatch * 1.0 + 0.0+ 0.1)]);\n"
"tc.x = dot(pos, vfuniforms[int(vbatch * 4.0 + 2.0+ 0.1 + 0.0)]);\n"
"tc.y = dot(pos, vfuniforms[int(vbatch * 4.0 + 2.0+ 0.1 + 1.0)]);\n"
"fucxadd = vfuniforms[int(vbatch * 4.0 + 0.0+ 0.1)];\n"
"fucxmul = vfuniforms[int(vbatch * 4.0 + 1.0+ 0.1)];\n"
"}\n";

extern const char* pSource_VBatchPosition3dTexTGCxformEAlpha;
const char* pSource_VBatchPosition3dTexTGCxformEAlpha = 
"uniform mat4 vfmuniforms[24];\n"
"uniform vec4 vfuniforms[96];\n"
"attribute vec4 afactor;\n"
"attribute vec4 pos;\n"
"varying vec4 factor;\n"
"varying vec4 fucxadd;\n"
"varying vec4 fucxmul;\n"
"varying vec2 tc;\n"
"void main() {\n"
"gl_Position = (pos) * ( vfmuniforms[int(afactor.b*255.01 * 1.0 + 0.0+ 0.1)]);\n"
"tc.x = dot(pos, vfuniforms[int(afactor.b*255.01 * 4.0 + 2.0+ 0.1 + 0.0)]);\n"
"tc.y = dot(pos, vfuniforms[int(afactor.b*255.01 * 4.0 + 2.0+ 0.1 + 1.0)]);\n"
"fucxadd = vfuniforms[int(afactor.b*255.01 * 4.0 + 0.0+ 0.1)];\n"
"fucxmul = vfuniforms[int(afactor.b*255.01 * 4.0 + 1.0+ 0.1)];\n"
"factor = afactor;\n"
"}\n";

extern const char* pSource_VBatchPosition3dVertex;
const char* pSource_VBatchPosition3dVertex = 
"uniform mat4 vfmuniforms[24];\n"
"attribute vec4 acolor;\n"
"attribute vec4 pos;\n"
"attribute float vbatch;\n"
"varying vec4 color;\n"
"void main() {\n"
"gl_Position = (pos) * ( vfmuniforms[int(vbatch * 1.0 + 0.0+ 0.1)]);\n"
"color = acolor;\n"
"}\n";

extern const char* pSource_VBatchPosition3dVertexEAlpha;
const char* pSource_VBatchPosition3dVertexEAlpha = 
"uniform mat4 vfmuniforms[24];\n"
"attribute vec4 acolor;\n"
"attribute vec4 afactor;\n"
"attribute vec4 pos;\n"
"varying vec4 color;\n"
"varying vec4 factor;\n"
"void main() {\n"
"gl_Position = (pos) * ( vfmuniforms[int(afactor.b*255.01 * 1.0 + 0.0+ 0.1)]);\n"
"color = acolor;\n"
"factor = afactor;\n"
"}\n";

extern const char* pSource_VBatchPosition3dVertexCxform;
const char* pSource_VBatchPosition3dVertexCxform = 
"uniform mat4 vfmuniforms[24];\n"
"uniform vec4 vfuniforms[48];\n"
"attribute vec4 acolor;\n"
"attribute vec4 pos;\n"
"attribute float vbatch;\n"
"varying vec4 color;\n"
"varying vec4 fucxadd;\n"
"varying vec4 fucxmul;\n"
"void main() {\n"
"gl_Position = (pos) * ( vfmuniforms[int(vbatch * 1.0 + 0.0+ 0.1)]);\n"
"color = acolor;\n"
"fucxadd = vfuniforms[int(vbatch * 2.0 + 0.0+ 0.1)];\n"
"fucxmul = vfuniforms[int(vbatch * 2.0 + 1.0+ 0.1)];\n"
"}\n";

extern const char* pSource_VBatchPosition3dVertexCxformEAlpha;
const char* pSource_VBatchPosition3dVertexCxformEAlpha = 
"uniform mat4 vfmuniforms[24];\n"
"uniform vec4 vfuniforms[48];\n"
"attribute vec4 acolor;\n"
"attribute vec4 afactor;\n"
"attribute vec4 pos;\n"
"varying vec4 color;\n"
"varying vec4 factor;\n"
"varying vec4 fucxadd;\n"
"varying vec4 fucxmul;\n"
"void main() {\n"
"gl_Position = (pos) * ( vfmuniforms[int(afactor.b*255.01 * 1.0 + 0.0+ 0.1)]);\n"
"color = acolor;\n"
"fucxadd = vfuniforms[int(afactor.b*255.01 * 2.0 + 0.0+ 0.1)];\n"
"fucxmul = vfuniforms[int(afactor.b*255.01 * 2.0 + 1.0+ 0.1)];\n"
"factor = afactor;\n"
"}\n";

extern const char* pSource_VBatchPosition3dTexTGTexTG;
const char* pSource_VBatchPosition3dTexTGTexTG = 
"uniform mat4 vfmuniforms[24];\n"
"uniform vec4 vfuniforms[96];\n"
"attribute vec4 afactor;\n"
"attribute vec4 pos;\n"
"varying vec4 factor;\n"
"varying vec2 tc0;\n"
"varying vec2 tc1;\n"
"void main() {\n"
"gl_Position = (pos) * ( vfmuniforms[int(afactor.b*255.01 * 1.0 + 0.0+ 0.1)]);\n"
"tc0.x = dot(pos, vfuniforms[int(afactor.b*255.01 * 4.0 + 0.0+ 0.1 + 0.0)]);\n"
"tc0.y = dot(pos, vfuniforms[int(afactor.b*255.01 * 4.0 + 0.0+ 0.1 + 1.0)]);\n"
"tc1.x = dot(pos, vfuniforms[int(afactor.b*255.01 * 4.0 + 0.0+ 0.1 + 2.0)]);\n"
"tc1.y = dot(pos, vfuniforms[int(afactor.b*255.01 * 4.0 + 0.0+ 0.1 + 3.0)]);\n"
"factor = afactor;\n"
"}\n";

extern const char* pSource_VBatchPosition3dTexTGTexTGCxform;
const char* pSource_VBatchPosition3dTexTGTexTGCxform = 
"uniform mat4 vfmuniforms[24];\n"
"uniform vec4 vfuniforms[144];\n"
"attribute vec4 afactor;\n"
"attribute vec4 pos;\n"
"varying vec4 factor;\n"
"varying vec4 fucxadd;\n"
"varying vec4 fucxmul;\n"
"varying vec2 tc0;\n"
"varying vec2 tc1;\n"
"void main() {\n"
"gl_Position = (pos) * ( vfmuniforms[int(afactor.b*255.01 * 1.0 + 0.0+ 0.1)]);\n"
"tc0.x = dot(pos, vfuniforms[int(afactor.b*255.01 * 6.0 + 2.0+ 0.1 + 0.0)]);\n"
"tc0.y = dot(pos, vfuniforms[int(afactor.b*255.01 * 6.0 + 2.0+ 0.1 + 1.0)]);\n"
"tc1.x = dot(pos, vfuniforms[int(afactor.b*255.01 * 6.0 + 2.0+ 0.1 + 2.0)]);\n"
"tc1.y = dot(pos, vfuniforms[int(afactor.b*255.01 * 6.0 + 2.0+ 0.1 + 3.0)]);\n"
"fucxadd = vfuniforms[int(afactor.b*255.01 * 6.0 + 0.0+ 0.1)];\n"
"fucxmul = vfuniforms[int(afactor.b*255.01 * 6.0 + 1.0+ 0.1)];\n"
"factor = afactor;\n"
"}\n";

extern const char* pSource_VBatchPosition3dTexTGVertex;
const char* pSource_VBatchPosition3dTexTGVertex = 
"uniform mat4 vfmuniforms[24];\n"
"uniform vec4 vfuniforms[48];\n"
"attribute vec4 acolor;\n"
"attribute vec4 afactor;\n"
"attribute vec4 pos;\n"
"varying vec4 color;\n"
"varying vec4 factor;\n"
"varying vec2 tc;\n"
"void main() {\n"
"gl_Position = (pos) * ( vfmuniforms[int(afactor.b*255.01 * 1.0 + 0.0+ 0.1)]);\n"
"color = acolor;\n"
"tc.x = dot(pos, vfuniforms[int(afactor.b*255.01 * 2.0 + 0.0+ 0.1 + 0.0)]);\n"
"tc.y = dot(pos, vfuniforms[int(afactor.b*255.01 * 2.0 + 0.0+ 0.1 + 1.0)]);\n"
"factor = afactor;\n"
"}\n";

extern const char* pSource_VBatchPosition3dTexTGVertexCxform;
const char* pSource_VBatchPosition3dTexTGVertexCxform = 
"uniform mat4 vfmuniforms[24];\n"
"uniform vec4 vfuniforms[96];\n"
"attribute vec4 acolor;\n"
"attribute vec4 afactor;\n"
"attribute vec4 pos;\n"
"varying vec4 color;\n"
"varying vec4 factor;\n"
"varying vec4 fucxadd;\n"
"varying vec4 fucxmul;\n"
"varying vec2 tc;\n"
"void main() {\n"
"gl_Position = (pos) * ( vfmuniforms[int(afactor.b*255.01 * 1.0 + 0.0+ 0.1)]);\n"
"color = acolor;\n"
"tc.x = dot(pos, vfuniforms[int(afactor.b*255.01 * 4.0 + 2.0+ 0.1 + 0.0)]);\n"
"tc.y = dot(pos, vfuniforms[int(afactor.b*255.01 * 4.0 + 2.0+ 0.1 + 1.0)]);\n"
"fucxadd = vfuniforms[int(afactor.b*255.01 * 4.0 + 0.0+ 0.1)];\n"
"fucxmul = vfuniforms[int(afactor.b*255.01 * 4.0 + 1.0+ 0.1)];\n"
"factor = afactor;\n"
"}\n";

extern const char* pSource_VInstancedTexTG;
const char* pSource_VInstancedTexTG = 
"#extension GL_ARB_draw_instanced : enable\n"
"uniform vec4 vfuniforms[96];\n"
"attribute vec4 pos;\n"
"varying vec2 tc;\n"
"void main() {\n"
"gl_Position = vec4(0.0,0,0.0,1);\n"
"gl_Position.x = dot(pos, vfuniforms[int(gl_InstanceID * 4.0 + 0.0+ 0.1 + 0.0)]);\n"
"gl_Position.y = dot(pos, vfuniforms[int(gl_InstanceID * 4.0 + 0.0+ 0.1 + 1.0)]);\n"
"tc.x = dot(pos, vfuniforms[int(gl_InstanceID * 4.0 + 2.0+ 0.1 + 0.0)]);\n"
"tc.y = dot(pos, vfuniforms[int(gl_InstanceID * 4.0 + 2.0+ 0.1 + 1.0)]);\n"
"}\n";

extern const char* pSource_FInstancedTexTG;
const char* pSource_FInstancedTexTG = 
"#extension GL_ARB_draw_instanced : enable\n"
"uniform sampler2D tex;\n"
"varying vec2 tc;\n"
"void main() {\n"
"gl_FragColor = texture2D(tex,tc);\n"
"}\n";

extern const char* pSource_FInstancedTexTGMul;
const char* pSource_FInstancedTexTGMul = 
"#extension GL_ARB_draw_instanced : enable\n"
"uniform sampler2D tex;\n"
"varying vec2 tc;\n"
"void main() {\n"
"gl_FragColor = texture2D(tex,tc);\n"
"gl_FragColor = mix(vec4(1.0,1,1.0,1), gl_FragColor, gl_FragColor.a);\n"
"}\n";

extern const char* pSource_VInstancedTexTGEAlpha;
const char* pSource_VInstancedTexTGEAlpha = 
"#extension GL_ARB_draw_instanced : enable\n"
"uniform vec4 vfuniforms[96];\n"
"attribute vec4 afactor;\n"
"attribute vec4 pos;\n"
"varying vec4 factor;\n"
"varying vec2 tc;\n"
"void main() {\n"
"gl_Position = vec4(0.0,0,0.0,1);\n"
"gl_Position.x = dot(pos, vfuniforms[int(gl_InstanceID * 4.0 + 0.0+ 0.1 + 0.0)]);\n"
"gl_Position.y = dot(pos, vfuniforms[int(gl_InstanceID * 4.0 + 0.0+ 0.1 + 1.0)]);\n"
"tc.x = dot(pos, vfuniforms[int(gl_InstanceID * 4.0 + 2.0+ 0.1 + 0.0)]);\n"
"tc.y = dot(pos, vfuniforms[int(gl_InstanceID * 4.0 + 2.0+ 0.1 + 1.0)]);\n"
"factor = afactor;\n"
"}\n";

extern const char* pSource_FInstancedTexTGEAlpha;
const char* pSource_FInstancedTexTGEAlpha = 
"#extension GL_ARB_draw_instanced : enable\n"
"uniform sampler2D tex;\n"
"varying vec4 factor;\n"
"varying vec2 tc;\n"
"void main() {\n"
"gl_FragColor = texture2D(tex,tc);\n"
"gl_FragColor.a *= factor.a;\n"
"}\n";

extern const char* pSource_FInstancedTexTGEAlphaMul;
const char* pSource_FInstancedTexTGEAlphaMul = 
"#extension GL_ARB_draw_instanced : enable\n"
"uniform sampler2D tex;\n"
"varying vec4 factor;\n"
"varying vec2 tc;\n"
"void main() {\n"
"gl_FragColor = texture2D(tex,tc);\n"
"gl_FragColor.a *= factor.a;\n"
"gl_FragColor = mix(vec4(1.0,1,1.0,1), gl_FragColor, gl_FragColor.a);\n"
"}\n";

extern const char* pSource_VInstancedTexTGCxform;
const char* pSource_VInstancedTexTGCxform = 
"#extension GL_ARB_draw_instanced : enable\n"
"uniform vec4 vfuniforms[144];\n"
"attribute vec4 pos;\n"
"varying vec4 fucxadd;\n"
"varying vec4 fucxmul;\n"
"varying vec2 tc;\n"
"void main() {\n"
"gl_Position = vec4(0.0,0,0.0,1);\n"
"gl_Position.x = dot(pos, vfuniforms[int(gl_InstanceID * 6.0 + 2.0+ 0.1 + 0.0)]);\n"
"gl_Position.y = dot(pos, vfuniforms[int(gl_InstanceID * 6.0 + 2.0+ 0.1 + 1.0)]);\n"
"tc.x = dot(pos, vfuniforms[int(gl_InstanceID * 6.0 + 4.0+ 0.1 + 0.0)]);\n"
"tc.y = dot(pos, vfuniforms[int(gl_InstanceID * 6.0 + 4.0+ 0.1 + 1.0)]);\n"
"fucxadd = vfuniforms[int(gl_InstanceID * 6.0 + 0.0+ 0.1)];\n"
"fucxmul = vfuniforms[int(gl_InstanceID * 6.0 + 1.0+ 0.1)];\n"
"}\n";

extern const char* pSource_FInstancedTexTGCxform;
const char* pSource_FInstancedTexTGCxform = 
"#extension GL_ARB_draw_instanced : enable\n"
"uniform sampler2D tex;\n"
"varying vec4 fucxadd;\n"
"varying vec4 fucxmul;\n"
"varying vec2 tc;\n"
"void main() {\n"
"gl_FragColor = texture2D(tex,tc);\n"
"gl_FragColor = gl_FragColor * fucxmul + fucxadd;\n"
"}\n";

extern const char* pSource_FInstancedTexTGCxformMul;
const char* pSource_FInstancedTexTGCxformMul = 
"#extension GL_ARB_draw_instanced : enable\n"
"uniform sampler2D tex;\n"
"varying vec4 fucxadd;\n"
"varying vec4 fucxmul;\n"
"varying vec2 tc;\n"
"void main() {\n"
"gl_FragColor = texture2D(tex,tc);\n"
"gl_FragColor = gl_FragColor * fucxmul + fucxadd;\n"
"gl_FragColor = mix(vec4(1.0,1,1.0,1), gl_FragColor, gl_FragColor.a);\n"
"}\n";

extern const char* pSource_VInstancedTexTGCxformEAlpha;
const char* pSource_VInstancedTexTGCxformEAlpha = 
"#extension GL_ARB_draw_instanced : enable\n"
"uniform vec4 vfuniforms[144];\n"
"attribute vec4 afactor;\n"
"attribute vec4 pos;\n"
"varying vec4 factor;\n"
"varying vec4 fucxadd;\n"
"varying vec4 fucxmul;\n"
"varying vec2 tc;\n"
"void main() {\n"
"gl_Position = vec4(0.0,0,0.0,1);\n"
"gl_Position.x = dot(pos, vfuniforms[int(gl_InstanceID * 6.0 + 2.0+ 0.1 + 0.0)]);\n"
"gl_Position.y = dot(pos, vfuniforms[int(gl_InstanceID * 6.0 + 2.0+ 0.1 + 1.0)]);\n"
"tc.x = dot(pos, vfuniforms[int(gl_InstanceID * 6.0 + 4.0+ 0.1 + 0.0)]);\n"
"tc.y = dot(pos, vfuniforms[int(gl_InstanceID * 6.0 + 4.0+ 0.1 + 1.0)]);\n"
"fucxadd = vfuniforms[int(gl_InstanceID * 6.0 + 0.0+ 0.1)];\n"
"fucxmul = vfuniforms[int(gl_InstanceID * 6.0 + 1.0+ 0.1)];\n"
"factor = afactor;\n"
"}\n";

extern const char* pSource_FInstancedTexTGCxformEAlpha;
const char* pSource_FInstancedTexTGCxformEAlpha = 
"#extension GL_ARB_draw_instanced : enable\n"
"uniform sampler2D tex;\n"
"varying vec4 factor;\n"
"varying vec4 fucxadd;\n"
"varying vec4 fucxmul;\n"
"varying vec2 tc;\n"
"void main() {\n"
"gl_FragColor = texture2D(tex,tc);\n"
"gl_FragColor = gl_FragColor * fucxmul + fucxadd;\n"
"gl_FragColor.a *= factor.a;\n"
"}\n";

extern const char* pSource_FInstancedTexTGCxformEAlphaMul;
const char* pSource_FInstancedTexTGCxformEAlphaMul = 
"#extension GL_ARB_draw_instanced : enable\n"
"uniform sampler2D tex;\n"
"varying vec4 factor;\n"
"varying vec4 fucxadd;\n"
"varying vec4 fucxmul;\n"
"varying vec2 tc;\n"
"void main() {\n"
"gl_FragColor = texture2D(tex,tc);\n"
"gl_FragColor = gl_FragColor * fucxmul + fucxadd;\n"
"gl_FragColor.a *= factor.a;\n"
"gl_FragColor = mix(vec4(1.0,1,1.0,1), gl_FragColor, gl_FragColor.a);\n"
"}\n";

extern const char* pSource_VInstancedVertex;
const char* pSource_VInstancedVertex = 
"#extension GL_ARB_draw_instanced : enable\n"
"uniform vec4 vfuniforms[48];\n"
"attribute vec4 acolor;\n"
"attribute vec4 pos;\n"
"varying vec4 color;\n"
"void main() {\n"
"gl_Position = vec4(0.0,0,0.0,1);\n"
"gl_Position.x = dot(pos, vfuniforms[int(gl_InstanceID * 2.0 + 0.0+ 0.1 + 0.0)]);\n"
"gl_Position.y = dot(pos, vfuniforms[int(gl_InstanceID * 2.0 + 0.0+ 0.1 + 1.0)]);\n"
"color = acolor;\n"
"}\n";

extern const char* pSource_FInstancedVertex;
const char* pSource_FInstancedVertex = 
"#extension GL_ARB_draw_instanced : enable\n"
"varying vec4 color;\n"
"void main() {\n"
"gl_FragColor = color;\n"
"}\n";

extern const char* pSource_FInstancedVertexMul;
const char* pSource_FInstancedVertexMul = 
"#extension GL_ARB_draw_instanced : enable\n"
"varying vec4 color;\n"
"void main() {\n"
"gl_FragColor = color;\n"
"gl_FragColor = mix(vec4(1.0,1,1.0,1), gl_FragColor, gl_FragColor.a);\n"
"}\n";

extern const char* pSource_VInstancedVertexEAlpha;
const char* pSource_VInstancedVertexEAlpha = 
"#extension GL_ARB_draw_instanced : enable\n"
"uniform vec4 vfuniforms[48];\n"
"attribute vec4 acolor;\n"
"attribute vec4 afactor;\n"
"attribute vec4 pos;\n"
"varying vec4 color;\n"
"varying vec4 factor;\n"
"void main() {\n"
"gl_Position = vec4(0.0,0,0.0,1);\n"
"gl_Position.x = dot(pos, vfuniforms[int(gl_InstanceID * 2.0 + 0.0+ 0.1 + 0.0)]);\n"
"gl_Position.y = dot(pos, vfuniforms[int(gl_InstanceID * 2.0 + 0.0+ 0.1 + 1.0)]);\n"
"color = acolor;\n"
"factor = afactor;\n"
"}\n";

extern const char* pSource_FInstancedVertexEAlpha;
const char* pSource_FInstancedVertexEAlpha = 
"#extension GL_ARB_draw_instanced : enable\n"
"varying vec4 color;\n"
"varying vec4 factor;\n"
"void main() {\n"
"gl_FragColor = color;\n"
"gl_FragColor.a *= factor.a;\n"
"}\n";

extern const char* pSource_FInstancedVertexEAlphaMul;
const char* pSource_FInstancedVertexEAlphaMul = 
"#extension GL_ARB_draw_instanced : enable\n"
"varying vec4 color;\n"
"varying vec4 factor;\n"
"void main() {\n"
"gl_FragColor = color;\n"
"gl_FragColor.a *= factor.a;\n"
"gl_FragColor = mix(vec4(1.0,1,1.0,1), gl_FragColor, gl_FragColor.a);\n"
"}\n";

extern const char* pSource_VInstancedVertexCxform;
const char* pSource_VInstancedVertexCxform = 
"#extension GL_ARB_draw_instanced : enable\n"
"uniform vec4 vfuniforms[96];\n"
"attribute vec4 acolor;\n"
"attribute vec4 pos;\n"
"varying vec4 color;\n"
"varying vec4 fucxadd;\n"
"varying vec4 fucxmul;\n"
"void main() {\n"
"gl_Position = vec4(0.0,0,0.0,1);\n"
"gl_Position.x = dot(pos, vfuniforms[int(gl_InstanceID * 4.0 + 2.0+ 0.1 + 0.0)]);\n"
"gl_Position.y = dot(pos, vfuniforms[int(gl_InstanceID * 4.0 + 2.0+ 0.1 + 1.0)]);\n"
"color = acolor;\n"
"fucxadd = vfuniforms[int(gl_InstanceID * 4.0 + 0.0+ 0.1)];\n"
"fucxmul = vfuniforms[int(gl_InstanceID * 4.0 + 1.0+ 0.1)];\n"
"}\n";

extern const char* pSource_FInstancedVertexCxform;
const char* pSource_FInstancedVertexCxform = 
"#extension GL_ARB_draw_instanced : enable\n"
"varying vec4 color;\n"
"varying vec4 fucxadd;\n"
"varying vec4 fucxmul;\n"
"void main() {\n"
"gl_FragColor = color;\n"
"gl_FragColor = gl_FragColor * fucxmul + fucxadd;\n"
"}\n";

extern const char* pSource_FInstancedVertexCxformMul;
const char* pSource_FInstancedVertexCxformMul = 
"#extension GL_ARB_draw_instanced : enable\n"
"varying vec4 color;\n"
"varying vec4 fucxadd;\n"
"varying vec4 fucxmul;\n"
"void main() {\n"
"gl_FragColor = color;\n"
"gl_FragColor = gl_FragColor * fucxmul + fucxadd;\n"
"gl_FragColor = mix(vec4(1.0,1,1.0,1), gl_FragColor, gl_FragColor.a);\n"
"}\n";

extern const char* pSource_VInstancedVertexCxformEAlpha;
const char* pSource_VInstancedVertexCxformEAlpha = 
"#extension GL_ARB_draw_instanced : enable\n"
"uniform vec4 vfuniforms[96];\n"
"attribute vec4 acolor;\n"
"attribute vec4 afactor;\n"
"attribute vec4 pos;\n"
"varying vec4 color;\n"
"varying vec4 factor;\n"
"varying vec4 fucxadd;\n"
"varying vec4 fucxmul;\n"
"void main() {\n"
"gl_Position = vec4(0.0,0,0.0,1);\n"
"gl_Position.x = dot(pos, vfuniforms[int(gl_InstanceID * 4.0 + 2.0+ 0.1 + 0.0)]);\n"
"gl_Position.y = dot(pos, vfuniforms[int(gl_InstanceID * 4.0 + 2.0+ 0.1 + 1.0)]);\n"
"color = acolor;\n"
"fucxadd = vfuniforms[int(gl_InstanceID * 4.0 + 0.0+ 0.1)];\n"
"fucxmul = vfuniforms[int(gl_InstanceID * 4.0 + 1.0+ 0.1)];\n"
"factor = afactor;\n"
"}\n";

extern const char* pSource_FInstancedVertexCxformEAlpha;
const char* pSource_FInstancedVertexCxformEAlpha = 
"#extension GL_ARB_draw_instanced : enable\n"
"varying vec4 color;\n"
"varying vec4 factor;\n"
"varying vec4 fucxadd;\n"
"varying vec4 fucxmul;\n"
"void main() {\n"
"gl_FragColor = color;\n"
"gl_FragColor = gl_FragColor * fucxmul + fucxadd;\n"
"gl_FragColor.a *= factor.a;\n"
"}\n";

extern const char* pSource_FInstancedVertexCxformEAlphaMul;
const char* pSource_FInstancedVertexCxformEAlphaMul = 
"#extension GL_ARB_draw_instanced : enable\n"
"varying vec4 color;\n"
"varying vec4 factor;\n"
"varying vec4 fucxadd;\n"
"varying vec4 fucxmul;\n"
"void main() {\n"
"gl_FragColor = color;\n"
"gl_FragColor = gl_FragColor * fucxmul + fucxadd;\n"
"gl_FragColor.a *= factor.a;\n"
"gl_FragColor = mix(vec4(1.0,1,1.0,1), gl_FragColor, gl_FragColor.a);\n"
"}\n";

extern const char* pSource_VInstancedTexTGTexTG;
const char* pSource_VInstancedTexTGTexTG = 
"#extension GL_ARB_draw_instanced : enable\n"
"uniform vec4 vfuniforms[144];\n"
"attribute vec4 afactor;\n"
"attribute vec4 pos;\n"
"varying vec4 factor;\n"
"varying vec2 tc0;\n"
"varying vec2 tc1;\n"
"void main() {\n"
"gl_Position = vec4(0.0,0,0.0,1);\n"
"gl_Position.x = dot(pos, vfuniforms[int(gl_InstanceID * 6.0 + 0.0+ 0.1 + 0.0)]);\n"
"gl_Position.y = dot(pos, vfuniforms[int(gl_InstanceID * 6.0 + 0.0+ 0.1 + 1.0)]);\n"
"tc0.x = dot(pos, vfuniforms[int(gl_InstanceID * 6.0 + 2.0+ 0.1 + 0.0)]);\n"
"tc0.y = dot(pos, vfuniforms[int(gl_InstanceID * 6.0 + 2.0+ 0.1 + 1.0)]);\n"
"tc1.x = dot(pos, vfuniforms[int(gl_InstanceID * 6.0 + 2.0+ 0.1 + 2.0)]);\n"
"tc1.y = dot(pos, vfuniforms[int(gl_InstanceID * 6.0 + 2.0+ 0.1 + 3.0)]);\n"
"factor = afactor;\n"
"}\n";

extern const char* pSource_FInstancedTexTGTexTG;
const char* pSource_FInstancedTexTGTexTG = 
"#extension GL_ARB_draw_instanced : enable\n"
"uniform sampler2D tex[2];\n"
"varying vec4 factor;\n"
"varying vec2 tc0;\n"
"varying vec2 tc1;\n"
"void main() {\n"
"vec4 fcolor0 = texture2D(tex[int(0.0)], tc0);\n"
"vec4 fcolor1 = texture2D(tex[int(1.0)], tc1);\n"
"gl_FragColor = mix(fcolor1, fcolor0, factor.r);\n"
"}\n";

extern const char* pSource_FInstancedTexTGTexTGMul;
const char* pSource_FInstancedTexTGTexTGMul = 
"#extension GL_ARB_draw_instanced : enable\n"
"uniform sampler2D tex[2];\n"
"varying vec4 factor;\n"
"varying vec2 tc0;\n"
"varying vec2 tc1;\n"
"void main() {\n"
"vec4 fcolor0 = texture2D(tex[int(0.0)], tc0);\n"
"vec4 fcolor1 = texture2D(tex[int(1.0)], tc1);\n"
"gl_FragColor = mix(fcolor1, fcolor0, factor.r);\n"
"gl_FragColor = mix(vec4(1.0,1,1.0,1), gl_FragColor, gl_FragColor.a);\n"
"}\n";

extern const char* pSource_FInstancedTexTGTexTGEAlpha;
const char* pSource_FInstancedTexTGTexTGEAlpha = 
"#extension GL_ARB_draw_instanced : enable\n"
"uniform sampler2D tex[2];\n"
"varying vec4 factor;\n"
"varying vec2 tc0;\n"
"varying vec2 tc1;\n"
"void main() {\n"
"vec4 fcolor0 = texture2D(tex[int(0.0)], tc0);\n"
"vec4 fcolor1 = texture2D(tex[int(1.0)], tc1);\n"
"gl_FragColor = mix(fcolor1, fcolor0, factor.r);\n"
"gl_FragColor.a *= factor.a;\n"
"}\n";

extern const char* pSource_FInstancedTexTGTexTGEAlphaMul;
const char* pSource_FInstancedTexTGTexTGEAlphaMul = 
"#extension GL_ARB_draw_instanced : enable\n"
"uniform sampler2D tex[2];\n"
"varying vec4 factor;\n"
"varying vec2 tc0;\n"
"varying vec2 tc1;\n"
"void main() {\n"
"vec4 fcolor0 = texture2D(tex[int(0.0)], tc0);\n"
"vec4 fcolor1 = texture2D(tex[int(1.0)], tc1);\n"
"gl_FragColor = mix(fcolor1, fcolor0, factor.r);\n"
"gl_FragColor.a *= factor.a;\n"
"gl_FragColor = mix(vec4(1.0,1,1.0,1), gl_FragColor, gl_FragColor.a);\n"
"}\n";

extern const char* pSource_VInstancedTexTGTexTGCxform;
const char* pSource_VInstancedTexTGTexTGCxform = 
"#extension GL_ARB_draw_instanced : enable\n"
"uniform vec4 vfuniforms[192];\n"
"attribute vec4 afactor;\n"
"attribute vec4 pos;\n"
"varying vec4 factor;\n"
"varying vec4 fucxadd;\n"
"varying vec4 fucxmul;\n"
"varying vec2 tc0;\n"
"varying vec2 tc1;\n"
"void main() {\n"
"gl_Position = vec4(0.0,0,0.0,1);\n"
"gl_Position.x = dot(pos, vfuniforms[int(gl_InstanceID * 8.0 + 2.0+ 0.1 + 0.0)]);\n"
"gl_Position.y = dot(pos, vfuniforms[int(gl_InstanceID * 8.0 + 2.0+ 0.1 + 1.0)]);\n"
"tc0.x = dot(pos, vfuniforms[int(gl_InstanceID * 8.0 + 4.0+ 0.1 + 0.0)]);\n"
"tc0.y = dot(pos, vfuniforms[int(gl_InstanceID * 8.0 + 4.0+ 0.1 + 1.0)]);\n"
"tc1.x = dot(pos, vfuniforms[int(gl_InstanceID * 8.0 + 4.0+ 0.1 + 2.0)]);\n"
"tc1.y = dot(pos, vfuniforms[int(gl_InstanceID * 8.0 + 4.0+ 0.1 + 3.0)]);\n"
"fucxadd = vfuniforms[int(gl_InstanceID * 8.0 + 0.0+ 0.1)];\n"
"fucxmul = vfuniforms[int(gl_InstanceID * 8.0 + 1.0+ 0.1)];\n"
"factor = afactor;\n"
"}\n";

extern const char* pSource_FInstancedTexTGTexTGCxform;
const char* pSource_FInstancedTexTGTexTGCxform = 
"#extension GL_ARB_draw_instanced : enable\n"
"uniform sampler2D tex[2];\n"
"varying vec4 factor;\n"
"varying vec4 fucxadd;\n"
"varying vec4 fucxmul;\n"
"varying vec2 tc0;\n"
"varying vec2 tc1;\n"
"void main() {\n"
"vec4 fcolor0 = texture2D(tex[int(0.0)], tc0);\n"
"vec4 fcolor1 = texture2D(tex[int(1.0)], tc1);\n"
"gl_FragColor = mix(fcolor1, fcolor0, factor.r);\n"
"gl_FragColor = gl_FragColor * fucxmul + fucxadd;\n"
"}\n";

extern const char* pSource_FInstancedTexTGTexTGCxformMul;
const char* pSource_FInstancedTexTGTexTGCxformMul = 
"#extension GL_ARB_draw_instanced : enable\n"
"uniform sampler2D tex[2];\n"
"varying vec4 factor;\n"
"varying vec4 fucxadd;\n"
"varying vec4 fucxmul;\n"
"varying vec2 tc0;\n"
"varying vec2 tc1;\n"
"void main() {\n"
"vec4 fcolor0 = texture2D(tex[int(0.0)], tc0);\n"
"vec4 fcolor1 = texture2D(tex[int(1.0)], tc1);\n"
"gl_FragColor = mix(fcolor1, fcolor0, factor.r);\n"
"gl_FragColor = gl_FragColor * fucxmul + fucxadd;\n"
"gl_FragColor = mix(vec4(1.0,1,1.0,1), gl_FragColor, gl_FragColor.a);\n"
"}\n";

extern const char* pSource_FInstancedTexTGTexTGCxformEAlpha;
const char* pSource_FInstancedTexTGTexTGCxformEAlpha = 
"#extension GL_ARB_draw_instanced : enable\n"
"uniform sampler2D tex[2];\n"
"varying vec4 factor;\n"
"varying vec4 fucxadd;\n"
"varying vec4 fucxmul;\n"
"varying vec2 tc0;\n"
"varying vec2 tc1;\n"
"void main() {\n"
"vec4 fcolor0 = texture2D(tex[int(0.0)], tc0);\n"
"vec4 fcolor1 = texture2D(tex[int(1.0)], tc1);\n"
"gl_FragColor = mix(fcolor1, fcolor0, factor.r);\n"
"gl_FragColor = gl_FragColor * fucxmul + fucxadd;\n"
"gl_FragColor.a *= factor.a;\n"
"}\n";

extern const char* pSource_FInstancedTexTGTexTGCxformEAlphaMul;
const char* pSource_FInstancedTexTGTexTGCxformEAlphaMul = 
"#extension GL_ARB_draw_instanced : enable\n"
"uniform sampler2D tex[2];\n"
"varying vec4 factor;\n"
"varying vec4 fucxadd;\n"
"varying vec4 fucxmul;\n"
"varying vec2 tc0;\n"
"varying vec2 tc1;\n"
"void main() {\n"
"vec4 fcolor0 = texture2D(tex[int(0.0)], tc0);\n"
"vec4 fcolor1 = texture2D(tex[int(1.0)], tc1);\n"
"gl_FragColor = mix(fcolor1, fcolor0, factor.r);\n"
"gl_FragColor = gl_FragColor * fucxmul + fucxadd;\n"
"gl_FragColor.a *= factor.a;\n"
"gl_FragColor = mix(vec4(1.0,1,1.0,1), gl_FragColor, gl_FragColor.a);\n"
"}\n";

extern const char* pSource_VInstancedTexTGVertex;
const char* pSource_VInstancedTexTGVertex = 
"#extension GL_ARB_draw_instanced : enable\n"
"uniform vec4 vfuniforms[96];\n"
"attribute vec4 acolor;\n"
"attribute vec4 afactor;\n"
"attribute vec4 pos;\n"
"varying vec4 color;\n"
"varying vec4 factor;\n"
"varying vec2 tc;\n"
"void main() {\n"
"gl_Position = vec4(0.0,0,0.0,1);\n"
"gl_Position.x = dot(pos, vfuniforms[int(gl_InstanceID * 4.0 + 0.0+ 0.1 + 0.0)]);\n"
"gl_Position.y = dot(pos, vfuniforms[int(gl_InstanceID * 4.0 + 0.0+ 0.1 + 1.0)]);\n"
"color = acolor;\n"
"tc.x = dot(pos, vfuniforms[int(gl_InstanceID * 4.0 + 2.0+ 0.1 + 0.0)]);\n"
"tc.y = dot(pos, vfuniforms[int(gl_InstanceID * 4.0 + 2.0+ 0.1 + 1.0)]);\n"
"factor = afactor;\n"
"}\n";

extern const char* pSource_FInstancedTexTGVertex;
const char* pSource_FInstancedTexTGVertex = 
"#extension GL_ARB_draw_instanced : enable\n"
"uniform sampler2D tex;\n"
"varying vec4 color;\n"
"varying vec4 factor;\n"
"varying vec2 tc;\n"
"void main() {\n"
"vec4 fcolor0 = texture2D(tex,tc);\n"
"vec4 fcolor1 = color;\n"
"gl_FragColor = mix(fcolor1, fcolor0, factor.r);\n"
"}\n";

extern const char* pSource_FInstancedTexTGVertexMul;
const char* pSource_FInstancedTexTGVertexMul = 
"#extension GL_ARB_draw_instanced : enable\n"
"uniform sampler2D tex;\n"
"varying vec4 color;\n"
"varying vec4 factor;\n"
"varying vec2 tc;\n"
"void main() {\n"
"vec4 fcolor0 = texture2D(tex,tc);\n"
"vec4 fcolor1 = color;\n"
"gl_FragColor = mix(fcolor1, fcolor0, factor.r);\n"
"gl_FragColor = mix(vec4(1.0,1,1.0,1), gl_FragColor, gl_FragColor.a);\n"
"}\n";

extern const char* pSource_FInstancedTexTGVertexEAlpha;
const char* pSource_FInstancedTexTGVertexEAlpha = 
"#extension GL_ARB_draw_instanced : enable\n"
"uniform sampler2D tex;\n"
"varying vec4 color;\n"
"varying vec4 factor;\n"
"varying vec2 tc;\n"
"void main() {\n"
"vec4 fcolor0 = texture2D(tex,tc);\n"
"vec4 fcolor1 = color;\n"
"gl_FragColor = mix(fcolor1, fcolor0, factor.r);\n"
"gl_FragColor.a *= factor.a;\n"
"}\n";

extern const char* pSource_FInstancedTexTGVertexEAlphaMul;
const char* pSource_FInstancedTexTGVertexEAlphaMul = 
"#extension GL_ARB_draw_instanced : enable\n"
"uniform sampler2D tex;\n"
"varying vec4 color;\n"
"varying vec4 factor;\n"
"varying vec2 tc;\n"
"void main() {\n"
"vec4 fcolor0 = texture2D(tex,tc);\n"
"vec4 fcolor1 = color;\n"
"gl_FragColor = mix(fcolor1, fcolor0, factor.r);\n"
"gl_FragColor.a *= factor.a;\n"
"gl_FragColor = mix(vec4(1.0,1,1.0,1), gl_FragColor, gl_FragColor.a);\n"
"}\n";

extern const char* pSource_VInstancedTexTGVertexCxform;
const char* pSource_VInstancedTexTGVertexCxform = 
"#extension GL_ARB_draw_instanced : enable\n"
"uniform vec4 vfuniforms[144];\n"
"attribute vec4 acolor;\n"
"attribute vec4 afactor;\n"
"attribute vec4 pos;\n"
"varying vec4 color;\n"
"varying vec4 factor;\n"
"varying vec4 fucxadd;\n"
"varying vec4 fucxmul;\n"
"varying vec2 tc;\n"
"void main() {\n"
"gl_Position = vec4(0.0,0,0.0,1);\n"
"gl_Position.x = dot(pos, vfuniforms[int(gl_InstanceID * 6.0 + 2.0+ 0.1 + 0.0)]);\n"
"gl_Position.y = dot(pos, vfuniforms[int(gl_InstanceID * 6.0 + 2.0+ 0.1 + 1.0)]);\n"
"color = acolor;\n"
"tc.x = dot(pos, vfuniforms[int(gl_InstanceID * 6.0 + 4.0+ 0.1 + 0.0)]);\n"
"tc.y = dot(pos, vfuniforms[int(gl_InstanceID * 6.0 + 4.0+ 0.1 + 1.0)]);\n"
"fucxadd = vfuniforms[int(gl_InstanceID * 6.0 + 0.0+ 0.1)];\n"
"fucxmul = vfuniforms[int(gl_InstanceID * 6.0 + 1.0+ 0.1)];\n"
"factor = afactor;\n"
"}\n";

extern const char* pSource_FInstancedTexTGVertexCxform;
const char* pSource_FInstancedTexTGVertexCxform = 
"#extension GL_ARB_draw_instanced : enable\n"
"uniform sampler2D tex;\n"
"varying vec4 color;\n"
"varying vec4 factor;\n"
"varying vec4 fucxadd;\n"
"varying vec4 fucxmul;\n"
"varying vec2 tc;\n"
"void main() {\n"
"vec4 fcolor0 = texture2D(tex,tc);\n"
"vec4 fcolor1 = color;\n"
"gl_FragColor = mix(fcolor1, fcolor0, factor.r);\n"
"gl_FragColor = gl_FragColor * fucxmul + fucxadd;\n"
"}\n";

extern const char* pSource_FInstancedTexTGVertexCxformMul;
const char* pSource_FInstancedTexTGVertexCxformMul = 
"#extension GL_ARB_draw_instanced : enable\n"
"uniform sampler2D tex;\n"
"varying vec4 color;\n"
"varying vec4 factor;\n"
"varying vec4 fucxadd;\n"
"varying vec4 fucxmul;\n"
"varying vec2 tc;\n"
"void main() {\n"
"vec4 fcolor0 = texture2D(tex,tc);\n"
"vec4 fcolor1 = color;\n"
"gl_FragColor = mix(fcolor1, fcolor0, factor.r);\n"
"gl_FragColor = gl_FragColor * fucxmul + fucxadd;\n"
"gl_FragColor = mix(vec4(1.0,1,1.0,1), gl_FragColor, gl_FragColor.a);\n"
"}\n";

extern const char* pSource_FInstancedTexTGVertexCxformEAlpha;
const char* pSource_FInstancedTexTGVertexCxformEAlpha = 
"#extension GL_ARB_draw_instanced : enable\n"
"uniform sampler2D tex;\n"
"varying vec4 color;\n"
"varying vec4 factor;\n"
"varying vec4 fucxadd;\n"
"varying vec4 fucxmul;\n"
"varying vec2 tc;\n"
"void main() {\n"
"vec4 fcolor0 = texture2D(tex,tc);\n"
"vec4 fcolor1 = color;\n"
"gl_FragColor = mix(fcolor1, fcolor0, factor.r);\n"
"gl_FragColor = gl_FragColor * fucxmul + fucxadd;\n"
"gl_FragColor.a *= factor.a;\n"
"}\n";

extern const char* pSource_FInstancedTexTGVertexCxformEAlphaMul;
const char* pSource_FInstancedTexTGVertexCxformEAlphaMul = 
"#extension GL_ARB_draw_instanced : enable\n"
"uniform sampler2D tex;\n"
"varying vec4 color;\n"
"varying vec4 factor;\n"
"varying vec4 fucxadd;\n"
"varying vec4 fucxmul;\n"
"varying vec2 tc;\n"
"void main() {\n"
"vec4 fcolor0 = texture2D(tex,tc);\n"
"vec4 fcolor1 = color;\n"
"gl_FragColor = mix(fcolor1, fcolor0, factor.r);\n"
"gl_FragColor = gl_FragColor * fucxmul + fucxadd;\n"
"gl_FragColor.a *= factor.a;\n"
"gl_FragColor = mix(vec4(1.0,1,1.0,1), gl_FragColor, gl_FragColor.a);\n"
"}\n";

extern const char* pSource_VInstancedPosition3dTexTG;
const char* pSource_VInstancedPosition3dTexTG = 
"#extension GL_ARB_draw_instanced : enable\n"
"uniform mat4 vfmuniforms[24];\n"
"uniform vec4 vfuniforms[48];\n"
"attribute vec4 pos;\n"
"varying vec2 tc;\n"
"void main() {\n"
"gl_Position = (pos) * ( vfmuniforms[int(gl_InstanceID * 1.0 + 0.0+ 0.1)]);\n"
"tc.x = dot(pos, vfuniforms[int(gl_InstanceID * 2.0 + 0.0+ 0.1 + 0.0)]);\n"
"tc.y = dot(pos, vfuniforms[int(gl_InstanceID * 2.0 + 0.0+ 0.1 + 1.0)]);\n"
"}\n";

extern const char* pSource_VInstancedPosition3dTexTGEAlpha;
const char* pSource_VInstancedPosition3dTexTGEAlpha = 
"#extension GL_ARB_draw_instanced : enable\n"
"uniform mat4 vfmuniforms[24];\n"
"uniform vec4 vfuniforms[48];\n"
"attribute vec4 afactor;\n"
"attribute vec4 pos;\n"
"varying vec4 factor;\n"
"varying vec2 tc;\n"
"void main() {\n"
"gl_Position = (pos) * ( vfmuniforms[int(gl_InstanceID * 1.0 + 0.0+ 0.1)]);\n"
"tc.x = dot(pos, vfuniforms[int(gl_InstanceID * 2.0 + 0.0+ 0.1 + 0.0)]);\n"
"tc.y = dot(pos, vfuniforms[int(gl_InstanceID * 2.0 + 0.0+ 0.1 + 1.0)]);\n"
"factor = afactor;\n"
"}\n";

extern const char* pSource_VInstancedPosition3dTexTGCxform;
const char* pSource_VInstancedPosition3dTexTGCxform = 
"#extension GL_ARB_draw_instanced : enable\n"
"uniform mat4 vfmuniforms[24];\n"
"uniform vec4 vfuniforms[96];\n"
"attribute vec4 pos;\n"
"varying vec4 fucxadd;\n"
"varying vec4 fucxmul;\n"
"varying vec2 tc;\n"
"void main() {\n"
"gl_Position = (pos) * ( vfmuniforms[int(gl_InstanceID * 1.0 + 0.0+ 0.1)]);\n"
"tc.x = dot(pos, vfuniforms[int(gl_InstanceID * 4.0 + 2.0+ 0.1 + 0.0)]);\n"
"tc.y = dot(pos, vfuniforms[int(gl_InstanceID * 4.0 + 2.0+ 0.1 + 1.0)]);\n"
"fucxadd = vfuniforms[int(gl_InstanceID * 4.0 + 0.0+ 0.1)];\n"
"fucxmul = vfuniforms[int(gl_InstanceID * 4.0 + 1.0+ 0.1)];\n"
"}\n";

extern const char* pSource_VInstancedPosition3dTexTGCxformEAlpha;
const char* pSource_VInstancedPosition3dTexTGCxformEAlpha = 
"#extension GL_ARB_draw_instanced : enable\n"
"uniform mat4 vfmuniforms[24];\n"
"uniform vec4 vfuniforms[96];\n"
"attribute vec4 afactor;\n"
"attribute vec4 pos;\n"
"varying vec4 factor;\n"
"varying vec4 fucxadd;\n"
"varying vec4 fucxmul;\n"
"varying vec2 tc;\n"
"void main() {\n"
"gl_Position = (pos) * ( vfmuniforms[int(gl_InstanceID * 1.0 + 0.0+ 0.1)]);\n"
"tc.x = dot(pos, vfuniforms[int(gl_InstanceID * 4.0 + 2.0+ 0.1 + 0.0)]);\n"
"tc.y = dot(pos, vfuniforms[int(gl_InstanceID * 4.0 + 2.0+ 0.1 + 1.0)]);\n"
"fucxadd = vfuniforms[int(gl_InstanceID * 4.0 + 0.0+ 0.1)];\n"
"fucxmul = vfuniforms[int(gl_InstanceID * 4.0 + 1.0+ 0.1)];\n"
"factor = afactor;\n"
"}\n";

extern const char* pSource_VInstancedPosition3dVertex;
const char* pSource_VInstancedPosition3dVertex = 
"#extension GL_ARB_draw_instanced : enable\n"
"uniform mat4 vfmuniforms[24];\n"
"attribute vec4 acolor;\n"
"attribute vec4 pos;\n"
"varying vec4 color;\n"
"void main() {\n"
"gl_Position = (pos) * ( vfmuniforms[int(gl_InstanceID * 1.0 + 0.0+ 0.1)]);\n"
"color = acolor;\n"
"}\n";

extern const char* pSource_VInstancedPosition3dVertexEAlpha;
const char* pSource_VInstancedPosition3dVertexEAlpha = 
"#extension GL_ARB_draw_instanced : enable\n"
"uniform mat4 vfmuniforms[24];\n"
"attribute vec4 acolor;\n"
"attribute vec4 afactor;\n"
"attribute vec4 pos;\n"
"varying vec4 color;\n"
"varying vec4 factor;\n"
"void main() {\n"
"gl_Position = (pos) * ( vfmuniforms[int(gl_InstanceID * 1.0 + 0.0+ 0.1)]);\n"
"color = acolor;\n"
"factor = afactor;\n"
"}\n";

extern const char* pSource_VInstancedPosition3dVertexCxform;
const char* pSource_VInstancedPosition3dVertexCxform = 
"#extension GL_ARB_draw_instanced : enable\n"
"uniform mat4 vfmuniforms[24];\n"
"uniform vec4 vfuniforms[48];\n"
"attribute vec4 acolor;\n"
"attribute vec4 pos;\n"
"varying vec4 color;\n"
"varying vec4 fucxadd;\n"
"varying vec4 fucxmul;\n"
"void main() {\n"
"gl_Position = (pos) * ( vfmuniforms[int(gl_InstanceID * 1.0 + 0.0+ 0.1)]);\n"
"color = acolor;\n"
"fucxadd = vfuniforms[int(gl_InstanceID * 2.0 + 0.0+ 0.1)];\n"
"fucxmul = vfuniforms[int(gl_InstanceID * 2.0 + 1.0+ 0.1)];\n"
"}\n";

extern const char* pSource_VInstancedPosition3dVertexCxformEAlpha;
const char* pSource_VInstancedPosition3dVertexCxformEAlpha = 
"#extension GL_ARB_draw_instanced : enable\n"
"uniform mat4 vfmuniforms[24];\n"
"uniform vec4 vfuniforms[48];\n"
"attribute vec4 acolor;\n"
"attribute vec4 afactor;\n"
"attribute vec4 pos;\n"
"varying vec4 color;\n"
"varying vec4 factor;\n"
"varying vec4 fucxadd;\n"
"varying vec4 fucxmul;\n"
"void main() {\n"
"gl_Position = (pos) * ( vfmuniforms[int(gl_InstanceID * 1.0 + 0.0+ 0.1)]);\n"
"color = acolor;\n"
"fucxadd = vfuniforms[int(gl_InstanceID * 2.0 + 0.0+ 0.1)];\n"
"fucxmul = vfuniforms[int(gl_InstanceID * 2.0 + 1.0+ 0.1)];\n"
"factor = afactor;\n"
"}\n";

extern const char* pSource_VInstancedPosition3dTexTGTexTG;
const char* pSource_VInstancedPosition3dTexTGTexTG = 
"#extension GL_ARB_draw_instanced : enable\n"
"uniform mat4 vfmuniforms[24];\n"
"uniform vec4 vfuniforms[96];\n"
"attribute vec4 afactor;\n"
"attribute vec4 pos;\n"
"varying vec4 factor;\n"
"varying vec2 tc0;\n"
"varying vec2 tc1;\n"
"void main() {\n"
"gl_Position = (pos) * ( vfmuniforms[int(gl_InstanceID * 1.0 + 0.0+ 0.1)]);\n"
"tc0.x = dot(pos, vfuniforms[int(gl_InstanceID * 4.0 + 0.0+ 0.1 + 0.0)]);\n"
"tc0.y = dot(pos, vfuniforms[int(gl_InstanceID * 4.0 + 0.0+ 0.1 + 1.0)]);\n"
"tc1.x = dot(pos, vfuniforms[int(gl_InstanceID * 4.0 + 0.0+ 0.1 + 2.0)]);\n"
"tc1.y = dot(pos, vfuniforms[int(gl_InstanceID * 4.0 + 0.0+ 0.1 + 3.0)]);\n"
"factor = afactor;\n"
"}\n";

extern const char* pSource_VInstancedPosition3dTexTGTexTGCxform;
const char* pSource_VInstancedPosition3dTexTGTexTGCxform = 
"#extension GL_ARB_draw_instanced : enable\n"
"uniform mat4 vfmuniforms[24];\n"
"uniform vec4 vfuniforms[144];\n"
"attribute vec4 afactor;\n"
"attribute vec4 pos;\n"
"varying vec4 factor;\n"
"varying vec4 fucxadd;\n"
"varying vec4 fucxmul;\n"
"varying vec2 tc0;\n"
"varying vec2 tc1;\n"
"void main() {\n"
"gl_Position = (pos) * ( vfmuniforms[int(gl_InstanceID * 1.0 + 0.0+ 0.1)]);\n"
"tc0.x = dot(pos, vfuniforms[int(gl_InstanceID * 6.0 + 2.0+ 0.1 + 0.0)]);\n"
"tc0.y = dot(pos, vfuniforms[int(gl_InstanceID * 6.0 + 2.0+ 0.1 + 1.0)]);\n"
"tc1.x = dot(pos, vfuniforms[int(gl_InstanceID * 6.0 + 2.0+ 0.1 + 2.0)]);\n"
"tc1.y = dot(pos, vfuniforms[int(gl_InstanceID * 6.0 + 2.0+ 0.1 + 3.0)]);\n"
"fucxadd = vfuniforms[int(gl_InstanceID * 6.0 + 0.0+ 0.1)];\n"
"fucxmul = vfuniforms[int(gl_InstanceID * 6.0 + 1.0+ 0.1)];\n"
"factor = afactor;\n"
"}\n";

extern const char* pSource_VInstancedPosition3dTexTGVertex;
const char* pSource_VInstancedPosition3dTexTGVertex = 
"#extension GL_ARB_draw_instanced : enable\n"
"uniform mat4 vfmuniforms[24];\n"
"uniform vec4 vfuniforms[48];\n"
"attribute vec4 acolor;\n"
"attribute vec4 afactor;\n"
"attribute vec4 pos;\n"
"varying vec4 color;\n"
"varying vec4 factor;\n"
"varying vec2 tc;\n"
"void main() {\n"
"gl_Position = (pos) * ( vfmuniforms[int(gl_InstanceID * 1.0 + 0.0+ 0.1)]);\n"
"color = acolor;\n"
"tc.x = dot(pos, vfuniforms[int(gl_InstanceID * 2.0 + 0.0+ 0.1 + 0.0)]);\n"
"tc.y = dot(pos, vfuniforms[int(gl_InstanceID * 2.0 + 0.0+ 0.1 + 1.0)]);\n"
"factor = afactor;\n"
"}\n";

extern const char* pSource_VInstancedPosition3dTexTGVertexCxform;
const char* pSource_VInstancedPosition3dTexTGVertexCxform = 
"#extension GL_ARB_draw_instanced : enable\n"
"uniform mat4 vfmuniforms[24];\n"
"uniform vec4 vfuniforms[96];\n"
"attribute vec4 acolor;\n"
"attribute vec4 afactor;\n"
"attribute vec4 pos;\n"
"varying vec4 color;\n"
"varying vec4 factor;\n"
"varying vec4 fucxadd;\n"
"varying vec4 fucxmul;\n"
"varying vec2 tc;\n"
"void main() {\n"
"gl_Position = (pos) * ( vfmuniforms[int(gl_InstanceID * 1.0 + 0.0+ 0.1)]);\n"
"color = acolor;\n"
"tc.x = dot(pos, vfuniforms[int(gl_InstanceID * 4.0 + 2.0+ 0.1 + 0.0)]);\n"
"tc.y = dot(pos, vfuniforms[int(gl_InstanceID * 4.0 + 2.0+ 0.1 + 1.0)]);\n"
"fucxadd = vfuniforms[int(gl_InstanceID * 4.0 + 0.0+ 0.1)];\n"
"fucxmul = vfuniforms[int(gl_InstanceID * 4.0 + 1.0+ 0.1)];\n"
"factor = afactor;\n"
"}\n";

extern const char* pSource_VSolid;
const char* pSource_VSolid = 
"uniform vec4 mvp[2];\n"
"attribute vec4 pos;\n"
"void main() {\n"
"gl_Position = vec4(0.0,0,0.0,1);\n"
"gl_Position.x = dot(pos, mvp[int(0.0)]);\n"
"gl_Position.y = dot(pos, mvp[int(1.0)]);\n"
"}\n";

extern const char* pSource_FSolid;
const char* pSource_FSolid = 
"uniform vec4 cxmul;\n"
"void main() {\n"
"gl_FragColor = cxmul;\n"
"}\n";

extern const char* pSource_FSolidMul;
const char* pSource_FSolidMul = 
"uniform vec4 cxmul;\n"
"void main() {\n"
"gl_FragColor = cxmul;\n"
"gl_FragColor = mix(vec4(1.0,1,1.0,1), gl_FragColor, gl_FragColor.a);\n"
"}\n";

extern const char* pSource_VText;
const char* pSource_VText = 
"uniform vec4 cxadd;\n"
"uniform vec4 cxmul;\n"
"uniform vec4 mvp[2];\n"
"attribute vec4 acolor;\n"
"attribute vec2 atc;\n"
"attribute vec4 pos;\n"
"varying vec2 tc;\n"
"varying vec4 vcolor;\n"
"void main() {\n"
"gl_Position = vec4(0.0,0,0.0,1);\n"
"gl_Position.x = dot(pos, mvp[int(0.0)]);\n"
"gl_Position.y = dot(pos, mvp[int(1.0)]);\n"
"vcolor = acolor * cxmul + cxadd;\n"
"tc = atc;\n"
"}\n";

extern const char* pSource_FText;
const char* pSource_FText = 
"uniform sampler2D tex;\n"
"varying vec2 tc;\n"
"varying vec4 vcolor;\n"
"void main() {\n"
"vec4 c = vcolor;\n"
"c.a = c.a * texture2D(tex, tc).a;\n"
"gl_FragColor = c;\n"
"}\n";

extern const char* pSource_FTextMul;
const char* pSource_FTextMul = 
"uniform sampler2D tex;\n"
"varying vec2 tc;\n"
"varying vec4 vcolor;\n"
"void main() {\n"
"vec4 c = vcolor;\n"
"c.a = c.a * texture2D(tex, tc).a;\n"
"gl_FragColor = c;\n"
"gl_FragColor = mix(vec4(1.0,1,1.0,1), gl_FragColor, gl_FragColor.a);\n"
"}\n";

extern const char* pSource_VTextColor;
const char* pSource_VTextColor = 
"uniform vec4 mvp[2];\n"
"attribute vec2 atc;\n"
"attribute vec4 pos;\n"
"varying vec2 tc;\n"
"void main() {\n"
"gl_Position = vec4(0.0,0,0.0,1);\n"
"gl_Position.x = dot(pos, mvp[int(0.0)]);\n"
"gl_Position.y = dot(pos, mvp[int(1.0)]);\n"
"tc = atc;\n"
"}\n";

extern const char* pSource_VTextColorCxform;
const char* pSource_VTextColorCxform = 
"uniform vec4 cxadd;\n"
"uniform vec4 cxmul;\n"
"uniform vec4 mvp[2];\n"
"attribute vec2 atc;\n"
"attribute vec4 pos;\n"
"varying vec4 fucxadd;\n"
"varying vec4 fucxmul;\n"
"varying vec2 tc;\n"
"void main() {\n"
"gl_Position = vec4(0.0,0,0.0,1);\n"
"gl_Position.x = dot(pos, mvp[int(0.0)]);\n"
"gl_Position.y = dot(pos, mvp[int(1.0)]);\n"
"tc = atc;\n"
"fucxadd = cxadd;\n"
"fucxmul = cxmul;\n"
"}\n";

extern const char* pSource_VPosition3dSolid;
const char* pSource_VPosition3dSolid = 
"uniform mat4 mvp;\n"
"attribute vec4 pos;\n"
"void main() {\n"
"gl_Position = (pos) * ( mvp);\n"
"}\n";

extern const char* pSource_VPosition3dText;
const char* pSource_VPosition3dText = 
"uniform vec4 cxadd;\n"
"uniform vec4 cxmul;\n"
"uniform mat4 mvp;\n"
"attribute vec4 acolor;\n"
"attribute vec2 atc;\n"
"attribute vec4 pos;\n"
"varying vec2 tc;\n"
"varying vec4 vcolor;\n"
"void main() {\n"
"gl_Position = (pos) * ( mvp);\n"
"vcolor = acolor * cxmul + cxadd;\n"
"tc = atc;\n"
"}\n";

extern const char* pSource_VPosition3dTextColor;
const char* pSource_VPosition3dTextColor = 
"uniform mat4 mvp;\n"
"attribute vec2 atc;\n"
"attribute vec4 pos;\n"
"varying vec2 tc;\n"
"void main() {\n"
"gl_Position = (pos) * ( mvp);\n"
"tc = atc;\n"
"}\n";

extern const char* pSource_VPosition3dTextColorCxform;
const char* pSource_VPosition3dTextColorCxform = 
"uniform vec4 cxadd;\n"
"uniform vec4 cxmul;\n"
"uniform mat4 mvp;\n"
"attribute vec2 atc;\n"
"attribute vec4 pos;\n"
"varying vec4 fucxadd;\n"
"varying vec4 fucxmul;\n"
"varying vec2 tc;\n"
"void main() {\n"
"gl_Position = (pos) * ( mvp);\n"
"tc = atc;\n"
"fucxadd = cxadd;\n"
"fucxmul = cxmul;\n"
"}\n";

extern const char* pSource_VBatchSolid;
const char* pSource_VBatchSolid = 
"uniform vec4 vfuniforms[48];\n"
"attribute vec4 pos;\n"
"attribute float vbatch;\n"
"void main() {\n"
"gl_Position = vec4(0.0,0,0.0,1);\n"
"gl_Position.x = dot(pos, vfuniforms[int(vbatch * 2.0 + 0.0+ 0.1 + 0.0)]);\n"
"gl_Position.y = dot(pos, vfuniforms[int(vbatch * 2.0 + 0.0+ 0.1 + 1.0)]);\n"
"}\n";

extern const char* pSource_VBatchText;
const char* pSource_VBatchText = 
"uniform vec4 vfuniforms[96];\n"
"attribute vec4 acolor;\n"
"attribute vec2 atc;\n"
"attribute vec4 pos;\n"
"attribute float vbatch;\n"
"varying vec2 tc;\n"
"varying vec4 vcolor;\n"
"void main() {\n"
"gl_Position = vec4(0.0,0,0.0,1);\n"
"gl_Position.x = dot(pos, vfuniforms[int(vbatch * 4.0 + 2.0+ 0.1 + 0.0)]);\n"
"gl_Position.y = dot(pos, vfuniforms[int(vbatch * 4.0 + 2.0+ 0.1 + 1.0)]);\n"
"vcolor = acolor * vfuniforms[int(vbatch * 4.0 + 1.0+ 0.1)] + vfuniforms[int(vbatch * 4.0 + 0.0+ 0.1)];\n"
"tc = atc;\n"
"}\n";

extern const char* pSource_VBatchTextColor;
const char* pSource_VBatchTextColor = 
"uniform vec4 vfuniforms[48];\n"
"attribute vec2 atc;\n"
"attribute vec4 pos;\n"
"attribute float vbatch;\n"
"varying vec2 tc;\n"
"void main() {\n"
"gl_Position = vec4(0.0,0,0.0,1);\n"
"gl_Position.x = dot(pos, vfuniforms[int(vbatch * 2.0 + 0.0+ 0.1 + 0.0)]);\n"
"gl_Position.y = dot(pos, vfuniforms[int(vbatch * 2.0 + 0.0+ 0.1 + 1.0)]);\n"
"tc = atc;\n"
"}\n";

extern const char* pSource_VBatchTextColorCxform;
const char* pSource_VBatchTextColorCxform = 
"uniform vec4 vfuniforms[96];\n"
"attribute vec2 atc;\n"
"attribute vec4 pos;\n"
"attribute float vbatch;\n"
"varying vec4 fucxadd;\n"
"varying vec4 fucxmul;\n"
"varying vec2 tc;\n"
"void main() {\n"
"gl_Position = vec4(0.0,0,0.0,1);\n"
"gl_Position.x = dot(pos, vfuniforms[int(vbatch * 4.0 + 2.0+ 0.1 + 0.0)]);\n"
"gl_Position.y = dot(pos, vfuniforms[int(vbatch * 4.0 + 2.0+ 0.1 + 1.0)]);\n"
"tc = atc;\n"
"fucxadd = vfuniforms[int(vbatch * 4.0 + 0.0+ 0.1)];\n"
"fucxmul = vfuniforms[int(vbatch * 4.0 + 1.0+ 0.1)];\n"
"}\n";

extern const char* pSource_VBatchPosition3dSolid;
const char* pSource_VBatchPosition3dSolid = 
"uniform mat4 vfmuniforms[24];\n"
"attribute vec4 pos;\n"
"attribute float vbatch;\n"
"void main() {\n"
"gl_Position = (pos) * ( vfmuniforms[int(vbatch * 1.0 + 0.0+ 0.1)]);\n"
"}\n";

extern const char* pSource_VBatchPosition3dText;
const char* pSource_VBatchPosition3dText = 
"uniform mat4 vfmuniforms[24];\n"
"uniform vec4 vfuniforms[48];\n"
"attribute vec4 acolor;\n"
"attribute vec2 atc;\n"
"attribute vec4 pos;\n"
"attribute float vbatch;\n"
"varying vec2 tc;\n"
"varying vec4 vcolor;\n"
"void main() {\n"
"gl_Position = (pos) * ( vfmuniforms[int(vbatch * 1.0 + 0.0+ 0.1)]);\n"
"vcolor = acolor * vfuniforms[int(vbatch * 2.0 + 1.0+ 0.1)] + vfuniforms[int(vbatch * 2.0 + 0.0+ 0.1)];\n"
"tc = atc;\n"
"}\n";

extern const char* pSource_VBatchPosition3dTextColor;
const char* pSource_VBatchPosition3dTextColor = 
"uniform mat4 vfmuniforms[24];\n"
"attribute vec2 atc;\n"
"attribute vec4 pos;\n"
"attribute float vbatch;\n"
"varying vec2 tc;\n"
"void main() {\n"
"gl_Position = (pos) * ( vfmuniforms[int(vbatch * 1.0 + 0.0+ 0.1)]);\n"
"tc = atc;\n"
"}\n";

extern const char* pSource_VBatchPosition3dTextColorCxform;
const char* pSource_VBatchPosition3dTextColorCxform = 
"uniform mat4 vfmuniforms[24];\n"
"uniform vec4 vfuniforms[48];\n"
"attribute vec2 atc;\n"
"attribute vec4 pos;\n"
"attribute float vbatch;\n"
"varying vec4 fucxadd;\n"
"varying vec4 fucxmul;\n"
"varying vec2 tc;\n"
"void main() {\n"
"gl_Position = (pos) * ( vfmuniforms[int(vbatch * 1.0 + 0.0+ 0.1)]);\n"
"tc = atc;\n"
"fucxadd = vfuniforms[int(vbatch * 2.0 + 0.0+ 0.1)];\n"
"fucxmul = vfuniforms[int(vbatch * 2.0 + 1.0+ 0.1)];\n"
"}\n";

extern const char* pSource_VInstancedSolid;
const char* pSource_VInstancedSolid = 
"#extension GL_ARB_draw_instanced : enable\n"
"uniform vec4 vfuniforms[48];\n"
"attribute vec4 pos;\n"
"void main() {\n"
"gl_Position = vec4(0.0,0,0.0,1);\n"
"gl_Position.x = dot(pos, vfuniforms[int(gl_InstanceID * 2.0 + 0.0+ 0.1 + 0.0)]);\n"
"gl_Position.y = dot(pos, vfuniforms[int(gl_InstanceID * 2.0 + 0.0+ 0.1 + 1.0)]);\n"
"}\n";

extern const char* pSource_FInstancedSolid;
const char* pSource_FInstancedSolid = 
"#extension GL_ARB_draw_instanced : enable\n"
"uniform vec4 cxmul;\n"
"void main() {\n"
"gl_FragColor = cxmul;\n"
"}\n";

extern const char* pSource_FInstancedSolidMul;
const char* pSource_FInstancedSolidMul = 
"#extension GL_ARB_draw_instanced : enable\n"
"uniform vec4 cxmul;\n"
"void main() {\n"
"gl_FragColor = cxmul;\n"
"gl_FragColor = mix(vec4(1.0,1,1.0,1), gl_FragColor, gl_FragColor.a);\n"
"}\n";

extern const char* pSource_VInstancedText;
const char* pSource_VInstancedText = 
"#extension GL_ARB_draw_instanced : enable\n"
"uniform vec4 vfuniforms[96];\n"
"attribute vec4 acolor;\n"
"attribute vec2 atc;\n"
"attribute vec4 pos;\n"
"varying vec2 tc;\n"
"varying vec4 vcolor;\n"
"void main() {\n"
"gl_Position = vec4(0.0,0,0.0,1);\n"
"gl_Position.x = dot(pos, vfuniforms[int(gl_InstanceID * 4.0 + 2.0+ 0.1 + 0.0)]);\n"
"gl_Position.y = dot(pos, vfuniforms[int(gl_InstanceID * 4.0 + 2.0+ 0.1 + 1.0)]);\n"
"vcolor = acolor * vfuniforms[int(gl_InstanceID * 4.0 + 1.0+ 0.1)] + vfuniforms[int(gl_InstanceID * 4.0 + 0.0+ 0.1)];\n"
"tc = atc;\n"
"}\n";

extern const char* pSource_FInstancedText;
const char* pSource_FInstancedText = 
"#extension GL_ARB_draw_instanced : enable\n"
"uniform sampler2D tex;\n"
"varying vec2 tc;\n"
"varying vec4 vcolor;\n"
"void main() {\n"
"vec4 c = vcolor;\n"
"c.a = c.a * texture2D(tex, tc).a;\n"
"gl_FragColor = c;\n"
"}\n";

extern const char* pSource_FInstancedTextMul;
const char* pSource_FInstancedTextMul = 
"#extension GL_ARB_draw_instanced : enable\n"
"uniform sampler2D tex;\n"
"varying vec2 tc;\n"
"varying vec4 vcolor;\n"
"void main() {\n"
"vec4 c = vcolor;\n"
"c.a = c.a * texture2D(tex, tc).a;\n"
"gl_FragColor = c;\n"
"gl_FragColor = mix(vec4(1.0,1,1.0,1), gl_FragColor, gl_FragColor.a);\n"
"}\n";

extern const char* pSource_VInstancedTextColor;
const char* pSource_VInstancedTextColor = 
"#extension GL_ARB_draw_instanced : enable\n"
"uniform vec4 vfuniforms[48];\n"
"attribute vec2 atc;\n"
"attribute vec4 pos;\n"
"varying vec2 tc;\n"
"void main() {\n"
"gl_Position = vec4(0.0,0,0.0,1);\n"
"gl_Position.x = dot(pos, vfuniforms[int(gl_InstanceID * 2.0 + 0.0+ 0.1 + 0.0)]);\n"
"gl_Position.y = dot(pos, vfuniforms[int(gl_InstanceID * 2.0 + 0.0+ 0.1 + 1.0)]);\n"
"tc = atc;\n"
"}\n";

extern const char* pSource_VInstancedTextColorCxform;
const char* pSource_VInstancedTextColorCxform = 
"#extension GL_ARB_draw_instanced : enable\n"
"uniform vec4 vfuniforms[96];\n"
"attribute vec2 atc;\n"
"attribute vec4 pos;\n"
"varying vec4 fucxadd;\n"
"varying vec4 fucxmul;\n"
"varying vec2 tc;\n"
"void main() {\n"
"gl_Position = vec4(0.0,0,0.0,1);\n"
"gl_Position.x = dot(pos, vfuniforms[int(gl_InstanceID * 4.0 + 2.0+ 0.1 + 0.0)]);\n"
"gl_Position.y = dot(pos, vfuniforms[int(gl_InstanceID * 4.0 + 2.0+ 0.1 + 1.0)]);\n"
"tc = atc;\n"
"fucxadd = vfuniforms[int(gl_InstanceID * 4.0 + 0.0+ 0.1)];\n"
"fucxmul = vfuniforms[int(gl_InstanceID * 4.0 + 1.0+ 0.1)];\n"
"}\n";

extern const char* pSource_VInstancedPosition3dSolid;
const char* pSource_VInstancedPosition3dSolid = 
"#extension GL_ARB_draw_instanced : enable\n"
"uniform mat4 vfmuniforms[24];\n"
"attribute vec4 pos;\n"
"void main() {\n"
"gl_Position = (pos) * ( vfmuniforms[int(gl_InstanceID * 1.0 + 0.0+ 0.1)]);\n"
"}\n";

extern const char* pSource_VInstancedPosition3dText;
const char* pSource_VInstancedPosition3dText = 
"#extension GL_ARB_draw_instanced : enable\n"
"uniform mat4 vfmuniforms[24];\n"
"uniform vec4 vfuniforms[48];\n"
"attribute vec4 acolor;\n"
"attribute vec2 atc;\n"
"attribute vec4 pos;\n"
"varying vec2 tc;\n"
"varying vec4 vcolor;\n"
"void main() {\n"
"gl_Position = (pos) * ( vfmuniforms[int(gl_InstanceID * 1.0 + 0.0+ 0.1)]);\n"
"vcolor = acolor * vfuniforms[int(gl_InstanceID * 2.0 + 1.0+ 0.1)] + vfuniforms[int(gl_InstanceID * 2.0 + 0.0+ 0.1)];\n"
"tc = atc;\n"
"}\n";

extern const char* pSource_VInstancedPosition3dTextColor;
const char* pSource_VInstancedPosition3dTextColor = 
"#extension GL_ARB_draw_instanced : enable\n"
"uniform mat4 vfmuniforms[24];\n"
"attribute vec2 atc;\n"
"attribute vec4 pos;\n"
"varying vec2 tc;\n"
"void main() {\n"
"gl_Position = (pos) * ( vfmuniforms[int(gl_InstanceID * 1.0 + 0.0+ 0.1)]);\n"
"tc = atc;\n"
"}\n";

extern const char* pSource_VInstancedPosition3dTextColorCxform;
const char* pSource_VInstancedPosition3dTextColorCxform = 
"#extension GL_ARB_draw_instanced : enable\n"
"uniform mat4 vfmuniforms[24];\n"
"uniform vec4 vfuniforms[48];\n"
"attribute vec2 atc;\n"
"attribute vec4 pos;\n"
"varying vec4 fucxadd;\n"
"varying vec4 fucxmul;\n"
"varying vec2 tc;\n"
"void main() {\n"
"gl_Position = (pos) * ( vfmuniforms[int(gl_InstanceID * 1.0 + 0.0+ 0.1)]);\n"
"tc = atc;\n"
"fucxadd = vfuniforms[int(gl_InstanceID * 2.0 + 0.0+ 0.1)];\n"
"fucxmul = vfuniforms[int(gl_InstanceID * 2.0 + 1.0+ 0.1)];\n"
"}\n";

extern const char* pSource_FYUV;
const char* pSource_FYUV = 
"uniform sampler2D tex[3];\n"
"varying vec2 tc;\n"
"void main() {\n"
"float c0 = float((texture2D(tex[int(0.0)], tc).a - 16./255.) * 1.164);\n"
"float U0 = float(texture2D(tex[int(1.0)], tc).a - 128./255.);\n"
"float V0 = float(texture2D(tex[int(2.0)], tc).a - 128./255.);\n"
"vec4 c = vec4(c0,c0,c0,c0);\n"
"vec4 U = vec4(U0,U0,U0,U0);\n"
"vec4 V = vec4(V0,V0,V0,V0);\n"
"c += V * vec4(1.596, -0.813, 0.0, 0.0);\n"
"c += U * vec4(0.0, -0.392, 2.017, 0.0);\n"
"c.a = 1.0;\n"
"gl_FragColor = c;\n"
"}\n";

extern const char* pSource_FYUVMul;
const char* pSource_FYUVMul = 
"uniform sampler2D tex[3];\n"
"varying vec2 tc;\n"
"void main() {\n"
"float c0 = float((texture2D(tex[int(0.0)], tc).a - 16./255.) * 1.164);\n"
"float U0 = float(texture2D(tex[int(1.0)], tc).a - 128./255.);\n"
"float V0 = float(texture2D(tex[int(2.0)], tc).a - 128./255.);\n"
"vec4 c = vec4(c0,c0,c0,c0);\n"
"vec4 U = vec4(U0,U0,U0,U0);\n"
"vec4 V = vec4(V0,V0,V0,V0);\n"
"c += V * vec4(1.596, -0.813, 0.0, 0.0);\n"
"c += U * vec4(0.0, -0.392, 2.017, 0.0);\n"
"c.a = 1.0;\n"
"gl_FragColor = c;\n"
"gl_FragColor = mix(vec4(1.0,1,1.0,1), gl_FragColor, gl_FragColor.a);\n"
"}\n";

extern const char* pSource_FYUVEAlpha;
const char* pSource_FYUVEAlpha = 
"uniform sampler2D tex[3];\n"
"varying vec4 factor;\n"
"varying vec2 tc;\n"
"void main() {\n"
"float c0 = float((texture2D(tex[int(0.0)], tc).a - 16./255.) * 1.164);\n"
"float U0 = float(texture2D(tex[int(1.0)], tc).a - 128./255.);\n"
"float V0 = float(texture2D(tex[int(2.0)], tc).a - 128./255.);\n"
"vec4 c = vec4(c0,c0,c0,c0);\n"
"vec4 U = vec4(U0,U0,U0,U0);\n"
"vec4 V = vec4(V0,V0,V0,V0);\n"
"c += V * vec4(1.596, -0.813, 0.0, 0.0);\n"
"c += U * vec4(0.0, -0.392, 2.017, 0.0);\n"
"c.a = 1.0;\n"
"gl_FragColor = c;\n"
"gl_FragColor.a *= factor.a;\n"
"}\n";

extern const char* pSource_FYUVEAlphaMul;
const char* pSource_FYUVEAlphaMul = 
"uniform sampler2D tex[3];\n"
"varying vec4 factor;\n"
"varying vec2 tc;\n"
"void main() {\n"
"float c0 = float((texture2D(tex[int(0.0)], tc).a - 16./255.) * 1.164);\n"
"float U0 = float(texture2D(tex[int(1.0)], tc).a - 128./255.);\n"
"float V0 = float(texture2D(tex[int(2.0)], tc).a - 128./255.);\n"
"vec4 c = vec4(c0,c0,c0,c0);\n"
"vec4 U = vec4(U0,U0,U0,U0);\n"
"vec4 V = vec4(V0,V0,V0,V0);\n"
"c += V * vec4(1.596, -0.813, 0.0, 0.0);\n"
"c += U * vec4(0.0, -0.392, 2.017, 0.0);\n"
"c.a = 1.0;\n"
"gl_FragColor = c;\n"
"gl_FragColor.a *= factor.a;\n"
"gl_FragColor = mix(vec4(1.0,1,1.0,1), gl_FragColor, gl_FragColor.a);\n"
"}\n";

extern const char* pSource_FYUVCxform;
const char* pSource_FYUVCxform = 
"uniform sampler2D tex[3];\n"
"varying vec4 fucxadd;\n"
"varying vec4 fucxmul;\n"
"varying vec2 tc;\n"
"void main() {\n"
"float c0 = float((texture2D(tex[int(0.0)], tc).a - 16./255.) * 1.164);\n"
"float U0 = float(texture2D(tex[int(1.0)], tc).a - 128./255.);\n"
"float V0 = float(texture2D(tex[int(2.0)], tc).a - 128./255.);\n"
"vec4 c = vec4(c0,c0,c0,c0);\n"
"vec4 U = vec4(U0,U0,U0,U0);\n"
"vec4 V = vec4(V0,V0,V0,V0);\n"
"c += V * vec4(1.596, -0.813, 0.0, 0.0);\n"
"c += U * vec4(0.0, -0.392, 2.017, 0.0);\n"
"c.a = 1.0;\n"
"gl_FragColor = c;\n"
"gl_FragColor = gl_FragColor * fucxmul + fucxadd;\n"
"}\n";

extern const char* pSource_FYUVCxformMul;
const char* pSource_FYUVCxformMul = 
"uniform sampler2D tex[3];\n"
"varying vec4 fucxadd;\n"
"varying vec4 fucxmul;\n"
"varying vec2 tc;\n"
"void main() {\n"
"float c0 = float((texture2D(tex[int(0.0)], tc).a - 16./255.) * 1.164);\n"
"float U0 = float(texture2D(tex[int(1.0)], tc).a - 128./255.);\n"
"float V0 = float(texture2D(tex[int(2.0)], tc).a - 128./255.);\n"
"vec4 c = vec4(c0,c0,c0,c0);\n"
"vec4 U = vec4(U0,U0,U0,U0);\n"
"vec4 V = vec4(V0,V0,V0,V0);\n"
"c += V * vec4(1.596, -0.813, 0.0, 0.0);\n"
"c += U * vec4(0.0, -0.392, 2.017, 0.0);\n"
"c.a = 1.0;\n"
"gl_FragColor = c;\n"
"gl_FragColor = gl_FragColor * fucxmul + fucxadd;\n"
"gl_FragColor = mix(vec4(1.0,1,1.0,1), gl_FragColor, gl_FragColor.a);\n"
"}\n";

extern const char* pSource_FYUVCxformEAlpha;
const char* pSource_FYUVCxformEAlpha = 
"uniform sampler2D tex[3];\n"
"varying vec4 factor;\n"
"varying vec4 fucxadd;\n"
"varying vec4 fucxmul;\n"
"varying vec2 tc;\n"
"void main() {\n"
"float c0 = float((texture2D(tex[int(0.0)], tc).a - 16./255.) * 1.164);\n"
"float U0 = float(texture2D(tex[int(1.0)], tc).a - 128./255.);\n"
"float V0 = float(texture2D(tex[int(2.0)], tc).a - 128./255.);\n"
"vec4 c = vec4(c0,c0,c0,c0);\n"
"vec4 U = vec4(U0,U0,U0,U0);\n"
"vec4 V = vec4(V0,V0,V0,V0);\n"
"c += V * vec4(1.596, -0.813, 0.0, 0.0);\n"
"c += U * vec4(0.0, -0.392, 2.017, 0.0);\n"
"c.a = 1.0;\n"
"gl_FragColor = c;\n"
"gl_FragColor = gl_FragColor * fucxmul + fucxadd;\n"
"gl_FragColor.a *= factor.a;\n"
"}\n";

extern const char* pSource_FYUVCxformEAlphaMul;
const char* pSource_FYUVCxformEAlphaMul = 
"uniform sampler2D tex[3];\n"
"varying vec4 factor;\n"
"varying vec4 fucxadd;\n"
"varying vec4 fucxmul;\n"
"varying vec2 tc;\n"
"void main() {\n"
"float c0 = float((texture2D(tex[int(0.0)], tc).a - 16./255.) * 1.164);\n"
"float U0 = float(texture2D(tex[int(1.0)], tc).a - 128./255.);\n"
"float V0 = float(texture2D(tex[int(2.0)], tc).a - 128./255.);\n"
"vec4 c = vec4(c0,c0,c0,c0);\n"
"vec4 U = vec4(U0,U0,U0,U0);\n"
"vec4 V = vec4(V0,V0,V0,V0);\n"
"c += V * vec4(1.596, -0.813, 0.0, 0.0);\n"
"c += U * vec4(0.0, -0.392, 2.017, 0.0);\n"
"c.a = 1.0;\n"
"gl_FragColor = c;\n"
"gl_FragColor = gl_FragColor * fucxmul + fucxadd;\n"
"gl_FragColor.a *= factor.a;\n"
"gl_FragColor = mix(vec4(1.0,1,1.0,1), gl_FragColor, gl_FragColor.a);\n"
"}\n";

extern const char* pSource_FYUVA;
const char* pSource_FYUVA = 
"uniform sampler2D tex[4];\n"
"varying vec2 tc;\n"
"void main() {\n"
"float c0 = float((texture2D(tex[int(0.0)], tc).a - 16./255.) * 1.164);\n"
"float U0 = float(texture2D(tex[int(1.0)], tc).a - 128./255.);\n"
"float V0 = float(texture2D(tex[int(2.0)], tc).a - 128./255.);\n"
"vec4 c = vec4(c0,c0,c0,c0);\n"
"vec4 U = vec4(U0,U0,U0,U0);\n"
"vec4 V = vec4(V0,V0,V0,V0);\n"
"c += V * vec4(1.596, -0.813, 0.0, 0.0);\n"
"c += U * vec4(0.0, -0.392, 2.017, 0.0);\n"
"c.a = texture2D(tex[int(3.0)], tc).a;\n"
"gl_FragColor = c;\n"
"}\n";

extern const char* pSource_FYUVAMul;
const char* pSource_FYUVAMul = 
"uniform sampler2D tex[4];\n"
"varying vec2 tc;\n"
"void main() {\n"
"float c0 = float((texture2D(tex[int(0.0)], tc).a - 16./255.) * 1.164);\n"
"float U0 = float(texture2D(tex[int(1.0)], tc).a - 128./255.);\n"
"float V0 = float(texture2D(tex[int(2.0)], tc).a - 128./255.);\n"
"vec4 c = vec4(c0,c0,c0,c0);\n"
"vec4 U = vec4(U0,U0,U0,U0);\n"
"vec4 V = vec4(V0,V0,V0,V0);\n"
"c += V * vec4(1.596, -0.813, 0.0, 0.0);\n"
"c += U * vec4(0.0, -0.392, 2.017, 0.0);\n"
"c.a = texture2D(tex[int(3.0)], tc).a;\n"
"gl_FragColor = c;\n"
"gl_FragColor = mix(vec4(1.0,1,1.0,1), gl_FragColor, gl_FragColor.a);\n"
"}\n";

extern const char* pSource_FYUVAEAlpha;
const char* pSource_FYUVAEAlpha = 
"uniform sampler2D tex[4];\n"
"varying vec4 factor;\n"
"varying vec2 tc;\n"
"void main() {\n"
"float c0 = float((texture2D(tex[int(0.0)], tc).a - 16./255.) * 1.164);\n"
"float U0 = float(texture2D(tex[int(1.0)], tc).a - 128./255.);\n"
"float V0 = float(texture2D(tex[int(2.0)], tc).a - 128./255.);\n"
"vec4 c = vec4(c0,c0,c0,c0);\n"
"vec4 U = vec4(U0,U0,U0,U0);\n"
"vec4 V = vec4(V0,V0,V0,V0);\n"
"c += V * vec4(1.596, -0.813, 0.0, 0.0);\n"
"c += U * vec4(0.0, -0.392, 2.017, 0.0);\n"
"c.a = texture2D(tex[int(3.0)], tc).a;\n"
"gl_FragColor = c;\n"
"gl_FragColor.a *= factor.a;\n"
"}\n";

extern const char* pSource_FYUVAEAlphaMul;
const char* pSource_FYUVAEAlphaMul = 
"uniform sampler2D tex[4];\n"
"varying vec4 factor;\n"
"varying vec2 tc;\n"
"void main() {\n"
"float c0 = float((texture2D(tex[int(0.0)], tc).a - 16./255.) * 1.164);\n"
"float U0 = float(texture2D(tex[int(1.0)], tc).a - 128./255.);\n"
"float V0 = float(texture2D(tex[int(2.0)], tc).a - 128./255.);\n"
"vec4 c = vec4(c0,c0,c0,c0);\n"
"vec4 U = vec4(U0,U0,U0,U0);\n"
"vec4 V = vec4(V0,V0,V0,V0);\n"
"c += V * vec4(1.596, -0.813, 0.0, 0.0);\n"
"c += U * vec4(0.0, -0.392, 2.017, 0.0);\n"
"c.a = texture2D(tex[int(3.0)], tc).a;\n"
"gl_FragColor = c;\n"
"gl_FragColor.a *= factor.a;\n"
"gl_FragColor = mix(vec4(1.0,1,1.0,1), gl_FragColor, gl_FragColor.a);\n"
"}\n";

extern const char* pSource_FYUVACxform;
const char* pSource_FYUVACxform = 
"uniform sampler2D tex[4];\n"
"varying vec4 fucxadd;\n"
"varying vec4 fucxmul;\n"
"varying vec2 tc;\n"
"void main() {\n"
"float c0 = float((texture2D(tex[int(0.0)], tc).a - 16./255.) * 1.164);\n"
"float U0 = float(texture2D(tex[int(1.0)], tc).a - 128./255.);\n"
"float V0 = float(texture2D(tex[int(2.0)], tc).a - 128./255.);\n"
"vec4 c = vec4(c0,c0,c0,c0);\n"
"vec4 U = vec4(U0,U0,U0,U0);\n"
"vec4 V = vec4(V0,V0,V0,V0);\n"
"c += V * vec4(1.596, -0.813, 0.0, 0.0);\n"
"c += U * vec4(0.0, -0.392, 2.017, 0.0);\n"
"c.a = texture2D(tex[int(3.0)], tc).a;\n"
"gl_FragColor = c;\n"
"gl_FragColor = gl_FragColor * fucxmul + fucxadd;\n"
"}\n";

extern const char* pSource_FYUVACxformMul;
const char* pSource_FYUVACxformMul = 
"uniform sampler2D tex[4];\n"
"varying vec4 fucxadd;\n"
"varying vec4 fucxmul;\n"
"varying vec2 tc;\n"
"void main() {\n"
"float c0 = float((texture2D(tex[int(0.0)], tc).a - 16./255.) * 1.164);\n"
"float U0 = float(texture2D(tex[int(1.0)], tc).a - 128./255.);\n"
"float V0 = float(texture2D(tex[int(2.0)], tc).a - 128./255.);\n"
"vec4 c = vec4(c0,c0,c0,c0);\n"
"vec4 U = vec4(U0,U0,U0,U0);\n"
"vec4 V = vec4(V0,V0,V0,V0);\n"
"c += V * vec4(1.596, -0.813, 0.0, 0.0);\n"
"c += U * vec4(0.0, -0.392, 2.017, 0.0);\n"
"c.a = texture2D(tex[int(3.0)], tc).a;\n"
"gl_FragColor = c;\n"
"gl_FragColor = gl_FragColor * fucxmul + fucxadd;\n"
"gl_FragColor = mix(vec4(1.0,1,1.0,1), gl_FragColor, gl_FragColor.a);\n"
"}\n";

extern const char* pSource_FYUVACxformEAlpha;
const char* pSource_FYUVACxformEAlpha = 
"uniform sampler2D tex[4];\n"
"varying vec4 factor;\n"
"varying vec4 fucxadd;\n"
"varying vec4 fucxmul;\n"
"varying vec2 tc;\n"
"void main() {\n"
"float c0 = float((texture2D(tex[int(0.0)], tc).a - 16./255.) * 1.164);\n"
"float U0 = float(texture2D(tex[int(1.0)], tc).a - 128./255.);\n"
"float V0 = float(texture2D(tex[int(2.0)], tc).a - 128./255.);\n"
"vec4 c = vec4(c0,c0,c0,c0);\n"
"vec4 U = vec4(U0,U0,U0,U0);\n"
"vec4 V = vec4(V0,V0,V0,V0);\n"
"c += V * vec4(1.596, -0.813, 0.0, 0.0);\n"
"c += U * vec4(0.0, -0.392, 2.017, 0.0);\n"
"c.a = texture2D(tex[int(3.0)], tc).a;\n"
"gl_FragColor = c;\n"
"gl_FragColor = gl_FragColor * fucxmul + fucxadd;\n"
"gl_FragColor.a *= factor.a;\n"
"}\n";

extern const char* pSource_FYUVACxformEAlphaMul;
const char* pSource_FYUVACxformEAlphaMul = 
"uniform sampler2D tex[4];\n"
"varying vec4 factor;\n"
"varying vec4 fucxadd;\n"
"varying vec4 fucxmul;\n"
"varying vec2 tc;\n"
"void main() {\n"
"float c0 = float((texture2D(tex[int(0.0)], tc).a - 16./255.) * 1.164);\n"
"float U0 = float(texture2D(tex[int(1.0)], tc).a - 128./255.);\n"
"float V0 = float(texture2D(tex[int(2.0)], tc).a - 128./255.);\n"
"vec4 c = vec4(c0,c0,c0,c0);\n"
"vec4 U = vec4(U0,U0,U0,U0);\n"
"vec4 V = vec4(V0,V0,V0,V0);\n"
"c += V * vec4(1.596, -0.813, 0.0, 0.0);\n"
"c += U * vec4(0.0, -0.392, 2.017, 0.0);\n"
"c.a = texture2D(tex[int(3.0)], tc).a;\n"
"gl_FragColor = c;\n"
"gl_FragColor = gl_FragColor * fucxmul + fucxadd;\n"
"gl_FragColor.a *= factor.a;\n"
"gl_FragColor = mix(vec4(1.0,1,1.0,1), gl_FragColor, gl_FragColor.a);\n"
"}\n";

extern const char* pSource_FInstancedYUV;
const char* pSource_FInstancedYUV = 
"#extension GL_ARB_draw_instanced : enable\n"
"uniform sampler2D tex[3];\n"
"varying vec2 tc;\n"
"void main() {\n"
"float c0 = float((texture2D(tex[int(0.0)], tc).a - 16./255.) * 1.164);\n"
"float U0 = float(texture2D(tex[int(1.0)], tc).a - 128./255.);\n"
"float V0 = float(texture2D(tex[int(2.0)], tc).a - 128./255.);\n"
"vec4 c = vec4(c0,c0,c0,c0);\n"
"vec4 U = vec4(U0,U0,U0,U0);\n"
"vec4 V = vec4(V0,V0,V0,V0);\n"
"c += V * vec4(1.596, -0.813, 0.0, 0.0);\n"
"c += U * vec4(0.0, -0.392, 2.017, 0.0);\n"
"c.a = 1.0;\n"
"gl_FragColor = c;\n"
"}\n";

extern const char* pSource_FInstancedYUVMul;
const char* pSource_FInstancedYUVMul = 
"#extension GL_ARB_draw_instanced : enable\n"
"uniform sampler2D tex[3];\n"
"varying vec2 tc;\n"
"void main() {\n"
"float c0 = float((texture2D(tex[int(0.0)], tc).a - 16./255.) * 1.164);\n"
"float U0 = float(texture2D(tex[int(1.0)], tc).a - 128./255.);\n"
"float V0 = float(texture2D(tex[int(2.0)], tc).a - 128./255.);\n"
"vec4 c = vec4(c0,c0,c0,c0);\n"
"vec4 U = vec4(U0,U0,U0,U0);\n"
"vec4 V = vec4(V0,V0,V0,V0);\n"
"c += V * vec4(1.596, -0.813, 0.0, 0.0);\n"
"c += U * vec4(0.0, -0.392, 2.017, 0.0);\n"
"c.a = 1.0;\n"
"gl_FragColor = c;\n"
"gl_FragColor = mix(vec4(1.0,1,1.0,1), gl_FragColor, gl_FragColor.a);\n"
"}\n";

extern const char* pSource_FInstancedYUVEAlpha;
const char* pSource_FInstancedYUVEAlpha = 
"#extension GL_ARB_draw_instanced : enable\n"
"uniform sampler2D tex[3];\n"
"varying vec4 factor;\n"
"varying vec2 tc;\n"
"void main() {\n"
"float c0 = float((texture2D(tex[int(0.0)], tc).a - 16./255.) * 1.164);\n"
"float U0 = float(texture2D(tex[int(1.0)], tc).a - 128./255.);\n"
"float V0 = float(texture2D(tex[int(2.0)], tc).a - 128./255.);\n"
"vec4 c = vec4(c0,c0,c0,c0);\n"
"vec4 U = vec4(U0,U0,U0,U0);\n"
"vec4 V = vec4(V0,V0,V0,V0);\n"
"c += V * vec4(1.596, -0.813, 0.0, 0.0);\n"
"c += U * vec4(0.0, -0.392, 2.017, 0.0);\n"
"c.a = 1.0;\n"
"gl_FragColor = c;\n"
"gl_FragColor.a *= factor.a;\n"
"}\n";

extern const char* pSource_FInstancedYUVEAlphaMul;
const char* pSource_FInstancedYUVEAlphaMul = 
"#extension GL_ARB_draw_instanced : enable\n"
"uniform sampler2D tex[3];\n"
"varying vec4 factor;\n"
"varying vec2 tc;\n"
"void main() {\n"
"float c0 = float((texture2D(tex[int(0.0)], tc).a - 16./255.) * 1.164);\n"
"float U0 = float(texture2D(tex[int(1.0)], tc).a - 128./255.);\n"
"float V0 = float(texture2D(tex[int(2.0)], tc).a - 128./255.);\n"
"vec4 c = vec4(c0,c0,c0,c0);\n"
"vec4 U = vec4(U0,U0,U0,U0);\n"
"vec4 V = vec4(V0,V0,V0,V0);\n"
"c += V * vec4(1.596, -0.813, 0.0, 0.0);\n"
"c += U * vec4(0.0, -0.392, 2.017, 0.0);\n"
"c.a = 1.0;\n"
"gl_FragColor = c;\n"
"gl_FragColor.a *= factor.a;\n"
"gl_FragColor = mix(vec4(1.0,1,1.0,1), gl_FragColor, gl_FragColor.a);\n"
"}\n";

extern const char* pSource_FInstancedYUVCxform;
const char* pSource_FInstancedYUVCxform = 
"#extension GL_ARB_draw_instanced : enable\n"
"uniform sampler2D tex[3];\n"
"varying vec4 fucxadd;\n"
"varying vec4 fucxmul;\n"
"varying vec2 tc;\n"
"void main() {\n"
"float c0 = float((texture2D(tex[int(0.0)], tc).a - 16./255.) * 1.164);\n"
"float U0 = float(texture2D(tex[int(1.0)], tc).a - 128./255.);\n"
"float V0 = float(texture2D(tex[int(2.0)], tc).a - 128./255.);\n"
"vec4 c = vec4(c0,c0,c0,c0);\n"
"vec4 U = vec4(U0,U0,U0,U0);\n"
"vec4 V = vec4(V0,V0,V0,V0);\n"
"c += V * vec4(1.596, -0.813, 0.0, 0.0);\n"
"c += U * vec4(0.0, -0.392, 2.017, 0.0);\n"
"c.a = 1.0;\n"
"gl_FragColor = c;\n"
"gl_FragColor = gl_FragColor * fucxmul + fucxadd;\n"
"}\n";

extern const char* pSource_FInstancedYUVCxformMul;
const char* pSource_FInstancedYUVCxformMul = 
"#extension GL_ARB_draw_instanced : enable\n"
"uniform sampler2D tex[3];\n"
"varying vec4 fucxadd;\n"
"varying vec4 fucxmul;\n"
"varying vec2 tc;\n"
"void main() {\n"
"float c0 = float((texture2D(tex[int(0.0)], tc).a - 16./255.) * 1.164);\n"
"float U0 = float(texture2D(tex[int(1.0)], tc).a - 128./255.);\n"
"float V0 = float(texture2D(tex[int(2.0)], tc).a - 128./255.);\n"
"vec4 c = vec4(c0,c0,c0,c0);\n"
"vec4 U = vec4(U0,U0,U0,U0);\n"
"vec4 V = vec4(V0,V0,V0,V0);\n"
"c += V * vec4(1.596, -0.813, 0.0, 0.0);\n"
"c += U * vec4(0.0, -0.392, 2.017, 0.0);\n"
"c.a = 1.0;\n"
"gl_FragColor = c;\n"
"gl_FragColor = gl_FragColor * fucxmul + fucxadd;\n"
"gl_FragColor = mix(vec4(1.0,1,1.0,1), gl_FragColor, gl_FragColor.a);\n"
"}\n";

extern const char* pSource_FInstancedYUVCxformEAlpha;
const char* pSource_FInstancedYUVCxformEAlpha = 
"#extension GL_ARB_draw_instanced : enable\n"
"uniform sampler2D tex[3];\n"
"varying vec4 factor;\n"
"varying vec4 fucxadd;\n"
"varying vec4 fucxmul;\n"
"varying vec2 tc;\n"
"void main() {\n"
"float c0 = float((texture2D(tex[int(0.0)], tc).a - 16./255.) * 1.164);\n"
"float U0 = float(texture2D(tex[int(1.0)], tc).a - 128./255.);\n"
"float V0 = float(texture2D(tex[int(2.0)], tc).a - 128./255.);\n"
"vec4 c = vec4(c0,c0,c0,c0);\n"
"vec4 U = vec4(U0,U0,U0,U0);\n"
"vec4 V = vec4(V0,V0,V0,V0);\n"
"c += V * vec4(1.596, -0.813, 0.0, 0.0);\n"
"c += U * vec4(0.0, -0.392, 2.017, 0.0);\n"
"c.a = 1.0;\n"
"gl_FragColor = c;\n"
"gl_FragColor = gl_FragColor * fucxmul + fucxadd;\n"
"gl_FragColor.a *= factor.a;\n"
"}\n";

extern const char* pSource_FInstancedYUVCxformEAlphaMul;
const char* pSource_FInstancedYUVCxformEAlphaMul = 
"#extension GL_ARB_draw_instanced : enable\n"
"uniform sampler2D tex[3];\n"
"varying vec4 factor;\n"
"varying vec4 fucxadd;\n"
"varying vec4 fucxmul;\n"
"varying vec2 tc;\n"
"void main() {\n"
"float c0 = float((texture2D(tex[int(0.0)], tc).a - 16./255.) * 1.164);\n"
"float U0 = float(texture2D(tex[int(1.0)], tc).a - 128./255.);\n"
"float V0 = float(texture2D(tex[int(2.0)], tc).a - 128./255.);\n"
"vec4 c = vec4(c0,c0,c0,c0);\n"
"vec4 U = vec4(U0,U0,U0,U0);\n"
"vec4 V = vec4(V0,V0,V0,V0);\n"
"c += V * vec4(1.596, -0.813, 0.0, 0.0);\n"
"c += U * vec4(0.0, -0.392, 2.017, 0.0);\n"
"c.a = 1.0;\n"
"gl_FragColor = c;\n"
"gl_FragColor = gl_FragColor * fucxmul + fucxadd;\n"
"gl_FragColor.a *= factor.a;\n"
"gl_FragColor = mix(vec4(1.0,1,1.0,1), gl_FragColor, gl_FragColor.a);\n"
"}\n";

extern const char* pSource_FInstancedYUVA;
const char* pSource_FInstancedYUVA = 
"#extension GL_ARB_draw_instanced : enable\n"
"uniform sampler2D tex[4];\n"
"varying vec2 tc;\n"
"void main() {\n"
"float c0 = float((texture2D(tex[int(0.0)], tc).a - 16./255.) * 1.164);\n"
"float U0 = float(texture2D(tex[int(1.0)], tc).a - 128./255.);\n"
"float V0 = float(texture2D(tex[int(2.0)], tc).a - 128./255.);\n"
"vec4 c = vec4(c0,c0,c0,c0);\n"
"vec4 U = vec4(U0,U0,U0,U0);\n"
"vec4 V = vec4(V0,V0,V0,V0);\n"
"c += V * vec4(1.596, -0.813, 0.0, 0.0);\n"
"c += U * vec4(0.0, -0.392, 2.017, 0.0);\n"
"c.a = texture2D(tex[int(3.0)], tc).a;\n"
"gl_FragColor = c;\n"
"}\n";

extern const char* pSource_FInstancedYUVAMul;
const char* pSource_FInstancedYUVAMul = 
"#extension GL_ARB_draw_instanced : enable\n"
"uniform sampler2D tex[4];\n"
"varying vec2 tc;\n"
"void main() {\n"
"float c0 = float((texture2D(tex[int(0.0)], tc).a - 16./255.) * 1.164);\n"
"float U0 = float(texture2D(tex[int(1.0)], tc).a - 128./255.);\n"
"float V0 = float(texture2D(tex[int(2.0)], tc).a - 128./255.);\n"
"vec4 c = vec4(c0,c0,c0,c0);\n"
"vec4 U = vec4(U0,U0,U0,U0);\n"
"vec4 V = vec4(V0,V0,V0,V0);\n"
"c += V * vec4(1.596, -0.813, 0.0, 0.0);\n"
"c += U * vec4(0.0, -0.392, 2.017, 0.0);\n"
"c.a = texture2D(tex[int(3.0)], tc).a;\n"
"gl_FragColor = c;\n"
"gl_FragColor = mix(vec4(1.0,1,1.0,1), gl_FragColor, gl_FragColor.a);\n"
"}\n";

extern const char* pSource_FInstancedYUVAEAlpha;
const char* pSource_FInstancedYUVAEAlpha = 
"#extension GL_ARB_draw_instanced : enable\n"
"uniform sampler2D tex[4];\n"
"varying vec4 factor;\n"
"varying vec2 tc;\n"
"void main() {\n"
"float c0 = float((texture2D(tex[int(0.0)], tc).a - 16./255.) * 1.164);\n"
"float U0 = float(texture2D(tex[int(1.0)], tc).a - 128./255.);\n"
"float V0 = float(texture2D(tex[int(2.0)], tc).a - 128./255.);\n"
"vec4 c = vec4(c0,c0,c0,c0);\n"
"vec4 U = vec4(U0,U0,U0,U0);\n"
"vec4 V = vec4(V0,V0,V0,V0);\n"
"c += V * vec4(1.596, -0.813, 0.0, 0.0);\n"
"c += U * vec4(0.0, -0.392, 2.017, 0.0);\n"
"c.a = texture2D(tex[int(3.0)], tc).a;\n"
"gl_FragColor = c;\n"
"gl_FragColor.a *= factor.a;\n"
"}\n";

extern const char* pSource_FInstancedYUVAEAlphaMul;
const char* pSource_FInstancedYUVAEAlphaMul = 
"#extension GL_ARB_draw_instanced : enable\n"
"uniform sampler2D tex[4];\n"
"varying vec4 factor;\n"
"varying vec2 tc;\n"
"void main() {\n"
"float c0 = float((texture2D(tex[int(0.0)], tc).a - 16./255.) * 1.164);\n"
"float U0 = float(texture2D(tex[int(1.0)], tc).a - 128./255.);\n"
"float V0 = float(texture2D(tex[int(2.0)], tc).a - 128./255.);\n"
"vec4 c = vec4(c0,c0,c0,c0);\n"
"vec4 U = vec4(U0,U0,U0,U0);\n"
"vec4 V = vec4(V0,V0,V0,V0);\n"
"c += V * vec4(1.596, -0.813, 0.0, 0.0);\n"
"c += U * vec4(0.0, -0.392, 2.017, 0.0);\n"
"c.a = texture2D(tex[int(3.0)], tc).a;\n"
"gl_FragColor = c;\n"
"gl_FragColor.a *= factor.a;\n"
"gl_FragColor = mix(vec4(1.0,1,1.0,1), gl_FragColor, gl_FragColor.a);\n"
"}\n";

extern const char* pSource_FInstancedYUVACxform;
const char* pSource_FInstancedYUVACxform = 
"#extension GL_ARB_draw_instanced : enable\n"
"uniform sampler2D tex[4];\n"
"varying vec4 fucxadd;\n"
"varying vec4 fucxmul;\n"
"varying vec2 tc;\n"
"void main() {\n"
"float c0 = float((texture2D(tex[int(0.0)], tc).a - 16./255.) * 1.164);\n"
"float U0 = float(texture2D(tex[int(1.0)], tc).a - 128./255.);\n"
"float V0 = float(texture2D(tex[int(2.0)], tc).a - 128./255.);\n"
"vec4 c = vec4(c0,c0,c0,c0);\n"
"vec4 U = vec4(U0,U0,U0,U0);\n"
"vec4 V = vec4(V0,V0,V0,V0);\n"
"c += V * vec4(1.596, -0.813, 0.0, 0.0);\n"
"c += U * vec4(0.0, -0.392, 2.017, 0.0);\n"
"c.a = texture2D(tex[int(3.0)], tc).a;\n"
"gl_FragColor = c;\n"
"gl_FragColor = gl_FragColor * fucxmul + fucxadd;\n"
"}\n";

extern const char* pSource_FInstancedYUVACxformMul;
const char* pSource_FInstancedYUVACxformMul = 
"#extension GL_ARB_draw_instanced : enable\n"
"uniform sampler2D tex[4];\n"
"varying vec4 fucxadd;\n"
"varying vec4 fucxmul;\n"
"varying vec2 tc;\n"
"void main() {\n"
"float c0 = float((texture2D(tex[int(0.0)], tc).a - 16./255.) * 1.164);\n"
"float U0 = float(texture2D(tex[int(1.0)], tc).a - 128./255.);\n"
"float V0 = float(texture2D(tex[int(2.0)], tc).a - 128./255.);\n"
"vec4 c = vec4(c0,c0,c0,c0);\n"
"vec4 U = vec4(U0,U0,U0,U0);\n"
"vec4 V = vec4(V0,V0,V0,V0);\n"
"c += V * vec4(1.596, -0.813, 0.0, 0.0);\n"
"c += U * vec4(0.0, -0.392, 2.017, 0.0);\n"
"c.a = texture2D(tex[int(3.0)], tc).a;\n"
"gl_FragColor = c;\n"
"gl_FragColor = gl_FragColor * fucxmul + fucxadd;\n"
"gl_FragColor = mix(vec4(1.0,1,1.0,1), gl_FragColor, gl_FragColor.a);\n"
"}\n";

extern const char* pSource_FInstancedYUVACxformEAlpha;
const char* pSource_FInstancedYUVACxformEAlpha = 
"#extension GL_ARB_draw_instanced : enable\n"
"uniform sampler2D tex[4];\n"
"varying vec4 factor;\n"
"varying vec4 fucxadd;\n"
"varying vec4 fucxmul;\n"
"varying vec2 tc;\n"
"void main() {\n"
"float c0 = float((texture2D(tex[int(0.0)], tc).a - 16./255.) * 1.164);\n"
"float U0 = float(texture2D(tex[int(1.0)], tc).a - 128./255.);\n"
"float V0 = float(texture2D(tex[int(2.0)], tc).a - 128./255.);\n"
"vec4 c = vec4(c0,c0,c0,c0);\n"
"vec4 U = vec4(U0,U0,U0,U0);\n"
"vec4 V = vec4(V0,V0,V0,V0);\n"
"c += V * vec4(1.596, -0.813, 0.0, 0.0);\n"
"c += U * vec4(0.0, -0.392, 2.017, 0.0);\n"
"c.a = texture2D(tex[int(3.0)], tc).a;\n"
"gl_FragColor = c;\n"
"gl_FragColor = gl_FragColor * fucxmul + fucxadd;\n"
"gl_FragColor.a *= factor.a;\n"
"}\n";

extern const char* pSource_FInstancedYUVACxformEAlphaMul;
const char* pSource_FInstancedYUVACxformEAlphaMul = 
"#extension GL_ARB_draw_instanced : enable\n"
"uniform sampler2D tex[4];\n"
"varying vec4 factor;\n"
"varying vec4 fucxadd;\n"
"varying vec4 fucxmul;\n"
"varying vec2 tc;\n"
"void main() {\n"
"float c0 = float((texture2D(tex[int(0.0)], tc).a - 16./255.) * 1.164);\n"
"float U0 = float(texture2D(tex[int(1.0)], tc).a - 128./255.);\n"
"float V0 = float(texture2D(tex[int(2.0)], tc).a - 128./255.);\n"
"vec4 c = vec4(c0,c0,c0,c0);\n"
"vec4 U = vec4(U0,U0,U0,U0);\n"
"vec4 V = vec4(V0,V0,V0,V0);\n"
"c += V * vec4(1.596, -0.813, 0.0, 0.0);\n"
"c += U * vec4(0.0, -0.392, 2.017, 0.0);\n"
"c.a = texture2D(tex[int(3.0)], tc).a;\n"
"gl_FragColor = c;\n"
"gl_FragColor = gl_FragColor * fucxmul + fucxadd;\n"
"gl_FragColor.a *= factor.a;\n"
"gl_FragColor = mix(vec4(1.0,1,1.0,1), gl_FragColor, gl_FragColor.a);\n"
"}\n";

extern const char* pSource_FTexTGCMatrixAc;
const char* pSource_FTexTGCMatrixAc = 
"uniform vec4 cxadd;\n"
"uniform mat4 cxmul;\n"
"uniform sampler2D tex;\n"
"varying vec2 tc;\n"
"void main() {\n"
"gl_FragColor = texture2D(tex,tc);\n"
"gl_FragColor = (gl_FragColor) * (cxmul) + cxadd * (gl_FragColor.a + cxadd.a);\n"
"}\n";

extern const char* pSource_FTexTGCMatrixAcMul;
const char* pSource_FTexTGCMatrixAcMul = 
"uniform vec4 cxadd;\n"
"uniform mat4 cxmul;\n"
"uniform sampler2D tex;\n"
"varying vec2 tc;\n"
"void main() {\n"
"gl_FragColor = texture2D(tex,tc);\n"
"gl_FragColor = (gl_FragColor) * (cxmul) + cxadd * (gl_FragColor.a + cxadd.a);\n"
"gl_FragColor = mix(vec4(1.0,1,1.0,1), gl_FragColor, gl_FragColor.a);\n"
"}\n";

extern const char* pSource_FTexTGCMatrixAcEAlpha;
const char* pSource_FTexTGCMatrixAcEAlpha = 
"uniform vec4 cxadd;\n"
"uniform mat4 cxmul;\n"
"uniform sampler2D tex;\n"
"varying vec4 factor;\n"
"varying vec2 tc;\n"
"void main() {\n"
"gl_FragColor = texture2D(tex,tc);\n"
"gl_FragColor = (gl_FragColor) * (cxmul) + cxadd * (gl_FragColor.a + cxadd.a);\n"
"gl_FragColor.a *= factor.a;\n"
"}\n";

extern const char* pSource_FTexTGCMatrixAcEAlphaMul;
const char* pSource_FTexTGCMatrixAcEAlphaMul = 
"uniform vec4 cxadd;\n"
"uniform mat4 cxmul;\n"
"uniform sampler2D tex;\n"
"varying vec4 factor;\n"
"varying vec2 tc;\n"
"void main() {\n"
"gl_FragColor = texture2D(tex,tc);\n"
"gl_FragColor = (gl_FragColor) * (cxmul) + cxadd * (gl_FragColor.a + cxadd.a);\n"
"gl_FragColor.a *= factor.a;\n"
"gl_FragColor = mix(vec4(1.0,1,1.0,1), gl_FragColor, gl_FragColor.a);\n"
"}\n";

extern const char* pSource_FBox1Blur;
const char* pSource_FBox1Blur = 
"uniform vec4 fsize;\n"
"uniform sampler2D tex;\n"
"uniform vec4 texscale;\n"
"varying vec4 fucxadd;\n"
"varying vec4 fucxmul;\n"
"varying vec2 tc;\n"
"void main() {\n"
"gl_FragColor       = vec4(0.0, 0.0, 0.0, 0.0);\n"
"vec4 color = vec4(0.0, 0.0, 0.0, 0.0);\n"
"float  i = 0.0;\n"
"for (i = -fsize.x; i <= fsize.x; i++)\n"
"{{\n"
"color += texture2DLod(tex, tc + i * texscale.xy, 0.0);\n"
"}} // EndBox1/2.\n"
"gl_FragColor = color * fsize.w;\n"
"gl_FragColor = (gl_FragColor * vec4(fucxmul.rgb,1.0)) * fucxmul.a;\n"
"gl_FragColor += fucxadd * gl_FragColor.a;\n"
"}\n";

extern const char* pSource_FBox1BlurMul;
const char* pSource_FBox1BlurMul = 
"uniform vec4 fsize;\n"
"uniform sampler2D tex;\n"
"uniform vec4 texscale;\n"
"varying vec4 fucxadd;\n"
"varying vec4 fucxmul;\n"
"varying vec2 tc;\n"
"void main() {\n"
"gl_FragColor       = vec4(0.0, 0.0, 0.0, 0.0);\n"
"vec4 color = vec4(0.0, 0.0, 0.0, 0.0);\n"
"float  i = 0.0;\n"
"for (i = -fsize.x; i <= fsize.x; i++)\n"
"{{\n"
"color += texture2DLod(tex, tc + i * texscale.xy, 0.0);\n"
"}} // EndBox1/2.\n"
"gl_FragColor = color * fsize.w;\n"
"gl_FragColor = (gl_FragColor * vec4(fucxmul.rgb,1.0)) * fucxmul.a;\n"
"gl_FragColor += fucxadd * gl_FragColor.a;\n"
"gl_FragColor = mix(vec4(1.0,1,1.0,1), gl_FragColor, gl_FragColor.a);\n"
"}\n";

extern const char* pSource_FBox2Blur;
const char* pSource_FBox2Blur = 
"uniform vec4 fsize;\n"
"uniform sampler2D tex;\n"
"uniform vec4 texscale;\n"
"varying vec4 fucxadd;\n"
"varying vec4 fucxmul;\n"
"varying vec2 tc;\n"
"void main() {\n"
"gl_FragColor       = vec4(0.0, 0.0, 0.0, 0.0);\n"
"vec4 color = vec4(0.0, 0.0, 0.0, 0.0);\n"
"vec2 i = vec2(0.0, 0.0);\n"
"for (i.x = -fsize.x; i.x <= fsize.x; i.x++)\n"
"{\n"
"for (i.y = -fsize.y; i.y <= fsize.y; i.y++)\n"
"{\n"
"color += texture2DLod(tex, tc + i * texscale.xy, 0.0);\n"
"}} // EndBox1/2.\n"
"gl_FragColor = color * fsize.w;\n"
"gl_FragColor = (gl_FragColor * vec4(fucxmul.rgb,1.0)) * fucxmul.a;\n"
"gl_FragColor += fucxadd * gl_FragColor.a;\n"
"}\n";

extern const char* pSource_FBox2BlurMul;
const char* pSource_FBox2BlurMul = 
"uniform vec4 fsize;\n"
"uniform sampler2D tex;\n"
"uniform vec4 texscale;\n"
"varying vec4 fucxadd;\n"
"varying vec4 fucxmul;\n"
"varying vec2 tc;\n"
"void main() {\n"
"gl_FragColor       = vec4(0.0, 0.0, 0.0, 0.0);\n"
"vec4 color = vec4(0.0, 0.0, 0.0, 0.0);\n"
"vec2 i = vec2(0.0, 0.0);\n"
"for (i.x = -fsize.x; i.x <= fsize.x; i.x++)\n"
"{\n"
"for (i.y = -fsize.y; i.y <= fsize.y; i.y++)\n"
"{\n"
"color += texture2DLod(tex, tc + i * texscale.xy, 0.0);\n"
"}} // EndBox1/2.\n"
"gl_FragColor = color * fsize.w;\n"
"gl_FragColor = (gl_FragColor * vec4(fucxmul.rgb,1.0)) * fucxmul.a;\n"
"gl_FragColor += fucxadd * gl_FragColor.a;\n"
"gl_FragColor = mix(vec4(1.0,1,1.0,1), gl_FragColor, gl_FragColor.a);\n"
"}\n";

extern const char* pSource_FBox2Shadow;
const char* pSource_FBox2Shadow = 
"uniform vec4 fsize;\n"
"uniform vec4 offset;\n"
"uniform vec4 scolor;\n"
"uniform sampler2D srctex;\n"
"uniform vec4 srctexscale;\n"
"uniform sampler2D tex;\n"
"uniform vec4 texscale;\n"
"varying vec4 fucxadd;\n"
"varying vec4 fucxmul;\n"
"varying vec2 tc;\n"
"void main() {\n"
"gl_FragColor       = vec4(0.0, 0.0, 0.0, 0.0);\n"
"vec4 color = vec4(0.0, 0.0, 0.0, 0.0);\n"
"vec2 i = vec2(0.0, 0.0);\n"
"for (i.x = -fsize.x; i.x <= fsize.x; i.x++)\n"
"{\n"
"for (i.y = -fsize.y; i.y <= fsize.y; i.y++)\n"
"{\n"
"color += texture2DLod(tex, tc + (offset.xy + i) * texscale.xy, 0.0);\n"
"}\n"
"} // EndBox2.\n"
"gl_FragColor = color * fsize.w;\n"
"gl_FragColor.a = gl_FragColor.a * fsize.z;\n"
"vec4 base = texture2D(srctex, tc * srctexscale.xy);\n"
"gl_FragColor = scolor * gl_FragColor.a * (1.0-base.a) + base;\n"
"gl_FragColor = (gl_FragColor * vec4(fucxmul.rgb,1.0)) * fucxmul.a;\n"
"gl_FragColor += fucxadd * gl_FragColor.a;\n"
"}\n";

extern const char* pSource_FBox2ShadowMul;
const char* pSource_FBox2ShadowMul = 
"uniform vec4 fsize;\n"
"uniform vec4 offset;\n"
"uniform vec4 scolor;\n"
"uniform sampler2D srctex;\n"
"uniform vec4 srctexscale;\n"
"uniform sampler2D tex;\n"
"uniform vec4 texscale;\n"
"varying vec4 fucxadd;\n"
"varying vec4 fucxmul;\n"
"varying vec2 tc;\n"
"void main() {\n"
"gl_FragColor       = vec4(0.0, 0.0, 0.0, 0.0);\n"
"vec4 color = vec4(0.0, 0.0, 0.0, 0.0);\n"
"vec2 i = vec2(0.0, 0.0);\n"
"for (i.x = -fsize.x; i.x <= fsize.x; i.x++)\n"
"{\n"
"for (i.y = -fsize.y; i.y <= fsize.y; i.y++)\n"
"{\n"
"color += texture2DLod(tex, tc + (offset.xy + i) * texscale.xy, 0.0);\n"
"}\n"
"} // EndBox2.\n"
"gl_FragColor = color * fsize.w;\n"
"gl_FragColor.a = gl_FragColor.a * fsize.z;\n"
"vec4 base = texture2D(srctex, tc * srctexscale.xy);\n"
"gl_FragColor = scolor * gl_FragColor.a * (1.0-base.a) + base;\n"
"gl_FragColor = (gl_FragColor * vec4(fucxmul.rgb,1.0)) * fucxmul.a;\n"
"gl_FragColor += fucxadd * gl_FragColor.a;\n"
"gl_FragColor = mix(vec4(1.0,1,1.0,1), gl_FragColor, gl_FragColor.a);\n"
"}\n";

extern const char* pSource_FBox2ShadowKnockout;
const char* pSource_FBox2ShadowKnockout = 
"uniform vec4 fsize;\n"
"uniform vec4 offset;\n"
"uniform vec4 scolor;\n"
"uniform sampler2D srctex;\n"
"uniform vec4 srctexscale;\n"
"uniform sampler2D tex;\n"
"uniform vec4 texscale;\n"
"varying vec4 fucxadd;\n"
"varying vec4 fucxmul;\n"
"varying vec2 tc;\n"
"void main() {\n"
"gl_FragColor       = vec4(0.0, 0.0, 0.0, 0.0);\n"
"vec4 color = vec4(0.0, 0.0, 0.0, 0.0);\n"
"vec2 i = vec2(0.0, 0.0);\n"
"for (i.x = -fsize.x; i.x <= fsize.x; i.x++)\n"
"{\n"
"for (i.y = -fsize.y; i.y <= fsize.y; i.y++)\n"
"{\n"
"color += texture2DLod(tex, tc + (offset.xy + i) * texscale.xy, 0.0);\n"
"}\n"
"} // EndBox2.\n"
"gl_FragColor = color * fsize.w;\n"
"gl_FragColor.a = gl_FragColor.a * fsize.z;\n"
"vec4 base = texture2D(srctex, tc * srctexscale.xy);\n"
"gl_FragColor = scolor * gl_FragColor.a * (1.0-base.a) + base;\n"
"gl_FragColor -= base;\n"
"gl_FragColor = (gl_FragColor * vec4(fucxmul.rgb,1.0)) * fucxmul.a;\n"
"gl_FragColor += fucxadd * gl_FragColor.a;\n"
"}\n";

extern const char* pSource_FBox2ShadowKnockoutMul;
const char* pSource_FBox2ShadowKnockoutMul = 
"uniform vec4 fsize;\n"
"uniform vec4 offset;\n"
"uniform vec4 scolor;\n"
"uniform sampler2D srctex;\n"
"uniform vec4 srctexscale;\n"
"uniform sampler2D tex;\n"
"uniform vec4 texscale;\n"
"varying vec4 fucxadd;\n"
"varying vec4 fucxmul;\n"
"varying vec2 tc;\n"
"void main() {\n"
"gl_FragColor       = vec4(0.0, 0.0, 0.0, 0.0);\n"
"vec4 color = vec4(0.0, 0.0, 0.0, 0.0);\n"
"vec2 i = vec2(0.0, 0.0);\n"
"for (i.x = -fsize.x; i.x <= fsize.x; i.x++)\n"
"{\n"
"for (i.y = -fsize.y; i.y <= fsize.y; i.y++)\n"
"{\n"
"color += texture2DLod(tex, tc + (offset.xy + i) * texscale.xy, 0.0);\n"
"}\n"
"} // EndBox2.\n"
"gl_FragColor = color * fsize.w;\n"
"gl_FragColor.a = gl_FragColor.a * fsize.z;\n"
"vec4 base = texture2D(srctex, tc * srctexscale.xy);\n"
"gl_FragColor = scolor * gl_FragColor.a * (1.0-base.a) + base;\n"
"gl_FragColor -= base;\n"
"gl_FragColor = (gl_FragColor * vec4(fucxmul.rgb,1.0)) * fucxmul.a;\n"
"gl_FragColor += fucxadd * gl_FragColor.a;\n"
"gl_FragColor = mix(vec4(1.0,1,1.0,1), gl_FragColor, gl_FragColor.a);\n"
"}\n";

extern const char* pSource_FBox2InnerShadow;
const char* pSource_FBox2InnerShadow = 
"uniform vec4 fsize;\n"
"uniform vec4 offset;\n"
"uniform vec4 scolor;\n"
"uniform sampler2D srctex;\n"
"uniform vec4 srctexscale;\n"
"uniform sampler2D tex;\n"
"uniform vec4 texscale;\n"
"varying vec4 fucxadd;\n"
"varying vec4 fucxmul;\n"
"varying vec2 tc;\n"
"void main() {\n"
"gl_FragColor       = vec4(0.0, 0.0, 0.0, 0.0);\n"
"vec4 color = vec4(0.0, 0.0, 0.0, 0.0);\n"
"vec2 i = vec2(0.0, 0.0);\n"
"for (i.x = -fsize.x; i.x <= fsize.x; i.x++)\n"
"{\n"
"for (i.y = -fsize.y; i.y <= fsize.y; i.y++)\n"
"{\n"
"color += texture2DLod(tex, tc + (offset.xy + i) * texscale.xy, 0.0);\n"
"}\n"
"} // EndBox2.\n"
"gl_FragColor = color * fsize.w;\n"
"gl_FragColor.a = gl_FragColor.a * fsize.z;\n"
"vec4 base = texture2DLod(srctex, tc * srctexscale.xy, 0.0);\n"
"gl_FragColor = mix(scolor, base, gl_FragColor.a) * base.a;\n"
"gl_FragColor = (gl_FragColor * vec4(fucxmul.rgb,1.0)) * fucxmul.a;\n"
"gl_FragColor += fucxadd * gl_FragColor.a;\n"
"}\n";

extern const char* pSource_FBox2InnerShadowMul;
const char* pSource_FBox2InnerShadowMul = 
"uniform vec4 fsize;\n"
"uniform vec4 offset;\n"
"uniform vec4 scolor;\n"
"uniform sampler2D srctex;\n"
"uniform vec4 srctexscale;\n"
"uniform sampler2D tex;\n"
"uniform vec4 texscale;\n"
"varying vec4 fucxadd;\n"
"varying vec4 fucxmul;\n"
"varying vec2 tc;\n"
"void main() {\n"
"gl_FragColor       = vec4(0.0, 0.0, 0.0, 0.0);\n"
"vec4 color = vec4(0.0, 0.0, 0.0, 0.0);\n"
"vec2 i = vec2(0.0, 0.0);\n"
"for (i.x = -fsize.x; i.x <= fsize.x; i.x++)\n"
"{\n"
"for (i.y = -fsize.y; i.y <= fsize.y; i.y++)\n"
"{\n"
"color += texture2DLod(tex, tc + (offset.xy + i) * texscale.xy, 0.0);\n"
"}\n"
"} // EndBox2.\n"
"gl_FragColor = color * fsize.w;\n"
"gl_FragColor.a = gl_FragColor.a * fsize.z;\n"
"vec4 base = texture2DLod(srctex, tc * srctexscale.xy, 0.0);\n"
"gl_FragColor = mix(scolor, base, gl_FragColor.a) * base.a;\n"
"gl_FragColor = (gl_FragColor * vec4(fucxmul.rgb,1.0)) * fucxmul.a;\n"
"gl_FragColor += fucxadd * gl_FragColor.a;\n"
"gl_FragColor = mix(vec4(1.0,1,1.0,1), gl_FragColor, gl_FragColor.a);\n"
"}\n";

extern const char* pSource_FBox2InnerShadowKnockout;
const char* pSource_FBox2InnerShadowKnockout = 
"uniform vec4 fsize;\n"
"uniform vec4 offset;\n"
"uniform vec4 scolor;\n"
"uniform sampler2D srctex;\n"
"uniform vec4 srctexscale;\n"
"uniform sampler2D tex;\n"
"uniform vec4 texscale;\n"
"varying vec4 fucxadd;\n"
"varying vec4 fucxmul;\n"
"varying vec2 tc;\n"
"void main() {\n"
"gl_FragColor       = vec4(0.0, 0.0, 0.0, 0.0);\n"
"vec4 color = vec4(0.0, 0.0, 0.0, 0.0);\n"
"vec2 i = vec2(0.0, 0.0);\n"
"for (i.x = -fsize.x; i.x <= fsize.x; i.x++)\n"
"{\n"
"for (i.y = -fsize.y; i.y <= fsize.y; i.y++)\n"
"{\n"
"color += texture2DLod(tex, tc + (offset.xy + i) * texscale.xy, 0.0);\n"
"}\n"
"} // EndBox2.\n"
"gl_FragColor = color * fsize.w;\n"
"gl_FragColor.a = gl_FragColor.a * fsize.z;\n"
"vec4 base = texture2DLod(srctex, tc * srctexscale.xy, 0.0);\n"
"gl_FragColor = mix(scolor, base, gl_FragColor.a) * base.a;\n"
"gl_FragColor -= base;\n"
"gl_FragColor = (gl_FragColor * vec4(fucxmul.rgb,1.0)) * fucxmul.a;\n"
"gl_FragColor += fucxadd * gl_FragColor.a;\n"
"}\n";

extern const char* pSource_FBox2InnerShadowKnockoutMul;
const char* pSource_FBox2InnerShadowKnockoutMul = 
"uniform vec4 fsize;\n"
"uniform vec4 offset;\n"
"uniform vec4 scolor;\n"
"uniform sampler2D srctex;\n"
"uniform vec4 srctexscale;\n"
"uniform sampler2D tex;\n"
"uniform vec4 texscale;\n"
"varying vec4 fucxadd;\n"
"varying vec4 fucxmul;\n"
"varying vec2 tc;\n"
"void main() {\n"
"gl_FragColor       = vec4(0.0, 0.0, 0.0, 0.0);\n"
"vec4 color = vec4(0.0, 0.0, 0.0, 0.0);\n"
"vec2 i = vec2(0.0, 0.0);\n"
"for (i.x = -fsize.x; i.x <= fsize.x; i.x++)\n"
"{\n"
"for (i.y = -fsize.y; i.y <= fsize.y; i.y++)\n"
"{\n"
"color += texture2DLod(tex, tc + (offset.xy + i) * texscale.xy, 0.0);\n"
"}\n"
"} // EndBox2.\n"
"gl_FragColor = color * fsize.w;\n"
"gl_FragColor.a = gl_FragColor.a * fsize.z;\n"
"vec4 base = texture2DLod(srctex, tc * srctexscale.xy, 0.0);\n"
"gl_FragColor = mix(scolor, base, gl_FragColor.a) * base.a;\n"
"gl_FragColor -= base;\n"
"gl_FragColor = (gl_FragColor * vec4(fucxmul.rgb,1.0)) * fucxmul.a;\n"
"gl_FragColor += fucxadd * gl_FragColor.a;\n"
"gl_FragColor = mix(vec4(1.0,1,1.0,1), gl_FragColor, gl_FragColor.a);\n"
"}\n";

extern const char* pSource_FBox2Shadowonly;
const char* pSource_FBox2Shadowonly = 
"uniform vec4 fsize;\n"
"uniform vec4 offset;\n"
"uniform vec4 scolor;\n"
"uniform sampler2D tex;\n"
"uniform vec4 texscale;\n"
"varying vec4 fucxadd;\n"
"varying vec4 fucxmul;\n"
"varying vec2 tc;\n"
"void main() {\n"
"gl_FragColor       = vec4(0.0, 0.0, 0.0, 0.0);\n"
"vec4 color = vec4(0.0, 0.0, 0.0, 0.0);\n"
"vec2 i = vec2(0.0, 0.0);\n"
"for (i.x = -fsize.x; i.x <= fsize.x; i.x++)\n"
"{\n"
"for (i.y = -fsize.y; i.y <= fsize.y; i.y++)\n"
"{\n"
"color += texture2DLod(tex, tc + (offset.xy + i) * texscale.xy, 0.0);\n"
"}\n"
"} // EndBox2.\n"
"gl_FragColor = color * fsize.w;\n"
"gl_FragColor.a = gl_FragColor.a * fsize.z;\n"
"gl_FragColor = scolor * gl_FragColor.a;\n"
"gl_FragColor = (gl_FragColor * vec4(fucxmul.rgb,1.0)) * fucxmul.a;\n"
"gl_FragColor += fucxadd * gl_FragColor.a;\n"
"}\n";

extern const char* pSource_FBox2ShadowonlyMul;
const char* pSource_FBox2ShadowonlyMul = 
"uniform vec4 fsize;\n"
"uniform vec4 offset;\n"
"uniform vec4 scolor;\n"
"uniform sampler2D tex;\n"
"uniform vec4 texscale;\n"
"varying vec4 fucxadd;\n"
"varying vec4 fucxmul;\n"
"varying vec2 tc;\n"
"void main() {\n"
"gl_FragColor       = vec4(0.0, 0.0, 0.0, 0.0);\n"
"vec4 color = vec4(0.0, 0.0, 0.0, 0.0);\n"
"vec2 i = vec2(0.0, 0.0);\n"
"for (i.x = -fsize.x; i.x <= fsize.x; i.x++)\n"
"{\n"
"for (i.y = -fsize.y; i.y <= fsize.y; i.y++)\n"
"{\n"
"color += texture2DLod(tex, tc + (offset.xy + i) * texscale.xy, 0.0);\n"
"}\n"
"} // EndBox2.\n"
"gl_FragColor = color * fsize.w;\n"
"gl_FragColor.a = gl_FragColor.a * fsize.z;\n"
"gl_FragColor = scolor * gl_FragColor.a;\n"
"gl_FragColor = (gl_FragColor * vec4(fucxmul.rgb,1.0)) * fucxmul.a;\n"
"gl_FragColor += fucxadd * gl_FragColor.a;\n"
"gl_FragColor = mix(vec4(1.0,1,1.0,1), gl_FragColor, gl_FragColor.a);\n"
"}\n";

extern const char* pSource_FBox2ShadowHighlight;
const char* pSource_FBox2ShadowHighlight = 
"uniform vec4 fsize;\n"
"uniform vec4 offset;\n"
"uniform vec4 scolor;\n"
"uniform vec4 scolor2;\n"
"uniform sampler2D srctex;\n"
"uniform vec4 srctexscale;\n"
"uniform sampler2D tex;\n"
"uniform vec4 texscale;\n"
"varying vec4 fucxadd;\n"
"varying vec4 fucxmul;\n"
"varying vec2 tc;\n"
"void main() {\n"
"gl_FragColor       = vec4(0.0, 0.0, 0.0, 0.0);\n"
"vec4 color = vec4(0.0, 0.0, 0.0, 0.0);\n"
"vec2 i = vec2(0.0, 0.0);\n"
"for (i.x = -fsize.x; i.x <= fsize.x; i.x++)\n"
"{\n"
"for (i.y = -fsize.y; i.y <= fsize.y; i.y++)\n"
"{\n"
"color.a += texture2DLod(tex, tc + (offset.xy + i) * texscale.xy, 0.0).a;\n"
"color.r += texture2DLod(tex, tc - (offset.xy + i) * texscale.xy, 0.0).a;\n"
"}\n"
"} // EndBox2.\n"
"gl_FragColor = color * fsize.w;\n"
"gl_FragColor.a = gl_FragColor.a * fsize.z;\n"
"vec4 base = texture2D(srctex, tc * srctexscale.xy);\n"
"gl_FragColor = (scolor * gl_FragColor.a + scolor2 * gl_FragColor.r) * (1.0-base.a) + base;\n"
"gl_FragColor = (gl_FragColor * vec4(fucxmul.rgb,1.0)) * fucxmul.a;\n"
"gl_FragColor += fucxadd * gl_FragColor.a;\n"
"}\n";

extern const char* pSource_FBox2ShadowHighlightMul;
const char* pSource_FBox2ShadowHighlightMul = 
"uniform vec4 fsize;\n"
"uniform vec4 offset;\n"
"uniform vec4 scolor;\n"
"uniform vec4 scolor2;\n"
"uniform sampler2D srctex;\n"
"uniform vec4 srctexscale;\n"
"uniform sampler2D tex;\n"
"uniform vec4 texscale;\n"
"varying vec4 fucxadd;\n"
"varying vec4 fucxmul;\n"
"varying vec2 tc;\n"
"void main() {\n"
"gl_FragColor       = vec4(0.0, 0.0, 0.0, 0.0);\n"
"vec4 color = vec4(0.0, 0.0, 0.0, 0.0);\n"
"vec2 i = vec2(0.0, 0.0);\n"
"for (i.x = -fsize.x; i.x <= fsize.x; i.x++)\n"
"{\n"
"for (i.y = -fsize.y; i.y <= fsize.y; i.y++)\n"
"{\n"
"color.a += texture2DLod(tex, tc + (offset.xy + i) * texscale.xy, 0.0).a;\n"
"color.r += texture2DLod(tex, tc - (offset.xy + i) * texscale.xy, 0.0).a;\n"
"}\n"
"} // EndBox2.\n"
"gl_FragColor = color * fsize.w;\n"
"gl_FragColor.a = gl_FragColor.a * fsize.z;\n"
"vec4 base = texture2D(srctex, tc * srctexscale.xy);\n"
"gl_FragColor = (scolor * gl_FragColor.a + scolor2 * gl_FragColor.r) * (1.0-base.a) + base;\n"
"gl_FragColor = (gl_FragColor * vec4(fucxmul.rgb,1.0)) * fucxmul.a;\n"
"gl_FragColor += fucxadd * gl_FragColor.a;\n"
"gl_FragColor = mix(vec4(1.0,1,1.0,1), gl_FragColor, gl_FragColor.a);\n"
"}\n";

extern const char* pSource_FBox2ShadowHighlightKnockout;
const char* pSource_FBox2ShadowHighlightKnockout = 
"uniform vec4 fsize;\n"
"uniform vec4 offset;\n"
"uniform vec4 scolor;\n"
"uniform vec4 scolor2;\n"
"uniform sampler2D srctex;\n"
"uniform vec4 srctexscale;\n"
"uniform sampler2D tex;\n"
"uniform vec4 texscale;\n"
"varying vec4 fucxadd;\n"
"varying vec4 fucxmul;\n"
"varying vec2 tc;\n"
"void main() {\n"
"gl_FragColor       = vec4(0.0, 0.0, 0.0, 0.0);\n"
"vec4 color = vec4(0.0, 0.0, 0.0, 0.0);\n"
"vec2 i = vec2(0.0, 0.0);\n"
"for (i.x = -fsize.x; i.x <= fsize.x; i.x++)\n"
"{\n"
"for (i.y = -fsize.y; i.y <= fsize.y; i.y++)\n"
"{\n"
"color.a += texture2DLod(tex, tc + (offset.xy + i) * texscale.xy, 0.0).a;\n"
"color.r += texture2DLod(tex, tc - (offset.xy + i) * texscale.xy, 0.0).a;\n"
"}\n"
"} // EndBox2.\n"
"gl_FragColor = color * fsize.w;\n"
"gl_FragColor.a = gl_FragColor.a * fsize.z;\n"
"vec4 base = texture2D(srctex, tc * srctexscale.xy);\n"
"gl_FragColor = (scolor * gl_FragColor.a + scolor2 * gl_FragColor.r) * (1.0-base.a) + base;\n"
"gl_FragColor -= base;\n"
"gl_FragColor = (gl_FragColor * vec4(fucxmul.rgb,1.0)) * fucxmul.a;\n"
"gl_FragColor += fucxadd * gl_FragColor.a;\n"
"}\n";

extern const char* pSource_FBox2ShadowHighlightKnockoutMul;
const char* pSource_FBox2ShadowHighlightKnockoutMul = 
"uniform vec4 fsize;\n"
"uniform vec4 offset;\n"
"uniform vec4 scolor;\n"
"uniform vec4 scolor2;\n"
"uniform sampler2D srctex;\n"
"uniform vec4 srctexscale;\n"
"uniform sampler2D tex;\n"
"uniform vec4 texscale;\n"
"varying vec4 fucxadd;\n"
"varying vec4 fucxmul;\n"
"varying vec2 tc;\n"
"void main() {\n"
"gl_FragColor       = vec4(0.0, 0.0, 0.0, 0.0);\n"
"vec4 color = vec4(0.0, 0.0, 0.0, 0.0);\n"
"vec2 i = vec2(0.0, 0.0);\n"
"for (i.x = -fsize.x; i.x <= fsize.x; i.x++)\n"
"{\n"
"for (i.y = -fsize.y; i.y <= fsize.y; i.y++)\n"
"{\n"
"color.a += texture2DLod(tex, tc + (offset.xy + i) * texscale.xy, 0.0).a;\n"
"color.r += texture2DLod(tex, tc - (offset.xy + i) * texscale.xy, 0.0).a;\n"
"}\n"
"} // EndBox2.\n"
"gl_FragColor = color * fsize.w;\n"
"gl_FragColor.a = gl_FragColor.a * fsize.z;\n"
"vec4 base = texture2D(srctex, tc * srctexscale.xy);\n"
"gl_FragColor = (scolor * gl_FragColor.a + scolor2 * gl_FragColor.r) * (1.0-base.a) + base;\n"
"gl_FragColor -= base;\n"
"gl_FragColor = (gl_FragColor * vec4(fucxmul.rgb,1.0)) * fucxmul.a;\n"
"gl_FragColor += fucxadd * gl_FragColor.a;\n"
"gl_FragColor = mix(vec4(1.0,1,1.0,1), gl_FragColor, gl_FragColor.a);\n"
"}\n";

extern const char* pSource_FBox2InnerShadowHighlight;
const char* pSource_FBox2InnerShadowHighlight = 
"uniform vec4 fsize;\n"
"uniform vec4 offset;\n"
"uniform vec4 scolor;\n"
"uniform vec4 scolor2;\n"
"uniform sampler2D srctex;\n"
"uniform vec4 srctexscale;\n"
"uniform sampler2D tex;\n"
"uniform vec4 texscale;\n"
"varying vec4 fucxadd;\n"
"varying vec4 fucxmul;\n"
"varying vec2 tc;\n"
"void main() {\n"
"gl_FragColor       = vec4(0.0, 0.0, 0.0, 0.0);\n"
"vec4 color = vec4(0.0, 0.0, 0.0, 0.0);\n"
"vec2 i = vec2(0.0, 0.0);\n"
"for (i.x = -fsize.x; i.x <= fsize.x; i.x++)\n"
"{\n"
"for (i.y = -fsize.y; i.y <= fsize.y; i.y++)\n"
"{\n"
"color.a += texture2DLod(tex, tc + (offset.xy + i) * texscale.xy, 0.0).a;\n"
"color.r += texture2DLod(tex, tc - (offset.xy + i) * texscale.xy, 0.0).a;\n"
"}\n"
"} // EndBox2.\n"
"gl_FragColor = color * fsize.w;\n"
"gl_FragColor.a = gl_FragColor.a * fsize.z;\n"
"vec4 base = texture2DLod(srctex, tc * srctexscale.xy, 0.0);\n"
"gl_FragColor.ar = clamp((1.0 - gl_FragColor.ar) - (1.0 - gl_FragColor.ra) * 0.5, 0.0, 1.0);\n"
"gl_FragColor = (scolor * gl_FragColor.a + scolor2 * gl_FragColor.r\n"
"+ base * (1.0 - gl_FragColor.a - gl_FragColor.r)) * base.a;\n"
"gl_FragColor = (gl_FragColor * vec4(fucxmul.rgb,1.0)) * fucxmul.a;\n"
"gl_FragColor += fucxadd * gl_FragColor.a;\n"
"}\n";

extern const char* pSource_FBox2InnerShadowHighlightMul;
const char* pSource_FBox2InnerShadowHighlightMul = 
"uniform vec4 fsize;\n"
"uniform vec4 offset;\n"
"uniform vec4 scolor;\n"
"uniform vec4 scolor2;\n"
"uniform sampler2D srctex;\n"
"uniform vec4 srctexscale;\n"
"uniform sampler2D tex;\n"
"uniform vec4 texscale;\n"
"varying vec4 fucxadd;\n"
"varying vec4 fucxmul;\n"
"varying vec2 tc;\n"
"void main() {\n"
"gl_FragColor       = vec4(0.0, 0.0, 0.0, 0.0);\n"
"vec4 color = vec4(0.0, 0.0, 0.0, 0.0);\n"
"vec2 i = vec2(0.0, 0.0);\n"
"for (i.x = -fsize.x; i.x <= fsize.x; i.x++)\n"
"{\n"
"for (i.y = -fsize.y; i.y <= fsize.y; i.y++)\n"
"{\n"
"color.a += texture2DLod(tex, tc + (offset.xy + i) * texscale.xy, 0.0).a;\n"
"color.r += texture2DLod(tex, tc - (offset.xy + i) * texscale.xy, 0.0).a;\n"
"}\n"
"} // EndBox2.\n"
"gl_FragColor = color * fsize.w;\n"
"gl_FragColor.a = gl_FragColor.a * fsize.z;\n"
"vec4 base = texture2DLod(srctex, tc * srctexscale.xy, 0.0);\n"
"gl_FragColor.ar = clamp((1.0 - gl_FragColor.ar) - (1.0 - gl_FragColor.ra) * 0.5, 0.0, 1.0);\n"
"gl_FragColor = (scolor * gl_FragColor.a + scolor2 * gl_FragColor.r\n"
"+ base * (1.0 - gl_FragColor.a - gl_FragColor.r)) * base.a;\n"
"gl_FragColor = (gl_FragColor * vec4(fucxmul.rgb,1.0)) * fucxmul.a;\n"
"gl_FragColor += fucxadd * gl_FragColor.a;\n"
"gl_FragColor = mix(vec4(1.0,1,1.0,1), gl_FragColor, gl_FragColor.a);\n"
"}\n";

extern const char* pSource_FBox2InnerShadowHighlightKnockout;
const char* pSource_FBox2InnerShadowHighlightKnockout = 
"uniform vec4 fsize;\n"
"uniform vec4 offset;\n"
"uniform vec4 scolor;\n"
"uniform vec4 scolor2;\n"
"uniform sampler2D srctex;\n"
"uniform vec4 srctexscale;\n"
"uniform sampler2D tex;\n"
"uniform vec4 texscale;\n"
"varying vec4 fucxadd;\n"
"varying vec4 fucxmul;\n"
"varying vec2 tc;\n"
"void main() {\n"
"gl_FragColor       = vec4(0.0, 0.0, 0.0, 0.0);\n"
"vec4 color = vec4(0.0, 0.0, 0.0, 0.0);\n"
"vec2 i = vec2(0.0, 0.0);\n"
"for (i.x = -fsize.x; i.x <= fsize.x; i.x++)\n"
"{\n"
"for (i.y = -fsize.y; i.y <= fsize.y; i.y++)\n"
"{\n"
"color.a += texture2DLod(tex, tc + (offset.xy + i) * texscale.xy, 0.0).a;\n"
"color.r += texture2DLod(tex, tc - (offset.xy + i) * texscale.xy, 0.0).a;\n"
"}\n"
"} // EndBox2.\n"
"gl_FragColor = color * fsize.w;\n"
"gl_FragColor.a = gl_FragColor.a * fsize.z;\n"
"vec4 base = texture2DLod(srctex, tc * srctexscale.xy, 0.0);\n"
"gl_FragColor.ar = clamp((1.0 - gl_FragColor.ar) - (1.0 - gl_FragColor.ra) * 0.5, 0.0, 1.0);\n"
"gl_FragColor = (scolor * gl_FragColor.a + scolor2 * gl_FragColor.r\n"
"+ base * (1.0 - gl_FragColor.a - gl_FragColor.r)) * base.a;\n"
"gl_FragColor -= base;\n"
"gl_FragColor = (gl_FragColor * vec4(fucxmul.rgb,1.0)) * fucxmul.a;\n"
"gl_FragColor += fucxadd * gl_FragColor.a;\n"
"}\n";

extern const char* pSource_FBox2InnerShadowHighlightKnockoutMul;
const char* pSource_FBox2InnerShadowHighlightKnockoutMul = 
"uniform vec4 fsize;\n"
"uniform vec4 offset;\n"
"uniform vec4 scolor;\n"
"uniform vec4 scolor2;\n"
"uniform sampler2D srctex;\n"
"uniform vec4 srctexscale;\n"
"uniform sampler2D tex;\n"
"uniform vec4 texscale;\n"
"varying vec4 fucxadd;\n"
"varying vec4 fucxmul;\n"
"varying vec2 tc;\n"
"void main() {\n"
"gl_FragColor       = vec4(0.0, 0.0, 0.0, 0.0);\n"
"vec4 color = vec4(0.0, 0.0, 0.0, 0.0);\n"
"vec2 i = vec2(0.0, 0.0);\n"
"for (i.x = -fsize.x; i.x <= fsize.x; i.x++)\n"
"{\n"
"for (i.y = -fsize.y; i.y <= fsize.y; i.y++)\n"
"{\n"
"color.a += texture2DLod(tex, tc + (offset.xy + i) * texscale.xy, 0.0).a;\n"
"color.r += texture2DLod(tex, tc - (offset.xy + i) * texscale.xy, 0.0).a;\n"
"}\n"
"} // EndBox2.\n"
"gl_FragColor = color * fsize.w;\n"
"gl_FragColor.a = gl_FragColor.a * fsize.z;\n"
"vec4 base = texture2DLod(srctex, tc * srctexscale.xy, 0.0);\n"
"gl_FragColor.ar = clamp((1.0 - gl_FragColor.ar) - (1.0 - gl_FragColor.ra) * 0.5, 0.0, 1.0);\n"
"gl_FragColor = (scolor * gl_FragColor.a + scolor2 * gl_FragColor.r\n"
"+ base * (1.0 - gl_FragColor.a - gl_FragColor.r)) * base.a;\n"
"gl_FragColor -= base;\n"
"gl_FragColor = (gl_FragColor * vec4(fucxmul.rgb,1.0)) * fucxmul.a;\n"
"gl_FragColor += fucxadd * gl_FragColor.a;\n"
"gl_FragColor = mix(vec4(1.0,1,1.0,1), gl_FragColor, gl_FragColor.a);\n"
"}\n";

extern const char* pSource_FBox2ShadowonlyHighlight;
const char* pSource_FBox2ShadowonlyHighlight = 
"uniform vec4 fsize;\n"
"uniform vec4 offset;\n"
"uniform vec4 scolor;\n"
"uniform sampler2D tex;\n"
"uniform vec4 texscale;\n"
"varying vec4 fucxadd;\n"
"varying vec4 fucxmul;\n"
"varying vec2 tc;\n"
"void main() {\n"
"gl_FragColor       = vec4(0.0, 0.0, 0.0, 0.0);\n"
"vec4 color = vec4(0.0, 0.0, 0.0, 0.0);\n"
"vec2 i = vec2(0.0, 0.0);\n"
"for (i.x = -fsize.x; i.x <= fsize.x; i.x++)\n"
"{\n"
"for (i.y = -fsize.y; i.y <= fsize.y; i.y++)\n"
"{\n"
"color.a += texture2DLod(tex, tc + (offset.xy + i) * texscale.xy, 0.0).a;\n"
"color.r += texture2DLod(tex, tc - (offset.xy + i) * texscale.xy, 0.0).a;\n"
"}\n"
"} // EndBox2.\n"
"gl_FragColor = color * fsize.w;\n"
"gl_FragColor.a = gl_FragColor.a * fsize.z;\n"
"gl_FragColor = scolor * gl_FragColor.a;\n"
"gl_FragColor = (gl_FragColor * vec4(fucxmul.rgb,1.0)) * fucxmul.a;\n"
"gl_FragColor += fucxadd * gl_FragColor.a;\n"
"}\n";

extern const char* pSource_FBox2ShadowonlyHighlightMul;
const char* pSource_FBox2ShadowonlyHighlightMul = 
"uniform vec4 fsize;\n"
"uniform vec4 offset;\n"
"uniform vec4 scolor;\n"
"uniform sampler2D tex;\n"
"uniform vec4 texscale;\n"
"varying vec4 fucxadd;\n"
"varying vec4 fucxmul;\n"
"varying vec2 tc;\n"
"void main() {\n"
"gl_FragColor       = vec4(0.0, 0.0, 0.0, 0.0);\n"
"vec4 color = vec4(0.0, 0.0, 0.0, 0.0);\n"
"vec2 i = vec2(0.0, 0.0);\n"
"for (i.x = -fsize.x; i.x <= fsize.x; i.x++)\n"
"{\n"
"for (i.y = -fsize.y; i.y <= fsize.y; i.y++)\n"
"{\n"
"color.a += texture2DLod(tex, tc + (offset.xy + i) * texscale.xy, 0.0).a;\n"
"color.r += texture2DLod(tex, tc - (offset.xy + i) * texscale.xy, 0.0).a;\n"
"}\n"
"} // EndBox2.\n"
"gl_FragColor = color * fsize.w;\n"
"gl_FragColor.a = gl_FragColor.a * fsize.z;\n"
"gl_FragColor = scolor * gl_FragColor.a;\n"
"gl_FragColor = (gl_FragColor * vec4(fucxmul.rgb,1.0)) * fucxmul.a;\n"
"gl_FragColor += fucxadd * gl_FragColor.a;\n"
"gl_FragColor = mix(vec4(1.0,1,1.0,1), gl_FragColor, gl_FragColor.a);\n"
"}\n";

extern const char* pSource_VDrawableCopyPixels;
const char* pSource_VDrawableCopyPixels = 
"uniform vec4 mvp[2];\n"
"uniform vec4 texgen[4];\n"
"attribute vec4 pos;\n"
"varying vec2 tc0;\n"
"varying vec2 tc1;\n"
"void main() {\n"
"gl_Position = vec4(0.0,0,0.0,1);\n"
"gl_Position.x = dot(pos, mvp[int(0.0)]);\n"
"gl_Position.y = dot(pos, mvp[int(1.0)]);\n"
"tc0.x = dot(pos, texgen[int(0.0)]);\n"
"tc0.y = dot(pos, texgen[int(1.0)]);\n"
"tc1.x = dot(pos, texgen[int(2.0)]);\n"
"tc1.y = dot(pos, texgen[int(3.0)]);\n"
"}\n";

extern const char* pSource_FDrawableCopyPixels;
const char* pSource_FDrawableCopyPixels = 
"uniform sampler2D tex[2];\n"
"varying vec2 tc0;\n"
"varying vec2 tc1;\n"
"void main() {\n"
"vec4 fcolor_org = texture2D(tex[int(0.0)], tc0);\n"
"vec4 fcolor_src = texture2D(tex[int(1.0)], tc1);\n"
"gl_FragColor = fcolor_src;\n"
"float inAlpha = fcolor_src.a;\n"
"}\n";

extern const char* pSource_FDrawableCopyPixelsCopyLerp;
const char* pSource_FDrawableCopyPixelsCopyLerp = 
"uniform sampler2D tex[2];\n"
"varying vec2 tc0;\n"
"varying vec2 tc1;\n"
"void main() {\n"
"vec4 fcolor_org = texture2D(tex[int(0.0)], tc0);\n"
"vec4 fcolor_src = texture2D(tex[int(1.0)], tc1);\n"
"gl_FragColor = fcolor_src;\n"
"float inAlpha = fcolor_src.a;\n"
"gl_FragColor.rgb = mix(fcolor_org.rgb, fcolor_src.rgb, inAlpha / gl_FragColor.a);\n"
"}\n";

extern const char* pSource_FDrawableCopyPixelsNoDestAlpha;
const char* pSource_FDrawableCopyPixelsNoDestAlpha = 
"uniform sampler2D tex[2];\n"
"varying vec2 tc0;\n"
"varying vec2 tc1;\n"
"void main() {\n"
"vec4 fcolor_org = texture2D(tex[int(0.0)], tc0);\n"
"vec4 fcolor_src = texture2D(tex[int(1.0)], tc1);\n"
"gl_FragColor = fcolor_src;\n"
"float inAlpha = fcolor_src.a;\n"
"gl_FragColor.a = 1.0;\n"
"}\n";

extern const char* pSource_FDrawableCopyPixelsNoDestAlphaCopyLerp;
const char* pSource_FDrawableCopyPixelsNoDestAlphaCopyLerp = 
"uniform sampler2D tex[2];\n"
"varying vec2 tc0;\n"
"varying vec2 tc1;\n"
"void main() {\n"
"vec4 fcolor_org = texture2D(tex[int(0.0)], tc0);\n"
"vec4 fcolor_src = texture2D(tex[int(1.0)], tc1);\n"
"gl_FragColor = fcolor_src;\n"
"float inAlpha = fcolor_src.a;\n"
"gl_FragColor.a = 1.0;\n"
"gl_FragColor.rgb = mix(fcolor_org.rgb, fcolor_src.rgb, inAlpha / gl_FragColor.a);\n"
"}\n";

extern const char* pSource_FDrawableCopyPixelsMergeAlpha;
const char* pSource_FDrawableCopyPixelsMergeAlpha = 
"uniform sampler2D tex[2];\n"
"varying vec2 tc0;\n"
"varying vec2 tc1;\n"
"void main() {\n"
"vec4 fcolor_org = texture2D(tex[int(0.0)], tc0);\n"
"vec4 fcolor_src = texture2D(tex[int(1.0)], tc1);\n"
"gl_FragColor = fcolor_src;\n"
"float inAlpha = fcolor_src.a;\n"
"gl_FragColor.a = mix(inAlpha, 1.0, fcolor_org.a);\n"
"}\n";

extern const char* pSource_FDrawableCopyPixelsMergeAlphaCopyLerp;
const char* pSource_FDrawableCopyPixelsMergeAlphaCopyLerp = 
"uniform sampler2D tex[2];\n"
"varying vec2 tc0;\n"
"varying vec2 tc1;\n"
"void main() {\n"
"vec4 fcolor_org = texture2D(tex[int(0.0)], tc0);\n"
"vec4 fcolor_src = texture2D(tex[int(1.0)], tc1);\n"
"gl_FragColor = fcolor_src;\n"
"float inAlpha = fcolor_src.a;\n"
"gl_FragColor.a = mix(inAlpha, 1.0, fcolor_org.a);\n"
"gl_FragColor.rgb = mix(fcolor_org.rgb, fcolor_src.rgb, inAlpha / gl_FragColor.a);\n"
"}\n";

extern const char* pSource_FDrawableCopyPixelsMergeAlphaNoDestAlpha;
const char* pSource_FDrawableCopyPixelsMergeAlphaNoDestAlpha = 
"uniform sampler2D tex[2];\n"
"varying vec2 tc0;\n"
"varying vec2 tc1;\n"
"void main() {\n"
"vec4 fcolor_org = texture2D(tex[int(0.0)], tc0);\n"
"vec4 fcolor_src = texture2D(tex[int(1.0)], tc1);\n"
"gl_FragColor = fcolor_src;\n"
"float inAlpha = fcolor_src.a;\n"
"gl_FragColor.a = mix(inAlpha, 1.0, fcolor_org.a);\n"
"gl_FragColor.a = 1.0;\n"
"}\n";

extern const char* pSource_FDrawableCopyPixelsMergeAlphaNoDestAlphaCopyLerp;
const char* pSource_FDrawableCopyPixelsMergeAlphaNoDestAlphaCopyLerp = 
"uniform sampler2D tex[2];\n"
"varying vec2 tc0;\n"
"varying vec2 tc1;\n"
"void main() {\n"
"vec4 fcolor_org = texture2D(tex[int(0.0)], tc0);\n"
"vec4 fcolor_src = texture2D(tex[int(1.0)], tc1);\n"
"gl_FragColor = fcolor_src;\n"
"float inAlpha = fcolor_src.a;\n"
"gl_FragColor.a = mix(inAlpha, 1.0, fcolor_org.a);\n"
"gl_FragColor.a = 1.0;\n"
"gl_FragColor.rgb = mix(fcolor_org.rgb, fcolor_src.rgb, inAlpha / gl_FragColor.a);\n"
"}\n";

extern const char* pSource_VDrawableCopyPixelsAlpha;
const char* pSource_VDrawableCopyPixelsAlpha = 
"uniform vec4 mvp[2];\n"
"uniform vec4 texgen[6];\n"
"attribute vec4 pos;\n"
"varying vec2 tc0;\n"
"varying vec2 tc1;\n"
"varying vec2 tc2;\n"
"void main() {\n"
"gl_Position = vec4(0.0,0,0.0,1);\n"
"gl_Position.x = dot(pos, mvp[int(0.0)]);\n"
"gl_Position.y = dot(pos, mvp[int(1.0)]);\n"
"tc0.x = dot(pos, texgen[int(0.0)]);\n"
"tc0.y = dot(pos, texgen[int(1.0)]);\n"
"tc1.x = dot(pos, texgen[int(2.0)]);\n"
"tc1.y = dot(pos, texgen[int(3.0)]);\n"
"tc2.x = dot(pos, texgen[int(4.0)]);\n"
"tc2.y = dot(pos, texgen[int(5.0)]);\n"
"}\n";

extern const char* pSource_FDrawableCopyPixelsAlpha;
const char* pSource_FDrawableCopyPixelsAlpha = 
"uniform sampler2D tex[3];\n"
"varying vec2 tc0;\n"
"varying vec2 tc1;\n"
"varying vec2 tc2;\n"
"void main() {\n"
"vec4 fcolor_org = texture2D(tex[int(0.0)], tc0);\n"
"vec4 fcolor_src = texture2D(tex[int(1.0)], tc1);\n"
"vec4 fcolor_alp = texture2D(tex[int(2.0)], tc2);\n"
"gl_FragColor = fcolor_src;\n"
"float inAlpha = fcolor_src.a * fcolor_alp.a;\n"
"gl_FragColor.a = inAlpha;\n"
"}\n";

extern const char* pSource_FDrawableCopyPixelsAlphaCopyLerp;
const char* pSource_FDrawableCopyPixelsAlphaCopyLerp = 
"uniform sampler2D tex[3];\n"
"varying vec2 tc0;\n"
"varying vec2 tc1;\n"
"varying vec2 tc2;\n"
"void main() {\n"
"vec4 fcolor_org = texture2D(tex[int(0.0)], tc0);\n"
"vec4 fcolor_src = texture2D(tex[int(1.0)], tc1);\n"
"vec4 fcolor_alp = texture2D(tex[int(2.0)], tc2);\n"
"gl_FragColor = fcolor_src;\n"
"float inAlpha = fcolor_src.a * fcolor_alp.a;\n"
"gl_FragColor.a = inAlpha;\n"
"gl_FragColor.rgb = mix(fcolor_org.rgb, fcolor_src.rgb, inAlpha / gl_FragColor.a);\n"
"}\n";

extern const char* pSource_FDrawableCopyPixelsAlphaNoDestAlpha;
const char* pSource_FDrawableCopyPixelsAlphaNoDestAlpha = 
"uniform sampler2D tex[3];\n"
"varying vec2 tc0;\n"
"varying vec2 tc1;\n"
"varying vec2 tc2;\n"
"void main() {\n"
"vec4 fcolor_org = texture2D(tex[int(0.0)], tc0);\n"
"vec4 fcolor_src = texture2D(tex[int(1.0)], tc1);\n"
"vec4 fcolor_alp = texture2D(tex[int(2.0)], tc2);\n"
"gl_FragColor = fcolor_src;\n"
"float inAlpha = fcolor_src.a * fcolor_alp.a;\n"
"gl_FragColor.a = inAlpha;\n"
"gl_FragColor.a = 1.0;\n"
"}\n";

extern const char* pSource_FDrawableCopyPixelsAlphaNoDestAlphaCopyLerp;
const char* pSource_FDrawableCopyPixelsAlphaNoDestAlphaCopyLerp = 
"uniform sampler2D tex[3];\n"
"varying vec2 tc0;\n"
"varying vec2 tc1;\n"
"varying vec2 tc2;\n"
"void main() {\n"
"vec4 fcolor_org = texture2D(tex[int(0.0)], tc0);\n"
"vec4 fcolor_src = texture2D(tex[int(1.0)], tc1);\n"
"vec4 fcolor_alp = texture2D(tex[int(2.0)], tc2);\n"
"gl_FragColor = fcolor_src;\n"
"float inAlpha = fcolor_src.a * fcolor_alp.a;\n"
"gl_FragColor.a = inAlpha;\n"
"gl_FragColor.a = 1.0;\n"
"gl_FragColor.rgb = mix(fcolor_org.rgb, fcolor_src.rgb, inAlpha / gl_FragColor.a);\n"
"}\n";

extern const char* pSource_FDrawableCopyPixelsAlphaMergeAlpha;
const char* pSource_FDrawableCopyPixelsAlphaMergeAlpha = 
"uniform sampler2D tex[3];\n"
"varying vec2 tc0;\n"
"varying vec2 tc1;\n"
"varying vec2 tc2;\n"
"void main() {\n"
"vec4 fcolor_org = texture2D(tex[int(0.0)], tc0);\n"
"vec4 fcolor_src = texture2D(tex[int(1.0)], tc1);\n"
"vec4 fcolor_alp = texture2D(tex[int(2.0)], tc2);\n"
"gl_FragColor = fcolor_src;\n"
"float inAlpha = fcolor_src.a * fcolor_alp.a;\n"
"gl_FragColor.a = inAlpha;\n"
"gl_FragColor.a = mix(inAlpha, 1.0, fcolor_org.a);\n"
"}\n";

extern const char* pSource_FDrawableCopyPixelsAlphaMergeAlphaCopyLerp;
const char* pSource_FDrawableCopyPixelsAlphaMergeAlphaCopyLerp = 
"uniform sampler2D tex[3];\n"
"varying vec2 tc0;\n"
"varying vec2 tc1;\n"
"varying vec2 tc2;\n"
"void main() {\n"
"vec4 fcolor_org = texture2D(tex[int(0.0)], tc0);\n"
"vec4 fcolor_src = texture2D(tex[int(1.0)], tc1);\n"
"vec4 fcolor_alp = texture2D(tex[int(2.0)], tc2);\n"
"gl_FragColor = fcolor_src;\n"
"float inAlpha = fcolor_src.a * fcolor_alp.a;\n"
"gl_FragColor.a = inAlpha;\n"
"gl_FragColor.a = mix(inAlpha, 1.0, fcolor_org.a);\n"
"gl_FragColor.rgb = mix(fcolor_org.rgb, fcolor_src.rgb, inAlpha / gl_FragColor.a);\n"
"}\n";

extern const char* pSource_FDrawableCopyPixelsAlphaMergeAlphaNoDestAlpha;
const char* pSource_FDrawableCopyPixelsAlphaMergeAlphaNoDestAlpha = 
"uniform sampler2D tex[3];\n"
"varying vec2 tc0;\n"
"varying vec2 tc1;\n"
"varying vec2 tc2;\n"
"void main() {\n"
"vec4 fcolor_org = texture2D(tex[int(0.0)], tc0);\n"
"vec4 fcolor_src = texture2D(tex[int(1.0)], tc1);\n"
"vec4 fcolor_alp = texture2D(tex[int(2.0)], tc2);\n"
"gl_FragColor = fcolor_src;\n"
"float inAlpha = fcolor_src.a * fcolor_alp.a;\n"
"gl_FragColor.a = inAlpha;\n"
"gl_FragColor.a = mix(inAlpha, 1.0, fcolor_org.a);\n"
"gl_FragColor.a = 1.0;\n"
"}\n";

extern const char* pSource_FDrawableCopyPixelsAlphaMergeAlphaNoDestAlphaCopyLerp;
const char* pSource_FDrawableCopyPixelsAlphaMergeAlphaNoDestAlphaCopyLerp = 
"uniform sampler2D tex[3];\n"
"varying vec2 tc0;\n"
"varying vec2 tc1;\n"
"varying vec2 tc2;\n"
"void main() {\n"
"vec4 fcolor_org = texture2D(tex[int(0.0)], tc0);\n"
"vec4 fcolor_src = texture2D(tex[int(1.0)], tc1);\n"
"vec4 fcolor_alp = texture2D(tex[int(2.0)], tc2);\n"
"gl_FragColor = fcolor_src;\n"
"float inAlpha = fcolor_src.a * fcolor_alp.a;\n"
"gl_FragColor.a = inAlpha;\n"
"gl_FragColor.a = mix(inAlpha, 1.0, fcolor_org.a);\n"
"gl_FragColor.a = 1.0;\n"
"gl_FragColor.rgb = mix(fcolor_org.rgb, fcolor_src.rgb, inAlpha / gl_FragColor.a);\n"
"}\n";

extern const char* pSource_FDrawableMerge;
const char* pSource_FDrawableMerge = 
"uniform mat4 cxmul;\n"
"uniform mat4 cxmul1;\n"
"uniform sampler2D tex[2];\n"
"varying vec2 tc0;\n"
"varying vec2 tc1;\n"
"void main() {\n"
"vec4 fcolor_original = texture2D(tex[int(0.0)], tc0);\n"
"vec4 fcolor_source   = texture2D(tex[int(1.0)], tc1);\n"
"gl_FragColor = (fcolor_original) * ( cxmul) + (fcolor_source) * ( cxmul1);\n"
"}\n";

extern const char* pSource_FDrawableCompare;
const char* pSource_FDrawableCompare = 
"uniform sampler2D tex[2];\n"
"varying vec2 tc0;\n"
"varying vec2 tc1;\n"
"void main() {\n"
"vec4 fcolor0 = texture2D(tex[int(0.0)], tc0);\n"
"vec4 fcolor1 = texture2D(tex[int(1.0)], tc1);\n"
"vec4 diff = fcolor0 - fcolor1;\n"
"vec4 oneValue = vec4(1.0/255.0, 1.0/255.0, 1.0/255.0, 1.0/255.0 );\n"
"vec4 ltZero = (sign(diff)+vec4(1.0,1,1.0,1))*-0.25;\n"
"vec4 partDiff = oneValue * (sign(ltZero)+vec4(1.0,1,1.0,1));\n"
"vec4 wrapDiff = fract(diff + vec4(1.0,1,1.0,1)) + partDiff;\n"
"float rgbdiff = sign(dot(wrapDiff.rgb, vec3(1.0,1,1.0)));\n"
"gl_FragColor = mix( vec4(1.0,1,1.0, wrapDiff.a), vec4(wrapDiff.rgb, 1.0), rgbdiff );\n"
"}\n";

extern const char* pSource_FDrawablePaletteMap;
const char* pSource_FDrawablePaletteMap = 
"uniform sampler2D srctex;\n"
"uniform sampler2D tex;\n"
"varying vec2 tc;\n"
"void main() {\n"
"vec4 fchannels = texture2D(tex, tc);\n"
"gl_FragColor  = texture2D(srctex, vec2(fchannels.r, 0.125));\n"
"gl_FragColor += texture2D(srctex, vec2(fchannels.g, 0.375));\n"
"gl_FragColor += texture2D(srctex, vec2(fchannels.b, 0.625));\n"
"gl_FragColor += texture2D(srctex, vec2(fchannels.a, 0.875));\n"
"}\n";

}}}; // Scaleform::Render::GL


