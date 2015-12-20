//-----------------------------------------------------------------------------
// File: D3DFont.cpp
//
// Desc: Texture-based font class
//
// Copyright (c) 1999-2000 Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#include "r3dPCH.h"
#include "r3d.h"

//#define	RECREATE_BMP_FONT	1

#define OUT_TEXTURE_32BIT	1

#ifdef OUT_TEXTURE_32BIT	
  #define OUT_TEXTURE_TYPE	D3DFMT_A8R8G8B8
  #define OUT_TEXTURE_PSIZE	DWORD
#else
  #define OUT_TEXTURE_TYPE	D3DFMT_A4R4G4B4
  #define OUT_TEXTURE_PSIZE	WORD
#endif

CD3DFont	*_r3dSystemFont;
int		r3d_CD3DFont_Quality = ANTIALIASED_QUALITY;

// Return the next power of 2 higher than the input
// If the input is already a power of 2, the output will be the same as the input
//  Got this from Brian Sharp's sweng mailing list.
int r3dNextPowerOf2(int in)
{
  r3d_assert(in);
  
  in -= 1;

  in |= in >> 16;
  in |= in >> 8;
  in |= in >> 4;
  in |= in >> 2;
  in |= in >> 1;

  return in + 1;
}

HFONT r3dCreateHFont(HDC hDC, const char *fontName, int fontHeight, int bBold, int bItalic)
{
  int lpy     = 96; //calc font size regardless of current DPI - GetDeviceCaps(hDC, LOGPIXELSY);
  int nHeight = -MulDiv(fontHeight, lpy, 72);
  DWORD dwBold   = bBold   ? FW_BOLD : FW_NORMAL;
  DWORD dwItalic = bItalic ? TRUE    : FALSE;

  HFONT hFont = ::CreateFontA(
    nHeight,				// height of font
    0,					// average character width
    0,					// angle of escapement
    0,					// base-line orientation angle
    dwBold,				// font weight
    dwItalic,				// italic attribute option
    FALSE,				// underline attribute option
    FALSE,				// strikeout attribute option
    DEFAULT_CHARSET,			// character set identifier
    OUT_DEFAULT_PRECIS,			// output precision
    CLIP_DEFAULT_PRECIS,		// clipping precision
    ANTIALIASED_QUALITY,	        // output quality
    DEFAULT_PITCH | FF_DONTCARE,	// pitch and family
    fontName);

  if(NULL == hFont) {
    r3dError("unable to create %s font\n", fontName);
    return 0;
  }

  return hFont;
}

//-----------------------------------------------------------------------------
// Name: CD3DFont()
// Desc: Font class constructor
//-----------------------------------------------------------------------------
CD3DFont::CD3DFont(const r3dIntegrityGuardian& ig, const char* strFontName, DWORD dwHeight, DWORD dwFlags)
: r3dIResource( ig )
{
  r3dscpy(fnt_fontName, strFontName);
  fnt_fontHeight = dwHeight;
  fnt_fontFlags  = dwFlags;
   
  glyphTexScale  = 1.0f; // Draw fonts into texture without scaling
  glyphTexture   = NULL;

  fnt_pD3DXFont  = NULL;
}
    


//-----------------------------------------------------------------------------
// Name: ~CD3DFont()
// Desc: Font class destructor
//-----------------------------------------------------------------------------
CD3DFont::~CD3DFont()
{
  R3D_ENSURE_MAIN_THREAD();
  DeleteDeviceObjects();
}

void CD3DFont::D3DCreateResource()
{
  R3D_ENSURE_MAIN_THREAD();
  if(fnt_pD3DXFont) fnt_pD3DXFont->OnResetDevice();
  
  return;
}

void CD3DFont::D3DReleaseResource()
{
  R3D_ENSURE_MAIN_THREAD();
  if(fnt_pD3DXFont) fnt_pD3DXFont->OnLostDevice();

  return;
}

