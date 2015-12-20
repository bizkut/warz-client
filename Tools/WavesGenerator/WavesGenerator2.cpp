#include <tchar.h>
#include <d3d9.h>
#include <d3dx9.h>
#include "wavesGenerator.h"


int Nx = 256;
int Ny = 256;
int Nf = 25;

float amplitude = 0.005f;
float waveLenX = 10.0f;
float waveLenY = 10.0f;

float cutoff = 0.0f;
float depth = 20.0f;
float windVel = 100.0f;
float t = 1.0f;


int main(int argc, const char* argv[])
{
	const char* usage = "Command line options is optional and override default values: \n\
    -Nx,Ny = 256, texture dimension\n\
    -Nf = 25, number of frames [hardcoded in engine]\n\
    -amplitude = 0.005f\n\
    -waveLenX = 10.0f\n\
    -waveLenY = 10.0f\n\
    -cutoff = 0.0f, frequency cutoff of caustic\n\
    -depth = 20.0f, of water on wich caustic appears\n\
    -windVel = 100.0f\n";

	for(int a=1; a<argc; a++)
	{
		if(argv[a][0]!='-')
		{
			printf("Invalid option %s specified\n%s", argv[a],usage);
			return 0;
		}
		if(a==argc-1)
		{
			printf("Can't find value for option %s\n%s", argv[a], usage);
			return 0;
		}

		if(_strcmpi(argv[a],"-Nx")==0)
		{
			Nx = atoi(argv[a+1]);
			if(Nx==0)
			{
				printf("Invalid value %s specified for option %s\n%s", argv[a+1], argv[a], usage);
				return 0;
			}
		}
		else	if(_strcmpi(argv[a],"-Ny")==0)
		{
			Ny = atoi(argv[a+1]);
			if(Ny==0)
			{
				printf("Invalid value %s specified for option %s\n%s", argv[a+1], argv[a], usage);
				return 0;
			}
		}
		else	if(_strcmpi(argv[a],"-Nf")==0)
		{
			Nf = atoi(argv[a+1]);
			if(Nf==0)
			{
				printf("Invalid value %s specified for option %s\n%s", argv[a+1], argv[a], usage);
				return 0;
			}
		}
		else	if(_strcmpi(argv[a],"-amplitude")==0)	amplitude = float(atof(argv[a+1]));
		else	if(_strcmpi(argv[a],"-waveLenX")==0)	waveLenX = float(atof(argv[a+1]));
		else	if(_strcmpi(argv[a],"-waveLenY")==0)	waveLenY = float(atof(argv[a+1]));
		else	if(_strcmpi(argv[a],"-cutoff")==0)	cutoff = float(atof(argv[a+1]));
		else	if(_strcmpi(argv[a],"-depth")==0)	depth = float(atof(argv[a+1]));
		else	if(_strcmpi(argv[a],"-windVel")==0)	windVel = float(atof(argv[a+1]));
		else
		{
			printf("Invalid option %s specified\n%s", argv[a],usage);
			return 0;
		}
		a++;
	}

	//create null-device to access D3DX
	IDirect3D9* d3d = Direct3DCreate9( D3D_SDK_VERSION );
	D3DDISPLAYMODE Mode;
	d3d->GetAdapterDisplayMode( 0, &Mode );
	D3DPRESENT_PARAMETERS pp;
	ZeroMemory( &pp, sizeof( D3DPRESENT_PARAMETERS ) );
	pp.BackBufferWidth = 1;
	pp.BackBufferHeight = 1;
	pp.BackBufferFormat = Mode.Format;
	pp.BackBufferCount = 1;
	pp.SwapEffect = D3DSWAPEFFECT_COPY;
	pp.Windowed = TRUE;
	IDirect3DDevice9* device;
	HRESULT hr = d3d->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_NULLREF, GetConsoleWindow(), D3DCREATE_HARDWARE_VERTEXPROCESSING, &pp, &device);

	WavesGenerator WG(Nf, Nx, Ny);
	WG.setAmplitude(amplitude);
	WG.setLength(waveLenX, waveLenY);
	WG.setCutoff(0.0f);
	WG.setDepth(depth);
	WG.setWindVel(windVel);
	WG.setWindDir(1.0f, 0.0f);
	WG.makeAni(1.0f, true);

	//create temp texture
	LPDIRECT3DTEXTURE9* texture = new LPDIRECT3DTEXTURE9[Nf];
	for (int i = 0; i < Nf; ++i)
	{
		hr = D3DXCreateTexture(device, Nx, Ny, WG.result.images[0].lodsCount, D3DUSAGE_DYNAMIC, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &texture[i]);
		WavesGenerator::ImageData& img = WG.result.images[i];

		for (int l = 0; l < img.lodsCount; ++l)
		{
			D3DSURFACE_DESC surfaceDesc;
			hr = texture[i]->GetLevelDesc(l, &surfaceDesc);
			D3DLOCKED_RECT rect;
			hr = texture[i]->LockRect(l, &rect, 0, 0);
			//TODO: unsafe due to pitch
			memcpy(rect.pBits, img.images[l], img.sizesX[l] * img.sizesY[l] * sizeof(WavesGenerator::Color));
			hr = texture[i]->UnlockRect(l);
		}
	}

	WG.setCutoff(cutoff);
	WG.makeAni(1.0f, true);

	//write caustic
	for (int i = 0; i < Nf; ++i)
	{
		WavesGenerator::ImageData& img = WG.result.images[i];

		for (int l = 0; l < img.lodsCount; ++l)
		{
			D3DSURFACE_DESC surfaceDesc;
			hr = texture[i]->GetLevelDesc(l, &surfaceDesc);
			D3DLOCKED_RECT rect;
			hr = texture[i]->LockRect(l, &rect, 0, 0);
			WavesGenerator::Color* bmp = (WavesGenerator::Color*)rect.pBits;

			//TODO: unsafe due to pitch
			for(int p=0; p<img.sizesX[l] * img.sizesY[l]; p++)
				bmp[p].a = img.images[l][p].a;

			hr = texture[i]->UnlockRect(l);
		}

		char fileName[256];
		sprintf(fileName, "waves_%.2d.dds", i);
		hr = D3DXSaveTextureToFileA(fileName, D3DXIFF_DDS, texture[i], 0);
		texture[i]->Release();
	}

	delete[] texture;




	//release device
	device->Release();
	d3d->Release();

	return 0;
}



