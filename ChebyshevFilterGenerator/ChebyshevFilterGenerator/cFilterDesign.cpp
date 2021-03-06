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

// ---------------------------------------------------------------------------
// SYSTEM INCLUDE FILES

// ---------------------------------------------------------------------------
// LIBRARY INCLUDE FILES
#include <cmath>
#include <math.h>
#include <iostream>

// ---------------------------------------------------------------------------
// LOCAL INCLUDE FILES
#include "cFilterDesign.h"


// ---------- CONSTRUCTOR CALLS ----------
cFilterDesign::cFilterDesign() :
	m_fOmegaPass_rads(0.0),
	m_fOmegaStop_rads(0.0),
	m_fRipplePass_ratio(0.0),
	m_fRippleStop_ratio(0.0)
{
	// DEFAULT CONSTRUCTOR
}

cFilterDesign::cFilterDesign(int& iOmegaPass_Hz, int& iOmegaStop_Hz, double& iRipplePass_dB, double& iRippleStop_dB, int& iSampleRate_Hz)
{
	// Set filter design paramters based on Input variables
	m_iSampleRate_Hz = iSampleRate_Hz;
	m_fDiscretePass_rads = (double)(iOmegaPass_Hz * 2 * M_PI) / (double)(m_iSampleRate_Hz);
	m_fDiscreteStop_rads = (double)(iOmegaStop_Hz * 2 * M_PI) / (double)(m_iSampleRate_Hz);
	m_fRipplePass_ratio = pow(10, iRipplePass_dB / (double)20);
	m_fRippleStop_ratio = pow(10, iRippleStop_dB / (double)20);	

	// Transform the Discrete Cut Off Frequencies to the Analog Domain
	m_fOmegaPass_rads = (tan(m_fDiscretePass_rads / (double)2));
	m_fOmegaStop_rads = (tan(m_fDiscreteStop_rads / (double)2));	
}

// ---------- DISPLAY FILTER PARAMETERS ----------
void cFilterDesign::displayFilterParameters()
{
	std::cout << "Discrete Pass Freq (rad/s): " << m_fDiscretePass_rads << std::endl;
	std::cout << "Discrete Stop Freq (rad/s): " << m_fDiscreteStop_rads << std::endl;
	std::cout << "Analog Pass Freq (Hz):      " << m_fOmegaPass_rads << std::endl;
	std::cout << "Analog Stop Freq (Hz):      " << m_fOmegaStop_rads << std::endl;
}

