#pragma once

#include <complex>

typedef std::complex<float> Complex;

void four1D(int n, Complex* data);
void invfour1D(int n, Complex* data);

void four2D(int nx, int ny, Complex* data);
void invfour2D(int nx, int ny, Complex* data);