void CD3DFont::CreateD3DXFont()
{
    R3D_ENSURE_MAIN_THREAD();

    HDC hDC     = ::CreateCompatibleDC(NULL);
    int lpy     = 96; //calc font size regardless of current DPI - GetDeviceCaps(hDC, LOGPIXELSY);
    int nHeight = -MulDiv(fnt_fontHeight, lpy, 72);

    HRESULT hr;
    // (p.s. - nice comment formatting, isn't?)
    hr = D3DXCreateFontA(r3dRenderer->pd3ddev, 
      /* Height          */ nHeight, 
      /* Width           */ 0, 
      /* Weight          */ (fnt_fontFlags & D3DFONT_BOLD)   ? FW_BOLD : FW_NORMAL,
      /* MipLevels       */ 1, 
      /* Italic          */ (fnt_fontFlags & D3DFONT_ITALIC) ? TRUE : FALSE,
      /* CharSet         */ DEFAULT_CHARSET, 
      /* OutputPrecision */ OUT_DEFAULT_PRECIS, 
      /* Quality         */ r3d_CD3DFont_Quality, 
      /* PitchAndFamily  */ DEFAULT_PITCH | FF_DONTCARE, 
      /* pFaceName       */ fnt_fontName, 
      &fnt_pD3DXFont);
    
    if(S_OK != hr) {
      r3dError("Unable to create d3dxfont %s, hr=%d\n", fnt_fontName, hr);
    }
    
    ::DeleteDC(hDC);
  
    return;
}


void CD3DFont::WriteGlyphDefinition(const char* fname)
{
  FILE* f = fopen(fname, "wb");
  if(f == NULL) r3dError("can't open %s for writing", fname);

  char buf[MAX_PATH];
  sprintf(buf, "%s_%d_%x", fnt_fontName, fnt_fontHeight, fnt_fontFlags);

  fprintf(f, "struct CD3DFont::glyphData_s glyphData_%s = {\n", buf);
  fprintf(f, "  %ff,\n", glyphTexScale);
  for(int gc=0; gc<GlyphCacheSize; gc++) {
    fprintf(f, "  %ff, %ff, %ff, %ff,\n", glyphTexCoords[gc][0], glyphTexCoords[gc][1], glyphTexCoords[gc][2], glyphTexCoords[gc][3]);
  }
  fprintf(f, "};\n");
  
  fclose(f);
  return;
}

HRESULT	CD3DFont::CreateFromDefinition(const glyphData_s& gl)
{
  r3dOutToLog("r3dFont: Creating %s %dpt from definition\n", fnt_fontName, fnt_fontHeight);
  CLOG_INDENT;
  

  // create id3dxfont. 
  CreateD3DXFont();
  
  assert(glyphTexture == NULL);

  char buf[MAX_PATH];
  sprintf(buf, "Data\\Fonts\\%s_%d_%x.tga", fnt_fontName, fnt_fontHeight, fnt_fontFlags);
  glyphTexture = r3dRenderer->LoadTexture(buf, OUT_TEXTURE_TYPE);
  
  glyphTexScale = gl.texScale;
  
  for(int gc=0; gc<GlyphCacheSize; gc++) {
    glyphTexCoords[gc][0] = gl.tx[gc][0];
    glyphTexCoords[gc][1] = gl.tx[gc][1];
    glyphTexCoords[gc][2] = gl.tx[gc][2];
    glyphTexCoords[gc][3] = gl.tx[gc][3];
  };

  return S_OK;
}


void CD3DFont::LoadGlyphsFromFile()
{
  r3d_assert(!glyphTexture);
  
  // load font texcoords data
  char FN[MAX_PATH];
  sprintf(FN, "Data\\Fonts\\%s_%d_%x.dat", fnt_fontName, fnt_fontHeight, fnt_fontFlags);
  if(r3d_access(FN, 0) != 0)
    return;
    
  r3dFile* f = r3d_open(FN, "rt");
  if(!f) return; //FIXME: throw error here. 

  char buf[1024];
  fgets(buf, sizeof(buf), f);
  sscanf(buf, "%f\n", &glyphTexScale); 

  for(int gc=0; gc<GlyphCacheSize; gc++) {
    fgets(buf, sizeof(buf), f);
    sscanf(buf, "%f %f %f %f\n", 
      &glyphTexCoords[gc][0], 
      &glyphTexCoords[gc][1], 
      &glyphTexCoords[gc][2], 
      &glyphTexCoords[gc][3]);
  }
  fclose(f);

  sprintf(FN, "Data\\Fonts\\%s_%d_%x.tga", fnt_fontName, fnt_fontHeight, fnt_fontFlags);
  glyphTexture = r3dRenderer->LoadTexture(FN, OUT_TEXTURE_TYPE);

  return;
}

