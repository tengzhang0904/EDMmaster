/*********************************************************************************************
*  This header gives:
       *** definition and methods for pause timing scans via PCI-6601
		   *channel for ext. triggered pulse generation
*  Last modified 2013/01/29 by KZ
**********************************************************************************************/

#ifndef _TimingPause_MT_h_included_
#define _TimingPause_MT_h_included_
#define _CRT_SECURE_NO_DEPRECATE

#include <stdio.h>
#include <math.h>
#include <iostream>
#include <iomanip>
#include <string>
#include <windows.h> 
#include <fstream> 
using namespace std;

#include "DAQmx/NIDAQmx.h" //Hardware control library for DAQmx
#pragma comment(lib, "NIDAQmx.lib")

// channel for ext. triggered delay pulse generation
bool TimingPause(float delay)
{
	#define DAQmxErrChk(functionCall) if( DAQmxFailed(error=(functionCall)) ) goto Error; else
	char        errBuff[2048]={'\0'};
	TaskHandle  taskHandle=0;
	int32       error=0;
	bool flag=true;
	float64     offset=0.000001;  //pulse offset =1 microsec
	float64     timeout=30.0; //time out in seconds
	float64     multiplier=0.001; //convert from input argument ms to sec
	float64     delaytime=0.001;
	if (delay<0.001)  
	{
		delaytime=0.001*multiplier; //set minimum delay time to be 1 microsec
	}
	else 
	{
		delaytime=delay*multiplier; 
		delaytime=ceil(delaytime*1000000.0)/1000000.0; //round numbers to 1 microsec
	}

	// DAQmx Configure Code
	DAQmxErrChk (DAQmxCreateTask("",&taskHandle));
	DAQmxErrChk (DAQmxCreateCOPulseChanTime(taskHandle,"Dev5/ctr1","",DAQmx_Val_Seconds,DAQmx_Val_Low,offset,offset,delaytime)); //idle state as HIGH
	DAQmxErrChk (DAQmxCfgDigEdgeStartTrig(taskHandle,"/Dev5/PFI34",DAQmx_Val_Rising));  //set ext. start trigger

	DAQmxErrChk (DAQmxStartTask(taskHandle)); // DAQmx Start Code
	DAQmxErrChk (DAQmxWaitUntilTaskDone(taskHandle,timeout)); // DAQmx Wait Code

  Error:
	if( DAQmxFailed(error) )
	{
		flag=false;
		DAQmxGetExtendedErrorInfo(errBuff,2048);
	}
	if( taskHandle!=0 ) {  // DAQmx Stop Code
		DAQmxStopTask(taskHandle);
		DAQmxClearTask(taskHandle);
	}
	if( DAQmxFailed(error) )
		printf("DAQmx Error: %s\n",errBuff);

	return flag;
}

#endif

