#include "wavesGenerator.h"
#include <d3dx9.h>

typedef D3DXVECTOR3 Vector3;


inline float rnd() { return float(rand())/float(RAND_MAX); }   
float N1()
{
	const int   n = 100;
	const float S = sqrt(float(n)/12);
	const float M = float(n)/2;

	float x = 0.0f;
	for (int i = 0; i < n; ++i) x += rnd();
	return (x - M)/S;
}
inline float Nf(float m, float s) { return (s*N1() + m); }
inline float sqr(float x) { return x*x; }

const float PI = (6.28318530717959f / 2);

void WavesGenerator::ImageData::init(int w, int h)
{
	w = max(w, h);
	h = w;

	int i;
	for (i = 0;  w > 0 && i < MAX_LODS; ++i, w>>=1)
	{
		images[i] = new Color[w*w];
		sizesX[i] = w;
		sizesY[i] = w;
	}
	lodsCount = i;
}

WavesGenerator::ImageData::~ImageData()
{
	for (int i = 0; i < lodsCount; ++i)
	{
		delete [] images[i];
	}
}

WavesGenerator::Result::Result(int nFrames, int w, int h)
:numFrames(nFrames), width(w), height(h)
{
	images = new ImageData[nFrames];
	for(int i = 0; i < nFrames; ++i)
	{
		images[i].init(w, h);
	}
}

WavesGenerator::Result::~Result()
{
	delete [] images;
}

WavesGenerator::WavesGenerator(int nFrames, int w, int h)
:result(nFrames, w, h)
{
	m_amplitude = 0.002f;
	m_length    = Complex(10.0f, 10.0f);
	m_cutoff    = 0.0f;
	m_gravity   = 9.8f;
	m_wind_vel  = 100.0f;
	m_wind_dir  = Complex(1.0f, 0.0f);
	m_depth = 5.0f;

	m_seed = 1;//0x76543210;
}

void WavesGenerator::makeAni(float T, bool bCaustic)
{
	int Nx = result.width;
	int Ny = result.height;

	int lods = result.images[0].lodsCount;

	Complex* h0 = (Complex*)malloc( Nx * Ny * sizeof(Complex) );
	Complex* hn[MAX_LODS];
	for (int l = 0; l < lods; ++l)
	{
		hn[l] = (Complex*)malloc( (Nx * Ny >> (l+l)) * sizeof(Complex) );
	}

	srand(m_seed);

	makeH0(Nx, Ny, h0);

	float w0 = (2*PI / T);
	int num = result.numFrames;
	for (int i = 0; i < num; ++i)
	{
		float t = (T * i / num);
		makeHN(Nx, Ny, lods, hn, h0, w0, t);
		makeNMap(result.images[i], hn, bCaustic);
	}

	for (int l = 0; l < lods; ++l)
	{
		free(hn[l]);
	}

	free(h0);
}
/*
void WavesGen::make(float T, int Nx, int Ny, Complex* hn, unsigned long* nmap)
{
if (m_h0 == 0)
{
m_h0 = (Complex*)::operator new(sizeof(Complex)*Nx*Ny);
makeH0(Nx, Ny, m_h0);
}

float w0 = (2*PI / T);
makeHN(Nx, Ny, hn, m_h0, 0.0f, T);

makeNMap(Nx, Ny, nmap, hn);
}
*/

float WavesGenerator::Ph(const Complex& k)
{
	float L2 = sqr( sqr(m_wind_vel) / m_gravity );
	float k2 = norm(k);
	float c = k.real()*m_wind_dir.real() + k.imag()*m_wind_dir.imag();
	if (c < 0.0f) c = 0.0f;
	float c2 = sqr(c) / k2;
	float l2 = sqr(m_cutoff);

	float s = exp(-1.0f / (k2*L2)) / sqr(k2);
	return m_amplitude * s * (c2) *exp(-k2*l2);
}

