//=========================================================================
//	Module: HeightNormalVert.h
//	Copyright (C) 2011.
//=========================================================================

#pragma once

//////////////////////////////////////////////////////////////////////////

static DWORD UnpackNorm(uint16_t Norm)
{
	int nx = Norm & 0xff;
	int nz = Norm >> 8 & 0xff;

	float fnx = ( nx - 127.5f ) / 127.5f;
	float fnz = ( nz - 127.5f ) / 127.5f;
	float fny = sqrtf( R3D_MAX( 1.f - fnx * fnx - fnz * fnz, 0.f ) );

	int ny = (int)( fny * 127.5f + 127.5f );

	return 0xff000000 | nz << 16 | ny << 8 | nx;
}

//////////////////////////////////////////////////////////////////////////

static uint16_t PackNorm(DWORD normal)
{
	return (normal & 0x00ff0000 ) >> 8 | (normal & 0x000000ff);
}

//////////////////////////////////////////////////////////////////////////

static int16_t PackHeight(float h)
{
	float minH = Terrain1->m_MinHeight;
	float maxH = Terrain1->m_MaxHeight;

	if (minH == maxH)
		return static_cast<int16_t>(-32767);

	float x = (h - minH) / (maxH - minH) * 2.0f - 1;
	return static_cast<int16_t>(x * 32767);
}

static uint16_t PackHeightU( float h )
{
	float minH = Terrain1->m_MinHeight;
	float maxH = Terrain1->m_MaxHeight;

	if (minH == maxH)
		return 0;

	float x = (h - minH) / (maxH - minH);
	return static_cast<uint16_t>( x * 65535 );
}


//////////////////////////////////////////////////////////////////////////

static float UnpackHeight(int16_t h)
{
	float minH = Terrain1->m_MinHeight;
	float maxH = Terrain1->m_MaxHeight;
	return (h / 32767.f * 0.5f + 0.5f) * (maxH - minH) + minH;
}

//------------------------------------------------------------------------

static float UnpackHeightU(uint16_t h)
{
	float minH = Terrain1->m_MinHeight;
	float maxH = Terrain1->m_MaxHeight;
	return ( h / 65535.f ) * (maxH - minH) + minH;
}


//////////////////////////////////////////////////////////////////////////

static int16_t PackDelta(float d)
{
	float minH = Terrain1->m_MinHeight;
	float maxH = Terrain1->m_MaxHeight;
	float half_range = 0.5f * (maxH - minH);
	return static_cast<int16_t>(d / half_range * 32767);
}

//////////////////////////////////////////////////////////////////////////

static float UnpackDelta(int16_t d)
{
	float minH = Terrain1->m_MinHeight;
	float maxH = Terrain1->m_MaxHeight;
	float half_range = 0.5f * (maxH - minH);
	return static_cast<int16_t>(d * half_range / 32767);
}

//-------------------------------------------------------------------------
//	HeightNormalVert_t
//-------------------------------------------------------------------------

#pragma pack(push)
#pragma pack(1)

struct HeightNormalVert
{
private:
	int16_t		height;
	int16_t		delta;
	uint16_t	wNormal;
	uint16_t	wMorphNormal;
	uint32_t	dwColor;

public:
	float		Height() const;
	void		SetHeight(float h);
	void		SetHeight(int16_t h);
	uint16_t	Normal() const;
	void		SetNormal(uint16_t n);
	uint32_t	Color() const;
	void		SetColor(uint32_t c);
	uint16_t	MorphNormal() const;
	void		SetMorphNormal(uint16_t n);
	float		Delta() const;
	void		SetDelta(float d);
};

struct HeightNormalVertLQ
{
	uint16_t	height;
	uint16_t	normal;
	uint32_t	color;

	float		Height() const;
	void		SetHeight(float h);
	void		SetHeight(uint16_t h);
	uint16_t	Normal() const;
	void		SetNormal(uint16_t n);
	uint32_t	Color() const;
	void		SetColor(uint32_t c);
	uint16_t	MorphNormal() const;
	void		SetMorphNormal(uint16_t n);
	float		Delta() const;
	void		SetDelta(float d);
};

#pragma pack(pop)


//////////////////////////////////////////////////////////////////////////

float HeightNormalVert::Height() const
{
	return UnpackHeight(height);
}

//////////////////////////////////////////////////////////////////////////

void HeightNormalVert::SetHeight(float h)
{
	height = PackHeight(h);
}

//////////////////////////////////////////////////////////////////////////

void HeightNormalVert::SetHeight(int16_t h)
{
	height = h;
}

//////////////////////////////////////////////////////////////////////////

uint16_t HeightNormalVert::Normal() const
{
	return wNormal;
}

//////////////////////////////////////////////////////////////////////////

void HeightNormalVert::SetNormal(uint16_t n)
{
	wNormal = n;
}

//////////////////////////////////////////////////////////////////////////

uint32_t HeightNormalVert::Color() const
{
	return dwColor;
}

//////////////////////////////////////////////////////////////////////////

void HeightNormalVert::SetColor(uint32_t c)
{
	dwColor = c;
}

//////////////////////////////////////////////////////////////////////////

uint16_t HeightNormalVert::MorphNormal() const
{
	return wMorphNormal;
}

//////////////////////////////////////////////////////////////////////////

void HeightNormalVert::SetMorphNormal(uint16_t n)
{
	wMorphNormal = n;
}

//////////////////////////////////////////////////////////////////////////

float HeightNormalVert::Delta() const
{
	return UnpackDelta(delta);
}

//////////////////////////////////////////////////////////////////////////

void HeightNormalVert::SetDelta(float d)
{
	delta = PackDelta(d);
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////

float HeightNormalVertLQ::Height() const
{
	return UnpackHeightU(height);
}

//////////////////////////////////////////////////////////////////////////

void HeightNormalVertLQ::SetHeight(float h)
{
	height = PackHeightU(h);
}

//////////////////////////////////////////////////////////////////////////

void HeightNormalVertLQ::SetHeight(uint16_t h)
{
	height = h;
}

//////////////////////////////////////////////////////////////////////////

uint16_t HeightNormalVertLQ::Normal() const
{
	return normal;
}

//////////////////////////////////////////////////////////////////////////

void HeightNormalVertLQ::SetNormal(uint16_t n)
{
	normal = n;
}

//////////////////////////////////////////////////////////////////////////

uint32_t HeightNormalVertLQ::Color() const
{
	return color;
}

//////////////////////////////////////////////////////////////////////////

void HeightNormalVertLQ::SetColor(uint32_t c)
{
	color = c;
}

//////////////////////////////////////////////////////////////////////////

uint16_t HeightNormalVertLQ::MorphNormal() const
{
	return 0 ;
}

//////////////////////////////////////////////////////////////////////////

void HeightNormalVertLQ::SetMorphNormal(uint16_t n)
{
	(void)n ;
}

//////////////////////////////////////////////////////////////////////////

float HeightNormalVertLQ::Delta() const
{
	return 0.f;
}

//////////////////////////////////////////////////////////////////////////

void HeightNormalVertLQ::SetDelta(float d)
{
	(void)d ;
}