void CD3DFont::SaveGlyphsToFile()
{
  r3d_assert(glyphTexture);

  char fname[MAX_PATH];

  _mkdir("Data\\Fonts");
  sprintf(fname, "Data\\Fonts\\%s_%d_%x.tga", fnt_fontName, fnt_fontHeight, fnt_fontFlags);
  
  // save texture as TGA
  {
    IDirect3DSurface9* surf0;
    glyphTexture->AsTex2D()->GetSurfaceLevel(0, &surf0);
    D3DXSaveSurfaceToFile(fname, D3DXIFF_TGA, surf0, NULL, NULL);
    surf0->Release();
  }

  sprintf(fname, "Data\\Fonts\\%s_%d_%x.dat", fnt_fontName, fnt_fontHeight, fnt_fontFlags);
  FILE *f = fopen_for_write(fname,"wt");
  r3d_assert(f);
  if(!f) 
    return;

  fprintf(f, "%f\n", glyphTexScale); 

  for(int gc=0; gc<GlyphCacheSize; gc++) {
    fprintf(f, "%1.8f %1.8f %1.8f %1.8f\n", 
     glyphTexCoords[gc][0], 
     glyphTexCoords[gc][1], 
     glyphTexCoords[gc][2], 
     glyphTexCoords[gc][3]);
  }

  fclose(f);
  
  return;
}

