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

cFilterDesign::cFilterDesign(int& iOmegaPass_Hz, int& iOmegaStop_Hz, int& iRipplePass_dB, int& iRippleStop_dB, int& iSampleRate_Hz)
{
	// Set filter design paramters based on Input variables
	m_fOmegaPass_rads = iOmegaPass_Hz * 2 * M_PI;
	m_fOmegaStop_rads = iOmegaStop_Hz * 2 * M_PI;
	m_fRipplePass_ratio = pow(10, (double)iRipplePass_dB / (double)20);
	m_fRippleStop_ratio = pow(10, (double)iRippleStop_dB / (double)20);
	m_iSampleRate_Hz = iSampleRate_Hz;

	// Calculate the Analog Transfer Function
	this->setAnalogFilterTF();
	this->setAnalogMagnitude();
	this->setAnalogPhase();
}


// ---------- CALCULATE ANALOG FILTER CHARACTERISTICS ----------
void cFilterDesign::setAnalogFilterTF()
{
	// Set Order N
	double fOrder = acosh(sqrt(((1 / pow(m_fRippleStop_ratio, 2)) - 1) / ((1 / pow(m_fRipplePass_ratio, 2)) - 1))) / acosh(m_fOmegaStop_rads / m_fOmegaPass_rads);
	m_iOrder_N = ceil(fOrder);
	std::cout << "Order: " << m_iOrder_N << std::endl;

	// Calculate the Minor and Major Axis
	fEpsilon = sqrt(1 / pow(m_fRipplePass_ratio, 2) - 1);
	std::cout << "Epsilon: " << fEpsilon << std::endl;
	double fUnit = pow(fEpsilon, -1) + sqrt(1 + pow(fEpsilon, -2));
	double fMinorAxis = m_fOmegaPass_rads * ((pow(fUnit, (double)1 / (double)m_iOrder_N) - pow(fUnit, (double)-1 / (double)m_iOrder_N)) / (double)2);
	double fMajorAxis = m_fOmegaPass_rads * ((pow(fUnit, (double)1 / (double)m_iOrder_N) + pow(fUnit, (double)-1 / (double)m_iOrder_N)) / (double)2);

	// Determine the Denominator of the Transfer Function
	std::vector<std::complex<double>> vfDenominator;
	for (int i = 0; i < m_iOrder_N; i++) {

		double fPhi = (M_PI / (double)2) + ((((2*i) + 1)*M_PI) / (2*(double)m_iOrder_N));
		//std::complex<double> fPole((fMinorAxis * cos(fPhi)) / m_fOmegaPass_rads, (fMajorAxis * sin(fPhi)) / m_fOmegaPass_rads);
		std::complex<double> fPole((fMinorAxis * cos(fPhi)), (fMajorAxis * sin(fPhi)));
		vfDenominator.push_back(fPole);
		if (m_iOrder_N % 2 == 0) {
			fPole = std::complex<double>(fPole.real(), fPole.imag() * -1);
			vfDenominator.push_back(fPole);
			i++;
		}		
	}
	std::cout << "======================" << std::endl;
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
	this->printPoly(m_vfDenominator_s);
	std::cout << "\n======================" << std::endl << std::endl;

	// Determine the Numerator of the Transfer Function
	std::complex<double> num(1,0);
	for (int i = 0; i < vfDenominator.size(); i++) {
		num = num * vfDenominator.at(i);
	}
	if (m_iOrder_N % 2 == 0) {
		num = num / sqrt(1 + pow(fEpsilon, 2));			// If Even order, divide by sqrt (1 + epsilon^2)
	}
	m_fNumerator_s = num.real();
	std::cout << "Numerator: " << num << std::endl;
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

		std::cout << "\t\t\t" << vfPolynomial.at(i) << std::endl;

		std::cout << std::endl;			
	}
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
		// Using Forumla
		double fFreq = (i * 2 * M_PI) / m_fOmegaPass_rads;
		double fMag = 1 / sqrt(1 + pow(fEpsilon, 2) * pow(t_n(fFreq), 2));
		m_vfX_s.push_back(i);
		m_vfMagnitude_s.push_back(20 * log10(fMag));		

		// Sweeping through possible frequencies
		/*std::complex<double> num(m_fNumerator_s, 0);
		std::complex<double> den(0, 0);

		double jw = i * 2 * M_PI;
		for (int j = 0; j < m_vfDenominator_s.size(); j++) {
			den = den + (m_vfDenominator_s.at(j) * pow(std::complex<double>(0, jw), m_vfDenominator_s.size() - 1 - j));
		}		

		std::complex<double> val = num / den;
		m_vfMagnitude_s.push_back(20 * log10(abs(val)));*/
	}
}

// ---------- Determine Analog Phase ----------
void cFilterDesign::setAnalogPhase()
{
	m_vfPhase_s = { };
	for (int i = 0; i < m_iSampleRate_Hz; i++) {
		std::complex<double> num(m_fNumerator_s, 0);
		std::complex<double> den(0, 0);
		
		double jw = i * 2 * M_PI;
		for (int j = 0; j < m_vfDenominator_s.size(); j++) {
			den = den + (m_vfDenominator_s.at(j) * pow(std::complex<double>(0, jw), m_vfDenominator_s.size() - 1 - j));
		}

		std::complex<double> val = num / den;
		m_vfPhase_s.push_back(atan(val.imag() / val.real()));

		/*if (i < 1000) {
			std::cout << i << ":\t" << m_vfMagnitude_s.at(i) << "\t" << m_vfPhase_s.at(i) << std::endl;
		}*/
	}
}