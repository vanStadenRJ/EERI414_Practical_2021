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

cFilterDesign::cFilterDesign(int& iOmegaPass_Hz, int& iOmegaStop_Hz, int& iRipplePass_dB, int& iRippleStop_dB)
{
	// Set filter design paramters based on Input variables
	m_fOmegaPass_rads = iOmegaPass_Hz * 2 * M_PI;
	m_fOmegaStop_rads = iOmegaStop_Hz * 2 * M_PI;
	m_fRipplePass_ratio = pow(10, (double)iRipplePass_dB / (double)20);
	m_fRippleStop_ratio = pow(10, (double)iRippleStop_dB / (double)20);

	// Calculate the Analog Transfer Function
	this->setAnalogFilterTF();
}


// ---------- CALCULATE ANALOG FILTER CHARACTERISTICS ----------
void cFilterDesign::setAnalogFilterTF()
{
	// Set Order N
	double fOrder = acosh(sqrt(((1 / pow(m_fRippleStop_ratio, 2)) - 1) / ((1 / pow(m_fRipplePass_ratio, 2)) - 1))) / acosh(m_fOmegaStop_rads / m_fOmegaPass_rads);
	m_iOrder_N = ceil(fOrder);
	std::cout << "Order: " << m_iOrder_N << std::endl;

	// Calculate the Minor and Major Axis
	double fEpsilon = sqrt(1 / pow(m_fRipplePass_ratio, 2) - 1);
	std::cout << "Epsilon: " << fEpsilon << std::endl;
	double fUnit = pow(fEpsilon, -1) + sqrt(1 + pow(fEpsilon, -2));
	double fMinorAxis = m_fOmegaPass_rads * ((pow(fUnit, (double)1 / (double)m_iOrder_N) - pow(fUnit, (double)-1 / (double)m_iOrder_N)) / (double)2);
	double fMajorAxis = m_fOmegaPass_rads * ((pow(fUnit, (double)1 / (double)m_iOrder_N) + pow(fUnit, (double)-1 / (double)m_iOrder_N)) / (double)2);

	// Determine the Denominator of the Transfer Function
	std::vector<std::complex<double>> vfDenominator;
	for (int i = 0; i < m_iOrder_N; i++) {

		double fPhi = (M_PI / (double)2) + ((((2*i) + 1)*M_PI) / (2*(double)m_iOrder_N));
		//std::complex<double> fPole((fMinorAxis * cos(fPhi)) / m_fOmegaPass_rads, (fMajorAxis * sin(fPhi)) / m_fOmegaPass_rads);
		std::complex<double> fPole(fMinorAxis * cos(fPhi), fMajorAxis * sin(fPhi));
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
			
	}
}