void CD3DFont::CreateGlyphs(int cellOffX1, int cellOffY1, int cellOffX2, int cellOffY2)
{
  r3d_assert(!glyphTexture);

  R3D_ENSURE_MAIN_THREAD();

  const int CHAR_SAFE_OFFSET = 2;	// actual glyph will start after this number of pixels

  // total cell size.
  int	cellAddW  = cellOffX1 + cellOffX2 + (CHAR_SAFE_OFFSET*2);
  int	cellAddH  = cellOffY1 + cellOffY2 + (CHAR_SAFE_OFFSET*2);

  DWORD m_dwTexWidth  = 0;
  DWORD m_dwTexHeight = 0;
  
  HFONT hFont = 0;

 // Create a DC
  HDC hDC = CreateCompatibleDC(NULL);
  
  // loop until we find correct scale for the font
  while(1) {
    int  bBold   = (fnt_fontFlags & D3DFONT_BOLD)   ? true : false;
    int  bItalic = (fnt_fontFlags & D3DFONT_ITALIC) ? true : false;
    hFont = r3dCreateHFont(hDC, fnt_fontName, (int)((float)fnt_fontHeight * glyphTexScale), bBold, bItalic);

    SelectObject(hDC, hFont);

    // calc how many pixels will be in that font, and calc texture size
    unsigned int numpix = 0;

    char str[2] = "x";
    SIZE size;
    for(int gc=0; gc<GlyphCacheSize; gc++) {
      str[0] = (char)(gc + GlyphCodeCacheMin);
      ::GetTextExtentPoint32A(hDC, str, 1, &size);
      
      // need 2 pixels between them
      numpix += (size.cx + cellAddW) * (size.cy + cellAddH);
    }
    
    // we need to increase number of pixels a little to avoid glyphs moving to next line, leaving unused space
    m_dwTexWidth  = r3dNextPowerOf2((int)sqrtf((float)numpix * 1.1f));
    m_dwTexHeight = m_dwTexWidth;

    // If requested texture is too big for d3d, or just too big (over 1024)
    // use a smaller texture and smaller font, and scale up when rendering.
    D3DCAPS9 d3dCaps;
    r3dRenderer->pd3ddev->GetDeviceCaps(&d3dCaps);
    if(m_dwTexWidth > d3dCaps.MaxTextureWidth)
	{
      glyphTexScale  = (float)d3dCaps.MaxTextureWidth / (float)m_dwTexWidth;
      m_dwTexWidth  = d3dCaps.MaxTextureWidth;
      m_dwTexHeight = d3dCaps.MaxTextureHeight;
      
      ::DeleteObject(hFont);
      continue;
    }

    if(m_dwTexWidth > 1024) {
      glyphTexScale  = (float)1024 / (float)m_dwTexWidth;
      m_dwTexWidth  = 1024;
      m_dwTexHeight = 1024;
      
      ::DeleteObject(hFont);
      continue;
    }
    
    // check if we can fit into half of texture
    if(m_dwTexWidth * (m_dwTexHeight / 2) > numpix) {
      m_dwTexHeight /= 2;
    }

    r3d_assert(numpix < m_dwTexWidth * m_dwTexHeight);
  
    break;
  }

  // Set text properties
  SetTextColor( hDC, RGB(255,255,255));
  SetBkColor(   hDC, 0x00000000);
  SetTextAlign( hDC, TA_TOP);

 // allocate memory bitmap
  BITMAPINFO bmi;
  ZeroMemory(&bmi.bmiHeader, sizeof(BITMAPINFOHEADER));
  bmi.bmiHeader.biSize        = sizeof(BITMAPINFOHEADER);
  bmi.bmiHeader.biWidth       =  (int)m_dwTexWidth;
  bmi.bmiHeader.biHeight      = -(int)m_dwTexHeight;
  bmi.bmiHeader.biPlanes      = 1;
  bmi.bmiHeader.biCompression = BI_RGB;
  bmi.bmiHeader.biBitCount    = 32;

  const DWORD* pBitmapBits;
  HBITMAP hbmBitmap = CreateDIBSection(hDC, &bmi, DIB_RGB_COLORS, (VOID**)&pBitmapBits, NULL, 0 );
  SelectObject(hDC, hbmBitmap);
  SetMapMode(hDC, MM_TEXT);
  HPEN hPen = ::CreatePen(PS_SOLID, 1, RGB(0, 0, 255));
  SelectObject(hDC, hPen);

  // Loop through all printable character and output them to the bitmap..
  // Meanwhile, keep track of the corresponding tex coords for each character.
  DWORD x = 0;
  DWORD y = 0;
  char  str[2] = "x";
  SIZE  size;

  for(int gc=0; gc<GlyphCacheSize; gc++) {
    str[0] = (char)(gc + GlyphCodeCacheMin);
    GetTextExtentPoint32A(hDC, str, 1, &size);

    if((DWORD)(x+size.cx+cellAddW) >= m_dwTexWidth) {
      x  = 0;
      y += cellAddH + size.cy;
      r3d_assert(y < m_dwTexHeight);
    }

    // start of actual font cell
    int cx = x+CHAR_SAFE_OFFSET;
    int cy = y+CHAR_SAFE_OFFSET;
    ExtTextOut(hDC, cx+cellOffX1, cy+cellOffY1, ETO_OPAQUE, NULL, str, 1, NULL );

    glyphTexCoords[gc][0] = ((float)(cx))/m_dwTexWidth;
    glyphTexCoords[gc][1] = ((float)(cy))/m_dwTexHeight;
    glyphTexCoords[gc][2] = ((float)(cx+size.cx+cellOffX1+cellOffX2))/m_dwTexWidth;
    glyphTexCoords[gc][3] = ((float)(cy+size.cy+cellOffY1+cellOffY2))/m_dwTexHeight;
    
    MoveToEx(hDC, x, y, NULL);
    LineTo(hDC, x+cellAddW+size.cx, y);
    LineTo(hDC, x+cellAddW+size.cx, y+cellAddH+size.cy);
    LineTo(hDC, x, y+cellAddH+size.cy);
    LineTo(hDC, x, y);

    x += cellAddW + size.cx;
  }

 // allocate texture
  glyphTexture = r3dRenderer->AllocateTexture();
  glyphTexture->Create(m_dwTexWidth, m_dwTexHeight, OUT_TEXTURE_TYPE, 1);

  // Lock the surface and write the alpha values for the set pixels
  {
	  D3DLOCKED_RECT d3dlr;
	  glyphTexture->AsTex2D()->LockRect(0, &d3dlr, 0, 0);
	  OUT_TEXTURE_PSIZE* pDst32 = (OUT_TEXTURE_PSIZE*)d3dlr.pBits;

	  // DC bits contains rendered white font with antialiasing. DC does not contain alpha, so we should convert shades of gray to alpha
	  // our target font image will be white, with alpha emulating aliasing
	  for(y=0; y < m_dwTexHeight; y++) {
		  for(x=0; x < m_dwTexWidth; x++) {
#ifdef OUT_TEXTURE_32BIT
			  DWORD clr = pBitmapBits[m_dwTexWidth*y + x];
			  if(clr == 0x000000ff) clr &= ~0xFF000000;
			  else if(clr) clr = ((clr&0xFF) << 24) | 0x00FFFFFF;
			  pDst32[x] = clr;
#else
			  BYTE *row8 = (BYTE*)&pBitmapBits[m_dwTexWidth*y + x];
			  if(*(DWORD*)row8 == 0x000000ff) pDst32[x] = (WORD)r3dColorConv::MakeA4R4G4B4(0, row8[2], row8[1], row8[0]);
			  else if(row8[0]) pDst32[x] = (WORD)r3dColorConv::MakeA4R4G4B4(row8[0], 255, 255, 255);
			  else        pDst32[x] = 0;
#endif
		  }

		  pDst32 += d3dlr.Pitch / sizeof(OUT_TEXTURE_PSIZE);
	  }

	  // Done updating texture, so clean up used objects
	  glyphTexture->AsTex2D()->UnlockRect(0);
  }

 // clean up
  DeleteObject(hbmBitmap);
  DeleteDC(hDC);
  DeleteObject(hFont);
  DeleteObject(hPen);

  return;
}

