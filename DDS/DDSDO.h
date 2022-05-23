/***************************************************************

*  This file gives PCI6534 output settings and functions

*  Last modified 2011/11/22 by KZ

****************************************************************/


#ifndef _DDSDO_h_included_
#define _DDSDO_h_included_
#define _CRT_SECURE_NO_DEPRECATE

#include "DDSfunc.h" //Basic DDS conversion, arbitrary waveform control functions
#include "DAQmx/NIDAQmx.h" //Hardware control library for DAQmx
#include <stdio.h>
#include <math.h>
#include <iostream>
#include <iomanip>
#include <string>
#include <windows.h> 
#include "Timer/Timer.h" //precision timer between two break points
#pragma comment(lib, "NIDAQmx.lib")
using namespace std; 

#define DAQmxErrChk(functionCall) if( DAQmxFailed(error=(functionCall)) ) goto Error; else
const extern float Timeout;  //Max time waiting for trigger, in seconds

void OutputDataOnce(uInt32 Data[], unsigned int &DataSize)//output calulated pulse train once
{
	int         error=0;
	TaskHandle	taskHandle=0;
	char        errBuff[2048]={'\0'};
	int32		written;
    uInt64      generated;  

	// DAQmx Configure Code (takes ~200ms time; DMA-FIFO data transfer takes <10ms)
	DAQmxErrChk (DAQmxCreateTask("",&taskHandle));
	DAQmxErrChk (DAQmxCreateDOChan(taskHandle,"Dev2/port0:3","",DAQmx_Val_ChanForAllLines)); 
	DAQmxErrChk (DAQmxTaskControl(taskHandle,DAQmx_Val_Task_Commit));
    DAQmxErrChk (DAQmxCfgSampClkTiming(taskHandle,"/Dev2/PFI6",FIFOCLK,DAQmx_Val_Rising,DAQmx_Val_FiniteSamps,DataSize)); //External CLK on REQ1
    //DAQmxErrChk (DAQmxCfgSampClkTiming(taskHandle,NULL,FIFOCLK,DAQmx_Val_Rising,DAQmx_Val_FiniteSamps,DataSize));//Internal CLK

    // DAQmx Write/Start
	DAQmxErrChk (DAQmxWriteDigitalU32(taskHandle,DataSize,FALSE,Timeout,DAQmx_Val_GroupByScanNumber,Data,&written,NULL));	
	cout<<"DDS::Number of samples written to on-board FIFO: "<<written<<endl
		<<"DDS::Waiting for supertime trigger..."<<endl; 

    DAQmxErrChk (DAQmxStartTask(taskHandle));	
  
Error:
	if( DAQmxFailed(error) )
		DAQmxGetExtendedErrorInfo(errBuff,2048);
	if( taskHandle!=0 ) {   		// DAQmx Stop/Clear 
		DAQmxWaitUntilTaskDone(taskHandle,Timeout);
		DAQmxGetWriteTotalSampPerChanGenerated(taskHandle,&generated);
		cout<<"DDS::Number of samples output sucessfully: "<<generated<<endl;
		//DAQmxWriteDigitalU32(taskHandle,1,TRUE,Timeout,DAQmx_Val_GroupByScanNumber,[0],&written,NULL);//added by Teng
		DAQmxStopTask(taskHandle);
		DAQmxClearTask(taskHandle);
	}
	if( DAQmxFailed(error) )
		printf("DDS::DAQmx Error: %s\n",errBuff);
}

void OutputDataRepeat(int Nloop, uInt32 Data[], unsigned int &DataSize) //output calulated pulse train "Nloop" times
{
	int         error=0;
	TaskHandle	taskHandle=0;
	char        errBuff[2048]={'\0'};
	int32		written;
    uInt64      generated;  

	// DAQmx Configure Code (takes ~200ms time; DMA-FIFO data transfer takes <10ms)
	DAQmxErrChk (DAQmxCreateTask("",&taskHandle));
	DAQmxErrChk (DAQmxCreateDOChan(taskHandle,"Dev2/port0:3","",DAQmx_Val_ChanForAllLines)); 
	DAQmxErrChk (DAQmxTaskControl(taskHandle,DAQmx_Val_Task_Commit));
    DAQmxErrChk (DAQmxCfgSampClkTiming(taskHandle,"/Dev2/PFI6",FIFOCLK,DAQmx_Val_Rising,DAQmx_Val_FiniteSamps,DataSize)); //External CLK on REQ1

	for (int i=1; i<=Nloop; i++)
	{
        // DAQmx Write/Start
	    DAQmxErrChk (DAQmxWriteDigitalU32(taskHandle,DataSize,FALSE,Timeout,DAQmx_Val_GroupByScanNumber,Data,&written,NULL));	
		cout<<"DDS::Number of samples written to on-board FIFO: "<<written<<endl
			<<"DDS::Loop#"<<i<<":Waiting for supertime trigger..."<<endl; 

	    DAQmxErrChk (DAQmxStartTask(taskHandle));	
		if( taskHandle!=0 ) 
	       {   		
		     DAQmxWaitUntilTaskDone(taskHandle,Timeout);
		     DAQmxGetWriteTotalSampPerChanGenerated(taskHandle,&generated);
		     cout<<"DDS::Number of samples output sucessfully: "<<generated<<endl;
		     DAQmxStopTask(taskHandle);
	       }
	}//end of loop

	if( taskHandle!=0 ) 
	   DAQmxClearTask(taskHandle);

Error:
	if( DAQmxFailed(error) )
		DAQmxGetExtendedErrorInfo(errBuff,2048);
	if( DAQmxFailed(error) )
		printf("DDS::DAQmx Error: %s\n",errBuff); 
}

#endif