// ---------- CALCULATE ANALOG FILTER CHARACTERISTICS ----------
void cFilterDesign::setAnalogFilterTF()
{
	// Display Filter Parameters
	this->displayFilterParameters();

	// Set Order N
	double fOrder = acosh(sqrt(((1 / pow(m_fRippleStop_ratio, 2)) - 1) / ((1 / pow(m_fRipplePass_ratio, 2)) - 1))) / acosh(m_fOmegaStop_rads / m_fOmegaPass_rads);
	m_iOrder_N = ceil(fOrder);
	std::cout << "Order (N):                  " << fOrder << "~~ " << m_iOrder_N <<std::endl;

	// Calculate the Minor and Major Axis
	fEpsilon = sqrt(1 / pow(m_fRipplePass_ratio, 2) - 1);
	std::cout << "Epsilon:                    " << fEpsilon << std::endl;
	double fUnit = pow(fEpsilon, -1) + sqrt(1 + pow(fEpsilon, -2));
	std::cout << "A:                          " << fUnit << std::endl;
	double fMinorAxis = m_fOmegaPass_rads * ((pow(fUnit, (double)1 / (double)m_iOrder_N) - pow(fUnit, (double)-1 / (double)m_iOrder_N)) / (double)2);
	std::cout << "Minor Axis:                 " << fMinorAxis << std::endl;
	double fMajorAxis = m_fOmegaPass_rads * ((pow(fUnit, (double)1 / (double)m_iOrder_N) + pow(fUnit, (double)-1 / (double)m_iOrder_N)) / (double)2);
	std::cout << "Major Axis:                 " << fMajorAxis << std::endl;

	// Determine the Denominator of the Transfer Function
	std::vector<std::complex<double>> vfDenominator;
	for (int i = 0; i < m_iOrder_N; i++) {

		double fPhi = (M_PI / (double)2) + ((((2*i) + 1)*M_PI) / (2*(double)m_iOrder_N));
		std::complex<double> fPole((fMinorAxis * cos(fPhi)), (fMajorAxis * sin(fPhi)));
		vfDenominator.push_back(fPole);
		if (m_iOrder_N % 2 == 0) {
			fPole = std::complex<double>(fPole.real(), fPole.imag() * -1);
			vfDenominator.push_back(fPole);
			i++;
		}		
	}
	std::cout << "=======================" << std::endl;
	std::cout << "LOW PASS FILTER DESIGN:" << std::endl;
	std::cout << "=======================" << std::endl;
	for (int i = 0; i < vfDenominator.size(); i++) {
		std::cout << "Pole " << i << ": " << vfDenominator.at(i) << std::endl;
	}
	std::vector<std::complex<double>> vfA{ 1 };
	for (int i = 0; i < vfDenominator.size(); i++) {
		vfDenominator[i] = std::complex<double>(vfDenominator[i].real() * -1, vfDenominator[i].imag());
		std::vector<std::complex<double>> vfB{ vfDenominator.at(i), 1 };
		vfA = polyMul(vfA, vfB);		
	}
	std::cout << std::endl;
	for (int i = 0; i < vfA.size(); i++) {
		m_vfDenominator_s.push_back(vfA[vfA.size() - 1 - i].real());
	}	

	// Determine the Numerator of the Transfer Function
	std::complex<double> num(1,0);
	for (int i = 0; i < vfDenominator.size(); i++) {
		num = num * vfDenominator.at(i);
	}
	if (m_iOrder_N % 2 == 0) {
		num = num / sqrt(1 + pow(fEpsilon, 2));			// If Even order, divide by sqrt (1 + epsilon^2)
	}
	m_fNumerator_s = num.real();

	// Display LP Transfer Function
	std::cout << "\t\t\t" << m_fNumerator_s << std::endl;
	std::cout << "------------------------------------------------------------------" << std::endl;
	this->printPoly(m_vfDenominator_s);

	std::cout << "\n======================" << std::endl << std::endl;

	// Continue to Rest of Filter Design
	this->setAnalogMagnitude();
	this->setAnalogPhase();
	this->setDigitalMagnitude();
	this->setDigitalPhase();	
}

// ---------- Multiply Factors of Polynomial ----------
std::vector<std::complex<double>> cFilterDesign::polyMul(std::vector<std::complex<double>> A, std::vector<std::complex<double>> B)
{
	std::vector<std::complex<double>> vfProd;
	int m = A.size();
	int n = B.size();
	// Initialize the porduct polynomial
	for (int i = 0; i < m + n - 1; i++)
		vfProd.push_back((0, 0));

	// Multiply two polynomials term by term

	// Take ever term of first polynomial
	for (int i = 0; i < m; i++)
	{
		// Multiply the current term of first polynomial
		// with every term of second polynomial.
		for (int j = 0; j < n; j++)
			vfProd.at(i + j) += A.at(i) * B.at(j);
	}

	return vfProd;
}

// ---------- Print a Multiplied Polynomial ----------
void cFilterDesign::printPoly(std::vector<double>& vfPolynomial)
{
	int n = vfPolynomial.size();
	for (int i = 0; i < n; i++)
	{
		std::cout << vfPolynomial.at(i);			
		if (i != n - 1) {			
			std::cout << "s^" << (n - 1 - i);
			std::cout << " + ";			
		}		
	}
	std::cout << std::endl;
}

// ---------- Analog Design: T(N) Function ----------
double cFilterDesign::t_n(double& fFreq)
{
	if (abs(fFreq) <= 1) {
		return cos(m_iOrder_N * acos(fFreq));
	}
	else {
		return cosh(m_iOrder_N * acosh(fFreq));
	}
}