//-----------------------------------------------------------------------------
// Desc: Initializes device-dependent objects, including the vertex buffer used
//       for rendering text and the texture map which stores the font image.
//-----------------------------------------------------------------------------
HRESULT CD3DFont::CreateSystemFont(bool forceRecreate, int cellOffX1, int cellOffY1, int cellOffX2, int cellOffY2)
{
#ifdef RECREATE_BMP_FONT
	forceRecreate = true;
#endif

	r3dOutToLog("r3dFont: Creating %s %dpt\n", fnt_fontName, fnt_fontHeight);

	// create id3dxfont. 
	CreateD3DXFont();

	if(fnt_fontFlags & D3DFONT_SKIPGLYPH) 
	{
		r3dOutToLog("r3dFont: Finished creating %s %dpt\n", fnt_fontName, fnt_fontHeight);
		return S_OK;
	}

	if(!forceRecreate) 
	{
		LoadGlyphsFromFile();
		if(glyphTexture) 
		{
			r3dOutToLog("r3dFont: Finished creating %s %dpt\n", fnt_fontName, fnt_fontHeight);
			return S_OK;
		}
		forceRecreate = 1;
	}

	CreateGlyphs(cellOffX1, cellOffY1, cellOffX2, cellOffY2);

	if(forceRecreate)
		SaveGlyphsToFile();

	r3dOutToLog("r3dFont: Finished creating %s %dpt\n", fnt_fontName, fnt_fontHeight);
	return S_OK;
}

//-----------------------------------------------------------------------------
// Name: DeleteDeviceObjects()
// Desc: Destroys all device-dependent objects
//-----------------------------------------------------------------------------
HRESULT CD3DFont::DeleteDeviceObjects()
{
    R3D_ENSURE_MAIN_THREAD();
    r3dRenderer->DeleteTexture(glyphTexture);

	SAFE_RELEASE( fnt_pD3DXFont );

    return S_OK;
}

const char* CD3DFont::ParseSpecialCodes(const char* in)
{
  if(in[0] != '#') 
    return in;

  switch(in[1]) 
  {
    case 'c':
    {
      // color code - in form of #cFF00FF (6 hex digits)
      char clrstr[7];
      memcpy(clrstr, in+2, 6);
      clrstr[6] = 0;
      
      DWORD clr;
      sscanf(clrstr, "%x", &clr);
      clr |= 0xFF000000;
      
      m_curColor.SetPacked(clr);
      return in + 8;
    }
      
      
    default:
      // wrong special code - pretend it wasn't code at all
      return in;
  }
}


//-----------------------------------------------------------------------------
// Name: GetTextExtent()
// Desc: Get the dimensions of a text string
//-----------------------------------------------------------------------------
HRESULT CD3DFont::GetTextExtent(const char* strText, SIZE* pSize)
{
    if(NULL==strText || NULL==pSize)
        return E_FAIL;

	if(glyphTexture == NULL) 
	{
		RECT  r = { 0 };

		DWORD flags = DT_CALCRECT;

		fnt_pD3DXFont->DrawTextA( NULL, strText, strlen(strText), &r, flags, 0 );

		pSize->cx = r.right - r.left ;
		pSize->cy = r.bottom - r.top ;

		return S_OK ;
	}

    float texWidth   = (float)glyphTexture->GetWidth();
    float texHeight  = (float)glyphTexture->GetHeight();

    float fRowWidth  = 0.0f;
    float fRowHeight = (glyphTexCoords[0][3]-glyphTexCoords[0][1])*texHeight;
    float fWidth     = 0.0f;
    float fHeight    = fRowHeight;

    while(*strText)
    {
	strText = ParseSpecialCodes(strText);
        char c = *strText++;

        if(c == '\n')
        {
            fRowWidth = 0.0f;
            fHeight  += fRowHeight;
        }
        if(c < ' ')
            continue;

        float tx1 = glyphTexCoords[c-32][0];
        float tx2 = glyphTexCoords[c-32][2];

        fRowWidth += (tx2-tx1)*texWidth/glyphTexScale;

        if(fRowWidth > fWidth)
            fWidth = fRowWidth;
    }

    pSize->cx = (int)fWidth;
    pSize->cy = (int)fHeight;

    return S_OK;
}


