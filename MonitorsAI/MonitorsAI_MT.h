// Function call to Analog Monitors
// This file is created to wrap some implementations of analog monitors up
// so MMC doesn't have to create a new analog monitor object. It is meant to save
// some lines in the MMC dialog function. 
// By Teng Zhang 12/14/2017


#ifndef _MonitorsAI_MT_included_
#define _MonitorsAI_MT_included_
#define _CRT_SECURE_NO_DEPRECATE
//#pragma warning(disable: 4345)

#include "MonitorsAI.h"

#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <math.h>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <windows.h> 
using namespace std;

const extern string MMCfolder; //MMC_SYSINT folder

float64 TopLeakageAI(int shotid) // Using two analog input channels to monitor leakage currents
	                           // from the Keithley picoammeters.
							   // The shotid is dummy. Always pass 0 to call this function
							   // because we're only display it once.
{
	float64 TLeakageCurrent;
	//string AIChls="DevAI/ai0:15"; //specify AI channel(s) for monitors input // change this to DevAI from cDAQ1Mod1
	//string AIpat="DevAI/ai30:31"; //use last 2 channels as pattern match channel

	Amonitors *LeakageMonitor = new Amonitors();
	LeakageMonitor->init(); 
	LeakageMonitor->Measure(shotid); // Measure all the specified AI channels.

	TLeakageCurrent = LeakageMonitor->data[shotid][0]; // return the 1st channel which is Top Picoammeter
	delete LeakageMonitor;
	return TLeakageCurrent;
}

float64 BotLeakageAI(int shotid) // Using two analog input channels to monitor leakage currents
	                           // from the Keithley picoammeters.
							   // The shotid is dummy. Always pass 0 to call this function
							   // because we're only display it once.
{
	float64 BLeakageCurrent;
	//string AIChls="DevAI/ai0:15"; //specify AI channel(s) for monitors input // change this to DevAI from cDAQ1Mod1
	//string AIpat="DevAI/ai30:31"; //use last 2 channels as pattern match channel

	Amonitors *LeakageMonitor = new Amonitors();
	LeakageMonitor->init(); 
	LeakageMonitor->Measure(shotid); // Measure all the specified AI channels.

	BLeakageCurrent = LeakageMonitor->data[shotid][1]; // return the 2nd channel which is Bottom Picoammeter
	delete LeakageMonitor;
	return BLeakageCurrent;
}

#endif