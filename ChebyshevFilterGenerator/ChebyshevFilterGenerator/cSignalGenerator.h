#pragma once
// ---------------------------------------------------------------------------
// LIBRARY INCLUDE FILES
#include <iostream>
#include <string>
#include <vector>
#include <cmath>

// ---------------------------------------------------------------------------
// LOCAL INCLUDE FILES


class cSignalGenerator
{
public:
	// Constructors
	cSignalGenerator();
	cSignalGenerator(int &iSampleFreq_Hz, int &iSignalLength_ms, int &iLowestFreq_Hz, int &iHighestFreq_Hz, int iSWeepType);

	// Public Methods
	std::vector<double> getSignal_Time();
	std::vector<double> getSignal_Freq();

	// Public Attributes

private:
	// Private Methods
	void generateSignal(int &iSweepType);

	// Private Attributes
	std::vector<double> m_vfSignal_Time;
	std::vector<double> m_vfSignal_Freq;
	int m_iSampleFreq_Hz;
	int m_iLowestFreq_Hz;
	int m_iHighestFreq_Hz;
	int m_iSignalLength_ms;
};