// ---------- Determine Analog Magnitude ----------
void cFilterDesign::setAnalogMagnitude()
{
	m_vfMagnitude_s = { };
	//m_vfPhase_s = { };
	m_vfX_s = { };
	for (int i = 0; i < m_iSampleRate_Hz; i++) {
		// Sweeping through possible frequencies
		std::complex<double> num(m_fNumerator_s, 0);
		std::complex<double> den(0, 0);

		//double jw = i * 2 * M_PI;
		double jw = (i * 2 * M_PI) / (double)m_iSampleRate_Hz;
		for (int j = 0; j < m_vfDenominator_s.size(); j++) {
			//std::complex<double> val = (m_fOmegaPass_rads * m_fOmegaStop_rads) / std::complex<double>(0, jw);

			std::complex<double> s = std::complex<double>(0, jw);
			std::complex<double> val = std::complex<double>(m_fOmegaStop_rads * m_fOmegaPass_rads, 0) / s;

			//den = den + (m_vfDenominator_s.at(j) * pow(std::complex<double>(0, jw), m_vfDenominator_s.size() - 1 - j));
			den = den + (m_vfDenominator_s.at(j) * pow(val, m_vfDenominator_s.size() - 1 - j));
		}		

		std::complex<double> val = num / den;
		m_vfMagnitude_s.push_back(20 * log10(abs(val)));
		m_vfX_s.push_back(jw);
	}
}

// ---------- Determine Digital Magnitude ----------
void cFilterDesign::setDigitalMagnitude()
{
	m_vfMagnitude_z = { };
	//m_vfPhase_s = { };
	m_vfX_z = { };
	for (int i = 0; i < m_iSampleRate_Hz / 2; i++) {
		// Sweeping through possible frequencies
		std::complex<double> num(m_fNumerator_s, 0);
		std::complex<double> den(0, 0);

		//double jw = i * 2 * M_PI;
		double jw = (i * 2 * M_PI) / (double)m_iSampleRate_Hz;
		for (int j = 0; j < m_vfDenominator_s.size(); j++) {

			std::complex<double> ejw = exp(std::complex<double>(0, jw));
			std::complex<double> s = (ejw - std::complex<double>(1, 0)) / (ejw + std::complex<double>(1, 0));
			std::complex<double> val = std::complex<double>(m_fOmegaStop_rads * m_fOmegaPass_rads, 0) / s;

			//den = den + (m_vfDenominator_s.at(j) * pow(s, m_vfDenominator_s.size() - 1 - j));
			den = den + (m_vfDenominator_s.at(j) * pow(val, m_vfDenominator_s.size() - 1 - j));
		}

		std::complex<double> val = num / den;
		double mag = sqrt(pow(val.real(), 2) + pow(val.imag(), 2));
		m_vfMagnitude_z.push_back(20 * log10(mag));
		m_vfX_z.push_back(jw);
	}
}

// ---------- Determine Analog Phase ----------
void cFilterDesign::setAnalogPhase()
{
	m_vfPhase_s = { };
	m_vfTransferFunction_s = { };
	for (int i = 0; i < m_iSampleRate_Hz; i++) {
		// Sweeping through possible frequencies
		std::complex<double> num(m_fNumerator_s, 0);
		std::complex<double> den(0, 0);

		//double jw = i * 2 * M_PI;
		double jw = (i * 2 * M_PI) / (double)m_iSampleRate_Hz;
		for (int j = 0; j < m_vfDenominator_s.size(); j++) {

			std::complex<double> s = std::complex<double>(0, jw);
			std::complex<double> val = std::complex<double>(m_fOmegaStop_rads * m_fOmegaPass_rads, 0) / s;

			//den = den + (m_vfDenominator_s.at(j) * pow(std::complex<double>(0, jw), m_vfDenominator_s.size() - 1 - j));
			den = den + (m_vfDenominator_s.at(j) * pow(val, m_vfDenominator_s.size() - 1 - j));
		}

		std::complex<double> val = num / den;
		m_vfTransferFunction_s.push_back(val);
		m_vfPhase_s.push_back(atan(val.imag() / val.real()));
	}
}

