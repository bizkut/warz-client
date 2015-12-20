#pragma once

#include "fft.h"

const int MAX_LODS = 5;

class WavesGenerator
{
public:
	struct Color
	{
		unsigned char b, g, r, a;
	};

	typedef Color* Image;

	struct ImageData 
	{
		ImageData():lodsCount(0){};
		void init(int w, int h);
		~ImageData();
		Image images[16];
		int sizesX[16];
		int sizesY[16];
		int lodsCount;
	};

	struct Result
	{
		Result(int nFrames, int w, int h);
		~Result();

		int numFrames;
		int width;
		int height;

		ImageData* images;
	};

	WavesGenerator(int nFrames, int w, int h);

	void setSeed(unsigned int seed) { m_seed = seed; }
	void setAmplitude(float a) { m_amplitude = a; }
	void setLength(float lx, float ly) { m_length = Complex(lx, ly); }
	void setCutoff(float l) { m_cutoff = l; }
	void setGravity(float g) { m_gravity = g; }
	void setWindVel(float wv) { m_wind_vel = wv; }
	void setWindDir(float wx, float wy) { 
		m_wind_dir = Complex(wx, wy); 
		m_wind_dir /= abs(m_wind_dir);
	}
	void setDepth(float d) { m_depth = d; }

	void makeAni(float T, bool bCaustic = false);

	Result result;

private:
	float Ph(const Complex& k);
	void makeH0(int Nx, int Ny, Complex* h0);
	void makeHN(int Nx, int Ny, int lods, Complex* hn[], Complex* h0, float w0, float t);

	void makeNMap(ImageData& img, Complex* hn[], bool bCaustic);

	float   m_amplitude;
	Complex m_length;
	float   m_cutoff;
	float   m_gravity;
	float   m_wind_vel;
	Complex m_wind_dir;
	float   m_depth;

	unsigned int m_seed;
};