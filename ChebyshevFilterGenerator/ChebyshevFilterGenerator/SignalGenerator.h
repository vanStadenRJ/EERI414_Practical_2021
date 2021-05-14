#pragma once
// ---------------------------------------------------------------------------
// LIBRARY INCLUDE FILES
#include <iostream>
#include <string>
#include <vector>
#include <cmath>

class SignalGenerator
{
public:
	// Constructors
	SignalGenerator();
	SignalGenerator(int iSampleFreq_Hz, int iSignalLength_ms, int iLowestFreq_Hz, int iHighestFreq_Hz, int iSWeepType);

	// Public Methods
	std::vector<float> getSignal_Time();
	std::vector<float> getSignal_Freq();

	// Public Attributes

private:
	// Private Methods
	void generateSignal(int iSweepType);
	void calculateFFT();

	// Private Attributes
	std::vector<float> m_vfSignal_Time;
	std::vector<float> m_vfSignal_Freq;
	int m_iSampleFreq_Hz;
	int m_iLowestFreq_Hz;
	int m_iHighestFreq_Hz;
	int m_iSignalLength_ms;
};