void _DrawChar(float sx, float sy, float w, float h, float tx1, float ty1, float tx2, float ty2, const r3dColor24& clr)
{
 R3D_SCREEN_VERTEX 	V[4];
 int		i;

 for(i=0; i<4; i++)
 {
  V[i].color = clr.GetPacked();
  V[i].z     = r3dRenderer->GetNearPlaneZValue();
  V[i].rwh   = 1;
 }

 V[0].x   = ceilf(sx);  	V[0].y   = ceilf(sy);
 V[1].x   = ceilf(sx+w);  	V[1].y   = ceilf(sy);
 V[2].x   = ceilf(sx+w);  	V[2].y   = ceilf(sy+h);
 V[3].x   = ceilf(sx);  	V[3].y   = ceilf(sy+h);

 V[0].tu = tx1; 	V[0].tv = ty1; 
 V[1].tu = tx2; 	V[1].tv = ty1;
 V[2].tu = tx2; 	V[2].tv = ty2;
 V[3].tu = tx1; 	V[3].tv = ty2;
  
 r3dRenderer->Render2DPolygon(4, V);
}

static const int CharBufferSize = 8192;
static R3D_SCREEN_VERTEX _V[CharBufferSize];
static int nVerts=0;
void _DrawChar(int idx, float sx, float sy, float w, float h, float tx1, float ty1, float tx2, float ty2, const r3dColor24& clr)
{
	if(idx+4 > CharBufferSize) r3dError("_DrawChar buffer overflow\n");

	R3D_SCREEN_VERTEX* V = &_V[idx];

	for(int i=0; i<4; i++)
	{
		V[i].color = clr.GetPacked();
		V[i].z     = r3dRenderer->GetNearPlaneZValue();
		V[i].rwh   = 1;
	}

	V[0].x   = ceilf(sx);  	V[0].y   = ceilf(sy);
	V[1].x   = ceilf(sx+w);  	V[1].y   = ceilf(sy);
	V[2].x   = ceilf(sx+w);  	V[2].y   = ceilf(sy+h);
	V[3].x   = ceilf(sx);  	V[3].y   = ceilf(sy+h);

	V[0].tu = tx1; 	V[0].tv = ty1;
	V[1].tu = tx2; 	V[1].tv = ty1;
	V[2].tu = tx2; 	V[2].tv = ty2;
	V[3].tu = tx1; 	V[3].tv = ty2;
	nVerts += 4;
}



