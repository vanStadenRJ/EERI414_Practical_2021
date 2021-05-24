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
	cFilterDesign(int &iOmegaPass_Hz, int &iOmegaStop_Hz, double &iRipplePass_dB, double &iRippleStop_dB, int &iSampleRate_Hz);

	// Public Methods
	void setAnalogFilterTF();
	void setGrayMarkel(std::vector<double>& vfNumerator, std::vector<double>& vfDenominator);
	std::vector<std::complex<double>> getTransferFunction(bool bAnalog);
	std::vector<double> getYAxis(bool bAnalog, bool bMag);
	std::vector<double> getXAxis(bool bAnalog);
	std::vector<double> getLattice();
	std::vector<double> getFeedForward();

	// Public Attributes

private:
	// Private Methods	
	void setAnalogMagnitude();
	void setAnalogPhase();
	void setDigitalMagnitude();
	void setDigitalPhase();
	void displayFilterParameters();
	
	std::vector<std::complex<double>> polyMul(std::vector<std::complex<double>> A, std::vector<std::complex<double>> B);
	void printPoly(std::vector<double>& vfPolynomial);
	double t_n(double& fFreq);

	// Private Attributes
	double m_fDiscretePass_rads;										// Discrete Edge Frequency (rad/s)
	double m_fDiscreteStop_rads;										// Discrete Corner Frequency (rad/s)
	double m_fOmegaPass_rads;											// Analog Edge Frequency (rad/s)
	double m_fOmegaStop_rads;											// Analog Corner Frequency (rad/s)
	double m_fRipplePass_ratio;											// Analog Ripple Passband (ratio)
	double m_fRippleStop_ratio;											// Analog Ripple Stopband (ratio)
	double fEpsilon;													// Epsilon for Analog Filter Design
	double m_fNumerator_s;												// Numerator of Analog Transfer Function
	
	int m_iSampleRate_Hz;												// Sample Rate (Hz)
	int m_iOrder_N;														// Order of designed filter

	std::vector<std::complex<double>> m_vfTransferFunction_s;			// Transfer Function vector of Analog Filter
	std::vector<double> m_vfDenominator_s;								// Denominator of Analog Transfer Function
	std::vector<double> m_vfMagnitude_s;								// Analog Filter Magnitude Response	
	std::vector<double> m_vfMagnitude_z;								// Digital Filter Magnitude Response	
	std::vector<double> m_vfPhase_s;									// Analog Filter Phase Response
	std::vector<double> m_vfPhase_z;									// Digital Filter Phase Response
	std::vector<double> m_vfX_s;										// X-axis of the Analog Magnitude	
	std::vector<double> m_vfX_z;										// X-axis of the Digital Magnitude
	std::vector<double> m_vfLattice_k;									// Gray-Markel Realisation Lattice Vector
	std::vector<double> m_vfFeedForward_a;								// Gray-Markel Realisation Feedforward Vector
	
};

