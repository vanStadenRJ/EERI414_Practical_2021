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
	std::vector<std::complex<double>> getTransferFunction(bool bAnalog);
	std::vector<float> getYAxis(bool bAnalog, bool bMag);
	std::vector<float> getXAxis(bool bAnalog);

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
	double m_fDiscretePass_rads;									// Discrete Edge Frequency (rad/s)
	double m_fDiscreteStop_rads;									// Discrete Corner Frequency (rad/s)
	double m_fOmegaPass_rads;										// Analog Edge Frequency (rad/s)
	double m_fOmegaStop_rads;										// Analog Corner Frequency (rad/s)
	double m_fRipplePass_ratio;										// Analog Ripple Passband (ratio)
	double m_fRippleStop_ratio;										// Analog Ripple Stopband (ratio)
	double fEpsilon;												// Epsilon for Analog Filter Design
	double m_fNumerator_s;											// Numerator of Analog Transfer Function
	
	int m_iSampleRate_Hz;											// Sample Rate (Hz)
	int m_iOrder_N;													// Order of designed filter

	std::vector<std::complex<double>> m_vfTransferFunction_s;		// Transfer Function vector of Analog Filter
	std::vector<double> m_vfDenominator_s;							// Denominator of Analog Transfer Function
	std::vector<float> m_vfMagnitude_s;							// Analog Filter Magnitude Response	
	std::vector<float> m_vfPhase_s;								// Analog Filter Phase Response
	std::vector<float> m_vfX_s;									// X-axis of the Analog Magnitude
	
};