//-----------------------------------------------------------------------------
// Name: DrawText()
// Desc: Draws 2D text
//-----------------------------------------------------------------------------
void CD3DFont::DrawText(float sx, float sy, float sw, float sh, const r3dColor24& Color, const char* strText, DWORD dwFlags)
{
  // Setup renderstate
  r3dRenderer->Flush();

  if(glyphTexture == NULL) 
  {
    RECT  r;
    r.left   = (long)sx;
    r.top    = (long)sy;
    r.right  = r.left + (long)sw;
    r.bottom = r.top  + (long)sh;
    
    DWORD flags = DT_NOCLIP;
    if(dwFlags & D3DFONT_CENTERED) {
      // clip in specified rectangle
      flags = DT_CENTER | DT_VCENTER | DT_WORDBREAK;
    }
  
    fnt_pD3DXFont->DrawTextA(
      NULL, //fnt_sprite,
      strText,
      strlen(strText), 
      &r,
      flags,
      Color.GetPacked());
  
    return;
  }




  //r3dRenderer->SetMaterial(NULL);
  r3dRenderer->SetTex(glyphTexture);
//  m_pd3dDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);

  #define DEF_GLYPH_TX(gc) \
      float tx1 = glyphTexCoords[gc][0];			\
      float ty1 = glyphTexCoords[gc][1];			\
      float tx2 = glyphTexCoords[gc][2];			\
      float ty2 = glyphTexCoords[gc][3];			\
      float w   = (tx2-tx1) * texWidth  / glyphTexScale;	\
      float h   = (ty2-ty1) * texHeight / glyphTexScale;
      
  float texWidth  = (float)glyphTexture->GetWidth();
  float texHeight = (float)glyphTexture->GetHeight();

  float fStartX = sx;
  float CurX    = sx;
  float CurY    = sy;

  if(dwFlags & D3DFONT_CENTERED) {
    SIZE sz;
    GetTextExtent(strText, &sz);

    fStartX -= sz.cx / 2;
    CurX    -= sz.cx / 2;
    CurY    -= sz.cy / 2;
  }

	nVerts=0;

  if(sw > 0) 
  {
    float space_width;
    {
      // size of ' ' char
      char c = ' ';
      DEF_GLYPH_TX(0);
      space_width = w;
    }
      
    while(*strText) {
      // try to find some delimiters, while inserting
      const char* p  = strText;
      const char* br = NULL;

      int bJustify  = 0;
      int iNumSpaces = 0;
    
      float totw = 0, brw = 0;
      while(*p) {
        p = ParseSpecialCodes(p);
        char c = *p++;
        switch(c) {
          case ' ':
            iNumSpaces++;
          case '.':
          case ';':
          case '!':
          case '?':
          case '\n':
          case '\0':
            brw = totw;
            br  = p;
            break;
        }
        if(c == '\n') break;
        if(c < ' ')   continue;

        // make glyph code  
        int gc = (unsigned int)c - GlyphCodeCacheMin;
        if(gc < 0 || gc >= GlyphCacheSize)
          continue;

        DEF_GLYPH_TX(gc);
        totw += w; 
        if(totw + space_width >= sw) {
          bJustify = 1;
          break;
        }
      }
      if(!br || !bJustify) br = p;

      m_curColor = Color;
      
      // draw justified line of text
      for(; strText < br; strText++) {
        strText = ParseSpecialCodes(strText);

        char c = *strText;
        if(c == '\n') continue;

        // make glyph code  
        int gc = (unsigned int)c - GlyphCodeCacheMin;
        if(gc < 0 || gc >= GlyphCacheSize)
          continue;

        DEF_GLYPH_TX(gc);
        _DrawChar(nVerts, CurX, CurY, w, h, tx1-0.5f/glyphTexture->GetWidth(), ty1-0.5f/glyphTexture->GetHeight(), tx2-0.5f/glyphTexture->GetWidth(), ty2-0.5f/glyphTexture->GetHeight(), m_curColor);
        CurX += w;
        
        if(c == ' ' && bJustify) CurX += (sw - (brw + space_width)) / iNumSpaces;
      }

      CurX = fStartX;
      CurY += (glyphTexCoords[0][3]-glyphTexCoords[0][1])*texHeight;
    }
  }

  // generic case - draw w/o box
  if(sw < 0) {

    m_curColor = Color;

    while(*strText) {
    
      strText = ParseSpecialCodes(strText);
    
      char c = *strText++;
      if(c == '\n') {
        CurX = fStartX;
        CurY += (glyphTexCoords[0][3]-glyphTexCoords[0][1])*texHeight;
        continue;
      }

      // make glyph code  
      int gc = (unsigned int)c - GlyphCodeCacheMin;
      if(gc < 0 || gc >= GlyphCacheSize)
        continue;
 
      DEF_GLYPH_TX(gc);
			_DrawChar(nVerts, CurX, CurY, w, h, tx1-0.5f/glyphTexture->GetWidth(), ty1-0.5f/glyphTexture->GetHeight(), tx2-0.5f/glyphTexture->GetWidth(), ty2-0.5f/glyphTexture->GetHeight(), m_curColor);
      CurX += w;
    }
  }
	if(nVerts>0)
	{
		r3dRenderer->Begin2DPolygon(4, nVerts/4);
		for(int p=0; p<nVerts; p += 4)	r3dRenderer->Fill2DPolygon(4, &_V[p]);
		r3dRenderer->End2DPolygon();
	}
  
  #undef DEF_GLYPH_TX
 
  r3dRenderer->Flush();
  r3dRenderer->SetTex(NULL);
//  r3dRenderer->SetMaterial(NULL);
//  m_pd3dDevice->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
  return;
}



void CD3DFont::DrawScaledText(float sx, float sy, float XScale, float YScale, float DistanceScale, const r3dColor24& Color, const char* strText, DWORD dwFlags)
{
	r3dRenderer->Flush();
	r3dRenderer->SetTex(glyphTexture);
// m_pd3dDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);

 float fStartX = sx;
 float CurX = sx;
 float CurY = sy;

 SIZE pt;
 GetTextExtent(strText, &pt);

 if (dwFlags & D3DFONT_CENTERED)
 {
  fStartX -= (pt.cx/2)*XScale;
  CurX -= (pt.cx/2)*XScale*DistanceScale;
  CurY -= (pt.cy/2)*YScale;
 }
  
 m_curColor = Color;

 float texWidth   = (float)glyphTexture->GetWidth();
 float texHeight  = (float)glyphTexture->GetHeight();

 nVerts=0;
 while(*strText)
 {
   strText = ParseSpecialCodes(strText);
   char c = *strText++;

   if(c == '\n') {
     CurX = fStartX;
     CurY += ((glyphTexCoords[0][3]-glyphTexCoords[0][1])*texHeight)*DistanceScale;
     continue;
   }

   // make glyph code  
   int gc = (unsigned int)c - GlyphCodeCacheMin;
   if(gc < 0 || gc >= GlyphCacheSize)
     continue;

   float tx1 = glyphTexCoords[gc][0];
   float ty1 = glyphTexCoords[gc][1];
   float tx2 = glyphTexCoords[gc][2];
   float ty2 = glyphTexCoords[gc][3];

   float w = ((tx2-tx1) * texWidth  / glyphTexScale)*XScale;
   float h = ((ty2-ty1) * texHeight / glyphTexScale)*YScale;

   _DrawChar(nVerts, CurX,CurY,w,h, tx1-0.5f/glyphTexture->GetWidth(), ty1-0.5f/glyphTexture->GetHeight(), tx2-0.5f/glyphTexture->GetWidth(), ty2-0.5f/glyphTexture->GetHeight(), m_curColor);
   CurX += w*DistanceScale;
 }
	if(nVerts>0)
	{
		r3dRenderer->Begin2DPolygon(4, nVerts/4);
		for(int p=0; p<nVerts; p += 4)	r3dRenderer->Fill2DPolygon(4, &_V[p]);
		r3dRenderer->End2DPolygon();
	}

 r3dRenderer->Flush();
 r3dRenderer->SetTex(NULL);
// m_pd3dDevice->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);

 return;
}

