#pragma once
// ---------------------------------------------------------------------------
// SYSTEM INCLUDE FILES

// ---------------------------------------------------------------------------
// LIBRARY INCLUDE FILES

// ---------------------------------------------------------------------------
// LOCAL INCLUDE FILES

class cFilterDesign
{
public:
	// Constructors
	cFilterDesign();
	cFilterDesign(int &iOmegaPass_Hz, int &iOmegaStop_Hz, int &iRipplePass_dB, int &iRippleStop_dB);

	// Public Methods

	// Public Attributes

private:
	// Private Methods
	void setOrder();

	// Private Attributes
	int m_iOmegaPass_Hz;			// Analog Edge Frequency
	int m_iOmegaStop_Hz;			// Analog Corner Frequency
	int m_iRipplePass;				// Analog Ripple Passband 
	int m_iRippleStop;				// Analog Stopband Passband
	
	float m_fEps;
	float m_fEps2;
	float m_fA2;
	float m_fk1;
	float m_fk;
	float m_fOrder;
};

