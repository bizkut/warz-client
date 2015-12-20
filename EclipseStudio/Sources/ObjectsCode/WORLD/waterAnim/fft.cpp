#include "r3dPCH.h"
#include "r3d.h"
#include "fft.h"

void FFT(int n, Complex* data, int sign, int stride = 1)
{
	int     i, j, m, mmax, istep;
	Complex w, wp, temp;
	float   theta;

	j = 0;	   
	for(i = 0; i < n; i++)
	{									  
		if(j > i)
		{
			temp = data[j*stride];
			data[j*stride] = data[i*stride];
			data[i*stride] = temp;
		}
		m = n >> 1;
		while(m >= 1 && j >= m) { j -= m; m >>= 1; }
		j += m;
	}

	mmax = 1; theta = sign*R3D_PI;
	while(n > mmax)
	{
		istep = 2*mmax;
		wp = Complex(cos(theta), sin(theta));
		w  = Complex(1.0f, 0.0f);
		for(m = 0; m < mmax; m++)
		{
			for(i = m; i < n; i += istep)
			{
				j = i + mmax; temp = w*data[j*stride];
				data[j*stride] = data[i*stride] - temp;
				data[i*stride] = data[i*stride] + temp;
			}
			w = w*wp;
		}
		mmax = istep; theta = theta*0.5f;
	}
}

void four1D(int n, Complex* data)
{
	FFT(n, data, 1);

	float nf = 1.0f/n;
	for(int i = 0; i < n; ++i) data[i] *= nf;
}

void invfour1D(int n, Complex* data)
{
	FFT(n, data, -1);
}

void four2D(int nx, int ny, Complex* data)
{
	for(int y = 0; y < ny; ++y) FFT(nx, data + nx * y, 1,  1);
	for(int x = 0; x < nx; ++x) FFT(ny, data +  1 * x, 1, nx);

	float nf = 1.0f/(nx*ny);
	for(int i = 0; i < (nx*ny); ++i) data[i] *= nf;
}

void invfour2D(int nx, int ny, Complex* data)
{
	for(int y = 0; y < ny; ++y) FFT(nx, data + nx * y, -1,  1);
	for(int x = 0; x < nx; ++x) FFT(ny, data +  1 * x, -1, nx);
}
