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
#include <complex>

// ---------------------------------------------------------------------------
// LOCAL INCLUDE FILES
#include "cSignalGenerator.h"

cSignalGenerator::cSignalGenerator(void) :
	m_iHighestFreq_Hz(0),
	m_iLowestFreq_Hz(0),
	m_iSampleFreq_Hz(0),
	m_iSignalLength_ms(0)
{
	// DEFAULT CONSTRUCTOR
}

cSignalGenerator::cSignalGenerator(int &iSampleFreq_Hz, int &iSignalLength_ms, int &iLowestFreq_Hz, int &iHighestFreq_Hz)
{
	m_iSampleFreq_Hz = iSampleFreq_Hz;			// Get Sample Rate from main
	m_iLowestFreq_Hz = iLowestFreq_Hz;			// Get smallest Frequency in range for generation
	m_iHighestFreq_Hz = iHighestFreq_Hz;		// Get largest frequency in range for generation
	m_iSignalLength_ms = iSignalLength_ms;		// Get length of signal in seconds	
}

// ---------- GENERATE TIME-DOMAIN SIGNAL ----------
std::vector<double> cSignalGenerator::getSignal_Time()
{
	return m_vfSignal_Time;
}

std::vector<double> cSignalGenerator::getSignal_Freq()
{
	return m_vfSignal_Freq;
}

void cSignalGenerator::generateSignal(int iSweepType)
{
	/*
	Generate a 5 second Signal with frequencies ranging from 0 30 kHz
	- Sweep for 0 khz- 30kHz in first 2.5 seconds
	- Sweep from 30kHz - 0kHz in last 2.5 seconds	
	*/
	m_vfSignal_Time = { };
	double T = (double)m_iSignalLength_ms / (double)1000;
	double fNrSamples = T * m_iSampleFreq_Hz;

	for (int i = 0; i < fNrSamples; i++) {
		// Translate Nr of Samples to Times
		double t = i * (T / fNrSamples);

		switch (iSweepType) {
		// Calculate the Linear Sweep from Lowest to Highest Frequency
		case 0:
			m_vfSignal_Time.push_back(1 * sin(2 * M_PI * ((m_iLowestFreq_Hz * t) + (((m_iHighestFreq_Hz - m_iLowestFreq_Hz) / (2 * T)) * pow(t, 2)))));
			break;
		
		// Calculate the Logarithmic Sweep from Lowest to Highest Frequency
		case 1:
			m_vfSignal_Time.push_back(1 * sin(2 * M_PI * m_iLowestFreq_Hz * T * ((pow(((m_iHighestFreq_Hz) / (m_iLowestFreq_Hz)), (t / T)) - 1) / (log((m_iHighestFreq_Hz) / (m_iLowestFreq_Hz))))));
			break;

		// Calculate the summation of 5 different frequencies, equally distanced
		case 2:
			double d = sin(2 * M_PI * m_iLowestFreq_Hz * t);
			for (int j = 1; j <= 500; j++) {
				double f = ((double)m_iHighestFreq_Hz / (double)500) * j;
				d = d + sin(2 * M_PI * f * t);
			}
			d = d + sin(2 * M_PI * 72250 * t) + sin(2 * M_PI * 42250 * t);
			m_vfSignal_Time.push_back(d);
			break;
		}		
	}
}