// ---------- Determine Digital Phase ----------
void cFilterDesign::setDigitalPhase()
{
	m_vfPhase_z = { };
	m_vfTransferFunction_s = { };
	for (int i = 0; i < m_iSampleRate_Hz / 2; i++) {
		
		// Sweeping through possible frequencies
		std::complex<double> num(m_fNumerator_s, 0);
		std::complex<double> den(0, 0);

		//double jw = i * 2 * M_PI;
		double jw = (i * 2 * M_PI) / (double)m_iSampleRate_Hz;
		for (int j = 0; j < m_vfDenominator_s.size(); j++) {
			//std::complex<double> val = (m_fOmegaPass_rads * m_fOmegaStop_rads) / std::complex<double>(0, jw);

			std::complex<double> ejw = exp(std::complex<double>(0, jw));
			std::complex<double> s = (ejw - std::complex<double>(1, 0)) / (ejw + std::complex<double>(1, 0));
			std::complex<double> val = std::complex<double>(m_fOmegaStop_rads * m_fOmegaPass_rads, 0) / s;

			//den = den + (m_vfDenominator_s.at(j) * pow(s, m_vfDenominator_s.size() - 1 - j));
			den = den + (m_vfDenominator_s.at(j) * pow(val, m_vfDenominator_s.size() - 1 - j));
		}

		std::complex<double> val = num / den;
		m_vfTransferFunction_s.push_back(val);
		m_vfPhase_z.push_back(atan(val.imag() / val.real()));
	}
}

// ---------- Determine Analog Phase ----------
std::vector<std::complex<double>> cFilterDesign::getTransferFunction(bool bAnalog)
{
	if (bAnalog == true) {
		// Return Analog Transfer Function
		return m_vfTransferFunction_s;
	}
	else {
		// Return Digital Transfer Function
		//return m_vfTransferFunction_z;
	}	
}

// ---------- Determine Analog Phase ----------
std::vector<double> cFilterDesign::getYAxis(bool bAnalog, bool bMag)
{
	if (bAnalog == true) {
		// Return Analog Magnitude / Phase
		if (bMag == true) {
			// Return Magnitude Spectrum
			return m_vfMagnitude_s;
		}
		else {
			return m_vfPhase_s;
		}
	}
	else {
		// Return Digital Magnitude / Phase
		if (bMag == true) {
			// Return Magnitude Spectrum
			return m_vfMagnitude_z;
		}
		else {
			return m_vfPhase_z;
		}
	}
}

// ---------- Gray-Markel Realisation
void cFilterDesign::setGrayMarkel(std::vector<double>& vfNumerator, std::vector<double>& vfDenominator)
{
	// Clear Lattice and Feedback Vectors
	m_vfLattice_k = { };
	m_vfFeedForward_a = { };

	// Get Degree of Polynomial
	int N = vfDenominator.size() - 1;

	std::vector<double> a_1;
	std::vector<double> del;
	for (int i = 0; i < N;  i++) {
		// Initialise Lattice Vector with ones
		m_vfLattice_k.push_back(1);	
	}
	
	// Determine a_1
	for (int i = 0; i < N + 1; i++) {		
		a_1.push_back(vfDenominator.at(i) / vfDenominator.at(0));
		del.push_back(vfDenominator.at(i) / vfDenominator.at(0));
	}

	// Initialise Feed-Forward vector
	for (int i = N; i >= 0; i--) {
		m_vfFeedForward_a.push_back(vfNumerator.at(i) / vfDenominator.at(0));
	}

	// Gray-Markel Realisation
	for (int i = N - 1; i >= 0; i--) {
		// Set Feed-Forward
		int t = 1;
		for (int j = N - i; j <= N; j++) {
			m_vfFeedForward_a[j] = m_vfFeedForward_a[j] - (m_vfFeedForward_a[N - i - 1] * a_1[t]);
			t++;
		}

		// Set Lattice
		m_vfLattice_k[i] = a_1[i+1];

		// Set a_1
		t = i+1;
		for (int j = 0; j <= i+1; j++) {
			del[j] = (a_1[j] - (m_vfLattice_k[i] * a_1[t])) / ((double)1 - (m_vfLattice_k[i] * m_vfLattice_k[i]));
			t--;
		}
		a_1 = del;
	}	
}

// ---------- Determine Analog Phase ----------
std::vector<double> cFilterDesign::getXAxis(bool bAnalog)
{
	if (bAnalog == true) {
		// Return Analog x-axis
		return m_vfX_s;
	}
	else {
		// Return Digital x-axis
		return m_vfX_z;
	}
}

// ---------- Return Lattice Vector (Gray-Markel) ----------
std::vector<double> cFilterDesign::getLattice()
{
	return m_vfLattice_k;
}

// ---------- Return Feed-Forward Vector (Gray-Markel) ----------
std::vector<double> cFilterDesign::getFeedForward()
{
	return m_vfFeedForward_a;
}