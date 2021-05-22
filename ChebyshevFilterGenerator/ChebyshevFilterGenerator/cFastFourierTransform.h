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
	void getFFT(std::vector<double>& vfSignal_Time);
	std::vector<double> getMagnitude();
	std::vector<double> getPhase();

	// Public Attributes

private:
	// Private Methods
	void fft(fftw_complex* in, fftw_complex* out);
	void ifft(fftw_complex* in, fftw_complex* out);
	void displayComplex(fftw_complex* y);
	void displayReal(fftw_complex* x);
	void setMagnitude(std::vector<std::complex<double>>& vfFFT_complex);
	void setPhase(std::vector<std::complex<double>>& vfFFT_complex);
	//std::vector<double> getMagnitude();
	//std::vector<double> getPhase();

	// Private Attributes
	int m_iFFTSize;
	std::vector<std::complex<double>> m_vfFFT_complex;
	std::vector<double> m_vfMagnitude;
	std::vector<double> m_vfPhase;
	fftw_complex* x;
	fftw_complex* y;
};