void WavesGenerator::makeH0(int Nx, int Ny, Complex* h0)
{
	Complex k0(2*PI / m_length.real(), 2*PI / m_length.imag());

	int nx, ny, nx0, ny0;
	for(ny = 0; ny < Ny; ++ny)
	{
		for(nx = 0; nx < Nx; ++nx)
		{
			nx0 = nx - ((nx*2) & Nx);
			ny0 = ny - ((ny*2) & Ny);

			Complex k(nx0 * k0.real(), ny0 * k0.imag());
			float s = sqrt(Ph(k) * 0.5f);
			h0[ny*Nx + nx] = Complex(s * N1(), s * N1());
		}
	}

	h0[0] = 0;
	for(ny = 0; ny < Ny; ++ny) h0[ny*Nx + (Nx>>1)] = 0;
	for(nx = 0; nx < Nx; ++nx) h0[(Ny>>1)*Nx + nx] = 0;
}

void WavesGenerator::makeHN(int Nx, int Ny, int lods, Complex** hn, Complex* h0, float w0, float t)
{
	Complex k0(2*PI / m_length.real(), 2*PI / m_length.imag());

	int nx, ny, nx0, ny0, _nx, _ny;
	for(ny = 0; ny < Ny; ++ny)
	{
		for(nx = 0; nx < Nx; ++nx)
		{
			nx0 = nx - ((nx*2) & Nx);
			ny0 = ny - ((ny*2) & Ny);

			_nx = (Nx - nx) & (Nx - 1);
			_ny = (Ny - ny) & (Ny - 1);

			Complex k(nx0 * k0.real(), ny0 * k0.imag());
			float kf = abs(k);
			float w = sqrt(kf * m_gravity * tanh(kf * m_depth));
			//float w = sqrt(kf * m_gravity);
			if (w0 > 0.0f) w = int(w / w0) * w0;

			float theta = (w * t);
			Complex h = h0[ny*Nx + nx] * std::polar(1.0f, theta);
			h += conj(h0[_ny*Nx + _nx]) * std::polar(1.0f, -theta);

			//h[ny*Nx + nx] = h;
			hn[0][ny*Nx + nx] = h * Complex(-k.imag(), k.real());
		}
	}

	int l, Nx0 = Nx, Ny0 = Ny;
	for (l = 1, Nx = Nx0>>1, Ny = Ny0>>1; l < lods; ++l, Nx>>=1, Ny>>=1)
	{
		for (ny = 0; ny < Nx>>1; ++ny)
			for (nx = 0; nx < Ny>>1; ++nx)
			{
				hn[l][ny*Nx + nx]               = hn[0][ny*Nx0 + nx];
				hn[l][ny*Nx + (Nx-1-nx)]        = hn[0][ny*Nx0 + (Nx0-1-nx)];
				hn[l][(Ny-1-ny)*Nx + nx]        = hn[0][(Ny0-1-ny)*Nx0 + nx];
				hn[l][(Ny-1-ny)*Nx + (Nx-1-nx)] = hn[0][(Ny0-1-ny)*Nx0 + (Nx0-1-nx)];
			}
	}

	for (l = 0, Nx = Nx0, Ny = Ny0; l < lods; ++l, Nx>>=1, Ny>>=1)
	{
		invfour2D(Nx, Ny, hn[l]);
	}
}


Vector3 refract(const Vector3& N, const Vector3& I, float n)
{
	float ci = D3DXVec3Dot(&N, &I);	
	float ct = sqrt(1.0f + n*n*(ci*ci - 1.0f));
	return (n*ci - ct)*N - n*I;
}

template<class T>
T bilerp(float u, float v, T* map, int Nx, int Ny)
{
	float tx = (u*Nx - 0.5f) + Nx;
	float ty = (v*Ny - 0.5f) + Ny;
	int nx = int(tx);
	int ny = int(ty);
	float fx1 = (tx - nx), fx0 = 1.0f - fx1;
	float fy1 = (ty - ny), fy0 = 1.0f - fy1;
	int nx0 = (nx+0) & (Nx-1), nx1 = (nx+1) & (Nx-1);
	int ny0 = (ny+0) & (Ny-1), ny1 = (ny+1) & (Ny-1);

	return map[ny0*Nx + nx0]*(fx0*fy0) + map[ny0*Nx + nx1]*(fx1*fy0)
		+ map[ny1*Nx + nx0]*(fx0*fy1) + map[ny1*Nx + nx1]*(fx1*fy1);
}