void CD3DFont::PrintF(float X, float Y, const r3dColor24& Color, const char* fmt, ...)
{
  char 	buf[4096];

  va_list ap;
  va_start(ap, fmt);
  StringCbVPrintfA(buf, sizeof(buf), fmt, ap);
  va_end(ap);

  DrawText(X, Y, -1, -1, Color, buf);

  return;
}

void CD3DFont::PrintF(float X, float Y, float W, float H, const r3dColor24& Color, const char* fmt, ...)
{
  char 	buf[4096];

  va_list ap;
  va_start(ap, fmt);
  StringCbVPrintfA(buf, sizeof(buf), fmt, ap);
  va_end(ap);

  DrawText(X, Y, W, H, Color, buf);

  return;
}

void CD3DFont::PrintF(float X, float Y, float W, float H,  float StartTime, float CurrentTime, float PrintSpeed, const r3dColor24& Color, const char* fmt, ...)
{
  char 	buf[4096];

  va_list ap;
  va_start(ap, fmt);
  StringCbVPrintfA(buf, sizeof(buf), fmt, ap);
  va_end(ap);

  int Len = strlen(buf);

  int Val = int((CurrentTime - StartTime) * PrintSpeed);
  if (Val > Len) Val = Len;

  buf[Val] = 0;
  DrawText(X, Y, W, H, Color, buf);

  return;
}



void CD3DFont::ScaledPrintF(float X, float Y, float XScale, float YScale, float DistanceScale, const r3dColor24& Color, const char* fmt, ...)
{
  char buf[512];

  va_list ap;
  va_start(ap, fmt);
  StringCbVPrintfA(buf, sizeof(buf), fmt, ap);
  va_end(ap);

  DrawScaledText(X, Y, XScale, YScale, DistanceScale,Color, buf);

  return;
}


void CD3DFont::DrawTextW(float x, float y, float w, float h, const r3dColor24& Color, const wchar_t* strText, DWORD dwFlags)
{
  // this is needed, because we might have some data sitting in our renderer
  r3dRenderer->Flush();
  
  RECT  r;
  r.left   = (long)x;
  r.top    = (long)y;
  r.right  = r.left + (long)w;
  r.bottom = r.top  + (long)h;
    
  DWORD flags = DT_NOCLIP;
  if(dwFlags & D3DFONT_CENTERED) {
    // clip in specified rectangle
    flags = DT_CENTER | DT_VCENTER | DT_WORDBREAK;
  }
  
  fnt_pD3DXFont->DrawTextW(
    NULL, //fnt_sprite,
    strText,
    wcslen(strText), 
    &r,
    flags,
    Color.GetPacked());
    
  return;
}

void CD3DFont::PrintFW(float x, float y, const r3dColor24 &color, const wchar_t *fmt, ...)
{
  wchar_t buf[4096];
  va_list ap;
  va_start(ap, fmt);
  StringCbVPrintfW(buf, sizeof(buf), fmt, ap);  
  va_end(ap);
  
  //TODO: maybe we need quickly check if we can draw text without using id3dxfont?
  DrawTextW(x, y, 99999, 99999, color, buf, 0);
  return;
}

void CD3DFont::GetTextExtentW(const wchar_t* strText, SIZE* pSize) const
{
  RECT r;
  r.left   = 0;
  r.top    = 0;
  r.right  = 99999;
  r.bottom = 99999;

  fnt_pD3DXFont->DrawTextW(
    NULL,
    strText,
    wcslen(strText), 
    &r,
    DT_CALCRECT,
    0xFFFFFFFF);
    
  pSize->cx = r.right  - r.left;
  pSize->cy = r.bottom - r.top;

  return;
}
