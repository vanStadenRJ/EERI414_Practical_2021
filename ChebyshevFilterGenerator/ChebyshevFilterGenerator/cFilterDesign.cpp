// ---------------------------------------------------------------------------
// NAME & SURNAME       : RJ VAN STADEN
// STUDENTNUMBER        : 30026792
// ---------------------------------------------------------------------------
// DATE                 : 2021/05/05
// REVISION             : rev01
// ---------------------------------------------------------------------------
// PROJECT NAME         : ChebyshevFilterGenerator           
// ---------------------------------------------------------------------------


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
cFilterDesign::cFilterDesign():
	m_iOmegaPass_Hz(0),			
	m_iOmegaStop_Hz(0),			
	m_iRipplePass(0),				
	m_iRippleStop(0),
	m_fEps(0),
	m_fEps2(0),
	m_fA2(0),
	m_fk1(0),
	m_fk(0),
	m_fOrder(0)
{
	// DEFAULT CONSTRUCTOR
}

cFilterDesign::cFilterDesign(int& iOmegaPass_Hz, int& iOmegaStop_Hz, int& iRipplePass_dB, int& iRippleStop_dB):
	m_fEps(0),
	m_fEps2(0),
	m_fA2(0),
	m_fk1(0),
	m_fk(0),
	m_fOrder(0)
{
	// Populate Input Variable
	m_iOmegaPass_Hz = iOmegaPass_Hz;
	m_iOmegaStop_Hz = iOmegaStop_Hz;
	m_iRipplePass = iRipplePass_dB;
	m_iRippleStop = iRippleStop_dB;

	// Set the Order
	this->setOrder();
}

void cFilterDesign::setOrder()
{
	// Convert dB values to Ratios
	float fRipplePass_ratio = powf((float)10, (float)m_iRipplePass / (float)10);
	float fRippleStop_ratio = powf((float)10, (float)m_iRippleStop / (float)10);

	// Calculate Epsilon^2 and Epsilon
	std::cout << "Chebyshev Design" << std::endl;
	m_fEps2 = ((float)1 / fRipplePass_ratio) - 1;
	std::cout << "Epsilon^2:\t " << m_fEps2 << std::endl;	
	m_fEps = sqrtf(m_fEps2);
	std::cout << "Epsilon:\t " << m_fEps << std::endl;

	// Calculate A2
	m_fA2 = (float)1 / (float)fRippleStop_ratio;
	std::cout << "A^2:\t\t " << m_fA2 << std::endl;

	// Calculate Order of Filter
	m_fk = (float)m_iOmegaPass_Hz / (float)m_iOmegaStop_Hz;
	m_fk1 = m_fEps / sqrtf(m_fA2 - 1);
	m_fOrder = acoshf((float)1 / m_fk1) / acoshf((float)1 / m_fk);
	std::cout << "Order (N):\t " << m_fOrder << std::endl;
}