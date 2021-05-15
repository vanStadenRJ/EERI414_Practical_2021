#pragma once
// ---------------------------------------------------------------------------
// LIBRARY INCLUDE FILES
#include <iostream>
#include <string>
#include <vector>
#include <cmath>
#include <math.h>
#include <complex>

// ---------------------------------------------------------------------------
// LOCAL INCLUDE FILES
#include "fftw3.h"

class cFastFourierTransform
{
public:
	// Constructors
	cFastFourierTransform();
	cFastFourierTransform(std::vector<float>& vfSignal_Time);

	// Public Methods
	std::vector<std::complex<float>> getFFT();
	std::vector<float> getMagnitude(std::vector<std::complex<float>> &vfFFT_complex);
	std::vector<float> getPhase(std::vector<std::complex<float>>& vfFFT_complex);

	// Public Attributes

private:
	// Private Methods
	void fft(fftw_complex* in, fftw_complex* out);
	void ifft(fftw_complex* in, fftw_complex* out);
	void displayComplex(fftw_complex* y);
	void displayReal(fftw_complex* x);
	//std::vector<float> getMagnitude();
	//std::vector<float> getPhase();

	// Private Attributes
	int m_iFFTSize;
	fftw_complex* x;
	fftw_complex* y;
};

