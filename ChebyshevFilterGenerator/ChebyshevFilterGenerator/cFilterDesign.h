#pragma once
// ---------------------------------------------------------------------------
// SYSTEM INCLUDE FILES
#include <vector>
#include <complex>

// ---------------------------------------------------------------------------
// LIBRARY INCLUDE FILES

// ---------------------------------------------------------------------------
// LOCAL INCLUDE FILES

class cFilterDesign
{
public:
	// Constructors
	cFilterDesign();
	cFilterDesign(int &iOmegaPass_Hz, int &iOmegaStop_Hz, int &iRipplePass_dB, int &iRippleStop_dB, int &iSampleRate_Hz);

	// Public Methods

	// Public Attributes

private:
	// Private Methods
	void setAnalogFilterTF();
	void setAnalogMagnitude();
	void setAnalogPhase();
	std::vector<std::complex<double>> polyMul(std::vector<std::complex<double>> A, std::vector<std::complex<double>> B);
	void printPoly(std::vector<double>& vfPolynomial);
	double t_n(double& fFreq);

	// Private Attributes
	double m_fOmegaPass_rads;					// Analog Edge Frequency (rad/s)
	double m_fOmegaStop_rads;					// Analog Corner Frequency (rad/s)
	double m_fRipplePass_ratio;					// Analog Ripple Passband (ratio)
	double m_fRippleStop_ratio;					// Analog Ripple Stopband (ratio)
	double fEpsilon;							// Epsilon for Analog Filter Design
	double m_fNumerator_s;						// Numerator of Analog Transfer Function
	
	int m_iSampleRate_Hz;						// Sample Rate (Hz)
	int m_iOrder_N;								// Order of designed filter

	std::vector<double> m_vfDenominator_s;		// Denominator of Analog Transfer Function
	std::vector<double> m_vfMagnitude_s;		// Analog Filter Magnitude Response
	std::vector<double> m_vfPhase_s;			// Analog Filter Phase Response
	std::vector<double> m_vfX_s;				// X-axis of the Analog Magnitude
};

