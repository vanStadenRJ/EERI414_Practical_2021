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
#define N 524288

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

// ---------- RETURN FFT IN VECTOR FORMAT ----------
void cFastFourierTransform::getFFT(std::vector<double>& vfSignal_Time)
{
	// Set FFT array size
	m_iFFTSize = vfSignal_Time.size();

	// Input array
	x = new fftw_complex[N];

	// Output array
	y = new fftw_complex[N];

	int u = 0;

	// Populate the Array with values for sampling rate
	for (int i = 0; i < m_iFFTSize; i++) {
		x[i][REAL] = vfSignal_Time.at(i);
		x[i][IMAG] = 0;
		u++;
	}

	std::cout << "Length Before Padding: " << u << std::endl;

	// Zero Padding for efficiency
	for (int i = m_iFFTSize; i < N; i++) {
		x[i][REAL] = 0;
		x[i][IMAG] = 0;
		u++;
	}

	std::cout << "Length After Padding: " << u << std::endl;

	// Compute FFT
	this->fft(x, y);

	// Display the results
	//std::cout << "FFT:" << std::endl;
	//this->displayComplex(y);

	// Compute IFFT
	//this->ifft(y, x);

	// Display the results
	//std::cout << "\nIFFT:" << std::endl;
	//this->displayReal(x);

	m_vfFFT_complex.clear();
	for (int i = 0; i < N; i++) {
		std::complex<double> mycomplex(y[i][REAL], y[i][IMAG]);
		m_vfFFT_complex.push_back(mycomplex);
		//std::cout << "\n" << y[i][REAL] << " " << y[i][IMAG] << "i\n" << std::endl;
	}

	// Calculate Magnitude and Phase Spectrums
	this->setMagnitude(m_vfFFT_complex);
	this->setPhase(m_vfFFT_complex);
}

// ---------- RETURN IFFT IN VECTOR FORMAT ----------

// ---------- RETURN MAGNITUDE RESPONSE ----------
std::vector<double> cFastFourierTransform::getMagnitude()
{
	return m_vfMagnitude;
}

// ---------- RETURN PHASE RESPONSE ----------
std::vector<double> cFastFourierTransform::getPhase()
{
	return m_vfPhase;
}

// ---------- COMPUTE MAGNITUDE SPECTRUM ----------
void cFastFourierTransform::setMagnitude(std::vector<std::complex<double>>& vfFFT_complex)
{
	m_vfMagnitude.clear();
	for (int i = 0; i < vfFFT_complex.size() / 2; i++) {
		double mag = abs(vfFFT_complex.at(i)) / (double)N;
		m_vfMagnitude.push_back(2*mag);
	}

	//m_vfMagnitude = 2 * m_vfMagnitude;
}

// ---------- COMPUTE PHASE SPECTRUM ----------
void cFastFourierTransform::setPhase(std::vector<std::complex<double>>& vfFFT_complex)
{
	m_vfPhase.clear();
	for (int i = 0; i < vfFFT_complex.size(); i++) {
		double real = vfFFT_complex.at(i).real();
		double imag = vfFFT_complex.at(i).imag();
		double phase = atan(imag / real);
		m_vfPhase.push_back(phase);
	}
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
