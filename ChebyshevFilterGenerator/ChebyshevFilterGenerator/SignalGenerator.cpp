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

// ---------------------------------------------------------------------------
// LOCAL INCLUDE FILES
#include "SignalGenerator.h"

SignalGenerator::SignalGenerator(void) :
	m_iHighestFreq_Hz(0),
	m_iLowestFreq_Hz(0),
	m_iSampleFreq_Hz(0),
	m_iSignalLength_ms(0)
{
	
}

SignalGenerator::SignalGenerator(int iSampleFreq_Hz, int iSignalLength_ms, int iLowestFreq_Hz, int iHighestFreq_Hz, int iSWeepType)
{
	m_iSampleFreq_Hz = iSampleFreq_Hz;			// Get Sample Rate from main
	m_iLowestFreq_Hz = iLowestFreq_Hz;			// Get smallest Frequency in range for generation
	m_iHighestFreq_Hz = iHighestFreq_Hz;		// Get largest frequency in range for generation
	m_iSignalLength_ms = iSignalLength_ms;		// Get length of signal in seconds

	this->generateSignal(iSWeepType);						// Generate the Time-Domain Signal	
	this->calculateFFT();			// Calculate the FFT of the generated signal
}

// ---------- GENERATE TIME-DOMAIN SIGNAL ----------
std::vector<float> SignalGenerator::getSignal_Time()
{
	return m_vfSignal_Freq;
}

std::vector<float> SignalGenerator::getSignal_Freq()
{
	return m_vfSignal_Freq;
}

void SignalGenerator::generateSignal(int iSweepType)
{
	/*
	Generate a 5 second Signal with frequencies ranging from 0 30 kHz
	- Sweep for 0 khz- 30kHz in first 2.5 seconds
	- Sweep from 30kHz - 0kHz in last 2.5 seconds	
	*/

	float T = (float)m_iSignalLength_ms / (float)1000;
	float fNrSamples = T * m_iSampleFreq_Hz;

	for (int i = 0; i <= fNrSamples; i++) {
		// Translate Nr of Samples to Times
		float t = i * (T / fNrSamples);

		switch (iSweepType) {
		// Calculate the Linear Sweep from Lowest to Highest Frequency
		case 0:
			m_vfSignal_Time.push_back(sin(2 * M_PI * ((m_iLowestFreq_Hz * t) + (((m_iHighestFreq_Hz - m_iLowestFreq_Hz) / (2 * T)) * pow(t, 2)))));
			break;
		
		// Calculate the Logarithmic Sweep from Lowest to Highest Frequency
		case 1:
			m_vfSignal_Time.push_back(sin(2 * M_PI * m_iLowestFreq_Hz * T * ((pow(((m_iHighestFreq_Hz) / (m_iLowestFreq_Hz)), (t / T)) - 1) / (log((m_iHighestFreq_Hz) / (m_iLowestFreq_Hz))))));
			break;

		// Calculate the summation of 5 different frequencies, equally distanced
		case 2:
			float d = 0;
			for (int j = 0; i < 5; i++) {
				float f = ((float)m_iHighestFreq_Hz / (float)5) * j;
				d = d + sin(2 * M_PI * f * t);
			}
			m_vfSignal_Time.push_back(d);
			break;
		}		
	}

	// After calculating the Time-Domain variant of the signal, calculate FFT
	this->calculateFFT();
}

void SignalGenerator::calculateFFT()
{
	//
}