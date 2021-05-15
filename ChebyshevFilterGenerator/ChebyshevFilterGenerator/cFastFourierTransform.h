#pragma once
// ---------------------------------------------------------------------------
// LIBRARY INCLUDE FILES
#include <iostream>
#include <string>
#include <vector>
#include <cmath>

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
	void fft(fftw_complex* in, fftw_complex* out);
	void ifft(fftw_complex* in, fftw_complex* out);
	void displayComplex(fftw_complex* y);
	void displayReal(fftw_complex* x);

	// Public Attributes

private:
	// Private Methods

	// Private Attributes
	int m_iFFTSize;
};

