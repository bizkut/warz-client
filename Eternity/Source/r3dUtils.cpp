
#include "r3dPCH.h"
#include "r3d.h"

#include "r3dConst.h"
#include "r3dUtils.h"

//C
//
// r3dImageLoader
//
//

#define	R3D_BMP_SIGNATURE	'MB'

#pragma pack(push)
#pragma pack(1)
struct R3D_BMP_HEADER
{
	WORD		Signature;		// 'BM'
	DWORD		FileSize;
	DWORD		Unused;
	DWORD		OffsetOfData;

	DWORD		StructSize;		// 40
	DWORD		Width;
	DWORD		Height;
	WORD		NumPlanes;
	WORD		BitsPerPixel;
	DWORD		Compression;
	DWORD		ImageSize;
	DWORD		xPelsPerMeter;
	DWORD		yPelsPerMeter;
	DWORD		ColorsUsed;
	DWORD		ImportantColors;
};

struct R3D_BMP_RGBQUAD
{
  BYTE		b, g, r, u;
};

#pragma pack(pop)

r3dImageLoader::r3dImageLoader()
{
  ClrData = NULL;
}

r3dImageLoader::~r3dImageLoader()
{
  if(ClrData)
    delete[] ClrData;
}

int r3dImageLoader::Load(r3dFile *f)
{
	char		*RawData;
	R3D_BMP_HEADER 	hdr;

  if(!f->IsValid())
    return 0;

  fread(&hdr, sizeof(hdr), 1, f);
  if(hdr.Signature != R3D_BMP_SIGNATURE) {
    r3dArtBug("ImageLoader: [%s] - isn't BMP\n", f->GetFileName());
    return 0;
  }

  if(hdr.Compression) {
    r3dArtBug("ImageLoader: [%s] - compressed BMP is not supported\n", f->Location.FileName);
    return 0;
  }

  Width  = hdr.Width;
  Height = hdr.Height;

	R3D_BMP_RGBQUAD	OriginalPal[256];
	int		bpp  = (hdr.BitsPerPixel / 8);
	char		*src;
	char		*dst;
	int		x, y;

  if(hdr.BitsPerPixel == 8)
    fread(OriginalPal, 1024, 1, f);

  // read bitmap upside down
  RawData = game_new char[Width * Height * bpp];
  src     = RawData + (Height - 1) * hdr.Width * bpp;
  for(y = 0; y < Height; y++) {
    fread(src, hdr.Width * bpp, 1, f);
    src -= hdr.Width * bpp;
  }

  switch(hdr.BitsPerPixel) {
    // convert 8bpp to 24bpp
    case 8:
      ClrData = game_new char[Width * Height * 3];

      src = RawData;
      dst = ClrData;
      for(y = 0; y < Height; y++) {
	for(x = 0; x < Width; x++) {
	  *dst++ = OriginalPal[*src].b;
	  *dst++ = OriginalPal[*src].g;
	  *dst++ = OriginalPal[*src].r;
	  src++;
	}
      }
      delete[] RawData;
      break;

    case 24:
      // switch arrays
      ClrData = RawData;
      break;
  }

  return 1;
}


//C
//
// High Performance Counter
//
//
#define THIS_IN_ECX	1

void r3dPerfCounter::Start()
{
#if !THIS_IN_ECX
  __asm mov ecx, [this]
#endif
  __asm rdtsc
  __asm mov [ecx.perf_StartHi], edx
  __asm mov [ecx.perf_StartLo], eax
  __asm shr eax, 4
  __asm mov [ecx.perf_Start], eax
}

#pragma warning(disable: 4035)
// return value in EAX
long r3dPerfCounter::GetDiffTicks()
{
#if !THIS_IN_ECX
  __asm mov ecx, [this]
#endif
  __asm rdtsc
  __asm sub edx, [ecx.perf_StartHi]
  __asm shr eax, 4
  __asm shl edx, 28
  __asm or  eax, edx
  __asm sub eax, [ecx.perf_Start]
}
#pragma warning(default: 4035)

float r3dPerfCounter::GetDiff()
{
  return GetDiffTicks() / 50000000.0f;
}

void r3dPerfCounter::GetCurrent(long &Hi, long &Lo)
{
  __asm rdtsc
  __asm mov Hi, edx
  __asm mov Lo, eax
}

//C
//
// r3dFunctionStack
//
//

static	char		*FnStackNames[256];
static	int		FnStackTop = 0;

r3dFunctionStack::r3dFunctionStack(char *Name)
{
  FnStackNames[FnStackTop++] = Name;
}

r3dFunctionStack::~r3dFunctionStack()
{
  FnStackTop--;
}



#define MAX_VA_STRING	4096
#define	MAX_VA_BUFFERS	16

//---------------------------------------------------------------------------------------------
// Name: const char * Va( const char * str, ... )
// Desc:  
//---------------------------------------------------------------------------------------------
const char * Va( const char * str, ... )
{
	static char		_buffer_string[ MAX_VA_BUFFERS ][ MAX_VA_STRING ];
	static int		_index_string = 0;
	char			* buffer;
	va_list			args;

	buffer = _buffer_string[ _index_string ];

	if ( ++_index_string >= MAX_VA_BUFFERS )
		_index_string = 0;

	va_start( args, str );
	vsnprintf( buffer, MAX_VA_STRING, str, args );
	va_end( args );

	return buffer;
}

void DumbWCHAR( WCHAR* dest, const char* source )
{
	char* cdes = (char*)dest ;

	size_t len = strlen( source ) ;

	for( size_t i = 0, e = len ; i < e; i ++ )
	{
		*cdes ++ = *source ++ ;
		*cdes ++ = 0 ;
	}

	*cdes ++ = 0 ;
	*cdes ++ = 0 ;
}

void DumbANSI( char* dest, const WCHAR* source )
{
	size_t len = wcslen( source ) ;

	for( size_t i = 0, e = len ; i < e; i ++ )
	{
		*dest ++ = (char)*source ++ ;
	}

	*dest ++ = 0 ;
}