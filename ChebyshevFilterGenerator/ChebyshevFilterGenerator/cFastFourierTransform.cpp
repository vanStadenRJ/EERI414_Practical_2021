// ---------------------------------------------------------------------------
// NAME & SURNAME       : RJ VAN STADEN
// STUDENTNUMBER        : 30026792
// ---------------------------------------------------------------------------
// DATE                 : 2021/05/05
// REVISION             : rev01
// ---------------------------------------------------------------------------
// PROJECT NAME         : ChebyshevFilterGenerator           
// ---------------------------------------------------------------------------

#define _USE_MATH_DEFINES
#define REAL 0
#define IMAG 1

// ---------------------------------------------------------------------------
// SYSTEM INCLUDE FILES

// ---------------------------------------------------------------------------
// LIBRARY INCLUDE FILES

// ---------------------------------------------------------------------------
// LOCAL INCLUDE FILES
#include "cFastFourierTransform.h"

// ---------- CONSTRUCTOR CALLS ----------
cFastFourierTransform::cFastFourierTransform():
	m_iFFTSize(0)
{
	// DEFAULT CONSTRUCTOR
}

cFastFourierTransform::cFastFourierTransform(std::vector<float>& vfSignal_Time)
{
	// Set FFT array size
	m_iFFTSize = vfSignal_Time.size();

	// Input array
	fftw_complex x[m_iFFTSize];


	// Output array


}

// ---------- COMPUTE 1-D FAST FOURIER TRANSFORM ----------
void cFastFourierTransform::fft(fftw_complex* in, fftw_complex* out)
{
	// Create a DFT plan
	fftw_plan plan = fftw_plan_dft_1d(m_iFFTSize, in, out, FFTW_FORWARD, FFTW_ESTIMATE);

	// Execute the plan
	fftw_execute(plan);

	// Cleanup
	fftw_destroy_plan(plan);
	fftw_cleanup();
}

// ---------- COMPUTE 1-D INVERSE FAST FOURIER TRANSFORM ----------
void cFastFourierTransform::ifft(fftw_complex* in, fftw_complex* out)
{
	// Create a DFT plan
	fftw_plan plan = fftw_plan_dft_1d(m_iFFTSize, in, out, FFTW_BACKWARD, FFTW_ESTIMATE);

	// Execute the plane
	fftw_execute(plan);

	// Cleanup
	fftw_destroy_plan(plan);
	fftw_cleanup();

	// Scale the output to obtain exact inverse transform
	for (int i = 0; i < m_iFFTSize; i++) {
		out[i][REAL] /= m_iFFTSize;
		out[i][IMAG] /= m_iFFTSize;
	}
}

// ---------- DISPLAY COMPLEX NUMBERS IN THE FORM a +/- bi ----------
void cFastFourierTransform::displayComplex(fftw_complex* y)
{
	for (int i = 0; i < m_iFFTSize; i++) {
		if (y[i][IMAG] < 0) {
			std::cout << y[i][REAL] << " - " << abs(y[i][IMAG]) << "i" << std::endl;
		}
		else {
			std::cout << y[i][REAL] << " + " << y[i][IMAG] << "i" << std::endl;
		}
	}
}

// ---------- DISPLAY REAL PARTS OF COMPLEX NUMBERS ----------
void cFastFourierTransform::displayComplex(fftw_complex* y)
{
	for (int i = 0; i < m_iFFTSize; i++) {
		std::cout << y[i][REAL] << std::endl;
	}
}