template<class T>
void bilerpAdd(float a, float u, float v, T* map, int Nx, int Ny)
{
	float tx = (u*Nx - 0.5f) + Nx;
	float ty = (v*Ny - 0.5f) + Ny;
	int nx = int(tx);
	int ny = int(ty);
	float fx1 = (tx - nx), fx0 = 1.0f - fx1;
	float fy1 = (ty - ny), fy0 = 1.0f - fy1;
	int nx0 = (nx+0) & (Nx-1), nx1 = (nx+1) & (Nx-1);
	int ny0 = (ny+0) & (Ny-1), ny1 = (ny+1) & (Ny-1);

	map[ny0*Nx + nx0] += a*(fx0*fy0);
	map[ny0*Nx + nx1] += a*(fx1*fy0);
	map[ny1*Nx + nx0] += a*(fx0*fy1);
	map[ny1*Nx + nx1] += a*(fx1*fy1);
}

void WavesGenerator::makeNMap(ImageData& img, Complex* hn[], bool bCaustic)
{
	int sq = img.sizesX[0] * img.sizesY[0];
	Vector3* normMap = (Vector3*)malloc(sizeof(Vector3)*sq);
	float* caustMap = (float*)malloc(sizeof(float)*sq);

	for (int l = 0; l < img.lodsCount; ++l)
	{
		int Nx = img.sizesX[l];
		int Ny = img.sizesY[l];
		//select mip of img
		int nx, ny;
		for(ny = 0; ny <  Ny; ++ny)
			for(nx = 0; nx < Nx; ++nx)
			{
				Complex c = hn[l][ny*Nx + nx];
				normMap[ny*Nx + nx] = Vector3(-c.real(), -c.imag(), 1.0f);
				D3DXVec3Normalize(&normMap[ny*Nx + nx], &normMap[ny*Nx + nx]);
				caustMap[ny*Nx + nx] = 0.0f;
			}

			if (bCaustic)
			{
				int ns = 2;
				float cs = 1.0f/(ns*ns);
				int SNx = ns*Nx, SNy = ns*Ny;
				for(ny = 0; ny < SNy; ++ny)
					for(nx = 0; nx < SNx; ++nx)
					{
						float u = (nx + 0.5f)/SNx;
						float v = (ny + 0.5f)/SNy;

						Vector3 I(0.0f, 0.0f, 1.0f);
						Vector3 N = bilerp(u, v, normMap, Nx, Ny);
						Vector3 T = refract(N, I, 0.75);

						T *= (m_depth / T.z);
						u += T.x / m_length.real();
						v += T.y / m_length.imag();

						bilerpAdd(cs*D3DXVec3Dot(&N, &I), u, v, caustMap, Nx, Ny);
					}
			}

			for(ny = 0; ny < Ny; ++ny)
			{
				for(nx = 0; nx < Nx; ++nx)
				{
					Vector3 N = normMap[ny*Nx + nx];
 					N.y /= (l + 1);
					D3DXVec3Normalize(&N, &N);
					N = (N + Vector3(1.0, 1.0,1.0))*0.5f;
					float c = caustMap[ny*Nx + nx];
					c /= 4; if (c > 1.0f) c = 1.0f;
					Color& col = img.images[l][img.sizesX[l] * ny + nx];
										

					col.r = (unsigned char)(N.x * 255.0f);
					col.g = (unsigned char)(N.y * 255.0f);
					col.b = (unsigned char)(N.z * 255.0f);
					col.a = (unsigned char)(c * 255.0f);
				}
			}
	}

	free(caustMap);
	free(normMap);
}

