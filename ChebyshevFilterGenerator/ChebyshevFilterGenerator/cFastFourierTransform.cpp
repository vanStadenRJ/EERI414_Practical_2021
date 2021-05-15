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
#define N 262144

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
	// Input array
	x = new fftw_complex[N];

	// Output array
	y = new fftw_complex[N];

	// Populate the Array with values for sampling rate
	for (int i = 0; i < N; i++) {
		x[i][REAL] = (float)i/(float)9 + 1;
		x[i][IMAG] = 0;
	}

	// Compute FFT
	this->fft(x, y);

	// Display the results
	std::cout << "FFT:" << std::endl;
	//this->displayComplex(y);

	// Compute IFFT
	this->ifft(y, x);

	// Display the results
	std::cout << "\nIFFT:" << std::endl;
	//this->displayReal(x);
}

cFastFourierTransform::cFastFourierTransform(std::vector<float>& vfSignal_Time)
{
	// Set FFT array size
	m_iFFTSize = vfSignal_Time.size();

	// Input array
	x = new fftw_complex[N];

	// Output array
	y = new fftw_complex[N];

	// Populate the Array with values for sampling rate
	for (int i = 0; i < m_iFFTSize; i++) {
		x[i][REAL] = vfSignal_Time.at(i);
		x[i][IMAG] = 0;
	}

	// Zero Padding for efficiency
	for (int i = m_iFFTSize; i < N - m_iFFTSize; i++) {
		x[i][REAL] = 0;
		x[i][IMAG] = 0;
	}

	// Compute FFT
	this->fft(x, y);

	// Display the results
	std::cout << "FFT:" << std::endl;
	this->displayComplex(y);

	// Compute IFFT
	this->ifft(y, x);

	// Display the results
	std::cout << "\nIFFT:" << std::endl;
	this->displayReal(x);
}

// ---------- RETURN FFT IN VECTOR FORMAT ----------
std::vector<std::complex<float>> cFastFourierTransform::getFFT()
{
	std::vector<std::complex<float>> vfFFT_complex;
	for (int i = 0; i < N; i++) {
		vfFFT_complex.push_back((y[i][REAL], y[i][IMAG]));
	}

	return vfFFT_complex;
}

// ---------- RETURN IFFT IN VECTOR FORMAT ----------

// ---------- RETURN MAGNITUDE RESPONSE ----------
std::vector<float> cFastFourierTransform::getMagnitude(std::vector<std::complex<float>>& vfFFT_complex)
{
	std::vector<float> vfMagnitude;
	for (int i = 0; i < vfFFT_complex.size(); i++) {
		float real = vfFFT_complex.at(i).real();
		float imag = vfFFT_complex.at(i).imag();
		float mag = real * real + imag * imag;
		vfMagnitude.push_back(mag);
	}

	return vfMagnitude;
}

// ---------- RETURN PHASE RESPONSE ----------
std::vector<float> cFastFourierTransform::getPhase(std::vector<std::complex<float>>& vfFFT_complex)
{
	std::vector<float> vfPhase;
	for (int i = 0; i < vfFFT_complex.size(); i++) {
		float real = vfFFT_complex.at(i).real();
		float imag = vfFFT_complex.at(i).imag();
		float phase = atan(imag / real);
		vfPhase.push_back(phase);
	}

	return vfPhase;
}

// ---------- COMPUTE 1-D FAST FOURIER TRANSFORM ----------
void cFastFourierTransform::fft(fftw_complex* in, fftw_complex* out)
{
	// Create a DFT plan
	fftw_plan plan = fftw_plan_dft_1d(N, in, out, FFTW_FORWARD, FFTW_ESTIMATE);

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
	fftw_plan plan = fftw_plan_dft_1d(N, in, out, FFTW_BACKWARD, FFTW_ESTIMATE);

	// Execute the plane
	fftw_execute(plan);

	// Cleanup
	fftw_destroy_plan(plan);
	fftw_cleanup();

	// Scale the output to obtain exact inverse transform
	for (int i = 0; i < N; i++) {
		out[i][REAL] /= N;
		out[i][IMAG] /= N;
	}
}

// ---------- DISPLAY COMPLEX NUMBERS IN THE FORM a +/- bi ----------
void cFastFourierTransform::displayComplex(fftw_complex* y)
{
	for (int i = 0; i < N; i++) {
		if (y[i][IMAG] < 0) {
			std::cout << y[i][REAL] << " - " << abs(y[i][IMAG]) << "i" << std::endl;
		}
		else {
			std::cout << y[i][REAL] << " + " << y[i][IMAG] << "i" << std::endl;
		}
	}
}

// ---------- DISPLAY REAL PARTS OF COMPLEX NUMBERS ----------
void cFastFourierTransform::displayReal(fftw_complex* x)
{
	for (int i = 0; i < N; i++) {
		std::cout << x[i][REAL] << std::endl;
	}
}