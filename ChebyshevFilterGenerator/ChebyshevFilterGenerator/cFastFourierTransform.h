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

	// Public Methods
	void getFFT(std::vector<float>& vfSignal_Time);
	std::vector<float> getMagnitude();
	std::vector<float> getPhase();

	// Public Attributes

private:
	// Private Methods
	void fft(fftw_complex* in, fftw_complex* out);
	void ifft(fftw_complex* in, fftw_complex* out);
	void displayComplex(fftw_complex* y);
	void displayReal(fftw_complex* x);
	void setMagnitude(std::vector<std::complex<float>>& vfFFT_complex);
	void setPhase(std::vector<std::complex<float>>& vfFFT_complex);
	//std::vector<float> getMagnitude();
	//std::vector<float> getPhase();

	// Private Attributes
	int m_iFFTSize;
	std::vector<std::complex<float>> m_vfFFT_complex;
	std::vector<float> m_vfMagnitude;
	std::vector<float> m_vfPhase;
	fftw_complex* x;
	fftw_complex* y;
};

