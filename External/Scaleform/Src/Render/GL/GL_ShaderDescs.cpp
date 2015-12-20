/**************************************************************************

    PublicHeader:   Render
    Filename    :   GL_ShaderDescs.cpp
    Content     :   GL Shader descriptors
    Created     :   6/27/2012
    Authors     :   Automatically generated.

    Copyright   :   Copyright 2012 Autodesk, Inc. All Rights reserved.

    Use of this software is subject to the terms of the Autodesk license
    agreement provided at the time of installation or download, or which
    otherwise accompanies this software in either electronic or hard copy form.

**************************************************************************/

#include "Render/GL/GL_Shader.h"
#include "Render/GL/GL_ShaderDescs.h"

namespace Scaleform { namespace Render { namespace GL {

const char* ShaderUniformNames[Uniform::SU_Count] = {
    "cxadd",
    "cxmul",
    "cxmul1",
    "fsize",
    "mvp",
    "offset",
    "scolor",
    "scolor2",
    "srctex",
    "srctexscale",
    "tex",
    "texgen",
    "texscale",
    "vfmuniforms",
    "vfuniforms",
};

const unsigned char Uniform::UniformFlags[Uniform::SU_Count] = {
    0,  // cxadd
    0,  // cxmul
    0,  // cxmul1
    0,  // fsize
    0,  // mvp
    0,  // offset
    0,  // scolor
    0,  // scolor2
    0,  // srctex
    0,  // srctexscale
    0,  // tex
    0,  // texgen
    0,  // texscale
    0,  // vfmuniforms
    0,  // vfuniforms
};

extern const char* pSource_VTexTG;
extern const char* pSource_VTexTGEAlpha;
extern const char* pSource_VTexTGCxform;
extern const char* pSource_VTexTGCxformEAlpha;
extern const char* pSource_VVertex;
extern const char* pSource_VVertexEAlpha;
extern const char* pSource_VVertexCxform;
extern const char* pSource_VVertexCxformEAlpha;
extern const char* pSource_VTexTGTexTG;
extern const char* pSource_VTexTGTexTGCxform;
extern const char* pSource_VTexTGVertex;
extern const char* pSource_VTexTGVertexCxform;
extern const char* pSource_VPosition3dTexTG;
extern const char* pSource_VPosition3dTexTGEAlpha;
extern const char* pSource_VPosition3dTexTGCxform;
extern const char* pSource_VPosition3dTexTGCxformEAlpha;
extern const char* pSource_VPosition3dVertex;
extern const char* pSource_VPosition3dVertexEAlpha;
extern const char* pSource_VPosition3dVertexCxform;
extern const char* pSource_VPosition3dVertexCxformEAlpha;
extern const char* pSource_VPosition3dTexTGTexTG;
extern const char* pSource_VPosition3dTexTGTexTGCxform;
extern const char* pSource_VPosition3dTexTGVertex;
extern const char* pSource_VPosition3dTexTGVertexCxform;
extern const char* pSource_VBatchTexTG;
extern const char* pSource_VBatchTexTGEAlpha;
extern const char* pSource_VBatchTexTGCxform;
extern const char* pSource_VBatchTexTGCxformEAlpha;
extern const char* pSource_VBatchVertex;
extern const char* pSource_VBatchVertexEAlpha;
extern const char* pSource_VBatchVertexCxform;
extern const char* pSource_VBatchVertexCxformEAlpha;
extern const char* pSource_VBatchTexTGTexTG;
extern const char* pSource_VBatchTexTGTexTGCxform;
extern const char* pSource_VBatchTexTGVertex;
extern const char* pSource_VBatchTexTGVertexCxform;
extern const char* pSource_VBatchPosition3dTexTG;
extern const char* pSource_VBatchPosition3dTexTGEAlpha;
extern const char* pSource_VBatchPosition3dTexTGCxform;
extern const char* pSource_VBatchPosition3dTexTGCxformEAlpha;
extern const char* pSource_VBatchPosition3dVertex;
extern const char* pSource_VBatchPosition3dVertexEAlpha;
extern const char* pSource_VBatchPosition3dVertexCxform;
extern const char* pSource_VBatchPosition3dVertexCxformEAlpha;
extern const char* pSource_VBatchPosition3dTexTGTexTG;
extern const char* pSource_VBatchPosition3dTexTGTexTGCxform;
extern const char* pSource_VBatchPosition3dTexTGVertex;
extern const char* pSource_VBatchPosition3dTexTGVertexCxform;
extern const char* pSource_VInstancedTexTG;
extern const char* pSource_VInstancedTexTGEAlpha;
extern const char* pSource_VInstancedTexTGCxform;
extern const char* pSource_VInstancedTexTGCxformEAlpha;
extern const char* pSource_VInstancedVertex;
extern const char* pSource_VInstancedVertexEAlpha;
extern const char* pSource_VInstancedVertexCxform;
extern const char* pSource_VInstancedVertexCxformEAlpha;
extern const char* pSource_VInstancedTexTGTexTG;
extern const char* pSource_VInstancedTexTGTexTGCxform;
extern const char* pSource_VInstancedTexTGVertex;
extern const char* pSource_VInstancedTexTGVertexCxform;
extern const char* pSource_VInstancedPosition3dTexTG;
extern const char* pSource_VInstancedPosition3dTexTGEAlpha;
extern const char* pSource_VInstancedPosition3dTexTGCxform;
extern const char* pSource_VInstancedPosition3dTexTGCxformEAlpha;
extern const char* pSource_VInstancedPosition3dVertex;
extern const char* pSource_VInstancedPosition3dVertexEAlpha;
extern const char* pSource_VInstancedPosition3dVertexCxform;
extern const char* pSource_VInstancedPosition3dVertexCxformEAlpha;
extern const char* pSource_VInstancedPosition3dTexTGTexTG;
extern const char* pSource_VInstancedPosition3dTexTGTexTGCxform;
extern const char* pSource_VInstancedPosition3dTexTGVertex;
extern const char* pSource_VInstancedPosition3dTexTGVertexCxform;
extern const char* pSource_VSolid;
extern const char* pSource_VText;
extern const char* pSource_VTextColor;
extern const char* pSource_VTextColorCxform;
extern const char* pSource_VPosition3dSolid;
extern const char* pSource_VPosition3dText;
extern const char* pSource_VPosition3dTextColor;
extern const char* pSource_VPosition3dTextColorCxform;
extern const char* pSource_VBatchSolid;
extern const char* pSource_VBatchText;
extern const char* pSource_VBatchTextColor;
extern const char* pSource_VBatchTextColorCxform;
extern const char* pSource_VBatchPosition3dSolid;
extern const char* pSource_VBatchPosition3dText;
extern const char* pSource_VBatchPosition3dTextColor;
extern const char* pSource_VBatchPosition3dTextColorCxform;
extern const char* pSource_VInstancedSolid;
extern const char* pSource_VInstancedText;
extern const char* pSource_VInstancedTextColor;
extern const char* pSource_VInstancedTextColorCxform;
extern const char* pSource_VInstancedPosition3dSolid;
extern const char* pSource_VInstancedPosition3dText;
extern const char* pSource_VInstancedPosition3dTextColor;
extern const char* pSource_VInstancedPosition3dTextColorCxform;
extern const char* pSource_VTexTGCxform;
extern const char* pSource_VDrawableCopyPixels;
extern const char* pSource_VDrawableCopyPixelsAlpha;

static VertexShaderDesc ShaderDesc_VS_VTexTG = {
    /* Type */          VertexShaderDesc::VS_VTexTG,
    /* Flags */         0,
    /* pSource */       pSource_VTexTG,
    /* Uniforms */      { 
                          /* cxadd*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      { -1, 0, 0, 0, 0, 0 },
                          /* mvp*/        {0, 0, 4, 8, 4, 0 },
                          /* offset*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor2*/    { -1, 0, 0, 0, 0, 0 },
                          /* srctex*/     { -1, 0, 0, 0, 0, 0 },
                          /* srctexscale*/{ -1, 0, 0, 0, 0, 0 },
                          /* tex*/        { -1, 0, 0, 0, 0, 0 },
                          /* texgen*/     {2, 8, 4, 8, 4, 0 },
                          /* texscale*/   { -1, 0, 0, 0, 0, 0 },
                          /* vfmuniforms*/{ -1, 0, 0, 0, 0, 0 },
                          /* vfuniforms*/ { -1, 0, 0, 0, 0, 0 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_Count, -1, 0},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_Count, -1, 0},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    /* NumAttribs */    1,
    /* Attributes */    {
                          { "pos",        4 | VET_Pos},
                          },
    
    };

static VertexShaderDesc ShaderDesc_VS_VTexTGEAlpha = {
    /* Type */          VertexShaderDesc::VS_VTexTGEAlpha,
    /* Flags */         0,
    /* pSource */       pSource_VTexTGEAlpha,
    /* Uniforms */      { 
                          /* cxadd*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      { -1, 0, 0, 0, 0, 0 },
                          /* mvp*/        {0, 0, 4, 8, 4, 0 },
                          /* offset*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor2*/    { -1, 0, 0, 0, 0, 0 },
                          /* srctex*/     { -1, 0, 0, 0, 0, 0 },
                          /* srctexscale*/{ -1, 0, 0, 0, 0, 0 },
                          /* tex*/        { -1, 0, 0, 0, 0, 0 },
                          /* texgen*/     {2, 8, 4, 8, 4, 0 },
                          /* texscale*/   { -1, 0, 0, 0, 0, 0 },
                          /* vfmuniforms*/{ -1, 0, 0, 0, 0, 0 },
                          /* vfuniforms*/ { -1, 0, 0, 0, 0, 0 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_Count, -1, 0},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_Count, -1, 0},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    /* NumAttribs */    2,
    /* Attributes */    {
                          { "afactor",    4 | VET_Color | (1 << VET_Index_Shift)},
                          { "pos",        4 | VET_Pos},
                          },
    
    };

static VertexShaderDesc ShaderDesc_VS_VTexTGCxform = {
    /* Type */          VertexShaderDesc::VS_VTexTGCxform,
    /* Flags */         0,
    /* pSource */       pSource_VTexTGCxform,
    /* Uniforms */      { 
                          /* cxadd*/      {0, 0, 4, 4, 4, 0 },
                          /* cxmul*/      {1, 4, 4, 4, 4, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      { -1, 0, 0, 0, 0, 0 },
                          /* mvp*/        {2, 8, 4, 8, 4, 0 },
                          /* offset*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor2*/    { -1, 0, 0, 0, 0, 0 },
                          /* srctex*/     { -1, 0, 0, 0, 0, 0 },
                          /* srctexscale*/{ -1, 0, 0, 0, 0, 0 },
                          /* tex*/        { -1, 0, 0, 0, 0, 0 },
                          /* texgen*/     {4, 16, 4, 8, 4, 0 },
                          /* texscale*/   { -1, 0, 0, 0, 0, 0 },
                          /* vfmuniforms*/{ -1, 0, 0, 0, 0, 0 },
                          /* vfuniforms*/ { -1, 0, 0, 0, 0, 0 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_Count, -1, 0},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_Count, -1, 0},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    /* NumAttribs */    1,
    /* Attributes */    {
                          { "pos",        4 | VET_Pos},
                          },
    
    };

static VertexShaderDesc ShaderDesc_VS_VTexTGCxformEAlpha = {
    /* Type */          VertexShaderDesc::VS_VTexTGCxformEAlpha,
    /* Flags */         0,
    /* pSource */       pSource_VTexTGCxformEAlpha,
    /* Uniforms */      { 
                          /* cxadd*/      {0, 0, 4, 4, 4, 0 },
                          /* cxmul*/      {1, 4, 4, 4, 4, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      { -1, 0, 0, 0, 0, 0 },
                          /* mvp*/        {2, 8, 4, 8, 4, 0 },
                          /* offset*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor2*/    { -1, 0, 0, 0, 0, 0 },
                          /* srctex*/     { -1, 0, 0, 0, 0, 0 },
                          /* srctexscale*/{ -1, 0, 0, 0, 0, 0 },
                          /* tex*/        { -1, 0, 0, 0, 0, 0 },
                          /* texgen*/     {4, 16, 4, 8, 4, 0 },
                          /* texscale*/   { -1, 0, 0, 0, 0, 0 },
                          /* vfmuniforms*/{ -1, 0, 0, 0, 0, 0 },
                          /* vfuniforms*/ { -1, 0, 0, 0, 0, 0 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_Count, -1, 0},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_Count, -1, 0},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    /* NumAttribs */    2,
    /* Attributes */    {
                          { "afactor",    4 | VET_Color | (1 << VET_Index_Shift)},
                          { "pos",        4 | VET_Pos},
                          },
    
    };

static VertexShaderDesc ShaderDesc_VS_VVertex = {
    /* Type */          VertexShaderDesc::VS_VVertex,
    /* Flags */         0,
    /* pSource */       pSource_VVertex,
    /* Uniforms */      { 
                          /* cxadd*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      { -1, 0, 0, 0, 0, 0 },
                          /* mvp*/        {0, 0, 4, 8, 4, 0 },
                          /* offset*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor2*/    { -1, 0, 0, 0, 0, 0 },
                          /* srctex*/     { -1, 0, 0, 0, 0, 0 },
                          /* srctexscale*/{ -1, 0, 0, 0, 0, 0 },
                          /* tex*/        { -1, 0, 0, 0, 0, 0 },
                          /* texgen*/     { -1, 0, 0, 0, 0, 0 },
                          /* texscale*/   { -1, 0, 0, 0, 0, 0 },
                          /* vfmuniforms*/{ -1, 0, 0, 0, 0, 0 },
                          /* vfuniforms*/ { -1, 0, 0, 0, 0, 0 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_Count, -1, 0},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_Count, -1, 0},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    /* NumAttribs */    2,
    /* Attributes */    {
                          { "acolor",     4 | VET_Color},
                          { "pos",        4 | VET_Pos},
                          },
    
    };

static VertexShaderDesc ShaderDesc_VS_VVertexEAlpha = {
    /* Type */          VertexShaderDesc::VS_VVertexEAlpha,
    /* Flags */         0,
    /* pSource */       pSource_VVertexEAlpha,
    /* Uniforms */      { 
                          /* cxadd*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      { -1, 0, 0, 0, 0, 0 },
                          /* mvp*/        {0, 0, 4, 8, 4, 0 },
                          /* offset*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor2*/    { -1, 0, 0, 0, 0, 0 },
                          /* srctex*/     { -1, 0, 0, 0, 0, 0 },
                          /* srctexscale*/{ -1, 0, 0, 0, 0, 0 },
                          /* tex*/        { -1, 0, 0, 0, 0, 0 },
                          /* texgen*/     { -1, 0, 0, 0, 0, 0 },
                          /* texscale*/   { -1, 0, 0, 0, 0, 0 },
                          /* vfmuniforms*/{ -1, 0, 0, 0, 0, 0 },
                          /* vfuniforms*/ { -1, 0, 0, 0, 0, 0 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_Count, -1, 0},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_Count, -1, 0},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    /* NumAttribs */    3,
    /* Attributes */    {
                          { "acolor",     4 | VET_Color},
                          { "afactor",    4 | VET_Color | (1 << VET_Index_Shift)},
                          { "pos",        4 | VET_Pos},
                          },
    
    };

static VertexShaderDesc ShaderDesc_VS_VVertexCxform = {
    /* Type */          VertexShaderDesc::VS_VVertexCxform,
    /* Flags */         0,
    /* pSource */       pSource_VVertexCxform,
    /* Uniforms */      { 
                          /* cxadd*/      {0, 0, 4, 4, 4, 0 },
                          /* cxmul*/      {1, 4, 4, 4, 4, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      { -1, 0, 0, 0, 0, 0 },
                          /* mvp*/        {2, 8, 4, 8, 4, 0 },
                          /* offset*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor2*/    { -1, 0, 0, 0, 0, 0 },
                          /* srctex*/     { -1, 0, 0, 0, 0, 0 },
                          /* srctexscale*/{ -1, 0, 0, 0, 0, 0 },
                          /* tex*/        { -1, 0, 0, 0, 0, 0 },
                          /* texgen*/     { -1, 0, 0, 0, 0, 0 },
                          /* texscale*/   { -1, 0, 0, 0, 0, 0 },
                          /* vfmuniforms*/{ -1, 0, 0, 0, 0, 0 },
                          /* vfuniforms*/ { -1, 0, 0, 0, 0, 0 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_Count, -1, 0},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_Count, -1, 0},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    /* NumAttribs */    2,
    /* Attributes */    {
                          { "acolor",     4 | VET_Color},
                          { "pos",        4 | VET_Pos},
                          },
    
    };

static VertexShaderDesc ShaderDesc_VS_VVertexCxformEAlpha = {
    /* Type */          VertexShaderDesc::VS_VVertexCxformEAlpha,
    /* Flags */         0,
    /* pSource */       pSource_VVertexCxformEAlpha,
    /* Uniforms */      { 
                          /* cxadd*/      {0, 0, 4, 4, 4, 0 },
                          /* cxmul*/      {1, 4, 4, 4, 4, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      { -1, 0, 0, 0, 0, 0 },
                          /* mvp*/        {2, 8, 4, 8, 4, 0 },
                          /* offset*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor2*/    { -1, 0, 0, 0, 0, 0 },
                          /* srctex*/     { -1, 0, 0, 0, 0, 0 },
                          /* srctexscale*/{ -1, 0, 0, 0, 0, 0 },
                          /* tex*/        { -1, 0, 0, 0, 0, 0 },
                          /* texgen*/     { -1, 0, 0, 0, 0, 0 },
                          /* texscale*/   { -1, 0, 0, 0, 0, 0 },
                          /* vfmuniforms*/{ -1, 0, 0, 0, 0, 0 },
                          /* vfuniforms*/ { -1, 0, 0, 0, 0, 0 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_Count, -1, 0},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_Count, -1, 0},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    /* NumAttribs */    3,
    /* Attributes */    {
                          { "acolor",     4 | VET_Color},
                          { "afactor",    4 | VET_Color | (1 << VET_Index_Shift)},
                          { "pos",        4 | VET_Pos},
                          },
    
    };

static VertexShaderDesc ShaderDesc_VS_VTexTGTexTG = {
    /* Type */          VertexShaderDesc::VS_VTexTGTexTG,
    /* Flags */         0,
    /* pSource */       pSource_VTexTGTexTG,
    /* Uniforms */      { 
                          /* cxadd*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      { -1, 0, 0, 0, 0, 0 },
                          /* mvp*/        {0, 0, 4, 8, 4, 0 },
                          /* offset*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor2*/    { -1, 0, 0, 0, 0, 0 },
                          /* srctex*/     { -1, 0, 0, 0, 0, 0 },
                          /* srctexscale*/{ -1, 0, 0, 0, 0, 0 },
                          /* tex*/        { -1, 0, 0, 0, 0, 0 },
                          /* texgen*/     {2, 8, 4, 16, 4, 0 },
                          /* texscale*/   { -1, 0, 0, 0, 0, 0 },
                          /* vfmuniforms*/{ -1, 0, 0, 0, 0, 0 },
                          /* vfuniforms*/ { -1, 0, 0, 0, 0, 0 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_Count, -1, 0},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_Count, -1, 0},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    /* NumAttribs */    2,
    /* Attributes */    {
                          { "afactor",    4 | VET_Color | (1 << VET_Index_Shift)},
                          { "pos",        4 | VET_Pos},
                          },
    
    };

static VertexShaderDesc ShaderDesc_VS_VTexTGTexTGCxform = {
    /* Type */          VertexShaderDesc::VS_VTexTGTexTGCxform,
    /* Flags */         0,
    /* pSource */       pSource_VTexTGTexTGCxform,
    /* Uniforms */      { 
                          /* cxadd*/      {0, 0, 4, 4, 4, 0 },
                          /* cxmul*/      {1, 4, 4, 4, 4, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      { -1, 0, 0, 0, 0, 0 },
                          /* mvp*/        {2, 8, 4, 8, 4, 0 },
                          /* offset*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor2*/    { -1, 0, 0, 0, 0, 0 },
                          /* srctex*/     { -1, 0, 0, 0, 0, 0 },
                          /* srctexscale*/{ -1, 0, 0, 0, 0, 0 },
                          /* tex*/        { -1, 0, 0, 0, 0, 0 },
                          /* texgen*/     {4, 16, 4, 16, 4, 0 },
                          /* texscale*/   { -1, 0, 0, 0, 0, 0 },
                          /* vfmuniforms*/{ -1, 0, 0, 0, 0, 0 },
                          /* vfuniforms*/ { -1, 0, 0, 0, 0, 0 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_Count, -1, 0},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_Count, -1, 0},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    /* NumAttribs */    2,
    /* Attributes */    {
                          { "afactor",    4 | VET_Color | (1 << VET_Index_Shift)},
                          { "pos",        4 | VET_Pos},
                          },
    
    };

static VertexShaderDesc ShaderDesc_VS_VTexTGVertex = {
    /* Type */          VertexShaderDesc::VS_VTexTGVertex,
    /* Flags */         0,
    /* pSource */       pSource_VTexTGVertex,
    /* Uniforms */      { 
                          /* cxadd*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      { -1, 0, 0, 0, 0, 0 },
                          /* mvp*/        {0, 0, 4, 8, 4, 0 },
                          /* offset*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor2*/    { -1, 0, 0, 0, 0, 0 },
                          /* srctex*/     { -1, 0, 0, 0, 0, 0 },
                          /* srctexscale*/{ -1, 0, 0, 0, 0, 0 },
                          /* tex*/        { -1, 0, 0, 0, 0, 0 },
                          /* texgen*/     {2, 8, 4, 8, 4, 0 },
                          /* texscale*/   { -1, 0, 0, 0, 0, 0 },
                          /* vfmuniforms*/{ -1, 0, 0, 0, 0, 0 },
                          /* vfuniforms*/ { -1, 0, 0, 0, 0, 0 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_Count, -1, 0},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_Count, -1, 0},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    /* NumAttribs */    3,
    /* Attributes */    {
                          { "acolor",     4 | VET_Color},
                          { "afactor",    4 | VET_Color | (1 << VET_Index_Shift)},
                          { "pos",        4 | VET_Pos},
                          },
    
    };

static VertexShaderDesc ShaderDesc_VS_VTexTGVertexCxform = {
    /* Type */          VertexShaderDesc::VS_VTexTGVertexCxform,
    /* Flags */         0,
    /* pSource */       pSource_VTexTGVertexCxform,
    /* Uniforms */      { 
                          /* cxadd*/      {0, 0, 4, 4, 4, 0 },
                          /* cxmul*/      {1, 4, 4, 4, 4, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      { -1, 0, 0, 0, 0, 0 },
                          /* mvp*/        {2, 8, 4, 8, 4, 0 },
                          /* offset*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor2*/    { -1, 0, 0, 0, 0, 0 },
                          /* srctex*/     { -1, 0, 0, 0, 0, 0 },
                          /* srctexscale*/{ -1, 0, 0, 0, 0, 0 },
                          /* tex*/        { -1, 0, 0, 0, 0, 0 },
                          /* texgen*/     {4, 16, 4, 8, 4, 0 },
                          /* texscale*/   { -1, 0, 0, 0, 0, 0 },
                          /* vfmuniforms*/{ -1, 0, 0, 0, 0, 0 },
                          /* vfuniforms*/ { -1, 0, 0, 0, 0, 0 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_Count, -1, 0},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_Count, -1, 0},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    /* NumAttribs */    3,
    /* Attributes */    {
                          { "acolor",     4 | VET_Color},
                          { "afactor",    4 | VET_Color | (1 << VET_Index_Shift)},
                          { "pos",        4 | VET_Pos},
                          },
    
    };

static VertexShaderDesc ShaderDesc_VS_VPosition3dTexTG = {
    /* Type */          VertexShaderDesc::VS_VPosition3dTexTG,
    /* Flags */         0,
    /* pSource */       pSource_VPosition3dTexTG,
    /* Uniforms */      { 
                          /* cxadd*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      { -1, 0, 0, 0, 0, 0 },
                          /* mvp*/        {0, 0, 16, 16, 16, 0 },
                          /* offset*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor2*/    { -1, 0, 0, 0, 0, 0 },
                          /* srctex*/     { -1, 0, 0, 0, 0, 0 },
                          /* srctexscale*/{ -1, 0, 0, 0, 0, 0 },
                          /* tex*/        { -1, 0, 0, 0, 0, 0 },
                          /* texgen*/     {4, 16, 4, 8, 4, 0 },
                          /* texscale*/   { -1, 0, 0, 0, 0, 0 },
                          /* vfmuniforms*/{ -1, 0, 0, 0, 0, 0 },
                          /* vfuniforms*/ { -1, 0, 0, 0, 0, 0 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_Count, -1, 0},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_Count, -1, 0},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    /* NumAttribs */    1,
    /* Attributes */    {
                          { "pos",        4 | VET_Pos},
                          },
    
    };

static VertexShaderDesc ShaderDesc_VS_VPosition3dTexTGEAlpha = {
    /* Type */          VertexShaderDesc::VS_VPosition3dTexTGEAlpha,
    /* Flags */         0,
    /* pSource */       pSource_VPosition3dTexTGEAlpha,
    /* Uniforms */      { 
                          /* cxadd*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      { -1, 0, 0, 0, 0, 0 },
                          /* mvp*/        {0, 0, 16, 16, 16, 0 },
                          /* offset*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor2*/    { -1, 0, 0, 0, 0, 0 },
                          /* srctex*/     { -1, 0, 0, 0, 0, 0 },
                          /* srctexscale*/{ -1, 0, 0, 0, 0, 0 },
                          /* tex*/        { -1, 0, 0, 0, 0, 0 },
                          /* texgen*/     {4, 16, 4, 8, 4, 0 },
                          /* texscale*/   { -1, 0, 0, 0, 0, 0 },
                          /* vfmuniforms*/{ -1, 0, 0, 0, 0, 0 },
                          /* vfuniforms*/ { -1, 0, 0, 0, 0, 0 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_Count, -1, 0},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_Count, -1, 0},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    /* NumAttribs */    2,
    /* Attributes */    {
                          { "afactor",    4 | VET_Color | (1 << VET_Index_Shift)},
                          { "pos",        4 | VET_Pos},
                          },
    
    };

static VertexShaderDesc ShaderDesc_VS_VPosition3dTexTGCxform = {
    /* Type */          VertexShaderDesc::VS_VPosition3dTexTGCxform,
    /* Flags */         0,
    /* pSource */       pSource_VPosition3dTexTGCxform,
    /* Uniforms */      { 
                          /* cxadd*/      {0, 0, 4, 4, 4, 0 },
                          /* cxmul*/      {1, 4, 4, 4, 4, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      { -1, 0, 0, 0, 0, 0 },
                          /* mvp*/        {2, 8, 16, 16, 16, 0 },
                          /* offset*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor2*/    { -1, 0, 0, 0, 0, 0 },
                          /* srctex*/     { -1, 0, 0, 0, 0, 0 },
                          /* srctexscale*/{ -1, 0, 0, 0, 0, 0 },
                          /* tex*/        { -1, 0, 0, 0, 0, 0 },
                          /* texgen*/     {6, 24, 4, 8, 4, 0 },
                          /* texscale*/   { -1, 0, 0, 0, 0, 0 },
                          /* vfmuniforms*/{ -1, 0, 0, 0, 0, 0 },
                          /* vfuniforms*/ { -1, 0, 0, 0, 0, 0 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_Count, -1, 0},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_Count, -1, 0},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    /* NumAttribs */    1,
    /* Attributes */    {
                          { "pos",        4 | VET_Pos},
                          },
    
    };

static VertexShaderDesc ShaderDesc_VS_VPosition3dTexTGCxformEAlpha = {
    /* Type */          VertexShaderDesc::VS_VPosition3dTexTGCxformEAlpha,
    /* Flags */         0,
    /* pSource */       pSource_VPosition3dTexTGCxformEAlpha,
    /* Uniforms */      { 
                          /* cxadd*/      {0, 0, 4, 4, 4, 0 },
                          /* cxmul*/      {1, 4, 4, 4, 4, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      { -1, 0, 0, 0, 0, 0 },
                          /* mvp*/        {2, 8, 16, 16, 16, 0 },
                          /* offset*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor2*/    { -1, 0, 0, 0, 0, 0 },
                          /* srctex*/     { -1, 0, 0, 0, 0, 0 },
                          /* srctexscale*/{ -1, 0, 0, 0, 0, 0 },
                          /* tex*/        { -1, 0, 0, 0, 0, 0 },
                          /* texgen*/     {6, 24, 4, 8, 4, 0 },
                          /* texscale*/   { -1, 0, 0, 0, 0, 0 },
                          /* vfmuniforms*/{ -1, 0, 0, 0, 0, 0 },
                          /* vfuniforms*/ { -1, 0, 0, 0, 0, 0 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_Count, -1, 0},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_Count, -1, 0},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    /* NumAttribs */    2,
    /* Attributes */    {
                          { "afactor",    4 | VET_Color | (1 << VET_Index_Shift)},
                          { "pos",        4 | VET_Pos},
                          },
    
    };

static VertexShaderDesc ShaderDesc_VS_VPosition3dVertex = {
    /* Type */          VertexShaderDesc::VS_VPosition3dVertex,
    /* Flags */         0,
    /* pSource */       pSource_VPosition3dVertex,
    /* Uniforms */      { 
                          /* cxadd*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      { -1, 0, 0, 0, 0, 0 },
                          /* mvp*/        {0, 0, 16, 16, 16, 0 },
                          /* offset*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor2*/    { -1, 0, 0, 0, 0, 0 },
                          /* srctex*/     { -1, 0, 0, 0, 0, 0 },
                          /* srctexscale*/{ -1, 0, 0, 0, 0, 0 },
                          /* tex*/        { -1, 0, 0, 0, 0, 0 },
                          /* texgen*/     { -1, 0, 0, 0, 0, 0 },
                          /* texscale*/   { -1, 0, 0, 0, 0, 0 },
                          /* vfmuniforms*/{ -1, 0, 0, 0, 0, 0 },
                          /* vfuniforms*/ { -1, 0, 0, 0, 0, 0 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_Count, -1, 0},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_Count, -1, 0},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    /* NumAttribs */    2,
    /* Attributes */    {
                          { "acolor",     4 | VET_Color},
                          { "pos",        4 | VET_Pos},
                          },
    
    };

static VertexShaderDesc ShaderDesc_VS_VPosition3dVertexEAlpha = {
    /* Type */          VertexShaderDesc::VS_VPosition3dVertexEAlpha,
    /* Flags */         0,
    /* pSource */       pSource_VPosition3dVertexEAlpha,
    /* Uniforms */      { 
                          /* cxadd*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      { -1, 0, 0, 0, 0, 0 },
                          /* mvp*/        {0, 0, 16, 16, 16, 0 },
                          /* offset*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor2*/    { -1, 0, 0, 0, 0, 0 },
                          /* srctex*/     { -1, 0, 0, 0, 0, 0 },
                          /* srctexscale*/{ -1, 0, 0, 0, 0, 0 },
                          /* tex*/        { -1, 0, 0, 0, 0, 0 },
                          /* texgen*/     { -1, 0, 0, 0, 0, 0 },
                          /* texscale*/   { -1, 0, 0, 0, 0, 0 },
                          /* vfmuniforms*/{ -1, 0, 0, 0, 0, 0 },
                          /* vfuniforms*/ { -1, 0, 0, 0, 0, 0 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_Count, -1, 0},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_Count, -1, 0},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    /* NumAttribs */    3,
    /* Attributes */    {
                          { "acolor",     4 | VET_Color},
                          { "afactor",    4 | VET_Color | (1 << VET_Index_Shift)},
                          { "pos",        4 | VET_Pos},
                          },
    
    };

static VertexShaderDesc ShaderDesc_VS_VPosition3dVertexCxform = {
    /* Type */          VertexShaderDesc::VS_VPosition3dVertexCxform,
    /* Flags */         0,
    /* pSource */       pSource_VPosition3dVertexCxform,
    /* Uniforms */      { 
                          /* cxadd*/      {0, 0, 4, 4, 4, 0 },
                          /* cxmul*/      {1, 4, 4, 4, 4, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      { -1, 0, 0, 0, 0, 0 },
                          /* mvp*/        {2, 8, 16, 16, 16, 0 },
                          /* offset*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor2*/    { -1, 0, 0, 0, 0, 0 },
                          /* srctex*/     { -1, 0, 0, 0, 0, 0 },
                          /* srctexscale*/{ -1, 0, 0, 0, 0, 0 },
                          /* tex*/        { -1, 0, 0, 0, 0, 0 },
                          /* texgen*/     { -1, 0, 0, 0, 0, 0 },
                          /* texscale*/   { -1, 0, 0, 0, 0, 0 },
                          /* vfmuniforms*/{ -1, 0, 0, 0, 0, 0 },
                          /* vfuniforms*/ { -1, 0, 0, 0, 0, 0 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_Count, -1, 0},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_Count, -1, 0},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    /* NumAttribs */    2,
    /* Attributes */    {
                          { "acolor",     4 | VET_Color},
                          { "pos",        4 | VET_Pos},
                          },
    
    };

static VertexShaderDesc ShaderDesc_VS_VPosition3dVertexCxformEAlpha = {
    /* Type */          VertexShaderDesc::VS_VPosition3dVertexCxformEAlpha,
    /* Flags */         0,
    /* pSource */       pSource_VPosition3dVertexCxformEAlpha,
    /* Uniforms */      { 
                          /* cxadd*/      {0, 0, 4, 4, 4, 0 },
                          /* cxmul*/      {1, 4, 4, 4, 4, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      { -1, 0, 0, 0, 0, 0 },
                          /* mvp*/        {2, 8, 16, 16, 16, 0 },
                          /* offset*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor2*/    { -1, 0, 0, 0, 0, 0 },
                          /* srctex*/     { -1, 0, 0, 0, 0, 0 },
                          /* srctexscale*/{ -1, 0, 0, 0, 0, 0 },
                          /* tex*/        { -1, 0, 0, 0, 0, 0 },
                          /* texgen*/     { -1, 0, 0, 0, 0, 0 },
                          /* texscale*/   { -1, 0, 0, 0, 0, 0 },
                          /* vfmuniforms*/{ -1, 0, 0, 0, 0, 0 },
                          /* vfuniforms*/ { -1, 0, 0, 0, 0, 0 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_Count, -1, 0},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_Count, -1, 0},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    /* NumAttribs */    3,
    /* Attributes */    {
                          { "acolor",     4 | VET_Color},
                          { "afactor",    4 | VET_Color | (1 << VET_Index_Shift)},
                          { "pos",        4 | VET_Pos},
                          },
    
    };

static VertexShaderDesc ShaderDesc_VS_VPosition3dTexTGTexTG = {
    /* Type */          VertexShaderDesc::VS_VPosition3dTexTGTexTG,
    /* Flags */         0,
    /* pSource */       pSource_VPosition3dTexTGTexTG,
    /* Uniforms */      { 
                          /* cxadd*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      { -1, 0, 0, 0, 0, 0 },
                          /* mvp*/        {0, 0, 16, 16, 16, 0 },
                          /* offset*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor2*/    { -1, 0, 0, 0, 0, 0 },
                          /* srctex*/     { -1, 0, 0, 0, 0, 0 },
                          /* srctexscale*/{ -1, 0, 0, 0, 0, 0 },
                          /* tex*/        { -1, 0, 0, 0, 0, 0 },
                          /* texgen*/     {4, 16, 4, 16, 4, 0 },
                          /* texscale*/   { -1, 0, 0, 0, 0, 0 },
                          /* vfmuniforms*/{ -1, 0, 0, 0, 0, 0 },
                          /* vfuniforms*/ { -1, 0, 0, 0, 0, 0 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_Count, -1, 0},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_Count, -1, 0},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    /* NumAttribs */    2,
    /* Attributes */    {
                          { "afactor",    4 | VET_Color | (1 << VET_Index_Shift)},
                          { "pos",        4 | VET_Pos},
                          },
    
    };

static VertexShaderDesc ShaderDesc_VS_VPosition3dTexTGTexTGCxform = {
    /* Type */          VertexShaderDesc::VS_VPosition3dTexTGTexTGCxform,
    /* Flags */         0,
    /* pSource */       pSource_VPosition3dTexTGTexTGCxform,
    /* Uniforms */      { 
                          /* cxadd*/      {0, 0, 4, 4, 4, 0 },
                          /* cxmul*/      {1, 4, 4, 4, 4, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      { -1, 0, 0, 0, 0, 0 },
                          /* mvp*/        {2, 8, 16, 16, 16, 0 },
                          /* offset*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor2*/    { -1, 0, 0, 0, 0, 0 },
                          /* srctex*/     { -1, 0, 0, 0, 0, 0 },
                          /* srctexscale*/{ -1, 0, 0, 0, 0, 0 },
                          /* tex*/        { -1, 0, 0, 0, 0, 0 },
                          /* texgen*/     {6, 24, 4, 16, 4, 0 },
                          /* texscale*/   { -1, 0, 0, 0, 0, 0 },
                          /* vfmuniforms*/{ -1, 0, 0, 0, 0, 0 },
                          /* vfuniforms*/ { -1, 0, 0, 0, 0, 0 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_Count, -1, 0},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_Count, -1, 0},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    /* NumAttribs */    2,
    /* Attributes */    {
                          { "afactor",    4 | VET_Color | (1 << VET_Index_Shift)},
                          { "pos",        4 | VET_Pos},
                          },
    
    };

static VertexShaderDesc ShaderDesc_VS_VPosition3dTexTGVertex = {
    /* Type */          VertexShaderDesc::VS_VPosition3dTexTGVertex,
    /* Flags */         0,
    /* pSource */       pSource_VPosition3dTexTGVertex,
    /* Uniforms */      { 
                          /* cxadd*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      { -1, 0, 0, 0, 0, 0 },
                          /* mvp*/        {0, 0, 16, 16, 16, 0 },
                          /* offset*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor2*/    { -1, 0, 0, 0, 0, 0 },
                          /* srctex*/     { -1, 0, 0, 0, 0, 0 },
                          /* srctexscale*/{ -1, 0, 0, 0, 0, 0 },
                          /* tex*/        { -1, 0, 0, 0, 0, 0 },
                          /* texgen*/     {4, 16, 4, 8, 4, 0 },
                          /* texscale*/   { -1, 0, 0, 0, 0, 0 },
                          /* vfmuniforms*/{ -1, 0, 0, 0, 0, 0 },
                          /* vfuniforms*/ { -1, 0, 0, 0, 0, 0 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_Count, -1, 0},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_Count, -1, 0},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    /* NumAttribs */    3,
    /* Attributes */    {
                          { "acolor",     4 | VET_Color},
                          { "afactor",    4 | VET_Color | (1 << VET_Index_Shift)},
                          { "pos",        4 | VET_Pos},
                          },
    
    };

static VertexShaderDesc ShaderDesc_VS_VPosition3dTexTGVertexCxform = {
    /* Type */          VertexShaderDesc::VS_VPosition3dTexTGVertexCxform,
    /* Flags */         0,
    /* pSource */       pSource_VPosition3dTexTGVertexCxform,
    /* Uniforms */      { 
                          /* cxadd*/      {0, 0, 4, 4, 4, 0 },
                          /* cxmul*/      {1, 4, 4, 4, 4, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      { -1, 0, 0, 0, 0, 0 },
                          /* mvp*/        {2, 8, 16, 16, 16, 0 },
                          /* offset*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor2*/    { -1, 0, 0, 0, 0, 0 },
                          /* srctex*/     { -1, 0, 0, 0, 0, 0 },
                          /* srctexscale*/{ -1, 0, 0, 0, 0, 0 },
                          /* tex*/        { -1, 0, 0, 0, 0, 0 },
                          /* texgen*/     {6, 24, 4, 8, 4, 0 },
                          /* texscale*/   { -1, 0, 0, 0, 0, 0 },
                          /* vfmuniforms*/{ -1, 0, 0, 0, 0, 0 },
                          /* vfuniforms*/ { -1, 0, 0, 0, 0, 0 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_Count, -1, 0},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_Count, -1, 0},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    /* NumAttribs */    3,
    /* Attributes */    {
                          { "acolor",     4 | VET_Color},
                          { "afactor",    4 | VET_Color | (1 << VET_Index_Shift)},
                          { "pos",        4 | VET_Pos},
                          },
    
    };

static VertexShaderDesc ShaderDesc_VS_VBatchTexTG = {
    /* Type */          VertexShaderDesc::VS_VBatchTexTG,
    /* Flags */         Shader_Batch,
    /* pSource */       pSource_VBatchTexTG,
    /* Uniforms */      { 
                          /* cxadd*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      { -1, 0, 0, 0, 0, 0 },
                          /* mvp*/        { -1, 0, 0, 0, 0, 0 },
                          /* offset*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor2*/    { -1, 0, 0, 0, 0, 0 },
                          /* srctex*/     { -1, 0, 0, 0, 0, 0 },
                          /* srctexscale*/{ -1, 0, 0, 0, 0, 0 },
                          /* tex*/        { -1, 0, 0, 0, 0, 0 },
                          /* texgen*/     { -1, 0, 0, 0, 0, 0 },
                          /* texscale*/   { -1, 0, 0, 0, 0, 0 },
                          /* vfmuniforms*/{ -1, 0, 0, 0, 0, 0 },
                          /* vfuniforms*/ {0, 0, 4, 384, 4, 4 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_vfuniforms, 0, 2},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_vfuniforms, 2, 2},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    /* NumAttribs */    2,
    /* Attributes */    {
                          { "pos",        4 | VET_Pos},
                          { "vbatch",     1 | VET_Instance8},
                          },
    
    };

static VertexShaderDesc ShaderDesc_VS_VBatchTexTGEAlpha = {
    /* Type */          VertexShaderDesc::VS_VBatchTexTGEAlpha,
    /* Flags */         Shader_Batch,
    /* pSource */       pSource_VBatchTexTGEAlpha,
    /* Uniforms */      { 
                          /* cxadd*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      { -1, 0, 0, 0, 0, 0 },
                          /* mvp*/        { -1, 0, 0, 0, 0, 0 },
                          /* offset*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor2*/    { -1, 0, 0, 0, 0, 0 },
                          /* srctex*/     { -1, 0, 0, 0, 0, 0 },
                          /* srctexscale*/{ -1, 0, 0, 0, 0, 0 },
                          /* tex*/        { -1, 0, 0, 0, 0, 0 },
                          /* texgen*/     { -1, 0, 0, 0, 0, 0 },
                          /* texscale*/   { -1, 0, 0, 0, 0, 0 },
                          /* vfmuniforms*/{ -1, 0, 0, 0, 0, 0 },
                          /* vfuniforms*/ {0, 0, 4, 384, 4, 4 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_vfuniforms, 0, 2},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_vfuniforms, 2, 2},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    /* NumAttribs */    2,
    /* Attributes */    {
                          { "afactor",    4 | VET_Color | (1 << VET_Index_Shift)},
                          { "pos",        4 | VET_Pos},
                          },
    
    };

static VertexShaderDesc ShaderDesc_VS_VBatchTexTGCxform = {
    /* Type */          VertexShaderDesc::VS_VBatchTexTGCxform,
    /* Flags */         Shader_Batch,
    /* pSource */       pSource_VBatchTexTGCxform,
    /* Uniforms */      { 
                          /* cxadd*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      { -1, 0, 0, 0, 0, 0 },
                          /* mvp*/        { -1, 0, 0, 0, 0, 0 },
                          /* offset*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor2*/    { -1, 0, 0, 0, 0, 0 },
                          /* srctex*/     { -1, 0, 0, 0, 0, 0 },
                          /* srctexscale*/{ -1, 0, 0, 0, 0, 0 },
                          /* tex*/        { -1, 0, 0, 0, 0, 0 },
                          /* texgen*/     { -1, 0, 0, 0, 0, 0 },
                          /* texscale*/   { -1, 0, 0, 0, 0, 0 },
                          /* vfmuniforms*/{ -1, 0, 0, 0, 0, 0 },
                          /* vfuniforms*/ {0, 0, 4, 576, 4, 6 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_vfuniforms, 0, 1},
                          /* cxmul*/       {Uniform::SU_vfuniforms, 1, 1},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_vfuniforms, 2, 2},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_vfuniforms, 4, 2},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    /* NumAttribs */    2,
    /* Attributes */    {
                          { "pos",        4 | VET_Pos},
                          { "vbatch",     1 | VET_Instance8},
                          },
    
    };

static VertexShaderDesc ShaderDesc_VS_VBatchTexTGCxformEAlpha = {
    /* Type */          VertexShaderDesc::VS_VBatchTexTGCxformEAlpha,
    /* Flags */         Shader_Batch,
    /* pSource */       pSource_VBatchTexTGCxformEAlpha,
    /* Uniforms */      { 
                          /* cxadd*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      { -1, 0, 0, 0, 0, 0 },
                          /* mvp*/        { -1, 0, 0, 0, 0, 0 },
                          /* offset*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor2*/    { -1, 0, 0, 0, 0, 0 },
                          /* srctex*/     { -1, 0, 0, 0, 0, 0 },
                          /* srctexscale*/{ -1, 0, 0, 0, 0, 0 },
                          /* tex*/        { -1, 0, 0, 0, 0, 0 },
                          /* texgen*/     { -1, 0, 0, 0, 0, 0 },
                          /* texscale*/   { -1, 0, 0, 0, 0, 0 },
                          /* vfmuniforms*/{ -1, 0, 0, 0, 0, 0 },
                          /* vfuniforms*/ {0, 0, 4, 576, 4, 6 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_vfuniforms, 0, 1},
                          /* cxmul*/       {Uniform::SU_vfuniforms, 1, 1},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_vfuniforms, 2, 2},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_vfuniforms, 4, 2},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    /* NumAttribs */    2,
    /* Attributes */    {
                          { "afactor",    4 | VET_Color | (1 << VET_Index_Shift)},
                          { "pos",        4 | VET_Pos},
                          },
    
    };

static VertexShaderDesc ShaderDesc_VS_VBatchVertex = {
    /* Type */          VertexShaderDesc::VS_VBatchVertex,
    /* Flags */         Shader_Batch,
    /* pSource */       pSource_VBatchVertex,
    /* Uniforms */      { 
                          /* cxadd*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      { -1, 0, 0, 0, 0, 0 },
                          /* mvp*/        { -1, 0, 0, 0, 0, 0 },
                          /* offset*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor2*/    { -1, 0, 0, 0, 0, 0 },
                          /* srctex*/     { -1, 0, 0, 0, 0, 0 },
                          /* srctexscale*/{ -1, 0, 0, 0, 0, 0 },
                          /* tex*/        { -1, 0, 0, 0, 0, 0 },
                          /* texgen*/     { -1, 0, 0, 0, 0, 0 },
                          /* texscale*/   { -1, 0, 0, 0, 0, 0 },
                          /* vfmuniforms*/{ -1, 0, 0, 0, 0, 0 },
                          /* vfuniforms*/ {0, 0, 4, 192, 4, 2 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_vfuniforms, 0, 2},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_Count, -1, 0},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    /* NumAttribs */    3,
    /* Attributes */    {
                          { "acolor",     4 | VET_Color},
                          { "pos",        4 | VET_Pos},
                          { "vbatch",     1 | VET_Instance8},
                          },
    
    };

static VertexShaderDesc ShaderDesc_VS_VBatchVertexEAlpha = {
    /* Type */          VertexShaderDesc::VS_VBatchVertexEAlpha,
    /* Flags */         Shader_Batch,
    /* pSource */       pSource_VBatchVertexEAlpha,
    /* Uniforms */      { 
                          /* cxadd*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      { -1, 0, 0, 0, 0, 0 },
                          /* mvp*/        { -1, 0, 0, 0, 0, 0 },
                          /* offset*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor2*/    { -1, 0, 0, 0, 0, 0 },
                          /* srctex*/     { -1, 0, 0, 0, 0, 0 },
                          /* srctexscale*/{ -1, 0, 0, 0, 0, 0 },
                          /* tex*/        { -1, 0, 0, 0, 0, 0 },
                          /* texgen*/     { -1, 0, 0, 0, 0, 0 },
                          /* texscale*/   { -1, 0, 0, 0, 0, 0 },
                          /* vfmuniforms*/{ -1, 0, 0, 0, 0, 0 },
                          /* vfuniforms*/ {0, 0, 4, 192, 4, 2 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_vfuniforms, 0, 2},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_Count, -1, 0},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    /* NumAttribs */    3,
    /* Attributes */    {
                          { "acolor",     4 | VET_Color},
                          { "afactor",    4 | VET_Color | (1 << VET_Index_Shift)},
                          { "pos",        4 | VET_Pos},
                          },
    
    };

static VertexShaderDesc ShaderDesc_VS_VBatchVertexCxform = {
    /* Type */          VertexShaderDesc::VS_VBatchVertexCxform,
    /* Flags */         Shader_Batch,
    /* pSource */       pSource_VBatchVertexCxform,
    /* Uniforms */      { 
                          /* cxadd*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      { -1, 0, 0, 0, 0, 0 },
                          /* mvp*/        { -1, 0, 0, 0, 0, 0 },
                          /* offset*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor2*/    { -1, 0, 0, 0, 0, 0 },
                          /* srctex*/     { -1, 0, 0, 0, 0, 0 },
                          /* srctexscale*/{ -1, 0, 0, 0, 0, 0 },
                          /* tex*/        { -1, 0, 0, 0, 0, 0 },
                          /* texgen*/     { -1, 0, 0, 0, 0, 0 },
                          /* texscale*/   { -1, 0, 0, 0, 0, 0 },
                          /* vfmuniforms*/{ -1, 0, 0, 0, 0, 0 },
                          /* vfuniforms*/ {0, 0, 4, 384, 4, 4 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_vfuniforms, 0, 1},
                          /* cxmul*/       {Uniform::SU_vfuniforms, 1, 1},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_vfuniforms, 2, 2},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_Count, -1, 0},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    /* NumAttribs */    3,
    /* Attributes */    {
                          { "acolor",     4 | VET_Color},
                          { "pos",        4 | VET_Pos},
                          { "vbatch",     1 | VET_Instance8},
                          },
    
    };

static VertexShaderDesc ShaderDesc_VS_VBatchVertexCxformEAlpha = {
    /* Type */          VertexShaderDesc::VS_VBatchVertexCxformEAlpha,
    /* Flags */         Shader_Batch,
    /* pSource */       pSource_VBatchVertexCxformEAlpha,
    /* Uniforms */      { 
                          /* cxadd*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      { -1, 0, 0, 0, 0, 0 },
                          /* mvp*/        { -1, 0, 0, 0, 0, 0 },
                          /* offset*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor2*/    { -1, 0, 0, 0, 0, 0 },
                          /* srctex*/     { -1, 0, 0, 0, 0, 0 },
                          /* srctexscale*/{ -1, 0, 0, 0, 0, 0 },
                          /* tex*/        { -1, 0, 0, 0, 0, 0 },
                          /* texgen*/     { -1, 0, 0, 0, 0, 0 },
                          /* texscale*/   { -1, 0, 0, 0, 0, 0 },
                          /* vfmuniforms*/{ -1, 0, 0, 0, 0, 0 },
                          /* vfuniforms*/ {0, 0, 4, 384, 4, 4 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_vfuniforms, 0, 1},
                          /* cxmul*/       {Uniform::SU_vfuniforms, 1, 1},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_vfuniforms, 2, 2},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_Count, -1, 0},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    /* NumAttribs */    3,
    /* Attributes */    {
                          { "acolor",     4 | VET_Color},
                          { "afactor",    4 | VET_Color | (1 << VET_Index_Shift)},
                          { "pos",        4 | VET_Pos},
                          },
    
    };

static VertexShaderDesc ShaderDesc_VS_VBatchTexTGTexTG = {
    /* Type */          VertexShaderDesc::VS_VBatchTexTGTexTG,
    /* Flags */         Shader_Batch,
    /* pSource */       pSource_VBatchTexTGTexTG,
    /* Uniforms */      { 
                          /* cxadd*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      { -1, 0, 0, 0, 0, 0 },
                          /* mvp*/        { -1, 0, 0, 0, 0, 0 },
                          /* offset*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor2*/    { -1, 0, 0, 0, 0, 0 },
                          /* srctex*/     { -1, 0, 0, 0, 0, 0 },
                          /* srctexscale*/{ -1, 0, 0, 0, 0, 0 },
                          /* tex*/        { -1, 0, 0, 0, 0, 0 },
                          /* texgen*/     { -1, 0, 0, 0, 0, 0 },
                          /* texscale*/   { -1, 0, 0, 0, 0, 0 },
                          /* vfmuniforms*/{ -1, 0, 0, 0, 0, 0 },
                          /* vfuniforms*/ {0, 0, 4, 576, 4, 6 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_vfuniforms, 0, 2},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_vfuniforms, 2, 4},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    /* NumAttribs */    2,
    /* Attributes */    {
                          { "afactor",    4 | VET_Color | (1 << VET_Index_Shift)},
                          { "pos",        4 | VET_Pos},
                          },
    
    };

static VertexShaderDesc ShaderDesc_VS_VBatchTexTGTexTGCxform = {
    /* Type */          VertexShaderDesc::VS_VBatchTexTGTexTGCxform,
    /* Flags */         Shader_Batch,
    /* pSource */       pSource_VBatchTexTGTexTGCxform,
    /* Uniforms */      { 
                          /* cxadd*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      { -1, 0, 0, 0, 0, 0 },
                          /* mvp*/        { -1, 0, 0, 0, 0, 0 },
                          /* offset*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor2*/    { -1, 0, 0, 0, 0, 0 },
                          /* srctex*/     { -1, 0, 0, 0, 0, 0 },
                          /* srctexscale*/{ -1, 0, 0, 0, 0, 0 },
                          /* tex*/        { -1, 0, 0, 0, 0, 0 },
                          /* texgen*/     { -1, 0, 0, 0, 0, 0 },
                          /* texscale*/   { -1, 0, 0, 0, 0, 0 },
                          /* vfmuniforms*/{ -1, 0, 0, 0, 0, 0 },
                          /* vfuniforms*/ {0, 0, 4, 768, 4, 8 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_vfuniforms, 0, 1},
                          /* cxmul*/       {Uniform::SU_vfuniforms, 1, 1},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_vfuniforms, 2, 2},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_vfuniforms, 4, 4},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    /* NumAttribs */    2,
    /* Attributes */    {
                          { "afactor",    4 | VET_Color | (1 << VET_Index_Shift)},
                          { "pos",        4 | VET_Pos},
                          },
    
    };

static VertexShaderDesc ShaderDesc_VS_VBatchTexTGVertex = {
    /* Type */          VertexShaderDesc::VS_VBatchTexTGVertex,
    /* Flags */         Shader_Batch,
    /* pSource */       pSource_VBatchTexTGVertex,
    /* Uniforms */      { 
                          /* cxadd*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      { -1, 0, 0, 0, 0, 0 },
                          /* mvp*/        { -1, 0, 0, 0, 0, 0 },
                          /* offset*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor2*/    { -1, 0, 0, 0, 0, 0 },
                          /* srctex*/     { -1, 0, 0, 0, 0, 0 },
                          /* srctexscale*/{ -1, 0, 0, 0, 0, 0 },
                          /* tex*/        { -1, 0, 0, 0, 0, 0 },
                          /* texgen*/     { -1, 0, 0, 0, 0, 0 },
                          /* texscale*/   { -1, 0, 0, 0, 0, 0 },
                          /* vfmuniforms*/{ -1, 0, 0, 0, 0, 0 },
                          /* vfuniforms*/ {0, 0, 4, 384, 4, 4 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_vfuniforms, 0, 2},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_vfuniforms, 2, 2},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    /* NumAttribs */    3,
    /* Attributes */    {
                          { "acolor",     4 | VET_Color},
                          { "afactor",    4 | VET_Color | (1 << VET_Index_Shift)},
                          { "pos",        4 | VET_Pos},
                          },
    
    };

static VertexShaderDesc ShaderDesc_VS_VBatchTexTGVertexCxform = {
    /* Type */          VertexShaderDesc::VS_VBatchTexTGVertexCxform,
    /* Flags */         Shader_Batch,
    /* pSource */       pSource_VBatchTexTGVertexCxform,
    /* Uniforms */      { 
                          /* cxadd*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      { -1, 0, 0, 0, 0, 0 },
                          /* mvp*/        { -1, 0, 0, 0, 0, 0 },
                          /* offset*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor2*/    { -1, 0, 0, 0, 0, 0 },
                          /* srctex*/     { -1, 0, 0, 0, 0, 0 },
                          /* srctexscale*/{ -1, 0, 0, 0, 0, 0 },
                          /* tex*/        { -1, 0, 0, 0, 0, 0 },
                          /* texgen*/     { -1, 0, 0, 0, 0, 0 },
                          /* texscale*/   { -1, 0, 0, 0, 0, 0 },
                          /* vfmuniforms*/{ -1, 0, 0, 0, 0, 0 },
                          /* vfuniforms*/ {0, 0, 4, 576, 4, 6 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_vfuniforms, 0, 1},
                          /* cxmul*/       {Uniform::SU_vfuniforms, 1, 1},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_vfuniforms, 2, 2},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_vfuniforms, 4, 2},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    /* NumAttribs */    3,
    /* Attributes */    {
                          { "acolor",     4 | VET_Color},
                          { "afactor",    4 | VET_Color | (1 << VET_Index_Shift)},
                          { "pos",        4 | VET_Pos},
                          },
    
    };

static VertexShaderDesc ShaderDesc_VS_VBatchPosition3dTexTG = {
    /* Type */          VertexShaderDesc::VS_VBatchPosition3dTexTG,
    /* Flags */         Shader_Batch,
    /* pSource */       pSource_VBatchPosition3dTexTG,
    /* Uniforms */      { 
                          /* cxadd*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      { -1, 0, 0, 0, 0, 0 },
                          /* mvp*/        { -1, 0, 0, 0, 0, 0 },
                          /* offset*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor2*/    { -1, 0, 0, 0, 0, 0 },
                          /* srctex*/     { -1, 0, 0, 0, 0, 0 },
                          /* srctexscale*/{ -1, 0, 0, 0, 0, 0 },
                          /* tex*/        { -1, 0, 0, 0, 0, 0 },
                          /* texgen*/     { -1, 0, 0, 0, 0, 0 },
                          /* texscale*/   { -1, 0, 0, 0, 0, 0 },
                          /* vfmuniforms*/{0, 0, 16, 384, 16, 1 },
                          /* vfuniforms*/ {96, 384, 4, 192, 4, 2 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_vfmuniforms, 0, 1},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_vfuniforms, 0, 2},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    /* NumAttribs */    2,
    /* Attributes */    {
                          { "pos",        4 | VET_Pos},
                          { "vbatch",     1 | VET_Instance8},
                          },
    
    };

static VertexShaderDesc ShaderDesc_VS_VBatchPosition3dTexTGEAlpha = {
    /* Type */          VertexShaderDesc::VS_VBatchPosition3dTexTGEAlpha,
    /* Flags */         Shader_Batch,
    /* pSource */       pSource_VBatchPosition3dTexTGEAlpha,
    /* Uniforms */      { 
                          /* cxadd*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      { -1, 0, 0, 0, 0, 0 },
                          /* mvp*/        { -1, 0, 0, 0, 0, 0 },
                          /* offset*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor2*/    { -1, 0, 0, 0, 0, 0 },
                          /* srctex*/     { -1, 0, 0, 0, 0, 0 },
                          /* srctexscale*/{ -1, 0, 0, 0, 0, 0 },
                          /* tex*/        { -1, 0, 0, 0, 0, 0 },
                          /* texgen*/     { -1, 0, 0, 0, 0, 0 },
                          /* texscale*/   { -1, 0, 0, 0, 0, 0 },
                          /* vfmuniforms*/{0, 0, 16, 384, 16, 1 },
                          /* vfuniforms*/ {96, 384, 4, 192, 4, 2 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_vfmuniforms, 0, 1},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_vfuniforms, 0, 2},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    /* NumAttribs */    2,
    /* Attributes */    {
                          { "afactor",    4 | VET_Color | (1 << VET_Index_Shift)},
                          { "pos",        4 | VET_Pos},
                          },
    
    };

static VertexShaderDesc ShaderDesc_VS_VBatchPosition3dTexTGCxform = {
    /* Type */          VertexShaderDesc::VS_VBatchPosition3dTexTGCxform,
    /* Flags */         Shader_Batch,
    /* pSource */       pSource_VBatchPosition3dTexTGCxform,
    /* Uniforms */      { 
                          /* cxadd*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      { -1, 0, 0, 0, 0, 0 },
                          /* mvp*/        { -1, 0, 0, 0, 0, 0 },
                          /* offset*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor2*/    { -1, 0, 0, 0, 0, 0 },
                          /* srctex*/     { -1, 0, 0, 0, 0, 0 },
                          /* srctexscale*/{ -1, 0, 0, 0, 0, 0 },
                          /* tex*/        { -1, 0, 0, 0, 0, 0 },
                          /* texgen*/     { -1, 0, 0, 0, 0, 0 },
                          /* texscale*/   { -1, 0, 0, 0, 0, 0 },
                          /* vfmuniforms*/{0, 0, 16, 384, 16, 1 },
                          /* vfuniforms*/ {96, 384, 4, 384, 4, 4 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_vfuniforms, 0, 1},
                          /* cxmul*/       {Uniform::SU_vfuniforms, 1, 1},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_vfmuniforms, 0, 1},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_vfuniforms, 2, 2},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    /* NumAttribs */    2,
    /* Attributes */    {
                          { "pos",        4 | VET_Pos},
                          { "vbatch",     1 | VET_Instance8},
                          },
    
    };

static VertexShaderDesc ShaderDesc_VS_VBatchPosition3dTexTGCxformEAlpha = {
    /* Type */          VertexShaderDesc::VS_VBatchPosition3dTexTGCxformEAlpha,
    /* Flags */         Shader_Batch,
    /* pSource */       pSource_VBatchPosition3dTexTGCxformEAlpha,
    /* Uniforms */      { 
                          /* cxadd*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      { -1, 0, 0, 0, 0, 0 },
                          /* mvp*/        { -1, 0, 0, 0, 0, 0 },
                          /* offset*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor2*/    { -1, 0, 0, 0, 0, 0 },
                          /* srctex*/     { -1, 0, 0, 0, 0, 0 },
                          /* srctexscale*/{ -1, 0, 0, 0, 0, 0 },
                          /* tex*/        { -1, 0, 0, 0, 0, 0 },
                          /* texgen*/     { -1, 0, 0, 0, 0, 0 },
                          /* texscale*/   { -1, 0, 0, 0, 0, 0 },
                          /* vfmuniforms*/{0, 0, 16, 384, 16, 1 },
                          /* vfuniforms*/ {96, 384, 4, 384, 4, 4 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_vfuniforms, 0, 1},
                          /* cxmul*/       {Uniform::SU_vfuniforms, 1, 1},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_vfmuniforms, 0, 1},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_vfuniforms, 2, 2},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    /* NumAttribs */    2,
    /* Attributes */    {
                          { "afactor",    4 | VET_Color | (1 << VET_Index_Shift)},
                          { "pos",        4 | VET_Pos},
                          },
    
    };

static VertexShaderDesc ShaderDesc_VS_VBatchPosition3dVertex = {
    /* Type */          VertexShaderDesc::VS_VBatchPosition3dVertex,
    /* Flags */         Shader_Batch,
    /* pSource */       pSource_VBatchPosition3dVertex,
    /* Uniforms */      { 
                          /* cxadd*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      { -1, 0, 0, 0, 0, 0 },
                          /* mvp*/        { -1, 0, 0, 0, 0, 0 },
                          /* offset*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor2*/    { -1, 0, 0, 0, 0, 0 },
                          /* srctex*/     { -1, 0, 0, 0, 0, 0 },
                          /* srctexscale*/{ -1, 0, 0, 0, 0, 0 },
                          /* tex*/        { -1, 0, 0, 0, 0, 0 },
                          /* texgen*/     { -1, 0, 0, 0, 0, 0 },
                          /* texscale*/   { -1, 0, 0, 0, 0, 0 },
                          /* vfmuniforms*/{0, 0, 16, 384, 16, 1 },
                          /* vfuniforms*/ { -1, 0, 0, 0, 0, 0 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_vfmuniforms, 0, 1},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_Count, -1, 0},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    /* NumAttribs */    3,
    /* Attributes */    {
                          { "acolor",     4 | VET_Color},
                          { "pos",        4 | VET_Pos},
                          { "vbatch",     1 | VET_Instance8},
                          },
    
    };

static VertexShaderDesc ShaderDesc_VS_VBatchPosition3dVertexEAlpha = {
    /* Type */          VertexShaderDesc::VS_VBatchPosition3dVertexEAlpha,
    /* Flags */         Shader_Batch,
    /* pSource */       pSource_VBatchPosition3dVertexEAlpha,
    /* Uniforms */      { 
                          /* cxadd*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      { -1, 0, 0, 0, 0, 0 },
                          /* mvp*/        { -1, 0, 0, 0, 0, 0 },
                          /* offset*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor2*/    { -1, 0, 0, 0, 0, 0 },
                          /* srctex*/     { -1, 0, 0, 0, 0, 0 },
                          /* srctexscale*/{ -1, 0, 0, 0, 0, 0 },
                          /* tex*/        { -1, 0, 0, 0, 0, 0 },
                          /* texgen*/     { -1, 0, 0, 0, 0, 0 },
                          /* texscale*/   { -1, 0, 0, 0, 0, 0 },
                          /* vfmuniforms*/{0, 0, 16, 384, 16, 1 },
                          /* vfuniforms*/ { -1, 0, 0, 0, 0, 0 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_vfmuniforms, 0, 1},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_Count, -1, 0},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    /* NumAttribs */    3,
    /* Attributes */    {
                          { "acolor",     4 | VET_Color},
                          { "afactor",    4 | VET_Color | (1 << VET_Index_Shift)},
                          { "pos",        4 | VET_Pos},
                          },
    
    };

static VertexShaderDesc ShaderDesc_VS_VBatchPosition3dVertexCxform = {
    /* Type */          VertexShaderDesc::VS_VBatchPosition3dVertexCxform,
    /* Flags */         Shader_Batch,
    /* pSource */       pSource_VBatchPosition3dVertexCxform,
    /* Uniforms */      { 
                          /* cxadd*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      { -1, 0, 0, 0, 0, 0 },
                          /* mvp*/        { -1, 0, 0, 0, 0, 0 },
                          /* offset*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor2*/    { -1, 0, 0, 0, 0, 0 },
                          /* srctex*/     { -1, 0, 0, 0, 0, 0 },
                          /* srctexscale*/{ -1, 0, 0, 0, 0, 0 },
                          /* tex*/        { -1, 0, 0, 0, 0, 0 },
                          /* texgen*/     { -1, 0, 0, 0, 0, 0 },
                          /* texscale*/   { -1, 0, 0, 0, 0, 0 },
                          /* vfmuniforms*/{0, 0, 16, 384, 16, 1 },
                          /* vfuniforms*/ {96, 384, 4, 192, 4, 2 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_vfuniforms, 0, 1},
                          /* cxmul*/       {Uniform::SU_vfuniforms, 1, 1},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_vfmuniforms, 0, 1},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_Count, -1, 0},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    /* NumAttribs */    3,
    /* Attributes */    {
                          { "acolor",     4 | VET_Color},
                          { "pos",        4 | VET_Pos},
                          { "vbatch",     1 | VET_Instance8},
                          },
    
    };

static VertexShaderDesc ShaderDesc_VS_VBatchPosition3dVertexCxformEAlpha = {
    /* Type */          VertexShaderDesc::VS_VBatchPosition3dVertexCxformEAlpha,
    /* Flags */         Shader_Batch,
    /* pSource */       pSource_VBatchPosition3dVertexCxformEAlpha,
    /* Uniforms */      { 
                          /* cxadd*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      { -1, 0, 0, 0, 0, 0 },
                          /* mvp*/        { -1, 0, 0, 0, 0, 0 },
                          /* offset*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor2*/    { -1, 0, 0, 0, 0, 0 },
                          /* srctex*/     { -1, 0, 0, 0, 0, 0 },
                          /* srctexscale*/{ -1, 0, 0, 0, 0, 0 },
                          /* tex*/        { -1, 0, 0, 0, 0, 0 },
                          /* texgen*/     { -1, 0, 0, 0, 0, 0 },
                          /* texscale*/   { -1, 0, 0, 0, 0, 0 },
                          /* vfmuniforms*/{0, 0, 16, 384, 16, 1 },
                          /* vfuniforms*/ {96, 384, 4, 192, 4, 2 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_vfuniforms, 0, 1},
                          /* cxmul*/       {Uniform::SU_vfuniforms, 1, 1},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_vfmuniforms, 0, 1},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_Count, -1, 0},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    /* NumAttribs */    3,
    /* Attributes */    {
                          { "acolor",     4 | VET_Color},
                          { "afactor",    4 | VET_Color | (1 << VET_Index_Shift)},
                          { "pos",        4 | VET_Pos},
                          },
    
    };

static VertexShaderDesc ShaderDesc_VS_VBatchPosition3dTexTGTexTG = {
    /* Type */          VertexShaderDesc::VS_VBatchPosition3dTexTGTexTG,
    /* Flags */         Shader_Batch,
    /* pSource */       pSource_VBatchPosition3dTexTGTexTG,
    /* Uniforms */      { 
                          /* cxadd*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      { -1, 0, 0, 0, 0, 0 },
                          /* mvp*/        { -1, 0, 0, 0, 0, 0 },
                          /* offset*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor2*/    { -1, 0, 0, 0, 0, 0 },
                          /* srctex*/     { -1, 0, 0, 0, 0, 0 },
                          /* srctexscale*/{ -1, 0, 0, 0, 0, 0 },
                          /* tex*/        { -1, 0, 0, 0, 0, 0 },
                          /* texgen*/     { -1, 0, 0, 0, 0, 0 },
                          /* texscale*/   { -1, 0, 0, 0, 0, 0 },
                          /* vfmuniforms*/{0, 0, 16, 384, 16, 1 },
                          /* vfuniforms*/ {96, 384, 4, 384, 4, 4 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_vfmuniforms, 0, 1},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_vfuniforms, 0, 4},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    /* NumAttribs */    2,
    /* Attributes */    {
                          { "afactor",    4 | VET_Color | (1 << VET_Index_Shift)},
                          { "pos",        4 | VET_Pos},
                          },
    
    };

static VertexShaderDesc ShaderDesc_VS_VBatchPosition3dTexTGTexTGCxform = {
    /* Type */          VertexShaderDesc::VS_VBatchPosition3dTexTGTexTGCxform,
    /* Flags */         Shader_Batch,
    /* pSource */       pSource_VBatchPosition3dTexTGTexTGCxform,
    /* Uniforms */      { 
                          /* cxadd*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      { -1, 0, 0, 0, 0, 0 },
                          /* mvp*/        { -1, 0, 0, 0, 0, 0 },
                          /* offset*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor2*/    { -1, 0, 0, 0, 0, 0 },
                          /* srctex*/     { -1, 0, 0, 0, 0, 0 },
                          /* srctexscale*/{ -1, 0, 0, 0, 0, 0 },
                          /* tex*/        { -1, 0, 0, 0, 0, 0 },
                          /* texgen*/     { -1, 0, 0, 0, 0, 0 },
                          /* texscale*/   { -1, 0, 0, 0, 0, 0 },
                          /* vfmuniforms*/{0, 0, 16, 384, 16, 1 },
                          /* vfuniforms*/ {96, 384, 4, 576, 4, 6 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_vfuniforms, 0, 1},
                          /* cxmul*/       {Uniform::SU_vfuniforms, 1, 1},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_vfmuniforms, 0, 1},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_vfuniforms, 2, 4},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    /* NumAttribs */    2,
    /* Attributes */    {
                          { "afactor",    4 | VET_Color | (1 << VET_Index_Shift)},
                          { "pos",        4 | VET_Pos},
                          },
    
    };

static VertexShaderDesc ShaderDesc_VS_VBatchPosition3dTexTGVertex = {
    /* Type */          VertexShaderDesc::VS_VBatchPosition3dTexTGVertex,
    /* Flags */         Shader_Batch,
    /* pSource */       pSource_VBatchPosition3dTexTGVertex,
    /* Uniforms */      { 
                          /* cxadd*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      { -1, 0, 0, 0, 0, 0 },
                          /* mvp*/        { -1, 0, 0, 0, 0, 0 },
                          /* offset*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor2*/    { -1, 0, 0, 0, 0, 0 },
                          /* srctex*/     { -1, 0, 0, 0, 0, 0 },
                          /* srctexscale*/{ -1, 0, 0, 0, 0, 0 },
                          /* tex*/        { -1, 0, 0, 0, 0, 0 },
                          /* texgen*/     { -1, 0, 0, 0, 0, 0 },
                          /* texscale*/   { -1, 0, 0, 0, 0, 0 },
                          /* vfmuniforms*/{0, 0, 16, 384, 16, 1 },
                          /* vfuniforms*/ {96, 384, 4, 192, 4, 2 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_vfmuniforms, 0, 1},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_vfuniforms, 0, 2},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    /* NumAttribs */    3,
    /* Attributes */    {
                          { "acolor",     4 | VET_Color},
                          { "afactor",    4 | VET_Color | (1 << VET_Index_Shift)},
                          { "pos",        4 | VET_Pos},
                          },
    
    };

static VertexShaderDesc ShaderDesc_VS_VBatchPosition3dTexTGVertexCxform = {
    /* Type */          VertexShaderDesc::VS_VBatchPosition3dTexTGVertexCxform,
    /* Flags */         Shader_Batch,
    /* pSource */       pSource_VBatchPosition3dTexTGVertexCxform,
    /* Uniforms */      { 
                          /* cxadd*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      { -1, 0, 0, 0, 0, 0 },
                          /* mvp*/        { -1, 0, 0, 0, 0, 0 },
                          /* offset*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor2*/    { -1, 0, 0, 0, 0, 0 },
                          /* srctex*/     { -1, 0, 0, 0, 0, 0 },
                          /* srctexscale*/{ -1, 0, 0, 0, 0, 0 },
                          /* tex*/        { -1, 0, 0, 0, 0, 0 },
                          /* texgen*/     { -1, 0, 0, 0, 0, 0 },
                          /* texscale*/   { -1, 0, 0, 0, 0, 0 },
                          /* vfmuniforms*/{0, 0, 16, 384, 16, 1 },
                          /* vfuniforms*/ {96, 384, 4, 384, 4, 4 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_vfuniforms, 0, 1},
                          /* cxmul*/       {Uniform::SU_vfuniforms, 1, 1},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_vfmuniforms, 0, 1},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_vfuniforms, 2, 2},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    /* NumAttribs */    3,
    /* Attributes */    {
                          { "acolor",     4 | VET_Color},
                          { "afactor",    4 | VET_Color | (1 << VET_Index_Shift)},
                          { "pos",        4 | VET_Pos},
                          },
    
    };

static VertexShaderDesc ShaderDesc_VS_VInstancedTexTG = {
    /* Type */          VertexShaderDesc::VS_VInstancedTexTG,
    /* Flags */         Shader_Instanced,
    /* pSource */       pSource_VInstancedTexTG,
    /* Uniforms */      { 
                          /* cxadd*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      { -1, 0, 0, 0, 0, 0 },
                          /* mvp*/        { -1, 0, 0, 0, 0, 0 },
                          /* offset*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor2*/    { -1, 0, 0, 0, 0, 0 },
                          /* srctex*/     { -1, 0, 0, 0, 0, 0 },
                          /* srctexscale*/{ -1, 0, 0, 0, 0, 0 },
                          /* tex*/        { -1, 0, 0, 0, 0, 0 },
                          /* texgen*/     { -1, 0, 0, 0, 0, 0 },
                          /* texscale*/   { -1, 0, 0, 0, 0, 0 },
                          /* vfmuniforms*/{ -1, 0, 0, 0, 0, 0 },
                          /* vfuniforms*/ {0, 0, 4, 384, 4, 4 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_vfuniforms, 0, 2},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_vfuniforms, 2, 2},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    /* NumAttribs */    1,
    /* Attributes */    {
                          { "pos",        4 | VET_Pos},
                          },
    
    };

static VertexShaderDesc ShaderDesc_VS_VInstancedTexTGEAlpha = {
    /* Type */          VertexShaderDesc::VS_VInstancedTexTGEAlpha,
    /* Flags */         Shader_Instanced,
    /* pSource */       pSource_VInstancedTexTGEAlpha,
    /* Uniforms */      { 
                          /* cxadd*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      { -1, 0, 0, 0, 0, 0 },
                          /* mvp*/        { -1, 0, 0, 0, 0, 0 },
                          /* offset*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor2*/    { -1, 0, 0, 0, 0, 0 },
                          /* srctex*/     { -1, 0, 0, 0, 0, 0 },
                          /* srctexscale*/{ -1, 0, 0, 0, 0, 0 },
                          /* tex*/        { -1, 0, 0, 0, 0, 0 },
                          /* texgen*/     { -1, 0, 0, 0, 0, 0 },
                          /* texscale*/   { -1, 0, 0, 0, 0, 0 },
                          /* vfmuniforms*/{ -1, 0, 0, 0, 0, 0 },
                          /* vfuniforms*/ {0, 0, 4, 384, 4, 4 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_vfuniforms, 0, 2},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_vfuniforms, 2, 2},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    /* NumAttribs */    2,
    /* Attributes */    {
                          { "afactor",    4 | VET_Color | (1 << VET_Index_Shift)},
                          { "pos",        4 | VET_Pos},
                          },
    
    };

static VertexShaderDesc ShaderDesc_VS_VInstancedTexTGCxform = {
    /* Type */          VertexShaderDesc::VS_VInstancedTexTGCxform,
    /* Flags */         Shader_Instanced,
    /* pSource */       pSource_VInstancedTexTGCxform,
    /* Uniforms */      { 
                          /* cxadd*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      { -1, 0, 0, 0, 0, 0 },
                          /* mvp*/        { -1, 0, 0, 0, 0, 0 },
                          /* offset*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor2*/    { -1, 0, 0, 0, 0, 0 },
                          /* srctex*/     { -1, 0, 0, 0, 0, 0 },
                          /* srctexscale*/{ -1, 0, 0, 0, 0, 0 },
                          /* tex*/        { -1, 0, 0, 0, 0, 0 },
                          /* texgen*/     { -1, 0, 0, 0, 0, 0 },
                          /* texscale*/   { -1, 0, 0, 0, 0, 0 },
                          /* vfmuniforms*/{ -1, 0, 0, 0, 0, 0 },
                          /* vfuniforms*/ {0, 0, 4, 576, 4, 6 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_vfuniforms, 0, 1},
                          /* cxmul*/       {Uniform::SU_vfuniforms, 1, 1},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_vfuniforms, 2, 2},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_vfuniforms, 4, 2},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    /* NumAttribs */    1,
    /* Attributes */    {
                          { "pos",        4 | VET_Pos},
                          },
    
    };

static VertexShaderDesc ShaderDesc_VS_VInstancedTexTGCxformEAlpha = {
    /* Type */          VertexShaderDesc::VS_VInstancedTexTGCxformEAlpha,
    /* Flags */         Shader_Instanced,
    /* pSource */       pSource_VInstancedTexTGCxformEAlpha,
    /* Uniforms */      { 
                          /* cxadd*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      { -1, 0, 0, 0, 0, 0 },
                          /* mvp*/        { -1, 0, 0, 0, 0, 0 },
                          /* offset*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor2*/    { -1, 0, 0, 0, 0, 0 },
                          /* srctex*/     { -1, 0, 0, 0, 0, 0 },
                          /* srctexscale*/{ -1, 0, 0, 0, 0, 0 },
                          /* tex*/        { -1, 0, 0, 0, 0, 0 },
                          /* texgen*/     { -1, 0, 0, 0, 0, 0 },
                          /* texscale*/   { -1, 0, 0, 0, 0, 0 },
                          /* vfmuniforms*/{ -1, 0, 0, 0, 0, 0 },
                          /* vfuniforms*/ {0, 0, 4, 576, 4, 6 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_vfuniforms, 0, 1},
                          /* cxmul*/       {Uniform::SU_vfuniforms, 1, 1},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_vfuniforms, 2, 2},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_vfuniforms, 4, 2},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    /* NumAttribs */    2,
    /* Attributes */    {
                          { "afactor",    4 | VET_Color | (1 << VET_Index_Shift)},
                          { "pos",        4 | VET_Pos},
                          },
    
    };

static VertexShaderDesc ShaderDesc_VS_VInstancedVertex = {
    /* Type */          VertexShaderDesc::VS_VInstancedVertex,
    /* Flags */         Shader_Instanced,
    /* pSource */       pSource_VInstancedVertex,
    /* Uniforms */      { 
                          /* cxadd*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      { -1, 0, 0, 0, 0, 0 },
                          /* mvp*/        { -1, 0, 0, 0, 0, 0 },
                          /* offset*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor2*/    { -1, 0, 0, 0, 0, 0 },
                          /* srctex*/     { -1, 0, 0, 0, 0, 0 },
                          /* srctexscale*/{ -1, 0, 0, 0, 0, 0 },
                          /* tex*/        { -1, 0, 0, 0, 0, 0 },
                          /* texgen*/     { -1, 0, 0, 0, 0, 0 },
                          /* texscale*/   { -1, 0, 0, 0, 0, 0 },
                          /* vfmuniforms*/{ -1, 0, 0, 0, 0, 0 },
                          /* vfuniforms*/ {0, 0, 4, 192, 4, 2 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_vfuniforms, 0, 2},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_Count, -1, 0},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    /* NumAttribs */    2,
    /* Attributes */    {
                          { "acolor",     4 | VET_Color},
                          { "pos",        4 | VET_Pos},
                          },
    
    };

static VertexShaderDesc ShaderDesc_VS_VInstancedVertexEAlpha = {
    /* Type */          VertexShaderDesc::VS_VInstancedVertexEAlpha,
    /* Flags */         Shader_Instanced,
    /* pSource */       pSource_VInstancedVertexEAlpha,
    /* Uniforms */      { 
                          /* cxadd*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      { -1, 0, 0, 0, 0, 0 },
                          /* mvp*/        { -1, 0, 0, 0, 0, 0 },
                          /* offset*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor2*/    { -1, 0, 0, 0, 0, 0 },
                          /* srctex*/     { -1, 0, 0, 0, 0, 0 },
                          /* srctexscale*/{ -1, 0, 0, 0, 0, 0 },
                          /* tex*/        { -1, 0, 0, 0, 0, 0 },
                          /* texgen*/     { -1, 0, 0, 0, 0, 0 },
                          /* texscale*/   { -1, 0, 0, 0, 0, 0 },
                          /* vfmuniforms*/{ -1, 0, 0, 0, 0, 0 },
                          /* vfuniforms*/ {0, 0, 4, 192, 4, 2 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_vfuniforms, 0, 2},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_Count, -1, 0},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    /* NumAttribs */    3,
    /* Attributes */    {
                          { "acolor",     4 | VET_Color},
                          { "afactor",    4 | VET_Color | (1 << VET_Index_Shift)},
                          { "pos",        4 | VET_Pos},
                          },
    
    };

static VertexShaderDesc ShaderDesc_VS_VInstancedVertexCxform = {
    /* Type */          VertexShaderDesc::VS_VInstancedVertexCxform,
    /* Flags */         Shader_Instanced,
    /* pSource */       pSource_VInstancedVertexCxform,
    /* Uniforms */      { 
                          /* cxadd*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      { -1, 0, 0, 0, 0, 0 },
                          /* mvp*/        { -1, 0, 0, 0, 0, 0 },
                          /* offset*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor2*/    { -1, 0, 0, 0, 0, 0 },
                          /* srctex*/     { -1, 0, 0, 0, 0, 0 },
                          /* srctexscale*/{ -1, 0, 0, 0, 0, 0 },
                          /* tex*/        { -1, 0, 0, 0, 0, 0 },
                          /* texgen*/     { -1, 0, 0, 0, 0, 0 },
                          /* texscale*/   { -1, 0, 0, 0, 0, 0 },
                          /* vfmuniforms*/{ -1, 0, 0, 0, 0, 0 },
                          /* vfuniforms*/ {0, 0, 4, 384, 4, 4 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_vfuniforms, 0, 1},
                          /* cxmul*/       {Uniform::SU_vfuniforms, 1, 1},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_vfuniforms, 2, 2},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_Count, -1, 0},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    /* NumAttribs */    2,
    /* Attributes */    {
                          { "acolor",     4 | VET_Color},
                          { "pos",        4 | VET_Pos},
                          },
    
    };

static VertexShaderDesc ShaderDesc_VS_VInstancedVertexCxformEAlpha = {
    /* Type */          VertexShaderDesc::VS_VInstancedVertexCxformEAlpha,
    /* Flags */         Shader_Instanced,
    /* pSource */       pSource_VInstancedVertexCxformEAlpha,
    /* Uniforms */      { 
                          /* cxadd*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      { -1, 0, 0, 0, 0, 0 },
                          /* mvp*/        { -1, 0, 0, 0, 0, 0 },
                          /* offset*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor2*/    { -1, 0, 0, 0, 0, 0 },
                          /* srctex*/     { -1, 0, 0, 0, 0, 0 },
                          /* srctexscale*/{ -1, 0, 0, 0, 0, 0 },
                          /* tex*/        { -1, 0, 0, 0, 0, 0 },
                          /* texgen*/     { -1, 0, 0, 0, 0, 0 },
                          /* texscale*/   { -1, 0, 0, 0, 0, 0 },
                          /* vfmuniforms*/{ -1, 0, 0, 0, 0, 0 },
                          /* vfuniforms*/ {0, 0, 4, 384, 4, 4 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_vfuniforms, 0, 1},
                          /* cxmul*/       {Uniform::SU_vfuniforms, 1, 1},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_vfuniforms, 2, 2},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_Count, -1, 0},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    /* NumAttribs */    3,
    /* Attributes */    {
                          { "acolor",     4 | VET_Color},
                          { "afactor",    4 | VET_Color | (1 << VET_Index_Shift)},
                          { "pos",        4 | VET_Pos},
                          },
    
    };

static VertexShaderDesc ShaderDesc_VS_VInstancedTexTGTexTG = {
    /* Type */          VertexShaderDesc::VS_VInstancedTexTGTexTG,
    /* Flags */         Shader_Instanced,
    /* pSource */       pSource_VInstancedTexTGTexTG,
    /* Uniforms */      { 
                          /* cxadd*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      { -1, 0, 0, 0, 0, 0 },
                          /* mvp*/        { -1, 0, 0, 0, 0, 0 },
                          /* offset*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor2*/    { -1, 0, 0, 0, 0, 0 },
                          /* srctex*/     { -1, 0, 0, 0, 0, 0 },
                          /* srctexscale*/{ -1, 0, 0, 0, 0, 0 },
                          /* tex*/        { -1, 0, 0, 0, 0, 0 },
                          /* texgen*/     { -1, 0, 0, 0, 0, 0 },
                          /* texscale*/   { -1, 0, 0, 0, 0, 0 },
                          /* vfmuniforms*/{ -1, 0, 0, 0, 0, 0 },
                          /* vfuniforms*/ {0, 0, 4, 576, 4, 6 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_vfuniforms, 0, 2},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_vfuniforms, 2, 4},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    /* NumAttribs */    2,
    /* Attributes */    {
                          { "afactor",    4 | VET_Color | (1 << VET_Index_Shift)},
                          { "pos",        4 | VET_Pos},
                          },
    
    };

static VertexShaderDesc ShaderDesc_VS_VInstancedTexTGTexTGCxform = {
    /* Type */          VertexShaderDesc::VS_VInstancedTexTGTexTGCxform,
    /* Flags */         Shader_Instanced,
    /* pSource */       pSource_VInstancedTexTGTexTGCxform,
    /* Uniforms */      { 
                          /* cxadd*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      { -1, 0, 0, 0, 0, 0 },
                          /* mvp*/        { -1, 0, 0, 0, 0, 0 },
                          /* offset*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor2*/    { -1, 0, 0, 0, 0, 0 },
                          /* srctex*/     { -1, 0, 0, 0, 0, 0 },
                          /* srctexscale*/{ -1, 0, 0, 0, 0, 0 },
                          /* tex*/        { -1, 0, 0, 0, 0, 0 },
                          /* texgen*/     { -1, 0, 0, 0, 0, 0 },
                          /* texscale*/   { -1, 0, 0, 0, 0, 0 },
                          /* vfmuniforms*/{ -1, 0, 0, 0, 0, 0 },
                          /* vfuniforms*/ {0, 0, 4, 768, 4, 8 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_vfuniforms, 0, 1},
                          /* cxmul*/       {Uniform::SU_vfuniforms, 1, 1},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_vfuniforms, 2, 2},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_vfuniforms, 4, 4},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    /* NumAttribs */    2,
    /* Attributes */    {
                          { "afactor",    4 | VET_Color | (1 << VET_Index_Shift)},
                          { "pos",        4 | VET_Pos},
                          },
    
    };

static VertexShaderDesc ShaderDesc_VS_VInstancedTexTGVertex = {
    /* Type */          VertexShaderDesc::VS_VInstancedTexTGVertex,
    /* Flags */         Shader_Instanced,
    /* pSource */       pSource_VInstancedTexTGVertex,
    /* Uniforms */      { 
                          /* cxadd*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      { -1, 0, 0, 0, 0, 0 },
                          /* mvp*/        { -1, 0, 0, 0, 0, 0 },
                          /* offset*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor2*/    { -1, 0, 0, 0, 0, 0 },
                          /* srctex*/     { -1, 0, 0, 0, 0, 0 },
                          /* srctexscale*/{ -1, 0, 0, 0, 0, 0 },
                          /* tex*/        { -1, 0, 0, 0, 0, 0 },
                          /* texgen*/     { -1, 0, 0, 0, 0, 0 },
                          /* texscale*/   { -1, 0, 0, 0, 0, 0 },
                          /* vfmuniforms*/{ -1, 0, 0, 0, 0, 0 },
                          /* vfuniforms*/ {0, 0, 4, 384, 4, 4 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_vfuniforms, 0, 2},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_vfuniforms, 2, 2},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    /* NumAttribs */    3,
    /* Attributes */    {
                          { "acolor",     4 | VET_Color},
                          { "afactor",    4 | VET_Color | (1 << VET_Index_Shift)},
                          { "pos",        4 | VET_Pos},
                          },
    
    };

static VertexShaderDesc ShaderDesc_VS_VInstancedTexTGVertexCxform = {
    /* Type */          VertexShaderDesc::VS_VInstancedTexTGVertexCxform,
    /* Flags */         Shader_Instanced,
    /* pSource */       pSource_VInstancedTexTGVertexCxform,
    /* Uniforms */      { 
                          /* cxadd*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      { -1, 0, 0, 0, 0, 0 },
                          /* mvp*/        { -1, 0, 0, 0, 0, 0 },
                          /* offset*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor2*/    { -1, 0, 0, 0, 0, 0 },
                          /* srctex*/     { -1, 0, 0, 0, 0, 0 },
                          /* srctexscale*/{ -1, 0, 0, 0, 0, 0 },
                          /* tex*/        { -1, 0, 0, 0, 0, 0 },
                          /* texgen*/     { -1, 0, 0, 0, 0, 0 },
                          /* texscale*/   { -1, 0, 0, 0, 0, 0 },
                          /* vfmuniforms*/{ -1, 0, 0, 0, 0, 0 },
                          /* vfuniforms*/ {0, 0, 4, 576, 4, 6 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_vfuniforms, 0, 1},
                          /* cxmul*/       {Uniform::SU_vfuniforms, 1, 1},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_vfuniforms, 2, 2},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_vfuniforms, 4, 2},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    /* NumAttribs */    3,
    /* Attributes */    {
                          { "acolor",     4 | VET_Color},
                          { "afactor",    4 | VET_Color | (1 << VET_Index_Shift)},
                          { "pos",        4 | VET_Pos},
                          },
    
    };

static VertexShaderDesc ShaderDesc_VS_VInstancedPosition3dTexTG = {
    /* Type */          VertexShaderDesc::VS_VInstancedPosition3dTexTG,
    /* Flags */         Shader_Instanced,
    /* pSource */       pSource_VInstancedPosition3dTexTG,
    /* Uniforms */      { 
                          /* cxadd*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      { -1, 0, 0, 0, 0, 0 },
                          /* mvp*/        { -1, 0, 0, 0, 0, 0 },
                          /* offset*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor2*/    { -1, 0, 0, 0, 0, 0 },
                          /* srctex*/     { -1, 0, 0, 0, 0, 0 },
                          /* srctexscale*/{ -1, 0, 0, 0, 0, 0 },
                          /* tex*/        { -1, 0, 0, 0, 0, 0 },
                          /* texgen*/     { -1, 0, 0, 0, 0, 0 },
                          /* texscale*/   { -1, 0, 0, 0, 0, 0 },
                          /* vfmuniforms*/{0, 0, 16, 384, 16, 1 },
                          /* vfuniforms*/ {96, 384, 4, 192, 4, 2 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_vfmuniforms, 0, 1},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_vfuniforms, 0, 2},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    /* NumAttribs */    1,
    /* Attributes */    {
                          { "pos",        4 | VET_Pos},
                          },
    
    };

static VertexShaderDesc ShaderDesc_VS_VInstancedPosition3dTexTGEAlpha = {
    /* Type */          VertexShaderDesc::VS_VInstancedPosition3dTexTGEAlpha,
    /* Flags */         Shader_Instanced,
    /* pSource */       pSource_VInstancedPosition3dTexTGEAlpha,
    /* Uniforms */      { 
                          /* cxadd*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      { -1, 0, 0, 0, 0, 0 },
                          /* mvp*/        { -1, 0, 0, 0, 0, 0 },
                          /* offset*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor2*/    { -1, 0, 0, 0, 0, 0 },
                          /* srctex*/     { -1, 0, 0, 0, 0, 0 },
                          /* srctexscale*/{ -1, 0, 0, 0, 0, 0 },
                          /* tex*/        { -1, 0, 0, 0, 0, 0 },
                          /* texgen*/     { -1, 0, 0, 0, 0, 0 },
                          /* texscale*/   { -1, 0, 0, 0, 0, 0 },
                          /* vfmuniforms*/{0, 0, 16, 384, 16, 1 },
                          /* vfuniforms*/ {96, 384, 4, 192, 4, 2 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_vfmuniforms, 0, 1},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_vfuniforms, 0, 2},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    /* NumAttribs */    2,
    /* Attributes */    {
                          { "afactor",    4 | VET_Color | (1 << VET_Index_Shift)},
                          { "pos",        4 | VET_Pos},
                          },
    
    };

static VertexShaderDesc ShaderDesc_VS_VInstancedPosition3dTexTGCxform = {
    /* Type */          VertexShaderDesc::VS_VInstancedPosition3dTexTGCxform,
    /* Flags */         Shader_Instanced,
    /* pSource */       pSource_VInstancedPosition3dTexTGCxform,
    /* Uniforms */      { 
                          /* cxadd*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      { -1, 0, 0, 0, 0, 0 },
                          /* mvp*/        { -1, 0, 0, 0, 0, 0 },
                          /* offset*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor2*/    { -1, 0, 0, 0, 0, 0 },
                          /* srctex*/     { -1, 0, 0, 0, 0, 0 },
                          /* srctexscale*/{ -1, 0, 0, 0, 0, 0 },
                          /* tex*/        { -1, 0, 0, 0, 0, 0 },
                          /* texgen*/     { -1, 0, 0, 0, 0, 0 },
                          /* texscale*/   { -1, 0, 0, 0, 0, 0 },
                          /* vfmuniforms*/{0, 0, 16, 384, 16, 1 },
                          /* vfuniforms*/ {96, 384, 4, 384, 4, 4 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_vfuniforms, 0, 1},
                          /* cxmul*/       {Uniform::SU_vfuniforms, 1, 1},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_vfmuniforms, 0, 1},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_vfuniforms, 2, 2},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    /* NumAttribs */    1,
    /* Attributes */    {
                          { "pos",        4 | VET_Pos},
                          },
    
    };

static VertexShaderDesc ShaderDesc_VS_VInstancedPosition3dTexTGCxformEAlpha = {
    /* Type */          VertexShaderDesc::VS_VInstancedPosition3dTexTGCxformEAlpha,
    /* Flags */         Shader_Instanced,
    /* pSource */       pSource_VInstancedPosition3dTexTGCxformEAlpha,
    /* Uniforms */      { 
                          /* cxadd*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      { -1, 0, 0, 0, 0, 0 },
                          /* mvp*/        { -1, 0, 0, 0, 0, 0 },
                          /* offset*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor2*/    { -1, 0, 0, 0, 0, 0 },
                          /* srctex*/     { -1, 0, 0, 0, 0, 0 },
                          /* srctexscale*/{ -1, 0, 0, 0, 0, 0 },
                          /* tex*/        { -1, 0, 0, 0, 0, 0 },
                          /* texgen*/     { -1, 0, 0, 0, 0, 0 },
                          /* texscale*/   { -1, 0, 0, 0, 0, 0 },
                          /* vfmuniforms*/{0, 0, 16, 384, 16, 1 },
                          /* vfuniforms*/ {96, 384, 4, 384, 4, 4 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_vfuniforms, 0, 1},
                          /* cxmul*/       {Uniform::SU_vfuniforms, 1, 1},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_vfmuniforms, 0, 1},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_vfuniforms, 2, 2},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    /* NumAttribs */    2,
    /* Attributes */    {
                          { "afactor",    4 | VET_Color | (1 << VET_Index_Shift)},
                          { "pos",        4 | VET_Pos},
                          },
    
    };

static VertexShaderDesc ShaderDesc_VS_VInstancedPosition3dVertex = {
    /* Type */          VertexShaderDesc::VS_VInstancedPosition3dVertex,
    /* Flags */         Shader_Instanced,
    /* pSource */       pSource_VInstancedPosition3dVertex,
    /* Uniforms */      { 
                          /* cxadd*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      { -1, 0, 0, 0, 0, 0 },
                          /* mvp*/        { -1, 0, 0, 0, 0, 0 },
                          /* offset*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor2*/    { -1, 0, 0, 0, 0, 0 },
                          /* srctex*/     { -1, 0, 0, 0, 0, 0 },
                          /* srctexscale*/{ -1, 0, 0, 0, 0, 0 },
                          /* tex*/        { -1, 0, 0, 0, 0, 0 },
                          /* texgen*/     { -1, 0, 0, 0, 0, 0 },
                          /* texscale*/   { -1, 0, 0, 0, 0, 0 },
                          /* vfmuniforms*/{0, 0, 16, 384, 16, 1 },
                          /* vfuniforms*/ { -1, 0, 0, 0, 0, 0 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_vfmuniforms, 0, 1},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_Count, -1, 0},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    /* NumAttribs */    2,
    /* Attributes */    {
                          { "acolor",     4 | VET_Color},
                          { "pos",        4 | VET_Pos},
                          },
    
    };

static VertexShaderDesc ShaderDesc_VS_VInstancedPosition3dVertexEAlpha = {
    /* Type */          VertexShaderDesc::VS_VInstancedPosition3dVertexEAlpha,
    /* Flags */         Shader_Instanced,
    /* pSource */       pSource_VInstancedPosition3dVertexEAlpha,
    /* Uniforms */      { 
                          /* cxadd*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      { -1, 0, 0, 0, 0, 0 },
                          /* mvp*/        { -1, 0, 0, 0, 0, 0 },
                          /* offset*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor2*/    { -1, 0, 0, 0, 0, 0 },
                          /* srctex*/     { -1, 0, 0, 0, 0, 0 },
                          /* srctexscale*/{ -1, 0, 0, 0, 0, 0 },
                          /* tex*/        { -1, 0, 0, 0, 0, 0 },
                          /* texgen*/     { -1, 0, 0, 0, 0, 0 },
                          /* texscale*/   { -1, 0, 0, 0, 0, 0 },
                          /* vfmuniforms*/{0, 0, 16, 384, 16, 1 },
                          /* vfuniforms*/ { -1, 0, 0, 0, 0, 0 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_vfmuniforms, 0, 1},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_Count, -1, 0},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    /* NumAttribs */    3,
    /* Attributes */    {
                          { "acolor",     4 | VET_Color},
                          { "afactor",    4 | VET_Color | (1 << VET_Index_Shift)},
                          { "pos",        4 | VET_Pos},
                          },
    
    };

static VertexShaderDesc ShaderDesc_VS_VInstancedPosition3dVertexCxform = {
    /* Type */          VertexShaderDesc::VS_VInstancedPosition3dVertexCxform,
    /* Flags */         Shader_Instanced,
    /* pSource */       pSource_VInstancedPosition3dVertexCxform,
    /* Uniforms */      { 
                          /* cxadd*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      { -1, 0, 0, 0, 0, 0 },
                          /* mvp*/        { -1, 0, 0, 0, 0, 0 },
                          /* offset*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor2*/    { -1, 0, 0, 0, 0, 0 },
                          /* srctex*/     { -1, 0, 0, 0, 0, 0 },
                          /* srctexscale*/{ -1, 0, 0, 0, 0, 0 },
                          /* tex*/        { -1, 0, 0, 0, 0, 0 },
                          /* texgen*/     { -1, 0, 0, 0, 0, 0 },
                          /* texscale*/   { -1, 0, 0, 0, 0, 0 },
                          /* vfmuniforms*/{0, 0, 16, 384, 16, 1 },
                          /* vfuniforms*/ {96, 384, 4, 192, 4, 2 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_vfuniforms, 0, 1},
                          /* cxmul*/       {Uniform::SU_vfuniforms, 1, 1},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_vfmuniforms, 0, 1},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_Count, -1, 0},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    /* NumAttribs */    2,
    /* Attributes */    {
                          { "acolor",     4 | VET_Color},
                          { "pos",        4 | VET_Pos},
                          },
    
    };

static VertexShaderDesc ShaderDesc_VS_VInstancedPosition3dVertexCxformEAlpha = {
    /* Type */          VertexShaderDesc::VS_VInstancedPosition3dVertexCxformEAlpha,
    /* Flags */         Shader_Instanced,
    /* pSource */       pSource_VInstancedPosition3dVertexCxformEAlpha,
    /* Uniforms */      { 
                          /* cxadd*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      { -1, 0, 0, 0, 0, 0 },
                          /* mvp*/        { -1, 0, 0, 0, 0, 0 },
                          /* offset*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor2*/    { -1, 0, 0, 0, 0, 0 },
                          /* srctex*/     { -1, 0, 0, 0, 0, 0 },
                          /* srctexscale*/{ -1, 0, 0, 0, 0, 0 },
                          /* tex*/        { -1, 0, 0, 0, 0, 0 },
                          /* texgen*/     { -1, 0, 0, 0, 0, 0 },
                          /* texscale*/   { -1, 0, 0, 0, 0, 0 },
                          /* vfmuniforms*/{0, 0, 16, 384, 16, 1 },
                          /* vfuniforms*/ {96, 384, 4, 192, 4, 2 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_vfuniforms, 0, 1},
                          /* cxmul*/       {Uniform::SU_vfuniforms, 1, 1},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_vfmuniforms, 0, 1},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_Count, -1, 0},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    /* NumAttribs */    3,
    /* Attributes */    {
                          { "acolor",     4 | VET_Color},
                          { "afactor",    4 | VET_Color | (1 << VET_Index_Shift)},
                          { "pos",        4 | VET_Pos},
                          },
    
    };

static VertexShaderDesc ShaderDesc_VS_VInstancedPosition3dTexTGTexTG = {
    /* Type */          VertexShaderDesc::VS_VInstancedPosition3dTexTGTexTG,
    /* Flags */         Shader_Instanced,
    /* pSource */       pSource_VInstancedPosition3dTexTGTexTG,
    /* Uniforms */      { 
                          /* cxadd*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      { -1, 0, 0, 0, 0, 0 },
                          /* mvp*/        { -1, 0, 0, 0, 0, 0 },
                          /* offset*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor2*/    { -1, 0, 0, 0, 0, 0 },
                          /* srctex*/     { -1, 0, 0, 0, 0, 0 },
                          /* srctexscale*/{ -1, 0, 0, 0, 0, 0 },
                          /* tex*/        { -1, 0, 0, 0, 0, 0 },
                          /* texgen*/     { -1, 0, 0, 0, 0, 0 },
                          /* texscale*/   { -1, 0, 0, 0, 0, 0 },
                          /* vfmuniforms*/{0, 0, 16, 384, 16, 1 },
                          /* vfuniforms*/ {96, 384, 4, 384, 4, 4 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_vfmuniforms, 0, 1},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_vfuniforms, 0, 4},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    /* NumAttribs */    2,
    /* Attributes */    {
                          { "afactor",    4 | VET_Color | (1 << VET_Index_Shift)},
                          { "pos",        4 | VET_Pos},
                          },
    
    };

static VertexShaderDesc ShaderDesc_VS_VInstancedPosition3dTexTGTexTGCxform = {
    /* Type */          VertexShaderDesc::VS_VInstancedPosition3dTexTGTexTGCxform,
    /* Flags */         Shader_Instanced,
    /* pSource */       pSource_VInstancedPosition3dTexTGTexTGCxform,
    /* Uniforms */      { 
                          /* cxadd*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      { -1, 0, 0, 0, 0, 0 },
                          /* mvp*/        { -1, 0, 0, 0, 0, 0 },
                          /* offset*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor2*/    { -1, 0, 0, 0, 0, 0 },
                          /* srctex*/     { -1, 0, 0, 0, 0, 0 },
                          /* srctexscale*/{ -1, 0, 0, 0, 0, 0 },
                          /* tex*/        { -1, 0, 0, 0, 0, 0 },
                          /* texgen*/     { -1, 0, 0, 0, 0, 0 },
                          /* texscale*/   { -1, 0, 0, 0, 0, 0 },
                          /* vfmuniforms*/{0, 0, 16, 384, 16, 1 },
                          /* vfuniforms*/ {96, 384, 4, 576, 4, 6 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_vfuniforms, 0, 1},
                          /* cxmul*/       {Uniform::SU_vfuniforms, 1, 1},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_vfmuniforms, 0, 1},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_vfuniforms, 2, 4},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    /* NumAttribs */    2,
    /* Attributes */    {
                          { "afactor",    4 | VET_Color | (1 << VET_Index_Shift)},
                          { "pos",        4 | VET_Pos},
                          },
    
    };

static VertexShaderDesc ShaderDesc_VS_VInstancedPosition3dTexTGVertex = {
    /* Type */          VertexShaderDesc::VS_VInstancedPosition3dTexTGVertex,
    /* Flags */         Shader_Instanced,
    /* pSource */       pSource_VInstancedPosition3dTexTGVertex,
    /* Uniforms */      { 
                          /* cxadd*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      { -1, 0, 0, 0, 0, 0 },
                          /* mvp*/        { -1, 0, 0, 0, 0, 0 },
                          /* offset*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor2*/    { -1, 0, 0, 0, 0, 0 },
                          /* srctex*/     { -1, 0, 0, 0, 0, 0 },
                          /* srctexscale*/{ -1, 0, 0, 0, 0, 0 },
                          /* tex*/        { -1, 0, 0, 0, 0, 0 },
                          /* texgen*/     { -1, 0, 0, 0, 0, 0 },
                          /* texscale*/   { -1, 0, 0, 0, 0, 0 },
                          /* vfmuniforms*/{0, 0, 16, 384, 16, 1 },
                          /* vfuniforms*/ {96, 384, 4, 192, 4, 2 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_vfmuniforms, 0, 1},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_vfuniforms, 0, 2},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    /* NumAttribs */    3,
    /* Attributes */    {
                          { "acolor",     4 | VET_Color},
                          { "afactor",    4 | VET_Color | (1 << VET_Index_Shift)},
                          { "pos",        4 | VET_Pos},
                          },
    
    };

static VertexShaderDesc ShaderDesc_VS_VInstancedPosition3dTexTGVertexCxform = {
    /* Type */          VertexShaderDesc::VS_VInstancedPosition3dTexTGVertexCxform,
    /* Flags */         Shader_Instanced,
    /* pSource */       pSource_VInstancedPosition3dTexTGVertexCxform,
    /* Uniforms */      { 
                          /* cxadd*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      { -1, 0, 0, 0, 0, 0 },
                          /* mvp*/        { -1, 0, 0, 0, 0, 0 },
                          /* offset*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor2*/    { -1, 0, 0, 0, 0, 0 },
                          /* srctex*/     { -1, 0, 0, 0, 0, 0 },
                          /* srctexscale*/{ -1, 0, 0, 0, 0, 0 },
                          /* tex*/        { -1, 0, 0, 0, 0, 0 },
                          /* texgen*/     { -1, 0, 0, 0, 0, 0 },
                          /* texscale*/   { -1, 0, 0, 0, 0, 0 },
                          /* vfmuniforms*/{0, 0, 16, 384, 16, 1 },
                          /* vfuniforms*/ {96, 384, 4, 384, 4, 4 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_vfuniforms, 0, 1},
                          /* cxmul*/       {Uniform::SU_vfuniforms, 1, 1},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_vfmuniforms, 0, 1},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_vfuniforms, 2, 2},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    /* NumAttribs */    3,
    /* Attributes */    {
                          { "acolor",     4 | VET_Color},
                          { "afactor",    4 | VET_Color | (1 << VET_Index_Shift)},
                          { "pos",        4 | VET_Pos},
                          },
    
    };

static VertexShaderDesc ShaderDesc_VS_VSolid = {
    /* Type */          VertexShaderDesc::VS_VSolid,
    /* Flags */         0,
    /* pSource */       pSource_VSolid,
    /* Uniforms */      { 
                          /* cxadd*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      { -1, 0, 0, 0, 0, 0 },
                          /* mvp*/        {0, 0, 4, 8, 4, 0 },
                          /* offset*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor2*/    { -1, 0, 0, 0, 0, 0 },
                          /* srctex*/     { -1, 0, 0, 0, 0, 0 },
                          /* srctexscale*/{ -1, 0, 0, 0, 0, 0 },
                          /* tex*/        { -1, 0, 0, 0, 0, 0 },
                          /* texgen*/     { -1, 0, 0, 0, 0, 0 },
                          /* texscale*/   { -1, 0, 0, 0, 0, 0 },
                          /* vfmuniforms*/{ -1, 0, 0, 0, 0, 0 },
                          /* vfuniforms*/ { -1, 0, 0, 0, 0, 0 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_Count, -1, 0},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_Count, -1, 0},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    /* NumAttribs */    1,
    /* Attributes */    {
                          { "pos",        4 | VET_Pos},
                          },
    
    };

static VertexShaderDesc ShaderDesc_VS_VText = {
    /* Type */          VertexShaderDesc::VS_VText,
    /* Flags */         0,
    /* pSource */       pSource_VText,
    /* Uniforms */      { 
                          /* cxadd*/      {0, 0, 4, 4, 4, 0 },
                          /* cxmul*/      {1, 4, 4, 4, 4, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      { -1, 0, 0, 0, 0, 0 },
                          /* mvp*/        {2, 8, 4, 8, 4, 0 },
                          /* offset*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor2*/    { -1, 0, 0, 0, 0, 0 },
                          /* srctex*/     { -1, 0, 0, 0, 0, 0 },
                          /* srctexscale*/{ -1, 0, 0, 0, 0, 0 },
                          /* tex*/        { -1, 0, 0, 0, 0, 0 },
                          /* texgen*/     { -1, 0, 0, 0, 0, 0 },
                          /* texscale*/   { -1, 0, 0, 0, 0, 0 },
                          /* vfmuniforms*/{ -1, 0, 0, 0, 0, 0 },
                          /* vfuniforms*/ { -1, 0, 0, 0, 0, 0 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_Count, -1, 0},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_Count, -1, 0},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    /* NumAttribs */    3,
    /* Attributes */    {
                          { "acolor",     4 | VET_Color},
                          { "atc",        2 | VET_TexCoord},
                          { "pos",        4 | VET_Pos},
                          },
    
    };

static VertexShaderDesc ShaderDesc_VS_VTextColor = {
    /* Type */          VertexShaderDesc::VS_VTextColor,
    /* Flags */         0,
    /* pSource */       pSource_VTextColor,
    /* Uniforms */      { 
                          /* cxadd*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      { -1, 0, 0, 0, 0, 0 },
                          /* mvp*/        {0, 0, 4, 8, 4, 0 },
                          /* offset*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor2*/    { -1, 0, 0, 0, 0, 0 },
                          /* srctex*/     { -1, 0, 0, 0, 0, 0 },
                          /* srctexscale*/{ -1, 0, 0, 0, 0, 0 },
                          /* tex*/        { -1, 0, 0, 0, 0, 0 },
                          /* texgen*/     { -1, 0, 0, 0, 0, 0 },
                          /* texscale*/   { -1, 0, 0, 0, 0, 0 },
                          /* vfmuniforms*/{ -1, 0, 0, 0, 0, 0 },
                          /* vfuniforms*/ { -1, 0, 0, 0, 0, 0 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_Count, -1, 0},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_Count, -1, 0},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    /* NumAttribs */    2,
    /* Attributes */    {
                          { "atc",        2 | VET_TexCoord},
                          { "pos",        4 | VET_Pos},
                          },
    
    };

static VertexShaderDesc ShaderDesc_VS_VTextColorCxform = {
    /* Type */          VertexShaderDesc::VS_VTextColorCxform,
    /* Flags */         0,
    /* pSource */       pSource_VTextColorCxform,
    /* Uniforms */      { 
                          /* cxadd*/      {0, 0, 4, 4, 4, 0 },
                          /* cxmul*/      {1, 4, 4, 4, 4, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      { -1, 0, 0, 0, 0, 0 },
                          /* mvp*/        {2, 8, 4, 8, 4, 0 },
                          /* offset*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor2*/    { -1, 0, 0, 0, 0, 0 },
                          /* srctex*/     { -1, 0, 0, 0, 0, 0 },
                          /* srctexscale*/{ -1, 0, 0, 0, 0, 0 },
                          /* tex*/        { -1, 0, 0, 0, 0, 0 },
                          /* texgen*/     { -1, 0, 0, 0, 0, 0 },
                          /* texscale*/   { -1, 0, 0, 0, 0, 0 },
                          /* vfmuniforms*/{ -1, 0, 0, 0, 0, 0 },
                          /* vfuniforms*/ { -1, 0, 0, 0, 0, 0 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_Count, -1, 0},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_Count, -1, 0},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    /* NumAttribs */    2,
    /* Attributes */    {
                          { "atc",        2 | VET_TexCoord},
                          { "pos",        4 | VET_Pos},
                          },
    
    };

static VertexShaderDesc ShaderDesc_VS_VPosition3dSolid = {
    /* Type */          VertexShaderDesc::VS_VPosition3dSolid,
    /* Flags */         0,
    /* pSource */       pSource_VPosition3dSolid,
    /* Uniforms */      { 
                          /* cxadd*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      { -1, 0, 0, 0, 0, 0 },
                          /* mvp*/        {0, 0, 16, 16, 16, 0 },
                          /* offset*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor2*/    { -1, 0, 0, 0, 0, 0 },
                          /* srctex*/     { -1, 0, 0, 0, 0, 0 },
                          /* srctexscale*/{ -1, 0, 0, 0, 0, 0 },
                          /* tex*/        { -1, 0, 0, 0, 0, 0 },
                          /* texgen*/     { -1, 0, 0, 0, 0, 0 },
                          /* texscale*/   { -1, 0, 0, 0, 0, 0 },
                          /* vfmuniforms*/{ -1, 0, 0, 0, 0, 0 },
                          /* vfuniforms*/ { -1, 0, 0, 0, 0, 0 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_Count, -1, 0},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_Count, -1, 0},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    /* NumAttribs */    1,
    /* Attributes */    {
                          { "pos",        4 | VET_Pos},
                          },
    
    };

static VertexShaderDesc ShaderDesc_VS_VPosition3dText = {
    /* Type */          VertexShaderDesc::VS_VPosition3dText,
    /* Flags */         0,
    /* pSource */       pSource_VPosition3dText,
    /* Uniforms */      { 
                          /* cxadd*/      {0, 0, 4, 4, 4, 0 },
                          /* cxmul*/      {1, 4, 4, 4, 4, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      { -1, 0, 0, 0, 0, 0 },
                          /* mvp*/        {2, 8, 16, 16, 16, 0 },
                          /* offset*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor2*/    { -1, 0, 0, 0, 0, 0 },
                          /* srctex*/     { -1, 0, 0, 0, 0, 0 },
                          /* srctexscale*/{ -1, 0, 0, 0, 0, 0 },
                          /* tex*/        { -1, 0, 0, 0, 0, 0 },
                          /* texgen*/     { -1, 0, 0, 0, 0, 0 },
                          /* texscale*/   { -1, 0, 0, 0, 0, 0 },
                          /* vfmuniforms*/{ -1, 0, 0, 0, 0, 0 },
                          /* vfuniforms*/ { -1, 0, 0, 0, 0, 0 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_Count, -1, 0},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_Count, -1, 0},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    /* NumAttribs */    3,
    /* Attributes */    {
                          { "acolor",     4 | VET_Color},
                          { "atc",        2 | VET_TexCoord},
                          { "pos",        4 | VET_Pos},
                          },
    
    };

static VertexShaderDesc ShaderDesc_VS_VPosition3dTextColor = {
    /* Type */          VertexShaderDesc::VS_VPosition3dTextColor,
    /* Flags */         0,
    /* pSource */       pSource_VPosition3dTextColor,
    /* Uniforms */      { 
                          /* cxadd*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      { -1, 0, 0, 0, 0, 0 },
                          /* mvp*/        {0, 0, 16, 16, 16, 0 },
                          /* offset*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor2*/    { -1, 0, 0, 0, 0, 0 },
                          /* srctex*/     { -1, 0, 0, 0, 0, 0 },
                          /* srctexscale*/{ -1, 0, 0, 0, 0, 0 },
                          /* tex*/        { -1, 0, 0, 0, 0, 0 },
                          /* texgen*/     { -1, 0, 0, 0, 0, 0 },
                          /* texscale*/   { -1, 0, 0, 0, 0, 0 },
                          /* vfmuniforms*/{ -1, 0, 0, 0, 0, 0 },
                          /* vfuniforms*/ { -1, 0, 0, 0, 0, 0 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_Count, -1, 0},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_Count, -1, 0},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    /* NumAttribs */    2,
    /* Attributes */    {
                          { "atc",        2 | VET_TexCoord},
                          { "pos",        4 | VET_Pos},
                          },
    
    };

static VertexShaderDesc ShaderDesc_VS_VPosition3dTextColorCxform = {
    /* Type */          VertexShaderDesc::VS_VPosition3dTextColorCxform,
    /* Flags */         0,
    /* pSource */       pSource_VPosition3dTextColorCxform,
    /* Uniforms */      { 
                          /* cxadd*/      {0, 0, 4, 4, 4, 0 },
                          /* cxmul*/      {1, 4, 4, 4, 4, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      { -1, 0, 0, 0, 0, 0 },
                          /* mvp*/        {2, 8, 16, 16, 16, 0 },
                          /* offset*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor2*/    { -1, 0, 0, 0, 0, 0 },
                          /* srctex*/     { -1, 0, 0, 0, 0, 0 },
                          /* srctexscale*/{ -1, 0, 0, 0, 0, 0 },
                          /* tex*/        { -1, 0, 0, 0, 0, 0 },
                          /* texgen*/     { -1, 0, 0, 0, 0, 0 },
                          /* texscale*/   { -1, 0, 0, 0, 0, 0 },
                          /* vfmuniforms*/{ -1, 0, 0, 0, 0, 0 },
                          /* vfuniforms*/ { -1, 0, 0, 0, 0, 0 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_Count, -1, 0},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_Count, -1, 0},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    /* NumAttribs */    2,
    /* Attributes */    {
                          { "atc",        2 | VET_TexCoord},
                          { "pos",        4 | VET_Pos},
                          },
    
    };

static VertexShaderDesc ShaderDesc_VS_VBatchSolid = {
    /* Type */          VertexShaderDesc::VS_VBatchSolid,
    /* Flags */         Shader_Batch,
    /* pSource */       pSource_VBatchSolid,
    /* Uniforms */      { 
                          /* cxadd*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      { -1, 0, 0, 0, 0, 0 },
                          /* mvp*/        { -1, 0, 0, 0, 0, 0 },
                          /* offset*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor2*/    { -1, 0, 0, 0, 0, 0 },
                          /* srctex*/     { -1, 0, 0, 0, 0, 0 },
                          /* srctexscale*/{ -1, 0, 0, 0, 0, 0 },
                          /* tex*/        { -1, 0, 0, 0, 0, 0 },
                          /* texgen*/     { -1, 0, 0, 0, 0, 0 },
                          /* texscale*/   { -1, 0, 0, 0, 0, 0 },
                          /* vfmuniforms*/{ -1, 0, 0, 0, 0, 0 },
                          /* vfuniforms*/ {0, 0, 4, 192, 4, 2 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_vfuniforms, 0, 2},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_Count, -1, 0},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    /* NumAttribs */    2,
    /* Attributes */    {
                          { "pos",        4 | VET_Pos},
                          { "vbatch",     1 | VET_Instance8},
                          },
    
    };

static VertexShaderDesc ShaderDesc_VS_VBatchText = {
    /* Type */          VertexShaderDesc::VS_VBatchText,
    /* Flags */         Shader_Batch,
    /* pSource */       pSource_VBatchText,
    /* Uniforms */      { 
                          /* cxadd*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      { -1, 0, 0, 0, 0, 0 },
                          /* mvp*/        { -1, 0, 0, 0, 0, 0 },
                          /* offset*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor2*/    { -1, 0, 0, 0, 0, 0 },
                          /* srctex*/     { -1, 0, 0, 0, 0, 0 },
                          /* srctexscale*/{ -1, 0, 0, 0, 0, 0 },
                          /* tex*/        { -1, 0, 0, 0, 0, 0 },
                          /* texgen*/     { -1, 0, 0, 0, 0, 0 },
                          /* texscale*/   { -1, 0, 0, 0, 0, 0 },
                          /* vfmuniforms*/{ -1, 0, 0, 0, 0, 0 },
                          /* vfuniforms*/ {0, 0, 4, 384, 4, 4 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_vfuniforms, 0, 1},
                          /* cxmul*/       {Uniform::SU_vfuniforms, 1, 1},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_vfuniforms, 2, 2},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_Count, -1, 0},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    /* NumAttribs */    4,
    /* Attributes */    {
                          { "acolor",     4 | VET_Color},
                          { "atc",        2 | VET_TexCoord},
                          { "pos",        4 | VET_Pos},
                          { "vbatch",     1 | VET_Instance8},
                          },
    
    };

static VertexShaderDesc ShaderDesc_VS_VBatchTextColor = {
    /* Type */          VertexShaderDesc::VS_VBatchTextColor,
    /* Flags */         Shader_Batch,
    /* pSource */       pSource_VBatchTextColor,
    /* Uniforms */      { 
                          /* cxadd*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      { -1, 0, 0, 0, 0, 0 },
                          /* mvp*/        { -1, 0, 0, 0, 0, 0 },
                          /* offset*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor2*/    { -1, 0, 0, 0, 0, 0 },
                          /* srctex*/     { -1, 0, 0, 0, 0, 0 },
                          /* srctexscale*/{ -1, 0, 0, 0, 0, 0 },
                          /* tex*/        { -1, 0, 0, 0, 0, 0 },
                          /* texgen*/     { -1, 0, 0, 0, 0, 0 },
                          /* texscale*/   { -1, 0, 0, 0, 0, 0 },
                          /* vfmuniforms*/{ -1, 0, 0, 0, 0, 0 },
                          /* vfuniforms*/ {0, 0, 4, 192, 4, 2 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_vfuniforms, 0, 2},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_Count, -1, 0},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    /* NumAttribs */    3,
    /* Attributes */    {
                          { "atc",        2 | VET_TexCoord},
                          { "pos",        4 | VET_Pos},
                          { "vbatch",     1 | VET_Instance8},
                          },
    
    };

static VertexShaderDesc ShaderDesc_VS_VBatchTextColorCxform = {
    /* Type */          VertexShaderDesc::VS_VBatchTextColorCxform,
    /* Flags */         Shader_Batch,
    /* pSource */       pSource_VBatchTextColorCxform,
    /* Uniforms */      { 
                          /* cxadd*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      { -1, 0, 0, 0, 0, 0 },
                          /* mvp*/        { -1, 0, 0, 0, 0, 0 },
                          /* offset*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor2*/    { -1, 0, 0, 0, 0, 0 },
                          /* srctex*/     { -1, 0, 0, 0, 0, 0 },
                          /* srctexscale*/{ -1, 0, 0, 0, 0, 0 },
                          /* tex*/        { -1, 0, 0, 0, 0, 0 },
                          /* texgen*/     { -1, 0, 0, 0, 0, 0 },
                          /* texscale*/   { -1, 0, 0, 0, 0, 0 },
                          /* vfmuniforms*/{ -1, 0, 0, 0, 0, 0 },
                          /* vfuniforms*/ {0, 0, 4, 384, 4, 4 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_vfuniforms, 0, 1},
                          /* cxmul*/       {Uniform::SU_vfuniforms, 1, 1},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_vfuniforms, 2, 2},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_Count, -1, 0},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    /* NumAttribs */    3,
    /* Attributes */    {
                          { "atc",        2 | VET_TexCoord},
                          { "pos",        4 | VET_Pos},
                          { "vbatch",     1 | VET_Instance8},
                          },
    
    };

static VertexShaderDesc ShaderDesc_VS_VBatchPosition3dSolid = {
    /* Type */          VertexShaderDesc::VS_VBatchPosition3dSolid,
    /* Flags */         Shader_Batch,
    /* pSource */       pSource_VBatchPosition3dSolid,
    /* Uniforms */      { 
                          /* cxadd*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      { -1, 0, 0, 0, 0, 0 },
                          /* mvp*/        { -1, 0, 0, 0, 0, 0 },
                          /* offset*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor2*/    { -1, 0, 0, 0, 0, 0 },
                          /* srctex*/     { -1, 0, 0, 0, 0, 0 },
                          /* srctexscale*/{ -1, 0, 0, 0, 0, 0 },
                          /* tex*/        { -1, 0, 0, 0, 0, 0 },
                          /* texgen*/     { -1, 0, 0, 0, 0, 0 },
                          /* texscale*/   { -1, 0, 0, 0, 0, 0 },
                          /* vfmuniforms*/{0, 0, 16, 384, 16, 1 },
                          /* vfuniforms*/ { -1, 0, 0, 0, 0, 0 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_vfmuniforms, 0, 1},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_Count, -1, 0},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    /* NumAttribs */    2,
    /* Attributes */    {
                          { "pos",        4 | VET_Pos},
                          { "vbatch",     1 | VET_Instance8},
                          },
    
    };

static VertexShaderDesc ShaderDesc_VS_VBatchPosition3dText = {
    /* Type */          VertexShaderDesc::VS_VBatchPosition3dText,
    /* Flags */         Shader_Batch,
    /* pSource */       pSource_VBatchPosition3dText,
    /* Uniforms */      { 
                          /* cxadd*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      { -1, 0, 0, 0, 0, 0 },
                          /* mvp*/        { -1, 0, 0, 0, 0, 0 },
                          /* offset*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor2*/    { -1, 0, 0, 0, 0, 0 },
                          /* srctex*/     { -1, 0, 0, 0, 0, 0 },
                          /* srctexscale*/{ -1, 0, 0, 0, 0, 0 },
                          /* tex*/        { -1, 0, 0, 0, 0, 0 },
                          /* texgen*/     { -1, 0, 0, 0, 0, 0 },
                          /* texscale*/   { -1, 0, 0, 0, 0, 0 },
                          /* vfmuniforms*/{0, 0, 16, 384, 16, 1 },
                          /* vfuniforms*/ {96, 384, 4, 192, 4, 2 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_vfuniforms, 0, 1},
                          /* cxmul*/       {Uniform::SU_vfuniforms, 1, 1},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_vfmuniforms, 0, 1},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_Count, -1, 0},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    /* NumAttribs */    4,
    /* Attributes */    {
                          { "acolor",     4 | VET_Color},
                          { "atc",        2 | VET_TexCoord},
                          { "pos",        4 | VET_Pos},
                          { "vbatch",     1 | VET_Instance8},
                          },
    
    };

static VertexShaderDesc ShaderDesc_VS_VBatchPosition3dTextColor = {
    /* Type */          VertexShaderDesc::VS_VBatchPosition3dTextColor,
    /* Flags */         Shader_Batch,
    /* pSource */       pSource_VBatchPosition3dTextColor,
    /* Uniforms */      { 
                          /* cxadd*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      { -1, 0, 0, 0, 0, 0 },
                          /* mvp*/        { -1, 0, 0, 0, 0, 0 },
                          /* offset*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor2*/    { -1, 0, 0, 0, 0, 0 },
                          /* srctex*/     { -1, 0, 0, 0, 0, 0 },
                          /* srctexscale*/{ -1, 0, 0, 0, 0, 0 },
                          /* tex*/        { -1, 0, 0, 0, 0, 0 },
                          /* texgen*/     { -1, 0, 0, 0, 0, 0 },
                          /* texscale*/   { -1, 0, 0, 0, 0, 0 },
                          /* vfmuniforms*/{0, 0, 16, 384, 16, 1 },
                          /* vfuniforms*/ { -1, 0, 0, 0, 0, 0 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_vfmuniforms, 0, 1},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_Count, -1, 0},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    /* NumAttribs */    3,
    /* Attributes */    {
                          { "atc",        2 | VET_TexCoord},
                          { "pos",        4 | VET_Pos},
                          { "vbatch",     1 | VET_Instance8},
                          },
    
    };

static VertexShaderDesc ShaderDesc_VS_VBatchPosition3dTextColorCxform = {
    /* Type */          VertexShaderDesc::VS_VBatchPosition3dTextColorCxform,
    /* Flags */         Shader_Batch,
    /* pSource */       pSource_VBatchPosition3dTextColorCxform,
    /* Uniforms */      { 
                          /* cxadd*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      { -1, 0, 0, 0, 0, 0 },
                          /* mvp*/        { -1, 0, 0, 0, 0, 0 },
                          /* offset*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor2*/    { -1, 0, 0, 0, 0, 0 },
                          /* srctex*/     { -1, 0, 0, 0, 0, 0 },
                          /* srctexscale*/{ -1, 0, 0, 0, 0, 0 },
                          /* tex*/        { -1, 0, 0, 0, 0, 0 },
                          /* texgen*/     { -1, 0, 0, 0, 0, 0 },
                          /* texscale*/   { -1, 0, 0, 0, 0, 0 },
                          /* vfmuniforms*/{0, 0, 16, 384, 16, 1 },
                          /* vfuniforms*/ {96, 384, 4, 192, 4, 2 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_vfuniforms, 0, 1},
                          /* cxmul*/       {Uniform::SU_vfuniforms, 1, 1},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_vfmuniforms, 0, 1},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_Count, -1, 0},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    /* NumAttribs */    3,
    /* Attributes */    {
                          { "atc",        2 | VET_TexCoord},
                          { "pos",        4 | VET_Pos},
                          { "vbatch",     1 | VET_Instance8},
                          },
    
    };

static VertexShaderDesc ShaderDesc_VS_VInstancedSolid = {
    /* Type */          VertexShaderDesc::VS_VInstancedSolid,
    /* Flags */         Shader_Instanced,
    /* pSource */       pSource_VInstancedSolid,
    /* Uniforms */      { 
                          /* cxadd*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      { -1, 0, 0, 0, 0, 0 },
                          /* mvp*/        { -1, 0, 0, 0, 0, 0 },
                          /* offset*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor2*/    { -1, 0, 0, 0, 0, 0 },
                          /* srctex*/     { -1, 0, 0, 0, 0, 0 },
                          /* srctexscale*/{ -1, 0, 0, 0, 0, 0 },
                          /* tex*/        { -1, 0, 0, 0, 0, 0 },
                          /* texgen*/     { -1, 0, 0, 0, 0, 0 },
                          /* texscale*/   { -1, 0, 0, 0, 0, 0 },
                          /* vfmuniforms*/{ -1, 0, 0, 0, 0, 0 },
                          /* vfuniforms*/ {0, 0, 4, 192, 4, 2 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_vfuniforms, 0, 2},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_Count, -1, 0},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    /* NumAttribs */    1,
    /* Attributes */    {
                          { "pos",        4 | VET_Pos},
                          },
    
    };

static VertexShaderDesc ShaderDesc_VS_VInstancedText = {
    /* Type */          VertexShaderDesc::VS_VInstancedText,
    /* Flags */         Shader_Instanced,
    /* pSource */       pSource_VInstancedText,
    /* Uniforms */      { 
                          /* cxadd*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      { -1, 0, 0, 0, 0, 0 },
                          /* mvp*/        { -1, 0, 0, 0, 0, 0 },
                          /* offset*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor2*/    { -1, 0, 0, 0, 0, 0 },
                          /* srctex*/     { -1, 0, 0, 0, 0, 0 },
                          /* srctexscale*/{ -1, 0, 0, 0, 0, 0 },
                          /* tex*/        { -1, 0, 0, 0, 0, 0 },
                          /* texgen*/     { -1, 0, 0, 0, 0, 0 },
                          /* texscale*/   { -1, 0, 0, 0, 0, 0 },
                          /* vfmuniforms*/{ -1, 0, 0, 0, 0, 0 },
                          /* vfuniforms*/ {0, 0, 4, 384, 4, 4 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_vfuniforms, 0, 1},
                          /* cxmul*/       {Uniform::SU_vfuniforms, 1, 1},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_vfuniforms, 2, 2},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_Count, -1, 0},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    /* NumAttribs */    3,
    /* Attributes */    {
                          { "acolor",     4 | VET_Color},
                          { "atc",        2 | VET_TexCoord},
                          { "pos",        4 | VET_Pos},
                          },
    
    };

static VertexShaderDesc ShaderDesc_VS_VInstancedTextColor = {
    /* Type */          VertexShaderDesc::VS_VInstancedTextColor,
    /* Flags */         Shader_Instanced,
    /* pSource */       pSource_VInstancedTextColor,
    /* Uniforms */      { 
                          /* cxadd*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      { -1, 0, 0, 0, 0, 0 },
                          /* mvp*/        { -1, 0, 0, 0, 0, 0 },
                          /* offset*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor2*/    { -1, 0, 0, 0, 0, 0 },
                          /* srctex*/     { -1, 0, 0, 0, 0, 0 },
                          /* srctexscale*/{ -1, 0, 0, 0, 0, 0 },
                          /* tex*/        { -1, 0, 0, 0, 0, 0 },
                          /* texgen*/     { -1, 0, 0, 0, 0, 0 },
                          /* texscale*/   { -1, 0, 0, 0, 0, 0 },
                          /* vfmuniforms*/{ -1, 0, 0, 0, 0, 0 },
                          /* vfuniforms*/ {0, 0, 4, 192, 4, 2 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_vfuniforms, 0, 2},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_Count, -1, 0},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    /* NumAttribs */    2,
    /* Attributes */    {
                          { "atc",        2 | VET_TexCoord},
                          { "pos",        4 | VET_Pos},
                          },
    
    };

static VertexShaderDesc ShaderDesc_VS_VInstancedTextColorCxform = {
    /* Type */          VertexShaderDesc::VS_VInstancedTextColorCxform,
    /* Flags */         Shader_Instanced,
    /* pSource */       pSource_VInstancedTextColorCxform,
    /* Uniforms */      { 
                          /* cxadd*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      { -1, 0, 0, 0, 0, 0 },
                          /* mvp*/        { -1, 0, 0, 0, 0, 0 },
                          /* offset*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor2*/    { -1, 0, 0, 0, 0, 0 },
                          /* srctex*/     { -1, 0, 0, 0, 0, 0 },
                          /* srctexscale*/{ -1, 0, 0, 0, 0, 0 },
                          /* tex*/        { -1, 0, 0, 0, 0, 0 },
                          /* texgen*/     { -1, 0, 0, 0, 0, 0 },
                          /* texscale*/   { -1, 0, 0, 0, 0, 0 },
                          /* vfmuniforms*/{ -1, 0, 0, 0, 0, 0 },
                          /* vfuniforms*/ {0, 0, 4, 384, 4, 4 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_vfuniforms, 0, 1},
                          /* cxmul*/       {Uniform::SU_vfuniforms, 1, 1},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_vfuniforms, 2, 2},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_Count, -1, 0},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    /* NumAttribs */    2,
    /* Attributes */    {
                          { "atc",        2 | VET_TexCoord},
                          { "pos",        4 | VET_Pos},
                          },
    
    };

static VertexShaderDesc ShaderDesc_VS_VInstancedPosition3dSolid = {
    /* Type */          VertexShaderDesc::VS_VInstancedPosition3dSolid,
    /* Flags */         Shader_Instanced,
    /* pSource */       pSource_VInstancedPosition3dSolid,
    /* Uniforms */      { 
                          /* cxadd*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      { -1, 0, 0, 0, 0, 0 },
                          /* mvp*/        { -1, 0, 0, 0, 0, 0 },
                          /* offset*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor2*/    { -1, 0, 0, 0, 0, 0 },
                          /* srctex*/     { -1, 0, 0, 0, 0, 0 },
                          /* srctexscale*/{ -1, 0, 0, 0, 0, 0 },
                          /* tex*/        { -1, 0, 0, 0, 0, 0 },
                          /* texgen*/     { -1, 0, 0, 0, 0, 0 },
                          /* texscale*/   { -1, 0, 0, 0, 0, 0 },
                          /* vfmuniforms*/{0, 0, 16, 384, 16, 1 },
                          /* vfuniforms*/ { -1, 0, 0, 0, 0, 0 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_vfmuniforms, 0, 1},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_Count, -1, 0},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    /* NumAttribs */    1,
    /* Attributes */    {
                          { "pos",        4 | VET_Pos},
                          },
    
    };

static VertexShaderDesc ShaderDesc_VS_VInstancedPosition3dText = {
    /* Type */          VertexShaderDesc::VS_VInstancedPosition3dText,
    /* Flags */         Shader_Instanced,
    /* pSource */       pSource_VInstancedPosition3dText,
    /* Uniforms */      { 
                          /* cxadd*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      { -1, 0, 0, 0, 0, 0 },
                          /* mvp*/        { -1, 0, 0, 0, 0, 0 },
                          /* offset*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor2*/    { -1, 0, 0, 0, 0, 0 },
                          /* srctex*/     { -1, 0, 0, 0, 0, 0 },
                          /* srctexscale*/{ -1, 0, 0, 0, 0, 0 },
                          /* tex*/        { -1, 0, 0, 0, 0, 0 },
                          /* texgen*/     { -1, 0, 0, 0, 0, 0 },
                          /* texscale*/   { -1, 0, 0, 0, 0, 0 },
                          /* vfmuniforms*/{0, 0, 16, 384, 16, 1 },
                          /* vfuniforms*/ {96, 384, 4, 192, 4, 2 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_vfuniforms, 0, 1},
                          /* cxmul*/       {Uniform::SU_vfuniforms, 1, 1},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_vfmuniforms, 0, 1},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_Count, -1, 0},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    /* NumAttribs */    3,
    /* Attributes */    {
                          { "acolor",     4 | VET_Color},
                          { "atc",        2 | VET_TexCoord},
                          { "pos",        4 | VET_Pos},
                          },
    
    };

static VertexShaderDesc ShaderDesc_VS_VInstancedPosition3dTextColor = {
    /* Type */          VertexShaderDesc::VS_VInstancedPosition3dTextColor,
    /* Flags */         Shader_Instanced,
    /* pSource */       pSource_VInstancedPosition3dTextColor,
    /* Uniforms */      { 
                          /* cxadd*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      { -1, 0, 0, 0, 0, 0 },
                          /* mvp*/        { -1, 0, 0, 0, 0, 0 },
                          /* offset*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor2*/    { -1, 0, 0, 0, 0, 0 },
                          /* srctex*/     { -1, 0, 0, 0, 0, 0 },
                          /* srctexscale*/{ -1, 0, 0, 0, 0, 0 },
                          /* tex*/        { -1, 0, 0, 0, 0, 0 },
                          /* texgen*/     { -1, 0, 0, 0, 0, 0 },
                          /* texscale*/   { -1, 0, 0, 0, 0, 0 },
                          /* vfmuniforms*/{0, 0, 16, 384, 16, 1 },
                          /* vfuniforms*/ { -1, 0, 0, 0, 0, 0 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_vfmuniforms, 0, 1},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_Count, -1, 0},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    /* NumAttribs */    2,
    /* Attributes */    {
                          { "atc",        2 | VET_TexCoord},
                          { "pos",        4 | VET_Pos},
                          },
    
    };

static VertexShaderDesc ShaderDesc_VS_VInstancedPosition3dTextColorCxform = {
    /* Type */          VertexShaderDesc::VS_VInstancedPosition3dTextColorCxform,
    /* Flags */         Shader_Instanced,
    /* pSource */       pSource_VInstancedPosition3dTextColorCxform,
    /* Uniforms */      { 
                          /* cxadd*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      { -1, 0, 0, 0, 0, 0 },
                          /* mvp*/        { -1, 0, 0, 0, 0, 0 },
                          /* offset*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor2*/    { -1, 0, 0, 0, 0, 0 },
                          /* srctex*/     { -1, 0, 0, 0, 0, 0 },
                          /* srctexscale*/{ -1, 0, 0, 0, 0, 0 },
                          /* tex*/        { -1, 0, 0, 0, 0, 0 },
                          /* texgen*/     { -1, 0, 0, 0, 0, 0 },
                          /* texscale*/   { -1, 0, 0, 0, 0, 0 },
                          /* vfmuniforms*/{0, 0, 16, 384, 16, 1 },
                          /* vfuniforms*/ {96, 384, 4, 192, 4, 2 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_vfuniforms, 0, 1},
                          /* cxmul*/       {Uniform::SU_vfuniforms, 1, 1},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_vfmuniforms, 0, 1},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_Count, -1, 0},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    /* NumAttribs */    2,
    /* Attributes */    {
                          { "atc",        2 | VET_TexCoord},
                          { "pos",        4 | VET_Pos},
                          },
    
    };

static VertexShaderDesc ShaderDesc_VS_VBox1Blur = {
    /* Type */          VertexShaderDesc::VS_VBox1Blur,
    /* Flags */         Shader_DynamicLoop,
    /* pSource */       pSource_VTexTGCxform,
    /* Uniforms */      { 
                          /* cxadd*/      {0, 0, 4, 4, 4, 0 },
                          /* cxmul*/      {1, 4, 4, 4, 4, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      { -1, 0, 0, 0, 0, 0 },
                          /* mvp*/        {2, 8, 4, 8, 4, 0 },
                          /* offset*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor2*/    { -1, 0, 0, 0, 0, 0 },
                          /* srctex*/     { -1, 0, 0, 0, 0, 0 },
                          /* srctexscale*/{ -1, 0, 0, 0, 0, 0 },
                          /* tex*/        { -1, 0, 0, 0, 0, 0 },
                          /* texgen*/     {4, 16, 4, 8, 4, 0 },
                          /* texscale*/   { -1, 0, 0, 0, 0, 0 },
                          /* vfmuniforms*/{ -1, 0, 0, 0, 0, 0 },
                          /* vfuniforms*/ { -1, 0, 0, 0, 0, 0 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_Count, -1, 0},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_Count, -1, 0},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    /* NumAttribs */    1,
    /* Attributes */    {
                          { "pos",        4 | VET_Pos},
                          },
    
    };

static VertexShaderDesc ShaderDesc_VS_VDrawableCopyPixels = {
    /* Type */          VertexShaderDesc::VS_VDrawableCopyPixels,
    /* Flags */         0,
    /* pSource */       pSource_VDrawableCopyPixels,
    /* Uniforms */      { 
                          /* cxadd*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      { -1, 0, 0, 0, 0, 0 },
                          /* mvp*/        {0, 0, 4, 8, 4, 0 },
                          /* offset*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor2*/    { -1, 0, 0, 0, 0, 0 },
                          /* srctex*/     { -1, 0, 0, 0, 0, 0 },
                          /* srctexscale*/{ -1, 0, 0, 0, 0, 0 },
                          /* tex*/        { -1, 0, 0, 0, 0, 0 },
                          /* texgen*/     {2, 8, 4, 16, 4, 0 },
                          /* texscale*/   { -1, 0, 0, 0, 0, 0 },
                          /* vfmuniforms*/{ -1, 0, 0, 0, 0, 0 },
                          /* vfuniforms*/ { -1, 0, 0, 0, 0, 0 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_Count, -1, 0},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_Count, -1, 0},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    /* NumAttribs */    1,
    /* Attributes */    {
                          { "pos",        4 | VET_Pos},
                          },
    
    };

static VertexShaderDesc ShaderDesc_VS_VDrawableCopyPixelsAlpha = {
    /* Type */          VertexShaderDesc::VS_VDrawableCopyPixelsAlpha,
    /* Flags */         0,
    /* pSource */       pSource_VDrawableCopyPixelsAlpha,
    /* Uniforms */      { 
                          /* cxadd*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      { -1, 0, 0, 0, 0, 0 },
                          /* mvp*/        {0, 0, 4, 8, 4, 0 },
                          /* offset*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor2*/    { -1, 0, 0, 0, 0, 0 },
                          /* srctex*/     { -1, 0, 0, 0, 0, 0 },
                          /* srctexscale*/{ -1, 0, 0, 0, 0, 0 },
                          /* tex*/        { -1, 0, 0, 0, 0, 0 },
                          /* texgen*/     {2, 8, 4, 24, 4, 0 },
                          /* texscale*/   { -1, 0, 0, 0, 0, 0 },
                          /* vfmuniforms*/{ -1, 0, 0, 0, 0, 0 },
                          /* vfuniforms*/ { -1, 0, 0, 0, 0, 0 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_Count, -1, 0},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_Count, -1, 0},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    /* NumAttribs */    1,
    /* Attributes */    {
                          { "pos",        4 | VET_Pos},
                          },
    
    };

const VertexShaderDesc* VertexShaderDesc::Descs[VSI_Count] = {
    0,
    &ShaderDesc_VS_VTexTG,                                       // 1
    &ShaderDesc_VS_VTexTGEAlpha,                                 // 2
    &ShaderDesc_VS_VTexTGCxform,                                 // 3
    &ShaderDesc_VS_VTexTGCxformEAlpha,                           // 4
    &ShaderDesc_VS_VVertex,                                      // 5
    &ShaderDesc_VS_VVertexEAlpha,                                // 6
    &ShaderDesc_VS_VVertexCxform,                                // 7
    &ShaderDesc_VS_VVertexCxformEAlpha,                          // 8
    &ShaderDesc_VS_VTexTGTexTG,                                  // 9
    &ShaderDesc_VS_VTexTGTexTGCxform,                            // 10
    &ShaderDesc_VS_VTexTGVertex,                                 // 11
    &ShaderDesc_VS_VTexTGVertexCxform,                           // 12
    &ShaderDesc_VS_VPosition3dTexTG,                             // 13
    &ShaderDesc_VS_VPosition3dTexTGEAlpha,                       // 14
    &ShaderDesc_VS_VPosition3dTexTGCxform,                       // 15
    &ShaderDesc_VS_VPosition3dTexTGCxformEAlpha,                 // 16
    &ShaderDesc_VS_VPosition3dVertex,                            // 17
    &ShaderDesc_VS_VPosition3dVertexEAlpha,                      // 18
    &ShaderDesc_VS_VPosition3dVertexCxform,                      // 19
    &ShaderDesc_VS_VPosition3dVertexCxformEAlpha,                // 20
    &ShaderDesc_VS_VPosition3dTexTGTexTG,                        // 21
    &ShaderDesc_VS_VPosition3dTexTGTexTGCxform,                  // 22
    &ShaderDesc_VS_VPosition3dTexTGVertex,                       // 23
    &ShaderDesc_VS_VPosition3dTexTGVertexCxform,                 // 24
    &ShaderDesc_VS_VBatchTexTG,                                  // 25
    &ShaderDesc_VS_VBatchTexTGEAlpha,                            // 26
    &ShaderDesc_VS_VBatchTexTGCxform,                            // 27
    &ShaderDesc_VS_VBatchTexTGCxformEAlpha,                      // 28
    &ShaderDesc_VS_VBatchVertex,                                 // 29
    &ShaderDesc_VS_VBatchVertexEAlpha,                           // 30
    &ShaderDesc_VS_VBatchVertexCxform,                           // 31
    &ShaderDesc_VS_VBatchVertexCxformEAlpha,                     // 32
    &ShaderDesc_VS_VBatchTexTGTexTG,                             // 33
    &ShaderDesc_VS_VBatchTexTGTexTGCxform,                       // 34
    &ShaderDesc_VS_VBatchTexTGVertex,                            // 35
    &ShaderDesc_VS_VBatchTexTGVertexCxform,                      // 36
    &ShaderDesc_VS_VBatchPosition3dTexTG,                        // 37
    &ShaderDesc_VS_VBatchPosition3dTexTGEAlpha,                  // 38
    &ShaderDesc_VS_VBatchPosition3dTexTGCxform,                  // 39
    &ShaderDesc_VS_VBatchPosition3dTexTGCxformEAlpha,            // 40
    &ShaderDesc_VS_VBatchPosition3dVertex,                       // 41
    &ShaderDesc_VS_VBatchPosition3dVertexEAlpha,                 // 42
    &ShaderDesc_VS_VBatchPosition3dVertexCxform,                 // 43
    &ShaderDesc_VS_VBatchPosition3dVertexCxformEAlpha,           // 44
    &ShaderDesc_VS_VBatchPosition3dTexTGTexTG,                   // 45
    &ShaderDesc_VS_VBatchPosition3dTexTGTexTGCxform,             // 46
    &ShaderDesc_VS_VBatchPosition3dTexTGVertex,                  // 47
    &ShaderDesc_VS_VBatchPosition3dTexTGVertexCxform,            // 48
    &ShaderDesc_VS_VInstancedTexTG,                              // 49
    &ShaderDesc_VS_VInstancedTexTGEAlpha,                        // 50
    &ShaderDesc_VS_VInstancedTexTGCxform,                        // 51
    &ShaderDesc_VS_VInstancedTexTGCxformEAlpha,                  // 52
    &ShaderDesc_VS_VInstancedVertex,                             // 53
    &ShaderDesc_VS_VInstancedVertexEAlpha,                       // 54
    &ShaderDesc_VS_VInstancedVertexCxform,                       // 55
    &ShaderDesc_VS_VInstancedVertexCxformEAlpha,                 // 56
    &ShaderDesc_VS_VInstancedTexTGTexTG,                         // 57
    &ShaderDesc_VS_VInstancedTexTGTexTGCxform,                   // 58
    &ShaderDesc_VS_VInstancedTexTGVertex,                        // 59
    &ShaderDesc_VS_VInstancedTexTGVertexCxform,                  // 60
    &ShaderDesc_VS_VInstancedPosition3dTexTG,                    // 61
    &ShaderDesc_VS_VInstancedPosition3dTexTGEAlpha,              // 62
    &ShaderDesc_VS_VInstancedPosition3dTexTGCxform,              // 63
    &ShaderDesc_VS_VInstancedPosition3dTexTGCxformEAlpha,        // 64
    &ShaderDesc_VS_VInstancedPosition3dVertex,                   // 65
    &ShaderDesc_VS_VInstancedPosition3dVertexEAlpha,             // 66
    &ShaderDesc_VS_VInstancedPosition3dVertexCxform,             // 67
    &ShaderDesc_VS_VInstancedPosition3dVertexCxformEAlpha,       // 68
    &ShaderDesc_VS_VInstancedPosition3dTexTGTexTG,               // 69
    &ShaderDesc_VS_VInstancedPosition3dTexTGTexTGCxform,         // 70
    &ShaderDesc_VS_VInstancedPosition3dTexTGVertex,              // 71
    &ShaderDesc_VS_VInstancedPosition3dTexTGVertexCxform,        // 72
    &ShaderDesc_VS_VSolid,                                       // 73
    &ShaderDesc_VS_VText,                                        // 74
    &ShaderDesc_VS_VTextColor,                                   // 75
    &ShaderDesc_VS_VTextColorCxform,                             // 76
    &ShaderDesc_VS_VPosition3dSolid,                             // 77
    &ShaderDesc_VS_VPosition3dText,                              // 78
    &ShaderDesc_VS_VPosition3dTextColor,                         // 79
    &ShaderDesc_VS_VPosition3dTextColorCxform,                   // 80
    &ShaderDesc_VS_VBatchSolid,                                  // 81
    &ShaderDesc_VS_VBatchText,                                   // 82
    &ShaderDesc_VS_VBatchTextColor,                              // 83
    &ShaderDesc_VS_VBatchTextColorCxform,                        // 84
    &ShaderDesc_VS_VBatchPosition3dSolid,                        // 85
    &ShaderDesc_VS_VBatchPosition3dText,                         // 86
    &ShaderDesc_VS_VBatchPosition3dTextColor,                    // 87
    &ShaderDesc_VS_VBatchPosition3dTextColorCxform,              // 88
    &ShaderDesc_VS_VInstancedSolid,                              // 89
    &ShaderDesc_VS_VInstancedText,                               // 90
    &ShaderDesc_VS_VInstancedTextColor,                          // 91
    &ShaderDesc_VS_VInstancedTextColorCxform,                    // 92
    &ShaderDesc_VS_VInstancedPosition3dSolid,                    // 93
    &ShaderDesc_VS_VInstancedPosition3dText,                     // 94
    &ShaderDesc_VS_VInstancedPosition3dTextColor,                // 95
    &ShaderDesc_VS_VInstancedPosition3dTextColorCxform,          // 96
    &ShaderDesc_VS_VBox1Blur,                                    // 97
    &ShaderDesc_VS_VDrawableCopyPixels,                          // 98
    &ShaderDesc_VS_VDrawableCopyPixelsAlpha,                     // 99
};

const VertexShaderDesc* VertexShaderDesc::GetDesc(ShaderType shader)
{
    return Descs[GetShaderIndex(shader)];
};

VertexShaderDesc::ShaderIndex VertexShaderDesc::GetShaderIndex(ShaderType shader)
{
switch(shader)
{
    case VS_VTexTG:                                                  return VSI_VTexTG;
    case VS_VTexTGMul:                                               return VSI_VTexTGMul;
    case VS_VBatchTexTG:                                             return VSI_VBatchTexTG;
    case VS_VBatchTexTGMul:                                          return VSI_VBatchTexTGMul;
    case VS_VInstancedTexTG:                                         return VSI_VInstancedTexTG;
    case VS_VInstancedTexTGMul:                                      return VSI_VInstancedTexTGMul;
    case VS_VPosition3dTexTG:                                        return VSI_VPosition3dTexTG;
    case VS_VPosition3dTexTGMul:                                     return VSI_VPosition3dTexTGMul;
    case VS_VBatchPosition3dTexTG:                                   return VSI_VBatchPosition3dTexTG;
    case VS_VBatchPosition3dTexTGMul:                                return VSI_VBatchPosition3dTexTGMul;
    case VS_VInstancedPosition3dTexTG:                               return VSI_VInstancedPosition3dTexTG;
    case VS_VInstancedPosition3dTexTGMul:                            return VSI_VInstancedPosition3dTexTGMul;
    case VS_VTexTGCxform:                                            return VSI_VTexTGCxform;
    case VS_VTexTGCxformMul:                                         return VSI_VTexTGCxformMul;
    case VS_VBatchTexTGCxform:                                       return VSI_VBatchTexTGCxform;
    case VS_VBatchTexTGCxformMul:                                    return VSI_VBatchTexTGCxformMul;
    case VS_VInstancedTexTGCxform:                                   return VSI_VInstancedTexTGCxform;
    case VS_VInstancedTexTGCxformMul:                                return VSI_VInstancedTexTGCxformMul;
    case VS_VPosition3dTexTGCxform:                                  return VSI_VPosition3dTexTGCxform;
    case VS_VPosition3dTexTGCxformMul:                               return VSI_VPosition3dTexTGCxformMul;
    case VS_VBatchPosition3dTexTGCxform:                             return VSI_VBatchPosition3dTexTGCxform;
    case VS_VBatchPosition3dTexTGCxformMul:                          return VSI_VBatchPosition3dTexTGCxformMul;
    case VS_VInstancedPosition3dTexTGCxform:                         return VSI_VInstancedPosition3dTexTGCxform;
    case VS_VInstancedPosition3dTexTGCxformMul:                      return VSI_VInstancedPosition3dTexTGCxformMul;
    case VS_VTexTGEAlpha:                                            return VSI_VTexTGEAlpha;
    case VS_VTexTGEAlphaMul:                                         return VSI_VTexTGEAlphaMul;
    case VS_VBatchTexTGEAlpha:                                       return VSI_VBatchTexTGEAlpha;
    case VS_VBatchTexTGEAlphaMul:                                    return VSI_VBatchTexTGEAlphaMul;
    case VS_VInstancedTexTGEAlpha:                                   return VSI_VInstancedTexTGEAlpha;
    case VS_VInstancedTexTGEAlphaMul:                                return VSI_VInstancedTexTGEAlphaMul;
    case VS_VPosition3dTexTGEAlpha:                                  return VSI_VPosition3dTexTGEAlpha;
    case VS_VPosition3dTexTGEAlphaMul:                               return VSI_VPosition3dTexTGEAlphaMul;
    case VS_VBatchPosition3dTexTGEAlpha:                             return VSI_VBatchPosition3dTexTGEAlpha;
    case VS_VBatchPosition3dTexTGEAlphaMul:                          return VSI_VBatchPosition3dTexTGEAlphaMul;
    case VS_VInstancedPosition3dTexTGEAlpha:                         return VSI_VInstancedPosition3dTexTGEAlpha;
    case VS_VInstancedPosition3dTexTGEAlphaMul:                      return VSI_VInstancedPosition3dTexTGEAlphaMul;
    case VS_VTexTGCxformEAlpha:                                      return VSI_VTexTGCxformEAlpha;
    case VS_VTexTGCxformEAlphaMul:                                   return VSI_VTexTGCxformEAlphaMul;
    case VS_VBatchTexTGCxformEAlpha:                                 return VSI_VBatchTexTGCxformEAlpha;
    case VS_VBatchTexTGCxformEAlphaMul:                              return VSI_VBatchTexTGCxformEAlphaMul;
    case VS_VInstancedTexTGCxformEAlpha:                             return VSI_VInstancedTexTGCxformEAlpha;
    case VS_VInstancedTexTGCxformEAlphaMul:                          return VSI_VInstancedTexTGCxformEAlphaMul;
    case VS_VPosition3dTexTGCxformEAlpha:                            return VSI_VPosition3dTexTGCxformEAlpha;
    case VS_VPosition3dTexTGCxformEAlphaMul:                         return VSI_VPosition3dTexTGCxformEAlphaMul;
    case VS_VBatchPosition3dTexTGCxformEAlpha:                       return VSI_VBatchPosition3dTexTGCxformEAlpha;
    case VS_VBatchPosition3dTexTGCxformEAlphaMul:                    return VSI_VBatchPosition3dTexTGCxformEAlphaMul;
    case VS_VInstancedPosition3dTexTGCxformEAlpha:                   return VSI_VInstancedPosition3dTexTGCxformEAlpha;
    case VS_VInstancedPosition3dTexTGCxformEAlphaMul:                return VSI_VInstancedPosition3dTexTGCxformEAlphaMul;
    case VS_VVertex:                                                 return VSI_VVertex;
    case VS_VVertexMul:                                              return VSI_VVertexMul;
    case VS_VBatchVertex:                                            return VSI_VBatchVertex;
    case VS_VBatchVertexMul:                                         return VSI_VBatchVertexMul;
    case VS_VInstancedVertex:                                        return VSI_VInstancedVertex;
    case VS_VInstancedVertexMul:                                     return VSI_VInstancedVertexMul;
    case VS_VPosition3dVertex:                                       return VSI_VPosition3dVertex;
    case VS_VPosition3dVertexMul:                                    return VSI_VPosition3dVertexMul;
    case VS_VBatchPosition3dVertex:                                  return VSI_VBatchPosition3dVertex;
    case VS_VBatchPosition3dVertexMul:                               return VSI_VBatchPosition3dVertexMul;
    case VS_VInstancedPosition3dVertex:                              return VSI_VInstancedPosition3dVertex;
    case VS_VInstancedPosition3dVertexMul:                           return VSI_VInstancedPosition3dVertexMul;
    case VS_VVertexCxform:                                           return VSI_VVertexCxform;
    case VS_VVertexCxformMul:                                        return VSI_VVertexCxformMul;
    case VS_VBatchVertexCxform:                                      return VSI_VBatchVertexCxform;
    case VS_VBatchVertexCxformMul:                                   return VSI_VBatchVertexCxformMul;
    case VS_VInstancedVertexCxform:                                  return VSI_VInstancedVertexCxform;
    case VS_VInstancedVertexCxformMul:                               return VSI_VInstancedVertexCxformMul;
    case VS_VPosition3dVertexCxform:                                 return VSI_VPosition3dVertexCxform;
    case VS_VPosition3dVertexCxformMul:                              return VSI_VPosition3dVertexCxformMul;
    case VS_VBatchPosition3dVertexCxform:                            return VSI_VBatchPosition3dVertexCxform;
    case VS_VBatchPosition3dVertexCxformMul:                         return VSI_VBatchPosition3dVertexCxformMul;
    case VS_VInstancedPosition3dVertexCxform:                        return VSI_VInstancedPosition3dVertexCxform;
    case VS_VInstancedPosition3dVertexCxformMul:                     return VSI_VInstancedPosition3dVertexCxformMul;
    case VS_VVertexEAlpha:                                           return VSI_VVertexEAlpha;
    case VS_VVertexEAlphaMul:                                        return VSI_VVertexEAlphaMul;
    case VS_VBatchVertexEAlpha:                                      return VSI_VBatchVertexEAlpha;
    case VS_VBatchVertexEAlphaMul:                                   return VSI_VBatchVertexEAlphaMul;
    case VS_VInstancedVertexEAlpha:                                  return VSI_VInstancedVertexEAlpha;
    case VS_VInstancedVertexEAlphaMul:                               return VSI_VInstancedVertexEAlphaMul;
    case VS_VPosition3dVertexEAlpha:                                 return VSI_VPosition3dVertexEAlpha;
    case VS_VPosition3dVertexEAlphaMul:                              return VSI_VPosition3dVertexEAlphaMul;
    case VS_VBatchPosition3dVertexEAlpha:                            return VSI_VBatchPosition3dVertexEAlpha;
    case VS_VBatchPosition3dVertexEAlphaMul:                         return VSI_VBatchPosition3dVertexEAlphaMul;
    case VS_VInstancedPosition3dVertexEAlpha:                        return VSI_VInstancedPosition3dVertexEAlpha;
    case VS_VInstancedPosition3dVertexEAlphaMul:                     return VSI_VInstancedPosition3dVertexEAlphaMul;
    case VS_VVertexCxformEAlpha:                                     return VSI_VVertexCxformEAlpha;
    case VS_VVertexCxformEAlphaMul:                                  return VSI_VVertexCxformEAlphaMul;
    case VS_VBatchVertexCxformEAlpha:                                return VSI_VBatchVertexCxformEAlpha;
    case VS_VBatchVertexCxformEAlphaMul:                             return VSI_VBatchVertexCxformEAlphaMul;
    case VS_VInstancedVertexCxformEAlpha:                            return VSI_VInstancedVertexCxformEAlpha;
    case VS_VInstancedVertexCxformEAlphaMul:                         return VSI_VInstancedVertexCxformEAlphaMul;
    case VS_VPosition3dVertexCxformEAlpha:                           return VSI_VPosition3dVertexCxformEAlpha;
    case VS_VPosition3dVertexCxformEAlphaMul:                        return VSI_VPosition3dVertexCxformEAlphaMul;
    case VS_VBatchPosition3dVertexCxformEAlpha:                      return VSI_VBatchPosition3dVertexCxformEAlpha;
    case VS_VBatchPosition3dVertexCxformEAlphaMul:                   return VSI_VBatchPosition3dVertexCxformEAlphaMul;
    case VS_VInstancedPosition3dVertexCxformEAlpha:                  return VSI_VInstancedPosition3dVertexCxformEAlpha;
    case VS_VInstancedPosition3dVertexCxformEAlphaMul:               return VSI_VInstancedPosition3dVertexCxformEAlphaMul;
    case VS_VTexTGTexTG:                                             return VSI_VTexTGTexTG;
    case VS_VTexTGTexTGMul:                                          return VSI_VTexTGTexTGMul;
    case VS_VBatchTexTGTexTG:                                        return VSI_VBatchTexTGTexTG;
    case VS_VBatchTexTGTexTGMul:                                     return VSI_VBatchTexTGTexTGMul;
    case VS_VInstancedTexTGTexTG:                                    return VSI_VInstancedTexTGTexTG;
    case VS_VInstancedTexTGTexTGMul:                                 return VSI_VInstancedTexTGTexTGMul;
    case VS_VPosition3dTexTGTexTG:                                   return VSI_VPosition3dTexTGTexTG;
    case VS_VPosition3dTexTGTexTGMul:                                return VSI_VPosition3dTexTGTexTGMul;
    case VS_VBatchPosition3dTexTGTexTG:                              return VSI_VBatchPosition3dTexTGTexTG;
    case VS_VBatchPosition3dTexTGTexTGMul:                           return VSI_VBatchPosition3dTexTGTexTGMul;
    case VS_VInstancedPosition3dTexTGTexTG:                          return VSI_VInstancedPosition3dTexTGTexTG;
    case VS_VInstancedPosition3dTexTGTexTGMul:                       return VSI_VInstancedPosition3dTexTGTexTGMul;
    case VS_VTexTGTexTGCxform:                                       return VSI_VTexTGTexTGCxform;
    case VS_VTexTGTexTGCxformMul:                                    return VSI_VTexTGTexTGCxformMul;
    case VS_VBatchTexTGTexTGCxform:                                  return VSI_VBatchTexTGTexTGCxform;
    case VS_VBatchTexTGTexTGCxformMul:                               return VSI_VBatchTexTGTexTGCxformMul;
    case VS_VInstancedTexTGTexTGCxform:                              return VSI_VInstancedTexTGTexTGCxform;
    case VS_VInstancedTexTGTexTGCxformMul:                           return VSI_VInstancedTexTGTexTGCxformMul;
    case VS_VPosition3dTexTGTexTGCxform:                             return VSI_VPosition3dTexTGTexTGCxform;
    case VS_VPosition3dTexTGTexTGCxformMul:                          return VSI_VPosition3dTexTGTexTGCxformMul;
    case VS_VBatchPosition3dTexTGTexTGCxform:                        return VSI_VBatchPosition3dTexTGTexTGCxform;
    case VS_VBatchPosition3dTexTGTexTGCxformMul:                     return VSI_VBatchPosition3dTexTGTexTGCxformMul;
    case VS_VInstancedPosition3dTexTGTexTGCxform:                    return VSI_VInstancedPosition3dTexTGTexTGCxform;
    case VS_VInstancedPosition3dTexTGTexTGCxformMul:                 return VSI_VInstancedPosition3dTexTGTexTGCxformMul;
    case VS_VTexTGTexTGEAlpha:                                       return VSI_VTexTGTexTGEAlpha;
    case VS_VTexTGTexTGEAlphaMul:                                    return VSI_VTexTGTexTGEAlphaMul;
    case VS_VBatchTexTGTexTGEAlpha:                                  return VSI_VBatchTexTGTexTGEAlpha;
    case VS_VBatchTexTGTexTGEAlphaMul:                               return VSI_VBatchTexTGTexTGEAlphaMul;
    case VS_VInstancedTexTGTexTGEAlpha:                              return VSI_VInstancedTexTGTexTGEAlpha;
    case VS_VInstancedTexTGTexTGEAlphaMul:                           return VSI_VInstancedTexTGTexTGEAlphaMul;
    case VS_VPosition3dTexTGTexTGEAlpha:                             return VSI_VPosition3dTexTGTexTGEAlpha;
    case VS_VPosition3dTexTGTexTGEAlphaMul:                          return VSI_VPosition3dTexTGTexTGEAlphaMul;
    case VS_VBatchPosition3dTexTGTexTGEAlpha:                        return VSI_VBatchPosition3dTexTGTexTGEAlpha;
    case VS_VBatchPosition3dTexTGTexTGEAlphaMul:                     return VSI_VBatchPosition3dTexTGTexTGEAlphaMul;
    case VS_VInstancedPosition3dTexTGTexTGEAlpha:                    return VSI_VInstancedPosition3dTexTGTexTGEAlpha;
    case VS_VInstancedPosition3dTexTGTexTGEAlphaMul:                 return VSI_VInstancedPosition3dTexTGTexTGEAlphaMul;
    case VS_VTexTGTexTGCxformEAlpha:                                 return VSI_VTexTGTexTGCxformEAlpha;
    case VS_VTexTGTexTGCxformEAlphaMul:                              return VSI_VTexTGTexTGCxformEAlphaMul;
    case VS_VBatchTexTGTexTGCxformEAlpha:                            return VSI_VBatchTexTGTexTGCxformEAlpha;
    case VS_VBatchTexTGTexTGCxformEAlphaMul:                         return VSI_VBatchTexTGTexTGCxformEAlphaMul;
    case VS_VInstancedTexTGTexTGCxformEAlpha:                        return VSI_VInstancedTexTGTexTGCxformEAlpha;
    case VS_VInstancedTexTGTexTGCxformEAlphaMul:                     return VSI_VInstancedTexTGTexTGCxformEAlphaMul;
    case VS_VPosition3dTexTGTexTGCxformEAlpha:                       return VSI_VPosition3dTexTGTexTGCxformEAlpha;
    case VS_VPosition3dTexTGTexTGCxformEAlphaMul:                    return VSI_VPosition3dTexTGTexTGCxformEAlphaMul;
    case VS_VBatchPosition3dTexTGTexTGCxformEAlpha:                  return VSI_VBatchPosition3dTexTGTexTGCxformEAlpha;
    case VS_VBatchPosition3dTexTGTexTGCxformEAlphaMul:               return VSI_VBatchPosition3dTexTGTexTGCxformEAlphaMul;
    case VS_VInstancedPosition3dTexTGTexTGCxformEAlpha:              return VSI_VInstancedPosition3dTexTGTexTGCxformEAlpha;
    case VS_VInstancedPosition3dTexTGTexTGCxformEAlphaMul:           return VSI_VInstancedPosition3dTexTGTexTGCxformEAlphaMul;
    case VS_VTexTGVertex:                                            return VSI_VTexTGVertex;
    case VS_VTexTGVertexMul:                                         return VSI_VTexTGVertexMul;
    case VS_VBatchTexTGVertex:                                       return VSI_VBatchTexTGVertex;
    case VS_VBatchTexTGVertexMul:                                    return VSI_VBatchTexTGVertexMul;
    case VS_VInstancedTexTGVertex:                                   return VSI_VInstancedTexTGVertex;
    case VS_VInstancedTexTGVertexMul:                                return VSI_VInstancedTexTGVertexMul;
    case VS_VPosition3dTexTGVertex:                                  return VSI_VPosition3dTexTGVertex;
    case VS_VPosition3dTexTGVertexMul:                               return VSI_VPosition3dTexTGVertexMul;
    case VS_VBatchPosition3dTexTGVertex:                             return VSI_VBatchPosition3dTexTGVertex;
    case VS_VBatchPosition3dTexTGVertexMul:                          return VSI_VBatchPosition3dTexTGVertexMul;
    case VS_VInstancedPosition3dTexTGVertex:                         return VSI_VInstancedPosition3dTexTGVertex;
    case VS_VInstancedPosition3dTexTGVertexMul:                      return VSI_VInstancedPosition3dTexTGVertexMul;
    case VS_VTexTGVertexCxform:                                      return VSI_VTexTGVertexCxform;
    case VS_VTexTGVertexCxformMul:                                   return VSI_VTexTGVertexCxformMul;
    case VS_VBatchTexTGVertexCxform:                                 return VSI_VBatchTexTGVertexCxform;
    case VS_VBatchTexTGVertexCxformMul:                              return VSI_VBatchTexTGVertexCxformMul;
    case VS_VInstancedTexTGVertexCxform:                             return VSI_VInstancedTexTGVertexCxform;
    case VS_VInstancedTexTGVertexCxformMul:                          return VSI_VInstancedTexTGVertexCxformMul;
    case VS_VPosition3dTexTGVertexCxform:                            return VSI_VPosition3dTexTGVertexCxform;
    case VS_VPosition3dTexTGVertexCxformMul:                         return VSI_VPosition3dTexTGVertexCxformMul;
    case VS_VBatchPosition3dTexTGVertexCxform:                       return VSI_VBatchPosition3dTexTGVertexCxform;
    case VS_VBatchPosition3dTexTGVertexCxformMul:                    return VSI_VBatchPosition3dTexTGVertexCxformMul;
    case VS_VInstancedPosition3dTexTGVertexCxform:                   return VSI_VInstancedPosition3dTexTGVertexCxform;
    case VS_VInstancedPosition3dTexTGVertexCxformMul:                return VSI_VInstancedPosition3dTexTGVertexCxformMul;
    case VS_VTexTGVertexEAlpha:                                      return VSI_VTexTGVertexEAlpha;
    case VS_VTexTGVertexEAlphaMul:                                   return VSI_VTexTGVertexEAlphaMul;
    case VS_VBatchTexTGVertexEAlpha:                                 return VSI_VBatchTexTGVertexEAlpha;
    case VS_VBatchTexTGVertexEAlphaMul:                              return VSI_VBatchTexTGVertexEAlphaMul;
    case VS_VInstancedTexTGVertexEAlpha:                             return VSI_VInstancedTexTGVertexEAlpha;
    case VS_VInstancedTexTGVertexEAlphaMul:                          return VSI_VInstancedTexTGVertexEAlphaMul;
    case VS_VPosition3dTexTGVertexEAlpha:                            return VSI_VPosition3dTexTGVertexEAlpha;
    case VS_VPosition3dTexTGVertexEAlphaMul:                         return VSI_VPosition3dTexTGVertexEAlphaMul;
    case VS_VBatchPosition3dTexTGVertexEAlpha:                       return VSI_VBatchPosition3dTexTGVertexEAlpha;
    case VS_VBatchPosition3dTexTGVertexEAlphaMul:                    return VSI_VBatchPosition3dTexTGVertexEAlphaMul;
    case VS_VInstancedPosition3dTexTGVertexEAlpha:                   return VSI_VInstancedPosition3dTexTGVertexEAlpha;
    case VS_VInstancedPosition3dTexTGVertexEAlphaMul:                return VSI_VInstancedPosition3dTexTGVertexEAlphaMul;
    case VS_VTexTGVertexCxformEAlpha:                                return VSI_VTexTGVertexCxformEAlpha;
    case VS_VTexTGVertexCxformEAlphaMul:                             return VSI_VTexTGVertexCxformEAlphaMul;
    case VS_VBatchTexTGVertexCxformEAlpha:                           return VSI_VBatchTexTGVertexCxformEAlpha;
    case VS_VBatchTexTGVertexCxformEAlphaMul:                        return VSI_VBatchTexTGVertexCxformEAlphaMul;
    case VS_VInstancedTexTGVertexCxformEAlpha:                       return VSI_VInstancedTexTGVertexCxformEAlpha;
    case VS_VInstancedTexTGVertexCxformEAlphaMul:                    return VSI_VInstancedTexTGVertexCxformEAlphaMul;
    case VS_VPosition3dTexTGVertexCxformEAlpha:                      return VSI_VPosition3dTexTGVertexCxformEAlpha;
    case VS_VPosition3dTexTGVertexCxformEAlphaMul:                   return VSI_VPosition3dTexTGVertexCxformEAlphaMul;
    case VS_VBatchPosition3dTexTGVertexCxformEAlpha:                 return VSI_VBatchPosition3dTexTGVertexCxformEAlpha;
    case VS_VBatchPosition3dTexTGVertexCxformEAlphaMul:              return VSI_VBatchPosition3dTexTGVertexCxformEAlphaMul;
    case VS_VInstancedPosition3dTexTGVertexCxformEAlpha:             return VSI_VInstancedPosition3dTexTGVertexCxformEAlpha;
    case VS_VInstancedPosition3dTexTGVertexCxformEAlphaMul:          return VSI_VInstancedPosition3dTexTGVertexCxformEAlphaMul;
    case VS_VSolid:                                                  return VSI_VSolid;
    case VS_VSolidMul:                                               return VSI_VSolidMul;
    case VS_VBatchSolid:                                             return VSI_VBatchSolid;
    case VS_VBatchSolidMul:                                          return VSI_VBatchSolidMul;
    case VS_VInstancedSolid:                                         return VSI_VInstancedSolid;
    case VS_VInstancedSolidMul:                                      return VSI_VInstancedSolidMul;
    case VS_VPosition3dSolid:                                        return VSI_VPosition3dSolid;
    case VS_VPosition3dSolidMul:                                     return VSI_VPosition3dSolidMul;
    case VS_VBatchPosition3dSolid:                                   return VSI_VBatchPosition3dSolid;
    case VS_VBatchPosition3dSolidMul:                                return VSI_VBatchPosition3dSolidMul;
    case VS_VInstancedPosition3dSolid:                               return VSI_VInstancedPosition3dSolid;
    case VS_VInstancedPosition3dSolidMul:                            return VSI_VInstancedPosition3dSolidMul;
    case VS_VText:                                                   return VSI_VText;
    case VS_VTextMul:                                                return VSI_VTextMul;
    case VS_VBatchText:                                              return VSI_VBatchText;
    case VS_VBatchTextMul:                                           return VSI_VBatchTextMul;
    case VS_VInstancedText:                                          return VSI_VInstancedText;
    case VS_VInstancedTextMul:                                       return VSI_VInstancedTextMul;
    case VS_VPosition3dText:                                         return VSI_VPosition3dText;
    case VS_VPosition3dTextMul:                                      return VSI_VPosition3dTextMul;
    case VS_VBatchPosition3dText:                                    return VSI_VBatchPosition3dText;
    case VS_VBatchPosition3dTextMul:                                 return VSI_VBatchPosition3dTextMul;
    case VS_VInstancedPosition3dText:                                return VSI_VInstancedPosition3dText;
    case VS_VInstancedPosition3dTextMul:                             return VSI_VInstancedPosition3dTextMul;
    case VS_VTextColor:                                              return VSI_VTextColor;
    case VS_VTextColorMul:                                           return VSI_VTextColorMul;
    case VS_VBatchTextColor:                                         return VSI_VBatchTextColor;
    case VS_VBatchTextColorMul:                                      return VSI_VBatchTextColorMul;
    case VS_VInstancedTextColor:                                     return VSI_VInstancedTextColor;
    case VS_VInstancedTextColorMul:                                  return VSI_VInstancedTextColorMul;
    case VS_VPosition3dTextColor:                                    return VSI_VPosition3dTextColor;
    case VS_VPosition3dTextColorMul:                                 return VSI_VPosition3dTextColorMul;
    case VS_VBatchPosition3dTextColor:                               return VSI_VBatchPosition3dTextColor;
    case VS_VBatchPosition3dTextColorMul:                            return VSI_VBatchPosition3dTextColorMul;
    case VS_VInstancedPosition3dTextColor:                           return VSI_VInstancedPosition3dTextColor;
    case VS_VInstancedPosition3dTextColorMul:                        return VSI_VInstancedPosition3dTextColorMul;
    case VS_VTextColorCxform:                                        return VSI_VTextColorCxform;
    case VS_VTextColorCxformMul:                                     return VSI_VTextColorCxformMul;
    case VS_VBatchTextColorCxform:                                   return VSI_VBatchTextColorCxform;
    case VS_VBatchTextColorCxformMul:                                return VSI_VBatchTextColorCxformMul;
    case VS_VInstancedTextColorCxform:                               return VSI_VInstancedTextColorCxform;
    case VS_VInstancedTextColorCxformMul:                            return VSI_VInstancedTextColorCxformMul;
    case VS_VPosition3dTextColorCxform:                              return VSI_VPosition3dTextColorCxform;
    case VS_VPosition3dTextColorCxformMul:                           return VSI_VPosition3dTextColorCxformMul;
    case VS_VBatchPosition3dTextColorCxform:                         return VSI_VBatchPosition3dTextColorCxform;
    case VS_VBatchPosition3dTextColorCxformMul:                      return VSI_VBatchPosition3dTextColorCxformMul;
    case VS_VInstancedPosition3dTextColorCxform:                     return VSI_VInstancedPosition3dTextColorCxform;
    case VS_VInstancedPosition3dTextColorCxformMul:                  return VSI_VInstancedPosition3dTextColorCxformMul;
    case VS_VYUV:                                                    return VSI_VYUV;
    case VS_VYUVMul:                                                 return VSI_VYUVMul;
    case VS_VBatchYUV:                                               return VSI_VBatchYUV;
    case VS_VBatchYUVMul:                                            return VSI_VBatchYUVMul;
    case VS_VInstancedYUV:                                           return VSI_VInstancedYUV;
    case VS_VInstancedYUVMul:                                        return VSI_VInstancedYUVMul;
    case VS_VPosition3dYUV:                                          return VSI_VPosition3dYUV;
    case VS_VPosition3dYUVMul:                                       return VSI_VPosition3dYUVMul;
    case VS_VBatchPosition3dYUV:                                     return VSI_VBatchPosition3dYUV;
    case VS_VBatchPosition3dYUVMul:                                  return VSI_VBatchPosition3dYUVMul;
    case VS_VInstancedPosition3dYUV:                                 return VSI_VInstancedPosition3dYUV;
    case VS_VInstancedPosition3dYUVMul:                              return VSI_VInstancedPosition3dYUVMul;
    case VS_VYUVCxform:                                              return VSI_VYUVCxform;
    case VS_VYUVCxformMul:                                           return VSI_VYUVCxformMul;
    case VS_VBatchYUVCxform:                                         return VSI_VBatchYUVCxform;
    case VS_VBatchYUVCxformMul:                                      return VSI_VBatchYUVCxformMul;
    case VS_VInstancedYUVCxform:                                     return VSI_VInstancedYUVCxform;
    case VS_VInstancedYUVCxformMul:                                  return VSI_VInstancedYUVCxformMul;
    case VS_VPosition3dYUVCxform:                                    return VSI_VPosition3dYUVCxform;
    case VS_VPosition3dYUVCxformMul:                                 return VSI_VPosition3dYUVCxformMul;
    case VS_VBatchPosition3dYUVCxform:                               return VSI_VBatchPosition3dYUVCxform;
    case VS_VBatchPosition3dYUVCxformMul:                            return VSI_VBatchPosition3dYUVCxformMul;
    case VS_VInstancedPosition3dYUVCxform:                           return VSI_VInstancedPosition3dYUVCxform;
    case VS_VInstancedPosition3dYUVCxformMul:                        return VSI_VInstancedPosition3dYUVCxformMul;
    case VS_VYUVEAlpha:                                              return VSI_VYUVEAlpha;
    case VS_VYUVEAlphaMul:                                           return VSI_VYUVEAlphaMul;
    case VS_VBatchYUVEAlpha:                                         return VSI_VBatchYUVEAlpha;
    case VS_VBatchYUVEAlphaMul:                                      return VSI_VBatchYUVEAlphaMul;
    case VS_VInstancedYUVEAlpha:                                     return VSI_VInstancedYUVEAlpha;
    case VS_VInstancedYUVEAlphaMul:                                  return VSI_VInstancedYUVEAlphaMul;
    case VS_VPosition3dYUVEAlpha:                                    return VSI_VPosition3dYUVEAlpha;
    case VS_VPosition3dYUVEAlphaMul:                                 return VSI_VPosition3dYUVEAlphaMul;
    case VS_VBatchPosition3dYUVEAlpha:                               return VSI_VBatchPosition3dYUVEAlpha;
    case VS_VBatchPosition3dYUVEAlphaMul:                            return VSI_VBatchPosition3dYUVEAlphaMul;
    case VS_VInstancedPosition3dYUVEAlpha:                           return VSI_VInstancedPosition3dYUVEAlpha;
    case VS_VInstancedPosition3dYUVEAlphaMul:                        return VSI_VInstancedPosition3dYUVEAlphaMul;
    case VS_VYUVCxformEAlpha:                                        return VSI_VYUVCxformEAlpha;
    case VS_VYUVCxformEAlphaMul:                                     return VSI_VYUVCxformEAlphaMul;
    case VS_VBatchYUVCxformEAlpha:                                   return VSI_VBatchYUVCxformEAlpha;
    case VS_VBatchYUVCxformEAlphaMul:                                return VSI_VBatchYUVCxformEAlphaMul;
    case VS_VInstancedYUVCxformEAlpha:                               return VSI_VInstancedYUVCxformEAlpha;
    case VS_VInstancedYUVCxformEAlphaMul:                            return VSI_VInstancedYUVCxformEAlphaMul;
    case VS_VPosition3dYUVCxformEAlpha:                              return VSI_VPosition3dYUVCxformEAlpha;
    case VS_VPosition3dYUVCxformEAlphaMul:                           return VSI_VPosition3dYUVCxformEAlphaMul;
    case VS_VBatchPosition3dYUVCxformEAlpha:                         return VSI_VBatchPosition3dYUVCxformEAlpha;
    case VS_VBatchPosition3dYUVCxformEAlphaMul:                      return VSI_VBatchPosition3dYUVCxformEAlphaMul;
    case VS_VInstancedPosition3dYUVCxformEAlpha:                     return VSI_VInstancedPosition3dYUVCxformEAlpha;
    case VS_VInstancedPosition3dYUVCxformEAlphaMul:                  return VSI_VInstancedPosition3dYUVCxformEAlphaMul;
    case VS_VYUVA:                                                   return VSI_VYUVA;
    case VS_VYUVAMul:                                                return VSI_VYUVAMul;
    case VS_VBatchYUVA:                                              return VSI_VBatchYUVA;
    case VS_VBatchYUVAMul:                                           return VSI_VBatchYUVAMul;
    case VS_VInstancedYUVA:                                          return VSI_VInstancedYUVA;
    case VS_VInstancedYUVAMul:                                       return VSI_VInstancedYUVAMul;
    case VS_VPosition3dYUVA:                                         return VSI_VPosition3dYUVA;
    case VS_VPosition3dYUVAMul:                                      return VSI_VPosition3dYUVAMul;
    case VS_VBatchPosition3dYUVA:                                    return VSI_VBatchPosition3dYUVA;
    case VS_VBatchPosition3dYUVAMul:                                 return VSI_VBatchPosition3dYUVAMul;
    case VS_VInstancedPosition3dYUVA:                                return VSI_VInstancedPosition3dYUVA;
    case VS_VInstancedPosition3dYUVAMul:                             return VSI_VInstancedPosition3dYUVAMul;
    case VS_VYUVACxform:                                             return VSI_VYUVACxform;
    case VS_VYUVACxformMul:                                          return VSI_VYUVACxformMul;
    case VS_VBatchYUVACxform:                                        return VSI_VBatchYUVACxform;
    case VS_VBatchYUVACxformMul:                                     return VSI_VBatchYUVACxformMul;
    case VS_VInstancedYUVACxform:                                    return VSI_VInstancedYUVACxform;
    case VS_VInstancedYUVACxformMul:                                 return VSI_VInstancedYUVACxformMul;
    case VS_VPosition3dYUVACxform:                                   return VSI_VPosition3dYUVACxform;
    case VS_VPosition3dYUVACxformMul:                                return VSI_VPosition3dYUVACxformMul;
    case VS_VBatchPosition3dYUVACxform:                              return VSI_VBatchPosition3dYUVACxform;
    case VS_VBatchPosition3dYUVACxformMul:                           return VSI_VBatchPosition3dYUVACxformMul;
    case VS_VInstancedPosition3dYUVACxform:                          return VSI_VInstancedPosition3dYUVACxform;
    case VS_VInstancedPosition3dYUVACxformMul:                       return VSI_VInstancedPosition3dYUVACxformMul;
    case VS_VYUVAEAlpha:                                             return VSI_VYUVAEAlpha;
    case VS_VYUVAEAlphaMul:                                          return VSI_VYUVAEAlphaMul;
    case VS_VBatchYUVAEAlpha:                                        return VSI_VBatchYUVAEAlpha;
    case VS_VBatchYUVAEAlphaMul:                                     return VSI_VBatchYUVAEAlphaMul;
    case VS_VInstancedYUVAEAlpha:                                    return VSI_VInstancedYUVAEAlpha;
    case VS_VInstancedYUVAEAlphaMul:                                 return VSI_VInstancedYUVAEAlphaMul;
    case VS_VPosition3dYUVAEAlpha:                                   return VSI_VPosition3dYUVAEAlpha;
    case VS_VPosition3dYUVAEAlphaMul:                                return VSI_VPosition3dYUVAEAlphaMul;
    case VS_VBatchPosition3dYUVAEAlpha:                              return VSI_VBatchPosition3dYUVAEAlpha;
    case VS_VBatchPosition3dYUVAEAlphaMul:                           return VSI_VBatchPosition3dYUVAEAlphaMul;
    case VS_VInstancedPosition3dYUVAEAlpha:                          return VSI_VInstancedPosition3dYUVAEAlpha;
    case VS_VInstancedPosition3dYUVAEAlphaMul:                       return VSI_VInstancedPosition3dYUVAEAlphaMul;
    case VS_VYUVACxformEAlpha:                                       return VSI_VYUVACxformEAlpha;
    case VS_VYUVACxformEAlphaMul:                                    return VSI_VYUVACxformEAlphaMul;
    case VS_VBatchYUVACxformEAlpha:                                  return VSI_VBatchYUVACxformEAlpha;
    case VS_VBatchYUVACxformEAlphaMul:                               return VSI_VBatchYUVACxformEAlphaMul;
    case VS_VInstancedYUVACxformEAlpha:                              return VSI_VInstancedYUVACxformEAlpha;
    case VS_VInstancedYUVACxformEAlphaMul:                           return VSI_VInstancedYUVACxformEAlphaMul;
    case VS_VPosition3dYUVACxformEAlpha:                             return VSI_VPosition3dYUVACxformEAlpha;
    case VS_VPosition3dYUVACxformEAlphaMul:                          return VSI_VPosition3dYUVACxformEAlphaMul;
    case VS_VBatchPosition3dYUVACxformEAlpha:                        return VSI_VBatchPosition3dYUVACxformEAlpha;
    case VS_VBatchPosition3dYUVACxformEAlphaMul:                     return VSI_VBatchPosition3dYUVACxformEAlphaMul;
    case VS_VInstancedPosition3dYUVACxformEAlpha:                    return VSI_VInstancedPosition3dYUVACxformEAlpha;
    case VS_VInstancedPosition3dYUVACxformEAlphaMul:                 return VSI_VInstancedPosition3dYUVACxformEAlphaMul;
    case VS_VTexTGCMatrixAc:                                         return VSI_VTexTGCMatrixAc;
    case VS_VTexTGCMatrixAcMul:                                      return VSI_VTexTGCMatrixAcMul;
    case VS_VTexTGCMatrixAcEAlpha:                                   return VSI_VTexTGCMatrixAcEAlpha;
    case VS_VTexTGCMatrixAcEAlphaMul:                                return VSI_VTexTGCMatrixAcEAlphaMul;
    case VS_VBox1Blur:                                               return VSI_VBox1Blur;
    case VS_VBox1BlurMul:                                            return VSI_VBox1BlurMul;
    case VS_VBox2Blur:                                               return VSI_VBox2Blur;
    case VS_VBox2BlurMul:                                            return VSI_VBox2BlurMul;
    case VS_VBox2Shadow:                                             return VSI_VBox2Shadow;
    case VS_VBox2ShadowMul:                                          return VSI_VBox2ShadowMul;
    case VS_VBox2ShadowKnockout:                                     return VSI_VBox2ShadowKnockout;
    case VS_VBox2ShadowKnockoutMul:                                  return VSI_VBox2ShadowKnockoutMul;
    case VS_VBox2Shadowonly:                                         return VSI_VBox2Shadowonly;
    case VS_VBox2ShadowonlyMul:                                      return VSI_VBox2ShadowonlyMul;
    case VS_VBox2InnerShadow:                                        return VSI_VBox2InnerShadow;
    case VS_VBox2InnerShadowMul:                                     return VSI_VBox2InnerShadowMul;
    case VS_VBox2InnerShadowKnockout:                                return VSI_VBox2InnerShadowKnockout;
    case VS_VBox2InnerShadowKnockoutMul:                             return VSI_VBox2InnerShadowKnockoutMul;
    case VS_VBox2ShadowonlyHighlight:                                return VSI_VBox2ShadowonlyHighlight;
    case VS_VBox2ShadowonlyHighlightMul:                             return VSI_VBox2ShadowonlyHighlightMul;
    case VS_VBox2InnerShadowHighlight:                               return VSI_VBox2InnerShadowHighlight;
    case VS_VBox2InnerShadowHighlightMul:                            return VSI_VBox2InnerShadowHighlightMul;
    case VS_VBox2InnerShadowHighlightKnockout:                       return VSI_VBox2InnerShadowHighlightKnockout;
    case VS_VBox2InnerShadowHighlightKnockoutMul:                    return VSI_VBox2InnerShadowHighlightKnockoutMul;
    case VS_VBox2ShadowHighlight:                                    return VSI_VBox2ShadowHighlight;
    case VS_VBox2ShadowHighlightMul:                                 return VSI_VBox2ShadowHighlightMul;
    case VS_VBox2ShadowHighlightKnockout:                            return VSI_VBox2ShadowHighlightKnockout;
    case VS_VBox2ShadowHighlightKnockoutMul:                         return VSI_VBox2ShadowHighlightKnockoutMul;
    case VS_VDrawableCopyPixels:                                     return VSI_VDrawableCopyPixels;
    case VS_VDrawableCopyPixelsCopyLerp:                             return VSI_VDrawableCopyPixelsCopyLerp;
    case VS_VDrawableCopyPixelsNoDestAlpha:                          return VSI_VDrawableCopyPixelsNoDestAlpha;
    case VS_VDrawableCopyPixelsNoDestAlphaCopyLerp:                  return VSI_VDrawableCopyPixelsNoDestAlphaCopyLerp;
    case VS_VDrawableCopyPixelsMergeAlpha:                           return VSI_VDrawableCopyPixelsMergeAlpha;
    case VS_VDrawableCopyPixelsMergeAlphaCopyLerp:                   return VSI_VDrawableCopyPixelsMergeAlphaCopyLerp;
    case VS_VDrawableCopyPixelsMergeAlphaNoDestAlpha:                return VSI_VDrawableCopyPixelsMergeAlphaNoDestAlpha;
    case VS_VDrawableCopyPixelsMergeAlphaNoDestAlphaCopyLerp:        return VSI_VDrawableCopyPixelsMergeAlphaNoDestAlphaCopyLerp;
    case VS_VDrawableCopyPixelsAlpha:                                return VSI_VDrawableCopyPixelsAlpha;
    case VS_VDrawableCopyPixelsAlphaCopyLerp:                        return VSI_VDrawableCopyPixelsAlphaCopyLerp;
    case VS_VDrawableCopyPixelsAlphaNoDestAlpha:                     return VSI_VDrawableCopyPixelsAlphaNoDestAlpha;
    case VS_VDrawableCopyPixelsAlphaNoDestAlphaCopyLerp:             return VSI_VDrawableCopyPixelsAlphaNoDestAlphaCopyLerp;
    case VS_VDrawableCopyPixelsAlphaMergeAlpha:                      return VSI_VDrawableCopyPixelsAlphaMergeAlpha;
    case VS_VDrawableCopyPixelsAlphaMergeAlphaCopyLerp:              return VSI_VDrawableCopyPixelsAlphaMergeAlphaCopyLerp;
    case VS_VDrawableCopyPixelsAlphaMergeAlphaNoDestAlpha:           return VSI_VDrawableCopyPixelsAlphaMergeAlphaNoDestAlpha;
    case VS_VDrawableCopyPixelsAlphaMergeAlphaNoDestAlphaCopyLerp:   return VSI_VDrawableCopyPixelsAlphaMergeAlphaNoDestAlphaCopyLerp;
    case VS_VDrawableMerge:                                          return VSI_VDrawableMerge;
    case VS_VDrawableCompare:                                        return VSI_VDrawableCompare;
    case VS_VDrawablePaletteMap:                                     return VSI_VDrawablePaletteMap;
    default: SF_ASSERT(0); return VSI_None;
    }
};

VertexShaderDesc::ShaderIndex VertexShaderDesc::GetShaderIndexForComboIndex(unsigned index)
{
    switch(index)
    {
        case 0:             return VSI_VTexTG;
        case 1:             return VSI_VTexTGMul;
        case 2:             return VSI_VTexTGEAlpha;
        case 3:             return VSI_VTexTGEAlphaMul;
        case 4:             return VSI_VTexTGCxform;
        case 5:             return VSI_VTexTGCxformMul;
        case 6:             return VSI_VTexTGCxformEAlpha;
        case 7:             return VSI_VTexTGCxformEAlphaMul;
        case 8:             return VSI_VVertex;
        case 9:             return VSI_VVertexMul;
        case 10:            return VSI_VVertexEAlpha;
        case 11:            return VSI_VVertexEAlphaMul;
        case 12:            return VSI_VVertexCxform;
        case 13:            return VSI_VVertexCxformMul;
        case 14:            return VSI_VVertexCxformEAlpha;
        case 15:            return VSI_VVertexCxformEAlphaMul;
        case 16:            return VSI_VTexTGTexTG;
        case 17:            return VSI_VTexTGTexTGMul;
        case 18:            return VSI_VTexTGTexTGEAlpha;
        case 19:            return VSI_VTexTGTexTGEAlphaMul;
        case 20:            return VSI_VTexTGTexTGCxform;
        case 21:            return VSI_VTexTGTexTGCxformMul;
        case 22:            return VSI_VTexTGTexTGCxformEAlpha;
        case 23:            return VSI_VTexTGTexTGCxformEAlphaMul;
        case 24:            return VSI_VTexTGVertex;
        case 25:            return VSI_VTexTGVertexMul;
        case 26:            return VSI_VTexTGVertexEAlpha;
        case 27:            return VSI_VTexTGVertexEAlphaMul;
        case 28:            return VSI_VTexTGVertexCxform;
        case 29:            return VSI_VTexTGVertexCxformMul;
        case 30:            return VSI_VTexTGVertexCxformEAlpha;
        case 31:            return VSI_VTexTGVertexCxformEAlphaMul;
        case 32:            return VSI_VPosition3dTexTG;
        case 33:            return VSI_VPosition3dTexTGMul;
        case 34:            return VSI_VPosition3dTexTGEAlpha;
        case 35:            return VSI_VPosition3dTexTGEAlphaMul;
        case 36:            return VSI_VPosition3dTexTGCxform;
        case 37:            return VSI_VPosition3dTexTGCxformMul;
        case 38:            return VSI_VPosition3dTexTGCxformEAlpha;
        case 39:            return VSI_VPosition3dTexTGCxformEAlphaMul;
        case 40:            return VSI_VPosition3dVertex;
        case 41:            return VSI_VPosition3dVertexMul;
        case 42:            return VSI_VPosition3dVertexEAlpha;
        case 43:            return VSI_VPosition3dVertexEAlphaMul;
        case 44:            return VSI_VPosition3dVertexCxform;
        case 45:            return VSI_VPosition3dVertexCxformMul;
        case 46:            return VSI_VPosition3dVertexCxformEAlpha;
        case 47:            return VSI_VPosition3dVertexCxformEAlphaMul;
        case 48:            return VSI_VPosition3dTexTGTexTG;
        case 49:            return VSI_VPosition3dTexTGTexTGMul;
        case 50:            return VSI_VPosition3dTexTGTexTGEAlpha;
        case 51:            return VSI_VPosition3dTexTGTexTGEAlphaMul;
        case 52:            return VSI_VPosition3dTexTGTexTGCxform;
        case 53:            return VSI_VPosition3dTexTGTexTGCxformMul;
        case 54:            return VSI_VPosition3dTexTGTexTGCxformEAlpha;
        case 55:            return VSI_VPosition3dTexTGTexTGCxformEAlphaMul;
        case 56:            return VSI_VPosition3dTexTGVertex;
        case 57:            return VSI_VPosition3dTexTGVertexMul;
        case 58:            return VSI_VPosition3dTexTGVertexEAlpha;
        case 59:            return VSI_VPosition3dTexTGVertexEAlphaMul;
        case 60:            return VSI_VPosition3dTexTGVertexCxform;
        case 61:            return VSI_VPosition3dTexTGVertexCxformMul;
        case 62:            return VSI_VPosition3dTexTGVertexCxformEAlpha;
        case 63:            return VSI_VPosition3dTexTGVertexCxformEAlphaMul;
        case 64:            return VSI_VBatchTexTG;
        case 65:            return VSI_VBatchTexTGMul;
        case 66:            return VSI_VBatchTexTGEAlpha;
        case 67:            return VSI_VBatchTexTGEAlphaMul;
        case 68:            return VSI_VBatchTexTGCxform;
        case 69:            return VSI_VBatchTexTGCxformMul;
        case 70:            return VSI_VBatchTexTGCxformEAlpha;
        case 71:            return VSI_VBatchTexTGCxformEAlphaMul;
        case 72:            return VSI_VBatchVertex;
        case 73:            return VSI_VBatchVertexMul;
        case 74:            return VSI_VBatchVertexEAlpha;
        case 75:            return VSI_VBatchVertexEAlphaMul;
        case 76:            return VSI_VBatchVertexCxform;
        case 77:            return VSI_VBatchVertexCxformMul;
        case 78:            return VSI_VBatchVertexCxformEAlpha;
        case 79:            return VSI_VBatchVertexCxformEAlphaMul;
        case 80:            return VSI_VBatchTexTGTexTG;
        case 81:            return VSI_VBatchTexTGTexTGMul;
        case 82:            return VSI_VBatchTexTGTexTGEAlpha;
        case 83:            return VSI_VBatchTexTGTexTGEAlphaMul;
        case 84:            return VSI_VBatchTexTGTexTGCxform;
        case 85:            return VSI_VBatchTexTGTexTGCxformMul;
        case 86:            return VSI_VBatchTexTGTexTGCxformEAlpha;
        case 87:            return VSI_VBatchTexTGTexTGCxformEAlphaMul;
        case 88:            return VSI_VBatchTexTGVertex;
        case 89:            return VSI_VBatchTexTGVertexMul;
        case 90:            return VSI_VBatchTexTGVertexEAlpha;
        case 91:            return VSI_VBatchTexTGVertexEAlphaMul;
        case 92:            return VSI_VBatchTexTGVertexCxform;
        case 93:            return VSI_VBatchTexTGVertexCxformMul;
        case 94:            return VSI_VBatchTexTGVertexCxformEAlpha;
        case 95:            return VSI_VBatchTexTGVertexCxformEAlphaMul;
        case 96:            return VSI_VBatchPosition3dTexTG;
        case 97:            return VSI_VBatchPosition3dTexTGMul;
        case 98:            return VSI_VBatchPosition3dTexTGEAlpha;
        case 99:            return VSI_VBatchPosition3dTexTGEAlphaMul;
        case 100:           return VSI_VBatchPosition3dTexTGCxform;
        case 101:           return VSI_VBatchPosition3dTexTGCxformMul;
        case 102:           return VSI_VBatchPosition3dTexTGCxformEAlpha;
        case 103:           return VSI_VBatchPosition3dTexTGCxformEAlphaMul;
        case 104:           return VSI_VBatchPosition3dVertex;
        case 105:           return VSI_VBatchPosition3dVertexMul;
        case 106:           return VSI_VBatchPosition3dVertexEAlpha;
        case 107:           return VSI_VBatchPosition3dVertexEAlphaMul;
        case 108:           return VSI_VBatchPosition3dVertexCxform;
        case 109:           return VSI_VBatchPosition3dVertexCxformMul;
        case 110:           return VSI_VBatchPosition3dVertexCxformEAlpha;
        case 111:           return VSI_VBatchPosition3dVertexCxformEAlphaMul;
        case 112:           return VSI_VBatchPosition3dTexTGTexTG;
        case 113:           return VSI_VBatchPosition3dTexTGTexTGMul;
        case 114:           return VSI_VBatchPosition3dTexTGTexTGEAlpha;
        case 115:           return VSI_VBatchPosition3dTexTGTexTGEAlphaMul;
        case 116:           return VSI_VBatchPosition3dTexTGTexTGCxform;
        case 117:           return VSI_VBatchPosition3dTexTGTexTGCxformMul;
        case 118:           return VSI_VBatchPosition3dTexTGTexTGCxformEAlpha;
        case 119:           return VSI_VBatchPosition3dTexTGTexTGCxformEAlphaMul;
        case 120:           return VSI_VBatchPosition3dTexTGVertex;
        case 121:           return VSI_VBatchPosition3dTexTGVertexMul;
        case 122:           return VSI_VBatchPosition3dTexTGVertexEAlpha;
        case 123:           return VSI_VBatchPosition3dTexTGVertexEAlphaMul;
        case 124:           return VSI_VBatchPosition3dTexTGVertexCxform;
        case 125:           return VSI_VBatchPosition3dTexTGVertexCxformMul;
        case 126:           return VSI_VBatchPosition3dTexTGVertexCxformEAlpha;
        case 127:           return VSI_VBatchPosition3dTexTGVertexCxformEAlphaMul;
        case 128:           return VSI_VInstancedTexTG;
        case 129:           return VSI_VInstancedTexTGMul;
        case 130:           return VSI_VInstancedTexTGEAlpha;
        case 131:           return VSI_VInstancedTexTGEAlphaMul;
        case 132:           return VSI_VInstancedTexTGCxform;
        case 133:           return VSI_VInstancedTexTGCxformMul;
        case 134:           return VSI_VInstancedTexTGCxformEAlpha;
        case 135:           return VSI_VInstancedTexTGCxformEAlphaMul;
        case 136:           return VSI_VInstancedVertex;
        case 137:           return VSI_VInstancedVertexMul;
        case 138:           return VSI_VInstancedVertexEAlpha;
        case 139:           return VSI_VInstancedVertexEAlphaMul;
        case 140:           return VSI_VInstancedVertexCxform;
        case 141:           return VSI_VInstancedVertexCxformMul;
        case 142:           return VSI_VInstancedVertexCxformEAlpha;
        case 143:           return VSI_VInstancedVertexCxformEAlphaMul;
        case 144:           return VSI_VInstancedTexTGTexTG;
        case 145:           return VSI_VInstancedTexTGTexTGMul;
        case 146:           return VSI_VInstancedTexTGTexTGEAlpha;
        case 147:           return VSI_VInstancedTexTGTexTGEAlphaMul;
        case 148:           return VSI_VInstancedTexTGTexTGCxform;
        case 149:           return VSI_VInstancedTexTGTexTGCxformMul;
        case 150:           return VSI_VInstancedTexTGTexTGCxformEAlpha;
        case 151:           return VSI_VInstancedTexTGTexTGCxformEAlphaMul;
        case 152:           return VSI_VInstancedTexTGVertex;
        case 153:           return VSI_VInstancedTexTGVertexMul;
        case 154:           return VSI_VInstancedTexTGVertexEAlpha;
        case 155:           return VSI_VInstancedTexTGVertexEAlphaMul;
        case 156:           return VSI_VInstancedTexTGVertexCxform;
        case 157:           return VSI_VInstancedTexTGVertexCxformMul;
        case 158:           return VSI_VInstancedTexTGVertexCxformEAlpha;
        case 159:           return VSI_VInstancedTexTGVertexCxformEAlphaMul;
        case 160:           return VSI_VInstancedPosition3dTexTG;
        case 161:           return VSI_VInstancedPosition3dTexTGMul;
        case 162:           return VSI_VInstancedPosition3dTexTGEAlpha;
        case 163:           return VSI_VInstancedPosition3dTexTGEAlphaMul;
        case 164:           return VSI_VInstancedPosition3dTexTGCxform;
        case 165:           return VSI_VInstancedPosition3dTexTGCxformMul;
        case 166:           return VSI_VInstancedPosition3dTexTGCxformEAlpha;
        case 167:           return VSI_VInstancedPosition3dTexTGCxformEAlphaMul;
        case 168:           return VSI_VInstancedPosition3dVertex;
        case 169:           return VSI_VInstancedPosition3dVertexMul;
        case 170:           return VSI_VInstancedPosition3dVertexEAlpha;
        case 171:           return VSI_VInstancedPosition3dVertexEAlphaMul;
        case 172:           return VSI_VInstancedPosition3dVertexCxform;
        case 173:           return VSI_VInstancedPosition3dVertexCxformMul;
        case 174:           return VSI_VInstancedPosition3dVertexCxformEAlpha;
        case 175:           return VSI_VInstancedPosition3dVertexCxformEAlphaMul;
        case 176:           return VSI_VInstancedPosition3dTexTGTexTG;
        case 177:           return VSI_VInstancedPosition3dTexTGTexTGMul;
        case 178:           return VSI_VInstancedPosition3dTexTGTexTGEAlpha;
        case 179:           return VSI_VInstancedPosition3dTexTGTexTGEAlphaMul;
        case 180:           return VSI_VInstancedPosition3dTexTGTexTGCxform;
        case 181:           return VSI_VInstancedPosition3dTexTGTexTGCxformMul;
        case 182:           return VSI_VInstancedPosition3dTexTGTexTGCxformEAlpha;
        case 183:           return VSI_VInstancedPosition3dTexTGTexTGCxformEAlphaMul;
        case 184:           return VSI_VInstancedPosition3dTexTGVertex;
        case 185:           return VSI_VInstancedPosition3dTexTGVertexMul;
        case 186:           return VSI_VInstancedPosition3dTexTGVertexEAlpha;
        case 187:           return VSI_VInstancedPosition3dTexTGVertexEAlphaMul;
        case 188:           return VSI_VInstancedPosition3dTexTGVertexCxform;
        case 189:           return VSI_VInstancedPosition3dTexTGVertexCxformMul;
        case 190:           return VSI_VInstancedPosition3dTexTGVertexCxformEAlpha;
        case 191:           return VSI_VInstancedPosition3dTexTGVertexCxformEAlphaMul;
        case 192:           return VSI_VSolid;
        case 193:           return VSI_VSolidMul;
        case 194:           return VSI_VText;
        case 195:           return VSI_VTextMul;
        case 196:           return VSI_VTextColor;
        case 197:           return VSI_VTextColorMul;
        case 198:           return VSI_VTextColorCxform;
        case 199:           return VSI_VTextColorCxformMul;
        case 200:           return VSI_VPosition3dSolid;
        case 201:           return VSI_VPosition3dSolidMul;
        case 202:           return VSI_VPosition3dText;
        case 203:           return VSI_VPosition3dTextMul;
        case 204:           return VSI_VPosition3dTextColor;
        case 205:           return VSI_VPosition3dTextColorMul;
        case 206:           return VSI_VPosition3dTextColorCxform;
        case 207:           return VSI_VPosition3dTextColorCxformMul;
        case 208:           return VSI_VBatchSolid;
        case 209:           return VSI_VBatchSolidMul;
        case 210:           return VSI_VBatchText;
        case 211:           return VSI_VBatchTextMul;
        case 212:           return VSI_VBatchTextColor;
        case 213:           return VSI_VBatchTextColorMul;
        case 214:           return VSI_VBatchTextColorCxform;
        case 215:           return VSI_VBatchTextColorCxformMul;
        case 216:           return VSI_VBatchPosition3dSolid;
        case 217:           return VSI_VBatchPosition3dSolidMul;
        case 218:           return VSI_VBatchPosition3dText;
        case 219:           return VSI_VBatchPosition3dTextMul;
        case 220:           return VSI_VBatchPosition3dTextColor;
        case 221:           return VSI_VBatchPosition3dTextColorMul;
        case 222:           return VSI_VBatchPosition3dTextColorCxform;
        case 223:           return VSI_VBatchPosition3dTextColorCxformMul;
        case 224:           return VSI_VInstancedSolid;
        case 225:           return VSI_VInstancedSolidMul;
        case 226:           return VSI_VInstancedText;
        case 227:           return VSI_VInstancedTextMul;
        case 228:           return VSI_VInstancedTextColor;
        case 229:           return VSI_VInstancedTextColorMul;
        case 230:           return VSI_VInstancedTextColorCxform;
        case 231:           return VSI_VInstancedTextColorCxformMul;
        case 232:           return VSI_VInstancedPosition3dSolid;
        case 233:           return VSI_VInstancedPosition3dSolidMul;
        case 234:           return VSI_VInstancedPosition3dText;
        case 235:           return VSI_VInstancedPosition3dTextMul;
        case 236:           return VSI_VInstancedPosition3dTextColor;
        case 237:           return VSI_VInstancedPosition3dTextColorMul;
        case 238:           return VSI_VInstancedPosition3dTextColorCxform;
        case 239:           return VSI_VInstancedPosition3dTextColorCxformMul;
        case 240:           return VSI_VYUV;
        case 241:           return VSI_VYUVMul;
        case 242:           return VSI_VYUVEAlpha;
        case 243:           return VSI_VYUVEAlphaMul;
        case 244:           return VSI_VYUVCxform;
        case 245:           return VSI_VYUVCxformMul;
        case 246:           return VSI_VYUVCxformEAlpha;
        case 247:           return VSI_VYUVCxformEAlphaMul;
        case 248:           return VSI_VYUVA;
        case 249:           return VSI_VYUVAMul;
        case 250:           return VSI_VYUVAEAlpha;
        case 251:           return VSI_VYUVAEAlphaMul;
        case 252:           return VSI_VYUVACxform;
        case 253:           return VSI_VYUVACxformMul;
        case 254:           return VSI_VYUVACxformEAlpha;
        case 255:           return VSI_VYUVACxformEAlphaMul;
        case 256:           return VSI_VPosition3dYUV;
        case 257:           return VSI_VPosition3dYUVMul;
        case 258:           return VSI_VPosition3dYUVEAlpha;
        case 259:           return VSI_VPosition3dYUVEAlphaMul;
        case 260:           return VSI_VPosition3dYUVCxform;
        case 261:           return VSI_VPosition3dYUVCxformMul;
        case 262:           return VSI_VPosition3dYUVCxformEAlpha;
        case 263:           return VSI_VPosition3dYUVCxformEAlphaMul;
        case 264:           return VSI_VPosition3dYUVA;
        case 265:           return VSI_VPosition3dYUVAMul;
        case 266:           return VSI_VPosition3dYUVAEAlpha;
        case 267:           return VSI_VPosition3dYUVAEAlphaMul;
        case 268:           return VSI_VPosition3dYUVACxform;
        case 269:           return VSI_VPosition3dYUVACxformMul;
        case 270:           return VSI_VPosition3dYUVACxformEAlpha;
        case 271:           return VSI_VPosition3dYUVACxformEAlphaMul;
        case 272:           return VSI_VBatchYUV;
        case 273:           return VSI_VBatchYUVMul;
        case 274:           return VSI_VBatchYUVEAlpha;
        case 275:           return VSI_VBatchYUVEAlphaMul;
        case 276:           return VSI_VBatchYUVCxform;
        case 277:           return VSI_VBatchYUVCxformMul;
        case 278:           return VSI_VBatchYUVCxformEAlpha;
        case 279:           return VSI_VBatchYUVCxformEAlphaMul;
        case 280:           return VSI_VBatchYUVA;
        case 281:           return VSI_VBatchYUVAMul;
        case 282:           return VSI_VBatchYUVAEAlpha;
        case 283:           return VSI_VBatchYUVAEAlphaMul;
        case 284:           return VSI_VBatchYUVACxform;
        case 285:           return VSI_VBatchYUVACxformMul;
        case 286:           return VSI_VBatchYUVACxformEAlpha;
        case 287:           return VSI_VBatchYUVACxformEAlphaMul;
        case 288:           return VSI_VBatchPosition3dYUV;
        case 289:           return VSI_VBatchPosition3dYUVMul;
        case 290:           return VSI_VBatchPosition3dYUVEAlpha;
        case 291:           return VSI_VBatchPosition3dYUVEAlphaMul;
        case 292:           return VSI_VBatchPosition3dYUVCxform;
        case 293:           return VSI_VBatchPosition3dYUVCxformMul;
        case 294:           return VSI_VBatchPosition3dYUVCxformEAlpha;
        case 295:           return VSI_VBatchPosition3dYUVCxformEAlphaMul;
        case 296:           return VSI_VBatchPosition3dYUVA;
        case 297:           return VSI_VBatchPosition3dYUVAMul;
        case 298:           return VSI_VBatchPosition3dYUVAEAlpha;
        case 299:           return VSI_VBatchPosition3dYUVAEAlphaMul;
        case 300:           return VSI_VBatchPosition3dYUVACxform;
        case 301:           return VSI_VBatchPosition3dYUVACxformMul;
        case 302:           return VSI_VBatchPosition3dYUVACxformEAlpha;
        case 303:           return VSI_VBatchPosition3dYUVACxformEAlphaMul;
        case 304:           return VSI_VInstancedYUV;
        case 305:           return VSI_VInstancedYUVMul;
        case 306:           return VSI_VInstancedYUVEAlpha;
        case 307:           return VSI_VInstancedYUVEAlphaMul;
        case 308:           return VSI_VInstancedYUVCxform;
        case 309:           return VSI_VInstancedYUVCxformMul;
        case 310:           return VSI_VInstancedYUVCxformEAlpha;
        case 311:           return VSI_VInstancedYUVCxformEAlphaMul;
        case 312:           return VSI_VInstancedYUVA;
        case 313:           return VSI_VInstancedYUVAMul;
        case 314:           return VSI_VInstancedYUVAEAlpha;
        case 315:           return VSI_VInstancedYUVAEAlphaMul;
        case 316:           return VSI_VInstancedYUVACxform;
        case 317:           return VSI_VInstancedYUVACxformMul;
        case 318:           return VSI_VInstancedYUVACxformEAlpha;
        case 319:           return VSI_VInstancedYUVACxformEAlphaMul;
        case 320:           return VSI_VInstancedPosition3dYUV;
        case 321:           return VSI_VInstancedPosition3dYUVMul;
        case 322:           return VSI_VInstancedPosition3dYUVEAlpha;
        case 323:           return VSI_VInstancedPosition3dYUVEAlphaMul;
        case 324:           return VSI_VInstancedPosition3dYUVCxform;
        case 325:           return VSI_VInstancedPosition3dYUVCxformMul;
        case 326:           return VSI_VInstancedPosition3dYUVCxformEAlpha;
        case 327:           return VSI_VInstancedPosition3dYUVCxformEAlphaMul;
        case 328:           return VSI_VInstancedPosition3dYUVA;
        case 329:           return VSI_VInstancedPosition3dYUVAMul;
        case 330:           return VSI_VInstancedPosition3dYUVAEAlpha;
        case 331:           return VSI_VInstancedPosition3dYUVAEAlphaMul;
        case 332:           return VSI_VInstancedPosition3dYUVACxform;
        case 333:           return VSI_VInstancedPosition3dYUVACxformMul;
        case 334:           return VSI_VInstancedPosition3dYUVACxformEAlpha;
        case 335:           return VSI_VInstancedPosition3dYUVACxformEAlphaMul;
        case 336:           return VSI_VTexTGCMatrixAc;
        case 337:           return VSI_VTexTGCMatrixAcMul;
        case 338:           return VSI_VTexTGCMatrixAcEAlpha;
        case 339:           return VSI_VTexTGCMatrixAcEAlphaMul;
        case 340:           return VSI_VBox1Blur;
        case 341:           return VSI_VBox1BlurMul;
        case 342:           return VSI_VBox2Blur;
        case 343:           return VSI_VBox2BlurMul;
        case 344:           return VSI_VBox2Shadow;
        case 345:           return VSI_VBox2ShadowMul;
        case 346:           return VSI_VBox2ShadowKnockout;
        case 347:           return VSI_VBox2ShadowKnockoutMul;
        case 348:           return VSI_VBox2InnerShadow;
        case 349:           return VSI_VBox2InnerShadowMul;
        case 350:           return VSI_VBox2InnerShadowKnockout;
        case 351:           return VSI_VBox2InnerShadowKnockoutMul;
        case 352:           return VSI_VBox2Shadowonly;
        case 353:           return VSI_VBox2ShadowonlyMul;
        case 354:           return VSI_VBox2ShadowHighlight;
        case 355:           return VSI_VBox2ShadowHighlightMul;
        case 356:           return VSI_VBox2ShadowHighlightKnockout;
        case 357:           return VSI_VBox2ShadowHighlightKnockoutMul;
        case 358:           return VSI_VBox2InnerShadowHighlight;
        case 359:           return VSI_VBox2InnerShadowHighlightMul;
        case 360:           return VSI_VBox2InnerShadowHighlightKnockout;
        case 361:           return VSI_VBox2InnerShadowHighlightKnockoutMul;
        case 362:           return VSI_VBox2ShadowonlyHighlight;
        case 363:           return VSI_VBox2ShadowonlyHighlightMul;
        case 364:           return VSI_VDrawableCopyPixels;
        case 365:           return VSI_VDrawableCopyPixelsCopyLerp;
        case 366:           return VSI_VDrawableCopyPixelsNoDestAlpha;
        case 367:           return VSI_VDrawableCopyPixelsNoDestAlphaCopyLerp;
        case 368:           return VSI_VDrawableCopyPixelsMergeAlpha;
        case 369:           return VSI_VDrawableCopyPixelsMergeAlphaCopyLerp;
        case 370:           return VSI_VDrawableCopyPixelsMergeAlphaNoDestAlpha;
        case 371:           return VSI_VDrawableCopyPixelsMergeAlphaNoDestAlphaCopyLerp;
        case 372:           return VSI_VDrawableCopyPixelsAlpha;
        case 373:           return VSI_VDrawableCopyPixelsAlphaCopyLerp;
        case 374:           return VSI_VDrawableCopyPixelsAlphaNoDestAlpha;
        case 375:           return VSI_VDrawableCopyPixelsAlphaNoDestAlphaCopyLerp;
        case 376:           return VSI_VDrawableCopyPixelsAlphaMergeAlpha;
        case 377:           return VSI_VDrawableCopyPixelsAlphaMergeAlphaCopyLerp;
        case 378:           return VSI_VDrawableCopyPixelsAlphaMergeAlphaNoDestAlpha;
        case 379:           return VSI_VDrawableCopyPixelsAlphaMergeAlphaNoDestAlphaCopyLerp;
        case 380:           return VSI_VDrawableMerge;
        case 381:           return VSI_VDrawableCompare;
        case 382:           return VSI_VDrawablePaletteMap;
        default: SF_ASSERT(0); return VSI_None;
    }
}

unsigned VertexShaderDesc::GetShaderComboIndex(ShaderType shader)
{
    switch(shader)
    {
        case VS_VTexTG:                                                  return 0;
        case VS_VTexTGMul:                                               return 1;
        case VS_VBatchTexTG:                                             return 64;
        case VS_VBatchTexTGMul:                                          return 65;
        case VS_VInstancedTexTG:                                         return 128;
        case VS_VInstancedTexTGMul:                                      return 129;
        case VS_VPosition3dTexTG:                                        return 32;
        case VS_VPosition3dTexTGMul:                                     return 33;
        case VS_VBatchPosition3dTexTG:                                   return 96;
        case VS_VBatchPosition3dTexTGMul:                                return 97;
        case VS_VInstancedPosition3dTexTG:                               return 160;
        case VS_VInstancedPosition3dTexTGMul:                            return 161;
        case VS_VTexTGCxform:                                            return 4;
        case VS_VTexTGCxformMul:                                         return 5;
        case VS_VBatchTexTGCxform:                                       return 68;
        case VS_VBatchTexTGCxformMul:                                    return 69;
        case VS_VInstancedTexTGCxform:                                   return 132;
        case VS_VInstancedTexTGCxformMul:                                return 133;
        case VS_VPosition3dTexTGCxform:                                  return 36;
        case VS_VPosition3dTexTGCxformMul:                               return 37;
        case VS_VBatchPosition3dTexTGCxform:                             return 100;
        case VS_VBatchPosition3dTexTGCxformMul:                          return 101;
        case VS_VInstancedPosition3dTexTGCxform:                         return 164;
        case VS_VInstancedPosition3dTexTGCxformMul:                      return 165;
        case VS_VTexTGEAlpha:                                            return 2;
        case VS_VTexTGEAlphaMul:                                         return 3;
        case VS_VBatchTexTGEAlpha:                                       return 66;
        case VS_VBatchTexTGEAlphaMul:                                    return 67;
        case VS_VInstancedTexTGEAlpha:                                   return 130;
        case VS_VInstancedTexTGEAlphaMul:                                return 131;
        case VS_VPosition3dTexTGEAlpha:                                  return 34;
        case VS_VPosition3dTexTGEAlphaMul:                               return 35;
        case VS_VBatchPosition3dTexTGEAlpha:                             return 98;
        case VS_VBatchPosition3dTexTGEAlphaMul:                          return 99;
        case VS_VInstancedPosition3dTexTGEAlpha:                         return 162;
        case VS_VInstancedPosition3dTexTGEAlphaMul:                      return 163;
        case VS_VTexTGCxformEAlpha:                                      return 6;
        case VS_VTexTGCxformEAlphaMul:                                   return 7;
        case VS_VBatchTexTGCxformEAlpha:                                 return 70;
        case VS_VBatchTexTGCxformEAlphaMul:                              return 71;
        case VS_VInstancedTexTGCxformEAlpha:                             return 134;
        case VS_VInstancedTexTGCxformEAlphaMul:                          return 135;
        case VS_VPosition3dTexTGCxformEAlpha:                            return 38;
        case VS_VPosition3dTexTGCxformEAlphaMul:                         return 39;
        case VS_VBatchPosition3dTexTGCxformEAlpha:                       return 102;
        case VS_VBatchPosition3dTexTGCxformEAlphaMul:                    return 103;
        case VS_VInstancedPosition3dTexTGCxformEAlpha:                   return 166;
        case VS_VInstancedPosition3dTexTGCxformEAlphaMul:                return 167;
        case VS_VVertex:                                                 return 8;
        case VS_VVertexMul:                                              return 9;
        case VS_VBatchVertex:                                            return 72;
        case VS_VBatchVertexMul:                                         return 73;
        case VS_VInstancedVertex:                                        return 136;
        case VS_VInstancedVertexMul:                                     return 137;
        case VS_VPosition3dVertex:                                       return 40;
        case VS_VPosition3dVertexMul:                                    return 41;
        case VS_VBatchPosition3dVertex:                                  return 104;
        case VS_VBatchPosition3dVertexMul:                               return 105;
        case VS_VInstancedPosition3dVertex:                              return 168;
        case VS_VInstancedPosition3dVertexMul:                           return 169;
        case VS_VVertexCxform:                                           return 12;
        case VS_VVertexCxformMul:                                        return 13;
        case VS_VBatchVertexCxform:                                      return 76;
        case VS_VBatchVertexCxformMul:                                   return 77;
        case VS_VInstancedVertexCxform:                                  return 140;
        case VS_VInstancedVertexCxformMul:                               return 141;
        case VS_VPosition3dVertexCxform:                                 return 44;
        case VS_VPosition3dVertexCxformMul:                              return 45;
        case VS_VBatchPosition3dVertexCxform:                            return 108;
        case VS_VBatchPosition3dVertexCxformMul:                         return 109;
        case VS_VInstancedPosition3dVertexCxform:                        return 172;
        case VS_VInstancedPosition3dVertexCxformMul:                     return 173;
        case VS_VVertexEAlpha:                                           return 10;
        case VS_VVertexEAlphaMul:                                        return 11;
        case VS_VBatchVertexEAlpha:                                      return 74;
        case VS_VBatchVertexEAlphaMul:                                   return 75;
        case VS_VInstancedVertexEAlpha:                                  return 138;
        case VS_VInstancedVertexEAlphaMul:                               return 139;
        case VS_VPosition3dVertexEAlpha:                                 return 42;
        case VS_VPosition3dVertexEAlphaMul:                              return 43;
        case VS_VBatchPosition3dVertexEAlpha:                            return 106;
        case VS_VBatchPosition3dVertexEAlphaMul:                         return 107;
        case VS_VInstancedPosition3dVertexEAlpha:                        return 170;
        case VS_VInstancedPosition3dVertexEAlphaMul:                     return 171;
        case VS_VVertexCxformEAlpha:                                     return 14;
        case VS_VVertexCxformEAlphaMul:                                  return 15;
        case VS_VBatchVertexCxformEAlpha:                                return 78;
        case VS_VBatchVertexCxformEAlphaMul:                             return 79;
        case VS_VInstancedVertexCxformEAlpha:                            return 142;
        case VS_VInstancedVertexCxformEAlphaMul:                         return 143;
        case VS_VPosition3dVertexCxformEAlpha:                           return 46;
        case VS_VPosition3dVertexCxformEAlphaMul:                        return 47;
        case VS_VBatchPosition3dVertexCxformEAlpha:                      return 110;
        case VS_VBatchPosition3dVertexCxformEAlphaMul:                   return 111;
        case VS_VInstancedPosition3dVertexCxformEAlpha:                  return 174;
        case VS_VInstancedPosition3dVertexCxformEAlphaMul:               return 175;
        case VS_VTexTGTexTG:                                             return 16;
        case VS_VTexTGTexTGMul:                                          return 17;
        case VS_VBatchTexTGTexTG:                                        return 80;
        case VS_VBatchTexTGTexTGMul:                                     return 81;
        case VS_VInstancedTexTGTexTG:                                    return 144;
        case VS_VInstancedTexTGTexTGMul:                                 return 145;
        case VS_VPosition3dTexTGTexTG:                                   return 48;
        case VS_VPosition3dTexTGTexTGMul:                                return 49;
        case VS_VBatchPosition3dTexTGTexTG:                              return 112;
        case VS_VBatchPosition3dTexTGTexTGMul:                           return 113;
        case VS_VInstancedPosition3dTexTGTexTG:                          return 176;
        case VS_VInstancedPosition3dTexTGTexTGMul:                       return 177;
        case VS_VTexTGTexTGCxform:                                       return 20;
        case VS_VTexTGTexTGCxformMul:                                    return 21;
        case VS_VBatchTexTGTexTGCxform:                                  return 84;
        case VS_VBatchTexTGTexTGCxformMul:                               return 85;
        case VS_VInstancedTexTGTexTGCxform:                              return 148;
        case VS_VInstancedTexTGTexTGCxformMul:                           return 149;
        case VS_VPosition3dTexTGTexTGCxform:                             return 52;
        case VS_VPosition3dTexTGTexTGCxformMul:                          return 53;
        case VS_VBatchPosition3dTexTGTexTGCxform:                        return 116;
        case VS_VBatchPosition3dTexTGTexTGCxformMul:                     return 117;
        case VS_VInstancedPosition3dTexTGTexTGCxform:                    return 180;
        case VS_VInstancedPosition3dTexTGTexTGCxformMul:                 return 181;
        case VS_VTexTGTexTGEAlpha:                                       return 18;
        case VS_VTexTGTexTGEAlphaMul:                                    return 19;
        case VS_VBatchTexTGTexTGEAlpha:                                  return 82;
        case VS_VBatchTexTGTexTGEAlphaMul:                               return 83;
        case VS_VInstancedTexTGTexTGEAlpha:                              return 146;
        case VS_VInstancedTexTGTexTGEAlphaMul:                           return 147;
        case VS_VPosition3dTexTGTexTGEAlpha:                             return 50;
        case VS_VPosition3dTexTGTexTGEAlphaMul:                          return 51;
        case VS_VBatchPosition3dTexTGTexTGEAlpha:                        return 114;
        case VS_VBatchPosition3dTexTGTexTGEAlphaMul:                     return 115;
        case VS_VInstancedPosition3dTexTGTexTGEAlpha:                    return 178;
        case VS_VInstancedPosition3dTexTGTexTGEAlphaMul:                 return 179;
        case VS_VTexTGTexTGCxformEAlpha:                                 return 22;
        case VS_VTexTGTexTGCxformEAlphaMul:                              return 23;
        case VS_VBatchTexTGTexTGCxformEAlpha:                            return 86;
        case VS_VBatchTexTGTexTGCxformEAlphaMul:                         return 87;
        case VS_VInstancedTexTGTexTGCxformEAlpha:                        return 150;
        case VS_VInstancedTexTGTexTGCxformEAlphaMul:                     return 151;
        case VS_VPosition3dTexTGTexTGCxformEAlpha:                       return 54;
        case VS_VPosition3dTexTGTexTGCxformEAlphaMul:                    return 55;
        case VS_VBatchPosition3dTexTGTexTGCxformEAlpha:                  return 118;
        case VS_VBatchPosition3dTexTGTexTGCxformEAlphaMul:               return 119;
        case VS_VInstancedPosition3dTexTGTexTGCxformEAlpha:              return 182;
        case VS_VInstancedPosition3dTexTGTexTGCxformEAlphaMul:           return 183;
        case VS_VTexTGVertex:                                            return 24;
        case VS_VTexTGVertexMul:                                         return 25;
        case VS_VBatchTexTGVertex:                                       return 88;
        case VS_VBatchTexTGVertexMul:                                    return 89;
        case VS_VInstancedTexTGVertex:                                   return 152;
        case VS_VInstancedTexTGVertexMul:                                return 153;
        case VS_VPosition3dTexTGVertex:                                  return 56;
        case VS_VPosition3dTexTGVertexMul:                               return 57;
        case VS_VBatchPosition3dTexTGVertex:                             return 120;
        case VS_VBatchPosition3dTexTGVertexMul:                          return 121;
        case VS_VInstancedPosition3dTexTGVertex:                         return 184;
        case VS_VInstancedPosition3dTexTGVertexMul:                      return 185;
        case VS_VTexTGVertexCxform:                                      return 28;
        case VS_VTexTGVertexCxformMul:                                   return 29;
        case VS_VBatchTexTGVertexCxform:                                 return 92;
        case VS_VBatchTexTGVertexCxformMul:                              return 93;
        case VS_VInstancedTexTGVertexCxform:                             return 156;
        case VS_VInstancedTexTGVertexCxformMul:                          return 157;
        case VS_VPosition3dTexTGVertexCxform:                            return 60;
        case VS_VPosition3dTexTGVertexCxformMul:                         return 61;
        case VS_VBatchPosition3dTexTGVertexCxform:                       return 124;
        case VS_VBatchPosition3dTexTGVertexCxformMul:                    return 125;
        case VS_VInstancedPosition3dTexTGVertexCxform:                   return 188;
        case VS_VInstancedPosition3dTexTGVertexCxformMul:                return 189;
        case VS_VTexTGVertexEAlpha:                                      return 26;
        case VS_VTexTGVertexEAlphaMul:                                   return 27;
        case VS_VBatchTexTGVertexEAlpha:                                 return 90;
        case VS_VBatchTexTGVertexEAlphaMul:                              return 91;
        case VS_VInstancedTexTGVertexEAlpha:                             return 154;
        case VS_VInstancedTexTGVertexEAlphaMul:                          return 155;
        case VS_VPosition3dTexTGVertexEAlpha:                            return 58;
        case VS_VPosition3dTexTGVertexEAlphaMul:                         return 59;
        case VS_VBatchPosition3dTexTGVertexEAlpha:                       return 122;
        case VS_VBatchPosition3dTexTGVertexEAlphaMul:                    return 123;
        case VS_VInstancedPosition3dTexTGVertexEAlpha:                   return 186;
        case VS_VInstancedPosition3dTexTGVertexEAlphaMul:                return 187;
        case VS_VTexTGVertexCxformEAlpha:                                return 30;
        case VS_VTexTGVertexCxformEAlphaMul:                             return 31;
        case VS_VBatchTexTGVertexCxformEAlpha:                           return 94;
        case VS_VBatchTexTGVertexCxformEAlphaMul:                        return 95;
        case VS_VInstancedTexTGVertexCxformEAlpha:                       return 158;
        case VS_VInstancedTexTGVertexCxformEAlphaMul:                    return 159;
        case VS_VPosition3dTexTGVertexCxformEAlpha:                      return 62;
        case VS_VPosition3dTexTGVertexCxformEAlphaMul:                   return 63;
        case VS_VBatchPosition3dTexTGVertexCxformEAlpha:                 return 126;
        case VS_VBatchPosition3dTexTGVertexCxformEAlphaMul:              return 127;
        case VS_VInstancedPosition3dTexTGVertexCxformEAlpha:             return 190;
        case VS_VInstancedPosition3dTexTGVertexCxformEAlphaMul:          return 191;
        case VS_VSolid:                                                  return 192;
        case VS_VSolidMul:                                               return 193;
        case VS_VBatchSolid:                                             return 208;
        case VS_VBatchSolidMul:                                          return 209;
        case VS_VInstancedSolid:                                         return 224;
        case VS_VInstancedSolidMul:                                      return 225;
        case VS_VPosition3dSolid:                                        return 200;
        case VS_VPosition3dSolidMul:                                     return 201;
        case VS_VBatchPosition3dSolid:                                   return 216;
        case VS_VBatchPosition3dSolidMul:                                return 217;
        case VS_VInstancedPosition3dSolid:                               return 232;
        case VS_VInstancedPosition3dSolidMul:                            return 233;
        case VS_VText:                                                   return 194;
        case VS_VTextMul:                                                return 195;
        case VS_VBatchText:                                              return 210;
        case VS_VBatchTextMul:                                           return 211;
        case VS_VInstancedText:                                          return 226;
        case VS_VInstancedTextMul:                                       return 227;
        case VS_VPosition3dText:                                         return 202;
        case VS_VPosition3dTextMul:                                      return 203;
        case VS_VBatchPosition3dText:                                    return 218;
        case VS_VBatchPosition3dTextMul:                                 return 219;
        case VS_VInstancedPosition3dText:                                return 234;
        case VS_VInstancedPosition3dTextMul:                             return 235;
        case VS_VTextColor:                                              return 196;
        case VS_VTextColorMul:                                           return 197;
        case VS_VBatchTextColor:                                         return 212;
        case VS_VBatchTextColorMul:                                      return 213;
        case VS_VInstancedTextColor:                                     return 228;
        case VS_VInstancedTextColorMul:                                  return 229;
        case VS_VPosition3dTextColor:                                    return 204;
        case VS_VPosition3dTextColorMul:                                 return 205;
        case VS_VBatchPosition3dTextColor:                               return 220;
        case VS_VBatchPosition3dTextColorMul:                            return 221;
        case VS_VInstancedPosition3dTextColor:                           return 236;
        case VS_VInstancedPosition3dTextColorMul:                        return 237;
        case VS_VTextColorCxform:                                        return 198;
        case VS_VTextColorCxformMul:                                     return 199;
        case VS_VBatchTextColorCxform:                                   return 214;
        case VS_VBatchTextColorCxformMul:                                return 215;
        case VS_VInstancedTextColorCxform:                               return 230;
        case VS_VInstancedTextColorCxformMul:                            return 231;
        case VS_VPosition3dTextColorCxform:                              return 206;
        case VS_VPosition3dTextColorCxformMul:                           return 207;
        case VS_VBatchPosition3dTextColorCxform:                         return 222;
        case VS_VBatchPosition3dTextColorCxformMul:                      return 223;
        case VS_VInstancedPosition3dTextColorCxform:                     return 238;
        case VS_VInstancedPosition3dTextColorCxformMul:                  return 239;
        case VS_VYUV:                                                    return 240;
        case VS_VYUVMul:                                                 return 241;
        case VS_VBatchYUV:                                               return 272;
        case VS_VBatchYUVMul:                                            return 273;
        case VS_VInstancedYUV:                                           return 304;
        case VS_VInstancedYUVMul:                                        return 305;
        case VS_VPosition3dYUV:                                          return 256;
        case VS_VPosition3dYUVMul:                                       return 257;
        case VS_VBatchPosition3dYUV:                                     return 288;
        case VS_VBatchPosition3dYUVMul:                                  return 289;
        case VS_VInstancedPosition3dYUV:                                 return 320;
        case VS_VInstancedPosition3dYUVMul:                              return 321;
        case VS_VYUVCxform:                                              return 244;
        case VS_VYUVCxformMul:                                           return 245;
        case VS_VBatchYUVCxform:                                         return 276;
        case VS_VBatchYUVCxformMul:                                      return 277;
        case VS_VInstancedYUVCxform:                                     return 308;
        case VS_VInstancedYUVCxformMul:                                  return 309;
        case VS_VPosition3dYUVCxform:                                    return 260;
        case VS_VPosition3dYUVCxformMul:                                 return 261;
        case VS_VBatchPosition3dYUVCxform:                               return 292;
        case VS_VBatchPosition3dYUVCxformMul:                            return 293;
        case VS_VInstancedPosition3dYUVCxform:                           return 324;
        case VS_VInstancedPosition3dYUVCxformMul:                        return 325;
        case VS_VYUVEAlpha:                                              return 242;
        case VS_VYUVEAlphaMul:                                           return 243;
        case VS_VBatchYUVEAlpha:                                         return 274;
        case VS_VBatchYUVEAlphaMul:                                      return 275;
        case VS_VInstancedYUVEAlpha:                                     return 306;
        case VS_VInstancedYUVEAlphaMul:                                  return 307;
        case VS_VPosition3dYUVEAlpha:                                    return 258;
        case VS_VPosition3dYUVEAlphaMul:                                 return 259;
        case VS_VBatchPosition3dYUVEAlpha:                               return 290;
        case VS_VBatchPosition3dYUVEAlphaMul:                            return 291;
        case VS_VInstancedPosition3dYUVEAlpha:                           return 322;
        case VS_VInstancedPosition3dYUVEAlphaMul:                        return 323;
        case VS_VYUVCxformEAlpha:                                        return 246;
        case VS_VYUVCxformEAlphaMul:                                     return 247;
        case VS_VBatchYUVCxformEAlpha:                                   return 278;
        case VS_VBatchYUVCxformEAlphaMul:                                return 279;
        case VS_VInstancedYUVCxformEAlpha:                               return 310;
        case VS_VInstancedYUVCxformEAlphaMul:                            return 311;
        case VS_VPosition3dYUVCxformEAlpha:                              return 262;
        case VS_VPosition3dYUVCxformEAlphaMul:                           return 263;
        case VS_VBatchPosition3dYUVCxformEAlpha:                         return 294;
        case VS_VBatchPosition3dYUVCxformEAlphaMul:                      return 295;
        case VS_VInstancedPosition3dYUVCxformEAlpha:                     return 326;
        case VS_VInstancedPosition3dYUVCxformEAlphaMul:                  return 327;
        case VS_VYUVA:                                                   return 248;
        case VS_VYUVAMul:                                                return 249;
        case VS_VBatchYUVA:                                              return 280;
        case VS_VBatchYUVAMul:                                           return 281;
        case VS_VInstancedYUVA:                                          return 312;
        case VS_VInstancedYUVAMul:                                       return 313;
        case VS_VPosition3dYUVA:                                         return 264;
        case VS_VPosition3dYUVAMul:                                      return 265;
        case VS_VBatchPosition3dYUVA:                                    return 296;
        case VS_VBatchPosition3dYUVAMul:                                 return 297;
        case VS_VInstancedPosition3dYUVA:                                return 328;
        case VS_VInstancedPosition3dYUVAMul:                             return 329;
        case VS_VYUVACxform:                                             return 252;
        case VS_VYUVACxformMul:                                          return 253;
        case VS_VBatchYUVACxform:                                        return 284;
        case VS_VBatchYUVACxformMul:                                     return 285;
        case VS_VInstancedYUVACxform:                                    return 316;
        case VS_VInstancedYUVACxformMul:                                 return 317;
        case VS_VPosition3dYUVACxform:                                   return 268;
        case VS_VPosition3dYUVACxformMul:                                return 269;
        case VS_VBatchPosition3dYUVACxform:                              return 300;
        case VS_VBatchPosition3dYUVACxformMul:                           return 301;
        case VS_VInstancedPosition3dYUVACxform:                          return 332;
        case VS_VInstancedPosition3dYUVACxformMul:                       return 333;
        case VS_VYUVAEAlpha:                                             return 250;
        case VS_VYUVAEAlphaMul:                                          return 251;
        case VS_VBatchYUVAEAlpha:                                        return 282;
        case VS_VBatchYUVAEAlphaMul:                                     return 283;
        case VS_VInstancedYUVAEAlpha:                                    return 314;
        case VS_VInstancedYUVAEAlphaMul:                                 return 315;
        case VS_VPosition3dYUVAEAlpha:                                   return 266;
        case VS_VPosition3dYUVAEAlphaMul:                                return 267;
        case VS_VBatchPosition3dYUVAEAlpha:                              return 298;
        case VS_VBatchPosition3dYUVAEAlphaMul:                           return 299;
        case VS_VInstancedPosition3dYUVAEAlpha:                          return 330;
        case VS_VInstancedPosition3dYUVAEAlphaMul:                       return 331;
        case VS_VYUVACxformEAlpha:                                       return 254;
        case VS_VYUVACxformEAlphaMul:                                    return 255;
        case VS_VBatchYUVACxformEAlpha:                                  return 286;
        case VS_VBatchYUVACxformEAlphaMul:                               return 287;
        case VS_VInstancedYUVACxformEAlpha:                              return 318;
        case VS_VInstancedYUVACxformEAlphaMul:                           return 319;
        case VS_VPosition3dYUVACxformEAlpha:                             return 270;
        case VS_VPosition3dYUVACxformEAlphaMul:                          return 271;
        case VS_VBatchPosition3dYUVACxformEAlpha:                        return 302;
        case VS_VBatchPosition3dYUVACxformEAlphaMul:                     return 303;
        case VS_VInstancedPosition3dYUVACxformEAlpha:                    return 334;
        case VS_VInstancedPosition3dYUVACxformEAlphaMul:                 return 335;
        case VS_VTexTGCMatrixAc:                                         return 336;
        case VS_VTexTGCMatrixAcMul:                                      return 337;
        case VS_VTexTGCMatrixAcEAlpha:                                   return 338;
        case VS_VTexTGCMatrixAcEAlphaMul:                                return 339;
        case VS_VBox1Blur:                                               return 340;
        case VS_VBox1BlurMul:                                            return 341;
        case VS_VBox2Blur:                                               return 342;
        case VS_VBox2BlurMul:                                            return 343;
        case VS_VBox2Shadow:                                             return 344;
        case VS_VBox2ShadowMul:                                          return 345;
        case VS_VBox2ShadowKnockout:                                     return 346;
        case VS_VBox2ShadowKnockoutMul:                                  return 347;
        case VS_VBox2Shadowonly:                                         return 352;
        case VS_VBox2ShadowonlyMul:                                      return 353;
        case VS_VBox2InnerShadow:                                        return 348;
        case VS_VBox2InnerShadowMul:                                     return 349;
        case VS_VBox2InnerShadowKnockout:                                return 350;
        case VS_VBox2InnerShadowKnockoutMul:                             return 351;
        case VS_VBox2ShadowonlyHighlight:                                return 362;
        case VS_VBox2ShadowonlyHighlightMul:                             return 363;
        case VS_VBox2InnerShadowHighlight:                               return 358;
        case VS_VBox2InnerShadowHighlightMul:                            return 359;
        case VS_VBox2InnerShadowHighlightKnockout:                       return 360;
        case VS_VBox2InnerShadowHighlightKnockoutMul:                    return 361;
        case VS_VBox2ShadowHighlight:                                    return 354;
        case VS_VBox2ShadowHighlightMul:                                 return 355;
        case VS_VBox2ShadowHighlightKnockout:                            return 356;
        case VS_VBox2ShadowHighlightKnockoutMul:                         return 357;
        case VS_VDrawableCopyPixels:                                     return 364;
        case VS_VDrawableCopyPixelsCopyLerp:                             return 365;
        case VS_VDrawableCopyPixelsNoDestAlpha:                          return 366;
        case VS_VDrawableCopyPixelsNoDestAlphaCopyLerp:                  return 367;
        case VS_VDrawableCopyPixelsMergeAlpha:                           return 368;
        case VS_VDrawableCopyPixelsMergeAlphaCopyLerp:                   return 369;
        case VS_VDrawableCopyPixelsMergeAlphaNoDestAlpha:                return 370;
        case VS_VDrawableCopyPixelsMergeAlphaNoDestAlphaCopyLerp:        return 371;
        case VS_VDrawableCopyPixelsAlpha:                                return 372;
        case VS_VDrawableCopyPixelsAlphaCopyLerp:                        return 373;
        case VS_VDrawableCopyPixelsAlphaNoDestAlpha:                     return 374;
        case VS_VDrawableCopyPixelsAlphaNoDestAlphaCopyLerp:             return 375;
        case VS_VDrawableCopyPixelsAlphaMergeAlpha:                      return 376;
        case VS_VDrawableCopyPixelsAlphaMergeAlphaCopyLerp:              return 377;
        case VS_VDrawableCopyPixelsAlphaMergeAlphaNoDestAlpha:           return 378;
        case VS_VDrawableCopyPixelsAlphaMergeAlphaNoDestAlphaCopyLerp:   return 379;
        case VS_VDrawableMerge:                                          return 380;
        case VS_VDrawableCompare:                                        return 381;
        case VS_VDrawablePaletteMap:                                     return 382;
        default: SF_ASSERT(0); return 0;
    }
}

extern const char* pSource_FTexTG;
extern const char* pSource_FTexTGMul;
extern const char* pSource_FTexTGEAlpha;
extern const char* pSource_FTexTGEAlphaMul;
extern const char* pSource_FTexTGCxform;
extern const char* pSource_FTexTGCxformMul;
extern const char* pSource_FTexTGCxformEAlpha;
extern const char* pSource_FTexTGCxformEAlphaMul;
extern const char* pSource_FVertex;
extern const char* pSource_FVertexMul;
extern const char* pSource_FVertexEAlpha;
extern const char* pSource_FVertexEAlphaMul;
extern const char* pSource_FVertexCxform;
extern const char* pSource_FVertexCxformMul;
extern const char* pSource_FVertexCxformEAlpha;
extern const char* pSource_FVertexCxformEAlphaMul;
extern const char* pSource_FTexTGTexTG;
extern const char* pSource_FTexTGTexTGMul;
extern const char* pSource_FTexTGTexTGEAlpha;
extern const char* pSource_FTexTGTexTGEAlphaMul;
extern const char* pSource_FTexTGTexTGCxform;
extern const char* pSource_FTexTGTexTGCxformMul;
extern const char* pSource_FTexTGTexTGCxformEAlpha;
extern const char* pSource_FTexTGTexTGCxformEAlphaMul;
extern const char* pSource_FTexTGVertex;
extern const char* pSource_FTexTGVertexMul;
extern const char* pSource_FTexTGVertexEAlpha;
extern const char* pSource_FTexTGVertexEAlphaMul;
extern const char* pSource_FTexTGVertexCxform;
extern const char* pSource_FTexTGVertexCxformMul;
extern const char* pSource_FTexTGVertexCxformEAlpha;
extern const char* pSource_FTexTGVertexCxformEAlphaMul;
extern const char* pSource_FTexTG;
extern const char* pSource_FTexTGMul;
extern const char* pSource_FTexTGEAlpha;
extern const char* pSource_FTexTGEAlphaMul;
extern const char* pSource_FTexTGCxform;
extern const char* pSource_FTexTGCxformMul;
extern const char* pSource_FTexTGCxformEAlpha;
extern const char* pSource_FTexTGCxformEAlphaMul;
extern const char* pSource_FVertex;
extern const char* pSource_FVertexMul;
extern const char* pSource_FVertexEAlpha;
extern const char* pSource_FVertexEAlphaMul;
extern const char* pSource_FVertexCxform;
extern const char* pSource_FVertexCxformMul;
extern const char* pSource_FVertexCxformEAlpha;
extern const char* pSource_FVertexCxformEAlphaMul;
extern const char* pSource_FTexTGTexTG;
extern const char* pSource_FTexTGTexTGMul;
extern const char* pSource_FTexTGTexTGEAlpha;
extern const char* pSource_FTexTGTexTGEAlphaMul;
extern const char* pSource_FTexTGTexTGCxform;
extern const char* pSource_FTexTGTexTGCxformMul;
extern const char* pSource_FTexTGTexTGCxformEAlpha;
extern const char* pSource_FTexTGTexTGCxformEAlphaMul;
extern const char* pSource_FTexTGVertex;
extern const char* pSource_FTexTGVertexMul;
extern const char* pSource_FTexTGVertexEAlpha;
extern const char* pSource_FTexTGVertexEAlphaMul;
extern const char* pSource_FTexTGVertexCxform;
extern const char* pSource_FTexTGVertexCxformMul;
extern const char* pSource_FTexTGVertexCxformEAlpha;
extern const char* pSource_FTexTGVertexCxformEAlphaMul;
extern const char* pSource_FInstancedTexTG;
extern const char* pSource_FInstancedTexTGMul;
extern const char* pSource_FInstancedTexTGEAlpha;
extern const char* pSource_FInstancedTexTGEAlphaMul;
extern const char* pSource_FInstancedTexTGCxform;
extern const char* pSource_FInstancedTexTGCxformMul;
extern const char* pSource_FInstancedTexTGCxformEAlpha;
extern const char* pSource_FInstancedTexTGCxformEAlphaMul;
extern const char* pSource_FInstancedVertex;
extern const char* pSource_FInstancedVertexMul;
extern const char* pSource_FInstancedVertexEAlpha;
extern const char* pSource_FInstancedVertexEAlphaMul;
extern const char* pSource_FInstancedVertexCxform;
extern const char* pSource_FInstancedVertexCxformMul;
extern const char* pSource_FInstancedVertexCxformEAlpha;
extern const char* pSource_FInstancedVertexCxformEAlphaMul;
extern const char* pSource_FInstancedTexTGTexTG;
extern const char* pSource_FInstancedTexTGTexTGMul;
extern const char* pSource_FInstancedTexTGTexTGEAlpha;
extern const char* pSource_FInstancedTexTGTexTGEAlphaMul;
extern const char* pSource_FInstancedTexTGTexTGCxform;
extern const char* pSource_FInstancedTexTGTexTGCxformMul;
extern const char* pSource_FInstancedTexTGTexTGCxformEAlpha;
extern const char* pSource_FInstancedTexTGTexTGCxformEAlphaMul;
extern const char* pSource_FInstancedTexTGVertex;
extern const char* pSource_FInstancedTexTGVertexMul;
extern const char* pSource_FInstancedTexTGVertexEAlpha;
extern const char* pSource_FInstancedTexTGVertexEAlphaMul;
extern const char* pSource_FInstancedTexTGVertexCxform;
extern const char* pSource_FInstancedTexTGVertexCxformMul;
extern const char* pSource_FInstancedTexTGVertexCxformEAlpha;
extern const char* pSource_FInstancedTexTGVertexCxformEAlphaMul;
extern const char* pSource_FSolid;
extern const char* pSource_FSolidMul;
extern const char* pSource_FText;
extern const char* pSource_FTextMul;
extern const char* pSource_FSolid;
extern const char* pSource_FSolidMul;
extern const char* pSource_FText;
extern const char* pSource_FTextMul;
extern const char* pSource_FInstancedSolid;
extern const char* pSource_FInstancedSolidMul;
extern const char* pSource_FInstancedText;
extern const char* pSource_FInstancedTextMul;
extern const char* pSource_FYUV;
extern const char* pSource_FYUVMul;
extern const char* pSource_FYUVEAlpha;
extern const char* pSource_FYUVEAlphaMul;
extern const char* pSource_FYUVCxform;
extern const char* pSource_FYUVCxformMul;
extern const char* pSource_FYUVCxformEAlpha;
extern const char* pSource_FYUVCxformEAlphaMul;
extern const char* pSource_FYUVA;
extern const char* pSource_FYUVAMul;
extern const char* pSource_FYUVAEAlpha;
extern const char* pSource_FYUVAEAlphaMul;
extern const char* pSource_FYUVACxform;
extern const char* pSource_FYUVACxformMul;
extern const char* pSource_FYUVACxformEAlpha;
extern const char* pSource_FYUVACxformEAlphaMul;
extern const char* pSource_FYUV;
extern const char* pSource_FYUVMul;
extern const char* pSource_FYUVEAlpha;
extern const char* pSource_FYUVEAlphaMul;
extern const char* pSource_FYUVCxform;
extern const char* pSource_FYUVCxformMul;
extern const char* pSource_FYUVCxformEAlpha;
extern const char* pSource_FYUVCxformEAlphaMul;
extern const char* pSource_FYUVA;
extern const char* pSource_FYUVAMul;
extern const char* pSource_FYUVAEAlpha;
extern const char* pSource_FYUVAEAlphaMul;
extern const char* pSource_FYUVACxform;
extern const char* pSource_FYUVACxformMul;
extern const char* pSource_FYUVACxformEAlpha;
extern const char* pSource_FYUVACxformEAlphaMul;
extern const char* pSource_FInstancedYUV;
extern const char* pSource_FInstancedYUVMul;
extern const char* pSource_FInstancedYUVEAlpha;
extern const char* pSource_FInstancedYUVEAlphaMul;
extern const char* pSource_FInstancedYUVCxform;
extern const char* pSource_FInstancedYUVCxformMul;
extern const char* pSource_FInstancedYUVCxformEAlpha;
extern const char* pSource_FInstancedYUVCxformEAlphaMul;
extern const char* pSource_FInstancedYUVA;
extern const char* pSource_FInstancedYUVAMul;
extern const char* pSource_FInstancedYUVAEAlpha;
extern const char* pSource_FInstancedYUVAEAlphaMul;
extern const char* pSource_FInstancedYUVACxform;
extern const char* pSource_FInstancedYUVACxformMul;
extern const char* pSource_FInstancedYUVACxformEAlpha;
extern const char* pSource_FInstancedYUVACxformEAlphaMul;
extern const char* pSource_FTexTGCMatrixAc;
extern const char* pSource_FTexTGCMatrixAcMul;
extern const char* pSource_FTexTGCMatrixAcEAlpha;
extern const char* pSource_FTexTGCMatrixAcEAlphaMul;
extern const char* pSource_FBox1Blur;
extern const char* pSource_FBox1BlurMul;
extern const char* pSource_FBox2Blur;
extern const char* pSource_FBox2BlurMul;
extern const char* pSource_FBox2Shadow;
extern const char* pSource_FBox2ShadowMul;
extern const char* pSource_FBox2ShadowKnockout;
extern const char* pSource_FBox2ShadowKnockoutMul;
extern const char* pSource_FBox2InnerShadow;
extern const char* pSource_FBox2InnerShadowMul;
extern const char* pSource_FBox2InnerShadowKnockout;
extern const char* pSource_FBox2InnerShadowKnockoutMul;
extern const char* pSource_FBox2Shadowonly;
extern const char* pSource_FBox2ShadowonlyMul;
extern const char* pSource_FBox2ShadowHighlight;
extern const char* pSource_FBox2ShadowHighlightMul;
extern const char* pSource_FBox2ShadowHighlightKnockout;
extern const char* pSource_FBox2ShadowHighlightKnockoutMul;
extern const char* pSource_FBox2InnerShadowHighlight;
extern const char* pSource_FBox2InnerShadowHighlightMul;
extern const char* pSource_FBox2InnerShadowHighlightKnockout;
extern const char* pSource_FBox2InnerShadowHighlightKnockoutMul;
extern const char* pSource_FBox2ShadowonlyHighlight;
extern const char* pSource_FBox2ShadowonlyHighlightMul;
extern const char* pSource_FDrawableCopyPixels;
extern const char* pSource_FDrawableCopyPixelsCopyLerp;
extern const char* pSource_FDrawableCopyPixelsNoDestAlpha;
extern const char* pSource_FDrawableCopyPixelsNoDestAlphaCopyLerp;
extern const char* pSource_FDrawableCopyPixelsMergeAlpha;
extern const char* pSource_FDrawableCopyPixelsMergeAlphaCopyLerp;
extern const char* pSource_FDrawableCopyPixelsMergeAlphaNoDestAlpha;
extern const char* pSource_FDrawableCopyPixelsMergeAlphaNoDestAlphaCopyLerp;
extern const char* pSource_FDrawableCopyPixelsAlpha;
extern const char* pSource_FDrawableCopyPixelsAlphaCopyLerp;
extern const char* pSource_FDrawableCopyPixelsAlphaNoDestAlpha;
extern const char* pSource_FDrawableCopyPixelsAlphaNoDestAlphaCopyLerp;
extern const char* pSource_FDrawableCopyPixelsAlphaMergeAlpha;
extern const char* pSource_FDrawableCopyPixelsAlphaMergeAlphaCopyLerp;
extern const char* pSource_FDrawableCopyPixelsAlphaMergeAlphaNoDestAlpha;
extern const char* pSource_FDrawableCopyPixelsAlphaMergeAlphaNoDestAlphaCopyLerp;
extern const char* pSource_FDrawableMerge;
extern const char* pSource_FDrawableCompare;
extern const char* pSource_FDrawablePaletteMap;

static FragShaderDesc ShaderDesc_FS_FTexTG = {
    /* Type */          FragShaderDesc::FS_FTexTG,
    /* Flags */         0,
    /* pSource */       pSource_FTexTG,
    /* Uniforms */      { 
                          /* cxadd*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      { -1, 0, 0, 0, 0, 0 },
                          /* mvp*/        { -1, 0, 0, 0, 0, 0 },
                          /* offset*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor2*/    { -1, 0, 0, 0, 0, 0 },
                          /* srctex*/     { -1, 0, 0, 0, 0, 0 },
                          /* srctexscale*/{ -1, 0, 0, 0, 0, 0 },
                          /* tex*/        {0, 1032, 1, 1, 1, 0 },
                          /* texgen*/     { -1, 0, 0, 0, 0, 0 },
                          /* texscale*/   { -1, 0, 0, 0, 0, 0 },
                          /* vfmuniforms*/{ -1, 0, 0, 0, 0, 0 },
                          /* vfuniforms*/ { -1, 0, 0, 0, 0, 0 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_Count, -1, 0},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_Count, -1, 0},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    
    };

static FragShaderDesc ShaderDesc_FS_FTexTGMul = {
    /* Type */          FragShaderDesc::FS_FTexTGMul,
    /* Flags */         0,
    /* pSource */       pSource_FTexTGMul,
    /* Uniforms */      { 
                          /* cxadd*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      { -1, 0, 0, 0, 0, 0 },
                          /* mvp*/        { -1, 0, 0, 0, 0, 0 },
                          /* offset*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor2*/    { -1, 0, 0, 0, 0, 0 },
                          /* srctex*/     { -1, 0, 0, 0, 0, 0 },
                          /* srctexscale*/{ -1, 0, 0, 0, 0, 0 },
                          /* tex*/        {0, 1032, 1, 1, 1, 0 },
                          /* texgen*/     { -1, 0, 0, 0, 0, 0 },
                          /* texscale*/   { -1, 0, 0, 0, 0, 0 },
                          /* vfmuniforms*/{ -1, 0, 0, 0, 0, 0 },
                          /* vfuniforms*/ { -1, 0, 0, 0, 0, 0 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_Count, -1, 0},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_Count, -1, 0},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    
    };

static FragShaderDesc ShaderDesc_FS_FTexTGEAlpha = {
    /* Type */          FragShaderDesc::FS_FTexTGEAlpha,
    /* Flags */         0,
    /* pSource */       pSource_FTexTGEAlpha,
    /* Uniforms */      { 
                          /* cxadd*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      { -1, 0, 0, 0, 0, 0 },
                          /* mvp*/        { -1, 0, 0, 0, 0, 0 },
                          /* offset*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor2*/    { -1, 0, 0, 0, 0, 0 },
                          /* srctex*/     { -1, 0, 0, 0, 0, 0 },
                          /* srctexscale*/{ -1, 0, 0, 0, 0, 0 },
                          /* tex*/        {0, 1032, 1, 1, 1, 0 },
                          /* texgen*/     { -1, 0, 0, 0, 0, 0 },
                          /* texscale*/   { -1, 0, 0, 0, 0, 0 },
                          /* vfmuniforms*/{ -1, 0, 0, 0, 0, 0 },
                          /* vfuniforms*/ { -1, 0, 0, 0, 0, 0 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_Count, -1, 0},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_Count, -1, 0},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    
    };

static FragShaderDesc ShaderDesc_FS_FTexTGEAlphaMul = {
    /* Type */          FragShaderDesc::FS_FTexTGEAlphaMul,
    /* Flags */         0,
    /* pSource */       pSource_FTexTGEAlphaMul,
    /* Uniforms */      { 
                          /* cxadd*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      { -1, 0, 0, 0, 0, 0 },
                          /* mvp*/        { -1, 0, 0, 0, 0, 0 },
                          /* offset*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor2*/    { -1, 0, 0, 0, 0, 0 },
                          /* srctex*/     { -1, 0, 0, 0, 0, 0 },
                          /* srctexscale*/{ -1, 0, 0, 0, 0, 0 },
                          /* tex*/        {0, 1032, 1, 1, 1, 0 },
                          /* texgen*/     { -1, 0, 0, 0, 0, 0 },
                          /* texscale*/   { -1, 0, 0, 0, 0, 0 },
                          /* vfmuniforms*/{ -1, 0, 0, 0, 0, 0 },
                          /* vfuniforms*/ { -1, 0, 0, 0, 0, 0 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_Count, -1, 0},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_Count, -1, 0},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    
    };

static FragShaderDesc ShaderDesc_FS_FTexTGCxform = {
    /* Type */          FragShaderDesc::FS_FTexTGCxform,
    /* Flags */         0,
    /* pSource */       pSource_FTexTGCxform,
    /* Uniforms */      { 
                          /* cxadd*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      { -1, 0, 0, 0, 0, 0 },
                          /* mvp*/        { -1, 0, 0, 0, 0, 0 },
                          /* offset*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor2*/    { -1, 0, 0, 0, 0, 0 },
                          /* srctex*/     { -1, 0, 0, 0, 0, 0 },
                          /* srctexscale*/{ -1, 0, 0, 0, 0, 0 },
                          /* tex*/        {0, 1032, 1, 1, 1, 0 },
                          /* texgen*/     { -1, 0, 0, 0, 0, 0 },
                          /* texscale*/   { -1, 0, 0, 0, 0, 0 },
                          /* vfmuniforms*/{ -1, 0, 0, 0, 0, 0 },
                          /* vfuniforms*/ { -1, 0, 0, 0, 0, 0 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_Count, -1, 0},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_Count, -1, 0},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    
    };

static FragShaderDesc ShaderDesc_FS_FTexTGCxformMul = {
    /* Type */          FragShaderDesc::FS_FTexTGCxformMul,
    /* Flags */         0,
    /* pSource */       pSource_FTexTGCxformMul,
    /* Uniforms */      { 
                          /* cxadd*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      { -1, 0, 0, 0, 0, 0 },
                          /* mvp*/        { -1, 0, 0, 0, 0, 0 },
                          /* offset*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor2*/    { -1, 0, 0, 0, 0, 0 },
                          /* srctex*/     { -1, 0, 0, 0, 0, 0 },
                          /* srctexscale*/{ -1, 0, 0, 0, 0, 0 },
                          /* tex*/        {0, 1032, 1, 1, 1, 0 },
                          /* texgen*/     { -1, 0, 0, 0, 0, 0 },
                          /* texscale*/   { -1, 0, 0, 0, 0, 0 },
                          /* vfmuniforms*/{ -1, 0, 0, 0, 0, 0 },
                          /* vfuniforms*/ { -1, 0, 0, 0, 0, 0 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_Count, -1, 0},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_Count, -1, 0},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    
    };

static FragShaderDesc ShaderDesc_FS_FTexTGCxformEAlpha = {
    /* Type */          FragShaderDesc::FS_FTexTGCxformEAlpha,
    /* Flags */         0,
    /* pSource */       pSource_FTexTGCxformEAlpha,
    /* Uniforms */      { 
                          /* cxadd*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      { -1, 0, 0, 0, 0, 0 },
                          /* mvp*/        { -1, 0, 0, 0, 0, 0 },
                          /* offset*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor2*/    { -1, 0, 0, 0, 0, 0 },
                          /* srctex*/     { -1, 0, 0, 0, 0, 0 },
                          /* srctexscale*/{ -1, 0, 0, 0, 0, 0 },
                          /* tex*/        {0, 1032, 1, 1, 1, 0 },
                          /* texgen*/     { -1, 0, 0, 0, 0, 0 },
                          /* texscale*/   { -1, 0, 0, 0, 0, 0 },
                          /* vfmuniforms*/{ -1, 0, 0, 0, 0, 0 },
                          /* vfuniforms*/ { -1, 0, 0, 0, 0, 0 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_Count, -1, 0},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_Count, -1, 0},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    
    };

static FragShaderDesc ShaderDesc_FS_FTexTGCxformEAlphaMul = {
    /* Type */          FragShaderDesc::FS_FTexTGCxformEAlphaMul,
    /* Flags */         0,
    /* pSource */       pSource_FTexTGCxformEAlphaMul,
    /* Uniforms */      { 
                          /* cxadd*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      { -1, 0, 0, 0, 0, 0 },
                          /* mvp*/        { -1, 0, 0, 0, 0, 0 },
                          /* offset*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor2*/    { -1, 0, 0, 0, 0, 0 },
                          /* srctex*/     { -1, 0, 0, 0, 0, 0 },
                          /* srctexscale*/{ -1, 0, 0, 0, 0, 0 },
                          /* tex*/        {0, 1032, 1, 1, 1, 0 },
                          /* texgen*/     { -1, 0, 0, 0, 0, 0 },
                          /* texscale*/   { -1, 0, 0, 0, 0, 0 },
                          /* vfmuniforms*/{ -1, 0, 0, 0, 0, 0 },
                          /* vfuniforms*/ { -1, 0, 0, 0, 0, 0 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_Count, -1, 0},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_Count, -1, 0},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    
    };

static FragShaderDesc ShaderDesc_FS_FVertex = {
    /* Type */          FragShaderDesc::FS_FVertex,
    /* Flags */         0,
    /* pSource */       pSource_FVertex,
    /* Uniforms */      { 
                          /* cxadd*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      { -1, 0, 0, 0, 0, 0 },
                          /* mvp*/        { -1, 0, 0, 0, 0, 0 },
                          /* offset*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor2*/    { -1, 0, 0, 0, 0, 0 },
                          /* srctex*/     { -1, 0, 0, 0, 0, 0 },
                          /* srctexscale*/{ -1, 0, 0, 0, 0, 0 },
                          /* tex*/        { -1, 0, 0, 0, 0, 0 },
                          /* texgen*/     { -1, 0, 0, 0, 0, 0 },
                          /* texscale*/   { -1, 0, 0, 0, 0, 0 },
                          /* vfmuniforms*/{ -1, 0, 0, 0, 0, 0 },
                          /* vfuniforms*/ { -1, 0, 0, 0, 0, 0 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_Count, -1, 0},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_Count, -1, 0},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    
    };

static FragShaderDesc ShaderDesc_FS_FVertexMul = {
    /* Type */          FragShaderDesc::FS_FVertexMul,
    /* Flags */         0,
    /* pSource */       pSource_FVertexMul,
    /* Uniforms */      { 
                          /* cxadd*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      { -1, 0, 0, 0, 0, 0 },
                          /* mvp*/        { -1, 0, 0, 0, 0, 0 },
                          /* offset*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor2*/    { -1, 0, 0, 0, 0, 0 },
                          /* srctex*/     { -1, 0, 0, 0, 0, 0 },
                          /* srctexscale*/{ -1, 0, 0, 0, 0, 0 },
                          /* tex*/        { -1, 0, 0, 0, 0, 0 },
                          /* texgen*/     { -1, 0, 0, 0, 0, 0 },
                          /* texscale*/   { -1, 0, 0, 0, 0, 0 },
                          /* vfmuniforms*/{ -1, 0, 0, 0, 0, 0 },
                          /* vfuniforms*/ { -1, 0, 0, 0, 0, 0 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_Count, -1, 0},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_Count, -1, 0},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    
    };

static FragShaderDesc ShaderDesc_FS_FVertexEAlpha = {
    /* Type */          FragShaderDesc::FS_FVertexEAlpha,
    /* Flags */         0,
    /* pSource */       pSource_FVertexEAlpha,
    /* Uniforms */      { 
                          /* cxadd*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      { -1, 0, 0, 0, 0, 0 },
                          /* mvp*/        { -1, 0, 0, 0, 0, 0 },
                          /* offset*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor2*/    { -1, 0, 0, 0, 0, 0 },
                          /* srctex*/     { -1, 0, 0, 0, 0, 0 },
                          /* srctexscale*/{ -1, 0, 0, 0, 0, 0 },
                          /* tex*/        { -1, 0, 0, 0, 0, 0 },
                          /* texgen*/     { -1, 0, 0, 0, 0, 0 },
                          /* texscale*/   { -1, 0, 0, 0, 0, 0 },
                          /* vfmuniforms*/{ -1, 0, 0, 0, 0, 0 },
                          /* vfuniforms*/ { -1, 0, 0, 0, 0, 0 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_Count, -1, 0},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_Count, -1, 0},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    
    };

static FragShaderDesc ShaderDesc_FS_FVertexEAlphaMul = {
    /* Type */          FragShaderDesc::FS_FVertexEAlphaMul,
    /* Flags */         0,
    /* pSource */       pSource_FVertexEAlphaMul,
    /* Uniforms */      { 
                          /* cxadd*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      { -1, 0, 0, 0, 0, 0 },
                          /* mvp*/        { -1, 0, 0, 0, 0, 0 },
                          /* offset*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor2*/    { -1, 0, 0, 0, 0, 0 },
                          /* srctex*/     { -1, 0, 0, 0, 0, 0 },
                          /* srctexscale*/{ -1, 0, 0, 0, 0, 0 },
                          /* tex*/        { -1, 0, 0, 0, 0, 0 },
                          /* texgen*/     { -1, 0, 0, 0, 0, 0 },
                          /* texscale*/   { -1, 0, 0, 0, 0, 0 },
                          /* vfmuniforms*/{ -1, 0, 0, 0, 0, 0 },
                          /* vfuniforms*/ { -1, 0, 0, 0, 0, 0 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_Count, -1, 0},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_Count, -1, 0},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    
    };

static FragShaderDesc ShaderDesc_FS_FVertexCxform = {
    /* Type */          FragShaderDesc::FS_FVertexCxform,
    /* Flags */         0,
    /* pSource */       pSource_FVertexCxform,
    /* Uniforms */      { 
                          /* cxadd*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      { -1, 0, 0, 0, 0, 0 },
                          /* mvp*/        { -1, 0, 0, 0, 0, 0 },
                          /* offset*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor2*/    { -1, 0, 0, 0, 0, 0 },
                          /* srctex*/     { -1, 0, 0, 0, 0, 0 },
                          /* srctexscale*/{ -1, 0, 0, 0, 0, 0 },
                          /* tex*/        { -1, 0, 0, 0, 0, 0 },
                          /* texgen*/     { -1, 0, 0, 0, 0, 0 },
                          /* texscale*/   { -1, 0, 0, 0, 0, 0 },
                          /* vfmuniforms*/{ -1, 0, 0, 0, 0, 0 },
                          /* vfuniforms*/ { -1, 0, 0, 0, 0, 0 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_Count, -1, 0},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_Count, -1, 0},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    
    };

static FragShaderDesc ShaderDesc_FS_FVertexCxformMul = {
    /* Type */          FragShaderDesc::FS_FVertexCxformMul,
    /* Flags */         0,
    /* pSource */       pSource_FVertexCxformMul,
    /* Uniforms */      { 
                          /* cxadd*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      { -1, 0, 0, 0, 0, 0 },
                          /* mvp*/        { -1, 0, 0, 0, 0, 0 },
                          /* offset*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor2*/    { -1, 0, 0, 0, 0, 0 },
                          /* srctex*/     { -1, 0, 0, 0, 0, 0 },
                          /* srctexscale*/{ -1, 0, 0, 0, 0, 0 },
                          /* tex*/        { -1, 0, 0, 0, 0, 0 },
                          /* texgen*/     { -1, 0, 0, 0, 0, 0 },
                          /* texscale*/   { -1, 0, 0, 0, 0, 0 },
                          /* vfmuniforms*/{ -1, 0, 0, 0, 0, 0 },
                          /* vfuniforms*/ { -1, 0, 0, 0, 0, 0 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_Count, -1, 0},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_Count, -1, 0},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    
    };

static FragShaderDesc ShaderDesc_FS_FVertexCxformEAlpha = {
    /* Type */          FragShaderDesc::FS_FVertexCxformEAlpha,
    /* Flags */         0,
    /* pSource */       pSource_FVertexCxformEAlpha,
    /* Uniforms */      { 
                          /* cxadd*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      { -1, 0, 0, 0, 0, 0 },
                          /* mvp*/        { -1, 0, 0, 0, 0, 0 },
                          /* offset*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor2*/    { -1, 0, 0, 0, 0, 0 },
                          /* srctex*/     { -1, 0, 0, 0, 0, 0 },
                          /* srctexscale*/{ -1, 0, 0, 0, 0, 0 },
                          /* tex*/        { -1, 0, 0, 0, 0, 0 },
                          /* texgen*/     { -1, 0, 0, 0, 0, 0 },
                          /* texscale*/   { -1, 0, 0, 0, 0, 0 },
                          /* vfmuniforms*/{ -1, 0, 0, 0, 0, 0 },
                          /* vfuniforms*/ { -1, 0, 0, 0, 0, 0 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_Count, -1, 0},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_Count, -1, 0},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    
    };

static FragShaderDesc ShaderDesc_FS_FVertexCxformEAlphaMul = {
    /* Type */          FragShaderDesc::FS_FVertexCxformEAlphaMul,
    /* Flags */         0,
    /* pSource */       pSource_FVertexCxformEAlphaMul,
    /* Uniforms */      { 
                          /* cxadd*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      { -1, 0, 0, 0, 0, 0 },
                          /* mvp*/        { -1, 0, 0, 0, 0, 0 },
                          /* offset*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor2*/    { -1, 0, 0, 0, 0, 0 },
                          /* srctex*/     { -1, 0, 0, 0, 0, 0 },
                          /* srctexscale*/{ -1, 0, 0, 0, 0, 0 },
                          /* tex*/        { -1, 0, 0, 0, 0, 0 },
                          /* texgen*/     { -1, 0, 0, 0, 0, 0 },
                          /* texscale*/   { -1, 0, 0, 0, 0, 0 },
                          /* vfmuniforms*/{ -1, 0, 0, 0, 0, 0 },
                          /* vfuniforms*/ { -1, 0, 0, 0, 0, 0 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_Count, -1, 0},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_Count, -1, 0},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    
    };

static FragShaderDesc ShaderDesc_FS_FTexTGTexTG = {
    /* Type */          FragShaderDesc::FS_FTexTGTexTG,
    /* Flags */         0,
    /* pSource */       pSource_FTexTGTexTG,
    /* Uniforms */      { 
                          /* cxadd*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      { -1, 0, 0, 0, 0, 0 },
                          /* mvp*/        { -1, 0, 0, 0, 0, 0 },
                          /* offset*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor2*/    { -1, 0, 0, 0, 0, 0 },
                          /* srctex*/     { -1, 0, 0, 0, 0, 0 },
                          /* srctexscale*/{ -1, 0, 0, 0, 0, 0 },
                          /* tex*/        {0, 1032, 1, 2, 1, 0 },
                          /* texgen*/     { -1, 0, 0, 0, 0, 0 },
                          /* texscale*/   { -1, 0, 0, 0, 0, 0 },
                          /* vfmuniforms*/{ -1, 0, 0, 0, 0, 0 },
                          /* vfuniforms*/ { -1, 0, 0, 0, 0, 0 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_Count, -1, 0},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_Count, -1, 0},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    
    };

static FragShaderDesc ShaderDesc_FS_FTexTGTexTGMul = {
    /* Type */          FragShaderDesc::FS_FTexTGTexTGMul,
    /* Flags */         0,
    /* pSource */       pSource_FTexTGTexTGMul,
    /* Uniforms */      { 
                          /* cxadd*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      { -1, 0, 0, 0, 0, 0 },
                          /* mvp*/        { -1, 0, 0, 0, 0, 0 },
                          /* offset*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor2*/    { -1, 0, 0, 0, 0, 0 },
                          /* srctex*/     { -1, 0, 0, 0, 0, 0 },
                          /* srctexscale*/{ -1, 0, 0, 0, 0, 0 },
                          /* tex*/        {0, 1032, 1, 2, 1, 0 },
                          /* texgen*/     { -1, 0, 0, 0, 0, 0 },
                          /* texscale*/   { -1, 0, 0, 0, 0, 0 },
                          /* vfmuniforms*/{ -1, 0, 0, 0, 0, 0 },
                          /* vfuniforms*/ { -1, 0, 0, 0, 0, 0 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_Count, -1, 0},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_Count, -1, 0},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    
    };

static FragShaderDesc ShaderDesc_FS_FTexTGTexTGEAlpha = {
    /* Type */          FragShaderDesc::FS_FTexTGTexTGEAlpha,
    /* Flags */         0,
    /* pSource */       pSource_FTexTGTexTGEAlpha,
    /* Uniforms */      { 
                          /* cxadd*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      { -1, 0, 0, 0, 0, 0 },
                          /* mvp*/        { -1, 0, 0, 0, 0, 0 },
                          /* offset*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor2*/    { -1, 0, 0, 0, 0, 0 },
                          /* srctex*/     { -1, 0, 0, 0, 0, 0 },
                          /* srctexscale*/{ -1, 0, 0, 0, 0, 0 },
                          /* tex*/        {0, 1032, 1, 2, 1, 0 },
                          /* texgen*/     { -1, 0, 0, 0, 0, 0 },
                          /* texscale*/   { -1, 0, 0, 0, 0, 0 },
                          /* vfmuniforms*/{ -1, 0, 0, 0, 0, 0 },
                          /* vfuniforms*/ { -1, 0, 0, 0, 0, 0 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_Count, -1, 0},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_Count, -1, 0},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    
    };

static FragShaderDesc ShaderDesc_FS_FTexTGTexTGEAlphaMul = {
    /* Type */          FragShaderDesc::FS_FTexTGTexTGEAlphaMul,
    /* Flags */         0,
    /* pSource */       pSource_FTexTGTexTGEAlphaMul,
    /* Uniforms */      { 
                          /* cxadd*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      { -1, 0, 0, 0, 0, 0 },
                          /* mvp*/        { -1, 0, 0, 0, 0, 0 },
                          /* offset*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor2*/    { -1, 0, 0, 0, 0, 0 },
                          /* srctex*/     { -1, 0, 0, 0, 0, 0 },
                          /* srctexscale*/{ -1, 0, 0, 0, 0, 0 },
                          /* tex*/        {0, 1032, 1, 2, 1, 0 },
                          /* texgen*/     { -1, 0, 0, 0, 0, 0 },
                          /* texscale*/   { -1, 0, 0, 0, 0, 0 },
                          /* vfmuniforms*/{ -1, 0, 0, 0, 0, 0 },
                          /* vfuniforms*/ { -1, 0, 0, 0, 0, 0 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_Count, -1, 0},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_Count, -1, 0},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    
    };

static FragShaderDesc ShaderDesc_FS_FTexTGTexTGCxform = {
    /* Type */          FragShaderDesc::FS_FTexTGTexTGCxform,
    /* Flags */         0,
    /* pSource */       pSource_FTexTGTexTGCxform,
    /* Uniforms */      { 
                          /* cxadd*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      { -1, 0, 0, 0, 0, 0 },
                          /* mvp*/        { -1, 0, 0, 0, 0, 0 },
                          /* offset*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor2*/    { -1, 0, 0, 0, 0, 0 },
                          /* srctex*/     { -1, 0, 0, 0, 0, 0 },
                          /* srctexscale*/{ -1, 0, 0, 0, 0, 0 },
                          /* tex*/        {0, 1032, 1, 2, 1, 0 },
                          /* texgen*/     { -1, 0, 0, 0, 0, 0 },
                          /* texscale*/   { -1, 0, 0, 0, 0, 0 },
                          /* vfmuniforms*/{ -1, 0, 0, 0, 0, 0 },
                          /* vfuniforms*/ { -1, 0, 0, 0, 0, 0 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_Count, -1, 0},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_Count, -1, 0},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    
    };

static FragShaderDesc ShaderDesc_FS_FTexTGTexTGCxformMul = {
    /* Type */          FragShaderDesc::FS_FTexTGTexTGCxformMul,
    /* Flags */         0,
    /* pSource */       pSource_FTexTGTexTGCxformMul,
    /* Uniforms */      { 
                          /* cxadd*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      { -1, 0, 0, 0, 0, 0 },
                          /* mvp*/        { -1, 0, 0, 0, 0, 0 },
                          /* offset*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor2*/    { -1, 0, 0, 0, 0, 0 },
                          /* srctex*/     { -1, 0, 0, 0, 0, 0 },
                          /* srctexscale*/{ -1, 0, 0, 0, 0, 0 },
                          /* tex*/        {0, 1032, 1, 2, 1, 0 },
                          /* texgen*/     { -1, 0, 0, 0, 0, 0 },
                          /* texscale*/   { -1, 0, 0, 0, 0, 0 },
                          /* vfmuniforms*/{ -1, 0, 0, 0, 0, 0 },
                          /* vfuniforms*/ { -1, 0, 0, 0, 0, 0 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_Count, -1, 0},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_Count, -1, 0},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    
    };

static FragShaderDesc ShaderDesc_FS_FTexTGTexTGCxformEAlpha = {
    /* Type */          FragShaderDesc::FS_FTexTGTexTGCxformEAlpha,
    /* Flags */         0,
    /* pSource */       pSource_FTexTGTexTGCxformEAlpha,
    /* Uniforms */      { 
                          /* cxadd*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      { -1, 0, 0, 0, 0, 0 },
                          /* mvp*/        { -1, 0, 0, 0, 0, 0 },
                          /* offset*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor2*/    { -1, 0, 0, 0, 0, 0 },
                          /* srctex*/     { -1, 0, 0, 0, 0, 0 },
                          /* srctexscale*/{ -1, 0, 0, 0, 0, 0 },
                          /* tex*/        {0, 1032, 1, 2, 1, 0 },
                          /* texgen*/     { -1, 0, 0, 0, 0, 0 },
                          /* texscale*/   { -1, 0, 0, 0, 0, 0 },
                          /* vfmuniforms*/{ -1, 0, 0, 0, 0, 0 },
                          /* vfuniforms*/ { -1, 0, 0, 0, 0, 0 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_Count, -1, 0},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_Count, -1, 0},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    
    };

static FragShaderDesc ShaderDesc_FS_FTexTGTexTGCxformEAlphaMul = {
    /* Type */          FragShaderDesc::FS_FTexTGTexTGCxformEAlphaMul,
    /* Flags */         0,
    /* pSource */       pSource_FTexTGTexTGCxformEAlphaMul,
    /* Uniforms */      { 
                          /* cxadd*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      { -1, 0, 0, 0, 0, 0 },
                          /* mvp*/        { -1, 0, 0, 0, 0, 0 },
                          /* offset*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor2*/    { -1, 0, 0, 0, 0, 0 },
                          /* srctex*/     { -1, 0, 0, 0, 0, 0 },
                          /* srctexscale*/{ -1, 0, 0, 0, 0, 0 },
                          /* tex*/        {0, 1032, 1, 2, 1, 0 },
                          /* texgen*/     { -1, 0, 0, 0, 0, 0 },
                          /* texscale*/   { -1, 0, 0, 0, 0, 0 },
                          /* vfmuniforms*/{ -1, 0, 0, 0, 0, 0 },
                          /* vfuniforms*/ { -1, 0, 0, 0, 0, 0 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_Count, -1, 0},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_Count, -1, 0},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    
    };

static FragShaderDesc ShaderDesc_FS_FTexTGVertex = {
    /* Type */          FragShaderDesc::FS_FTexTGVertex,
    /* Flags */         0,
    /* pSource */       pSource_FTexTGVertex,
    /* Uniforms */      { 
                          /* cxadd*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      { -1, 0, 0, 0, 0, 0 },
                          /* mvp*/        { -1, 0, 0, 0, 0, 0 },
                          /* offset*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor2*/    { -1, 0, 0, 0, 0, 0 },
                          /* srctex*/     { -1, 0, 0, 0, 0, 0 },
                          /* srctexscale*/{ -1, 0, 0, 0, 0, 0 },
                          /* tex*/        {0, 1032, 1, 1, 1, 0 },
                          /* texgen*/     { -1, 0, 0, 0, 0, 0 },
                          /* texscale*/   { -1, 0, 0, 0, 0, 0 },
                          /* vfmuniforms*/{ -1, 0, 0, 0, 0, 0 },
                          /* vfuniforms*/ { -1, 0, 0, 0, 0, 0 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_Count, -1, 0},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_Count, -1, 0},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    
    };

static FragShaderDesc ShaderDesc_FS_FTexTGVertexMul = {
    /* Type */          FragShaderDesc::FS_FTexTGVertexMul,
    /* Flags */         0,
    /* pSource */       pSource_FTexTGVertexMul,
    /* Uniforms */      { 
                          /* cxadd*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      { -1, 0, 0, 0, 0, 0 },
                          /* mvp*/        { -1, 0, 0, 0, 0, 0 },
                          /* offset*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor2*/    { -1, 0, 0, 0, 0, 0 },
                          /* srctex*/     { -1, 0, 0, 0, 0, 0 },
                          /* srctexscale*/{ -1, 0, 0, 0, 0, 0 },
                          /* tex*/        {0, 1032, 1, 1, 1, 0 },
                          /* texgen*/     { -1, 0, 0, 0, 0, 0 },
                          /* texscale*/   { -1, 0, 0, 0, 0, 0 },
                          /* vfmuniforms*/{ -1, 0, 0, 0, 0, 0 },
                          /* vfuniforms*/ { -1, 0, 0, 0, 0, 0 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_Count, -1, 0},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_Count, -1, 0},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    
    };

static FragShaderDesc ShaderDesc_FS_FTexTGVertexEAlpha = {
    /* Type */          FragShaderDesc::FS_FTexTGVertexEAlpha,
    /* Flags */         0,
    /* pSource */       pSource_FTexTGVertexEAlpha,
    /* Uniforms */      { 
                          /* cxadd*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      { -1, 0, 0, 0, 0, 0 },
                          /* mvp*/        { -1, 0, 0, 0, 0, 0 },
                          /* offset*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor2*/    { -1, 0, 0, 0, 0, 0 },
                          /* srctex*/     { -1, 0, 0, 0, 0, 0 },
                          /* srctexscale*/{ -1, 0, 0, 0, 0, 0 },
                          /* tex*/        {0, 1032, 1, 1, 1, 0 },
                          /* texgen*/     { -1, 0, 0, 0, 0, 0 },
                          /* texscale*/   { -1, 0, 0, 0, 0, 0 },
                          /* vfmuniforms*/{ -1, 0, 0, 0, 0, 0 },
                          /* vfuniforms*/ { -1, 0, 0, 0, 0, 0 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_Count, -1, 0},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_Count, -1, 0},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    
    };

static FragShaderDesc ShaderDesc_FS_FTexTGVertexEAlphaMul = {
    /* Type */          FragShaderDesc::FS_FTexTGVertexEAlphaMul,
    /* Flags */         0,
    /* pSource */       pSource_FTexTGVertexEAlphaMul,
    /* Uniforms */      { 
                          /* cxadd*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      { -1, 0, 0, 0, 0, 0 },
                          /* mvp*/        { -1, 0, 0, 0, 0, 0 },
                          /* offset*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor2*/    { -1, 0, 0, 0, 0, 0 },
                          /* srctex*/     { -1, 0, 0, 0, 0, 0 },
                          /* srctexscale*/{ -1, 0, 0, 0, 0, 0 },
                          /* tex*/        {0, 1032, 1, 1, 1, 0 },
                          /* texgen*/     { -1, 0, 0, 0, 0, 0 },
                          /* texscale*/   { -1, 0, 0, 0, 0, 0 },
                          /* vfmuniforms*/{ -1, 0, 0, 0, 0, 0 },
                          /* vfuniforms*/ { -1, 0, 0, 0, 0, 0 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_Count, -1, 0},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_Count, -1, 0},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    
    };

static FragShaderDesc ShaderDesc_FS_FTexTGVertexCxform = {
    /* Type */          FragShaderDesc::FS_FTexTGVertexCxform,
    /* Flags */         0,
    /* pSource */       pSource_FTexTGVertexCxform,
    /* Uniforms */      { 
                          /* cxadd*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      { -1, 0, 0, 0, 0, 0 },
                          /* mvp*/        { -1, 0, 0, 0, 0, 0 },
                          /* offset*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor2*/    { -1, 0, 0, 0, 0, 0 },
                          /* srctex*/     { -1, 0, 0, 0, 0, 0 },
                          /* srctexscale*/{ -1, 0, 0, 0, 0, 0 },
                          /* tex*/        {0, 1032, 1, 1, 1, 0 },
                          /* texgen*/     { -1, 0, 0, 0, 0, 0 },
                          /* texscale*/   { -1, 0, 0, 0, 0, 0 },
                          /* vfmuniforms*/{ -1, 0, 0, 0, 0, 0 },
                          /* vfuniforms*/ { -1, 0, 0, 0, 0, 0 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_Count, -1, 0},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_Count, -1, 0},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    
    };

static FragShaderDesc ShaderDesc_FS_FTexTGVertexCxformMul = {
    /* Type */          FragShaderDesc::FS_FTexTGVertexCxformMul,
    /* Flags */         0,
    /* pSource */       pSource_FTexTGVertexCxformMul,
    /* Uniforms */      { 
                          /* cxadd*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      { -1, 0, 0, 0, 0, 0 },
                          /* mvp*/        { -1, 0, 0, 0, 0, 0 },
                          /* offset*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor2*/    { -1, 0, 0, 0, 0, 0 },
                          /* srctex*/     { -1, 0, 0, 0, 0, 0 },
                          /* srctexscale*/{ -1, 0, 0, 0, 0, 0 },
                          /* tex*/        {0, 1032, 1, 1, 1, 0 },
                          /* texgen*/     { -1, 0, 0, 0, 0, 0 },
                          /* texscale*/   { -1, 0, 0, 0, 0, 0 },
                          /* vfmuniforms*/{ -1, 0, 0, 0, 0, 0 },
                          /* vfuniforms*/ { -1, 0, 0, 0, 0, 0 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_Count, -1, 0},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_Count, -1, 0},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    
    };

static FragShaderDesc ShaderDesc_FS_FTexTGVertexCxformEAlpha = {
    /* Type */          FragShaderDesc::FS_FTexTGVertexCxformEAlpha,
    /* Flags */         0,
    /* pSource */       pSource_FTexTGVertexCxformEAlpha,
    /* Uniforms */      { 
                          /* cxadd*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      { -1, 0, 0, 0, 0, 0 },
                          /* mvp*/        { -1, 0, 0, 0, 0, 0 },
                          /* offset*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor2*/    { -1, 0, 0, 0, 0, 0 },
                          /* srctex*/     { -1, 0, 0, 0, 0, 0 },
                          /* srctexscale*/{ -1, 0, 0, 0, 0, 0 },
                          /* tex*/        {0, 1032, 1, 1, 1, 0 },
                          /* texgen*/     { -1, 0, 0, 0, 0, 0 },
                          /* texscale*/   { -1, 0, 0, 0, 0, 0 },
                          /* vfmuniforms*/{ -1, 0, 0, 0, 0, 0 },
                          /* vfuniforms*/ { -1, 0, 0, 0, 0, 0 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_Count, -1, 0},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_Count, -1, 0},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    
    };

static FragShaderDesc ShaderDesc_FS_FTexTGVertexCxformEAlphaMul = {
    /* Type */          FragShaderDesc::FS_FTexTGVertexCxformEAlphaMul,
    /* Flags */         0,
    /* pSource */       pSource_FTexTGVertexCxformEAlphaMul,
    /* Uniforms */      { 
                          /* cxadd*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      { -1, 0, 0, 0, 0, 0 },
                          /* mvp*/        { -1, 0, 0, 0, 0, 0 },
                          /* offset*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor2*/    { -1, 0, 0, 0, 0, 0 },
                          /* srctex*/     { -1, 0, 0, 0, 0, 0 },
                          /* srctexscale*/{ -1, 0, 0, 0, 0, 0 },
                          /* tex*/        {0, 1032, 1, 1, 1, 0 },
                          /* texgen*/     { -1, 0, 0, 0, 0, 0 },
                          /* texscale*/   { -1, 0, 0, 0, 0, 0 },
                          /* vfmuniforms*/{ -1, 0, 0, 0, 0, 0 },
                          /* vfuniforms*/ { -1, 0, 0, 0, 0, 0 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_Count, -1, 0},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_Count, -1, 0},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    
    };

static FragShaderDesc ShaderDesc_FS_FBatchTexTG = {
    /* Type */          FragShaderDesc::FS_FBatchTexTG,
    /* Flags */         Shader_Batch,
    /* pSource */       pSource_FTexTG,
    /* Uniforms */      { 
                          /* cxadd*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      { -1, 0, 0, 0, 0, 0 },
                          /* mvp*/        { -1, 0, 0, 0, 0, 0 },
                          /* offset*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor2*/    { -1, 0, 0, 0, 0, 0 },
                          /* srctex*/     { -1, 0, 0, 0, 0, 0 },
                          /* srctexscale*/{ -1, 0, 0, 0, 0, 0 },
                          /* tex*/        {0, 1032, 1, 1, 1, 0 },
                          /* texgen*/     { -1, 0, 0, 0, 0, 0 },
                          /* texscale*/   { -1, 0, 0, 0, 0, 0 },
                          /* vfmuniforms*/{ -1, 0, 0, 0, 0, 0 },
                          /* vfuniforms*/ { -1, 0, 0, 0, 0, 0 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_Count, -1, 0},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_Count, -1, 0},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    
    };

static FragShaderDesc ShaderDesc_FS_FBatchTexTGMul = {
    /* Type */          FragShaderDesc::FS_FBatchTexTGMul,
    /* Flags */         Shader_Batch,
    /* pSource */       pSource_FTexTGMul,
    /* Uniforms */      { 
                          /* cxadd*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      { -1, 0, 0, 0, 0, 0 },
                          /* mvp*/        { -1, 0, 0, 0, 0, 0 },
                          /* offset*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor2*/    { -1, 0, 0, 0, 0, 0 },
                          /* srctex*/     { -1, 0, 0, 0, 0, 0 },
                          /* srctexscale*/{ -1, 0, 0, 0, 0, 0 },
                          /* tex*/        {0, 1032, 1, 1, 1, 0 },
                          /* texgen*/     { -1, 0, 0, 0, 0, 0 },
                          /* texscale*/   { -1, 0, 0, 0, 0, 0 },
                          /* vfmuniforms*/{ -1, 0, 0, 0, 0, 0 },
                          /* vfuniforms*/ { -1, 0, 0, 0, 0, 0 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_Count, -1, 0},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_Count, -1, 0},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    
    };

static FragShaderDesc ShaderDesc_FS_FBatchTexTGEAlpha = {
    /* Type */          FragShaderDesc::FS_FBatchTexTGEAlpha,
    /* Flags */         Shader_Batch,
    /* pSource */       pSource_FTexTGEAlpha,
    /* Uniforms */      { 
                          /* cxadd*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      { -1, 0, 0, 0, 0, 0 },
                          /* mvp*/        { -1, 0, 0, 0, 0, 0 },
                          /* offset*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor2*/    { -1, 0, 0, 0, 0, 0 },
                          /* srctex*/     { -1, 0, 0, 0, 0, 0 },
                          /* srctexscale*/{ -1, 0, 0, 0, 0, 0 },
                          /* tex*/        {0, 1032, 1, 1, 1, 0 },
                          /* texgen*/     { -1, 0, 0, 0, 0, 0 },
                          /* texscale*/   { -1, 0, 0, 0, 0, 0 },
                          /* vfmuniforms*/{ -1, 0, 0, 0, 0, 0 },
                          /* vfuniforms*/ { -1, 0, 0, 0, 0, 0 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_Count, -1, 0},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_Count, -1, 0},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    
    };

static FragShaderDesc ShaderDesc_FS_FBatchTexTGEAlphaMul = {
    /* Type */          FragShaderDesc::FS_FBatchTexTGEAlphaMul,
    /* Flags */         Shader_Batch,
    /* pSource */       pSource_FTexTGEAlphaMul,
    /* Uniforms */      { 
                          /* cxadd*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      { -1, 0, 0, 0, 0, 0 },
                          /* mvp*/        { -1, 0, 0, 0, 0, 0 },
                          /* offset*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor2*/    { -1, 0, 0, 0, 0, 0 },
                          /* srctex*/     { -1, 0, 0, 0, 0, 0 },
                          /* srctexscale*/{ -1, 0, 0, 0, 0, 0 },
                          /* tex*/        {0, 1032, 1, 1, 1, 0 },
                          /* texgen*/     { -1, 0, 0, 0, 0, 0 },
                          /* texscale*/   { -1, 0, 0, 0, 0, 0 },
                          /* vfmuniforms*/{ -1, 0, 0, 0, 0, 0 },
                          /* vfuniforms*/ { -1, 0, 0, 0, 0, 0 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_Count, -1, 0},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_Count, -1, 0},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    
    };

static FragShaderDesc ShaderDesc_FS_FBatchTexTGCxform = {
    /* Type */          FragShaderDesc::FS_FBatchTexTGCxform,
    /* Flags */         Shader_Batch,
    /* pSource */       pSource_FTexTGCxform,
    /* Uniforms */      { 
                          /* cxadd*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      { -1, 0, 0, 0, 0, 0 },
                          /* mvp*/        { -1, 0, 0, 0, 0, 0 },
                          /* offset*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor2*/    { -1, 0, 0, 0, 0, 0 },
                          /* srctex*/     { -1, 0, 0, 0, 0, 0 },
                          /* srctexscale*/{ -1, 0, 0, 0, 0, 0 },
                          /* tex*/        {0, 1032, 1, 1, 1, 0 },
                          /* texgen*/     { -1, 0, 0, 0, 0, 0 },
                          /* texscale*/   { -1, 0, 0, 0, 0, 0 },
                          /* vfmuniforms*/{ -1, 0, 0, 0, 0, 0 },
                          /* vfuniforms*/ { -1, 0, 0, 0, 0, 0 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_Count, -1, 0},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_Count, -1, 0},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    
    };

static FragShaderDesc ShaderDesc_FS_FBatchTexTGCxformMul = {
    /* Type */          FragShaderDesc::FS_FBatchTexTGCxformMul,
    /* Flags */         Shader_Batch,
    /* pSource */       pSource_FTexTGCxformMul,
    /* Uniforms */      { 
                          /* cxadd*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      { -1, 0, 0, 0, 0, 0 },
                          /* mvp*/        { -1, 0, 0, 0, 0, 0 },
                          /* offset*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor2*/    { -1, 0, 0, 0, 0, 0 },
                          /* srctex*/     { -1, 0, 0, 0, 0, 0 },
                          /* srctexscale*/{ -1, 0, 0, 0, 0, 0 },
                          /* tex*/        {0, 1032, 1, 1, 1, 0 },
                          /* texgen*/     { -1, 0, 0, 0, 0, 0 },
                          /* texscale*/   { -1, 0, 0, 0, 0, 0 },
                          /* vfmuniforms*/{ -1, 0, 0, 0, 0, 0 },
                          /* vfuniforms*/ { -1, 0, 0, 0, 0, 0 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_Count, -1, 0},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_Count, -1, 0},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    
    };

static FragShaderDesc ShaderDesc_FS_FBatchTexTGCxformEAlpha = {
    /* Type */          FragShaderDesc::FS_FBatchTexTGCxformEAlpha,
    /* Flags */         Shader_Batch,
    /* pSource */       pSource_FTexTGCxformEAlpha,
    /* Uniforms */      { 
                          /* cxadd*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      { -1, 0, 0, 0, 0, 0 },
                          /* mvp*/        { -1, 0, 0, 0, 0, 0 },
                          /* offset*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor2*/    { -1, 0, 0, 0, 0, 0 },
                          /* srctex*/     { -1, 0, 0, 0, 0, 0 },
                          /* srctexscale*/{ -1, 0, 0, 0, 0, 0 },
                          /* tex*/        {0, 1032, 1, 1, 1, 0 },
                          /* texgen*/     { -1, 0, 0, 0, 0, 0 },
                          /* texscale*/   { -1, 0, 0, 0, 0, 0 },
                          /* vfmuniforms*/{ -1, 0, 0, 0, 0, 0 },
                          /* vfuniforms*/ { -1, 0, 0, 0, 0, 0 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_Count, -1, 0},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_Count, -1, 0},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    
    };

static FragShaderDesc ShaderDesc_FS_FBatchTexTGCxformEAlphaMul = {
    /* Type */          FragShaderDesc::FS_FBatchTexTGCxformEAlphaMul,
    /* Flags */         Shader_Batch,
    /* pSource */       pSource_FTexTGCxformEAlphaMul,
    /* Uniforms */      { 
                          /* cxadd*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      { -1, 0, 0, 0, 0, 0 },
                          /* mvp*/        { -1, 0, 0, 0, 0, 0 },
                          /* offset*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor2*/    { -1, 0, 0, 0, 0, 0 },
                          /* srctex*/     { -1, 0, 0, 0, 0, 0 },
                          /* srctexscale*/{ -1, 0, 0, 0, 0, 0 },
                          /* tex*/        {0, 1032, 1, 1, 1, 0 },
                          /* texgen*/     { -1, 0, 0, 0, 0, 0 },
                          /* texscale*/   { -1, 0, 0, 0, 0, 0 },
                          /* vfmuniforms*/{ -1, 0, 0, 0, 0, 0 },
                          /* vfuniforms*/ { -1, 0, 0, 0, 0, 0 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_Count, -1, 0},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_Count, -1, 0},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    
    };

static FragShaderDesc ShaderDesc_FS_FBatchVertex = {
    /* Type */          FragShaderDesc::FS_FBatchVertex,
    /* Flags */         Shader_Batch,
    /* pSource */       pSource_FVertex,
    /* Uniforms */      { 
                          /* cxadd*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      { -1, 0, 0, 0, 0, 0 },
                          /* mvp*/        { -1, 0, 0, 0, 0, 0 },
                          /* offset*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor2*/    { -1, 0, 0, 0, 0, 0 },
                          /* srctex*/     { -1, 0, 0, 0, 0, 0 },
                          /* srctexscale*/{ -1, 0, 0, 0, 0, 0 },
                          /* tex*/        { -1, 0, 0, 0, 0, 0 },
                          /* texgen*/     { -1, 0, 0, 0, 0, 0 },
                          /* texscale*/   { -1, 0, 0, 0, 0, 0 },
                          /* vfmuniforms*/{ -1, 0, 0, 0, 0, 0 },
                          /* vfuniforms*/ { -1, 0, 0, 0, 0, 0 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_Count, -1, 0},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_Count, -1, 0},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    
    };

static FragShaderDesc ShaderDesc_FS_FBatchVertexMul = {
    /* Type */          FragShaderDesc::FS_FBatchVertexMul,
    /* Flags */         Shader_Batch,
    /* pSource */       pSource_FVertexMul,
    /* Uniforms */      { 
                          /* cxadd*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      { -1, 0, 0, 0, 0, 0 },
                          /* mvp*/        { -1, 0, 0, 0, 0, 0 },
                          /* offset*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor2*/    { -1, 0, 0, 0, 0, 0 },
                          /* srctex*/     { -1, 0, 0, 0, 0, 0 },
                          /* srctexscale*/{ -1, 0, 0, 0, 0, 0 },
                          /* tex*/        { -1, 0, 0, 0, 0, 0 },
                          /* texgen*/     { -1, 0, 0, 0, 0, 0 },
                          /* texscale*/   { -1, 0, 0, 0, 0, 0 },
                          /* vfmuniforms*/{ -1, 0, 0, 0, 0, 0 },
                          /* vfuniforms*/ { -1, 0, 0, 0, 0, 0 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_Count, -1, 0},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_Count, -1, 0},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    
    };

static FragShaderDesc ShaderDesc_FS_FBatchVertexEAlpha = {
    /* Type */          FragShaderDesc::FS_FBatchVertexEAlpha,
    /* Flags */         Shader_Batch,
    /* pSource */       pSource_FVertexEAlpha,
    /* Uniforms */      { 
                          /* cxadd*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      { -1, 0, 0, 0, 0, 0 },
                          /* mvp*/        { -1, 0, 0, 0, 0, 0 },
                          /* offset*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor2*/    { -1, 0, 0, 0, 0, 0 },
                          /* srctex*/     { -1, 0, 0, 0, 0, 0 },
                          /* srctexscale*/{ -1, 0, 0, 0, 0, 0 },
                          /* tex*/        { -1, 0, 0, 0, 0, 0 },
                          /* texgen*/     { -1, 0, 0, 0, 0, 0 },
                          /* texscale*/   { -1, 0, 0, 0, 0, 0 },
                          /* vfmuniforms*/{ -1, 0, 0, 0, 0, 0 },
                          /* vfuniforms*/ { -1, 0, 0, 0, 0, 0 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_Count, -1, 0},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_Count, -1, 0},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    
    };

static FragShaderDesc ShaderDesc_FS_FBatchVertexEAlphaMul = {
    /* Type */          FragShaderDesc::FS_FBatchVertexEAlphaMul,
    /* Flags */         Shader_Batch,
    /* pSource */       pSource_FVertexEAlphaMul,
    /* Uniforms */      { 
                          /* cxadd*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      { -1, 0, 0, 0, 0, 0 },
                          /* mvp*/        { -1, 0, 0, 0, 0, 0 },
                          /* offset*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor2*/    { -1, 0, 0, 0, 0, 0 },
                          /* srctex*/     { -1, 0, 0, 0, 0, 0 },
                          /* srctexscale*/{ -1, 0, 0, 0, 0, 0 },
                          /* tex*/        { -1, 0, 0, 0, 0, 0 },
                          /* texgen*/     { -1, 0, 0, 0, 0, 0 },
                          /* texscale*/   { -1, 0, 0, 0, 0, 0 },
                          /* vfmuniforms*/{ -1, 0, 0, 0, 0, 0 },
                          /* vfuniforms*/ { -1, 0, 0, 0, 0, 0 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_Count, -1, 0},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_Count, -1, 0},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    
    };

static FragShaderDesc ShaderDesc_FS_FBatchVertexCxform = {
    /* Type */          FragShaderDesc::FS_FBatchVertexCxform,
    /* Flags */         Shader_Batch,
    /* pSource */       pSource_FVertexCxform,
    /* Uniforms */      { 
                          /* cxadd*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      { -1, 0, 0, 0, 0, 0 },
                          /* mvp*/        { -1, 0, 0, 0, 0, 0 },
                          /* offset*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor2*/    { -1, 0, 0, 0, 0, 0 },
                          /* srctex*/     { -1, 0, 0, 0, 0, 0 },
                          /* srctexscale*/{ -1, 0, 0, 0, 0, 0 },
                          /* tex*/        { -1, 0, 0, 0, 0, 0 },
                          /* texgen*/     { -1, 0, 0, 0, 0, 0 },
                          /* texscale*/   { -1, 0, 0, 0, 0, 0 },
                          /* vfmuniforms*/{ -1, 0, 0, 0, 0, 0 },
                          /* vfuniforms*/ { -1, 0, 0, 0, 0, 0 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_Count, -1, 0},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_Count, -1, 0},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    
    };

static FragShaderDesc ShaderDesc_FS_FBatchVertexCxformMul = {
    /* Type */          FragShaderDesc::FS_FBatchVertexCxformMul,
    /* Flags */         Shader_Batch,
    /* pSource */       pSource_FVertexCxformMul,
    /* Uniforms */      { 
                          /* cxadd*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      { -1, 0, 0, 0, 0, 0 },
                          /* mvp*/        { -1, 0, 0, 0, 0, 0 },
                          /* offset*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor2*/    { -1, 0, 0, 0, 0, 0 },
                          /* srctex*/     { -1, 0, 0, 0, 0, 0 },
                          /* srctexscale*/{ -1, 0, 0, 0, 0, 0 },
                          /* tex*/        { -1, 0, 0, 0, 0, 0 },
                          /* texgen*/     { -1, 0, 0, 0, 0, 0 },
                          /* texscale*/   { -1, 0, 0, 0, 0, 0 },
                          /* vfmuniforms*/{ -1, 0, 0, 0, 0, 0 },
                          /* vfuniforms*/ { -1, 0, 0, 0, 0, 0 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_Count, -1, 0},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_Count, -1, 0},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    
    };

static FragShaderDesc ShaderDesc_FS_FBatchVertexCxformEAlpha = {
    /* Type */          FragShaderDesc::FS_FBatchVertexCxformEAlpha,
    /* Flags */         Shader_Batch,
    /* pSource */       pSource_FVertexCxformEAlpha,
    /* Uniforms */      { 
                          /* cxadd*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      { -1, 0, 0, 0, 0, 0 },
                          /* mvp*/        { -1, 0, 0, 0, 0, 0 },
                          /* offset*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor2*/    { -1, 0, 0, 0, 0, 0 },
                          /* srctex*/     { -1, 0, 0, 0, 0, 0 },
                          /* srctexscale*/{ -1, 0, 0, 0, 0, 0 },
                          /* tex*/        { -1, 0, 0, 0, 0, 0 },
                          /* texgen*/     { -1, 0, 0, 0, 0, 0 },
                          /* texscale*/   { -1, 0, 0, 0, 0, 0 },
                          /* vfmuniforms*/{ -1, 0, 0, 0, 0, 0 },
                          /* vfuniforms*/ { -1, 0, 0, 0, 0, 0 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_Count, -1, 0},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_Count, -1, 0},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    
    };

static FragShaderDesc ShaderDesc_FS_FBatchVertexCxformEAlphaMul = {
    /* Type */          FragShaderDesc::FS_FBatchVertexCxformEAlphaMul,
    /* Flags */         Shader_Batch,
    /* pSource */       pSource_FVertexCxformEAlphaMul,
    /* Uniforms */      { 
                          /* cxadd*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      { -1, 0, 0, 0, 0, 0 },
                          /* mvp*/        { -1, 0, 0, 0, 0, 0 },
                          /* offset*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor2*/    { -1, 0, 0, 0, 0, 0 },
                          /* srctex*/     { -1, 0, 0, 0, 0, 0 },
                          /* srctexscale*/{ -1, 0, 0, 0, 0, 0 },
                          /* tex*/        { -1, 0, 0, 0, 0, 0 },
                          /* texgen*/     { -1, 0, 0, 0, 0, 0 },
                          /* texscale*/   { -1, 0, 0, 0, 0, 0 },
                          /* vfmuniforms*/{ -1, 0, 0, 0, 0, 0 },
                          /* vfuniforms*/ { -1, 0, 0, 0, 0, 0 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_Count, -1, 0},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_Count, -1, 0},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    
    };

static FragShaderDesc ShaderDesc_FS_FBatchTexTGTexTG = {
    /* Type */          FragShaderDesc::FS_FBatchTexTGTexTG,
    /* Flags */         Shader_Batch,
    /* pSource */       pSource_FTexTGTexTG,
    /* Uniforms */      { 
                          /* cxadd*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      { -1, 0, 0, 0, 0, 0 },
                          /* mvp*/        { -1, 0, 0, 0, 0, 0 },
                          /* offset*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor2*/    { -1, 0, 0, 0, 0, 0 },
                          /* srctex*/     { -1, 0, 0, 0, 0, 0 },
                          /* srctexscale*/{ -1, 0, 0, 0, 0, 0 },
                          /* tex*/        {0, 1032, 1, 2, 1, 0 },
                          /* texgen*/     { -1, 0, 0, 0, 0, 0 },
                          /* texscale*/   { -1, 0, 0, 0, 0, 0 },
                          /* vfmuniforms*/{ -1, 0, 0, 0, 0, 0 },
                          /* vfuniforms*/ { -1, 0, 0, 0, 0, 0 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_Count, -1, 0},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_Count, -1, 0},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    
    };

static FragShaderDesc ShaderDesc_FS_FBatchTexTGTexTGMul = {
    /* Type */          FragShaderDesc::FS_FBatchTexTGTexTGMul,
    /* Flags */         Shader_Batch,
    /* pSource */       pSource_FTexTGTexTGMul,
    /* Uniforms */      { 
                          /* cxadd*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      { -1, 0, 0, 0, 0, 0 },
                          /* mvp*/        { -1, 0, 0, 0, 0, 0 },
                          /* offset*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor2*/    { -1, 0, 0, 0, 0, 0 },
                          /* srctex*/     { -1, 0, 0, 0, 0, 0 },
                          /* srctexscale*/{ -1, 0, 0, 0, 0, 0 },
                          /* tex*/        {0, 1032, 1, 2, 1, 0 },
                          /* texgen*/     { -1, 0, 0, 0, 0, 0 },
                          /* texscale*/   { -1, 0, 0, 0, 0, 0 },
                          /* vfmuniforms*/{ -1, 0, 0, 0, 0, 0 },
                          /* vfuniforms*/ { -1, 0, 0, 0, 0, 0 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_Count, -1, 0},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_Count, -1, 0},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    
    };

static FragShaderDesc ShaderDesc_FS_FBatchTexTGTexTGEAlpha = {
    /* Type */          FragShaderDesc::FS_FBatchTexTGTexTGEAlpha,
    /* Flags */         Shader_Batch,
    /* pSource */       pSource_FTexTGTexTGEAlpha,
    /* Uniforms */      { 
                          /* cxadd*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      { -1, 0, 0, 0, 0, 0 },
                          /* mvp*/        { -1, 0, 0, 0, 0, 0 },
                          /* offset*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor2*/    { -1, 0, 0, 0, 0, 0 },
                          /* srctex*/     { -1, 0, 0, 0, 0, 0 },
                          /* srctexscale*/{ -1, 0, 0, 0, 0, 0 },
                          /* tex*/        {0, 1032, 1, 2, 1, 0 },
                          /* texgen*/     { -1, 0, 0, 0, 0, 0 },
                          /* texscale*/   { -1, 0, 0, 0, 0, 0 },
                          /* vfmuniforms*/{ -1, 0, 0, 0, 0, 0 },
                          /* vfuniforms*/ { -1, 0, 0, 0, 0, 0 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_Count, -1, 0},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_Count, -1, 0},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    
    };

static FragShaderDesc ShaderDesc_FS_FBatchTexTGTexTGEAlphaMul = {
    /* Type */          FragShaderDesc::FS_FBatchTexTGTexTGEAlphaMul,
    /* Flags */         Shader_Batch,
    /* pSource */       pSource_FTexTGTexTGEAlphaMul,
    /* Uniforms */      { 
                          /* cxadd*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      { -1, 0, 0, 0, 0, 0 },
                          /* mvp*/        { -1, 0, 0, 0, 0, 0 },
                          /* offset*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor2*/    { -1, 0, 0, 0, 0, 0 },
                          /* srctex*/     { -1, 0, 0, 0, 0, 0 },
                          /* srctexscale*/{ -1, 0, 0, 0, 0, 0 },
                          /* tex*/        {0, 1032, 1, 2, 1, 0 },
                          /* texgen*/     { -1, 0, 0, 0, 0, 0 },
                          /* texscale*/   { -1, 0, 0, 0, 0, 0 },
                          /* vfmuniforms*/{ -1, 0, 0, 0, 0, 0 },
                          /* vfuniforms*/ { -1, 0, 0, 0, 0, 0 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_Count, -1, 0},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_Count, -1, 0},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    
    };

static FragShaderDesc ShaderDesc_FS_FBatchTexTGTexTGCxform = {
    /* Type */          FragShaderDesc::FS_FBatchTexTGTexTGCxform,
    /* Flags */         Shader_Batch,
    /* pSource */       pSource_FTexTGTexTGCxform,
    /* Uniforms */      { 
                          /* cxadd*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      { -1, 0, 0, 0, 0, 0 },
                          /* mvp*/        { -1, 0, 0, 0, 0, 0 },
                          /* offset*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor2*/    { -1, 0, 0, 0, 0, 0 },
                          /* srctex*/     { -1, 0, 0, 0, 0, 0 },
                          /* srctexscale*/{ -1, 0, 0, 0, 0, 0 },
                          /* tex*/        {0, 1032, 1, 2, 1, 0 },
                          /* texgen*/     { -1, 0, 0, 0, 0, 0 },
                          /* texscale*/   { -1, 0, 0, 0, 0, 0 },
                          /* vfmuniforms*/{ -1, 0, 0, 0, 0, 0 },
                          /* vfuniforms*/ { -1, 0, 0, 0, 0, 0 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_Count, -1, 0},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_Count, -1, 0},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    
    };

static FragShaderDesc ShaderDesc_FS_FBatchTexTGTexTGCxformMul = {
    /* Type */          FragShaderDesc::FS_FBatchTexTGTexTGCxformMul,
    /* Flags */         Shader_Batch,
    /* pSource */       pSource_FTexTGTexTGCxformMul,
    /* Uniforms */      { 
                          /* cxadd*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      { -1, 0, 0, 0, 0, 0 },
                          /* mvp*/        { -1, 0, 0, 0, 0, 0 },
                          /* offset*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor2*/    { -1, 0, 0, 0, 0, 0 },
                          /* srctex*/     { -1, 0, 0, 0, 0, 0 },
                          /* srctexscale*/{ -1, 0, 0, 0, 0, 0 },
                          /* tex*/        {0, 1032, 1, 2, 1, 0 },
                          /* texgen*/     { -1, 0, 0, 0, 0, 0 },
                          /* texscale*/   { -1, 0, 0, 0, 0, 0 },
                          /* vfmuniforms*/{ -1, 0, 0, 0, 0, 0 },
                          /* vfuniforms*/ { -1, 0, 0, 0, 0, 0 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_Count, -1, 0},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_Count, -1, 0},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    
    };

static FragShaderDesc ShaderDesc_FS_FBatchTexTGTexTGCxformEAlpha = {
    /* Type */          FragShaderDesc::FS_FBatchTexTGTexTGCxformEAlpha,
    /* Flags */         Shader_Batch,
    /* pSource */       pSource_FTexTGTexTGCxformEAlpha,
    /* Uniforms */      { 
                          /* cxadd*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      { -1, 0, 0, 0, 0, 0 },
                          /* mvp*/        { -1, 0, 0, 0, 0, 0 },
                          /* offset*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor2*/    { -1, 0, 0, 0, 0, 0 },
                          /* srctex*/     { -1, 0, 0, 0, 0, 0 },
                          /* srctexscale*/{ -1, 0, 0, 0, 0, 0 },
                          /* tex*/        {0, 1032, 1, 2, 1, 0 },
                          /* texgen*/     { -1, 0, 0, 0, 0, 0 },
                          /* texscale*/   { -1, 0, 0, 0, 0, 0 },
                          /* vfmuniforms*/{ -1, 0, 0, 0, 0, 0 },
                          /* vfuniforms*/ { -1, 0, 0, 0, 0, 0 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_Count, -1, 0},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_Count, -1, 0},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    
    };

static FragShaderDesc ShaderDesc_FS_FBatchTexTGTexTGCxformEAlphaMul = {
    /* Type */          FragShaderDesc::FS_FBatchTexTGTexTGCxformEAlphaMul,
    /* Flags */         Shader_Batch,
    /* pSource */       pSource_FTexTGTexTGCxformEAlphaMul,
    /* Uniforms */      { 
                          /* cxadd*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      { -1, 0, 0, 0, 0, 0 },
                          /* mvp*/        { -1, 0, 0, 0, 0, 0 },
                          /* offset*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor2*/    { -1, 0, 0, 0, 0, 0 },
                          /* srctex*/     { -1, 0, 0, 0, 0, 0 },
                          /* srctexscale*/{ -1, 0, 0, 0, 0, 0 },
                          /* tex*/        {0, 1032, 1, 2, 1, 0 },
                          /* texgen*/     { -1, 0, 0, 0, 0, 0 },
                          /* texscale*/   { -1, 0, 0, 0, 0, 0 },
                          /* vfmuniforms*/{ -1, 0, 0, 0, 0, 0 },
                          /* vfuniforms*/ { -1, 0, 0, 0, 0, 0 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_Count, -1, 0},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_Count, -1, 0},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    
    };

static FragShaderDesc ShaderDesc_FS_FBatchTexTGVertex = {
    /* Type */          FragShaderDesc::FS_FBatchTexTGVertex,
    /* Flags */         Shader_Batch,
    /* pSource */       pSource_FTexTGVertex,
    /* Uniforms */      { 
                          /* cxadd*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      { -1, 0, 0, 0, 0, 0 },
                          /* mvp*/        { -1, 0, 0, 0, 0, 0 },
                          /* offset*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor2*/    { -1, 0, 0, 0, 0, 0 },
                          /* srctex*/     { -1, 0, 0, 0, 0, 0 },
                          /* srctexscale*/{ -1, 0, 0, 0, 0, 0 },
                          /* tex*/        {0, 1032, 1, 1, 1, 0 },
                          /* texgen*/     { -1, 0, 0, 0, 0, 0 },
                          /* texscale*/   { -1, 0, 0, 0, 0, 0 },
                          /* vfmuniforms*/{ -1, 0, 0, 0, 0, 0 },
                          /* vfuniforms*/ { -1, 0, 0, 0, 0, 0 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_Count, -1, 0},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_Count, -1, 0},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    
    };

static FragShaderDesc ShaderDesc_FS_FBatchTexTGVertexMul = {
    /* Type */          FragShaderDesc::FS_FBatchTexTGVertexMul,
    /* Flags */         Shader_Batch,
    /* pSource */       pSource_FTexTGVertexMul,
    /* Uniforms */      { 
                          /* cxadd*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      { -1, 0, 0, 0, 0, 0 },
                          /* mvp*/        { -1, 0, 0, 0, 0, 0 },
                          /* offset*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor2*/    { -1, 0, 0, 0, 0, 0 },
                          /* srctex*/     { -1, 0, 0, 0, 0, 0 },
                          /* srctexscale*/{ -1, 0, 0, 0, 0, 0 },
                          /* tex*/        {0, 1032, 1, 1, 1, 0 },
                          /* texgen*/     { -1, 0, 0, 0, 0, 0 },
                          /* texscale*/   { -1, 0, 0, 0, 0, 0 },
                          /* vfmuniforms*/{ -1, 0, 0, 0, 0, 0 },
                          /* vfuniforms*/ { -1, 0, 0, 0, 0, 0 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_Count, -1, 0},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_Count, -1, 0},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    
    };

static FragShaderDesc ShaderDesc_FS_FBatchTexTGVertexEAlpha = {
    /* Type */          FragShaderDesc::FS_FBatchTexTGVertexEAlpha,
    /* Flags */         Shader_Batch,
    /* pSource */       pSource_FTexTGVertexEAlpha,
    /* Uniforms */      { 
                          /* cxadd*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      { -1, 0, 0, 0, 0, 0 },
                          /* mvp*/        { -1, 0, 0, 0, 0, 0 },
                          /* offset*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor2*/    { -1, 0, 0, 0, 0, 0 },
                          /* srctex*/     { -1, 0, 0, 0, 0, 0 },
                          /* srctexscale*/{ -1, 0, 0, 0, 0, 0 },
                          /* tex*/        {0, 1032, 1, 1, 1, 0 },
                          /* texgen*/     { -1, 0, 0, 0, 0, 0 },
                          /* texscale*/   { -1, 0, 0, 0, 0, 0 },
                          /* vfmuniforms*/{ -1, 0, 0, 0, 0, 0 },
                          /* vfuniforms*/ { -1, 0, 0, 0, 0, 0 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_Count, -1, 0},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_Count, -1, 0},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    
    };

static FragShaderDesc ShaderDesc_FS_FBatchTexTGVertexEAlphaMul = {
    /* Type */          FragShaderDesc::FS_FBatchTexTGVertexEAlphaMul,
    /* Flags */         Shader_Batch,
    /* pSource */       pSource_FTexTGVertexEAlphaMul,
    /* Uniforms */      { 
                          /* cxadd*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      { -1, 0, 0, 0, 0, 0 },
                          /* mvp*/        { -1, 0, 0, 0, 0, 0 },
                          /* offset*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor2*/    { -1, 0, 0, 0, 0, 0 },
                          /* srctex*/     { -1, 0, 0, 0, 0, 0 },
                          /* srctexscale*/{ -1, 0, 0, 0, 0, 0 },
                          /* tex*/        {0, 1032, 1, 1, 1, 0 },
                          /* texgen*/     { -1, 0, 0, 0, 0, 0 },
                          /* texscale*/   { -1, 0, 0, 0, 0, 0 },
                          /* vfmuniforms*/{ -1, 0, 0, 0, 0, 0 },
                          /* vfuniforms*/ { -1, 0, 0, 0, 0, 0 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_Count, -1, 0},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_Count, -1, 0},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    
    };

static FragShaderDesc ShaderDesc_FS_FBatchTexTGVertexCxform = {
    /* Type */          FragShaderDesc::FS_FBatchTexTGVertexCxform,
    /* Flags */         Shader_Batch,
    /* pSource */       pSource_FTexTGVertexCxform,
    /* Uniforms */      { 
                          /* cxadd*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      { -1, 0, 0, 0, 0, 0 },
                          /* mvp*/        { -1, 0, 0, 0, 0, 0 },
                          /* offset*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor2*/    { -1, 0, 0, 0, 0, 0 },
                          /* srctex*/     { -1, 0, 0, 0, 0, 0 },
                          /* srctexscale*/{ -1, 0, 0, 0, 0, 0 },
                          /* tex*/        {0, 1032, 1, 1, 1, 0 },
                          /* texgen*/     { -1, 0, 0, 0, 0, 0 },
                          /* texscale*/   { -1, 0, 0, 0, 0, 0 },
                          /* vfmuniforms*/{ -1, 0, 0, 0, 0, 0 },
                          /* vfuniforms*/ { -1, 0, 0, 0, 0, 0 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_Count, -1, 0},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_Count, -1, 0},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    
    };

static FragShaderDesc ShaderDesc_FS_FBatchTexTGVertexCxformMul = {
    /* Type */          FragShaderDesc::FS_FBatchTexTGVertexCxformMul,
    /* Flags */         Shader_Batch,
    /* pSource */       pSource_FTexTGVertexCxformMul,
    /* Uniforms */      { 
                          /* cxadd*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      { -1, 0, 0, 0, 0, 0 },
                          /* mvp*/        { -1, 0, 0, 0, 0, 0 },
                          /* offset*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor2*/    { -1, 0, 0, 0, 0, 0 },
                          /* srctex*/     { -1, 0, 0, 0, 0, 0 },
                          /* srctexscale*/{ -1, 0, 0, 0, 0, 0 },
                          /* tex*/        {0, 1032, 1, 1, 1, 0 },
                          /* texgen*/     { -1, 0, 0, 0, 0, 0 },
                          /* texscale*/   { -1, 0, 0, 0, 0, 0 },
                          /* vfmuniforms*/{ -1, 0, 0, 0, 0, 0 },
                          /* vfuniforms*/ { -1, 0, 0, 0, 0, 0 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_Count, -1, 0},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_Count, -1, 0},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    
    };

static FragShaderDesc ShaderDesc_FS_FBatchTexTGVertexCxformEAlpha = {
    /* Type */          FragShaderDesc::FS_FBatchTexTGVertexCxformEAlpha,
    /* Flags */         Shader_Batch,
    /* pSource */       pSource_FTexTGVertexCxformEAlpha,
    /* Uniforms */      { 
                          /* cxadd*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      { -1, 0, 0, 0, 0, 0 },
                          /* mvp*/        { -1, 0, 0, 0, 0, 0 },
                          /* offset*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor2*/    { -1, 0, 0, 0, 0, 0 },
                          /* srctex*/     { -1, 0, 0, 0, 0, 0 },
                          /* srctexscale*/{ -1, 0, 0, 0, 0, 0 },
                          /* tex*/        {0, 1032, 1, 1, 1, 0 },
                          /* texgen*/     { -1, 0, 0, 0, 0, 0 },
                          /* texscale*/   { -1, 0, 0, 0, 0, 0 },
                          /* vfmuniforms*/{ -1, 0, 0, 0, 0, 0 },
                          /* vfuniforms*/ { -1, 0, 0, 0, 0, 0 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_Count, -1, 0},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_Count, -1, 0},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    
    };

static FragShaderDesc ShaderDesc_FS_FBatchTexTGVertexCxformEAlphaMul = {
    /* Type */          FragShaderDesc::FS_FBatchTexTGVertexCxformEAlphaMul,
    /* Flags */         Shader_Batch,
    /* pSource */       pSource_FTexTGVertexCxformEAlphaMul,
    /* Uniforms */      { 
                          /* cxadd*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      { -1, 0, 0, 0, 0, 0 },
                          /* mvp*/        { -1, 0, 0, 0, 0, 0 },
                          /* offset*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor2*/    { -1, 0, 0, 0, 0, 0 },
                          /* srctex*/     { -1, 0, 0, 0, 0, 0 },
                          /* srctexscale*/{ -1, 0, 0, 0, 0, 0 },
                          /* tex*/        {0, 1032, 1, 1, 1, 0 },
                          /* texgen*/     { -1, 0, 0, 0, 0, 0 },
                          /* texscale*/   { -1, 0, 0, 0, 0, 0 },
                          /* vfmuniforms*/{ -1, 0, 0, 0, 0, 0 },
                          /* vfuniforms*/ { -1, 0, 0, 0, 0, 0 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_Count, -1, 0},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_Count, -1, 0},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    
    };

static FragShaderDesc ShaderDesc_FS_FInstancedTexTG = {
    /* Type */          FragShaderDesc::FS_FInstancedTexTG,
    /* Flags */         Shader_Instanced,
    /* pSource */       pSource_FInstancedTexTG,
    /* Uniforms */      { 
                          /* cxadd*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      { -1, 0, 0, 0, 0, 0 },
                          /* mvp*/        { -1, 0, 0, 0, 0, 0 },
                          /* offset*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor2*/    { -1, 0, 0, 0, 0, 0 },
                          /* srctex*/     { -1, 0, 0, 0, 0, 0 },
                          /* srctexscale*/{ -1, 0, 0, 0, 0, 0 },
                          /* tex*/        {0, 1032, 1, 1, 1, 0 },
                          /* texgen*/     { -1, 0, 0, 0, 0, 0 },
                          /* texscale*/   { -1, 0, 0, 0, 0, 0 },
                          /* vfmuniforms*/{ -1, 0, 0, 0, 0, 0 },
                          /* vfuniforms*/ { -1, 0, 0, 0, 0, 0 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_Count, -1, 0},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_Count, -1, 0},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    
    };

static FragShaderDesc ShaderDesc_FS_FInstancedTexTGMul = {
    /* Type */          FragShaderDesc::FS_FInstancedTexTGMul,
    /* Flags */         Shader_Instanced,
    /* pSource */       pSource_FInstancedTexTGMul,
    /* Uniforms */      { 
                          /* cxadd*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      { -1, 0, 0, 0, 0, 0 },
                          /* mvp*/        { -1, 0, 0, 0, 0, 0 },
                          /* offset*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor2*/    { -1, 0, 0, 0, 0, 0 },
                          /* srctex*/     { -1, 0, 0, 0, 0, 0 },
                          /* srctexscale*/{ -1, 0, 0, 0, 0, 0 },
                          /* tex*/        {0, 1032, 1, 1, 1, 0 },
                          /* texgen*/     { -1, 0, 0, 0, 0, 0 },
                          /* texscale*/   { -1, 0, 0, 0, 0, 0 },
                          /* vfmuniforms*/{ -1, 0, 0, 0, 0, 0 },
                          /* vfuniforms*/ { -1, 0, 0, 0, 0, 0 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_Count, -1, 0},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_Count, -1, 0},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    
    };

static FragShaderDesc ShaderDesc_FS_FInstancedTexTGEAlpha = {
    /* Type */          FragShaderDesc::FS_FInstancedTexTGEAlpha,
    /* Flags */         Shader_Instanced,
    /* pSource */       pSource_FInstancedTexTGEAlpha,
    /* Uniforms */      { 
                          /* cxadd*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      { -1, 0, 0, 0, 0, 0 },
                          /* mvp*/        { -1, 0, 0, 0, 0, 0 },
                          /* offset*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor2*/    { -1, 0, 0, 0, 0, 0 },
                          /* srctex*/     { -1, 0, 0, 0, 0, 0 },
                          /* srctexscale*/{ -1, 0, 0, 0, 0, 0 },
                          /* tex*/        {0, 1032, 1, 1, 1, 0 },
                          /* texgen*/     { -1, 0, 0, 0, 0, 0 },
                          /* texscale*/   { -1, 0, 0, 0, 0, 0 },
                          /* vfmuniforms*/{ -1, 0, 0, 0, 0, 0 },
                          /* vfuniforms*/ { -1, 0, 0, 0, 0, 0 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_Count, -1, 0},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_Count, -1, 0},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    
    };

static FragShaderDesc ShaderDesc_FS_FInstancedTexTGEAlphaMul = {
    /* Type */          FragShaderDesc::FS_FInstancedTexTGEAlphaMul,
    /* Flags */         Shader_Instanced,
    /* pSource */       pSource_FInstancedTexTGEAlphaMul,
    /* Uniforms */      { 
                          /* cxadd*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      { -1, 0, 0, 0, 0, 0 },
                          /* mvp*/        { -1, 0, 0, 0, 0, 0 },
                          /* offset*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor2*/    { -1, 0, 0, 0, 0, 0 },
                          /* srctex*/     { -1, 0, 0, 0, 0, 0 },
                          /* srctexscale*/{ -1, 0, 0, 0, 0, 0 },
                          /* tex*/        {0, 1032, 1, 1, 1, 0 },
                          /* texgen*/     { -1, 0, 0, 0, 0, 0 },
                          /* texscale*/   { -1, 0, 0, 0, 0, 0 },
                          /* vfmuniforms*/{ -1, 0, 0, 0, 0, 0 },
                          /* vfuniforms*/ { -1, 0, 0, 0, 0, 0 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_Count, -1, 0},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_Count, -1, 0},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    
    };

static FragShaderDesc ShaderDesc_FS_FInstancedTexTGCxform = {
    /* Type */          FragShaderDesc::FS_FInstancedTexTGCxform,
    /* Flags */         Shader_Instanced,
    /* pSource */       pSource_FInstancedTexTGCxform,
    /* Uniforms */      { 
                          /* cxadd*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      { -1, 0, 0, 0, 0, 0 },
                          /* mvp*/        { -1, 0, 0, 0, 0, 0 },
                          /* offset*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor2*/    { -1, 0, 0, 0, 0, 0 },
                          /* srctex*/     { -1, 0, 0, 0, 0, 0 },
                          /* srctexscale*/{ -1, 0, 0, 0, 0, 0 },
                          /* tex*/        {0, 1032, 1, 1, 1, 0 },
                          /* texgen*/     { -1, 0, 0, 0, 0, 0 },
                          /* texscale*/   { -1, 0, 0, 0, 0, 0 },
                          /* vfmuniforms*/{ -1, 0, 0, 0, 0, 0 },
                          /* vfuniforms*/ { -1, 0, 0, 0, 0, 0 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_Count, -1, 0},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_Count, -1, 0},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    
    };

static FragShaderDesc ShaderDesc_FS_FInstancedTexTGCxformMul = {
    /* Type */          FragShaderDesc::FS_FInstancedTexTGCxformMul,
    /* Flags */         Shader_Instanced,
    /* pSource */       pSource_FInstancedTexTGCxformMul,
    /* Uniforms */      { 
                          /* cxadd*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      { -1, 0, 0, 0, 0, 0 },
                          /* mvp*/        { -1, 0, 0, 0, 0, 0 },
                          /* offset*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor2*/    { -1, 0, 0, 0, 0, 0 },
                          /* srctex*/     { -1, 0, 0, 0, 0, 0 },
                          /* srctexscale*/{ -1, 0, 0, 0, 0, 0 },
                          /* tex*/        {0, 1032, 1, 1, 1, 0 },
                          /* texgen*/     { -1, 0, 0, 0, 0, 0 },
                          /* texscale*/   { -1, 0, 0, 0, 0, 0 },
                          /* vfmuniforms*/{ -1, 0, 0, 0, 0, 0 },
                          /* vfuniforms*/ { -1, 0, 0, 0, 0, 0 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_Count, -1, 0},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_Count, -1, 0},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    
    };

static FragShaderDesc ShaderDesc_FS_FInstancedTexTGCxformEAlpha = {
    /* Type */          FragShaderDesc::FS_FInstancedTexTGCxformEAlpha,
    /* Flags */         Shader_Instanced,
    /* pSource */       pSource_FInstancedTexTGCxformEAlpha,
    /* Uniforms */      { 
                          /* cxadd*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      { -1, 0, 0, 0, 0, 0 },
                          /* mvp*/        { -1, 0, 0, 0, 0, 0 },
                          /* offset*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor2*/    { -1, 0, 0, 0, 0, 0 },
                          /* srctex*/     { -1, 0, 0, 0, 0, 0 },
                          /* srctexscale*/{ -1, 0, 0, 0, 0, 0 },
                          /* tex*/        {0, 1032, 1, 1, 1, 0 },
                          /* texgen*/     { -1, 0, 0, 0, 0, 0 },
                          /* texscale*/   { -1, 0, 0, 0, 0, 0 },
                          /* vfmuniforms*/{ -1, 0, 0, 0, 0, 0 },
                          /* vfuniforms*/ { -1, 0, 0, 0, 0, 0 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_Count, -1, 0},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_Count, -1, 0},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    
    };

static FragShaderDesc ShaderDesc_FS_FInstancedTexTGCxformEAlphaMul = {
    /* Type */          FragShaderDesc::FS_FInstancedTexTGCxformEAlphaMul,
    /* Flags */         Shader_Instanced,
    /* pSource */       pSource_FInstancedTexTGCxformEAlphaMul,
    /* Uniforms */      { 
                          /* cxadd*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      { -1, 0, 0, 0, 0, 0 },
                          /* mvp*/        { -1, 0, 0, 0, 0, 0 },
                          /* offset*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor2*/    { -1, 0, 0, 0, 0, 0 },
                          /* srctex*/     { -1, 0, 0, 0, 0, 0 },
                          /* srctexscale*/{ -1, 0, 0, 0, 0, 0 },
                          /* tex*/        {0, 1032, 1, 1, 1, 0 },
                          /* texgen*/     { -1, 0, 0, 0, 0, 0 },
                          /* texscale*/   { -1, 0, 0, 0, 0, 0 },
                          /* vfmuniforms*/{ -1, 0, 0, 0, 0, 0 },
                          /* vfuniforms*/ { -1, 0, 0, 0, 0, 0 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_Count, -1, 0},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_Count, -1, 0},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    
    };

static FragShaderDesc ShaderDesc_FS_FInstancedVertex = {
    /* Type */          FragShaderDesc::FS_FInstancedVertex,
    /* Flags */         Shader_Instanced,
    /* pSource */       pSource_FInstancedVertex,
    /* Uniforms */      { 
                          /* cxadd*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      { -1, 0, 0, 0, 0, 0 },
                          /* mvp*/        { -1, 0, 0, 0, 0, 0 },
                          /* offset*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor2*/    { -1, 0, 0, 0, 0, 0 },
                          /* srctex*/     { -1, 0, 0, 0, 0, 0 },
                          /* srctexscale*/{ -1, 0, 0, 0, 0, 0 },
                          /* tex*/        { -1, 0, 0, 0, 0, 0 },
                          /* texgen*/     { -1, 0, 0, 0, 0, 0 },
                          /* texscale*/   { -1, 0, 0, 0, 0, 0 },
                          /* vfmuniforms*/{ -1, 0, 0, 0, 0, 0 },
                          /* vfuniforms*/ { -1, 0, 0, 0, 0, 0 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_Count, -1, 0},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_Count, -1, 0},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    
    };

static FragShaderDesc ShaderDesc_FS_FInstancedVertexMul = {
    /* Type */          FragShaderDesc::FS_FInstancedVertexMul,
    /* Flags */         Shader_Instanced,
    /* pSource */       pSource_FInstancedVertexMul,
    /* Uniforms */      { 
                          /* cxadd*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      { -1, 0, 0, 0, 0, 0 },
                          /* mvp*/        { -1, 0, 0, 0, 0, 0 },
                          /* offset*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor2*/    { -1, 0, 0, 0, 0, 0 },
                          /* srctex*/     { -1, 0, 0, 0, 0, 0 },
                          /* srctexscale*/{ -1, 0, 0, 0, 0, 0 },
                          /* tex*/        { -1, 0, 0, 0, 0, 0 },
                          /* texgen*/     { -1, 0, 0, 0, 0, 0 },
                          /* texscale*/   { -1, 0, 0, 0, 0, 0 },
                          /* vfmuniforms*/{ -1, 0, 0, 0, 0, 0 },
                          /* vfuniforms*/ { -1, 0, 0, 0, 0, 0 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_Count, -1, 0},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_Count, -1, 0},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    
    };

static FragShaderDesc ShaderDesc_FS_FInstancedVertexEAlpha = {
    /* Type */          FragShaderDesc::FS_FInstancedVertexEAlpha,
    /* Flags */         Shader_Instanced,
    /* pSource */       pSource_FInstancedVertexEAlpha,
    /* Uniforms */      { 
                          /* cxadd*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      { -1, 0, 0, 0, 0, 0 },
                          /* mvp*/        { -1, 0, 0, 0, 0, 0 },
                          /* offset*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor2*/    { -1, 0, 0, 0, 0, 0 },
                          /* srctex*/     { -1, 0, 0, 0, 0, 0 },
                          /* srctexscale*/{ -1, 0, 0, 0, 0, 0 },
                          /* tex*/        { -1, 0, 0, 0, 0, 0 },
                          /* texgen*/     { -1, 0, 0, 0, 0, 0 },
                          /* texscale*/   { -1, 0, 0, 0, 0, 0 },
                          /* vfmuniforms*/{ -1, 0, 0, 0, 0, 0 },
                          /* vfuniforms*/ { -1, 0, 0, 0, 0, 0 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_Count, -1, 0},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_Count, -1, 0},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    
    };

static FragShaderDesc ShaderDesc_FS_FInstancedVertexEAlphaMul = {
    /* Type */          FragShaderDesc::FS_FInstancedVertexEAlphaMul,
    /* Flags */         Shader_Instanced,
    /* pSource */       pSource_FInstancedVertexEAlphaMul,
    /* Uniforms */      { 
                          /* cxadd*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      { -1, 0, 0, 0, 0, 0 },
                          /* mvp*/        { -1, 0, 0, 0, 0, 0 },
                          /* offset*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor2*/    { -1, 0, 0, 0, 0, 0 },
                          /* srctex*/     { -1, 0, 0, 0, 0, 0 },
                          /* srctexscale*/{ -1, 0, 0, 0, 0, 0 },
                          /* tex*/        { -1, 0, 0, 0, 0, 0 },
                          /* texgen*/     { -1, 0, 0, 0, 0, 0 },
                          /* texscale*/   { -1, 0, 0, 0, 0, 0 },
                          /* vfmuniforms*/{ -1, 0, 0, 0, 0, 0 },
                          /* vfuniforms*/ { -1, 0, 0, 0, 0, 0 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_Count, -1, 0},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_Count, -1, 0},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    
    };

static FragShaderDesc ShaderDesc_FS_FInstancedVertexCxform = {
    /* Type */          FragShaderDesc::FS_FInstancedVertexCxform,
    /* Flags */         Shader_Instanced,
    /* pSource */       pSource_FInstancedVertexCxform,
    /* Uniforms */      { 
                          /* cxadd*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      { -1, 0, 0, 0, 0, 0 },
                          /* mvp*/        { -1, 0, 0, 0, 0, 0 },
                          /* offset*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor2*/    { -1, 0, 0, 0, 0, 0 },
                          /* srctex*/     { -1, 0, 0, 0, 0, 0 },
                          /* srctexscale*/{ -1, 0, 0, 0, 0, 0 },
                          /* tex*/        { -1, 0, 0, 0, 0, 0 },
                          /* texgen*/     { -1, 0, 0, 0, 0, 0 },
                          /* texscale*/   { -1, 0, 0, 0, 0, 0 },
                          /* vfmuniforms*/{ -1, 0, 0, 0, 0, 0 },
                          /* vfuniforms*/ { -1, 0, 0, 0, 0, 0 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_Count, -1, 0},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_Count, -1, 0},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    
    };

static FragShaderDesc ShaderDesc_FS_FInstancedVertexCxformMul = {
    /* Type */          FragShaderDesc::FS_FInstancedVertexCxformMul,
    /* Flags */         Shader_Instanced,
    /* pSource */       pSource_FInstancedVertexCxformMul,
    /* Uniforms */      { 
                          /* cxadd*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      { -1, 0, 0, 0, 0, 0 },
                          /* mvp*/        { -1, 0, 0, 0, 0, 0 },
                          /* offset*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor2*/    { -1, 0, 0, 0, 0, 0 },
                          /* srctex*/     { -1, 0, 0, 0, 0, 0 },
                          /* srctexscale*/{ -1, 0, 0, 0, 0, 0 },
                          /* tex*/        { -1, 0, 0, 0, 0, 0 },
                          /* texgen*/     { -1, 0, 0, 0, 0, 0 },
                          /* texscale*/   { -1, 0, 0, 0, 0, 0 },
                          /* vfmuniforms*/{ -1, 0, 0, 0, 0, 0 },
                          /* vfuniforms*/ { -1, 0, 0, 0, 0, 0 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_Count, -1, 0},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_Count, -1, 0},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    
    };

static FragShaderDesc ShaderDesc_FS_FInstancedVertexCxformEAlpha = {
    /* Type */          FragShaderDesc::FS_FInstancedVertexCxformEAlpha,
    /* Flags */         Shader_Instanced,
    /* pSource */       pSource_FInstancedVertexCxformEAlpha,
    /* Uniforms */      { 
                          /* cxadd*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      { -1, 0, 0, 0, 0, 0 },
                          /* mvp*/        { -1, 0, 0, 0, 0, 0 },
                          /* offset*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor2*/    { -1, 0, 0, 0, 0, 0 },
                          /* srctex*/     { -1, 0, 0, 0, 0, 0 },
                          /* srctexscale*/{ -1, 0, 0, 0, 0, 0 },
                          /* tex*/        { -1, 0, 0, 0, 0, 0 },
                          /* texgen*/     { -1, 0, 0, 0, 0, 0 },
                          /* texscale*/   { -1, 0, 0, 0, 0, 0 },
                          /* vfmuniforms*/{ -1, 0, 0, 0, 0, 0 },
                          /* vfuniforms*/ { -1, 0, 0, 0, 0, 0 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_Count, -1, 0},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_Count, -1, 0},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    
    };

static FragShaderDesc ShaderDesc_FS_FInstancedVertexCxformEAlphaMul = {
    /* Type */          FragShaderDesc::FS_FInstancedVertexCxformEAlphaMul,
    /* Flags */         Shader_Instanced,
    /* pSource */       pSource_FInstancedVertexCxformEAlphaMul,
    /* Uniforms */      { 
                          /* cxadd*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      { -1, 0, 0, 0, 0, 0 },
                          /* mvp*/        { -1, 0, 0, 0, 0, 0 },
                          /* offset*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor2*/    { -1, 0, 0, 0, 0, 0 },
                          /* srctex*/     { -1, 0, 0, 0, 0, 0 },
                          /* srctexscale*/{ -1, 0, 0, 0, 0, 0 },
                          /* tex*/        { -1, 0, 0, 0, 0, 0 },
                          /* texgen*/     { -1, 0, 0, 0, 0, 0 },
                          /* texscale*/   { -1, 0, 0, 0, 0, 0 },
                          /* vfmuniforms*/{ -1, 0, 0, 0, 0, 0 },
                          /* vfuniforms*/ { -1, 0, 0, 0, 0, 0 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_Count, -1, 0},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_Count, -1, 0},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    
    };

static FragShaderDesc ShaderDesc_FS_FInstancedTexTGTexTG = {
    /* Type */          FragShaderDesc::FS_FInstancedTexTGTexTG,
    /* Flags */         Shader_Instanced,
    /* pSource */       pSource_FInstancedTexTGTexTG,
    /* Uniforms */      { 
                          /* cxadd*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      { -1, 0, 0, 0, 0, 0 },
                          /* mvp*/        { -1, 0, 0, 0, 0, 0 },
                          /* offset*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor2*/    { -1, 0, 0, 0, 0, 0 },
                          /* srctex*/     { -1, 0, 0, 0, 0, 0 },
                          /* srctexscale*/{ -1, 0, 0, 0, 0, 0 },
                          /* tex*/        {0, 1032, 1, 2, 1, 0 },
                          /* texgen*/     { -1, 0, 0, 0, 0, 0 },
                          /* texscale*/   { -1, 0, 0, 0, 0, 0 },
                          /* vfmuniforms*/{ -1, 0, 0, 0, 0, 0 },
                          /* vfuniforms*/ { -1, 0, 0, 0, 0, 0 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_Count, -1, 0},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_Count, -1, 0},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    
    };

static FragShaderDesc ShaderDesc_FS_FInstancedTexTGTexTGMul = {
    /* Type */          FragShaderDesc::FS_FInstancedTexTGTexTGMul,
    /* Flags */         Shader_Instanced,
    /* pSource */       pSource_FInstancedTexTGTexTGMul,
    /* Uniforms */      { 
                          /* cxadd*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      { -1, 0, 0, 0, 0, 0 },
                          /* mvp*/        { -1, 0, 0, 0, 0, 0 },
                          /* offset*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor2*/    { -1, 0, 0, 0, 0, 0 },
                          /* srctex*/     { -1, 0, 0, 0, 0, 0 },
                          /* srctexscale*/{ -1, 0, 0, 0, 0, 0 },
                          /* tex*/        {0, 1032, 1, 2, 1, 0 },
                          /* texgen*/     { -1, 0, 0, 0, 0, 0 },
                          /* texscale*/   { -1, 0, 0, 0, 0, 0 },
                          /* vfmuniforms*/{ -1, 0, 0, 0, 0, 0 },
                          /* vfuniforms*/ { -1, 0, 0, 0, 0, 0 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_Count, -1, 0},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_Count, -1, 0},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    
    };

static FragShaderDesc ShaderDesc_FS_FInstancedTexTGTexTGEAlpha = {
    /* Type */          FragShaderDesc::FS_FInstancedTexTGTexTGEAlpha,
    /* Flags */         Shader_Instanced,
    /* pSource */       pSource_FInstancedTexTGTexTGEAlpha,
    /* Uniforms */      { 
                          /* cxadd*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      { -1, 0, 0, 0, 0, 0 },
                          /* mvp*/        { -1, 0, 0, 0, 0, 0 },
                          /* offset*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor2*/    { -1, 0, 0, 0, 0, 0 },
                          /* srctex*/     { -1, 0, 0, 0, 0, 0 },
                          /* srctexscale*/{ -1, 0, 0, 0, 0, 0 },
                          /* tex*/        {0, 1032, 1, 2, 1, 0 },
                          /* texgen*/     { -1, 0, 0, 0, 0, 0 },
                          /* texscale*/   { -1, 0, 0, 0, 0, 0 },
                          /* vfmuniforms*/{ -1, 0, 0, 0, 0, 0 },
                          /* vfuniforms*/ { -1, 0, 0, 0, 0, 0 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_Count, -1, 0},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_Count, -1, 0},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    
    };

static FragShaderDesc ShaderDesc_FS_FInstancedTexTGTexTGEAlphaMul = {
    /* Type */          FragShaderDesc::FS_FInstancedTexTGTexTGEAlphaMul,
    /* Flags */         Shader_Instanced,
    /* pSource */       pSource_FInstancedTexTGTexTGEAlphaMul,
    /* Uniforms */      { 
                          /* cxadd*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      { -1, 0, 0, 0, 0, 0 },
                          /* mvp*/        { -1, 0, 0, 0, 0, 0 },
                          /* offset*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor2*/    { -1, 0, 0, 0, 0, 0 },
                          /* srctex*/     { -1, 0, 0, 0, 0, 0 },
                          /* srctexscale*/{ -1, 0, 0, 0, 0, 0 },
                          /* tex*/        {0, 1032, 1, 2, 1, 0 },
                          /* texgen*/     { -1, 0, 0, 0, 0, 0 },
                          /* texscale*/   { -1, 0, 0, 0, 0, 0 },
                          /* vfmuniforms*/{ -1, 0, 0, 0, 0, 0 },
                          /* vfuniforms*/ { -1, 0, 0, 0, 0, 0 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_Count, -1, 0},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_Count, -1, 0},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    
    };

static FragShaderDesc ShaderDesc_FS_FInstancedTexTGTexTGCxform = {
    /* Type */          FragShaderDesc::FS_FInstancedTexTGTexTGCxform,
    /* Flags */         Shader_Instanced,
    /* pSource */       pSource_FInstancedTexTGTexTGCxform,
    /* Uniforms */      { 
                          /* cxadd*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      { -1, 0, 0, 0, 0, 0 },
                          /* mvp*/        { -1, 0, 0, 0, 0, 0 },
                          /* offset*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor2*/    { -1, 0, 0, 0, 0, 0 },
                          /* srctex*/     { -1, 0, 0, 0, 0, 0 },
                          /* srctexscale*/{ -1, 0, 0, 0, 0, 0 },
                          /* tex*/        {0, 1032, 1, 2, 1, 0 },
                          /* texgen*/     { -1, 0, 0, 0, 0, 0 },
                          /* texscale*/   { -1, 0, 0, 0, 0, 0 },
                          /* vfmuniforms*/{ -1, 0, 0, 0, 0, 0 },
                          /* vfuniforms*/ { -1, 0, 0, 0, 0, 0 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_Count, -1, 0},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_Count, -1, 0},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    
    };

static FragShaderDesc ShaderDesc_FS_FInstancedTexTGTexTGCxformMul = {
    /* Type */          FragShaderDesc::FS_FInstancedTexTGTexTGCxformMul,
    /* Flags */         Shader_Instanced,
    /* pSource */       pSource_FInstancedTexTGTexTGCxformMul,
    /* Uniforms */      { 
                          /* cxadd*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      { -1, 0, 0, 0, 0, 0 },
                          /* mvp*/        { -1, 0, 0, 0, 0, 0 },
                          /* offset*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor2*/    { -1, 0, 0, 0, 0, 0 },
                          /* srctex*/     { -1, 0, 0, 0, 0, 0 },
                          /* srctexscale*/{ -1, 0, 0, 0, 0, 0 },
                          /* tex*/        {0, 1032, 1, 2, 1, 0 },
                          /* texgen*/     { -1, 0, 0, 0, 0, 0 },
                          /* texscale*/   { -1, 0, 0, 0, 0, 0 },
                          /* vfmuniforms*/{ -1, 0, 0, 0, 0, 0 },
                          /* vfuniforms*/ { -1, 0, 0, 0, 0, 0 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_Count, -1, 0},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_Count, -1, 0},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    
    };

static FragShaderDesc ShaderDesc_FS_FInstancedTexTGTexTGCxformEAlpha = {
    /* Type */          FragShaderDesc::FS_FInstancedTexTGTexTGCxformEAlpha,
    /* Flags */         Shader_Instanced,
    /* pSource */       pSource_FInstancedTexTGTexTGCxformEAlpha,
    /* Uniforms */      { 
                          /* cxadd*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      { -1, 0, 0, 0, 0, 0 },
                          /* mvp*/        { -1, 0, 0, 0, 0, 0 },
                          /* offset*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor2*/    { -1, 0, 0, 0, 0, 0 },
                          /* srctex*/     { -1, 0, 0, 0, 0, 0 },
                          /* srctexscale*/{ -1, 0, 0, 0, 0, 0 },
                          /* tex*/        {0, 1032, 1, 2, 1, 0 },
                          /* texgen*/     { -1, 0, 0, 0, 0, 0 },
                          /* texscale*/   { -1, 0, 0, 0, 0, 0 },
                          /* vfmuniforms*/{ -1, 0, 0, 0, 0, 0 },
                          /* vfuniforms*/ { -1, 0, 0, 0, 0, 0 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_Count, -1, 0},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_Count, -1, 0},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    
    };

static FragShaderDesc ShaderDesc_FS_FInstancedTexTGTexTGCxformEAlphaMul = {
    /* Type */          FragShaderDesc::FS_FInstancedTexTGTexTGCxformEAlphaMul,
    /* Flags */         Shader_Instanced,
    /* pSource */       pSource_FInstancedTexTGTexTGCxformEAlphaMul,
    /* Uniforms */      { 
                          /* cxadd*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      { -1, 0, 0, 0, 0, 0 },
                          /* mvp*/        { -1, 0, 0, 0, 0, 0 },
                          /* offset*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor2*/    { -1, 0, 0, 0, 0, 0 },
                          /* srctex*/     { -1, 0, 0, 0, 0, 0 },
                          /* srctexscale*/{ -1, 0, 0, 0, 0, 0 },
                          /* tex*/        {0, 1032, 1, 2, 1, 0 },
                          /* texgen*/     { -1, 0, 0, 0, 0, 0 },
                          /* texscale*/   { -1, 0, 0, 0, 0, 0 },
                          /* vfmuniforms*/{ -1, 0, 0, 0, 0, 0 },
                          /* vfuniforms*/ { -1, 0, 0, 0, 0, 0 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_Count, -1, 0},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_Count, -1, 0},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    
    };

static FragShaderDesc ShaderDesc_FS_FInstancedTexTGVertex = {
    /* Type */          FragShaderDesc::FS_FInstancedTexTGVertex,
    /* Flags */         Shader_Instanced,
    /* pSource */       pSource_FInstancedTexTGVertex,
    /* Uniforms */      { 
                          /* cxadd*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      { -1, 0, 0, 0, 0, 0 },
                          /* mvp*/        { -1, 0, 0, 0, 0, 0 },
                          /* offset*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor2*/    { -1, 0, 0, 0, 0, 0 },
                          /* srctex*/     { -1, 0, 0, 0, 0, 0 },
                          /* srctexscale*/{ -1, 0, 0, 0, 0, 0 },
                          /* tex*/        {0, 1032, 1, 1, 1, 0 },
                          /* texgen*/     { -1, 0, 0, 0, 0, 0 },
                          /* texscale*/   { -1, 0, 0, 0, 0, 0 },
                          /* vfmuniforms*/{ -1, 0, 0, 0, 0, 0 },
                          /* vfuniforms*/ { -1, 0, 0, 0, 0, 0 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_Count, -1, 0},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_Count, -1, 0},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    
    };

static FragShaderDesc ShaderDesc_FS_FInstancedTexTGVertexMul = {
    /* Type */          FragShaderDesc::FS_FInstancedTexTGVertexMul,
    /* Flags */         Shader_Instanced,
    /* pSource */       pSource_FInstancedTexTGVertexMul,
    /* Uniforms */      { 
                          /* cxadd*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      { -1, 0, 0, 0, 0, 0 },
                          /* mvp*/        { -1, 0, 0, 0, 0, 0 },
                          /* offset*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor2*/    { -1, 0, 0, 0, 0, 0 },
                          /* srctex*/     { -1, 0, 0, 0, 0, 0 },
                          /* srctexscale*/{ -1, 0, 0, 0, 0, 0 },
                          /* tex*/        {0, 1032, 1, 1, 1, 0 },
                          /* texgen*/     { -1, 0, 0, 0, 0, 0 },
                          /* texscale*/   { -1, 0, 0, 0, 0, 0 },
                          /* vfmuniforms*/{ -1, 0, 0, 0, 0, 0 },
                          /* vfuniforms*/ { -1, 0, 0, 0, 0, 0 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_Count, -1, 0},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_Count, -1, 0},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    
    };

static FragShaderDesc ShaderDesc_FS_FInstancedTexTGVertexEAlpha = {
    /* Type */          FragShaderDesc::FS_FInstancedTexTGVertexEAlpha,
    /* Flags */         Shader_Instanced,
    /* pSource */       pSource_FInstancedTexTGVertexEAlpha,
    /* Uniforms */      { 
                          /* cxadd*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      { -1, 0, 0, 0, 0, 0 },
                          /* mvp*/        { -1, 0, 0, 0, 0, 0 },
                          /* offset*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor2*/    { -1, 0, 0, 0, 0, 0 },
                          /* srctex*/     { -1, 0, 0, 0, 0, 0 },
                          /* srctexscale*/{ -1, 0, 0, 0, 0, 0 },
                          /* tex*/        {0, 1032, 1, 1, 1, 0 },
                          /* texgen*/     { -1, 0, 0, 0, 0, 0 },
                          /* texscale*/   { -1, 0, 0, 0, 0, 0 },
                          /* vfmuniforms*/{ -1, 0, 0, 0, 0, 0 },
                          /* vfuniforms*/ { -1, 0, 0, 0, 0, 0 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_Count, -1, 0},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_Count, -1, 0},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    
    };

static FragShaderDesc ShaderDesc_FS_FInstancedTexTGVertexEAlphaMul = {
    /* Type */          FragShaderDesc::FS_FInstancedTexTGVertexEAlphaMul,
    /* Flags */         Shader_Instanced,
    /* pSource */       pSource_FInstancedTexTGVertexEAlphaMul,
    /* Uniforms */      { 
                          /* cxadd*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      { -1, 0, 0, 0, 0, 0 },
                          /* mvp*/        { -1, 0, 0, 0, 0, 0 },
                          /* offset*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor2*/    { -1, 0, 0, 0, 0, 0 },
                          /* srctex*/     { -1, 0, 0, 0, 0, 0 },
                          /* srctexscale*/{ -1, 0, 0, 0, 0, 0 },
                          /* tex*/        {0, 1032, 1, 1, 1, 0 },
                          /* texgen*/     { -1, 0, 0, 0, 0, 0 },
                          /* texscale*/   { -1, 0, 0, 0, 0, 0 },
                          /* vfmuniforms*/{ -1, 0, 0, 0, 0, 0 },
                          /* vfuniforms*/ { -1, 0, 0, 0, 0, 0 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_Count, -1, 0},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_Count, -1, 0},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    
    };

static FragShaderDesc ShaderDesc_FS_FInstancedTexTGVertexCxform = {
    /* Type */          FragShaderDesc::FS_FInstancedTexTGVertexCxform,
    /* Flags */         Shader_Instanced,
    /* pSource */       pSource_FInstancedTexTGVertexCxform,
    /* Uniforms */      { 
                          /* cxadd*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      { -1, 0, 0, 0, 0, 0 },
                          /* mvp*/        { -1, 0, 0, 0, 0, 0 },
                          /* offset*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor2*/    { -1, 0, 0, 0, 0, 0 },
                          /* srctex*/     { -1, 0, 0, 0, 0, 0 },
                          /* srctexscale*/{ -1, 0, 0, 0, 0, 0 },
                          /* tex*/        {0, 1032, 1, 1, 1, 0 },
                          /* texgen*/     { -1, 0, 0, 0, 0, 0 },
                          /* texscale*/   { -1, 0, 0, 0, 0, 0 },
                          /* vfmuniforms*/{ -1, 0, 0, 0, 0, 0 },
                          /* vfuniforms*/ { -1, 0, 0, 0, 0, 0 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_Count, -1, 0},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_Count, -1, 0},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    
    };

static FragShaderDesc ShaderDesc_FS_FInstancedTexTGVertexCxformMul = {
    /* Type */          FragShaderDesc::FS_FInstancedTexTGVertexCxformMul,
    /* Flags */         Shader_Instanced,
    /* pSource */       pSource_FInstancedTexTGVertexCxformMul,
    /* Uniforms */      { 
                          /* cxadd*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      { -1, 0, 0, 0, 0, 0 },
                          /* mvp*/        { -1, 0, 0, 0, 0, 0 },
                          /* offset*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor2*/    { -1, 0, 0, 0, 0, 0 },
                          /* srctex*/     { -1, 0, 0, 0, 0, 0 },
                          /* srctexscale*/{ -1, 0, 0, 0, 0, 0 },
                          /* tex*/        {0, 1032, 1, 1, 1, 0 },
                          /* texgen*/     { -1, 0, 0, 0, 0, 0 },
                          /* texscale*/   { -1, 0, 0, 0, 0, 0 },
                          /* vfmuniforms*/{ -1, 0, 0, 0, 0, 0 },
                          /* vfuniforms*/ { -1, 0, 0, 0, 0, 0 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_Count, -1, 0},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_Count, -1, 0},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    
    };

static FragShaderDesc ShaderDesc_FS_FInstancedTexTGVertexCxformEAlpha = {
    /* Type */          FragShaderDesc::FS_FInstancedTexTGVertexCxformEAlpha,
    /* Flags */         Shader_Instanced,
    /* pSource */       pSource_FInstancedTexTGVertexCxformEAlpha,
    /* Uniforms */      { 
                          /* cxadd*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      { -1, 0, 0, 0, 0, 0 },
                          /* mvp*/        { -1, 0, 0, 0, 0, 0 },
                          /* offset*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor2*/    { -1, 0, 0, 0, 0, 0 },
                          /* srctex*/     { -1, 0, 0, 0, 0, 0 },
                          /* srctexscale*/{ -1, 0, 0, 0, 0, 0 },
                          /* tex*/        {0, 1032, 1, 1, 1, 0 },
                          /* texgen*/     { -1, 0, 0, 0, 0, 0 },
                          /* texscale*/   { -1, 0, 0, 0, 0, 0 },
                          /* vfmuniforms*/{ -1, 0, 0, 0, 0, 0 },
                          /* vfuniforms*/ { -1, 0, 0, 0, 0, 0 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_Count, -1, 0},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_Count, -1, 0},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    
    };

static FragShaderDesc ShaderDesc_FS_FInstancedTexTGVertexCxformEAlphaMul = {
    /* Type */          FragShaderDesc::FS_FInstancedTexTGVertexCxformEAlphaMul,
    /* Flags */         Shader_Instanced,
    /* pSource */       pSource_FInstancedTexTGVertexCxformEAlphaMul,
    /* Uniforms */      { 
                          /* cxadd*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      { -1, 0, 0, 0, 0, 0 },
                          /* mvp*/        { -1, 0, 0, 0, 0, 0 },
                          /* offset*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor2*/    { -1, 0, 0, 0, 0, 0 },
                          /* srctex*/     { -1, 0, 0, 0, 0, 0 },
                          /* srctexscale*/{ -1, 0, 0, 0, 0, 0 },
                          /* tex*/        {0, 1032, 1, 1, 1, 0 },
                          /* texgen*/     { -1, 0, 0, 0, 0, 0 },
                          /* texscale*/   { -1, 0, 0, 0, 0, 0 },
                          /* vfmuniforms*/{ -1, 0, 0, 0, 0, 0 },
                          /* vfuniforms*/ { -1, 0, 0, 0, 0, 0 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_Count, -1, 0},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_Count, -1, 0},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    
    };

static FragShaderDesc ShaderDesc_FS_FSolid = {
    /* Type */          FragShaderDesc::FS_FSolid,
    /* Flags */         0,
    /* pSource */       pSource_FSolid,
    /* Uniforms */      { 
                          /* cxadd*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul*/      {0, 1032, 4, 4, 4, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      { -1, 0, 0, 0, 0, 0 },
                          /* mvp*/        { -1, 0, 0, 0, 0, 0 },
                          /* offset*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor2*/    { -1, 0, 0, 0, 0, 0 },
                          /* srctex*/     { -1, 0, 0, 0, 0, 0 },
                          /* srctexscale*/{ -1, 0, 0, 0, 0, 0 },
                          /* tex*/        { -1, 0, 0, 0, 0, 0 },
                          /* texgen*/     { -1, 0, 0, 0, 0, 0 },
                          /* texscale*/   { -1, 0, 0, 0, 0, 0 },
                          /* vfmuniforms*/{ -1, 0, 0, 0, 0, 0 },
                          /* vfuniforms*/ { -1, 0, 0, 0, 0, 0 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_Count, -1, 0},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_Count, -1, 0},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    
    };

static FragShaderDesc ShaderDesc_FS_FSolidMul = {
    /* Type */          FragShaderDesc::FS_FSolidMul,
    /* Flags */         0,
    /* pSource */       pSource_FSolidMul,
    /* Uniforms */      { 
                          /* cxadd*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul*/      {0, 1032, 4, 4, 4, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      { -1, 0, 0, 0, 0, 0 },
                          /* mvp*/        { -1, 0, 0, 0, 0, 0 },
                          /* offset*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor2*/    { -1, 0, 0, 0, 0, 0 },
                          /* srctex*/     { -1, 0, 0, 0, 0, 0 },
                          /* srctexscale*/{ -1, 0, 0, 0, 0, 0 },
                          /* tex*/        { -1, 0, 0, 0, 0, 0 },
                          /* texgen*/     { -1, 0, 0, 0, 0, 0 },
                          /* texscale*/   { -1, 0, 0, 0, 0, 0 },
                          /* vfmuniforms*/{ -1, 0, 0, 0, 0, 0 },
                          /* vfuniforms*/ { -1, 0, 0, 0, 0, 0 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_Count, -1, 0},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_Count, -1, 0},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    
    };

static FragShaderDesc ShaderDesc_FS_FText = {
    /* Type */          FragShaderDesc::FS_FText,
    /* Flags */         0,
    /* pSource */       pSource_FText,
    /* Uniforms */      { 
                          /* cxadd*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      { -1, 0, 0, 0, 0, 0 },
                          /* mvp*/        { -1, 0, 0, 0, 0, 0 },
                          /* offset*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor2*/    { -1, 0, 0, 0, 0, 0 },
                          /* srctex*/     { -1, 0, 0, 0, 0, 0 },
                          /* srctexscale*/{ -1, 0, 0, 0, 0, 0 },
                          /* tex*/        {0, 1032, 1, 1, 1, 0 },
                          /* texgen*/     { -1, 0, 0, 0, 0, 0 },
                          /* texscale*/   { -1, 0, 0, 0, 0, 0 },
                          /* vfmuniforms*/{ -1, 0, 0, 0, 0, 0 },
                          /* vfuniforms*/ { -1, 0, 0, 0, 0, 0 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_Count, -1, 0},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_Count, -1, 0},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    
    };

static FragShaderDesc ShaderDesc_FS_FTextMul = {
    /* Type */          FragShaderDesc::FS_FTextMul,
    /* Flags */         0,
    /* pSource */       pSource_FTextMul,
    /* Uniforms */      { 
                          /* cxadd*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      { -1, 0, 0, 0, 0, 0 },
                          /* mvp*/        { -1, 0, 0, 0, 0, 0 },
                          /* offset*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor2*/    { -1, 0, 0, 0, 0, 0 },
                          /* srctex*/     { -1, 0, 0, 0, 0, 0 },
                          /* srctexscale*/{ -1, 0, 0, 0, 0, 0 },
                          /* tex*/        {0, 1032, 1, 1, 1, 0 },
                          /* texgen*/     { -1, 0, 0, 0, 0, 0 },
                          /* texscale*/   { -1, 0, 0, 0, 0, 0 },
                          /* vfmuniforms*/{ -1, 0, 0, 0, 0, 0 },
                          /* vfuniforms*/ { -1, 0, 0, 0, 0, 0 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_Count, -1, 0},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_Count, -1, 0},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    
    };

static FragShaderDesc ShaderDesc_FS_FBatchSolid = {
    /* Type */          FragShaderDesc::FS_FBatchSolid,
    /* Flags */         Shader_Batch,
    /* pSource */       pSource_FSolid,
    /* Uniforms */      { 
                          /* cxadd*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul*/      {0, 1032, 4, 4, 4, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      { -1, 0, 0, 0, 0, 0 },
                          /* mvp*/        { -1, 0, 0, 0, 0, 0 },
                          /* offset*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor2*/    { -1, 0, 0, 0, 0, 0 },
                          /* srctex*/     { -1, 0, 0, 0, 0, 0 },
                          /* srctexscale*/{ -1, 0, 0, 0, 0, 0 },
                          /* tex*/        { -1, 0, 0, 0, 0, 0 },
                          /* texgen*/     { -1, 0, 0, 0, 0, 0 },
                          /* texscale*/   { -1, 0, 0, 0, 0, 0 },
                          /* vfmuniforms*/{ -1, 0, 0, 0, 0, 0 },
                          /* vfuniforms*/ { -1, 0, 0, 0, 0, 0 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_Count, -1, 0},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_Count, -1, 0},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    
    };

static FragShaderDesc ShaderDesc_FS_FBatchSolidMul = {
    /* Type */          FragShaderDesc::FS_FBatchSolidMul,
    /* Flags */         Shader_Batch,
    /* pSource */       pSource_FSolidMul,
    /* Uniforms */      { 
                          /* cxadd*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul*/      {0, 1032, 4, 4, 4, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      { -1, 0, 0, 0, 0, 0 },
                          /* mvp*/        { -1, 0, 0, 0, 0, 0 },
                          /* offset*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor2*/    { -1, 0, 0, 0, 0, 0 },
                          /* srctex*/     { -1, 0, 0, 0, 0, 0 },
                          /* srctexscale*/{ -1, 0, 0, 0, 0, 0 },
                          /* tex*/        { -1, 0, 0, 0, 0, 0 },
                          /* texgen*/     { -1, 0, 0, 0, 0, 0 },
                          /* texscale*/   { -1, 0, 0, 0, 0, 0 },
                          /* vfmuniforms*/{ -1, 0, 0, 0, 0, 0 },
                          /* vfuniforms*/ { -1, 0, 0, 0, 0, 0 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_Count, -1, 0},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_Count, -1, 0},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    
    };

static FragShaderDesc ShaderDesc_FS_FBatchText = {
    /* Type */          FragShaderDesc::FS_FBatchText,
    /* Flags */         Shader_Batch,
    /* pSource */       pSource_FText,
    /* Uniforms */      { 
                          /* cxadd*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      { -1, 0, 0, 0, 0, 0 },
                          /* mvp*/        { -1, 0, 0, 0, 0, 0 },
                          /* offset*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor2*/    { -1, 0, 0, 0, 0, 0 },
                          /* srctex*/     { -1, 0, 0, 0, 0, 0 },
                          /* srctexscale*/{ -1, 0, 0, 0, 0, 0 },
                          /* tex*/        {0, 1032, 1, 1, 1, 0 },
                          /* texgen*/     { -1, 0, 0, 0, 0, 0 },
                          /* texscale*/   { -1, 0, 0, 0, 0, 0 },
                          /* vfmuniforms*/{ -1, 0, 0, 0, 0, 0 },
                          /* vfuniforms*/ { -1, 0, 0, 0, 0, 0 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_Count, -1, 0},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_Count, -1, 0},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    
    };

static FragShaderDesc ShaderDesc_FS_FBatchTextMul = {
    /* Type */          FragShaderDesc::FS_FBatchTextMul,
    /* Flags */         Shader_Batch,
    /* pSource */       pSource_FTextMul,
    /* Uniforms */      { 
                          /* cxadd*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      { -1, 0, 0, 0, 0, 0 },
                          /* mvp*/        { -1, 0, 0, 0, 0, 0 },
                          /* offset*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor2*/    { -1, 0, 0, 0, 0, 0 },
                          /* srctex*/     { -1, 0, 0, 0, 0, 0 },
                          /* srctexscale*/{ -1, 0, 0, 0, 0, 0 },
                          /* tex*/        {0, 1032, 1, 1, 1, 0 },
                          /* texgen*/     { -1, 0, 0, 0, 0, 0 },
                          /* texscale*/   { -1, 0, 0, 0, 0, 0 },
                          /* vfmuniforms*/{ -1, 0, 0, 0, 0, 0 },
                          /* vfuniforms*/ { -1, 0, 0, 0, 0, 0 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_Count, -1, 0},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_Count, -1, 0},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    
    };

static FragShaderDesc ShaderDesc_FS_FInstancedSolid = {
    /* Type */          FragShaderDesc::FS_FInstancedSolid,
    /* Flags */         Shader_Instanced,
    /* pSource */       pSource_FInstancedSolid,
    /* Uniforms */      { 
                          /* cxadd*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul*/      {0, 1032, 4, 4, 4, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      { -1, 0, 0, 0, 0, 0 },
                          /* mvp*/        { -1, 0, 0, 0, 0, 0 },
                          /* offset*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor2*/    { -1, 0, 0, 0, 0, 0 },
                          /* srctex*/     { -1, 0, 0, 0, 0, 0 },
                          /* srctexscale*/{ -1, 0, 0, 0, 0, 0 },
                          /* tex*/        { -1, 0, 0, 0, 0, 0 },
                          /* texgen*/     { -1, 0, 0, 0, 0, 0 },
                          /* texscale*/   { -1, 0, 0, 0, 0, 0 },
                          /* vfmuniforms*/{ -1, 0, 0, 0, 0, 0 },
                          /* vfuniforms*/ { -1, 0, 0, 0, 0, 0 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_Count, -1, 0},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_Count, -1, 0},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    
    };

static FragShaderDesc ShaderDesc_FS_FInstancedSolidMul = {
    /* Type */          FragShaderDesc::FS_FInstancedSolidMul,
    /* Flags */         Shader_Instanced,
    /* pSource */       pSource_FInstancedSolidMul,
    /* Uniforms */      { 
                          /* cxadd*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul*/      {0, 1032, 4, 4, 4, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      { -1, 0, 0, 0, 0, 0 },
                          /* mvp*/        { -1, 0, 0, 0, 0, 0 },
                          /* offset*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor2*/    { -1, 0, 0, 0, 0, 0 },
                          /* srctex*/     { -1, 0, 0, 0, 0, 0 },
                          /* srctexscale*/{ -1, 0, 0, 0, 0, 0 },
                          /* tex*/        { -1, 0, 0, 0, 0, 0 },
                          /* texgen*/     { -1, 0, 0, 0, 0, 0 },
                          /* texscale*/   { -1, 0, 0, 0, 0, 0 },
                          /* vfmuniforms*/{ -1, 0, 0, 0, 0, 0 },
                          /* vfuniforms*/ { -1, 0, 0, 0, 0, 0 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_Count, -1, 0},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_Count, -1, 0},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    
    };

static FragShaderDesc ShaderDesc_FS_FInstancedText = {
    /* Type */          FragShaderDesc::FS_FInstancedText,
    /* Flags */         Shader_Instanced,
    /* pSource */       pSource_FInstancedText,
    /* Uniforms */      { 
                          /* cxadd*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      { -1, 0, 0, 0, 0, 0 },
                          /* mvp*/        { -1, 0, 0, 0, 0, 0 },
                          /* offset*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor2*/    { -1, 0, 0, 0, 0, 0 },
                          /* srctex*/     { -1, 0, 0, 0, 0, 0 },
                          /* srctexscale*/{ -1, 0, 0, 0, 0, 0 },
                          /* tex*/        {0, 1032, 1, 1, 1, 0 },
                          /* texgen*/     { -1, 0, 0, 0, 0, 0 },
                          /* texscale*/   { -1, 0, 0, 0, 0, 0 },
                          /* vfmuniforms*/{ -1, 0, 0, 0, 0, 0 },
                          /* vfuniforms*/ { -1, 0, 0, 0, 0, 0 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_Count, -1, 0},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_Count, -1, 0},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    
    };

static FragShaderDesc ShaderDesc_FS_FInstancedTextMul = {
    /* Type */          FragShaderDesc::FS_FInstancedTextMul,
    /* Flags */         Shader_Instanced,
    /* pSource */       pSource_FInstancedTextMul,
    /* Uniforms */      { 
                          /* cxadd*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      { -1, 0, 0, 0, 0, 0 },
                          /* mvp*/        { -1, 0, 0, 0, 0, 0 },
                          /* offset*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor2*/    { -1, 0, 0, 0, 0, 0 },
                          /* srctex*/     { -1, 0, 0, 0, 0, 0 },
                          /* srctexscale*/{ -1, 0, 0, 0, 0, 0 },
                          /* tex*/        {0, 1032, 1, 1, 1, 0 },
                          /* texgen*/     { -1, 0, 0, 0, 0, 0 },
                          /* texscale*/   { -1, 0, 0, 0, 0, 0 },
                          /* vfmuniforms*/{ -1, 0, 0, 0, 0, 0 },
                          /* vfuniforms*/ { -1, 0, 0, 0, 0, 0 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_Count, -1, 0},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_Count, -1, 0},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    
    };

static FragShaderDesc ShaderDesc_FS_FYUV = {
    /* Type */          FragShaderDesc::FS_FYUV,
    /* Flags */         0,
    /* pSource */       pSource_FYUV,
    /* Uniforms */      { 
                          /* cxadd*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      { -1, 0, 0, 0, 0, 0 },
                          /* mvp*/        { -1, 0, 0, 0, 0, 0 },
                          /* offset*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor2*/    { -1, 0, 0, 0, 0, 0 },
                          /* srctex*/     { -1, 0, 0, 0, 0, 0 },
                          /* srctexscale*/{ -1, 0, 0, 0, 0, 0 },
                          /* tex*/        {0, 1032, 1, 3, 1, 0 },
                          /* texgen*/     { -1, 0, 0, 0, 0, 0 },
                          /* texscale*/   { -1, 0, 0, 0, 0, 0 },
                          /* vfmuniforms*/{ -1, 0, 0, 0, 0, 0 },
                          /* vfuniforms*/ { -1, 0, 0, 0, 0, 0 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_Count, -1, 0},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_Count, -1, 0},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    
    };

static FragShaderDesc ShaderDesc_FS_FYUVMul = {
    /* Type */          FragShaderDesc::FS_FYUVMul,
    /* Flags */         0,
    /* pSource */       pSource_FYUVMul,
    /* Uniforms */      { 
                          /* cxadd*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      { -1, 0, 0, 0, 0, 0 },
                          /* mvp*/        { -1, 0, 0, 0, 0, 0 },
                          /* offset*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor2*/    { -1, 0, 0, 0, 0, 0 },
                          /* srctex*/     { -1, 0, 0, 0, 0, 0 },
                          /* srctexscale*/{ -1, 0, 0, 0, 0, 0 },
                          /* tex*/        {0, 1032, 1, 3, 1, 0 },
                          /* texgen*/     { -1, 0, 0, 0, 0, 0 },
                          /* texscale*/   { -1, 0, 0, 0, 0, 0 },
                          /* vfmuniforms*/{ -1, 0, 0, 0, 0, 0 },
                          /* vfuniforms*/ { -1, 0, 0, 0, 0, 0 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_Count, -1, 0},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_Count, -1, 0},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    
    };

static FragShaderDesc ShaderDesc_FS_FYUVEAlpha = {
    /* Type */          FragShaderDesc::FS_FYUVEAlpha,
    /* Flags */         0,
    /* pSource */       pSource_FYUVEAlpha,
    /* Uniforms */      { 
                          /* cxadd*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      { -1, 0, 0, 0, 0, 0 },
                          /* mvp*/        { -1, 0, 0, 0, 0, 0 },
                          /* offset*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor2*/    { -1, 0, 0, 0, 0, 0 },
                          /* srctex*/     { -1, 0, 0, 0, 0, 0 },
                          /* srctexscale*/{ -1, 0, 0, 0, 0, 0 },
                          /* tex*/        {0, 1032, 1, 3, 1, 0 },
                          /* texgen*/     { -1, 0, 0, 0, 0, 0 },
                          /* texscale*/   { -1, 0, 0, 0, 0, 0 },
                          /* vfmuniforms*/{ -1, 0, 0, 0, 0, 0 },
                          /* vfuniforms*/ { -1, 0, 0, 0, 0, 0 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_Count, -1, 0},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_Count, -1, 0},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    
    };

static FragShaderDesc ShaderDesc_FS_FYUVEAlphaMul = {
    /* Type */          FragShaderDesc::FS_FYUVEAlphaMul,
    /* Flags */         0,
    /* pSource */       pSource_FYUVEAlphaMul,
    /* Uniforms */      { 
                          /* cxadd*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      { -1, 0, 0, 0, 0, 0 },
                          /* mvp*/        { -1, 0, 0, 0, 0, 0 },
                          /* offset*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor2*/    { -1, 0, 0, 0, 0, 0 },
                          /* srctex*/     { -1, 0, 0, 0, 0, 0 },
                          /* srctexscale*/{ -1, 0, 0, 0, 0, 0 },
                          /* tex*/        {0, 1032, 1, 3, 1, 0 },
                          /* texgen*/     { -1, 0, 0, 0, 0, 0 },
                          /* texscale*/   { -1, 0, 0, 0, 0, 0 },
                          /* vfmuniforms*/{ -1, 0, 0, 0, 0, 0 },
                          /* vfuniforms*/ { -1, 0, 0, 0, 0, 0 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_Count, -1, 0},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_Count, -1, 0},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    
    };

static FragShaderDesc ShaderDesc_FS_FYUVCxform = {
    /* Type */          FragShaderDesc::FS_FYUVCxform,
    /* Flags */         0,
    /* pSource */       pSource_FYUVCxform,
    /* Uniforms */      { 
                          /* cxadd*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      { -1, 0, 0, 0, 0, 0 },
                          /* mvp*/        { -1, 0, 0, 0, 0, 0 },
                          /* offset*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor2*/    { -1, 0, 0, 0, 0, 0 },
                          /* srctex*/     { -1, 0, 0, 0, 0, 0 },
                          /* srctexscale*/{ -1, 0, 0, 0, 0, 0 },
                          /* tex*/        {0, 1032, 1, 3, 1, 0 },
                          /* texgen*/     { -1, 0, 0, 0, 0, 0 },
                          /* texscale*/   { -1, 0, 0, 0, 0, 0 },
                          /* vfmuniforms*/{ -1, 0, 0, 0, 0, 0 },
                          /* vfuniforms*/ { -1, 0, 0, 0, 0, 0 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_Count, -1, 0},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_Count, -1, 0},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    
    };

static FragShaderDesc ShaderDesc_FS_FYUVCxformMul = {
    /* Type */          FragShaderDesc::FS_FYUVCxformMul,
    /* Flags */         0,
    /* pSource */       pSource_FYUVCxformMul,
    /* Uniforms */      { 
                          /* cxadd*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      { -1, 0, 0, 0, 0, 0 },
                          /* mvp*/        { -1, 0, 0, 0, 0, 0 },
                          /* offset*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor2*/    { -1, 0, 0, 0, 0, 0 },
                          /* srctex*/     { -1, 0, 0, 0, 0, 0 },
                          /* srctexscale*/{ -1, 0, 0, 0, 0, 0 },
                          /* tex*/        {0, 1032, 1, 3, 1, 0 },
                          /* texgen*/     { -1, 0, 0, 0, 0, 0 },
                          /* texscale*/   { -1, 0, 0, 0, 0, 0 },
                          /* vfmuniforms*/{ -1, 0, 0, 0, 0, 0 },
                          /* vfuniforms*/ { -1, 0, 0, 0, 0, 0 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_Count, -1, 0},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_Count, -1, 0},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    
    };

static FragShaderDesc ShaderDesc_FS_FYUVCxformEAlpha = {
    /* Type */          FragShaderDesc::FS_FYUVCxformEAlpha,
    /* Flags */         0,
    /* pSource */       pSource_FYUVCxformEAlpha,
    /* Uniforms */      { 
                          /* cxadd*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      { -1, 0, 0, 0, 0, 0 },
                          /* mvp*/        { -1, 0, 0, 0, 0, 0 },
                          /* offset*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor2*/    { -1, 0, 0, 0, 0, 0 },
                          /* srctex*/     { -1, 0, 0, 0, 0, 0 },
                          /* srctexscale*/{ -1, 0, 0, 0, 0, 0 },
                          /* tex*/        {0, 1032, 1, 3, 1, 0 },
                          /* texgen*/     { -1, 0, 0, 0, 0, 0 },
                          /* texscale*/   { -1, 0, 0, 0, 0, 0 },
                          /* vfmuniforms*/{ -1, 0, 0, 0, 0, 0 },
                          /* vfuniforms*/ { -1, 0, 0, 0, 0, 0 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_Count, -1, 0},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_Count, -1, 0},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    
    };

static FragShaderDesc ShaderDesc_FS_FYUVCxformEAlphaMul = {
    /* Type */          FragShaderDesc::FS_FYUVCxformEAlphaMul,
    /* Flags */         0,
    /* pSource */       pSource_FYUVCxformEAlphaMul,
    /* Uniforms */      { 
                          /* cxadd*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      { -1, 0, 0, 0, 0, 0 },
                          /* mvp*/        { -1, 0, 0, 0, 0, 0 },
                          /* offset*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor2*/    { -1, 0, 0, 0, 0, 0 },
                          /* srctex*/     { -1, 0, 0, 0, 0, 0 },
                          /* srctexscale*/{ -1, 0, 0, 0, 0, 0 },
                          /* tex*/        {0, 1032, 1, 3, 1, 0 },
                          /* texgen*/     { -1, 0, 0, 0, 0, 0 },
                          /* texscale*/   { -1, 0, 0, 0, 0, 0 },
                          /* vfmuniforms*/{ -1, 0, 0, 0, 0, 0 },
                          /* vfuniforms*/ { -1, 0, 0, 0, 0, 0 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_Count, -1, 0},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_Count, -1, 0},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    
    };

static FragShaderDesc ShaderDesc_FS_FYUVA = {
    /* Type */          FragShaderDesc::FS_FYUVA,
    /* Flags */         0,
    /* pSource */       pSource_FYUVA,
    /* Uniforms */      { 
                          /* cxadd*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      { -1, 0, 0, 0, 0, 0 },
                          /* mvp*/        { -1, 0, 0, 0, 0, 0 },
                          /* offset*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor2*/    { -1, 0, 0, 0, 0, 0 },
                          /* srctex*/     { -1, 0, 0, 0, 0, 0 },
                          /* srctexscale*/{ -1, 0, 0, 0, 0, 0 },
                          /* tex*/        {0, 1032, 1, 4, 1, 0 },
                          /* texgen*/     { -1, 0, 0, 0, 0, 0 },
                          /* texscale*/   { -1, 0, 0, 0, 0, 0 },
                          /* vfmuniforms*/{ -1, 0, 0, 0, 0, 0 },
                          /* vfuniforms*/ { -1, 0, 0, 0, 0, 0 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_Count, -1, 0},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_Count, -1, 0},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    
    };

static FragShaderDesc ShaderDesc_FS_FYUVAMul = {
    /* Type */          FragShaderDesc::FS_FYUVAMul,
    /* Flags */         0,
    /* pSource */       pSource_FYUVAMul,
    /* Uniforms */      { 
                          /* cxadd*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      { -1, 0, 0, 0, 0, 0 },
                          /* mvp*/        { -1, 0, 0, 0, 0, 0 },
                          /* offset*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor2*/    { -1, 0, 0, 0, 0, 0 },
                          /* srctex*/     { -1, 0, 0, 0, 0, 0 },
                          /* srctexscale*/{ -1, 0, 0, 0, 0, 0 },
                          /* tex*/        {0, 1032, 1, 4, 1, 0 },
                          /* texgen*/     { -1, 0, 0, 0, 0, 0 },
                          /* texscale*/   { -1, 0, 0, 0, 0, 0 },
                          /* vfmuniforms*/{ -1, 0, 0, 0, 0, 0 },
                          /* vfuniforms*/ { -1, 0, 0, 0, 0, 0 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_Count, -1, 0},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_Count, -1, 0},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    
    };

static FragShaderDesc ShaderDesc_FS_FYUVAEAlpha = {
    /* Type */          FragShaderDesc::FS_FYUVAEAlpha,
    /* Flags */         0,
    /* pSource */       pSource_FYUVAEAlpha,
    /* Uniforms */      { 
                          /* cxadd*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      { -1, 0, 0, 0, 0, 0 },
                          /* mvp*/        { -1, 0, 0, 0, 0, 0 },
                          /* offset*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor2*/    { -1, 0, 0, 0, 0, 0 },
                          /* srctex*/     { -1, 0, 0, 0, 0, 0 },
                          /* srctexscale*/{ -1, 0, 0, 0, 0, 0 },
                          /* tex*/        {0, 1032, 1, 4, 1, 0 },
                          /* texgen*/     { -1, 0, 0, 0, 0, 0 },
                          /* texscale*/   { -1, 0, 0, 0, 0, 0 },
                          /* vfmuniforms*/{ -1, 0, 0, 0, 0, 0 },
                          /* vfuniforms*/ { -1, 0, 0, 0, 0, 0 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_Count, -1, 0},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_Count, -1, 0},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    
    };

static FragShaderDesc ShaderDesc_FS_FYUVAEAlphaMul = {
    /* Type */          FragShaderDesc::FS_FYUVAEAlphaMul,
    /* Flags */         0,
    /* pSource */       pSource_FYUVAEAlphaMul,
    /* Uniforms */      { 
                          /* cxadd*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      { -1, 0, 0, 0, 0, 0 },
                          /* mvp*/        { -1, 0, 0, 0, 0, 0 },
                          /* offset*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor2*/    { -1, 0, 0, 0, 0, 0 },
                          /* srctex*/     { -1, 0, 0, 0, 0, 0 },
                          /* srctexscale*/{ -1, 0, 0, 0, 0, 0 },
                          /* tex*/        {0, 1032, 1, 4, 1, 0 },
                          /* texgen*/     { -1, 0, 0, 0, 0, 0 },
                          /* texscale*/   { -1, 0, 0, 0, 0, 0 },
                          /* vfmuniforms*/{ -1, 0, 0, 0, 0, 0 },
                          /* vfuniforms*/ { -1, 0, 0, 0, 0, 0 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_Count, -1, 0},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_Count, -1, 0},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    
    };

static FragShaderDesc ShaderDesc_FS_FYUVACxform = {
    /* Type */          FragShaderDesc::FS_FYUVACxform,
    /* Flags */         0,
    /* pSource */       pSource_FYUVACxform,
    /* Uniforms */      { 
                          /* cxadd*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      { -1, 0, 0, 0, 0, 0 },
                          /* mvp*/        { -1, 0, 0, 0, 0, 0 },
                          /* offset*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor2*/    { -1, 0, 0, 0, 0, 0 },
                          /* srctex*/     { -1, 0, 0, 0, 0, 0 },
                          /* srctexscale*/{ -1, 0, 0, 0, 0, 0 },
                          /* tex*/        {0, 1032, 1, 4, 1, 0 },
                          /* texgen*/     { -1, 0, 0, 0, 0, 0 },
                          /* texscale*/   { -1, 0, 0, 0, 0, 0 },
                          /* vfmuniforms*/{ -1, 0, 0, 0, 0, 0 },
                          /* vfuniforms*/ { -1, 0, 0, 0, 0, 0 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_Count, -1, 0},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_Count, -1, 0},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    
    };

static FragShaderDesc ShaderDesc_FS_FYUVACxformMul = {
    /* Type */          FragShaderDesc::FS_FYUVACxformMul,
    /* Flags */         0,
    /* pSource */       pSource_FYUVACxformMul,
    /* Uniforms */      { 
                          /* cxadd*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      { -1, 0, 0, 0, 0, 0 },
                          /* mvp*/        { -1, 0, 0, 0, 0, 0 },
                          /* offset*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor2*/    { -1, 0, 0, 0, 0, 0 },
                          /* srctex*/     { -1, 0, 0, 0, 0, 0 },
                          /* srctexscale*/{ -1, 0, 0, 0, 0, 0 },
                          /* tex*/        {0, 1032, 1, 4, 1, 0 },
                          /* texgen*/     { -1, 0, 0, 0, 0, 0 },
                          /* texscale*/   { -1, 0, 0, 0, 0, 0 },
                          /* vfmuniforms*/{ -1, 0, 0, 0, 0, 0 },
                          /* vfuniforms*/ { -1, 0, 0, 0, 0, 0 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_Count, -1, 0},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_Count, -1, 0},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    
    };

static FragShaderDesc ShaderDesc_FS_FYUVACxformEAlpha = {
    /* Type */          FragShaderDesc::FS_FYUVACxformEAlpha,
    /* Flags */         0,
    /* pSource */       pSource_FYUVACxformEAlpha,
    /* Uniforms */      { 
                          /* cxadd*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      { -1, 0, 0, 0, 0, 0 },
                          /* mvp*/        { -1, 0, 0, 0, 0, 0 },
                          /* offset*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor2*/    { -1, 0, 0, 0, 0, 0 },
                          /* srctex*/     { -1, 0, 0, 0, 0, 0 },
                          /* srctexscale*/{ -1, 0, 0, 0, 0, 0 },
                          /* tex*/        {0, 1032, 1, 4, 1, 0 },
                          /* texgen*/     { -1, 0, 0, 0, 0, 0 },
                          /* texscale*/   { -1, 0, 0, 0, 0, 0 },
                          /* vfmuniforms*/{ -1, 0, 0, 0, 0, 0 },
                          /* vfuniforms*/ { -1, 0, 0, 0, 0, 0 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_Count, -1, 0},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_Count, -1, 0},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    
    };

static FragShaderDesc ShaderDesc_FS_FYUVACxformEAlphaMul = {
    /* Type */          FragShaderDesc::FS_FYUVACxformEAlphaMul,
    /* Flags */         0,
    /* pSource */       pSource_FYUVACxformEAlphaMul,
    /* Uniforms */      { 
                          /* cxadd*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      { -1, 0, 0, 0, 0, 0 },
                          /* mvp*/        { -1, 0, 0, 0, 0, 0 },
                          /* offset*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor2*/    { -1, 0, 0, 0, 0, 0 },
                          /* srctex*/     { -1, 0, 0, 0, 0, 0 },
                          /* srctexscale*/{ -1, 0, 0, 0, 0, 0 },
                          /* tex*/        {0, 1032, 1, 4, 1, 0 },
                          /* texgen*/     { -1, 0, 0, 0, 0, 0 },
                          /* texscale*/   { -1, 0, 0, 0, 0, 0 },
                          /* vfmuniforms*/{ -1, 0, 0, 0, 0, 0 },
                          /* vfuniforms*/ { -1, 0, 0, 0, 0, 0 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_Count, -1, 0},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_Count, -1, 0},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    
    };

static FragShaderDesc ShaderDesc_FS_FBatchYUV = {
    /* Type */          FragShaderDesc::FS_FBatchYUV,
    /* Flags */         Shader_Batch,
    /* pSource */       pSource_FYUV,
    /* Uniforms */      { 
                          /* cxadd*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      { -1, 0, 0, 0, 0, 0 },
                          /* mvp*/        { -1, 0, 0, 0, 0, 0 },
                          /* offset*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor2*/    { -1, 0, 0, 0, 0, 0 },
                          /* srctex*/     { -1, 0, 0, 0, 0, 0 },
                          /* srctexscale*/{ -1, 0, 0, 0, 0, 0 },
                          /* tex*/        {0, 1032, 1, 3, 1, 0 },
                          /* texgen*/     { -1, 0, 0, 0, 0, 0 },
                          /* texscale*/   { -1, 0, 0, 0, 0, 0 },
                          /* vfmuniforms*/{ -1, 0, 0, 0, 0, 0 },
                          /* vfuniforms*/ { -1, 0, 0, 0, 0, 0 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_Count, -1, 0},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_Count, -1, 0},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    
    };

static FragShaderDesc ShaderDesc_FS_FBatchYUVMul = {
    /* Type */          FragShaderDesc::FS_FBatchYUVMul,
    /* Flags */         Shader_Batch,
    /* pSource */       pSource_FYUVMul,
    /* Uniforms */      { 
                          /* cxadd*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      { -1, 0, 0, 0, 0, 0 },
                          /* mvp*/        { -1, 0, 0, 0, 0, 0 },
                          /* offset*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor2*/    { -1, 0, 0, 0, 0, 0 },
                          /* srctex*/     { -1, 0, 0, 0, 0, 0 },
                          /* srctexscale*/{ -1, 0, 0, 0, 0, 0 },
                          /* tex*/        {0, 1032, 1, 3, 1, 0 },
                          /* texgen*/     { -1, 0, 0, 0, 0, 0 },
                          /* texscale*/   { -1, 0, 0, 0, 0, 0 },
                          /* vfmuniforms*/{ -1, 0, 0, 0, 0, 0 },
                          /* vfuniforms*/ { -1, 0, 0, 0, 0, 0 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_Count, -1, 0},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_Count, -1, 0},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    
    };

static FragShaderDesc ShaderDesc_FS_FBatchYUVEAlpha = {
    /* Type */          FragShaderDesc::FS_FBatchYUVEAlpha,
    /* Flags */         Shader_Batch,
    /* pSource */       pSource_FYUVEAlpha,
    /* Uniforms */      { 
                          /* cxadd*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      { -1, 0, 0, 0, 0, 0 },
                          /* mvp*/        { -1, 0, 0, 0, 0, 0 },
                          /* offset*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor2*/    { -1, 0, 0, 0, 0, 0 },
                          /* srctex*/     { -1, 0, 0, 0, 0, 0 },
                          /* srctexscale*/{ -1, 0, 0, 0, 0, 0 },
                          /* tex*/        {0, 1032, 1, 3, 1, 0 },
                          /* texgen*/     { -1, 0, 0, 0, 0, 0 },
                          /* texscale*/   { -1, 0, 0, 0, 0, 0 },
                          /* vfmuniforms*/{ -1, 0, 0, 0, 0, 0 },
                          /* vfuniforms*/ { -1, 0, 0, 0, 0, 0 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_Count, -1, 0},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_Count, -1, 0},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    
    };

static FragShaderDesc ShaderDesc_FS_FBatchYUVEAlphaMul = {
    /* Type */          FragShaderDesc::FS_FBatchYUVEAlphaMul,
    /* Flags */         Shader_Batch,
    /* pSource */       pSource_FYUVEAlphaMul,
    /* Uniforms */      { 
                          /* cxadd*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      { -1, 0, 0, 0, 0, 0 },
                          /* mvp*/        { -1, 0, 0, 0, 0, 0 },
                          /* offset*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor2*/    { -1, 0, 0, 0, 0, 0 },
                          /* srctex*/     { -1, 0, 0, 0, 0, 0 },
                          /* srctexscale*/{ -1, 0, 0, 0, 0, 0 },
                          /* tex*/        {0, 1032, 1, 3, 1, 0 },
                          /* texgen*/     { -1, 0, 0, 0, 0, 0 },
                          /* texscale*/   { -1, 0, 0, 0, 0, 0 },
                          /* vfmuniforms*/{ -1, 0, 0, 0, 0, 0 },
                          /* vfuniforms*/ { -1, 0, 0, 0, 0, 0 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_Count, -1, 0},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_Count, -1, 0},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    
    };

static FragShaderDesc ShaderDesc_FS_FBatchYUVCxform = {
    /* Type */          FragShaderDesc::FS_FBatchYUVCxform,
    /* Flags */         Shader_Batch,
    /* pSource */       pSource_FYUVCxform,
    /* Uniforms */      { 
                          /* cxadd*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      { -1, 0, 0, 0, 0, 0 },
                          /* mvp*/        { -1, 0, 0, 0, 0, 0 },
                          /* offset*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor2*/    { -1, 0, 0, 0, 0, 0 },
                          /* srctex*/     { -1, 0, 0, 0, 0, 0 },
                          /* srctexscale*/{ -1, 0, 0, 0, 0, 0 },
                          /* tex*/        {0, 1032, 1, 3, 1, 0 },
                          /* texgen*/     { -1, 0, 0, 0, 0, 0 },
                          /* texscale*/   { -1, 0, 0, 0, 0, 0 },
                          /* vfmuniforms*/{ -1, 0, 0, 0, 0, 0 },
                          /* vfuniforms*/ { -1, 0, 0, 0, 0, 0 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_Count, -1, 0},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_Count, -1, 0},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    
    };

static FragShaderDesc ShaderDesc_FS_FBatchYUVCxformMul = {
    /* Type */          FragShaderDesc::FS_FBatchYUVCxformMul,
    /* Flags */         Shader_Batch,
    /* pSource */       pSource_FYUVCxformMul,
    /* Uniforms */      { 
                          /* cxadd*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      { -1, 0, 0, 0, 0, 0 },
                          /* mvp*/        { -1, 0, 0, 0, 0, 0 },
                          /* offset*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor2*/    { -1, 0, 0, 0, 0, 0 },
                          /* srctex*/     { -1, 0, 0, 0, 0, 0 },
                          /* srctexscale*/{ -1, 0, 0, 0, 0, 0 },
                          /* tex*/        {0, 1032, 1, 3, 1, 0 },
                          /* texgen*/     { -1, 0, 0, 0, 0, 0 },
                          /* texscale*/   { -1, 0, 0, 0, 0, 0 },
                          /* vfmuniforms*/{ -1, 0, 0, 0, 0, 0 },
                          /* vfuniforms*/ { -1, 0, 0, 0, 0, 0 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_Count, -1, 0},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_Count, -1, 0},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    
    };

static FragShaderDesc ShaderDesc_FS_FBatchYUVCxformEAlpha = {
    /* Type */          FragShaderDesc::FS_FBatchYUVCxformEAlpha,
    /* Flags */         Shader_Batch,
    /* pSource */       pSource_FYUVCxformEAlpha,
    /* Uniforms */      { 
                          /* cxadd*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      { -1, 0, 0, 0, 0, 0 },
                          /* mvp*/        { -1, 0, 0, 0, 0, 0 },
                          /* offset*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor2*/    { -1, 0, 0, 0, 0, 0 },
                          /* srctex*/     { -1, 0, 0, 0, 0, 0 },
                          /* srctexscale*/{ -1, 0, 0, 0, 0, 0 },
                          /* tex*/        {0, 1032, 1, 3, 1, 0 },
                          /* texgen*/     { -1, 0, 0, 0, 0, 0 },
                          /* texscale*/   { -1, 0, 0, 0, 0, 0 },
                          /* vfmuniforms*/{ -1, 0, 0, 0, 0, 0 },
                          /* vfuniforms*/ { -1, 0, 0, 0, 0, 0 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_Count, -1, 0},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_Count, -1, 0},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    
    };

static FragShaderDesc ShaderDesc_FS_FBatchYUVCxformEAlphaMul = {
    /* Type */          FragShaderDesc::FS_FBatchYUVCxformEAlphaMul,
    /* Flags */         Shader_Batch,
    /* pSource */       pSource_FYUVCxformEAlphaMul,
    /* Uniforms */      { 
                          /* cxadd*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      { -1, 0, 0, 0, 0, 0 },
                          /* mvp*/        { -1, 0, 0, 0, 0, 0 },
                          /* offset*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor2*/    { -1, 0, 0, 0, 0, 0 },
                          /* srctex*/     { -1, 0, 0, 0, 0, 0 },
                          /* srctexscale*/{ -1, 0, 0, 0, 0, 0 },
                          /* tex*/        {0, 1032, 1, 3, 1, 0 },
                          /* texgen*/     { -1, 0, 0, 0, 0, 0 },
                          /* texscale*/   { -1, 0, 0, 0, 0, 0 },
                          /* vfmuniforms*/{ -1, 0, 0, 0, 0, 0 },
                          /* vfuniforms*/ { -1, 0, 0, 0, 0, 0 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_Count, -1, 0},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_Count, -1, 0},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    
    };

static FragShaderDesc ShaderDesc_FS_FBatchYUVA = {
    /* Type */          FragShaderDesc::FS_FBatchYUVA,
    /* Flags */         Shader_Batch,
    /* pSource */       pSource_FYUVA,
    /* Uniforms */      { 
                          /* cxadd*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      { -1, 0, 0, 0, 0, 0 },
                          /* mvp*/        { -1, 0, 0, 0, 0, 0 },
                          /* offset*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor2*/    { -1, 0, 0, 0, 0, 0 },
                          /* srctex*/     { -1, 0, 0, 0, 0, 0 },
                          /* srctexscale*/{ -1, 0, 0, 0, 0, 0 },
                          /* tex*/        {0, 1032, 1, 4, 1, 0 },
                          /* texgen*/     { -1, 0, 0, 0, 0, 0 },
                          /* texscale*/   { -1, 0, 0, 0, 0, 0 },
                          /* vfmuniforms*/{ -1, 0, 0, 0, 0, 0 },
                          /* vfuniforms*/ { -1, 0, 0, 0, 0, 0 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_Count, -1, 0},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_Count, -1, 0},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    
    };

static FragShaderDesc ShaderDesc_FS_FBatchYUVAMul = {
    /* Type */          FragShaderDesc::FS_FBatchYUVAMul,
    /* Flags */         Shader_Batch,
    /* pSource */       pSource_FYUVAMul,
    /* Uniforms */      { 
                          /* cxadd*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      { -1, 0, 0, 0, 0, 0 },
                          /* mvp*/        { -1, 0, 0, 0, 0, 0 },
                          /* offset*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor2*/    { -1, 0, 0, 0, 0, 0 },
                          /* srctex*/     { -1, 0, 0, 0, 0, 0 },
                          /* srctexscale*/{ -1, 0, 0, 0, 0, 0 },
                          /* tex*/        {0, 1032, 1, 4, 1, 0 },
                          /* texgen*/     { -1, 0, 0, 0, 0, 0 },
                          /* texscale*/   { -1, 0, 0, 0, 0, 0 },
                          /* vfmuniforms*/{ -1, 0, 0, 0, 0, 0 },
                          /* vfuniforms*/ { -1, 0, 0, 0, 0, 0 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_Count, -1, 0},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_Count, -1, 0},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    
    };

static FragShaderDesc ShaderDesc_FS_FBatchYUVAEAlpha = {
    /* Type */          FragShaderDesc::FS_FBatchYUVAEAlpha,
    /* Flags */         Shader_Batch,
    /* pSource */       pSource_FYUVAEAlpha,
    /* Uniforms */      { 
                          /* cxadd*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      { -1, 0, 0, 0, 0, 0 },
                          /* mvp*/        { -1, 0, 0, 0, 0, 0 },
                          /* offset*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor2*/    { -1, 0, 0, 0, 0, 0 },
                          /* srctex*/     { -1, 0, 0, 0, 0, 0 },
                          /* srctexscale*/{ -1, 0, 0, 0, 0, 0 },
                          /* tex*/        {0, 1032, 1, 4, 1, 0 },
                          /* texgen*/     { -1, 0, 0, 0, 0, 0 },
                          /* texscale*/   { -1, 0, 0, 0, 0, 0 },
                          /* vfmuniforms*/{ -1, 0, 0, 0, 0, 0 },
                          /* vfuniforms*/ { -1, 0, 0, 0, 0, 0 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_Count, -1, 0},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_Count, -1, 0},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    
    };

static FragShaderDesc ShaderDesc_FS_FBatchYUVAEAlphaMul = {
    /* Type */          FragShaderDesc::FS_FBatchYUVAEAlphaMul,
    /* Flags */         Shader_Batch,
    /* pSource */       pSource_FYUVAEAlphaMul,
    /* Uniforms */      { 
                          /* cxadd*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      { -1, 0, 0, 0, 0, 0 },
                          /* mvp*/        { -1, 0, 0, 0, 0, 0 },
                          /* offset*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor2*/    { -1, 0, 0, 0, 0, 0 },
                          /* srctex*/     { -1, 0, 0, 0, 0, 0 },
                          /* srctexscale*/{ -1, 0, 0, 0, 0, 0 },
                          /* tex*/        {0, 1032, 1, 4, 1, 0 },
                          /* texgen*/     { -1, 0, 0, 0, 0, 0 },
                          /* texscale*/   { -1, 0, 0, 0, 0, 0 },
                          /* vfmuniforms*/{ -1, 0, 0, 0, 0, 0 },
                          /* vfuniforms*/ { -1, 0, 0, 0, 0, 0 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_Count, -1, 0},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_Count, -1, 0},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    
    };

static FragShaderDesc ShaderDesc_FS_FBatchYUVACxform = {
    /* Type */          FragShaderDesc::FS_FBatchYUVACxform,
    /* Flags */         Shader_Batch,
    /* pSource */       pSource_FYUVACxform,
    /* Uniforms */      { 
                          /* cxadd*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      { -1, 0, 0, 0, 0, 0 },
                          /* mvp*/        { -1, 0, 0, 0, 0, 0 },
                          /* offset*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor2*/    { -1, 0, 0, 0, 0, 0 },
                          /* srctex*/     { -1, 0, 0, 0, 0, 0 },
                          /* srctexscale*/{ -1, 0, 0, 0, 0, 0 },
                          /* tex*/        {0, 1032, 1, 4, 1, 0 },
                          /* texgen*/     { -1, 0, 0, 0, 0, 0 },
                          /* texscale*/   { -1, 0, 0, 0, 0, 0 },
                          /* vfmuniforms*/{ -1, 0, 0, 0, 0, 0 },
                          /* vfuniforms*/ { -1, 0, 0, 0, 0, 0 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_Count, -1, 0},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_Count, -1, 0},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    
    };

static FragShaderDesc ShaderDesc_FS_FBatchYUVACxformMul = {
    /* Type */          FragShaderDesc::FS_FBatchYUVACxformMul,
    /* Flags */         Shader_Batch,
    /* pSource */       pSource_FYUVACxformMul,
    /* Uniforms */      { 
                          /* cxadd*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      { -1, 0, 0, 0, 0, 0 },
                          /* mvp*/        { -1, 0, 0, 0, 0, 0 },
                          /* offset*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor2*/    { -1, 0, 0, 0, 0, 0 },
                          /* srctex*/     { -1, 0, 0, 0, 0, 0 },
                          /* srctexscale*/{ -1, 0, 0, 0, 0, 0 },
                          /* tex*/        {0, 1032, 1, 4, 1, 0 },
                          /* texgen*/     { -1, 0, 0, 0, 0, 0 },
                          /* texscale*/   { -1, 0, 0, 0, 0, 0 },
                          /* vfmuniforms*/{ -1, 0, 0, 0, 0, 0 },
                          /* vfuniforms*/ { -1, 0, 0, 0, 0, 0 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_Count, -1, 0},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_Count, -1, 0},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    
    };

static FragShaderDesc ShaderDesc_FS_FBatchYUVACxformEAlpha = {
    /* Type */          FragShaderDesc::FS_FBatchYUVACxformEAlpha,
    /* Flags */         Shader_Batch,
    /* pSource */       pSource_FYUVACxformEAlpha,
    /* Uniforms */      { 
                          /* cxadd*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      { -1, 0, 0, 0, 0, 0 },
                          /* mvp*/        { -1, 0, 0, 0, 0, 0 },
                          /* offset*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor2*/    { -1, 0, 0, 0, 0, 0 },
                          /* srctex*/     { -1, 0, 0, 0, 0, 0 },
                          /* srctexscale*/{ -1, 0, 0, 0, 0, 0 },
                          /* tex*/        {0, 1032, 1, 4, 1, 0 },
                          /* texgen*/     { -1, 0, 0, 0, 0, 0 },
                          /* texscale*/   { -1, 0, 0, 0, 0, 0 },
                          /* vfmuniforms*/{ -1, 0, 0, 0, 0, 0 },
                          /* vfuniforms*/ { -1, 0, 0, 0, 0, 0 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_Count, -1, 0},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_Count, -1, 0},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    
    };

static FragShaderDesc ShaderDesc_FS_FBatchYUVACxformEAlphaMul = {
    /* Type */          FragShaderDesc::FS_FBatchYUVACxformEAlphaMul,
    /* Flags */         Shader_Batch,
    /* pSource */       pSource_FYUVACxformEAlphaMul,
    /* Uniforms */      { 
                          /* cxadd*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      { -1, 0, 0, 0, 0, 0 },
                          /* mvp*/        { -1, 0, 0, 0, 0, 0 },
                          /* offset*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor2*/    { -1, 0, 0, 0, 0, 0 },
                          /* srctex*/     { -1, 0, 0, 0, 0, 0 },
                          /* srctexscale*/{ -1, 0, 0, 0, 0, 0 },
                          /* tex*/        {0, 1032, 1, 4, 1, 0 },
                          /* texgen*/     { -1, 0, 0, 0, 0, 0 },
                          /* texscale*/   { -1, 0, 0, 0, 0, 0 },
                          /* vfmuniforms*/{ -1, 0, 0, 0, 0, 0 },
                          /* vfuniforms*/ { -1, 0, 0, 0, 0, 0 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_Count, -1, 0},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_Count, -1, 0},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    
    };

static FragShaderDesc ShaderDesc_FS_FInstancedYUV = {
    /* Type */          FragShaderDesc::FS_FInstancedYUV,
    /* Flags */         Shader_Instanced,
    /* pSource */       pSource_FInstancedYUV,
    /* Uniforms */      { 
                          /* cxadd*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      { -1, 0, 0, 0, 0, 0 },
                          /* mvp*/        { -1, 0, 0, 0, 0, 0 },
                          /* offset*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor2*/    { -1, 0, 0, 0, 0, 0 },
                          /* srctex*/     { -1, 0, 0, 0, 0, 0 },
                          /* srctexscale*/{ -1, 0, 0, 0, 0, 0 },
                          /* tex*/        {0, 1032, 1, 3, 1, 0 },
                          /* texgen*/     { -1, 0, 0, 0, 0, 0 },
                          /* texscale*/   { -1, 0, 0, 0, 0, 0 },
                          /* vfmuniforms*/{ -1, 0, 0, 0, 0, 0 },
                          /* vfuniforms*/ { -1, 0, 0, 0, 0, 0 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_Count, -1, 0},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_Count, -1, 0},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    
    };

static FragShaderDesc ShaderDesc_FS_FInstancedYUVMul = {
    /* Type */          FragShaderDesc::FS_FInstancedYUVMul,
    /* Flags */         Shader_Instanced,
    /* pSource */       pSource_FInstancedYUVMul,
    /* Uniforms */      { 
                          /* cxadd*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      { -1, 0, 0, 0, 0, 0 },
                          /* mvp*/        { -1, 0, 0, 0, 0, 0 },
                          /* offset*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor2*/    { -1, 0, 0, 0, 0, 0 },
                          /* srctex*/     { -1, 0, 0, 0, 0, 0 },
                          /* srctexscale*/{ -1, 0, 0, 0, 0, 0 },
                          /* tex*/        {0, 1032, 1, 3, 1, 0 },
                          /* texgen*/     { -1, 0, 0, 0, 0, 0 },
                          /* texscale*/   { -1, 0, 0, 0, 0, 0 },
                          /* vfmuniforms*/{ -1, 0, 0, 0, 0, 0 },
                          /* vfuniforms*/ { -1, 0, 0, 0, 0, 0 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_Count, -1, 0},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_Count, -1, 0},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    
    };

static FragShaderDesc ShaderDesc_FS_FInstancedYUVEAlpha = {
    /* Type */          FragShaderDesc::FS_FInstancedYUVEAlpha,
    /* Flags */         Shader_Instanced,
    /* pSource */       pSource_FInstancedYUVEAlpha,
    /* Uniforms */      { 
                          /* cxadd*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      { -1, 0, 0, 0, 0, 0 },
                          /* mvp*/        { -1, 0, 0, 0, 0, 0 },
                          /* offset*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor2*/    { -1, 0, 0, 0, 0, 0 },
                          /* srctex*/     { -1, 0, 0, 0, 0, 0 },
                          /* srctexscale*/{ -1, 0, 0, 0, 0, 0 },
                          /* tex*/        {0, 1032, 1, 3, 1, 0 },
                          /* texgen*/     { -1, 0, 0, 0, 0, 0 },
                          /* texscale*/   { -1, 0, 0, 0, 0, 0 },
                          /* vfmuniforms*/{ -1, 0, 0, 0, 0, 0 },
                          /* vfuniforms*/ { -1, 0, 0, 0, 0, 0 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_Count, -1, 0},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_Count, -1, 0},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    
    };

static FragShaderDesc ShaderDesc_FS_FInstancedYUVEAlphaMul = {
    /* Type */          FragShaderDesc::FS_FInstancedYUVEAlphaMul,
    /* Flags */         Shader_Instanced,
    /* pSource */       pSource_FInstancedYUVEAlphaMul,
    /* Uniforms */      { 
                          /* cxadd*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      { -1, 0, 0, 0, 0, 0 },
                          /* mvp*/        { -1, 0, 0, 0, 0, 0 },
                          /* offset*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor2*/    { -1, 0, 0, 0, 0, 0 },
                          /* srctex*/     { -1, 0, 0, 0, 0, 0 },
                          /* srctexscale*/{ -1, 0, 0, 0, 0, 0 },
                          /* tex*/        {0, 1032, 1, 3, 1, 0 },
                          /* texgen*/     { -1, 0, 0, 0, 0, 0 },
                          /* texscale*/   { -1, 0, 0, 0, 0, 0 },
                          /* vfmuniforms*/{ -1, 0, 0, 0, 0, 0 },
                          /* vfuniforms*/ { -1, 0, 0, 0, 0, 0 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_Count, -1, 0},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_Count, -1, 0},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    
    };

static FragShaderDesc ShaderDesc_FS_FInstancedYUVCxform = {
    /* Type */          FragShaderDesc::FS_FInstancedYUVCxform,
    /* Flags */         Shader_Instanced,
    /* pSource */       pSource_FInstancedYUVCxform,
    /* Uniforms */      { 
                          /* cxadd*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      { -1, 0, 0, 0, 0, 0 },
                          /* mvp*/        { -1, 0, 0, 0, 0, 0 },
                          /* offset*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor2*/    { -1, 0, 0, 0, 0, 0 },
                          /* srctex*/     { -1, 0, 0, 0, 0, 0 },
                          /* srctexscale*/{ -1, 0, 0, 0, 0, 0 },
                          /* tex*/        {0, 1032, 1, 3, 1, 0 },
                          /* texgen*/     { -1, 0, 0, 0, 0, 0 },
                          /* texscale*/   { -1, 0, 0, 0, 0, 0 },
                          /* vfmuniforms*/{ -1, 0, 0, 0, 0, 0 },
                          /* vfuniforms*/ { -1, 0, 0, 0, 0, 0 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_Count, -1, 0},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_Count, -1, 0},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    
    };

static FragShaderDesc ShaderDesc_FS_FInstancedYUVCxformMul = {
    /* Type */          FragShaderDesc::FS_FInstancedYUVCxformMul,
    /* Flags */         Shader_Instanced,
    /* pSource */       pSource_FInstancedYUVCxformMul,
    /* Uniforms */      { 
                          /* cxadd*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      { -1, 0, 0, 0, 0, 0 },
                          /* mvp*/        { -1, 0, 0, 0, 0, 0 },
                          /* offset*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor2*/    { -1, 0, 0, 0, 0, 0 },
                          /* srctex*/     { -1, 0, 0, 0, 0, 0 },
                          /* srctexscale*/{ -1, 0, 0, 0, 0, 0 },
                          /* tex*/        {0, 1032, 1, 3, 1, 0 },
                          /* texgen*/     { -1, 0, 0, 0, 0, 0 },
                          /* texscale*/   { -1, 0, 0, 0, 0, 0 },
                          /* vfmuniforms*/{ -1, 0, 0, 0, 0, 0 },
                          /* vfuniforms*/ { -1, 0, 0, 0, 0, 0 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_Count, -1, 0},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_Count, -1, 0},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    
    };

static FragShaderDesc ShaderDesc_FS_FInstancedYUVCxformEAlpha = {
    /* Type */          FragShaderDesc::FS_FInstancedYUVCxformEAlpha,
    /* Flags */         Shader_Instanced,
    /* pSource */       pSource_FInstancedYUVCxformEAlpha,
    /* Uniforms */      { 
                          /* cxadd*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      { -1, 0, 0, 0, 0, 0 },
                          /* mvp*/        { -1, 0, 0, 0, 0, 0 },
                          /* offset*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor2*/    { -1, 0, 0, 0, 0, 0 },
                          /* srctex*/     { -1, 0, 0, 0, 0, 0 },
                          /* srctexscale*/{ -1, 0, 0, 0, 0, 0 },
                          /* tex*/        {0, 1032, 1, 3, 1, 0 },
                          /* texgen*/     { -1, 0, 0, 0, 0, 0 },
                          /* texscale*/   { -1, 0, 0, 0, 0, 0 },
                          /* vfmuniforms*/{ -1, 0, 0, 0, 0, 0 },
                          /* vfuniforms*/ { -1, 0, 0, 0, 0, 0 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_Count, -1, 0},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_Count, -1, 0},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    
    };

static FragShaderDesc ShaderDesc_FS_FInstancedYUVCxformEAlphaMul = {
    /* Type */          FragShaderDesc::FS_FInstancedYUVCxformEAlphaMul,
    /* Flags */         Shader_Instanced,
    /* pSource */       pSource_FInstancedYUVCxformEAlphaMul,
    /* Uniforms */      { 
                          /* cxadd*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      { -1, 0, 0, 0, 0, 0 },
                          /* mvp*/        { -1, 0, 0, 0, 0, 0 },
                          /* offset*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor2*/    { -1, 0, 0, 0, 0, 0 },
                          /* srctex*/     { -1, 0, 0, 0, 0, 0 },
                          /* srctexscale*/{ -1, 0, 0, 0, 0, 0 },
                          /* tex*/        {0, 1032, 1, 3, 1, 0 },
                          /* texgen*/     { -1, 0, 0, 0, 0, 0 },
                          /* texscale*/   { -1, 0, 0, 0, 0, 0 },
                          /* vfmuniforms*/{ -1, 0, 0, 0, 0, 0 },
                          /* vfuniforms*/ { -1, 0, 0, 0, 0, 0 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_Count, -1, 0},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_Count, -1, 0},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    
    };

static FragShaderDesc ShaderDesc_FS_FInstancedYUVA = {
    /* Type */          FragShaderDesc::FS_FInstancedYUVA,
    /* Flags */         Shader_Instanced,
    /* pSource */       pSource_FInstancedYUVA,
    /* Uniforms */      { 
                          /* cxadd*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      { -1, 0, 0, 0, 0, 0 },
                          /* mvp*/        { -1, 0, 0, 0, 0, 0 },
                          /* offset*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor2*/    { -1, 0, 0, 0, 0, 0 },
                          /* srctex*/     { -1, 0, 0, 0, 0, 0 },
                          /* srctexscale*/{ -1, 0, 0, 0, 0, 0 },
                          /* tex*/        {0, 1032, 1, 4, 1, 0 },
                          /* texgen*/     { -1, 0, 0, 0, 0, 0 },
                          /* texscale*/   { -1, 0, 0, 0, 0, 0 },
                          /* vfmuniforms*/{ -1, 0, 0, 0, 0, 0 },
                          /* vfuniforms*/ { -1, 0, 0, 0, 0, 0 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_Count, -1, 0},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_Count, -1, 0},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    
    };

static FragShaderDesc ShaderDesc_FS_FInstancedYUVAMul = {
    /* Type */          FragShaderDesc::FS_FInstancedYUVAMul,
    /* Flags */         Shader_Instanced,
    /* pSource */       pSource_FInstancedYUVAMul,
    /* Uniforms */      { 
                          /* cxadd*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      { -1, 0, 0, 0, 0, 0 },
                          /* mvp*/        { -1, 0, 0, 0, 0, 0 },
                          /* offset*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor2*/    { -1, 0, 0, 0, 0, 0 },
                          /* srctex*/     { -1, 0, 0, 0, 0, 0 },
                          /* srctexscale*/{ -1, 0, 0, 0, 0, 0 },
                          /* tex*/        {0, 1032, 1, 4, 1, 0 },
                          /* texgen*/     { -1, 0, 0, 0, 0, 0 },
                          /* texscale*/   { -1, 0, 0, 0, 0, 0 },
                          /* vfmuniforms*/{ -1, 0, 0, 0, 0, 0 },
                          /* vfuniforms*/ { -1, 0, 0, 0, 0, 0 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_Count, -1, 0},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_Count, -1, 0},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    
    };

static FragShaderDesc ShaderDesc_FS_FInstancedYUVAEAlpha = {
    /* Type */          FragShaderDesc::FS_FInstancedYUVAEAlpha,
    /* Flags */         Shader_Instanced,
    /* pSource */       pSource_FInstancedYUVAEAlpha,
    /* Uniforms */      { 
                          /* cxadd*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      { -1, 0, 0, 0, 0, 0 },
                          /* mvp*/        { -1, 0, 0, 0, 0, 0 },
                          /* offset*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor2*/    { -1, 0, 0, 0, 0, 0 },
                          /* srctex*/     { -1, 0, 0, 0, 0, 0 },
                          /* srctexscale*/{ -1, 0, 0, 0, 0, 0 },
                          /* tex*/        {0, 1032, 1, 4, 1, 0 },
                          /* texgen*/     { -1, 0, 0, 0, 0, 0 },
                          /* texscale*/   { -1, 0, 0, 0, 0, 0 },
                          /* vfmuniforms*/{ -1, 0, 0, 0, 0, 0 },
                          /* vfuniforms*/ { -1, 0, 0, 0, 0, 0 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_Count, -1, 0},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_Count, -1, 0},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    
    };

static FragShaderDesc ShaderDesc_FS_FInstancedYUVAEAlphaMul = {
    /* Type */          FragShaderDesc::FS_FInstancedYUVAEAlphaMul,
    /* Flags */         Shader_Instanced,
    /* pSource */       pSource_FInstancedYUVAEAlphaMul,
    /* Uniforms */      { 
                          /* cxadd*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      { -1, 0, 0, 0, 0, 0 },
                          /* mvp*/        { -1, 0, 0, 0, 0, 0 },
                          /* offset*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor2*/    { -1, 0, 0, 0, 0, 0 },
                          /* srctex*/     { -1, 0, 0, 0, 0, 0 },
                          /* srctexscale*/{ -1, 0, 0, 0, 0, 0 },
                          /* tex*/        {0, 1032, 1, 4, 1, 0 },
                          /* texgen*/     { -1, 0, 0, 0, 0, 0 },
                          /* texscale*/   { -1, 0, 0, 0, 0, 0 },
                          /* vfmuniforms*/{ -1, 0, 0, 0, 0, 0 },
                          /* vfuniforms*/ { -1, 0, 0, 0, 0, 0 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_Count, -1, 0},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_Count, -1, 0},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    
    };

static FragShaderDesc ShaderDesc_FS_FInstancedYUVACxform = {
    /* Type */          FragShaderDesc::FS_FInstancedYUVACxform,
    /* Flags */         Shader_Instanced,
    /* pSource */       pSource_FInstancedYUVACxform,
    /* Uniforms */      { 
                          /* cxadd*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      { -1, 0, 0, 0, 0, 0 },
                          /* mvp*/        { -1, 0, 0, 0, 0, 0 },
                          /* offset*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor2*/    { -1, 0, 0, 0, 0, 0 },
                          /* srctex*/     { -1, 0, 0, 0, 0, 0 },
                          /* srctexscale*/{ -1, 0, 0, 0, 0, 0 },
                          /* tex*/        {0, 1032, 1, 4, 1, 0 },
                          /* texgen*/     { -1, 0, 0, 0, 0, 0 },
                          /* texscale*/   { -1, 0, 0, 0, 0, 0 },
                          /* vfmuniforms*/{ -1, 0, 0, 0, 0, 0 },
                          /* vfuniforms*/ { -1, 0, 0, 0, 0, 0 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_Count, -1, 0},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_Count, -1, 0},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    
    };

static FragShaderDesc ShaderDesc_FS_FInstancedYUVACxformMul = {
    /* Type */          FragShaderDesc::FS_FInstancedYUVACxformMul,
    /* Flags */         Shader_Instanced,
    /* pSource */       pSource_FInstancedYUVACxformMul,
    /* Uniforms */      { 
                          /* cxadd*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      { -1, 0, 0, 0, 0, 0 },
                          /* mvp*/        { -1, 0, 0, 0, 0, 0 },
                          /* offset*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor2*/    { -1, 0, 0, 0, 0, 0 },
                          /* srctex*/     { -1, 0, 0, 0, 0, 0 },
                          /* srctexscale*/{ -1, 0, 0, 0, 0, 0 },
                          /* tex*/        {0, 1032, 1, 4, 1, 0 },
                          /* texgen*/     { -1, 0, 0, 0, 0, 0 },
                          /* texscale*/   { -1, 0, 0, 0, 0, 0 },
                          /* vfmuniforms*/{ -1, 0, 0, 0, 0, 0 },
                          /* vfuniforms*/ { -1, 0, 0, 0, 0, 0 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_Count, -1, 0},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_Count, -1, 0},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    
    };

static FragShaderDesc ShaderDesc_FS_FInstancedYUVACxformEAlpha = {
    /* Type */          FragShaderDesc::FS_FInstancedYUVACxformEAlpha,
    /* Flags */         Shader_Instanced,
    /* pSource */       pSource_FInstancedYUVACxformEAlpha,
    /* Uniforms */      { 
                          /* cxadd*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      { -1, 0, 0, 0, 0, 0 },
                          /* mvp*/        { -1, 0, 0, 0, 0, 0 },
                          /* offset*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor2*/    { -1, 0, 0, 0, 0, 0 },
                          /* srctex*/     { -1, 0, 0, 0, 0, 0 },
                          /* srctexscale*/{ -1, 0, 0, 0, 0, 0 },
                          /* tex*/        {0, 1032, 1, 4, 1, 0 },
                          /* texgen*/     { -1, 0, 0, 0, 0, 0 },
                          /* texscale*/   { -1, 0, 0, 0, 0, 0 },
                          /* vfmuniforms*/{ -1, 0, 0, 0, 0, 0 },
                          /* vfuniforms*/ { -1, 0, 0, 0, 0, 0 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_Count, -1, 0},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_Count, -1, 0},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    
    };

static FragShaderDesc ShaderDesc_FS_FInstancedYUVACxformEAlphaMul = {
    /* Type */          FragShaderDesc::FS_FInstancedYUVACxformEAlphaMul,
    /* Flags */         Shader_Instanced,
    /* pSource */       pSource_FInstancedYUVACxformEAlphaMul,
    /* Uniforms */      { 
                          /* cxadd*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      { -1, 0, 0, 0, 0, 0 },
                          /* mvp*/        { -1, 0, 0, 0, 0, 0 },
                          /* offset*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor2*/    { -1, 0, 0, 0, 0, 0 },
                          /* srctex*/     { -1, 0, 0, 0, 0, 0 },
                          /* srctexscale*/{ -1, 0, 0, 0, 0, 0 },
                          /* tex*/        {0, 1032, 1, 4, 1, 0 },
                          /* texgen*/     { -1, 0, 0, 0, 0, 0 },
                          /* texscale*/   { -1, 0, 0, 0, 0, 0 },
                          /* vfmuniforms*/{ -1, 0, 0, 0, 0, 0 },
                          /* vfuniforms*/ { -1, 0, 0, 0, 0, 0 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_Count, -1, 0},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_Count, -1, 0},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    
    };

static FragShaderDesc ShaderDesc_FS_FTexTGCMatrixAc = {
    /* Type */          FragShaderDesc::FS_FTexTGCMatrixAc,
    /* Flags */         0,
    /* pSource */       pSource_FTexTGCMatrixAc,
    /* Uniforms */      { 
                          /* cxadd*/      {0, 1032, 4, 4, 4, 0 },
                          /* cxmul*/      {1, 1036, 16, 16, 16, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      { -1, 0, 0, 0, 0, 0 },
                          /* mvp*/        { -1, 0, 0, 0, 0, 0 },
                          /* offset*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor2*/    { -1, 0, 0, 0, 0, 0 },
                          /* srctex*/     { -1, 0, 0, 0, 0, 0 },
                          /* srctexscale*/{ -1, 0, 0, 0, 0, 0 },
                          /* tex*/        {0, 1032, 1, 1, 1, 0 },
                          /* texgen*/     { -1, 0, 0, 0, 0, 0 },
                          /* texscale*/   { -1, 0, 0, 0, 0, 0 },
                          /* vfmuniforms*/{ -1, 0, 0, 0, 0, 0 },
                          /* vfuniforms*/ { -1, 0, 0, 0, 0, 0 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_Count, -1, 0},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_Count, -1, 0},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    
    };

static FragShaderDesc ShaderDesc_FS_FTexTGCMatrixAcMul = {
    /* Type */          FragShaderDesc::FS_FTexTGCMatrixAcMul,
    /* Flags */         0,
    /* pSource */       pSource_FTexTGCMatrixAcMul,
    /* Uniforms */      { 
                          /* cxadd*/      {0, 1032, 4, 4, 4, 0 },
                          /* cxmul*/      {1, 1036, 16, 16, 16, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      { -1, 0, 0, 0, 0, 0 },
                          /* mvp*/        { -1, 0, 0, 0, 0, 0 },
                          /* offset*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor2*/    { -1, 0, 0, 0, 0, 0 },
                          /* srctex*/     { -1, 0, 0, 0, 0, 0 },
                          /* srctexscale*/{ -1, 0, 0, 0, 0, 0 },
                          /* tex*/        {0, 1032, 1, 1, 1, 0 },
                          /* texgen*/     { -1, 0, 0, 0, 0, 0 },
                          /* texscale*/   { -1, 0, 0, 0, 0, 0 },
                          /* vfmuniforms*/{ -1, 0, 0, 0, 0, 0 },
                          /* vfuniforms*/ { -1, 0, 0, 0, 0, 0 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_Count, -1, 0},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_Count, -1, 0},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    
    };

static FragShaderDesc ShaderDesc_FS_FTexTGCMatrixAcEAlpha = {
    /* Type */          FragShaderDesc::FS_FTexTGCMatrixAcEAlpha,
    /* Flags */         0,
    /* pSource */       pSource_FTexTGCMatrixAcEAlpha,
    /* Uniforms */      { 
                          /* cxadd*/      {0, 1032, 4, 4, 4, 0 },
                          /* cxmul*/      {1, 1036, 16, 16, 16, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      { -1, 0, 0, 0, 0, 0 },
                          /* mvp*/        { -1, 0, 0, 0, 0, 0 },
                          /* offset*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor2*/    { -1, 0, 0, 0, 0, 0 },
                          /* srctex*/     { -1, 0, 0, 0, 0, 0 },
                          /* srctexscale*/{ -1, 0, 0, 0, 0, 0 },
                          /* tex*/        {0, 1032, 1, 1, 1, 0 },
                          /* texgen*/     { -1, 0, 0, 0, 0, 0 },
                          /* texscale*/   { -1, 0, 0, 0, 0, 0 },
                          /* vfmuniforms*/{ -1, 0, 0, 0, 0, 0 },
                          /* vfuniforms*/ { -1, 0, 0, 0, 0, 0 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_Count, -1, 0},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_Count, -1, 0},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    
    };

static FragShaderDesc ShaderDesc_FS_FTexTGCMatrixAcEAlphaMul = {
    /* Type */          FragShaderDesc::FS_FTexTGCMatrixAcEAlphaMul,
    /* Flags */         0,
    /* pSource */       pSource_FTexTGCMatrixAcEAlphaMul,
    /* Uniforms */      { 
                          /* cxadd*/      {0, 1032, 4, 4, 4, 0 },
                          /* cxmul*/      {1, 1036, 16, 16, 16, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      { -1, 0, 0, 0, 0, 0 },
                          /* mvp*/        { -1, 0, 0, 0, 0, 0 },
                          /* offset*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor2*/    { -1, 0, 0, 0, 0, 0 },
                          /* srctex*/     { -1, 0, 0, 0, 0, 0 },
                          /* srctexscale*/{ -1, 0, 0, 0, 0, 0 },
                          /* tex*/        {0, 1032, 1, 1, 1, 0 },
                          /* texgen*/     { -1, 0, 0, 0, 0, 0 },
                          /* texscale*/   { -1, 0, 0, 0, 0, 0 },
                          /* vfmuniforms*/{ -1, 0, 0, 0, 0, 0 },
                          /* vfuniforms*/ { -1, 0, 0, 0, 0, 0 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_Count, -1, 0},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_Count, -1, 0},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    
    };

static FragShaderDesc ShaderDesc_FS_FBox1Blur = {
    /* Type */          FragShaderDesc::FS_FBox1Blur,
    /* Flags */         Shader_DynamicLoop,
    /* pSource */       pSource_FBox1Blur,
    /* Uniforms */      { 
                          /* cxadd*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      {0, 1032, 4, 4, 4, 0 },
                          /* mvp*/        { -1, 0, 0, 0, 0, 0 },
                          /* offset*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor2*/    { -1, 0, 0, 0, 0, 0 },
                          /* srctex*/     { -1, 0, 0, 0, 0, 0 },
                          /* srctexscale*/{ -1, 0, 0, 0, 0, 0 },
                          /* tex*/        {0, 1032, 1, 1, 1, 0 },
                          /* texgen*/     { -1, 0, 0, 0, 0, 0 },
                          /* texscale*/   {1, 1036, 4, 4, 4, 0 },
                          /* vfmuniforms*/{ -1, 0, 0, 0, 0, 0 },
                          /* vfuniforms*/ { -1, 0, 0, 0, 0, 0 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_Count, -1, 0},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_Count, -1, 0},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    
    };

static FragShaderDesc ShaderDesc_FS_FBox1BlurMul = {
    /* Type */          FragShaderDesc::FS_FBox1BlurMul,
    /* Flags */         Shader_DynamicLoop,
    /* pSource */       pSource_FBox1BlurMul,
    /* Uniforms */      { 
                          /* cxadd*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      {0, 1032, 4, 4, 4, 0 },
                          /* mvp*/        { -1, 0, 0, 0, 0, 0 },
                          /* offset*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor2*/    { -1, 0, 0, 0, 0, 0 },
                          /* srctex*/     { -1, 0, 0, 0, 0, 0 },
                          /* srctexscale*/{ -1, 0, 0, 0, 0, 0 },
                          /* tex*/        {0, 1032, 1, 1, 1, 0 },
                          /* texgen*/     { -1, 0, 0, 0, 0, 0 },
                          /* texscale*/   {1, 1036, 4, 4, 4, 0 },
                          /* vfmuniforms*/{ -1, 0, 0, 0, 0, 0 },
                          /* vfuniforms*/ { -1, 0, 0, 0, 0, 0 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_Count, -1, 0},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_Count, -1, 0},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    
    };

static FragShaderDesc ShaderDesc_FS_FBox2Blur = {
    /* Type */          FragShaderDesc::FS_FBox2Blur,
    /* Flags */         Shader_DynamicLoop,
    /* pSource */       pSource_FBox2Blur,
    /* Uniforms */      { 
                          /* cxadd*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      {0, 1032, 4, 4, 4, 0 },
                          /* mvp*/        { -1, 0, 0, 0, 0, 0 },
                          /* offset*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor2*/    { -1, 0, 0, 0, 0, 0 },
                          /* srctex*/     { -1, 0, 0, 0, 0, 0 },
                          /* srctexscale*/{ -1, 0, 0, 0, 0, 0 },
                          /* tex*/        {0, 1032, 1, 1, 1, 0 },
                          /* texgen*/     { -1, 0, 0, 0, 0, 0 },
                          /* texscale*/   {1, 1036, 4, 4, 4, 0 },
                          /* vfmuniforms*/{ -1, 0, 0, 0, 0, 0 },
                          /* vfuniforms*/ { -1, 0, 0, 0, 0, 0 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_Count, -1, 0},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_Count, -1, 0},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    
    };

static FragShaderDesc ShaderDesc_FS_FBox2BlurMul = {
    /* Type */          FragShaderDesc::FS_FBox2BlurMul,
    /* Flags */         Shader_DynamicLoop,
    /* pSource */       pSource_FBox2BlurMul,
    /* Uniforms */      { 
                          /* cxadd*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      {0, 1032, 4, 4, 4, 0 },
                          /* mvp*/        { -1, 0, 0, 0, 0, 0 },
                          /* offset*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor2*/    { -1, 0, 0, 0, 0, 0 },
                          /* srctex*/     { -1, 0, 0, 0, 0, 0 },
                          /* srctexscale*/{ -1, 0, 0, 0, 0, 0 },
                          /* tex*/        {0, 1032, 1, 1, 1, 0 },
                          /* texgen*/     { -1, 0, 0, 0, 0, 0 },
                          /* texscale*/   {1, 1036, 4, 4, 4, 0 },
                          /* vfmuniforms*/{ -1, 0, 0, 0, 0, 0 },
                          /* vfuniforms*/ { -1, 0, 0, 0, 0, 0 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_Count, -1, 0},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_Count, -1, 0},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    
    };

static FragShaderDesc ShaderDesc_FS_FBox2Shadow = {
    /* Type */          FragShaderDesc::FS_FBox2Shadow,
    /* Flags */         Shader_DynamicLoop,
    /* pSource */       pSource_FBox2Shadow,
    /* Uniforms */      { 
                          /* cxadd*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      {0, 1032, 4, 4, 4, 0 },
                          /* mvp*/        { -1, 0, 0, 0, 0, 0 },
                          /* offset*/     {1, 1036, 4, 4, 4, 0 },
                          /* scolor*/     {2, 1040, 4, 4, 4, 0 },
                          /* scolor2*/    { -1, 0, 0, 0, 0, 0 },
                          /* srctex*/     {0, 1032, 1, 1, 1, 0 },
                          /* srctexscale*/{3, 1044, 4, 4, 4, 0 },
                          /* tex*/        {1, 1036, 1, 1, 1, 0 },
                          /* texgen*/     { -1, 0, 0, 0, 0, 0 },
                          /* texscale*/   {4, 1048, 4, 4, 4, 0 },
                          /* vfmuniforms*/{ -1, 0, 0, 0, 0, 0 },
                          /* vfuniforms*/ { -1, 0, 0, 0, 0, 0 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_Count, -1, 0},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_Count, -1, 0},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    
    };

static FragShaderDesc ShaderDesc_FS_FBox2ShadowMul = {
    /* Type */          FragShaderDesc::FS_FBox2ShadowMul,
    /* Flags */         Shader_DynamicLoop,
    /* pSource */       pSource_FBox2ShadowMul,
    /* Uniforms */      { 
                          /* cxadd*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      {0, 1032, 4, 4, 4, 0 },
                          /* mvp*/        { -1, 0, 0, 0, 0, 0 },
                          /* offset*/     {1, 1036, 4, 4, 4, 0 },
                          /* scolor*/     {2, 1040, 4, 4, 4, 0 },
                          /* scolor2*/    { -1, 0, 0, 0, 0, 0 },
                          /* srctex*/     {0, 1032, 1, 1, 1, 0 },
                          /* srctexscale*/{3, 1044, 4, 4, 4, 0 },
                          /* tex*/        {1, 1036, 1, 1, 1, 0 },
                          /* texgen*/     { -1, 0, 0, 0, 0, 0 },
                          /* texscale*/   {4, 1048, 4, 4, 4, 0 },
                          /* vfmuniforms*/{ -1, 0, 0, 0, 0, 0 },
                          /* vfuniforms*/ { -1, 0, 0, 0, 0, 0 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_Count, -1, 0},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_Count, -1, 0},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    
    };

static FragShaderDesc ShaderDesc_FS_FBox2ShadowKnockout = {
    /* Type */          FragShaderDesc::FS_FBox2ShadowKnockout,
    /* Flags */         Shader_DynamicLoop,
    /* pSource */       pSource_FBox2ShadowKnockout,
    /* Uniforms */      { 
                          /* cxadd*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      {0, 1032, 4, 4, 4, 0 },
                          /* mvp*/        { -1, 0, 0, 0, 0, 0 },
                          /* offset*/     {1, 1036, 4, 4, 4, 0 },
                          /* scolor*/     {2, 1040, 4, 4, 4, 0 },
                          /* scolor2*/    { -1, 0, 0, 0, 0, 0 },
                          /* srctex*/     {0, 1032, 1, 1, 1, 0 },
                          /* srctexscale*/{3, 1044, 4, 4, 4, 0 },
                          /* tex*/        {1, 1036, 1, 1, 1, 0 },
                          /* texgen*/     { -1, 0, 0, 0, 0, 0 },
                          /* texscale*/   {4, 1048, 4, 4, 4, 0 },
                          /* vfmuniforms*/{ -1, 0, 0, 0, 0, 0 },
                          /* vfuniforms*/ { -1, 0, 0, 0, 0, 0 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_Count, -1, 0},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_Count, -1, 0},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    
    };

static FragShaderDesc ShaderDesc_FS_FBox2ShadowKnockoutMul = {
    /* Type */          FragShaderDesc::FS_FBox2ShadowKnockoutMul,
    /* Flags */         Shader_DynamicLoop,
    /* pSource */       pSource_FBox2ShadowKnockoutMul,
    /* Uniforms */      { 
                          /* cxadd*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      {0, 1032, 4, 4, 4, 0 },
                          /* mvp*/        { -1, 0, 0, 0, 0, 0 },
                          /* offset*/     {1, 1036, 4, 4, 4, 0 },
                          /* scolor*/     {2, 1040, 4, 4, 4, 0 },
                          /* scolor2*/    { -1, 0, 0, 0, 0, 0 },
                          /* srctex*/     {0, 1032, 1, 1, 1, 0 },
                          /* srctexscale*/{3, 1044, 4, 4, 4, 0 },
                          /* tex*/        {1, 1036, 1, 1, 1, 0 },
                          /* texgen*/     { -1, 0, 0, 0, 0, 0 },
                          /* texscale*/   {4, 1048, 4, 4, 4, 0 },
                          /* vfmuniforms*/{ -1, 0, 0, 0, 0, 0 },
                          /* vfuniforms*/ { -1, 0, 0, 0, 0, 0 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_Count, -1, 0},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_Count, -1, 0},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    
    };

static FragShaderDesc ShaderDesc_FS_FBox2InnerShadow = {
    /* Type */          FragShaderDesc::FS_FBox2InnerShadow,
    /* Flags */         Shader_DynamicLoop,
    /* pSource */       pSource_FBox2InnerShadow,
    /* Uniforms */      { 
                          /* cxadd*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      {0, 1032, 4, 4, 4, 0 },
                          /* mvp*/        { -1, 0, 0, 0, 0, 0 },
                          /* offset*/     {1, 1036, 4, 4, 4, 0 },
                          /* scolor*/     {2, 1040, 4, 4, 4, 0 },
                          /* scolor2*/    { -1, 0, 0, 0, 0, 0 },
                          /* srctex*/     {0, 1032, 1, 1, 1, 0 },
                          /* srctexscale*/{3, 1044, 4, 4, 4, 0 },
                          /* tex*/        {1, 1036, 1, 1, 1, 0 },
                          /* texgen*/     { -1, 0, 0, 0, 0, 0 },
                          /* texscale*/   {4, 1048, 4, 4, 4, 0 },
                          /* vfmuniforms*/{ -1, 0, 0, 0, 0, 0 },
                          /* vfuniforms*/ { -1, 0, 0, 0, 0, 0 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_Count, -1, 0},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_Count, -1, 0},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    
    };

static FragShaderDesc ShaderDesc_FS_FBox2InnerShadowMul = {
    /* Type */          FragShaderDesc::FS_FBox2InnerShadowMul,
    /* Flags */         Shader_DynamicLoop,
    /* pSource */       pSource_FBox2InnerShadowMul,
    /* Uniforms */      { 
                          /* cxadd*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      {0, 1032, 4, 4, 4, 0 },
                          /* mvp*/        { -1, 0, 0, 0, 0, 0 },
                          /* offset*/     {1, 1036, 4, 4, 4, 0 },
                          /* scolor*/     {2, 1040, 4, 4, 4, 0 },
                          /* scolor2*/    { -1, 0, 0, 0, 0, 0 },
                          /* srctex*/     {0, 1032, 1, 1, 1, 0 },
                          /* srctexscale*/{3, 1044, 4, 4, 4, 0 },
                          /* tex*/        {1, 1036, 1, 1, 1, 0 },
                          /* texgen*/     { -1, 0, 0, 0, 0, 0 },
                          /* texscale*/   {4, 1048, 4, 4, 4, 0 },
                          /* vfmuniforms*/{ -1, 0, 0, 0, 0, 0 },
                          /* vfuniforms*/ { -1, 0, 0, 0, 0, 0 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_Count, -1, 0},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_Count, -1, 0},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    
    };

static FragShaderDesc ShaderDesc_FS_FBox2InnerShadowKnockout = {
    /* Type */          FragShaderDesc::FS_FBox2InnerShadowKnockout,
    /* Flags */         Shader_DynamicLoop,
    /* pSource */       pSource_FBox2InnerShadowKnockout,
    /* Uniforms */      { 
                          /* cxadd*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      {0, 1032, 4, 4, 4, 0 },
                          /* mvp*/        { -1, 0, 0, 0, 0, 0 },
                          /* offset*/     {1, 1036, 4, 4, 4, 0 },
                          /* scolor*/     {2, 1040, 4, 4, 4, 0 },
                          /* scolor2*/    { -1, 0, 0, 0, 0, 0 },
                          /* srctex*/     {0, 1032, 1, 1, 1, 0 },
                          /* srctexscale*/{3, 1044, 4, 4, 4, 0 },
                          /* tex*/        {1, 1036, 1, 1, 1, 0 },
                          /* texgen*/     { -1, 0, 0, 0, 0, 0 },
                          /* texscale*/   {4, 1048, 4, 4, 4, 0 },
                          /* vfmuniforms*/{ -1, 0, 0, 0, 0, 0 },
                          /* vfuniforms*/ { -1, 0, 0, 0, 0, 0 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_Count, -1, 0},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_Count, -1, 0},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    
    };

static FragShaderDesc ShaderDesc_FS_FBox2InnerShadowKnockoutMul = {
    /* Type */          FragShaderDesc::FS_FBox2InnerShadowKnockoutMul,
    /* Flags */         Shader_DynamicLoop,
    /* pSource */       pSource_FBox2InnerShadowKnockoutMul,
    /* Uniforms */      { 
                          /* cxadd*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      {0, 1032, 4, 4, 4, 0 },
                          /* mvp*/        { -1, 0, 0, 0, 0, 0 },
                          /* offset*/     {1, 1036, 4, 4, 4, 0 },
                          /* scolor*/     {2, 1040, 4, 4, 4, 0 },
                          /* scolor2*/    { -1, 0, 0, 0, 0, 0 },
                          /* srctex*/     {0, 1032, 1, 1, 1, 0 },
                          /* srctexscale*/{3, 1044, 4, 4, 4, 0 },
                          /* tex*/        {1, 1036, 1, 1, 1, 0 },
                          /* texgen*/     { -1, 0, 0, 0, 0, 0 },
                          /* texscale*/   {4, 1048, 4, 4, 4, 0 },
                          /* vfmuniforms*/{ -1, 0, 0, 0, 0, 0 },
                          /* vfuniforms*/ { -1, 0, 0, 0, 0, 0 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_Count, -1, 0},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_Count, -1, 0},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    
    };

static FragShaderDesc ShaderDesc_FS_FBox2Shadowonly = {
    /* Type */          FragShaderDesc::FS_FBox2Shadowonly,
    /* Flags */         Shader_DynamicLoop,
    /* pSource */       pSource_FBox2Shadowonly,
    /* Uniforms */      { 
                          /* cxadd*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      {0, 1032, 4, 4, 4, 0 },
                          /* mvp*/        { -1, 0, 0, 0, 0, 0 },
                          /* offset*/     {1, 1036, 4, 4, 4, 0 },
                          /* scolor*/     {2, 1040, 4, 4, 4, 0 },
                          /* scolor2*/    { -1, 0, 0, 0, 0, 0 },
                          /* srctex*/     { -1, 0, 0, 0, 0, 0 },
                          /* srctexscale*/{ -1, 0, 0, 0, 0, 0 },
                          /* tex*/        {0, 1032, 1, 1, 1, 0 },
                          /* texgen*/     { -1, 0, 0, 0, 0, 0 },
                          /* texscale*/   {3, 1044, 4, 4, 4, 0 },
                          /* vfmuniforms*/{ -1, 0, 0, 0, 0, 0 },
                          /* vfuniforms*/ { -1, 0, 0, 0, 0, 0 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_Count, -1, 0},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_Count, -1, 0},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    
    };

static FragShaderDesc ShaderDesc_FS_FBox2ShadowonlyMul = {
    /* Type */          FragShaderDesc::FS_FBox2ShadowonlyMul,
    /* Flags */         Shader_DynamicLoop,
    /* pSource */       pSource_FBox2ShadowonlyMul,
    /* Uniforms */      { 
                          /* cxadd*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      {0, 1032, 4, 4, 4, 0 },
                          /* mvp*/        { -1, 0, 0, 0, 0, 0 },
                          /* offset*/     {1, 1036, 4, 4, 4, 0 },
                          /* scolor*/     {2, 1040, 4, 4, 4, 0 },
                          /* scolor2*/    { -1, 0, 0, 0, 0, 0 },
                          /* srctex*/     { -1, 0, 0, 0, 0, 0 },
                          /* srctexscale*/{ -1, 0, 0, 0, 0, 0 },
                          /* tex*/        {0, 1032, 1, 1, 1, 0 },
                          /* texgen*/     { -1, 0, 0, 0, 0, 0 },
                          /* texscale*/   {3, 1044, 4, 4, 4, 0 },
                          /* vfmuniforms*/{ -1, 0, 0, 0, 0, 0 },
                          /* vfuniforms*/ { -1, 0, 0, 0, 0, 0 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_Count, -1, 0},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_Count, -1, 0},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    
    };

static FragShaderDesc ShaderDesc_FS_FBox2ShadowHighlight = {
    /* Type */          FragShaderDesc::FS_FBox2ShadowHighlight,
    /* Flags */         Shader_DynamicLoop,
    /* pSource */       pSource_FBox2ShadowHighlight,
    /* Uniforms */      { 
                          /* cxadd*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      {0, 1032, 4, 4, 4, 0 },
                          /* mvp*/        { -1, 0, 0, 0, 0, 0 },
                          /* offset*/     {1, 1036, 4, 4, 4, 0 },
                          /* scolor*/     {2, 1040, 4, 4, 4, 0 },
                          /* scolor2*/    {3, 1044, 4, 4, 4, 0 },
                          /* srctex*/     {0, 1032, 1, 1, 1, 0 },
                          /* srctexscale*/{4, 1048, 4, 4, 4, 0 },
                          /* tex*/        {1, 1036, 1, 1, 1, 0 },
                          /* texgen*/     { -1, 0, 0, 0, 0, 0 },
                          /* texscale*/   {5, 1052, 4, 4, 4, 0 },
                          /* vfmuniforms*/{ -1, 0, 0, 0, 0, 0 },
                          /* vfuniforms*/ { -1, 0, 0, 0, 0, 0 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_Count, -1, 0},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_Count, -1, 0},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    
    };

static FragShaderDesc ShaderDesc_FS_FBox2ShadowHighlightMul = {
    /* Type */          FragShaderDesc::FS_FBox2ShadowHighlightMul,
    /* Flags */         Shader_DynamicLoop,
    /* pSource */       pSource_FBox2ShadowHighlightMul,
    /* Uniforms */      { 
                          /* cxadd*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      {0, 1032, 4, 4, 4, 0 },
                          /* mvp*/        { -1, 0, 0, 0, 0, 0 },
                          /* offset*/     {1, 1036, 4, 4, 4, 0 },
                          /* scolor*/     {2, 1040, 4, 4, 4, 0 },
                          /* scolor2*/    {3, 1044, 4, 4, 4, 0 },
                          /* srctex*/     {0, 1032, 1, 1, 1, 0 },
                          /* srctexscale*/{4, 1048, 4, 4, 4, 0 },
                          /* tex*/        {1, 1036, 1, 1, 1, 0 },
                          /* texgen*/     { -1, 0, 0, 0, 0, 0 },
                          /* texscale*/   {5, 1052, 4, 4, 4, 0 },
                          /* vfmuniforms*/{ -1, 0, 0, 0, 0, 0 },
                          /* vfuniforms*/ { -1, 0, 0, 0, 0, 0 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_Count, -1, 0},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_Count, -1, 0},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    
    };

static FragShaderDesc ShaderDesc_FS_FBox2ShadowHighlightKnockout = {
    /* Type */          FragShaderDesc::FS_FBox2ShadowHighlightKnockout,
    /* Flags */         Shader_DynamicLoop,
    /* pSource */       pSource_FBox2ShadowHighlightKnockout,
    /* Uniforms */      { 
                          /* cxadd*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      {0, 1032, 4, 4, 4, 0 },
                          /* mvp*/        { -1, 0, 0, 0, 0, 0 },
                          /* offset*/     {1, 1036, 4, 4, 4, 0 },
                          /* scolor*/     {2, 1040, 4, 4, 4, 0 },
                          /* scolor2*/    {3, 1044, 4, 4, 4, 0 },
                          /* srctex*/     {0, 1032, 1, 1, 1, 0 },
                          /* srctexscale*/{4, 1048, 4, 4, 4, 0 },
                          /* tex*/        {1, 1036, 1, 1, 1, 0 },
                          /* texgen*/     { -1, 0, 0, 0, 0, 0 },
                          /* texscale*/   {5, 1052, 4, 4, 4, 0 },
                          /* vfmuniforms*/{ -1, 0, 0, 0, 0, 0 },
                          /* vfuniforms*/ { -1, 0, 0, 0, 0, 0 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_Count, -1, 0},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_Count, -1, 0},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    
    };

static FragShaderDesc ShaderDesc_FS_FBox2ShadowHighlightKnockoutMul = {
    /* Type */          FragShaderDesc::FS_FBox2ShadowHighlightKnockoutMul,
    /* Flags */         Shader_DynamicLoop,
    /* pSource */       pSource_FBox2ShadowHighlightKnockoutMul,
    /* Uniforms */      { 
                          /* cxadd*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      {0, 1032, 4, 4, 4, 0 },
                          /* mvp*/        { -1, 0, 0, 0, 0, 0 },
                          /* offset*/     {1, 1036, 4, 4, 4, 0 },
                          /* scolor*/     {2, 1040, 4, 4, 4, 0 },
                          /* scolor2*/    {3, 1044, 4, 4, 4, 0 },
                          /* srctex*/     {0, 1032, 1, 1, 1, 0 },
                          /* srctexscale*/{4, 1048, 4, 4, 4, 0 },
                          /* tex*/        {1, 1036, 1, 1, 1, 0 },
                          /* texgen*/     { -1, 0, 0, 0, 0, 0 },
                          /* texscale*/   {5, 1052, 4, 4, 4, 0 },
                          /* vfmuniforms*/{ -1, 0, 0, 0, 0, 0 },
                          /* vfuniforms*/ { -1, 0, 0, 0, 0, 0 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_Count, -1, 0},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_Count, -1, 0},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    
    };

static FragShaderDesc ShaderDesc_FS_FBox2InnerShadowHighlight = {
    /* Type */          FragShaderDesc::FS_FBox2InnerShadowHighlight,
    /* Flags */         Shader_DynamicLoop,
    /* pSource */       pSource_FBox2InnerShadowHighlight,
    /* Uniforms */      { 
                          /* cxadd*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      {0, 1032, 4, 4, 4, 0 },
                          /* mvp*/        { -1, 0, 0, 0, 0, 0 },
                          /* offset*/     {1, 1036, 4, 4, 4, 0 },
                          /* scolor*/     {2, 1040, 4, 4, 4, 0 },
                          /* scolor2*/    {3, 1044, 4, 4, 4, 0 },
                          /* srctex*/     {0, 1032, 1, 1, 1, 0 },
                          /* srctexscale*/{4, 1048, 4, 4, 4, 0 },
                          /* tex*/        {1, 1036, 1, 1, 1, 0 },
                          /* texgen*/     { -1, 0, 0, 0, 0, 0 },
                          /* texscale*/   {5, 1052, 4, 4, 4, 0 },
                          /* vfmuniforms*/{ -1, 0, 0, 0, 0, 0 },
                          /* vfuniforms*/ { -1, 0, 0, 0, 0, 0 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_Count, -1, 0},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_Count, -1, 0},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    
    };

static FragShaderDesc ShaderDesc_FS_FBox2InnerShadowHighlightMul = {
    /* Type */          FragShaderDesc::FS_FBox2InnerShadowHighlightMul,
    /* Flags */         Shader_DynamicLoop,
    /* pSource */       pSource_FBox2InnerShadowHighlightMul,
    /* Uniforms */      { 
                          /* cxadd*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      {0, 1032, 4, 4, 4, 0 },
                          /* mvp*/        { -1, 0, 0, 0, 0, 0 },
                          /* offset*/     {1, 1036, 4, 4, 4, 0 },
                          /* scolor*/     {2, 1040, 4, 4, 4, 0 },
                          /* scolor2*/    {3, 1044, 4, 4, 4, 0 },
                          /* srctex*/     {0, 1032, 1, 1, 1, 0 },
                          /* srctexscale*/{4, 1048, 4, 4, 4, 0 },
                          /* tex*/        {1, 1036, 1, 1, 1, 0 },
                          /* texgen*/     { -1, 0, 0, 0, 0, 0 },
                          /* texscale*/   {5, 1052, 4, 4, 4, 0 },
                          /* vfmuniforms*/{ -1, 0, 0, 0, 0, 0 },
                          /* vfuniforms*/ { -1, 0, 0, 0, 0, 0 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_Count, -1, 0},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_Count, -1, 0},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    
    };

static FragShaderDesc ShaderDesc_FS_FBox2InnerShadowHighlightKnockout = {
    /* Type */          FragShaderDesc::FS_FBox2InnerShadowHighlightKnockout,
    /* Flags */         Shader_DynamicLoop,
    /* pSource */       pSource_FBox2InnerShadowHighlightKnockout,
    /* Uniforms */      { 
                          /* cxadd*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      {0, 1032, 4, 4, 4, 0 },
                          /* mvp*/        { -1, 0, 0, 0, 0, 0 },
                          /* offset*/     {1, 1036, 4, 4, 4, 0 },
                          /* scolor*/     {2, 1040, 4, 4, 4, 0 },
                          /* scolor2*/    {3, 1044, 4, 4, 4, 0 },
                          /* srctex*/     {0, 1032, 1, 1, 1, 0 },
                          /* srctexscale*/{4, 1048, 4, 4, 4, 0 },
                          /* tex*/        {1, 1036, 1, 1, 1, 0 },
                          /* texgen*/     { -1, 0, 0, 0, 0, 0 },
                          /* texscale*/   {5, 1052, 4, 4, 4, 0 },
                          /* vfmuniforms*/{ -1, 0, 0, 0, 0, 0 },
                          /* vfuniforms*/ { -1, 0, 0, 0, 0, 0 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_Count, -1, 0},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_Count, -1, 0},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    
    };

static FragShaderDesc ShaderDesc_FS_FBox2InnerShadowHighlightKnockoutMul = {
    /* Type */          FragShaderDesc::FS_FBox2InnerShadowHighlightKnockoutMul,
    /* Flags */         Shader_DynamicLoop,
    /* pSource */       pSource_FBox2InnerShadowHighlightKnockoutMul,
    /* Uniforms */      { 
                          /* cxadd*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      {0, 1032, 4, 4, 4, 0 },
                          /* mvp*/        { -1, 0, 0, 0, 0, 0 },
                          /* offset*/     {1, 1036, 4, 4, 4, 0 },
                          /* scolor*/     {2, 1040, 4, 4, 4, 0 },
                          /* scolor2*/    {3, 1044, 4, 4, 4, 0 },
                          /* srctex*/     {0, 1032, 1, 1, 1, 0 },
                          /* srctexscale*/{4, 1048, 4, 4, 4, 0 },
                          /* tex*/        {1, 1036, 1, 1, 1, 0 },
                          /* texgen*/     { -1, 0, 0, 0, 0, 0 },
                          /* texscale*/   {5, 1052, 4, 4, 4, 0 },
                          /* vfmuniforms*/{ -1, 0, 0, 0, 0, 0 },
                          /* vfuniforms*/ { -1, 0, 0, 0, 0, 0 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_Count, -1, 0},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_Count, -1, 0},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    
    };

static FragShaderDesc ShaderDesc_FS_FBox2ShadowonlyHighlight = {
    /* Type */          FragShaderDesc::FS_FBox2ShadowonlyHighlight,
    /* Flags */         Shader_DynamicLoop,
    /* pSource */       pSource_FBox2ShadowonlyHighlight,
    /* Uniforms */      { 
                          /* cxadd*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      {0, 1032, 4, 4, 4, 0 },
                          /* mvp*/        { -1, 0, 0, 0, 0, 0 },
                          /* offset*/     {1, 1036, 4, 4, 4, 0 },
                          /* scolor*/     {2, 1040, 4, 4, 4, 0 },
                          /* scolor2*/    { -1, 0, 0, 0, 0, 0 },
                          /* srctex*/     { -1, 0, 0, 0, 0, 0 },
                          /* srctexscale*/{ -1, 0, 0, 0, 0, 0 },
                          /* tex*/        {0, 1032, 1, 1, 1, 0 },
                          /* texgen*/     { -1, 0, 0, 0, 0, 0 },
                          /* texscale*/   {3, 1044, 4, 4, 4, 0 },
                          /* vfmuniforms*/{ -1, 0, 0, 0, 0, 0 },
                          /* vfuniforms*/ { -1, 0, 0, 0, 0, 0 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_Count, -1, 0},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_Count, -1, 0},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    
    };

static FragShaderDesc ShaderDesc_FS_FBox2ShadowonlyHighlightMul = {
    /* Type */          FragShaderDesc::FS_FBox2ShadowonlyHighlightMul,
    /* Flags */         Shader_DynamicLoop,
    /* pSource */       pSource_FBox2ShadowonlyHighlightMul,
    /* Uniforms */      { 
                          /* cxadd*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      {0, 1032, 4, 4, 4, 0 },
                          /* mvp*/        { -1, 0, 0, 0, 0, 0 },
                          /* offset*/     {1, 1036, 4, 4, 4, 0 },
                          /* scolor*/     {2, 1040, 4, 4, 4, 0 },
                          /* scolor2*/    { -1, 0, 0, 0, 0, 0 },
                          /* srctex*/     { -1, 0, 0, 0, 0, 0 },
                          /* srctexscale*/{ -1, 0, 0, 0, 0, 0 },
                          /* tex*/        {0, 1032, 1, 1, 1, 0 },
                          /* texgen*/     { -1, 0, 0, 0, 0, 0 },
                          /* texscale*/   {3, 1044, 4, 4, 4, 0 },
                          /* vfmuniforms*/{ -1, 0, 0, 0, 0, 0 },
                          /* vfuniforms*/ { -1, 0, 0, 0, 0, 0 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_Count, -1, 0},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_Count, -1, 0},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    
    };

static FragShaderDesc ShaderDesc_FS_FDrawableCopyPixels = {
    /* Type */          FragShaderDesc::FS_FDrawableCopyPixels,
    /* Flags */         0,
    /* pSource */       pSource_FDrawableCopyPixels,
    /* Uniforms */      { 
                          /* cxadd*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      { -1, 0, 0, 0, 0, 0 },
                          /* mvp*/        { -1, 0, 0, 0, 0, 0 },
                          /* offset*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor2*/    { -1, 0, 0, 0, 0, 0 },
                          /* srctex*/     { -1, 0, 0, 0, 0, 0 },
                          /* srctexscale*/{ -1, 0, 0, 0, 0, 0 },
                          /* tex*/        {0, 1032, 1, 2, 1, 0 },
                          /* texgen*/     { -1, 0, 0, 0, 0, 0 },
                          /* texscale*/   { -1, 0, 0, 0, 0, 0 },
                          /* vfmuniforms*/{ -1, 0, 0, 0, 0, 0 },
                          /* vfuniforms*/ { -1, 0, 0, 0, 0, 0 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_Count, -1, 0},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_Count, -1, 0},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    
    };

static FragShaderDesc ShaderDesc_FS_FDrawableCopyPixelsCopyLerp = {
    /* Type */          FragShaderDesc::FS_FDrawableCopyPixelsCopyLerp,
    /* Flags */         0,
    /* pSource */       pSource_FDrawableCopyPixelsCopyLerp,
    /* Uniforms */      { 
                          /* cxadd*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      { -1, 0, 0, 0, 0, 0 },
                          /* mvp*/        { -1, 0, 0, 0, 0, 0 },
                          /* offset*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor2*/    { -1, 0, 0, 0, 0, 0 },
                          /* srctex*/     { -1, 0, 0, 0, 0, 0 },
                          /* srctexscale*/{ -1, 0, 0, 0, 0, 0 },
                          /* tex*/        {0, 1032, 1, 2, 1, 0 },
                          /* texgen*/     { -1, 0, 0, 0, 0, 0 },
                          /* texscale*/   { -1, 0, 0, 0, 0, 0 },
                          /* vfmuniforms*/{ -1, 0, 0, 0, 0, 0 },
                          /* vfuniforms*/ { -1, 0, 0, 0, 0, 0 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_Count, -1, 0},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_Count, -1, 0},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    
    };

static FragShaderDesc ShaderDesc_FS_FDrawableCopyPixelsNoDestAlpha = {
    /* Type */          FragShaderDesc::FS_FDrawableCopyPixelsNoDestAlpha,
    /* Flags */         0,
    /* pSource */       pSource_FDrawableCopyPixelsNoDestAlpha,
    /* Uniforms */      { 
                          /* cxadd*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      { -1, 0, 0, 0, 0, 0 },
                          /* mvp*/        { -1, 0, 0, 0, 0, 0 },
                          /* offset*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor2*/    { -1, 0, 0, 0, 0, 0 },
                          /* srctex*/     { -1, 0, 0, 0, 0, 0 },
                          /* srctexscale*/{ -1, 0, 0, 0, 0, 0 },
                          /* tex*/        {0, 1032, 1, 2, 1, 0 },
                          /* texgen*/     { -1, 0, 0, 0, 0, 0 },
                          /* texscale*/   { -1, 0, 0, 0, 0, 0 },
                          /* vfmuniforms*/{ -1, 0, 0, 0, 0, 0 },
                          /* vfuniforms*/ { -1, 0, 0, 0, 0, 0 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_Count, -1, 0},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_Count, -1, 0},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    
    };

static FragShaderDesc ShaderDesc_FS_FDrawableCopyPixelsNoDestAlphaCopyLerp = {
    /* Type */          FragShaderDesc::FS_FDrawableCopyPixelsNoDestAlphaCopyLerp,
    /* Flags */         0,
    /* pSource */       pSource_FDrawableCopyPixelsNoDestAlphaCopyLerp,
    /* Uniforms */      { 
                          /* cxadd*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      { -1, 0, 0, 0, 0, 0 },
                          /* mvp*/        { -1, 0, 0, 0, 0, 0 },
                          /* offset*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor2*/    { -1, 0, 0, 0, 0, 0 },
                          /* srctex*/     { -1, 0, 0, 0, 0, 0 },
                          /* srctexscale*/{ -1, 0, 0, 0, 0, 0 },
                          /* tex*/        {0, 1032, 1, 2, 1, 0 },
                          /* texgen*/     { -1, 0, 0, 0, 0, 0 },
                          /* texscale*/   { -1, 0, 0, 0, 0, 0 },
                          /* vfmuniforms*/{ -1, 0, 0, 0, 0, 0 },
                          /* vfuniforms*/ { -1, 0, 0, 0, 0, 0 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_Count, -1, 0},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_Count, -1, 0},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    
    };

static FragShaderDesc ShaderDesc_FS_FDrawableCopyPixelsMergeAlpha = {
    /* Type */          FragShaderDesc::FS_FDrawableCopyPixelsMergeAlpha,
    /* Flags */         0,
    /* pSource */       pSource_FDrawableCopyPixelsMergeAlpha,
    /* Uniforms */      { 
                          /* cxadd*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      { -1, 0, 0, 0, 0, 0 },
                          /* mvp*/        { -1, 0, 0, 0, 0, 0 },
                          /* offset*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor2*/    { -1, 0, 0, 0, 0, 0 },
                          /* srctex*/     { -1, 0, 0, 0, 0, 0 },
                          /* srctexscale*/{ -1, 0, 0, 0, 0, 0 },
                          /* tex*/        {0, 1032, 1, 2, 1, 0 },
                          /* texgen*/     { -1, 0, 0, 0, 0, 0 },
                          /* texscale*/   { -1, 0, 0, 0, 0, 0 },
                          /* vfmuniforms*/{ -1, 0, 0, 0, 0, 0 },
                          /* vfuniforms*/ { -1, 0, 0, 0, 0, 0 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_Count, -1, 0},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_Count, -1, 0},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    
    };

static FragShaderDesc ShaderDesc_FS_FDrawableCopyPixelsMergeAlphaCopyLerp = {
    /* Type */          FragShaderDesc::FS_FDrawableCopyPixelsMergeAlphaCopyLerp,
    /* Flags */         0,
    /* pSource */       pSource_FDrawableCopyPixelsMergeAlphaCopyLerp,
    /* Uniforms */      { 
                          /* cxadd*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      { -1, 0, 0, 0, 0, 0 },
                          /* mvp*/        { -1, 0, 0, 0, 0, 0 },
                          /* offset*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor2*/    { -1, 0, 0, 0, 0, 0 },
                          /* srctex*/     { -1, 0, 0, 0, 0, 0 },
                          /* srctexscale*/{ -1, 0, 0, 0, 0, 0 },
                          /* tex*/        {0, 1032, 1, 2, 1, 0 },
                          /* texgen*/     { -1, 0, 0, 0, 0, 0 },
                          /* texscale*/   { -1, 0, 0, 0, 0, 0 },
                          /* vfmuniforms*/{ -1, 0, 0, 0, 0, 0 },
                          /* vfuniforms*/ { -1, 0, 0, 0, 0, 0 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_Count, -1, 0},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_Count, -1, 0},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    
    };

static FragShaderDesc ShaderDesc_FS_FDrawableCopyPixelsMergeAlphaNoDestAlpha = {
    /* Type */          FragShaderDesc::FS_FDrawableCopyPixelsMergeAlphaNoDestAlpha,
    /* Flags */         0,
    /* pSource */       pSource_FDrawableCopyPixelsMergeAlphaNoDestAlpha,
    /* Uniforms */      { 
                          /* cxadd*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      { -1, 0, 0, 0, 0, 0 },
                          /* mvp*/        { -1, 0, 0, 0, 0, 0 },
                          /* offset*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor2*/    { -1, 0, 0, 0, 0, 0 },
                          /* srctex*/     { -1, 0, 0, 0, 0, 0 },
                          /* srctexscale*/{ -1, 0, 0, 0, 0, 0 },
                          /* tex*/        {0, 1032, 1, 2, 1, 0 },
                          /* texgen*/     { -1, 0, 0, 0, 0, 0 },
                          /* texscale*/   { -1, 0, 0, 0, 0, 0 },
                          /* vfmuniforms*/{ -1, 0, 0, 0, 0, 0 },
                          /* vfuniforms*/ { -1, 0, 0, 0, 0, 0 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_Count, -1, 0},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_Count, -1, 0},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    
    };

static FragShaderDesc ShaderDesc_FS_FDrawableCopyPixelsMergeAlphaNoDestAlphaCopyLerp = {
    /* Type */          FragShaderDesc::FS_FDrawableCopyPixelsMergeAlphaNoDestAlphaCopyLerp,
    /* Flags */         0,
    /* pSource */       pSource_FDrawableCopyPixelsMergeAlphaNoDestAlphaCopyLerp,
    /* Uniforms */      { 
                          /* cxadd*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      { -1, 0, 0, 0, 0, 0 },
                          /* mvp*/        { -1, 0, 0, 0, 0, 0 },
                          /* offset*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor2*/    { -1, 0, 0, 0, 0, 0 },
                          /* srctex*/     { -1, 0, 0, 0, 0, 0 },
                          /* srctexscale*/{ -1, 0, 0, 0, 0, 0 },
                          /* tex*/        {0, 1032, 1, 2, 1, 0 },
                          /* texgen*/     { -1, 0, 0, 0, 0, 0 },
                          /* texscale*/   { -1, 0, 0, 0, 0, 0 },
                          /* vfmuniforms*/{ -1, 0, 0, 0, 0, 0 },
                          /* vfuniforms*/ { -1, 0, 0, 0, 0, 0 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_Count, -1, 0},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_Count, -1, 0},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    
    };

static FragShaderDesc ShaderDesc_FS_FDrawableCopyPixelsAlpha = {
    /* Type */          FragShaderDesc::FS_FDrawableCopyPixelsAlpha,
    /* Flags */         0,
    /* pSource */       pSource_FDrawableCopyPixelsAlpha,
    /* Uniforms */      { 
                          /* cxadd*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      { -1, 0, 0, 0, 0, 0 },
                          /* mvp*/        { -1, 0, 0, 0, 0, 0 },
                          /* offset*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor2*/    { -1, 0, 0, 0, 0, 0 },
                          /* srctex*/     { -1, 0, 0, 0, 0, 0 },
                          /* srctexscale*/{ -1, 0, 0, 0, 0, 0 },
                          /* tex*/        {0, 1032, 1, 3, 1, 0 },
                          /* texgen*/     { -1, 0, 0, 0, 0, 0 },
                          /* texscale*/   { -1, 0, 0, 0, 0, 0 },
                          /* vfmuniforms*/{ -1, 0, 0, 0, 0, 0 },
                          /* vfuniforms*/ { -1, 0, 0, 0, 0, 0 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_Count, -1, 0},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_Count, -1, 0},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    
    };

static FragShaderDesc ShaderDesc_FS_FDrawableCopyPixelsAlphaCopyLerp = {
    /* Type */          FragShaderDesc::FS_FDrawableCopyPixelsAlphaCopyLerp,
    /* Flags */         0,
    /* pSource */       pSource_FDrawableCopyPixelsAlphaCopyLerp,
    /* Uniforms */      { 
                          /* cxadd*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      { -1, 0, 0, 0, 0, 0 },
                          /* mvp*/        { -1, 0, 0, 0, 0, 0 },
                          /* offset*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor2*/    { -1, 0, 0, 0, 0, 0 },
                          /* srctex*/     { -1, 0, 0, 0, 0, 0 },
                          /* srctexscale*/{ -1, 0, 0, 0, 0, 0 },
                          /* tex*/        {0, 1032, 1, 3, 1, 0 },
                          /* texgen*/     { -1, 0, 0, 0, 0, 0 },
                          /* texscale*/   { -1, 0, 0, 0, 0, 0 },
                          /* vfmuniforms*/{ -1, 0, 0, 0, 0, 0 },
                          /* vfuniforms*/ { -1, 0, 0, 0, 0, 0 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_Count, -1, 0},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_Count, -1, 0},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    
    };

static FragShaderDesc ShaderDesc_FS_FDrawableCopyPixelsAlphaNoDestAlpha = {
    /* Type */          FragShaderDesc::FS_FDrawableCopyPixelsAlphaNoDestAlpha,
    /* Flags */         0,
    /* pSource */       pSource_FDrawableCopyPixelsAlphaNoDestAlpha,
    /* Uniforms */      { 
                          /* cxadd*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      { -1, 0, 0, 0, 0, 0 },
                          /* mvp*/        { -1, 0, 0, 0, 0, 0 },
                          /* offset*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor2*/    { -1, 0, 0, 0, 0, 0 },
                          /* srctex*/     { -1, 0, 0, 0, 0, 0 },
                          /* srctexscale*/{ -1, 0, 0, 0, 0, 0 },
                          /* tex*/        {0, 1032, 1, 3, 1, 0 },
                          /* texgen*/     { -1, 0, 0, 0, 0, 0 },
                          /* texscale*/   { -1, 0, 0, 0, 0, 0 },
                          /* vfmuniforms*/{ -1, 0, 0, 0, 0, 0 },
                          /* vfuniforms*/ { -1, 0, 0, 0, 0, 0 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_Count, -1, 0},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_Count, -1, 0},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    
    };

static FragShaderDesc ShaderDesc_FS_FDrawableCopyPixelsAlphaNoDestAlphaCopyLerp = {
    /* Type */          FragShaderDesc::FS_FDrawableCopyPixelsAlphaNoDestAlphaCopyLerp,
    /* Flags */         0,
    /* pSource */       pSource_FDrawableCopyPixelsAlphaNoDestAlphaCopyLerp,
    /* Uniforms */      { 
                          /* cxadd*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      { -1, 0, 0, 0, 0, 0 },
                          /* mvp*/        { -1, 0, 0, 0, 0, 0 },
                          /* offset*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor2*/    { -1, 0, 0, 0, 0, 0 },
                          /* srctex*/     { -1, 0, 0, 0, 0, 0 },
                          /* srctexscale*/{ -1, 0, 0, 0, 0, 0 },
                          /* tex*/        {0, 1032, 1, 3, 1, 0 },
                          /* texgen*/     { -1, 0, 0, 0, 0, 0 },
                          /* texscale*/   { -1, 0, 0, 0, 0, 0 },
                          /* vfmuniforms*/{ -1, 0, 0, 0, 0, 0 },
                          /* vfuniforms*/ { -1, 0, 0, 0, 0, 0 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_Count, -1, 0},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_Count, -1, 0},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    
    };

static FragShaderDesc ShaderDesc_FS_FDrawableCopyPixelsAlphaMergeAlpha = {
    /* Type */          FragShaderDesc::FS_FDrawableCopyPixelsAlphaMergeAlpha,
    /* Flags */         0,
    /* pSource */       pSource_FDrawableCopyPixelsAlphaMergeAlpha,
    /* Uniforms */      { 
                          /* cxadd*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      { -1, 0, 0, 0, 0, 0 },
                          /* mvp*/        { -1, 0, 0, 0, 0, 0 },
                          /* offset*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor2*/    { -1, 0, 0, 0, 0, 0 },
                          /* srctex*/     { -1, 0, 0, 0, 0, 0 },
                          /* srctexscale*/{ -1, 0, 0, 0, 0, 0 },
                          /* tex*/        {0, 1032, 1, 3, 1, 0 },
                          /* texgen*/     { -1, 0, 0, 0, 0, 0 },
                          /* texscale*/   { -1, 0, 0, 0, 0, 0 },
                          /* vfmuniforms*/{ -1, 0, 0, 0, 0, 0 },
                          /* vfuniforms*/ { -1, 0, 0, 0, 0, 0 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_Count, -1, 0},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_Count, -1, 0},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    
    };

static FragShaderDesc ShaderDesc_FS_FDrawableCopyPixelsAlphaMergeAlphaCopyLerp = {
    /* Type */          FragShaderDesc::FS_FDrawableCopyPixelsAlphaMergeAlphaCopyLerp,
    /* Flags */         0,
    /* pSource */       pSource_FDrawableCopyPixelsAlphaMergeAlphaCopyLerp,
    /* Uniforms */      { 
                          /* cxadd*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      { -1, 0, 0, 0, 0, 0 },
                          /* mvp*/        { -1, 0, 0, 0, 0, 0 },
                          /* offset*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor2*/    { -1, 0, 0, 0, 0, 0 },
                          /* srctex*/     { -1, 0, 0, 0, 0, 0 },
                          /* srctexscale*/{ -1, 0, 0, 0, 0, 0 },
                          /* tex*/        {0, 1032, 1, 3, 1, 0 },
                          /* texgen*/     { -1, 0, 0, 0, 0, 0 },
                          /* texscale*/   { -1, 0, 0, 0, 0, 0 },
                          /* vfmuniforms*/{ -1, 0, 0, 0, 0, 0 },
                          /* vfuniforms*/ { -1, 0, 0, 0, 0, 0 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_Count, -1, 0},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_Count, -1, 0},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    
    };

static FragShaderDesc ShaderDesc_FS_FDrawableCopyPixelsAlphaMergeAlphaNoDestAlpha = {
    /* Type */          FragShaderDesc::FS_FDrawableCopyPixelsAlphaMergeAlphaNoDestAlpha,
    /* Flags */         0,
    /* pSource */       pSource_FDrawableCopyPixelsAlphaMergeAlphaNoDestAlpha,
    /* Uniforms */      { 
                          /* cxadd*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      { -1, 0, 0, 0, 0, 0 },
                          /* mvp*/        { -1, 0, 0, 0, 0, 0 },
                          /* offset*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor2*/    { -1, 0, 0, 0, 0, 0 },
                          /* srctex*/     { -1, 0, 0, 0, 0, 0 },
                          /* srctexscale*/{ -1, 0, 0, 0, 0, 0 },
                          /* tex*/        {0, 1032, 1, 3, 1, 0 },
                          /* texgen*/     { -1, 0, 0, 0, 0, 0 },
                          /* texscale*/   { -1, 0, 0, 0, 0, 0 },
                          /* vfmuniforms*/{ -1, 0, 0, 0, 0, 0 },
                          /* vfuniforms*/ { -1, 0, 0, 0, 0, 0 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_Count, -1, 0},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_Count, -1, 0},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    
    };

static FragShaderDesc ShaderDesc_FS_FDrawableCopyPixelsAlphaMergeAlphaNoDestAlphaCopyLerp = {
    /* Type */          FragShaderDesc::FS_FDrawableCopyPixelsAlphaMergeAlphaNoDestAlphaCopyLerp,
    /* Flags */         0,
    /* pSource */       pSource_FDrawableCopyPixelsAlphaMergeAlphaNoDestAlphaCopyLerp,
    /* Uniforms */      { 
                          /* cxadd*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      { -1, 0, 0, 0, 0, 0 },
                          /* mvp*/        { -1, 0, 0, 0, 0, 0 },
                          /* offset*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor2*/    { -1, 0, 0, 0, 0, 0 },
                          /* srctex*/     { -1, 0, 0, 0, 0, 0 },
                          /* srctexscale*/{ -1, 0, 0, 0, 0, 0 },
                          /* tex*/        {0, 1032, 1, 3, 1, 0 },
                          /* texgen*/     { -1, 0, 0, 0, 0, 0 },
                          /* texscale*/   { -1, 0, 0, 0, 0, 0 },
                          /* vfmuniforms*/{ -1, 0, 0, 0, 0, 0 },
                          /* vfuniforms*/ { -1, 0, 0, 0, 0, 0 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_Count, -1, 0},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_Count, -1, 0},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    
    };

static FragShaderDesc ShaderDesc_FS_FDrawableMerge = {
    /* Type */          FragShaderDesc::FS_FDrawableMerge,
    /* Flags */         0,
    /* pSource */       pSource_FDrawableMerge,
    /* Uniforms */      { 
                          /* cxadd*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul*/      {0, 1032, 16, 16, 16, 0 },
                          /* cxmul1*/     {4, 1048, 16, 16, 16, 0 },
                          /* fsize*/      { -1, 0, 0, 0, 0, 0 },
                          /* mvp*/        { -1, 0, 0, 0, 0, 0 },
                          /* offset*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor2*/    { -1, 0, 0, 0, 0, 0 },
                          /* srctex*/     { -1, 0, 0, 0, 0, 0 },
                          /* srctexscale*/{ -1, 0, 0, 0, 0, 0 },
                          /* tex*/        {0, 1032, 1, 2, 1, 0 },
                          /* texgen*/     { -1, 0, 0, 0, 0, 0 },
                          /* texscale*/   { -1, 0, 0, 0, 0, 0 },
                          /* vfmuniforms*/{ -1, 0, 0, 0, 0, 0 },
                          /* vfuniforms*/ { -1, 0, 0, 0, 0, 0 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_Count, -1, 0},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_Count, -1, 0},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    
    };

static FragShaderDesc ShaderDesc_FS_FDrawableCompare = {
    /* Type */          FragShaderDesc::FS_FDrawableCompare,
    /* Flags */         0,
    /* pSource */       pSource_FDrawableCompare,
    /* Uniforms */      { 
                          /* cxadd*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      { -1, 0, 0, 0, 0, 0 },
                          /* mvp*/        { -1, 0, 0, 0, 0, 0 },
                          /* offset*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor2*/    { -1, 0, 0, 0, 0, 0 },
                          /* srctex*/     { -1, 0, 0, 0, 0, 0 },
                          /* srctexscale*/{ -1, 0, 0, 0, 0, 0 },
                          /* tex*/        {0, 1032, 1, 2, 1, 0 },
                          /* texgen*/     { -1, 0, 0, 0, 0, 0 },
                          /* texscale*/   { -1, 0, 0, 0, 0, 0 },
                          /* vfmuniforms*/{ -1, 0, 0, 0, 0, 0 },
                          /* vfuniforms*/ { -1, 0, 0, 0, 0, 0 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_Count, -1, 0},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_Count, -1, 0},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    
    };

static FragShaderDesc ShaderDesc_FS_FDrawablePaletteMap = {
    /* Type */          FragShaderDesc::FS_FDrawablePaletteMap,
    /* Flags */         0,
    /* pSource */       pSource_FDrawablePaletteMap,
    /* Uniforms */      { 
                          /* cxadd*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul*/      { -1, 0, 0, 0, 0, 0 },
                          /* cxmul1*/     { -1, 0, 0, 0, 0, 0 },
                          /* fsize*/      { -1, 0, 0, 0, 0, 0 },
                          /* mvp*/        { -1, 0, 0, 0, 0, 0 },
                          /* offset*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor*/     { -1, 0, 0, 0, 0, 0 },
                          /* scolor2*/    { -1, 0, 0, 0, 0, 0 },
                          /* srctex*/     {0, 1032, 1, 1, 1, 0 },
                          /* srctexscale*/{ -1, 0, 0, 0, 0, 0 },
                          /* tex*/        {1, 1036, 1, 1, 1, 0 },
                          /* texgen*/     { -1, 0, 0, 0, 0, 0 },
                          /* texscale*/   { -1, 0, 0, 0, 0, 0 },
                          /* vfmuniforms*/{ -1, 0, 0, 0, 0, 0 },
                          /* vfuniforms*/ { -1, 0, 0, 0, 0, 0 },
    },
    /* BatchUniforms */  {
                          /* cxadd*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul*/       {Uniform::SU_Count, -1, 0},
                          /* cxmul1*/      {Uniform::SU_Count, -1, 0},
                          /* fsize*/       {Uniform::SU_Count, -1, 0},
                          /* mvp*/         {Uniform::SU_Count, -1, 0},
                          /* offset*/      {Uniform::SU_Count, -1, 0},
                          /* scolor*/      {Uniform::SU_Count, -1, 0},
                          /* scolor2*/     {Uniform::SU_Count, -1, 0},
                          /* srctex*/      {Uniform::SU_Count, -1, 0},
                          /* srctexscale*/ {Uniform::SU_Count, -1, 0},
                          /* tex*/         {Uniform::SU_Count, -1, 0},
                          /* texgen*/      {Uniform::SU_Count, -1, 0},
                          /* texscale*/    {Uniform::SU_Count, -1, 0},
                          /* vfmuniforms*/ {Uniform::SU_Count, -1, 0},
                          /* vfuniforms*/  {Uniform::SU_Count, -1, 0},
                          },
    
    };

const FragShaderDesc* FragShaderDesc::Descs[FSI_Count] = {
    0,
    &ShaderDesc_FS_FTexTG,                                       // 1
    &ShaderDesc_FS_FTexTGMul,                                    // 2
    &ShaderDesc_FS_FTexTGEAlpha,                                 // 3
    &ShaderDesc_FS_FTexTGEAlphaMul,                              // 4
    &ShaderDesc_FS_FTexTGCxform,                                 // 5
    &ShaderDesc_FS_FTexTGCxformMul,                              // 6
    &ShaderDesc_FS_FTexTGCxformEAlpha,                           // 7
    &ShaderDesc_FS_FTexTGCxformEAlphaMul,                        // 8
    &ShaderDesc_FS_FVertex,                                      // 9
    &ShaderDesc_FS_FVertexMul,                                   // 10
    &ShaderDesc_FS_FVertexEAlpha,                                // 11
    &ShaderDesc_FS_FVertexEAlphaMul,                             // 12
    &ShaderDesc_FS_FVertexCxform,                                // 13
    &ShaderDesc_FS_FVertexCxformMul,                             // 14
    &ShaderDesc_FS_FVertexCxformEAlpha,                          // 15
    &ShaderDesc_FS_FVertexCxformEAlphaMul,                       // 16
    &ShaderDesc_FS_FTexTGTexTG,                                  // 17
    &ShaderDesc_FS_FTexTGTexTGMul,                               // 18
    &ShaderDesc_FS_FTexTGTexTGEAlpha,                            // 19
    &ShaderDesc_FS_FTexTGTexTGEAlphaMul,                         // 20
    &ShaderDesc_FS_FTexTGTexTGCxform,                            // 21
    &ShaderDesc_FS_FTexTGTexTGCxformMul,                         // 22
    &ShaderDesc_FS_FTexTGTexTGCxformEAlpha,                      // 23
    &ShaderDesc_FS_FTexTGTexTGCxformEAlphaMul,                   // 24
    &ShaderDesc_FS_FTexTGVertex,                                 // 25
    &ShaderDesc_FS_FTexTGVertexMul,                              // 26
    &ShaderDesc_FS_FTexTGVertexEAlpha,                           // 27
    &ShaderDesc_FS_FTexTGVertexEAlphaMul,                        // 28
    &ShaderDesc_FS_FTexTGVertexCxform,                           // 29
    &ShaderDesc_FS_FTexTGVertexCxformMul,                        // 30
    &ShaderDesc_FS_FTexTGVertexCxformEAlpha,                     // 31
    &ShaderDesc_FS_FTexTGVertexCxformEAlphaMul,                  // 32
    &ShaderDesc_FS_FBatchTexTG,                                  // 33
    &ShaderDesc_FS_FBatchTexTGMul,                               // 34
    &ShaderDesc_FS_FBatchTexTGEAlpha,                            // 35
    &ShaderDesc_FS_FBatchTexTGEAlphaMul,                         // 36
    &ShaderDesc_FS_FBatchTexTGCxform,                            // 37
    &ShaderDesc_FS_FBatchTexTGCxformMul,                         // 38
    &ShaderDesc_FS_FBatchTexTGCxformEAlpha,                      // 39
    &ShaderDesc_FS_FBatchTexTGCxformEAlphaMul,                   // 40
    &ShaderDesc_FS_FBatchVertex,                                 // 41
    &ShaderDesc_FS_FBatchVertexMul,                              // 42
    &ShaderDesc_FS_FBatchVertexEAlpha,                           // 43
    &ShaderDesc_FS_FBatchVertexEAlphaMul,                        // 44
    &ShaderDesc_FS_FBatchVertexCxform,                           // 45
    &ShaderDesc_FS_FBatchVertexCxformMul,                        // 46
    &ShaderDesc_FS_FBatchVertexCxformEAlpha,                     // 47
    &ShaderDesc_FS_FBatchVertexCxformEAlphaMul,                  // 48
    &ShaderDesc_FS_FBatchTexTGTexTG,                             // 49
    &ShaderDesc_FS_FBatchTexTGTexTGMul,                          // 50
    &ShaderDesc_FS_FBatchTexTGTexTGEAlpha,                       // 51
    &ShaderDesc_FS_FBatchTexTGTexTGEAlphaMul,                    // 52
    &ShaderDesc_FS_FBatchTexTGTexTGCxform,                       // 53
    &ShaderDesc_FS_FBatchTexTGTexTGCxformMul,                    // 54
    &ShaderDesc_FS_FBatchTexTGTexTGCxformEAlpha,                 // 55
    &ShaderDesc_FS_FBatchTexTGTexTGCxformEAlphaMul,              // 56
    &ShaderDesc_FS_FBatchTexTGVertex,                            // 57
    &ShaderDesc_FS_FBatchTexTGVertexMul,                         // 58
    &ShaderDesc_FS_FBatchTexTGVertexEAlpha,                      // 59
    &ShaderDesc_FS_FBatchTexTGVertexEAlphaMul,                   // 60
    &ShaderDesc_FS_FBatchTexTGVertexCxform,                      // 61
    &ShaderDesc_FS_FBatchTexTGVertexCxformMul,                   // 62
    &ShaderDesc_FS_FBatchTexTGVertexCxformEAlpha,                // 63
    &ShaderDesc_FS_FBatchTexTGVertexCxformEAlphaMul,             // 64
    &ShaderDesc_FS_FInstancedTexTG,                              // 65
    &ShaderDesc_FS_FInstancedTexTGMul,                           // 66
    &ShaderDesc_FS_FInstancedTexTGEAlpha,                        // 67
    &ShaderDesc_FS_FInstancedTexTGEAlphaMul,                     // 68
    &ShaderDesc_FS_FInstancedTexTGCxform,                        // 69
    &ShaderDesc_FS_FInstancedTexTGCxformMul,                     // 70
    &ShaderDesc_FS_FInstancedTexTGCxformEAlpha,                  // 71
    &ShaderDesc_FS_FInstancedTexTGCxformEAlphaMul,               // 72
    &ShaderDesc_FS_FInstancedVertex,                             // 73
    &ShaderDesc_FS_FInstancedVertexMul,                          // 74
    &ShaderDesc_FS_FInstancedVertexEAlpha,                       // 75
    &ShaderDesc_FS_FInstancedVertexEAlphaMul,                    // 76
    &ShaderDesc_FS_FInstancedVertexCxform,                       // 77
    &ShaderDesc_FS_FInstancedVertexCxformMul,                    // 78
    &ShaderDesc_FS_FInstancedVertexCxformEAlpha,                 // 79
    &ShaderDesc_FS_FInstancedVertexCxformEAlphaMul,              // 80
    &ShaderDesc_FS_FInstancedTexTGTexTG,                         // 81
    &ShaderDesc_FS_FInstancedTexTGTexTGMul,                      // 82
    &ShaderDesc_FS_FInstancedTexTGTexTGEAlpha,                   // 83
    &ShaderDesc_FS_FInstancedTexTGTexTGEAlphaMul,                // 84
    &ShaderDesc_FS_FInstancedTexTGTexTGCxform,                   // 85
    &ShaderDesc_FS_FInstancedTexTGTexTGCxformMul,                // 86
    &ShaderDesc_FS_FInstancedTexTGTexTGCxformEAlpha,             // 87
    &ShaderDesc_FS_FInstancedTexTGTexTGCxformEAlphaMul,          // 88
    &ShaderDesc_FS_FInstancedTexTGVertex,                        // 89
    &ShaderDesc_FS_FInstancedTexTGVertexMul,                     // 90
    &ShaderDesc_FS_FInstancedTexTGVertexEAlpha,                  // 91
    &ShaderDesc_FS_FInstancedTexTGVertexEAlphaMul,               // 92
    &ShaderDesc_FS_FInstancedTexTGVertexCxform,                  // 93
    &ShaderDesc_FS_FInstancedTexTGVertexCxformMul,               // 94
    &ShaderDesc_FS_FInstancedTexTGVertexCxformEAlpha,            // 95
    &ShaderDesc_FS_FInstancedTexTGVertexCxformEAlphaMul,         // 96
    &ShaderDesc_FS_FSolid,                                       // 97
    &ShaderDesc_FS_FSolidMul,                                    // 98
    &ShaderDesc_FS_FText,                                        // 99
    &ShaderDesc_FS_FTextMul,                                     // 100
    &ShaderDesc_FS_FBatchSolid,                                  // 101
    &ShaderDesc_FS_FBatchSolidMul,                               // 102
    &ShaderDesc_FS_FBatchText,                                   // 103
    &ShaderDesc_FS_FBatchTextMul,                                // 104
    &ShaderDesc_FS_FInstancedSolid,                              // 105
    &ShaderDesc_FS_FInstancedSolidMul,                           // 106
    &ShaderDesc_FS_FInstancedText,                               // 107
    &ShaderDesc_FS_FInstancedTextMul,                            // 108
    &ShaderDesc_FS_FYUV,                                         // 109
    &ShaderDesc_FS_FYUVMul,                                      // 110
    &ShaderDesc_FS_FYUVEAlpha,                                   // 111
    &ShaderDesc_FS_FYUVEAlphaMul,                                // 112
    &ShaderDesc_FS_FYUVCxform,                                   // 113
    &ShaderDesc_FS_FYUVCxformMul,                                // 114
    &ShaderDesc_FS_FYUVCxformEAlpha,                             // 115
    &ShaderDesc_FS_FYUVCxformEAlphaMul,                          // 116
    &ShaderDesc_FS_FYUVA,                                        // 117
    &ShaderDesc_FS_FYUVAMul,                                     // 118
    &ShaderDesc_FS_FYUVAEAlpha,                                  // 119
    &ShaderDesc_FS_FYUVAEAlphaMul,                               // 120
    &ShaderDesc_FS_FYUVACxform,                                  // 121
    &ShaderDesc_FS_FYUVACxformMul,                               // 122
    &ShaderDesc_FS_FYUVACxformEAlpha,                            // 123
    &ShaderDesc_FS_FYUVACxformEAlphaMul,                         // 124
    &ShaderDesc_FS_FBatchYUV,                                    // 125
    &ShaderDesc_FS_FBatchYUVMul,                                 // 126
    &ShaderDesc_FS_FBatchYUVEAlpha,                              // 127
    &ShaderDesc_FS_FBatchYUVEAlphaMul,                           // 128
    &ShaderDesc_FS_FBatchYUVCxform,                              // 129
    &ShaderDesc_FS_FBatchYUVCxformMul,                           // 130
    &ShaderDesc_FS_FBatchYUVCxformEAlpha,                        // 131
    &ShaderDesc_FS_FBatchYUVCxformEAlphaMul,                     // 132
    &ShaderDesc_FS_FBatchYUVA,                                   // 133
    &ShaderDesc_FS_FBatchYUVAMul,                                // 134
    &ShaderDesc_FS_FBatchYUVAEAlpha,                             // 135
    &ShaderDesc_FS_FBatchYUVAEAlphaMul,                          // 136
    &ShaderDesc_FS_FBatchYUVACxform,                             // 137
    &ShaderDesc_FS_FBatchYUVACxformMul,                          // 138
    &ShaderDesc_FS_FBatchYUVACxformEAlpha,                       // 139
    &ShaderDesc_FS_FBatchYUVACxformEAlphaMul,                    // 140
    &ShaderDesc_FS_FInstancedYUV,                                // 141
    &ShaderDesc_FS_FInstancedYUVMul,                             // 142
    &ShaderDesc_FS_FInstancedYUVEAlpha,                          // 143
    &ShaderDesc_FS_FInstancedYUVEAlphaMul,                       // 144
    &ShaderDesc_FS_FInstancedYUVCxform,                          // 145
    &ShaderDesc_FS_FInstancedYUVCxformMul,                       // 146
    &ShaderDesc_FS_FInstancedYUVCxformEAlpha,                    // 147
    &ShaderDesc_FS_FInstancedYUVCxformEAlphaMul,                 // 148
    &ShaderDesc_FS_FInstancedYUVA,                               // 149
    &ShaderDesc_FS_FInstancedYUVAMul,                            // 150
    &ShaderDesc_FS_FInstancedYUVAEAlpha,                         // 151
    &ShaderDesc_FS_FInstancedYUVAEAlphaMul,                      // 152
    &ShaderDesc_FS_FInstancedYUVACxform,                         // 153
    &ShaderDesc_FS_FInstancedYUVACxformMul,                      // 154
    &ShaderDesc_FS_FInstancedYUVACxformEAlpha,                   // 155
    &ShaderDesc_FS_FInstancedYUVACxformEAlphaMul,                // 156
    &ShaderDesc_FS_FTexTGCMatrixAc,                              // 157
    &ShaderDesc_FS_FTexTGCMatrixAcMul,                           // 158
    &ShaderDesc_FS_FTexTGCMatrixAcEAlpha,                        // 159
    &ShaderDesc_FS_FTexTGCMatrixAcEAlphaMul,                     // 160
    &ShaderDesc_FS_FBox1Blur,                                    // 161
    &ShaderDesc_FS_FBox1BlurMul,                                 // 162
    &ShaderDesc_FS_FBox2Blur,                                    // 163
    &ShaderDesc_FS_FBox2BlurMul,                                 // 164
    &ShaderDesc_FS_FBox2Shadow,                                  // 165
    &ShaderDesc_FS_FBox2ShadowMul,                               // 166
    &ShaderDesc_FS_FBox2ShadowKnockout,                          // 167
    &ShaderDesc_FS_FBox2ShadowKnockoutMul,                       // 168
    &ShaderDesc_FS_FBox2InnerShadow,                             // 169
    &ShaderDesc_FS_FBox2InnerShadowMul,                          // 170
    &ShaderDesc_FS_FBox2InnerShadowKnockout,                     // 171
    &ShaderDesc_FS_FBox2InnerShadowKnockoutMul,                  // 172
    &ShaderDesc_FS_FBox2Shadowonly,                              // 173
    &ShaderDesc_FS_FBox2ShadowonlyMul,                           // 174
    &ShaderDesc_FS_FBox2ShadowHighlight,                         // 175
    &ShaderDesc_FS_FBox2ShadowHighlightMul,                      // 176
    &ShaderDesc_FS_FBox2ShadowHighlightKnockout,                 // 177
    &ShaderDesc_FS_FBox2ShadowHighlightKnockoutMul,              // 178
    &ShaderDesc_FS_FBox2InnerShadowHighlight,                    // 179
    &ShaderDesc_FS_FBox2InnerShadowHighlightMul,                 // 180
    &ShaderDesc_FS_FBox2InnerShadowHighlightKnockout,            // 181
    &ShaderDesc_FS_FBox2InnerShadowHighlightKnockoutMul,         // 182
    &ShaderDesc_FS_FBox2ShadowonlyHighlight,                     // 183
    &ShaderDesc_FS_FBox2ShadowonlyHighlightMul,                  // 184
    &ShaderDesc_FS_FDrawableCopyPixels,                          // 185
    &ShaderDesc_FS_FDrawableCopyPixelsCopyLerp,                  // 186
    &ShaderDesc_FS_FDrawableCopyPixelsNoDestAlpha,               // 187
    &ShaderDesc_FS_FDrawableCopyPixelsNoDestAlphaCopyLerp,       // 188
    &ShaderDesc_FS_FDrawableCopyPixelsMergeAlpha,                // 189
    &ShaderDesc_FS_FDrawableCopyPixelsMergeAlphaCopyLerp,        // 190
    &ShaderDesc_FS_FDrawableCopyPixelsMergeAlphaNoDestAlpha,     // 191
    &ShaderDesc_FS_FDrawableCopyPixelsMergeAlphaNoDestAlphaCopyLerp, // 192
    &ShaderDesc_FS_FDrawableCopyPixelsAlpha,                     // 193
    &ShaderDesc_FS_FDrawableCopyPixelsAlphaCopyLerp,             // 194
    &ShaderDesc_FS_FDrawableCopyPixelsAlphaNoDestAlpha,          // 195
    &ShaderDesc_FS_FDrawableCopyPixelsAlphaNoDestAlphaCopyLerp,  // 196
    &ShaderDesc_FS_FDrawableCopyPixelsAlphaMergeAlpha,           // 197
    &ShaderDesc_FS_FDrawableCopyPixelsAlphaMergeAlphaCopyLerp,   // 198
    &ShaderDesc_FS_FDrawableCopyPixelsAlphaMergeAlphaNoDestAlpha, // 199
    &ShaderDesc_FS_FDrawableCopyPixelsAlphaMergeAlphaNoDestAlphaCopyLerp, // 200
    &ShaderDesc_FS_FDrawableMerge,                               // 201
    &ShaderDesc_FS_FDrawableCompare,                             // 202
    &ShaderDesc_FS_FDrawablePaletteMap,                          // 203
};

const FragShaderDesc* FragShaderDesc::GetDesc(ShaderType shader)
{
    return Descs[GetShaderIndex(shader)];
};

FragShaderDesc::ShaderIndex FragShaderDesc::GetShaderIndex(ShaderType shader)
{
switch(shader)
{
    case FS_FTexTG:                                                  return FSI_FTexTG;
    case FS_FTexTGMul:                                               return FSI_FTexTGMul;
    case FS_FBatchTexTG:                                             return FSI_FBatchTexTG;
    case FS_FBatchTexTGMul:                                          return FSI_FBatchTexTGMul;
    case FS_FInstancedTexTG:                                         return FSI_FInstancedTexTG;
    case FS_FInstancedTexTGMul:                                      return FSI_FInstancedTexTGMul;
    case FS_FPosition3dTexTG:                                        return FSI_FPosition3dTexTG;
    case FS_FPosition3dTexTGMul:                                     return FSI_FPosition3dTexTGMul;
    case FS_FBatchPosition3dTexTG:                                   return FSI_FBatchPosition3dTexTG;
    case FS_FBatchPosition3dTexTGMul:                                return FSI_FBatchPosition3dTexTGMul;
    case FS_FInstancedPosition3dTexTG:                               return FSI_FInstancedPosition3dTexTG;
    case FS_FInstancedPosition3dTexTGMul:                            return FSI_FInstancedPosition3dTexTGMul;
    case FS_FTexTGCxform:                                            return FSI_FTexTGCxform;
    case FS_FTexTGCxformMul:                                         return FSI_FTexTGCxformMul;
    case FS_FBatchTexTGCxform:                                       return FSI_FBatchTexTGCxform;
    case FS_FBatchTexTGCxformMul:                                    return FSI_FBatchTexTGCxformMul;
    case FS_FInstancedTexTGCxform:                                   return FSI_FInstancedTexTGCxform;
    case FS_FInstancedTexTGCxformMul:                                return FSI_FInstancedTexTGCxformMul;
    case FS_FPosition3dTexTGCxform:                                  return FSI_FPosition3dTexTGCxform;
    case FS_FPosition3dTexTGCxformMul:                               return FSI_FPosition3dTexTGCxformMul;
    case FS_FBatchPosition3dTexTGCxform:                             return FSI_FBatchPosition3dTexTGCxform;
    case FS_FBatchPosition3dTexTGCxformMul:                          return FSI_FBatchPosition3dTexTGCxformMul;
    case FS_FInstancedPosition3dTexTGCxform:                         return FSI_FInstancedPosition3dTexTGCxform;
    case FS_FInstancedPosition3dTexTGCxformMul:                      return FSI_FInstancedPosition3dTexTGCxformMul;
    case FS_FTexTGEAlpha:                                            return FSI_FTexTGEAlpha;
    case FS_FTexTGEAlphaMul:                                         return FSI_FTexTGEAlphaMul;
    case FS_FBatchTexTGEAlpha:                                       return FSI_FBatchTexTGEAlpha;
    case FS_FBatchTexTGEAlphaMul:                                    return FSI_FBatchTexTGEAlphaMul;
    case FS_FInstancedTexTGEAlpha:                                   return FSI_FInstancedTexTGEAlpha;
    case FS_FInstancedTexTGEAlphaMul:                                return FSI_FInstancedTexTGEAlphaMul;
    case FS_FPosition3dTexTGEAlpha:                                  return FSI_FPosition3dTexTGEAlpha;
    case FS_FPosition3dTexTGEAlphaMul:                               return FSI_FPosition3dTexTGEAlphaMul;
    case FS_FBatchPosition3dTexTGEAlpha:                             return FSI_FBatchPosition3dTexTGEAlpha;
    case FS_FBatchPosition3dTexTGEAlphaMul:                          return FSI_FBatchPosition3dTexTGEAlphaMul;
    case FS_FInstancedPosition3dTexTGEAlpha:                         return FSI_FInstancedPosition3dTexTGEAlpha;
    case FS_FInstancedPosition3dTexTGEAlphaMul:                      return FSI_FInstancedPosition3dTexTGEAlphaMul;
    case FS_FTexTGCxformEAlpha:                                      return FSI_FTexTGCxformEAlpha;
    case FS_FTexTGCxformEAlphaMul:                                   return FSI_FTexTGCxformEAlphaMul;
    case FS_FBatchTexTGCxformEAlpha:                                 return FSI_FBatchTexTGCxformEAlpha;
    case FS_FBatchTexTGCxformEAlphaMul:                              return FSI_FBatchTexTGCxformEAlphaMul;
    case FS_FInstancedTexTGCxformEAlpha:                             return FSI_FInstancedTexTGCxformEAlpha;
    case FS_FInstancedTexTGCxformEAlphaMul:                          return FSI_FInstancedTexTGCxformEAlphaMul;
    case FS_FPosition3dTexTGCxformEAlpha:                            return FSI_FPosition3dTexTGCxformEAlpha;
    case FS_FPosition3dTexTGCxformEAlphaMul:                         return FSI_FPosition3dTexTGCxformEAlphaMul;
    case FS_FBatchPosition3dTexTGCxformEAlpha:                       return FSI_FBatchPosition3dTexTGCxformEAlpha;
    case FS_FBatchPosition3dTexTGCxformEAlphaMul:                    return FSI_FBatchPosition3dTexTGCxformEAlphaMul;
    case FS_FInstancedPosition3dTexTGCxformEAlpha:                   return FSI_FInstancedPosition3dTexTGCxformEAlpha;
    case FS_FInstancedPosition3dTexTGCxformEAlphaMul:                return FSI_FInstancedPosition3dTexTGCxformEAlphaMul;
    case FS_FVertex:                                                 return FSI_FVertex;
    case FS_FVertexMul:                                              return FSI_FVertexMul;
    case FS_FBatchVertex:                                            return FSI_FBatchVertex;
    case FS_FBatchVertexMul:                                         return FSI_FBatchVertexMul;
    case FS_FInstancedVertex:                                        return FSI_FInstancedVertex;
    case FS_FInstancedVertexMul:                                     return FSI_FInstancedVertexMul;
    case FS_FPosition3dVertex:                                       return FSI_FPosition3dVertex;
    case FS_FPosition3dVertexMul:                                    return FSI_FPosition3dVertexMul;
    case FS_FBatchPosition3dVertex:                                  return FSI_FBatchPosition3dVertex;
    case FS_FBatchPosition3dVertexMul:                               return FSI_FBatchPosition3dVertexMul;
    case FS_FInstancedPosition3dVertex:                              return FSI_FInstancedPosition3dVertex;
    case FS_FInstancedPosition3dVertexMul:                           return FSI_FInstancedPosition3dVertexMul;
    case FS_FVertexCxform:                                           return FSI_FVertexCxform;
    case FS_FVertexCxformMul:                                        return FSI_FVertexCxformMul;
    case FS_FBatchVertexCxform:                                      return FSI_FBatchVertexCxform;
    case FS_FBatchVertexCxformMul:                                   return FSI_FBatchVertexCxformMul;
    case FS_FInstancedVertexCxform:                                  return FSI_FInstancedVertexCxform;
    case FS_FInstancedVertexCxformMul:                               return FSI_FInstancedVertexCxformMul;
    case FS_FPosition3dVertexCxform:                                 return FSI_FPosition3dVertexCxform;
    case FS_FPosition3dVertexCxformMul:                              return FSI_FPosition3dVertexCxformMul;
    case FS_FBatchPosition3dVertexCxform:                            return FSI_FBatchPosition3dVertexCxform;
    case FS_FBatchPosition3dVertexCxformMul:                         return FSI_FBatchPosition3dVertexCxformMul;
    case FS_FInstancedPosition3dVertexCxform:                        return FSI_FInstancedPosition3dVertexCxform;
    case FS_FInstancedPosition3dVertexCxformMul:                     return FSI_FInstancedPosition3dVertexCxformMul;
    case FS_FVertexEAlpha:                                           return FSI_FVertexEAlpha;
    case FS_FVertexEAlphaMul:                                        return FSI_FVertexEAlphaMul;
    case FS_FBatchVertexEAlpha:                                      return FSI_FBatchVertexEAlpha;
    case FS_FBatchVertexEAlphaMul:                                   return FSI_FBatchVertexEAlphaMul;
    case FS_FInstancedVertexEAlpha:                                  return FSI_FInstancedVertexEAlpha;
    case FS_FInstancedVertexEAlphaMul:                               return FSI_FInstancedVertexEAlphaMul;
    case FS_FPosition3dVertexEAlpha:                                 return FSI_FPosition3dVertexEAlpha;
    case FS_FPosition3dVertexEAlphaMul:                              return FSI_FPosition3dVertexEAlphaMul;
    case FS_FBatchPosition3dVertexEAlpha:                            return FSI_FBatchPosition3dVertexEAlpha;
    case FS_FBatchPosition3dVertexEAlphaMul:                         return FSI_FBatchPosition3dVertexEAlphaMul;
    case FS_FInstancedPosition3dVertexEAlpha:                        return FSI_FInstancedPosition3dVertexEAlpha;
    case FS_FInstancedPosition3dVertexEAlphaMul:                     return FSI_FInstancedPosition3dVertexEAlphaMul;
    case FS_FVertexCxformEAlpha:                                     return FSI_FVertexCxformEAlpha;
    case FS_FVertexCxformEAlphaMul:                                  return FSI_FVertexCxformEAlphaMul;
    case FS_FBatchVertexCxformEAlpha:                                return FSI_FBatchVertexCxformEAlpha;
    case FS_FBatchVertexCxformEAlphaMul:                             return FSI_FBatchVertexCxformEAlphaMul;
    case FS_FInstancedVertexCxformEAlpha:                            return FSI_FInstancedVertexCxformEAlpha;
    case FS_FInstancedVertexCxformEAlphaMul:                         return FSI_FInstancedVertexCxformEAlphaMul;
    case FS_FPosition3dVertexCxformEAlpha:                           return FSI_FPosition3dVertexCxformEAlpha;
    case FS_FPosition3dVertexCxformEAlphaMul:                        return FSI_FPosition3dVertexCxformEAlphaMul;
    case FS_FBatchPosition3dVertexCxformEAlpha:                      return FSI_FBatchPosition3dVertexCxformEAlpha;
    case FS_FBatchPosition3dVertexCxformEAlphaMul:                   return FSI_FBatchPosition3dVertexCxformEAlphaMul;
    case FS_FInstancedPosition3dVertexCxformEAlpha:                  return FSI_FInstancedPosition3dVertexCxformEAlpha;
    case FS_FInstancedPosition3dVertexCxformEAlphaMul:               return FSI_FInstancedPosition3dVertexCxformEAlphaMul;
    case FS_FTexTGTexTG:                                             return FSI_FTexTGTexTG;
    case FS_FTexTGTexTGMul:                                          return FSI_FTexTGTexTGMul;
    case FS_FBatchTexTGTexTG:                                        return FSI_FBatchTexTGTexTG;
    case FS_FBatchTexTGTexTGMul:                                     return FSI_FBatchTexTGTexTGMul;
    case FS_FInstancedTexTGTexTG:                                    return FSI_FInstancedTexTGTexTG;
    case FS_FInstancedTexTGTexTGMul:                                 return FSI_FInstancedTexTGTexTGMul;
    case FS_FPosition3dTexTGTexTG:                                   return FSI_FPosition3dTexTGTexTG;
    case FS_FPosition3dTexTGTexTGMul:                                return FSI_FPosition3dTexTGTexTGMul;
    case FS_FBatchPosition3dTexTGTexTG:                              return FSI_FBatchPosition3dTexTGTexTG;
    case FS_FBatchPosition3dTexTGTexTGMul:                           return FSI_FBatchPosition3dTexTGTexTGMul;
    case FS_FInstancedPosition3dTexTGTexTG:                          return FSI_FInstancedPosition3dTexTGTexTG;
    case FS_FInstancedPosition3dTexTGTexTGMul:                       return FSI_FInstancedPosition3dTexTGTexTGMul;
    case FS_FTexTGTexTGCxform:                                       return FSI_FTexTGTexTGCxform;
    case FS_FTexTGTexTGCxformMul:                                    return FSI_FTexTGTexTGCxformMul;
    case FS_FBatchTexTGTexTGCxform:                                  return FSI_FBatchTexTGTexTGCxform;
    case FS_FBatchTexTGTexTGCxformMul:                               return FSI_FBatchTexTGTexTGCxformMul;
    case FS_FInstancedTexTGTexTGCxform:                              return FSI_FInstancedTexTGTexTGCxform;
    case FS_FInstancedTexTGTexTGCxformMul:                           return FSI_FInstancedTexTGTexTGCxformMul;
    case FS_FPosition3dTexTGTexTGCxform:                             return FSI_FPosition3dTexTGTexTGCxform;
    case FS_FPosition3dTexTGTexTGCxformMul:                          return FSI_FPosition3dTexTGTexTGCxformMul;
    case FS_FBatchPosition3dTexTGTexTGCxform:                        return FSI_FBatchPosition3dTexTGTexTGCxform;
    case FS_FBatchPosition3dTexTGTexTGCxformMul:                     return FSI_FBatchPosition3dTexTGTexTGCxformMul;
    case FS_FInstancedPosition3dTexTGTexTGCxform:                    return FSI_FInstancedPosition3dTexTGTexTGCxform;
    case FS_FInstancedPosition3dTexTGTexTGCxformMul:                 return FSI_FInstancedPosition3dTexTGTexTGCxformMul;
    case FS_FTexTGTexTGEAlpha:                                       return FSI_FTexTGTexTGEAlpha;
    case FS_FTexTGTexTGEAlphaMul:                                    return FSI_FTexTGTexTGEAlphaMul;
    case FS_FBatchTexTGTexTGEAlpha:                                  return FSI_FBatchTexTGTexTGEAlpha;
    case FS_FBatchTexTGTexTGEAlphaMul:                               return FSI_FBatchTexTGTexTGEAlphaMul;
    case FS_FInstancedTexTGTexTGEAlpha:                              return FSI_FInstancedTexTGTexTGEAlpha;
    case FS_FInstancedTexTGTexTGEAlphaMul:                           return FSI_FInstancedTexTGTexTGEAlphaMul;
    case FS_FPosition3dTexTGTexTGEAlpha:                             return FSI_FPosition3dTexTGTexTGEAlpha;
    case FS_FPosition3dTexTGTexTGEAlphaMul:                          return FSI_FPosition3dTexTGTexTGEAlphaMul;
    case FS_FBatchPosition3dTexTGTexTGEAlpha:                        return FSI_FBatchPosition3dTexTGTexTGEAlpha;
    case FS_FBatchPosition3dTexTGTexTGEAlphaMul:                     return FSI_FBatchPosition3dTexTGTexTGEAlphaMul;
    case FS_FInstancedPosition3dTexTGTexTGEAlpha:                    return FSI_FInstancedPosition3dTexTGTexTGEAlpha;
    case FS_FInstancedPosition3dTexTGTexTGEAlphaMul:                 return FSI_FInstancedPosition3dTexTGTexTGEAlphaMul;
    case FS_FTexTGTexTGCxformEAlpha:                                 return FSI_FTexTGTexTGCxformEAlpha;
    case FS_FTexTGTexTGCxformEAlphaMul:                              return FSI_FTexTGTexTGCxformEAlphaMul;
    case FS_FBatchTexTGTexTGCxformEAlpha:                            return FSI_FBatchTexTGTexTGCxformEAlpha;
    case FS_FBatchTexTGTexTGCxformEAlphaMul:                         return FSI_FBatchTexTGTexTGCxformEAlphaMul;
    case FS_FInstancedTexTGTexTGCxformEAlpha:                        return FSI_FInstancedTexTGTexTGCxformEAlpha;
    case FS_FInstancedTexTGTexTGCxformEAlphaMul:                     return FSI_FInstancedTexTGTexTGCxformEAlphaMul;
    case FS_FPosition3dTexTGTexTGCxformEAlpha:                       return FSI_FPosition3dTexTGTexTGCxformEAlpha;
    case FS_FPosition3dTexTGTexTGCxformEAlphaMul:                    return FSI_FPosition3dTexTGTexTGCxformEAlphaMul;
    case FS_FBatchPosition3dTexTGTexTGCxformEAlpha:                  return FSI_FBatchPosition3dTexTGTexTGCxformEAlpha;
    case FS_FBatchPosition3dTexTGTexTGCxformEAlphaMul:               return FSI_FBatchPosition3dTexTGTexTGCxformEAlphaMul;
    case FS_FInstancedPosition3dTexTGTexTGCxformEAlpha:              return FSI_FInstancedPosition3dTexTGTexTGCxformEAlpha;
    case FS_FInstancedPosition3dTexTGTexTGCxformEAlphaMul:           return FSI_FInstancedPosition3dTexTGTexTGCxformEAlphaMul;
    case FS_FTexTGVertex:                                            return FSI_FTexTGVertex;
    case FS_FTexTGVertexMul:                                         return FSI_FTexTGVertexMul;
    case FS_FBatchTexTGVertex:                                       return FSI_FBatchTexTGVertex;
    case FS_FBatchTexTGVertexMul:                                    return FSI_FBatchTexTGVertexMul;
    case FS_FInstancedTexTGVertex:                                   return FSI_FInstancedTexTGVertex;
    case FS_FInstancedTexTGVertexMul:                                return FSI_FInstancedTexTGVertexMul;
    case FS_FPosition3dTexTGVertex:                                  return FSI_FPosition3dTexTGVertex;
    case FS_FPosition3dTexTGVertexMul:                               return FSI_FPosition3dTexTGVertexMul;
    case FS_FBatchPosition3dTexTGVertex:                             return FSI_FBatchPosition3dTexTGVertex;
    case FS_FBatchPosition3dTexTGVertexMul:                          return FSI_FBatchPosition3dTexTGVertexMul;
    case FS_FInstancedPosition3dTexTGVertex:                         return FSI_FInstancedPosition3dTexTGVertex;
    case FS_FInstancedPosition3dTexTGVertexMul:                      return FSI_FInstancedPosition3dTexTGVertexMul;
    case FS_FTexTGVertexCxform:                                      return FSI_FTexTGVertexCxform;
    case FS_FTexTGVertexCxformMul:                                   return FSI_FTexTGVertexCxformMul;
    case FS_FBatchTexTGVertexCxform:                                 return FSI_FBatchTexTGVertexCxform;
    case FS_FBatchTexTGVertexCxformMul:                              return FSI_FBatchTexTGVertexCxformMul;
    case FS_FInstancedTexTGVertexCxform:                             return FSI_FInstancedTexTGVertexCxform;
    case FS_FInstancedTexTGVertexCxformMul:                          return FSI_FInstancedTexTGVertexCxformMul;
    case FS_FPosition3dTexTGVertexCxform:                            return FSI_FPosition3dTexTGVertexCxform;
    case FS_FPosition3dTexTGVertexCxformMul:                         return FSI_FPosition3dTexTGVertexCxformMul;
    case FS_FBatchPosition3dTexTGVertexCxform:                       return FSI_FBatchPosition3dTexTGVertexCxform;
    case FS_FBatchPosition3dTexTGVertexCxformMul:                    return FSI_FBatchPosition3dTexTGVertexCxformMul;
    case FS_FInstancedPosition3dTexTGVertexCxform:                   return FSI_FInstancedPosition3dTexTGVertexCxform;
    case FS_FInstancedPosition3dTexTGVertexCxformMul:                return FSI_FInstancedPosition3dTexTGVertexCxformMul;
    case FS_FTexTGVertexEAlpha:                                      return FSI_FTexTGVertexEAlpha;
    case FS_FTexTGVertexEAlphaMul:                                   return FSI_FTexTGVertexEAlphaMul;
    case FS_FBatchTexTGVertexEAlpha:                                 return FSI_FBatchTexTGVertexEAlpha;
    case FS_FBatchTexTGVertexEAlphaMul:                              return FSI_FBatchTexTGVertexEAlphaMul;
    case FS_FInstancedTexTGVertexEAlpha:                             return FSI_FInstancedTexTGVertexEAlpha;
    case FS_FInstancedTexTGVertexEAlphaMul:                          return FSI_FInstancedTexTGVertexEAlphaMul;
    case FS_FPosition3dTexTGVertexEAlpha:                            return FSI_FPosition3dTexTGVertexEAlpha;
    case FS_FPosition3dTexTGVertexEAlphaMul:                         return FSI_FPosition3dTexTGVertexEAlphaMul;
    case FS_FBatchPosition3dTexTGVertexEAlpha:                       return FSI_FBatchPosition3dTexTGVertexEAlpha;
    case FS_FBatchPosition3dTexTGVertexEAlphaMul:                    return FSI_FBatchPosition3dTexTGVertexEAlphaMul;
    case FS_FInstancedPosition3dTexTGVertexEAlpha:                   return FSI_FInstancedPosition3dTexTGVertexEAlpha;
    case FS_FInstancedPosition3dTexTGVertexEAlphaMul:                return FSI_FInstancedPosition3dTexTGVertexEAlphaMul;
    case FS_FTexTGVertexCxformEAlpha:                                return FSI_FTexTGVertexCxformEAlpha;
    case FS_FTexTGVertexCxformEAlphaMul:                             return FSI_FTexTGVertexCxformEAlphaMul;
    case FS_FBatchTexTGVertexCxformEAlpha:                           return FSI_FBatchTexTGVertexCxformEAlpha;
    case FS_FBatchTexTGVertexCxformEAlphaMul:                        return FSI_FBatchTexTGVertexCxformEAlphaMul;
    case FS_FInstancedTexTGVertexCxformEAlpha:                       return FSI_FInstancedTexTGVertexCxformEAlpha;
    case FS_FInstancedTexTGVertexCxformEAlphaMul:                    return FSI_FInstancedTexTGVertexCxformEAlphaMul;
    case FS_FPosition3dTexTGVertexCxformEAlpha:                      return FSI_FPosition3dTexTGVertexCxformEAlpha;
    case FS_FPosition3dTexTGVertexCxformEAlphaMul:                   return FSI_FPosition3dTexTGVertexCxformEAlphaMul;
    case FS_FBatchPosition3dTexTGVertexCxformEAlpha:                 return FSI_FBatchPosition3dTexTGVertexCxformEAlpha;
    case FS_FBatchPosition3dTexTGVertexCxformEAlphaMul:              return FSI_FBatchPosition3dTexTGVertexCxformEAlphaMul;
    case FS_FInstancedPosition3dTexTGVertexCxformEAlpha:             return FSI_FInstancedPosition3dTexTGVertexCxformEAlpha;
    case FS_FInstancedPosition3dTexTGVertexCxformEAlphaMul:          return FSI_FInstancedPosition3dTexTGVertexCxformEAlphaMul;
    case FS_FSolid:                                                  return FSI_FSolid;
    case FS_FSolidMul:                                               return FSI_FSolidMul;
    case FS_FBatchSolid:                                             return FSI_FBatchSolid;
    case FS_FBatchSolidMul:                                          return FSI_FBatchSolidMul;
    case FS_FInstancedSolid:                                         return FSI_FInstancedSolid;
    case FS_FInstancedSolidMul:                                      return FSI_FInstancedSolidMul;
    case FS_FPosition3dSolid:                                        return FSI_FPosition3dSolid;
    case FS_FPosition3dSolidMul:                                     return FSI_FPosition3dSolidMul;
    case FS_FBatchPosition3dSolid:                                   return FSI_FBatchPosition3dSolid;
    case FS_FBatchPosition3dSolidMul:                                return FSI_FBatchPosition3dSolidMul;
    case FS_FInstancedPosition3dSolid:                               return FSI_FInstancedPosition3dSolid;
    case FS_FInstancedPosition3dSolidMul:                            return FSI_FInstancedPosition3dSolidMul;
    case FS_FText:                                                   return FSI_FText;
    case FS_FTextMul:                                                return FSI_FTextMul;
    case FS_FBatchText:                                              return FSI_FBatchText;
    case FS_FBatchTextMul:                                           return FSI_FBatchTextMul;
    case FS_FInstancedText:                                          return FSI_FInstancedText;
    case FS_FInstancedTextMul:                                       return FSI_FInstancedTextMul;
    case FS_FPosition3dText:                                         return FSI_FPosition3dText;
    case FS_FPosition3dTextMul:                                      return FSI_FPosition3dTextMul;
    case FS_FBatchPosition3dText:                                    return FSI_FBatchPosition3dText;
    case FS_FBatchPosition3dTextMul:                                 return FSI_FBatchPosition3dTextMul;
    case FS_FInstancedPosition3dText:                                return FSI_FInstancedPosition3dText;
    case FS_FInstancedPosition3dTextMul:                             return FSI_FInstancedPosition3dTextMul;
    case FS_FTextColor:                                              return FSI_FTextColor;
    case FS_FTextColorMul:                                           return FSI_FTextColorMul;
    case FS_FBatchTextColor:                                         return FSI_FBatchTextColor;
    case FS_FBatchTextColorMul:                                      return FSI_FBatchTextColorMul;
    case FS_FInstancedTextColor:                                     return FSI_FInstancedTextColor;
    case FS_FInstancedTextColorMul:                                  return FSI_FInstancedTextColorMul;
    case FS_FPosition3dTextColor:                                    return FSI_FPosition3dTextColor;
    case FS_FPosition3dTextColorMul:                                 return FSI_FPosition3dTextColorMul;
    case FS_FBatchPosition3dTextColor:                               return FSI_FBatchPosition3dTextColor;
    case FS_FBatchPosition3dTextColorMul:                            return FSI_FBatchPosition3dTextColorMul;
    case FS_FInstancedPosition3dTextColor:                           return FSI_FInstancedPosition3dTextColor;
    case FS_FInstancedPosition3dTextColorMul:                        return FSI_FInstancedPosition3dTextColorMul;
    case FS_FTextColorCxform:                                        return FSI_FTextColorCxform;
    case FS_FTextColorCxformMul:                                     return FSI_FTextColorCxformMul;
    case FS_FBatchTextColorCxform:                                   return FSI_FBatchTextColorCxform;
    case FS_FBatchTextColorCxformMul:                                return FSI_FBatchTextColorCxformMul;
    case FS_FInstancedTextColorCxform:                               return FSI_FInstancedTextColorCxform;
    case FS_FInstancedTextColorCxformMul:                            return FSI_FInstancedTextColorCxformMul;
    case FS_FPosition3dTextColorCxform:                              return FSI_FPosition3dTextColorCxform;
    case FS_FPosition3dTextColorCxformMul:                           return FSI_FPosition3dTextColorCxformMul;
    case FS_FBatchPosition3dTextColorCxform:                         return FSI_FBatchPosition3dTextColorCxform;
    case FS_FBatchPosition3dTextColorCxformMul:                      return FSI_FBatchPosition3dTextColorCxformMul;
    case FS_FInstancedPosition3dTextColorCxform:                     return FSI_FInstancedPosition3dTextColorCxform;
    case FS_FInstancedPosition3dTextColorCxformMul:                  return FSI_FInstancedPosition3dTextColorCxformMul;
    case FS_FYUV:                                                    return FSI_FYUV;
    case FS_FYUVMul:                                                 return FSI_FYUVMul;
    case FS_FBatchYUV:                                               return FSI_FBatchYUV;
    case FS_FBatchYUVMul:                                            return FSI_FBatchYUVMul;
    case FS_FInstancedYUV:                                           return FSI_FInstancedYUV;
    case FS_FInstancedYUVMul:                                        return FSI_FInstancedYUVMul;
    case FS_FPosition3dYUV:                                          return FSI_FPosition3dYUV;
    case FS_FPosition3dYUVMul:                                       return FSI_FPosition3dYUVMul;
    case FS_FBatchPosition3dYUV:                                     return FSI_FBatchPosition3dYUV;
    case FS_FBatchPosition3dYUVMul:                                  return FSI_FBatchPosition3dYUVMul;
    case FS_FInstancedPosition3dYUV:                                 return FSI_FInstancedPosition3dYUV;
    case FS_FInstancedPosition3dYUVMul:                              return FSI_FInstancedPosition3dYUVMul;
    case FS_FYUVCxform:                                              return FSI_FYUVCxform;
    case FS_FYUVCxformMul:                                           return FSI_FYUVCxformMul;
    case FS_FBatchYUVCxform:                                         return FSI_FBatchYUVCxform;
    case FS_FBatchYUVCxformMul:                                      return FSI_FBatchYUVCxformMul;
    case FS_FInstancedYUVCxform:                                     return FSI_FInstancedYUVCxform;
    case FS_FInstancedYUVCxformMul:                                  return FSI_FInstancedYUVCxformMul;
    case FS_FPosition3dYUVCxform:                                    return FSI_FPosition3dYUVCxform;
    case FS_FPosition3dYUVCxformMul:                                 return FSI_FPosition3dYUVCxformMul;
    case FS_FBatchPosition3dYUVCxform:                               return FSI_FBatchPosition3dYUVCxform;
    case FS_FBatchPosition3dYUVCxformMul:                            return FSI_FBatchPosition3dYUVCxformMul;
    case FS_FInstancedPosition3dYUVCxform:                           return FSI_FInstancedPosition3dYUVCxform;
    case FS_FInstancedPosition3dYUVCxformMul:                        return FSI_FInstancedPosition3dYUVCxformMul;
    case FS_FYUVEAlpha:                                              return FSI_FYUVEAlpha;
    case FS_FYUVEAlphaMul:                                           return FSI_FYUVEAlphaMul;
    case FS_FBatchYUVEAlpha:                                         return FSI_FBatchYUVEAlpha;
    case FS_FBatchYUVEAlphaMul:                                      return FSI_FBatchYUVEAlphaMul;
    case FS_FInstancedYUVEAlpha:                                     return FSI_FInstancedYUVEAlpha;
    case FS_FInstancedYUVEAlphaMul:                                  return FSI_FInstancedYUVEAlphaMul;
    case FS_FPosition3dYUVEAlpha:                                    return FSI_FPosition3dYUVEAlpha;
    case FS_FPosition3dYUVEAlphaMul:                                 return FSI_FPosition3dYUVEAlphaMul;
    case FS_FBatchPosition3dYUVEAlpha:                               return FSI_FBatchPosition3dYUVEAlpha;
    case FS_FBatchPosition3dYUVEAlphaMul:                            return FSI_FBatchPosition3dYUVEAlphaMul;
    case FS_FInstancedPosition3dYUVEAlpha:                           return FSI_FInstancedPosition3dYUVEAlpha;
    case FS_FInstancedPosition3dYUVEAlphaMul:                        return FSI_FInstancedPosition3dYUVEAlphaMul;
    case FS_FYUVCxformEAlpha:                                        return FSI_FYUVCxformEAlpha;
    case FS_FYUVCxformEAlphaMul:                                     return FSI_FYUVCxformEAlphaMul;
    case FS_FBatchYUVCxformEAlpha:                                   return FSI_FBatchYUVCxformEAlpha;
    case FS_FBatchYUVCxformEAlphaMul:                                return FSI_FBatchYUVCxformEAlphaMul;
    case FS_FInstancedYUVCxformEAlpha:                               return FSI_FInstancedYUVCxformEAlpha;
    case FS_FInstancedYUVCxformEAlphaMul:                            return FSI_FInstancedYUVCxformEAlphaMul;
    case FS_FPosition3dYUVCxformEAlpha:                              return FSI_FPosition3dYUVCxformEAlpha;
    case FS_FPosition3dYUVCxformEAlphaMul:                           return FSI_FPosition3dYUVCxformEAlphaMul;
    case FS_FBatchPosition3dYUVCxformEAlpha:                         return FSI_FBatchPosition3dYUVCxformEAlpha;
    case FS_FBatchPosition3dYUVCxformEAlphaMul:                      return FSI_FBatchPosition3dYUVCxformEAlphaMul;
    case FS_FInstancedPosition3dYUVCxformEAlpha:                     return FSI_FInstancedPosition3dYUVCxformEAlpha;
    case FS_FInstancedPosition3dYUVCxformEAlphaMul:                  return FSI_FInstancedPosition3dYUVCxformEAlphaMul;
    case FS_FYUVA:                                                   return FSI_FYUVA;
    case FS_FYUVAMul:                                                return FSI_FYUVAMul;
    case FS_FBatchYUVA:                                              return FSI_FBatchYUVA;
    case FS_FBatchYUVAMul:                                           return FSI_FBatchYUVAMul;
    case FS_FInstancedYUVA:                                          return FSI_FInstancedYUVA;
    case FS_FInstancedYUVAMul:                                       return FSI_FInstancedYUVAMul;
    case FS_FPosition3dYUVA:                                         return FSI_FPosition3dYUVA;
    case FS_FPosition3dYUVAMul:                                      return FSI_FPosition3dYUVAMul;
    case FS_FBatchPosition3dYUVA:                                    return FSI_FBatchPosition3dYUVA;
    case FS_FBatchPosition3dYUVAMul:                                 return FSI_FBatchPosition3dYUVAMul;
    case FS_FInstancedPosition3dYUVA:                                return FSI_FInstancedPosition3dYUVA;
    case FS_FInstancedPosition3dYUVAMul:                             return FSI_FInstancedPosition3dYUVAMul;
    case FS_FYUVACxform:                                             return FSI_FYUVACxform;
    case FS_FYUVACxformMul:                                          return FSI_FYUVACxformMul;
    case FS_FBatchYUVACxform:                                        return FSI_FBatchYUVACxform;
    case FS_FBatchYUVACxformMul:                                     return FSI_FBatchYUVACxformMul;
    case FS_FInstancedYUVACxform:                                    return FSI_FInstancedYUVACxform;
    case FS_FInstancedYUVACxformMul:                                 return FSI_FInstancedYUVACxformMul;
    case FS_FPosition3dYUVACxform:                                   return FSI_FPosition3dYUVACxform;
    case FS_FPosition3dYUVACxformMul:                                return FSI_FPosition3dYUVACxformMul;
    case FS_FBatchPosition3dYUVACxform:                              return FSI_FBatchPosition3dYUVACxform;
    case FS_FBatchPosition3dYUVACxformMul:                           return FSI_FBatchPosition3dYUVACxformMul;
    case FS_FInstancedPosition3dYUVACxform:                          return FSI_FInstancedPosition3dYUVACxform;
    case FS_FInstancedPosition3dYUVACxformMul:                       return FSI_FInstancedPosition3dYUVACxformMul;
    case FS_FYUVAEAlpha:                                             return FSI_FYUVAEAlpha;
    case FS_FYUVAEAlphaMul:                                          return FSI_FYUVAEAlphaMul;
    case FS_FBatchYUVAEAlpha:                                        return FSI_FBatchYUVAEAlpha;
    case FS_FBatchYUVAEAlphaMul:                                     return FSI_FBatchYUVAEAlphaMul;
    case FS_FInstancedYUVAEAlpha:                                    return FSI_FInstancedYUVAEAlpha;
    case FS_FInstancedYUVAEAlphaMul:                                 return FSI_FInstancedYUVAEAlphaMul;
    case FS_FPosition3dYUVAEAlpha:                                   return FSI_FPosition3dYUVAEAlpha;
    case FS_FPosition3dYUVAEAlphaMul:                                return FSI_FPosition3dYUVAEAlphaMul;
    case FS_FBatchPosition3dYUVAEAlpha:                              return FSI_FBatchPosition3dYUVAEAlpha;
    case FS_FBatchPosition3dYUVAEAlphaMul:                           return FSI_FBatchPosition3dYUVAEAlphaMul;
    case FS_FInstancedPosition3dYUVAEAlpha:                          return FSI_FInstancedPosition3dYUVAEAlpha;
    case FS_FInstancedPosition3dYUVAEAlphaMul:                       return FSI_FInstancedPosition3dYUVAEAlphaMul;
    case FS_FYUVACxformEAlpha:                                       return FSI_FYUVACxformEAlpha;
    case FS_FYUVACxformEAlphaMul:                                    return FSI_FYUVACxformEAlphaMul;
    case FS_FBatchYUVACxformEAlpha:                                  return FSI_FBatchYUVACxformEAlpha;
    case FS_FBatchYUVACxformEAlphaMul:                               return FSI_FBatchYUVACxformEAlphaMul;
    case FS_FInstancedYUVACxformEAlpha:                              return FSI_FInstancedYUVACxformEAlpha;
    case FS_FInstancedYUVACxformEAlphaMul:                           return FSI_FInstancedYUVACxformEAlphaMul;
    case FS_FPosition3dYUVACxformEAlpha:                             return FSI_FPosition3dYUVACxformEAlpha;
    case FS_FPosition3dYUVACxformEAlphaMul:                          return FSI_FPosition3dYUVACxformEAlphaMul;
    case FS_FBatchPosition3dYUVACxformEAlpha:                        return FSI_FBatchPosition3dYUVACxformEAlpha;
    case FS_FBatchPosition3dYUVACxformEAlphaMul:                     return FSI_FBatchPosition3dYUVACxformEAlphaMul;
    case FS_FInstancedPosition3dYUVACxformEAlpha:                    return FSI_FInstancedPosition3dYUVACxformEAlpha;
    case FS_FInstancedPosition3dYUVACxformEAlphaMul:                 return FSI_FInstancedPosition3dYUVACxformEAlphaMul;
    case FS_FTexTGCMatrixAc:                                         return FSI_FTexTGCMatrixAc;
    case FS_FTexTGCMatrixAcMul:                                      return FSI_FTexTGCMatrixAcMul;
    case FS_FTexTGCMatrixAcEAlpha:                                   return FSI_FTexTGCMatrixAcEAlpha;
    case FS_FTexTGCMatrixAcEAlphaMul:                                return FSI_FTexTGCMatrixAcEAlphaMul;
    case FS_FBox1Blur:                                               return FSI_FBox1Blur;
    case FS_FBox1BlurMul:                                            return FSI_FBox1BlurMul;
    case FS_FBox2Blur:                                               return FSI_FBox2Blur;
    case FS_FBox2BlurMul:                                            return FSI_FBox2BlurMul;
    case FS_FBox2Shadow:                                             return FSI_FBox2Shadow;
    case FS_FBox2ShadowMul:                                          return FSI_FBox2ShadowMul;
    case FS_FBox2ShadowKnockout:                                     return FSI_FBox2ShadowKnockout;
    case FS_FBox2ShadowKnockoutMul:                                  return FSI_FBox2ShadowKnockoutMul;
    case FS_FBox2Shadowonly:                                         return FSI_FBox2Shadowonly;
    case FS_FBox2ShadowonlyMul:                                      return FSI_FBox2ShadowonlyMul;
    case FS_FBox2InnerShadow:                                        return FSI_FBox2InnerShadow;
    case FS_FBox2InnerShadowMul:                                     return FSI_FBox2InnerShadowMul;
    case FS_FBox2InnerShadowKnockout:                                return FSI_FBox2InnerShadowKnockout;
    case FS_FBox2InnerShadowKnockoutMul:                             return FSI_FBox2InnerShadowKnockoutMul;
    case FS_FBox2ShadowonlyHighlight:                                return FSI_FBox2ShadowonlyHighlight;
    case FS_FBox2ShadowonlyHighlightMul:                             return FSI_FBox2ShadowonlyHighlightMul;
    case FS_FBox2InnerShadowHighlight:                               return FSI_FBox2InnerShadowHighlight;
    case FS_FBox2InnerShadowHighlightMul:                            return FSI_FBox2InnerShadowHighlightMul;
    case FS_FBox2InnerShadowHighlightKnockout:                       return FSI_FBox2InnerShadowHighlightKnockout;
    case FS_FBox2InnerShadowHighlightKnockoutMul:                    return FSI_FBox2InnerShadowHighlightKnockoutMul;
    case FS_FBox2ShadowHighlight:                                    return FSI_FBox2ShadowHighlight;
    case FS_FBox2ShadowHighlightMul:                                 return FSI_FBox2ShadowHighlightMul;
    case FS_FBox2ShadowHighlightKnockout:                            return FSI_FBox2ShadowHighlightKnockout;
    case FS_FBox2ShadowHighlightKnockoutMul:                         return FSI_FBox2ShadowHighlightKnockoutMul;
    case FS_FDrawableCopyPixels:                                     return FSI_FDrawableCopyPixels;
    case FS_FDrawableCopyPixelsCopyLerp:                             return FSI_FDrawableCopyPixelsCopyLerp;
    case FS_FDrawableCopyPixelsNoDestAlpha:                          return FSI_FDrawableCopyPixelsNoDestAlpha;
    case FS_FDrawableCopyPixelsNoDestAlphaCopyLerp:                  return FSI_FDrawableCopyPixelsNoDestAlphaCopyLerp;
    case FS_FDrawableCopyPixelsMergeAlpha:                           return FSI_FDrawableCopyPixelsMergeAlpha;
    case FS_FDrawableCopyPixelsMergeAlphaCopyLerp:                   return FSI_FDrawableCopyPixelsMergeAlphaCopyLerp;
    case FS_FDrawableCopyPixelsMergeAlphaNoDestAlpha:                return FSI_FDrawableCopyPixelsMergeAlphaNoDestAlpha;
    case FS_FDrawableCopyPixelsMergeAlphaNoDestAlphaCopyLerp:        return FSI_FDrawableCopyPixelsMergeAlphaNoDestAlphaCopyLerp;
    case FS_FDrawableCopyPixelsAlpha:                                return FSI_FDrawableCopyPixelsAlpha;
    case FS_FDrawableCopyPixelsAlphaCopyLerp:                        return FSI_FDrawableCopyPixelsAlphaCopyLerp;
    case FS_FDrawableCopyPixelsAlphaNoDestAlpha:                     return FSI_FDrawableCopyPixelsAlphaNoDestAlpha;
    case FS_FDrawableCopyPixelsAlphaNoDestAlphaCopyLerp:             return FSI_FDrawableCopyPixelsAlphaNoDestAlphaCopyLerp;
    case FS_FDrawableCopyPixelsAlphaMergeAlpha:                      return FSI_FDrawableCopyPixelsAlphaMergeAlpha;
    case FS_FDrawableCopyPixelsAlphaMergeAlphaCopyLerp:              return FSI_FDrawableCopyPixelsAlphaMergeAlphaCopyLerp;
    case FS_FDrawableCopyPixelsAlphaMergeAlphaNoDestAlpha:           return FSI_FDrawableCopyPixelsAlphaMergeAlphaNoDestAlpha;
    case FS_FDrawableCopyPixelsAlphaMergeAlphaNoDestAlphaCopyLerp:   return FSI_FDrawableCopyPixelsAlphaMergeAlphaNoDestAlphaCopyLerp;
    case FS_FDrawableMerge:                                          return FSI_FDrawableMerge;
    case FS_FDrawableCompare:                                        return FSI_FDrawableCompare;
    case FS_FDrawablePaletteMap:                                     return FSI_FDrawablePaletteMap;
    default: SF_ASSERT(0); return FSI_None;
    }
};

FragShaderDesc::ShaderIndex FragShaderDesc::GetShaderIndexForComboIndex(unsigned index)
{
    switch(index)
    {
        case 0:             return FSI_FTexTG;
        case 1:             return FSI_FTexTGMul;
        case 2:             return FSI_FTexTGEAlpha;
        case 3:             return FSI_FTexTGEAlphaMul;
        case 4:             return FSI_FTexTGCxform;
        case 5:             return FSI_FTexTGCxformMul;
        case 6:             return FSI_FTexTGCxformEAlpha;
        case 7:             return FSI_FTexTGCxformEAlphaMul;
        case 8:             return FSI_FVertex;
        case 9:             return FSI_FVertexMul;
        case 10:            return FSI_FVertexEAlpha;
        case 11:            return FSI_FVertexEAlphaMul;
        case 12:            return FSI_FVertexCxform;
        case 13:            return FSI_FVertexCxformMul;
        case 14:            return FSI_FVertexCxformEAlpha;
        case 15:            return FSI_FVertexCxformEAlphaMul;
        case 16:            return FSI_FTexTGTexTG;
        case 17:            return FSI_FTexTGTexTGMul;
        case 18:            return FSI_FTexTGTexTGEAlpha;
        case 19:            return FSI_FTexTGTexTGEAlphaMul;
        case 20:            return FSI_FTexTGTexTGCxform;
        case 21:            return FSI_FTexTGTexTGCxformMul;
        case 22:            return FSI_FTexTGTexTGCxformEAlpha;
        case 23:            return FSI_FTexTGTexTGCxformEAlphaMul;
        case 24:            return FSI_FTexTGVertex;
        case 25:            return FSI_FTexTGVertexMul;
        case 26:            return FSI_FTexTGVertexEAlpha;
        case 27:            return FSI_FTexTGVertexEAlphaMul;
        case 28:            return FSI_FTexTGVertexCxform;
        case 29:            return FSI_FTexTGVertexCxformMul;
        case 30:            return FSI_FTexTGVertexCxformEAlpha;
        case 31:            return FSI_FTexTGVertexCxformEAlphaMul;
        case 32:            return FSI_FPosition3dTexTG;
        case 33:            return FSI_FPosition3dTexTGMul;
        case 34:            return FSI_FPosition3dTexTGEAlpha;
        case 35:            return FSI_FPosition3dTexTGEAlphaMul;
        case 36:            return FSI_FPosition3dTexTGCxform;
        case 37:            return FSI_FPosition3dTexTGCxformMul;
        case 38:            return FSI_FPosition3dTexTGCxformEAlpha;
        case 39:            return FSI_FPosition3dTexTGCxformEAlphaMul;
        case 40:            return FSI_FPosition3dVertex;
        case 41:            return FSI_FPosition3dVertexMul;
        case 42:            return FSI_FPosition3dVertexEAlpha;
        case 43:            return FSI_FPosition3dVertexEAlphaMul;
        case 44:            return FSI_FPosition3dVertexCxform;
        case 45:            return FSI_FPosition3dVertexCxformMul;
        case 46:            return FSI_FPosition3dVertexCxformEAlpha;
        case 47:            return FSI_FPosition3dVertexCxformEAlphaMul;
        case 48:            return FSI_FPosition3dTexTGTexTG;
        case 49:            return FSI_FPosition3dTexTGTexTGMul;
        case 50:            return FSI_FPosition3dTexTGTexTGEAlpha;
        case 51:            return FSI_FPosition3dTexTGTexTGEAlphaMul;
        case 52:            return FSI_FPosition3dTexTGTexTGCxform;
        case 53:            return FSI_FPosition3dTexTGTexTGCxformMul;
        case 54:            return FSI_FPosition3dTexTGTexTGCxformEAlpha;
        case 55:            return FSI_FPosition3dTexTGTexTGCxformEAlphaMul;
        case 56:            return FSI_FPosition3dTexTGVertex;
        case 57:            return FSI_FPosition3dTexTGVertexMul;
        case 58:            return FSI_FPosition3dTexTGVertexEAlpha;
        case 59:            return FSI_FPosition3dTexTGVertexEAlphaMul;
        case 60:            return FSI_FPosition3dTexTGVertexCxform;
        case 61:            return FSI_FPosition3dTexTGVertexCxformMul;
        case 62:            return FSI_FPosition3dTexTGVertexCxformEAlpha;
        case 63:            return FSI_FPosition3dTexTGVertexCxformEAlphaMul;
        case 64:            return FSI_FBatchTexTG;
        case 65:            return FSI_FBatchTexTGMul;
        case 66:            return FSI_FBatchTexTGEAlpha;
        case 67:            return FSI_FBatchTexTGEAlphaMul;
        case 68:            return FSI_FBatchTexTGCxform;
        case 69:            return FSI_FBatchTexTGCxformMul;
        case 70:            return FSI_FBatchTexTGCxformEAlpha;
        case 71:            return FSI_FBatchTexTGCxformEAlphaMul;
        case 72:            return FSI_FBatchVertex;
        case 73:            return FSI_FBatchVertexMul;
        case 74:            return FSI_FBatchVertexEAlpha;
        case 75:            return FSI_FBatchVertexEAlphaMul;
        case 76:            return FSI_FBatchVertexCxform;
        case 77:            return FSI_FBatchVertexCxformMul;
        case 78:            return FSI_FBatchVertexCxformEAlpha;
        case 79:            return FSI_FBatchVertexCxformEAlphaMul;
        case 80:            return FSI_FBatchTexTGTexTG;
        case 81:            return FSI_FBatchTexTGTexTGMul;
        case 82:            return FSI_FBatchTexTGTexTGEAlpha;
        case 83:            return FSI_FBatchTexTGTexTGEAlphaMul;
        case 84:            return FSI_FBatchTexTGTexTGCxform;
        case 85:            return FSI_FBatchTexTGTexTGCxformMul;
        case 86:            return FSI_FBatchTexTGTexTGCxformEAlpha;
        case 87:            return FSI_FBatchTexTGTexTGCxformEAlphaMul;
        case 88:            return FSI_FBatchTexTGVertex;
        case 89:            return FSI_FBatchTexTGVertexMul;
        case 90:            return FSI_FBatchTexTGVertexEAlpha;
        case 91:            return FSI_FBatchTexTGVertexEAlphaMul;
        case 92:            return FSI_FBatchTexTGVertexCxform;
        case 93:            return FSI_FBatchTexTGVertexCxformMul;
        case 94:            return FSI_FBatchTexTGVertexCxformEAlpha;
        case 95:            return FSI_FBatchTexTGVertexCxformEAlphaMul;
        case 96:            return FSI_FBatchPosition3dTexTG;
        case 97:            return FSI_FBatchPosition3dTexTGMul;
        case 98:            return FSI_FBatchPosition3dTexTGEAlpha;
        case 99:            return FSI_FBatchPosition3dTexTGEAlphaMul;
        case 100:           return FSI_FBatchPosition3dTexTGCxform;
        case 101:           return FSI_FBatchPosition3dTexTGCxformMul;
        case 102:           return FSI_FBatchPosition3dTexTGCxformEAlpha;
        case 103:           return FSI_FBatchPosition3dTexTGCxformEAlphaMul;
        case 104:           return FSI_FBatchPosition3dVertex;
        case 105:           return FSI_FBatchPosition3dVertexMul;
        case 106:           return FSI_FBatchPosition3dVertexEAlpha;
        case 107:           return FSI_FBatchPosition3dVertexEAlphaMul;
        case 108:           return FSI_FBatchPosition3dVertexCxform;
        case 109:           return FSI_FBatchPosition3dVertexCxformMul;
        case 110:           return FSI_FBatchPosition3dVertexCxformEAlpha;
        case 111:           return FSI_FBatchPosition3dVertexCxformEAlphaMul;
        case 112:           return FSI_FBatchPosition3dTexTGTexTG;
        case 113:           return FSI_FBatchPosition3dTexTGTexTGMul;
        case 114:           return FSI_FBatchPosition3dTexTGTexTGEAlpha;
        case 115:           return FSI_FBatchPosition3dTexTGTexTGEAlphaMul;
        case 116:           return FSI_FBatchPosition3dTexTGTexTGCxform;
        case 117:           return FSI_FBatchPosition3dTexTGTexTGCxformMul;
        case 118:           return FSI_FBatchPosition3dTexTGTexTGCxformEAlpha;
        case 119:           return FSI_FBatchPosition3dTexTGTexTGCxformEAlphaMul;
        case 120:           return FSI_FBatchPosition3dTexTGVertex;
        case 121:           return FSI_FBatchPosition3dTexTGVertexMul;
        case 122:           return FSI_FBatchPosition3dTexTGVertexEAlpha;
        case 123:           return FSI_FBatchPosition3dTexTGVertexEAlphaMul;
        case 124:           return FSI_FBatchPosition3dTexTGVertexCxform;
        case 125:           return FSI_FBatchPosition3dTexTGVertexCxformMul;
        case 126:           return FSI_FBatchPosition3dTexTGVertexCxformEAlpha;
        case 127:           return FSI_FBatchPosition3dTexTGVertexCxformEAlphaMul;
        case 128:           return FSI_FInstancedTexTG;
        case 129:           return FSI_FInstancedTexTGMul;
        case 130:           return FSI_FInstancedTexTGEAlpha;
        case 131:           return FSI_FInstancedTexTGEAlphaMul;
        case 132:           return FSI_FInstancedTexTGCxform;
        case 133:           return FSI_FInstancedTexTGCxformMul;
        case 134:           return FSI_FInstancedTexTGCxformEAlpha;
        case 135:           return FSI_FInstancedTexTGCxformEAlphaMul;
        case 136:           return FSI_FInstancedVertex;
        case 137:           return FSI_FInstancedVertexMul;
        case 138:           return FSI_FInstancedVertexEAlpha;
        case 139:           return FSI_FInstancedVertexEAlphaMul;
        case 140:           return FSI_FInstancedVertexCxform;
        case 141:           return FSI_FInstancedVertexCxformMul;
        case 142:           return FSI_FInstancedVertexCxformEAlpha;
        case 143:           return FSI_FInstancedVertexCxformEAlphaMul;
        case 144:           return FSI_FInstancedTexTGTexTG;
        case 145:           return FSI_FInstancedTexTGTexTGMul;
        case 146:           return FSI_FInstancedTexTGTexTGEAlpha;
        case 147:           return FSI_FInstancedTexTGTexTGEAlphaMul;
        case 148:           return FSI_FInstancedTexTGTexTGCxform;
        case 149:           return FSI_FInstancedTexTGTexTGCxformMul;
        case 150:           return FSI_FInstancedTexTGTexTGCxformEAlpha;
        case 151:           return FSI_FInstancedTexTGTexTGCxformEAlphaMul;
        case 152:           return FSI_FInstancedTexTGVertex;
        case 153:           return FSI_FInstancedTexTGVertexMul;
        case 154:           return FSI_FInstancedTexTGVertexEAlpha;
        case 155:           return FSI_FInstancedTexTGVertexEAlphaMul;
        case 156:           return FSI_FInstancedTexTGVertexCxform;
        case 157:           return FSI_FInstancedTexTGVertexCxformMul;
        case 158:           return FSI_FInstancedTexTGVertexCxformEAlpha;
        case 159:           return FSI_FInstancedTexTGVertexCxformEAlphaMul;
        case 160:           return FSI_FInstancedPosition3dTexTG;
        case 161:           return FSI_FInstancedPosition3dTexTGMul;
        case 162:           return FSI_FInstancedPosition3dTexTGEAlpha;
        case 163:           return FSI_FInstancedPosition3dTexTGEAlphaMul;
        case 164:           return FSI_FInstancedPosition3dTexTGCxform;
        case 165:           return FSI_FInstancedPosition3dTexTGCxformMul;
        case 166:           return FSI_FInstancedPosition3dTexTGCxformEAlpha;
        case 167:           return FSI_FInstancedPosition3dTexTGCxformEAlphaMul;
        case 168:           return FSI_FInstancedPosition3dVertex;
        case 169:           return FSI_FInstancedPosition3dVertexMul;
        case 170:           return FSI_FInstancedPosition3dVertexEAlpha;
        case 171:           return FSI_FInstancedPosition3dVertexEAlphaMul;
        case 172:           return FSI_FInstancedPosition3dVertexCxform;
        case 173:           return FSI_FInstancedPosition3dVertexCxformMul;
        case 174:           return FSI_FInstancedPosition3dVertexCxformEAlpha;
        case 175:           return FSI_FInstancedPosition3dVertexCxformEAlphaMul;
        case 176:           return FSI_FInstancedPosition3dTexTGTexTG;
        case 177:           return FSI_FInstancedPosition3dTexTGTexTGMul;
        case 178:           return FSI_FInstancedPosition3dTexTGTexTGEAlpha;
        case 179:           return FSI_FInstancedPosition3dTexTGTexTGEAlphaMul;
        case 180:           return FSI_FInstancedPosition3dTexTGTexTGCxform;
        case 181:           return FSI_FInstancedPosition3dTexTGTexTGCxformMul;
        case 182:           return FSI_FInstancedPosition3dTexTGTexTGCxformEAlpha;
        case 183:           return FSI_FInstancedPosition3dTexTGTexTGCxformEAlphaMul;
        case 184:           return FSI_FInstancedPosition3dTexTGVertex;
        case 185:           return FSI_FInstancedPosition3dTexTGVertexMul;
        case 186:           return FSI_FInstancedPosition3dTexTGVertexEAlpha;
        case 187:           return FSI_FInstancedPosition3dTexTGVertexEAlphaMul;
        case 188:           return FSI_FInstancedPosition3dTexTGVertexCxform;
        case 189:           return FSI_FInstancedPosition3dTexTGVertexCxformMul;
        case 190:           return FSI_FInstancedPosition3dTexTGVertexCxformEAlpha;
        case 191:           return FSI_FInstancedPosition3dTexTGVertexCxformEAlphaMul;
        case 192:           return FSI_FSolid;
        case 193:           return FSI_FSolidMul;
        case 194:           return FSI_FText;
        case 195:           return FSI_FTextMul;
        case 196:           return FSI_FTextColor;
        case 197:           return FSI_FTextColorMul;
        case 198:           return FSI_FTextColorCxform;
        case 199:           return FSI_FTextColorCxformMul;
        case 200:           return FSI_FPosition3dSolid;
        case 201:           return FSI_FPosition3dSolidMul;
        case 202:           return FSI_FPosition3dText;
        case 203:           return FSI_FPosition3dTextMul;
        case 204:           return FSI_FPosition3dTextColor;
        case 205:           return FSI_FPosition3dTextColorMul;
        case 206:           return FSI_FPosition3dTextColorCxform;
        case 207:           return FSI_FPosition3dTextColorCxformMul;
        case 208:           return FSI_FBatchSolid;
        case 209:           return FSI_FBatchSolidMul;
        case 210:           return FSI_FBatchText;
        case 211:           return FSI_FBatchTextMul;
        case 212:           return FSI_FBatchTextColor;
        case 213:           return FSI_FBatchTextColorMul;
        case 214:           return FSI_FBatchTextColorCxform;
        case 215:           return FSI_FBatchTextColorCxformMul;
        case 216:           return FSI_FBatchPosition3dSolid;
        case 217:           return FSI_FBatchPosition3dSolidMul;
        case 218:           return FSI_FBatchPosition3dText;
        case 219:           return FSI_FBatchPosition3dTextMul;
        case 220:           return FSI_FBatchPosition3dTextColor;
        case 221:           return FSI_FBatchPosition3dTextColorMul;
        case 222:           return FSI_FBatchPosition3dTextColorCxform;
        case 223:           return FSI_FBatchPosition3dTextColorCxformMul;
        case 224:           return FSI_FInstancedSolid;
        case 225:           return FSI_FInstancedSolidMul;
        case 226:           return FSI_FInstancedText;
        case 227:           return FSI_FInstancedTextMul;
        case 228:           return FSI_FInstancedTextColor;
        case 229:           return FSI_FInstancedTextColorMul;
        case 230:           return FSI_FInstancedTextColorCxform;
        case 231:           return FSI_FInstancedTextColorCxformMul;
        case 232:           return FSI_FInstancedPosition3dSolid;
        case 233:           return FSI_FInstancedPosition3dSolidMul;
        case 234:           return FSI_FInstancedPosition3dText;
        case 235:           return FSI_FInstancedPosition3dTextMul;
        case 236:           return FSI_FInstancedPosition3dTextColor;
        case 237:           return FSI_FInstancedPosition3dTextColorMul;
        case 238:           return FSI_FInstancedPosition3dTextColorCxform;
        case 239:           return FSI_FInstancedPosition3dTextColorCxformMul;
        case 240:           return FSI_FYUV;
        case 241:           return FSI_FYUVMul;
        case 242:           return FSI_FYUVEAlpha;
        case 243:           return FSI_FYUVEAlphaMul;
        case 244:           return FSI_FYUVCxform;
        case 245:           return FSI_FYUVCxformMul;
        case 246:           return FSI_FYUVCxformEAlpha;
        case 247:           return FSI_FYUVCxformEAlphaMul;
        case 248:           return FSI_FYUVA;
        case 249:           return FSI_FYUVAMul;
        case 250:           return FSI_FYUVAEAlpha;
        case 251:           return FSI_FYUVAEAlphaMul;
        case 252:           return FSI_FYUVACxform;
        case 253:           return FSI_FYUVACxformMul;
        case 254:           return FSI_FYUVACxformEAlpha;
        case 255:           return FSI_FYUVACxformEAlphaMul;
        case 256:           return FSI_FPosition3dYUV;
        case 257:           return FSI_FPosition3dYUVMul;
        case 258:           return FSI_FPosition3dYUVEAlpha;
        case 259:           return FSI_FPosition3dYUVEAlphaMul;
        case 260:           return FSI_FPosition3dYUVCxform;
        case 261:           return FSI_FPosition3dYUVCxformMul;
        case 262:           return FSI_FPosition3dYUVCxformEAlpha;
        case 263:           return FSI_FPosition3dYUVCxformEAlphaMul;
        case 264:           return FSI_FPosition3dYUVA;
        case 265:           return FSI_FPosition3dYUVAMul;
        case 266:           return FSI_FPosition3dYUVAEAlpha;
        case 267:           return FSI_FPosition3dYUVAEAlphaMul;
        case 268:           return FSI_FPosition3dYUVACxform;
        case 269:           return FSI_FPosition3dYUVACxformMul;
        case 270:           return FSI_FPosition3dYUVACxformEAlpha;
        case 271:           return FSI_FPosition3dYUVACxformEAlphaMul;
        case 272:           return FSI_FBatchYUV;
        case 273:           return FSI_FBatchYUVMul;
        case 274:           return FSI_FBatchYUVEAlpha;
        case 275:           return FSI_FBatchYUVEAlphaMul;
        case 276:           return FSI_FBatchYUVCxform;
        case 277:           return FSI_FBatchYUVCxformMul;
        case 278:           return FSI_FBatchYUVCxformEAlpha;
        case 279:           return FSI_FBatchYUVCxformEAlphaMul;
        case 280:           return FSI_FBatchYUVA;
        case 281:           return FSI_FBatchYUVAMul;
        case 282:           return FSI_FBatchYUVAEAlpha;
        case 283:           return FSI_FBatchYUVAEAlphaMul;
        case 284:           return FSI_FBatchYUVACxform;
        case 285:           return FSI_FBatchYUVACxformMul;
        case 286:           return FSI_FBatchYUVACxformEAlpha;
        case 287:           return FSI_FBatchYUVACxformEAlphaMul;
        case 288:           return FSI_FBatchPosition3dYUV;
        case 289:           return FSI_FBatchPosition3dYUVMul;
        case 290:           return FSI_FBatchPosition3dYUVEAlpha;
        case 291:           return FSI_FBatchPosition3dYUVEAlphaMul;
        case 292:           return FSI_FBatchPosition3dYUVCxform;
        case 293:           return FSI_FBatchPosition3dYUVCxformMul;
        case 294:           return FSI_FBatchPosition3dYUVCxformEAlpha;
        case 295:           return FSI_FBatchPosition3dYUVCxformEAlphaMul;
        case 296:           return FSI_FBatchPosition3dYUVA;
        case 297:           return FSI_FBatchPosition3dYUVAMul;
        case 298:           return FSI_FBatchPosition3dYUVAEAlpha;
        case 299:           return FSI_FBatchPosition3dYUVAEAlphaMul;
        case 300:           return FSI_FBatchPosition3dYUVACxform;
        case 301:           return FSI_FBatchPosition3dYUVACxformMul;
        case 302:           return FSI_FBatchPosition3dYUVACxformEAlpha;
        case 303:           return FSI_FBatchPosition3dYUVACxformEAlphaMul;
        case 304:           return FSI_FInstancedYUV;
        case 305:           return FSI_FInstancedYUVMul;
        case 306:           return FSI_FInstancedYUVEAlpha;
        case 307:           return FSI_FInstancedYUVEAlphaMul;
        case 308:           return FSI_FInstancedYUVCxform;
        case 309:           return FSI_FInstancedYUVCxformMul;
        case 310:           return FSI_FInstancedYUVCxformEAlpha;
        case 311:           return FSI_FInstancedYUVCxformEAlphaMul;
        case 312:           return FSI_FInstancedYUVA;
        case 313:           return FSI_FInstancedYUVAMul;
        case 314:           return FSI_FInstancedYUVAEAlpha;
        case 315:           return FSI_FInstancedYUVAEAlphaMul;
        case 316:           return FSI_FInstancedYUVACxform;
        case 317:           return FSI_FInstancedYUVACxformMul;
        case 318:           return FSI_FInstancedYUVACxformEAlpha;
        case 319:           return FSI_FInstancedYUVACxformEAlphaMul;
        case 320:           return FSI_FInstancedPosition3dYUV;
        case 321:           return FSI_FInstancedPosition3dYUVMul;
        case 322:           return FSI_FInstancedPosition3dYUVEAlpha;
        case 323:           return FSI_FInstancedPosition3dYUVEAlphaMul;
        case 324:           return FSI_FInstancedPosition3dYUVCxform;
        case 325:           return FSI_FInstancedPosition3dYUVCxformMul;
        case 326:           return FSI_FInstancedPosition3dYUVCxformEAlpha;
        case 327:           return FSI_FInstancedPosition3dYUVCxformEAlphaMul;
        case 328:           return FSI_FInstancedPosition3dYUVA;
        case 329:           return FSI_FInstancedPosition3dYUVAMul;
        case 330:           return FSI_FInstancedPosition3dYUVAEAlpha;
        case 331:           return FSI_FInstancedPosition3dYUVAEAlphaMul;
        case 332:           return FSI_FInstancedPosition3dYUVACxform;
        case 333:           return FSI_FInstancedPosition3dYUVACxformMul;
        case 334:           return FSI_FInstancedPosition3dYUVACxformEAlpha;
        case 335:           return FSI_FInstancedPosition3dYUVACxformEAlphaMul;
        case 336:           return FSI_FTexTGCMatrixAc;
        case 337:           return FSI_FTexTGCMatrixAcMul;
        case 338:           return FSI_FTexTGCMatrixAcEAlpha;
        case 339:           return FSI_FTexTGCMatrixAcEAlphaMul;
        case 340:           return FSI_FBox1Blur;
        case 341:           return FSI_FBox1BlurMul;
        case 342:           return FSI_FBox2Blur;
        case 343:           return FSI_FBox2BlurMul;
        case 344:           return FSI_FBox2Shadow;
        case 345:           return FSI_FBox2ShadowMul;
        case 346:           return FSI_FBox2ShadowKnockout;
        case 347:           return FSI_FBox2ShadowKnockoutMul;
        case 348:           return FSI_FBox2InnerShadow;
        case 349:           return FSI_FBox2InnerShadowMul;
        case 350:           return FSI_FBox2InnerShadowKnockout;
        case 351:           return FSI_FBox2InnerShadowKnockoutMul;
        case 352:           return FSI_FBox2Shadowonly;
        case 353:           return FSI_FBox2ShadowonlyMul;
        case 354:           return FSI_FBox2ShadowHighlight;
        case 355:           return FSI_FBox2ShadowHighlightMul;
        case 356:           return FSI_FBox2ShadowHighlightKnockout;
        case 357:           return FSI_FBox2ShadowHighlightKnockoutMul;
        case 358:           return FSI_FBox2InnerShadowHighlight;
        case 359:           return FSI_FBox2InnerShadowHighlightMul;
        case 360:           return FSI_FBox2InnerShadowHighlightKnockout;
        case 361:           return FSI_FBox2InnerShadowHighlightKnockoutMul;
        case 362:           return FSI_FBox2ShadowonlyHighlight;
        case 363:           return FSI_FBox2ShadowonlyHighlightMul;
        case 364:           return FSI_FDrawableCopyPixels;
        case 365:           return FSI_FDrawableCopyPixelsCopyLerp;
        case 366:           return FSI_FDrawableCopyPixelsNoDestAlpha;
        case 367:           return FSI_FDrawableCopyPixelsNoDestAlphaCopyLerp;
        case 368:           return FSI_FDrawableCopyPixelsMergeAlpha;
        case 369:           return FSI_FDrawableCopyPixelsMergeAlphaCopyLerp;
        case 370:           return FSI_FDrawableCopyPixelsMergeAlphaNoDestAlpha;
        case 371:           return FSI_FDrawableCopyPixelsMergeAlphaNoDestAlphaCopyLerp;
        case 372:           return FSI_FDrawableCopyPixelsAlpha;
        case 373:           return FSI_FDrawableCopyPixelsAlphaCopyLerp;
        case 374:           return FSI_FDrawableCopyPixelsAlphaNoDestAlpha;
        case 375:           return FSI_FDrawableCopyPixelsAlphaNoDestAlphaCopyLerp;
        case 376:           return FSI_FDrawableCopyPixelsAlphaMergeAlpha;
        case 377:           return FSI_FDrawableCopyPixelsAlphaMergeAlphaCopyLerp;
        case 378:           return FSI_FDrawableCopyPixelsAlphaMergeAlphaNoDestAlpha;
        case 379:           return FSI_FDrawableCopyPixelsAlphaMergeAlphaNoDestAlphaCopyLerp;
        case 380:           return FSI_FDrawableMerge;
        case 381:           return FSI_FDrawableCompare;
        case 382:           return FSI_FDrawablePaletteMap;
        default: SF_ASSERT(0); return FSI_None;
    }
}

unsigned FragShaderDesc::GetShaderComboIndex(ShaderType shader)
{
    switch(shader)
    {
        case FS_FTexTG:                                                  return 0;
        case FS_FTexTGMul:                                               return 1;
        case FS_FBatchTexTG:                                             return 64;
        case FS_FBatchTexTGMul:                                          return 65;
        case FS_FInstancedTexTG:                                         return 128;
        case FS_FInstancedTexTGMul:                                      return 129;
        case FS_FPosition3dTexTG:                                        return 32;
        case FS_FPosition3dTexTGMul:                                     return 33;
        case FS_FBatchPosition3dTexTG:                                   return 96;
        case FS_FBatchPosition3dTexTGMul:                                return 97;
        case FS_FInstancedPosition3dTexTG:                               return 160;
        case FS_FInstancedPosition3dTexTGMul:                            return 161;
        case FS_FTexTGCxform:                                            return 4;
        case FS_FTexTGCxformMul:                                         return 5;
        case FS_FBatchTexTGCxform:                                       return 68;
        case FS_FBatchTexTGCxformMul:                                    return 69;
        case FS_FInstancedTexTGCxform:                                   return 132;
        case FS_FInstancedTexTGCxformMul:                                return 133;
        case FS_FPosition3dTexTGCxform:                                  return 36;
        case FS_FPosition3dTexTGCxformMul:                               return 37;
        case FS_FBatchPosition3dTexTGCxform:                             return 100;
        case FS_FBatchPosition3dTexTGCxformMul:                          return 101;
        case FS_FInstancedPosition3dTexTGCxform:                         return 164;
        case FS_FInstancedPosition3dTexTGCxformMul:                      return 165;
        case FS_FTexTGEAlpha:                                            return 2;
        case FS_FTexTGEAlphaMul:                                         return 3;
        case FS_FBatchTexTGEAlpha:                                       return 66;
        case FS_FBatchTexTGEAlphaMul:                                    return 67;
        case FS_FInstancedTexTGEAlpha:                                   return 130;
        case FS_FInstancedTexTGEAlphaMul:                                return 131;
        case FS_FPosition3dTexTGEAlpha:                                  return 34;
        case FS_FPosition3dTexTGEAlphaMul:                               return 35;
        case FS_FBatchPosition3dTexTGEAlpha:                             return 98;
        case FS_FBatchPosition3dTexTGEAlphaMul:                          return 99;
        case FS_FInstancedPosition3dTexTGEAlpha:                         return 162;
        case FS_FInstancedPosition3dTexTGEAlphaMul:                      return 163;
        case FS_FTexTGCxformEAlpha:                                      return 6;
        case FS_FTexTGCxformEAlphaMul:                                   return 7;
        case FS_FBatchTexTGCxformEAlpha:                                 return 70;
        case FS_FBatchTexTGCxformEAlphaMul:                              return 71;
        case FS_FInstancedTexTGCxformEAlpha:                             return 134;
        case FS_FInstancedTexTGCxformEAlphaMul:                          return 135;
        case FS_FPosition3dTexTGCxformEAlpha:                            return 38;
        case FS_FPosition3dTexTGCxformEAlphaMul:                         return 39;
        case FS_FBatchPosition3dTexTGCxformEAlpha:                       return 102;
        case FS_FBatchPosition3dTexTGCxformEAlphaMul:                    return 103;
        case FS_FInstancedPosition3dTexTGCxformEAlpha:                   return 166;
        case FS_FInstancedPosition3dTexTGCxformEAlphaMul:                return 167;
        case FS_FVertex:                                                 return 8;
        case FS_FVertexMul:                                              return 9;
        case FS_FBatchVertex:                                            return 72;
        case FS_FBatchVertexMul:                                         return 73;
        case FS_FInstancedVertex:                                        return 136;
        case FS_FInstancedVertexMul:                                     return 137;
        case FS_FPosition3dVertex:                                       return 40;
        case FS_FPosition3dVertexMul:                                    return 41;
        case FS_FBatchPosition3dVertex:                                  return 104;
        case FS_FBatchPosition3dVertexMul:                               return 105;
        case FS_FInstancedPosition3dVertex:                              return 168;
        case FS_FInstancedPosition3dVertexMul:                           return 169;
        case FS_FVertexCxform:                                           return 12;
        case FS_FVertexCxformMul:                                        return 13;
        case FS_FBatchVertexCxform:                                      return 76;
        case FS_FBatchVertexCxformMul:                                   return 77;
        case FS_FInstancedVertexCxform:                                  return 140;
        case FS_FInstancedVertexCxformMul:                               return 141;
        case FS_FPosition3dVertexCxform:                                 return 44;
        case FS_FPosition3dVertexCxformMul:                              return 45;
        case FS_FBatchPosition3dVertexCxform:                            return 108;
        case FS_FBatchPosition3dVertexCxformMul:                         return 109;
        case FS_FInstancedPosition3dVertexCxform:                        return 172;
        case FS_FInstancedPosition3dVertexCxformMul:                     return 173;
        case FS_FVertexEAlpha:                                           return 10;
        case FS_FVertexEAlphaMul:                                        return 11;
        case FS_FBatchVertexEAlpha:                                      return 74;
        case FS_FBatchVertexEAlphaMul:                                   return 75;
        case FS_FInstancedVertexEAlpha:                                  return 138;
        case FS_FInstancedVertexEAlphaMul:                               return 139;
        case FS_FPosition3dVertexEAlpha:                                 return 42;
        case FS_FPosition3dVertexEAlphaMul:                              return 43;
        case FS_FBatchPosition3dVertexEAlpha:                            return 106;
        case FS_FBatchPosition3dVertexEAlphaMul:                         return 107;
        case FS_FInstancedPosition3dVertexEAlpha:                        return 170;
        case FS_FInstancedPosition3dVertexEAlphaMul:                     return 171;
        case FS_FVertexCxformEAlpha:                                     return 14;
        case FS_FVertexCxformEAlphaMul:                                  return 15;
        case FS_FBatchVertexCxformEAlpha:                                return 78;
        case FS_FBatchVertexCxformEAlphaMul:                             return 79;
        case FS_FInstancedVertexCxformEAlpha:                            return 142;
        case FS_FInstancedVertexCxformEAlphaMul:                         return 143;
        case FS_FPosition3dVertexCxformEAlpha:                           return 46;
        case FS_FPosition3dVertexCxformEAlphaMul:                        return 47;
        case FS_FBatchPosition3dVertexCxformEAlpha:                      return 110;
        case FS_FBatchPosition3dVertexCxformEAlphaMul:                   return 111;
        case FS_FInstancedPosition3dVertexCxformEAlpha:                  return 174;
        case FS_FInstancedPosition3dVertexCxformEAlphaMul:               return 175;
        case FS_FTexTGTexTG:                                             return 16;
        case FS_FTexTGTexTGMul:                                          return 17;
        case FS_FBatchTexTGTexTG:                                        return 80;
        case FS_FBatchTexTGTexTGMul:                                     return 81;
        case FS_FInstancedTexTGTexTG:                                    return 144;
        case FS_FInstancedTexTGTexTGMul:                                 return 145;
        case FS_FPosition3dTexTGTexTG:                                   return 48;
        case FS_FPosition3dTexTGTexTGMul:                                return 49;
        case FS_FBatchPosition3dTexTGTexTG:                              return 112;
        case FS_FBatchPosition3dTexTGTexTGMul:                           return 113;
        case FS_FInstancedPosition3dTexTGTexTG:                          return 176;
        case FS_FInstancedPosition3dTexTGTexTGMul:                       return 177;
        case FS_FTexTGTexTGCxform:                                       return 20;
        case FS_FTexTGTexTGCxformMul:                                    return 21;
        case FS_FBatchTexTGTexTGCxform:                                  return 84;
        case FS_FBatchTexTGTexTGCxformMul:                               return 85;
        case FS_FInstancedTexTGTexTGCxform:                              return 148;
        case FS_FInstancedTexTGTexTGCxformMul:                           return 149;
        case FS_FPosition3dTexTGTexTGCxform:                             return 52;
        case FS_FPosition3dTexTGTexTGCxformMul:                          return 53;
        case FS_FBatchPosition3dTexTGTexTGCxform:                        return 116;
        case FS_FBatchPosition3dTexTGTexTGCxformMul:                     return 117;
        case FS_FInstancedPosition3dTexTGTexTGCxform:                    return 180;
        case FS_FInstancedPosition3dTexTGTexTGCxformMul:                 return 181;
        case FS_FTexTGTexTGEAlpha:                                       return 18;
        case FS_FTexTGTexTGEAlphaMul:                                    return 19;
        case FS_FBatchTexTGTexTGEAlpha:                                  return 82;
        case FS_FBatchTexTGTexTGEAlphaMul:                               return 83;
        case FS_FInstancedTexTGTexTGEAlpha:                              return 146;
        case FS_FInstancedTexTGTexTGEAlphaMul:                           return 147;
        case FS_FPosition3dTexTGTexTGEAlpha:                             return 50;
        case FS_FPosition3dTexTGTexTGEAlphaMul:                          return 51;
        case FS_FBatchPosition3dTexTGTexTGEAlpha:                        return 114;
        case FS_FBatchPosition3dTexTGTexTGEAlphaMul:                     return 115;
        case FS_FInstancedPosition3dTexTGTexTGEAlpha:                    return 178;
        case FS_FInstancedPosition3dTexTGTexTGEAlphaMul:                 return 179;
        case FS_FTexTGTexTGCxformEAlpha:                                 return 22;
        case FS_FTexTGTexTGCxformEAlphaMul:                              return 23;
        case FS_FBatchTexTGTexTGCxformEAlpha:                            return 86;
        case FS_FBatchTexTGTexTGCxformEAlphaMul:                         return 87;
        case FS_FInstancedTexTGTexTGCxformEAlpha:                        return 150;
        case FS_FInstancedTexTGTexTGCxformEAlphaMul:                     return 151;
        case FS_FPosition3dTexTGTexTGCxformEAlpha:                       return 54;
        case FS_FPosition3dTexTGTexTGCxformEAlphaMul:                    return 55;
        case FS_FBatchPosition3dTexTGTexTGCxformEAlpha:                  return 118;
        case FS_FBatchPosition3dTexTGTexTGCxformEAlphaMul:               return 119;
        case FS_FInstancedPosition3dTexTGTexTGCxformEAlpha:              return 182;
        case FS_FInstancedPosition3dTexTGTexTGCxformEAlphaMul:           return 183;
        case FS_FTexTGVertex:                                            return 24;
        case FS_FTexTGVertexMul:                                         return 25;
        case FS_FBatchTexTGVertex:                                       return 88;
        case FS_FBatchTexTGVertexMul:                                    return 89;
        case FS_FInstancedTexTGVertex:                                   return 152;
        case FS_FInstancedTexTGVertexMul:                                return 153;
        case FS_FPosition3dTexTGVertex:                                  return 56;
        case FS_FPosition3dTexTGVertexMul:                               return 57;
        case FS_FBatchPosition3dTexTGVertex:                             return 120;
        case FS_FBatchPosition3dTexTGVertexMul:                          return 121;
        case FS_FInstancedPosition3dTexTGVertex:                         return 184;
        case FS_FInstancedPosition3dTexTGVertexMul:                      return 185;
        case FS_FTexTGVertexCxform:                                      return 28;
        case FS_FTexTGVertexCxformMul:                                   return 29;
        case FS_FBatchTexTGVertexCxform:                                 return 92;
        case FS_FBatchTexTGVertexCxformMul:                              return 93;
        case FS_FInstancedTexTGVertexCxform:                             return 156;
        case FS_FInstancedTexTGVertexCxformMul:                          return 157;
        case FS_FPosition3dTexTGVertexCxform:                            return 60;
        case FS_FPosition3dTexTGVertexCxformMul:                         return 61;
        case FS_FBatchPosition3dTexTGVertexCxform:                       return 124;
        case FS_FBatchPosition3dTexTGVertexCxformMul:                    return 125;
        case FS_FInstancedPosition3dTexTGVertexCxform:                   return 188;
        case FS_FInstancedPosition3dTexTGVertexCxformMul:                return 189;
        case FS_FTexTGVertexEAlpha:                                      return 26;
        case FS_FTexTGVertexEAlphaMul:                                   return 27;
        case FS_FBatchTexTGVertexEAlpha:                                 return 90;
        case FS_FBatchTexTGVertexEAlphaMul:                              return 91;
        case FS_FInstancedTexTGVertexEAlpha:                             return 154;
        case FS_FInstancedTexTGVertexEAlphaMul:                          return 155;
        case FS_FPosition3dTexTGVertexEAlpha:                            return 58;
        case FS_FPosition3dTexTGVertexEAlphaMul:                         return 59;
        case FS_FBatchPosition3dTexTGVertexEAlpha:                       return 122;
        case FS_FBatchPosition3dTexTGVertexEAlphaMul:                    return 123;
        case FS_FInstancedPosition3dTexTGVertexEAlpha:                   return 186;
        case FS_FInstancedPosition3dTexTGVertexEAlphaMul:                return 187;
        case FS_FTexTGVertexCxformEAlpha:                                return 30;
        case FS_FTexTGVertexCxformEAlphaMul:                             return 31;
        case FS_FBatchTexTGVertexCxformEAlpha:                           return 94;
        case FS_FBatchTexTGVertexCxformEAlphaMul:                        return 95;
        case FS_FInstancedTexTGVertexCxformEAlpha:                       return 158;
        case FS_FInstancedTexTGVertexCxformEAlphaMul:                    return 159;
        case FS_FPosition3dTexTGVertexCxformEAlpha:                      return 62;
        case FS_FPosition3dTexTGVertexCxformEAlphaMul:                   return 63;
        case FS_FBatchPosition3dTexTGVertexCxformEAlpha:                 return 126;
        case FS_FBatchPosition3dTexTGVertexCxformEAlphaMul:              return 127;
        case FS_FInstancedPosition3dTexTGVertexCxformEAlpha:             return 190;
        case FS_FInstancedPosition3dTexTGVertexCxformEAlphaMul:          return 191;
        case FS_FSolid:                                                  return 192;
        case FS_FSolidMul:                                               return 193;
        case FS_FBatchSolid:                                             return 208;
        case FS_FBatchSolidMul:                                          return 209;
        case FS_FInstancedSolid:                                         return 224;
        case FS_FInstancedSolidMul:                                      return 225;
        case FS_FPosition3dSolid:                                        return 200;
        case FS_FPosition3dSolidMul:                                     return 201;
        case FS_FBatchPosition3dSolid:                                   return 216;
        case FS_FBatchPosition3dSolidMul:                                return 217;
        case FS_FInstancedPosition3dSolid:                               return 232;
        case FS_FInstancedPosition3dSolidMul:                            return 233;
        case FS_FText:                                                   return 194;
        case FS_FTextMul:                                                return 195;
        case FS_FBatchText:                                              return 210;
        case FS_FBatchTextMul:                                           return 211;
        case FS_FInstancedText:                                          return 226;
        case FS_FInstancedTextMul:                                       return 227;
        case FS_FPosition3dText:                                         return 202;
        case FS_FPosition3dTextMul:                                      return 203;
        case FS_FBatchPosition3dText:                                    return 218;
        case FS_FBatchPosition3dTextMul:                                 return 219;
        case FS_FInstancedPosition3dText:                                return 234;
        case FS_FInstancedPosition3dTextMul:                             return 235;
        case FS_FTextColor:                                              return 196;
        case FS_FTextColorMul:                                           return 197;
        case FS_FBatchTextColor:                                         return 212;
        case FS_FBatchTextColorMul:                                      return 213;
        case FS_FInstancedTextColor:                                     return 228;
        case FS_FInstancedTextColorMul:                                  return 229;
        case FS_FPosition3dTextColor:                                    return 204;
        case FS_FPosition3dTextColorMul:                                 return 205;
        case FS_FBatchPosition3dTextColor:                               return 220;
        case FS_FBatchPosition3dTextColorMul:                            return 221;
        case FS_FInstancedPosition3dTextColor:                           return 236;
        case FS_FInstancedPosition3dTextColorMul:                        return 237;
        case FS_FTextColorCxform:                                        return 198;
        case FS_FTextColorCxformMul:                                     return 199;
        case FS_FBatchTextColorCxform:                                   return 214;
        case FS_FBatchTextColorCxformMul:                                return 215;
        case FS_FInstancedTextColorCxform:                               return 230;
        case FS_FInstancedTextColorCxformMul:                            return 231;
        case FS_FPosition3dTextColorCxform:                              return 206;
        case FS_FPosition3dTextColorCxformMul:                           return 207;
        case FS_FBatchPosition3dTextColorCxform:                         return 222;
        case FS_FBatchPosition3dTextColorCxformMul:                      return 223;
        case FS_FInstancedPosition3dTextColorCxform:                     return 238;
        case FS_FInstancedPosition3dTextColorCxformMul:                  return 239;
        case FS_FYUV:                                                    return 240;
        case FS_FYUVMul:                                                 return 241;
        case FS_FBatchYUV:                                               return 272;
        case FS_FBatchYUVMul:                                            return 273;
        case FS_FInstancedYUV:                                           return 304;
        case FS_FInstancedYUVMul:                                        return 305;
        case FS_FPosition3dYUV:                                          return 256;
        case FS_FPosition3dYUVMul:                                       return 257;
        case FS_FBatchPosition3dYUV:                                     return 288;
        case FS_FBatchPosition3dYUVMul:                                  return 289;
        case FS_FInstancedPosition3dYUV:                                 return 320;
        case FS_FInstancedPosition3dYUVMul:                              return 321;
        case FS_FYUVCxform:                                              return 244;
        case FS_FYUVCxformMul:                                           return 245;
        case FS_FBatchYUVCxform:                                         return 276;
        case FS_FBatchYUVCxformMul:                                      return 277;
        case FS_FInstancedYUVCxform:                                     return 308;
        case FS_FInstancedYUVCxformMul:                                  return 309;
        case FS_FPosition3dYUVCxform:                                    return 260;
        case FS_FPosition3dYUVCxformMul:                                 return 261;
        case FS_FBatchPosition3dYUVCxform:                               return 292;
        case FS_FBatchPosition3dYUVCxformMul:                            return 293;
        case FS_FInstancedPosition3dYUVCxform:                           return 324;
        case FS_FInstancedPosition3dYUVCxformMul:                        return 325;
        case FS_FYUVEAlpha:                                              return 242;
        case FS_FYUVEAlphaMul:                                           return 243;
        case FS_FBatchYUVEAlpha:                                         return 274;
        case FS_FBatchYUVEAlphaMul:                                      return 275;
        case FS_FInstancedYUVEAlpha:                                     return 306;
        case FS_FInstancedYUVEAlphaMul:                                  return 307;
        case FS_FPosition3dYUVEAlpha:                                    return 258;
        case FS_FPosition3dYUVEAlphaMul:                                 return 259;
        case FS_FBatchPosition3dYUVEAlpha:                               return 290;
        case FS_FBatchPosition3dYUVEAlphaMul:                            return 291;
        case FS_FInstancedPosition3dYUVEAlpha:                           return 322;
        case FS_FInstancedPosition3dYUVEAlphaMul:                        return 323;
        case FS_FYUVCxformEAlpha:                                        return 246;
        case FS_FYUVCxformEAlphaMul:                                     return 247;
        case FS_FBatchYUVCxformEAlpha:                                   return 278;
        case FS_FBatchYUVCxformEAlphaMul:                                return 279;
        case FS_FInstancedYUVCxformEAlpha:                               return 310;
        case FS_FInstancedYUVCxformEAlphaMul:                            return 311;
        case FS_FPosition3dYUVCxformEAlpha:                              return 262;
        case FS_FPosition3dYUVCxformEAlphaMul:                           return 263;
        case FS_FBatchPosition3dYUVCxformEAlpha:                         return 294;
        case FS_FBatchPosition3dYUVCxformEAlphaMul:                      return 295;
        case FS_FInstancedPosition3dYUVCxformEAlpha:                     return 326;
        case FS_FInstancedPosition3dYUVCxformEAlphaMul:                  return 327;
        case FS_FYUVA:                                                   return 248;
        case FS_FYUVAMul:                                                return 249;
        case FS_FBatchYUVA:                                              return 280;
        case FS_FBatchYUVAMul:                                           return 281;
        case FS_FInstancedYUVA:                                          return 312;
        case FS_FInstancedYUVAMul:                                       return 313;
        case FS_FPosition3dYUVA:                                         return 264;
        case FS_FPosition3dYUVAMul:                                      return 265;
        case FS_FBatchPosition3dYUVA:                                    return 296;
        case FS_FBatchPosition3dYUVAMul:                                 return 297;
        case FS_FInstancedPosition3dYUVA:                                return 328;
        case FS_FInstancedPosition3dYUVAMul:                             return 329;
        case FS_FYUVACxform:                                             return 252;
        case FS_FYUVACxformMul:                                          return 253;
        case FS_FBatchYUVACxform:                                        return 284;
        case FS_FBatchYUVACxformMul:                                     return 285;
        case FS_FInstancedYUVACxform:                                    return 316;
        case FS_FInstancedYUVACxformMul:                                 return 317;
        case FS_FPosition3dYUVACxform:                                   return 268;
        case FS_FPosition3dYUVACxformMul:                                return 269;
        case FS_FBatchPosition3dYUVACxform:                              return 300;
        case FS_FBatchPosition3dYUVACxformMul:                           return 301;
        case FS_FInstancedPosition3dYUVACxform:                          return 332;
        case FS_FInstancedPosition3dYUVACxformMul:                       return 333;
        case FS_FYUVAEAlpha:                                             return 250;
        case FS_FYUVAEAlphaMul:                                          return 251;
        case FS_FBatchYUVAEAlpha:                                        return 282;
        case FS_FBatchYUVAEAlphaMul:                                     return 283;
        case FS_FInstancedYUVAEAlpha:                                    return 314;
        case FS_FInstancedYUVAEAlphaMul:                                 return 315;
        case FS_FPosition3dYUVAEAlpha:                                   return 266;
        case FS_FPosition3dYUVAEAlphaMul:                                return 267;
        case FS_FBatchPosition3dYUVAEAlpha:                              return 298;
        case FS_FBatchPosition3dYUVAEAlphaMul:                           return 299;
        case FS_FInstancedPosition3dYUVAEAlpha:                          return 330;
        case FS_FInstancedPosition3dYUVAEAlphaMul:                       return 331;
        case FS_FYUVACxformEAlpha:                                       return 254;
        case FS_FYUVACxformEAlphaMul:                                    return 255;
        case FS_FBatchYUVACxformEAlpha:                                  return 286;
        case FS_FBatchYUVACxformEAlphaMul:                               return 287;
        case FS_FInstancedYUVACxformEAlpha:                              return 318;
        case FS_FInstancedYUVACxformEAlphaMul:                           return 319;
        case FS_FPosition3dYUVACxformEAlpha:                             return 270;
        case FS_FPosition3dYUVACxformEAlphaMul:                          return 271;
        case FS_FBatchPosition3dYUVACxformEAlpha:                        return 302;
        case FS_FBatchPosition3dYUVACxformEAlphaMul:                     return 303;
        case FS_FInstancedPosition3dYUVACxformEAlpha:                    return 334;
        case FS_FInstancedPosition3dYUVACxformEAlphaMul:                 return 335;
        case FS_FTexTGCMatrixAc:                                         return 336;
        case FS_FTexTGCMatrixAcMul:                                      return 337;
        case FS_FTexTGCMatrixAcEAlpha:                                   return 338;
        case FS_FTexTGCMatrixAcEAlphaMul:                                return 339;
        case FS_FBox1Blur:                                               return 340;
        case FS_FBox1BlurMul:                                            return 341;
        case FS_FBox2Blur:                                               return 342;
        case FS_FBox2BlurMul:                                            return 343;
        case FS_FBox2Shadow:                                             return 344;
        case FS_FBox2ShadowMul:                                          return 345;
        case FS_FBox2ShadowKnockout:                                     return 346;
        case FS_FBox2ShadowKnockoutMul:                                  return 347;
        case FS_FBox2Shadowonly:                                         return 352;
        case FS_FBox2ShadowonlyMul:                                      return 353;
        case FS_FBox2InnerShadow:                                        return 348;
        case FS_FBox2InnerShadowMul:                                     return 349;
        case FS_FBox2InnerShadowKnockout:                                return 350;
        case FS_FBox2InnerShadowKnockoutMul:                             return 351;
        case FS_FBox2ShadowonlyHighlight:                                return 362;
        case FS_FBox2ShadowonlyHighlightMul:                             return 363;
        case FS_FBox2InnerShadowHighlight:                               return 358;
        case FS_FBox2InnerShadowHighlightMul:                            return 359;
        case FS_FBox2InnerShadowHighlightKnockout:                       return 360;
        case FS_FBox2InnerShadowHighlightKnockoutMul:                    return 361;
        case FS_FBox2ShadowHighlight:                                    return 354;
        case FS_FBox2ShadowHighlightMul:                                 return 355;
        case FS_FBox2ShadowHighlightKnockout:                            return 356;
        case FS_FBox2ShadowHighlightKnockoutMul:                         return 357;
        case FS_FDrawableCopyPixels:                                     return 364;
        case FS_FDrawableCopyPixelsCopyLerp:                             return 365;
        case FS_FDrawableCopyPixelsNoDestAlpha:                          return 366;
        case FS_FDrawableCopyPixelsNoDestAlphaCopyLerp:                  return 367;
        case FS_FDrawableCopyPixelsMergeAlpha:                           return 368;
        case FS_FDrawableCopyPixelsMergeAlphaCopyLerp:                   return 369;
        case FS_FDrawableCopyPixelsMergeAlphaNoDestAlpha:                return 370;
        case FS_FDrawableCopyPixelsMergeAlphaNoDestAlphaCopyLerp:        return 371;
        case FS_FDrawableCopyPixelsAlpha:                                return 372;
        case FS_FDrawableCopyPixelsAlphaCopyLerp:                        return 373;
        case FS_FDrawableCopyPixelsAlphaNoDestAlpha:                     return 374;
        case FS_FDrawableCopyPixelsAlphaNoDestAlphaCopyLerp:             return 375;
        case FS_FDrawableCopyPixelsAlphaMergeAlpha:                      return 376;
        case FS_FDrawableCopyPixelsAlphaMergeAlphaCopyLerp:              return 377;
        case FS_FDrawableCopyPixelsAlphaMergeAlphaNoDestAlpha:           return 378;
        case FS_FDrawableCopyPixelsAlphaMergeAlphaNoDestAlphaCopyLerp:   return 379;
        case FS_FDrawableMerge:                                          return 380;
        case FS_FDrawableCompare:                                        return 381;
        case FS_FDrawablePaletteMap:                                     return 382;
        default: SF_ASSERT(0); return 0;
    }
}

}}} // Scaleform::Render::GL

