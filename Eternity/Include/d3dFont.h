#ifndef __R3D__R3DFONT_H
#define __R3D__R3DFONT_H

#include "r3dPCH.h"
#include "r3d.h"


// Font creation flags
#define D3DFONT_BOLD        0x0001
#define D3DFONT_ITALIC      0x0002
#define D3DFONT_FILTERED    0x0004
#define D3DFONT_SKIPGLYPH   0x0008

// Font rendering flags
#define D3DFONT_CENTERED    0x0001


//-----------------------------------------------------------------------------
// Name: class CD3DFont
// Desc: Texture-based font class for doing text in a 3D scene.
//-----------------------------------------------------------------------------
class CD3DFont : public r3dIResource
{
  public:
	// glyph based data (we store characters code 32-127)
	enum { 
	  GlyphCodeCacheMin = 32, 
	  GlyphCodeCacheMax = 127, 
	  GlyphCacheSize    = GlyphCodeCacheMax - GlyphCodeCacheMin,
	};

	struct glyphData_s {
	  float		texScale;
	  float		tx[GlyphCacheSize][4];
	};

	DWORD getFontHeight() const { return fnt_fontHeight; }
  protected:
	char		fnt_fontName[80];            // Font properties
	DWORD   	fnt_fontHeight;
	DWORD   	fnt_fontFlags;

	r3dTexture*	glyphTexture;		// my cached texture font
	float   	glyphTexScale;
	float   	glyphTexCoords[GlyphCacheSize + 1][4];
	
	void		CreateGlyphs(int cellOffX1, int cellOffY1, int cellOffX2, int cellOffY2);
	void    	SaveGlyphsToFile();
	void    	LoadGlyphsFromFile();

	r3dColor24	m_curColor;
	const char*	ParseSpecialCodes(const char* in);

	ID3DXFont*	fnt_pD3DXFont;			// pointer to font d3dxinterface
	void		CreateD3DXFont();

virtual	void		D3DCreateResource();
virtual	void		D3DReleaseResource();

public:
    // 2D and 3D text drawing functions
	void		DrawText(float x, float y, float W, float H, const r3dColor24& Color, const char* strText, DWORD dwFlags=0L);
	void		DrawTextW(float x, float y, float W, float H, const r3dColor24& Color, const wchar_t* strText, DWORD dwFlags=0L);
	void		DrawScaledText(float x, float y, float XScale, float YScale, float DistanceScale, const r3dColor24& Color, const char* strText, DWORD dwFlags=0L);

	void 		PrintF(float X, float Y, const r3dColor24& Color, const char* str, ...);
	void		PrintF(float X, float Y, float W, float H, const r3dColor24& Color, const char* str, ...);
	void		PrintF(float X, float Y, float W, float H, float StartTime, float CurrentTime, float PrintSpeed, const r3dColor24& Color, const char* str, ...);
	void 		ScaledPrintF(float X, float Y, float XScale, float YScale, float DistanceScale, const r3dColor24& Color, const char* str, ...);

	void		PrintFW(float x, float y, const r3dColor24 &color, const wchar_t *fmt, ...);
	
	// Function to get extent of text
	HRESULT		GetTextExtent(const char* strText, SIZE *pSize);
	void		GetTextExtentW(const wchar_t* strText, SIZE *pSize) const;

	// Initializing and destroying device-dependent objects
	HRESULT		CreateSystemFont(bool forceRecreate = false, int cellOffX1=0, int cellOffY1=0, int cellOffX2=0, int cellOffY2=0);
	HRESULT		CreateSystemFontEx(int cellOffX1, int cellOffY1, int cellOffX2, int cellOffY2) {
	  return CreateSystemFont(true, cellOffX1, cellOffY1, cellOffX2, cellOffY2);
	}
	void		WriteGlyphDefinition(const char* fname);
	HRESULT		CreateFromDefinition(const glyphData_s& gl);
	
	HRESULT		DeleteDeviceObjects();

	ID3DXFont*	GetD3DFont() {
	  assert(fnt_pD3DXFont);
	  return fnt_pD3DXFont;
	}

	// Constructor / destructor
	CD3DFont(const r3dIntegrityGuardian& ig, const char* strFontName, DWORD dwHeight, DWORD dwFlags=0L );
	virtual ~CD3DFont();
};

extern CD3DFont	*_r3dSystemFont;

#endif
