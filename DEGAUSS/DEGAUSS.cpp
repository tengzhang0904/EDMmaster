/***************************************************************************
* Program to generate degauss waveform: int. CLK, finite sample continuous
* Last modified 07/17/2012
* PCI-6713: maxRate-1MS/s, Vres-12bits, FIFO-15536 samples, 8 AO channels
* To compile in cmd.exe "cl DEGAUSS.cpp /EHsc"
****************************************************************************/ 

/***************************************************************************
* System requirements
* (1)generate 10Hz sine wave
* (2)to start from and stop at zero crossing
* (3)amplitude change I.  ramp up from 0 to max in 10s or longer
                      II. stay at max for 20 cycles or longer
					  III. ramp down to zero linearly in ~3000 cycles or longer
* (4)at the end of output (delay<1.5ms), send digital HIGH to turn on the switch 
****************************************************************************/ 


#include <stdio.h>
#include <math.h>
#include <iostream>
#include <string>
#include <windows.h>
#include <time.h>
using namespace std;

#include "NIDAQmx.h"
#pragma comment(lib, "NIDAQmx.lib")

#define freq  2.0  //degauss frequency
#define Vpp  19.0 //Voltage pk-pk
#define Nup  100 //ramp up cycles
#define Nst  100   //cycles at amx
#define Ndn  1000 //ramp down cycles
#define PI 3.14159265359
#define SampleRate 10000.0  //Sample rate in Hz

//Function to compute waveform vs sample index
float64 waveform(unsigned int x,float64 Gain);

static TaskHandle  AOtaskHandle=0,DOtaskHandle=0;
#define DAQmxErrChk(functionCall) if( DAQmxFailed(error=(functionCall)) ) goto Error; else

int main(void)
{
	int         error=0;
	TaskHandle  taskHandle=0;
	char        errBuff[2048]={'\0'};
	int32   	AOwritten;

	/*********************************************/
	// Compute waveform data array
	/*********************************************/
	const int CalcNum=(int)(SampleRate/freq*(Nup+Nst+Ndn)); //Must be even
	static float64    data[CalcNum]={0.0}; //data array to output
    uInt32      SampleNumber=CalcNum;  //total sample number
	FILE * pFile; pFile = fopen ("data.txt","w");
	for (unsigned int i=0;i<SampleNumber;i++) 
	    {  data[i]=waveform(i+1,Vpp/2.0); 
	       //data[i]=data[i]+0.2;//Test effect of 1Hz high pass
	       fprintf(pFile,"%f\n", data[i]); 
	    }
    printf("Computation complete, %d samples\n", SampleNumber);
	fclose (pFile); 
    
	//////////////////////////////////////////////////////////////////////////////////////////
	// Digital switch settings
    /*DAQmxErrChk (DAQmxCreateTask("",&DOtaskHandle));
	DAQmxErrChk (DAQmxCreateDOChan(DOtaskHandle,"Dev4/port0/line0","",DAQmx_Val_ChanPerLine)); 
	static uInt8        SWLOW[1]={0},SWHIGH[1]={1};
	DAQmxErrChk (DAQmxStartTask(DOtaskHandle));
	DAQmxErrChk (DAQmxWriteDigitalLines(DOtaskHandle,1,0,10.0,DAQmx_Val_GroupByChannel,SWLOW,NULL,NULL)); 
	//data[SampleNumber-1]=-1.0; //AO-DO delay test, not-in-use for normal operation
    //////////////////////////////////////////////////////////////////////////////////////////
        //write digital HIGH to the switch
	DAQmxErrChk (DAQmxWriteDigitalLines(DOtaskHandle,1,0,10.0,DAQmx_Val_GroupByChannel,SWHIGH,NULL,NULL));*/
	/*********************************************/
	// Analog waveform Channel settings
	/*********************************************/
	DAQmxErrChk (DAQmxCreateTask("",&AOtaskHandle));
	DAQmxErrChk (DAQmxCreateAOVoltageChan(AOtaskHandle,"Dev4/ao0","",-10.0,10.0,DAQmx_Val_Volts,NULL));
	DAQmxErrChk (DAQmxCfgSampClkTiming(AOtaskHandle,"",SampleRate,DAQmx_Val_Rising,DAQmx_Val_FiniteSamps,SampleNumber));
	//DAQmxErrChk(DAQmxCfgDigEdgeStartTrig(AOtaskHandle,"Dev4/PFI0",DAQmx_Val_Rising));
//DAQmxErrChk(DAQmxTaskControl(AOtaskHandle,DAQmx_Val_Task_Commit));
	DAQmxErrChk (DAQmxWriteAnalogF64(AOtaskHandle,SampleNumber,0,10.0,DAQmx_Val_GroupByChannel,data,&AOwritten,NULL)); 
	printf("%d samples written, press any key to start output\n",AOwritten); getchar();printf("output in progress...\n");
	DAQmxErrChk (DAQmxStartTask(AOtaskHandle));
	DAQmxErrChk (DAQmxWaitUntilTaskDone(AOtaskHandle,DAQmx_Val_WaitInfinitely));
    
	
	
	//Manually turn off the switch
	printf("Waveform output complete/Switch on HIGH, please wait for a few minutes...\n"); 
	printf("Disconnect degauss coil from amplifier, then press any key to turn off switch\n"); 
	getchar();// wait for keyboard to disable the relay
	//DAQmxErrChk (DAQmxWriteDigitalLines(DOtaskHandle,1,0,10.0,DAQmx_Val_GroupByChannel,SWLOW,NULL,NULL));

Error:
	if( DAQmxFailed(error) )
		DAQmxGetExtendedErrorInfo(errBuff,2048);
	if( AOtaskHandle ) {
		/*********************************************/
		// DAQmx Stop Code
		/*********************************************/
		DAQmxStopTask(AOtaskHandle);
		DAQmxClearTask(AOtaskHandle);
		AOtaskHandle = 0;
	}
	
	//if( DOtaskHandle ) {
	//	/*********************************************/
	//	// DAQmx Stop Code
		/*********************************************/
	//	DAQmxStopTask(DOtaskHandle);
	//	DAQmxClearTask(DOtaskHandle);
	//	DOtaskHandle = 0;
	//}
	if( DAQmxFailed(error) )
		printf("DAQmx Error: %s\n",errBuff);
	printf("End of program.\n");
	return 0;
}


//Function to compute waveform vs sample index
float64 waveform(unsigned int x,float64 Gain)
{  
	float64 amp=0.0; //initiation
	float64 ampx=0.0;

    unsigned int N1=(int)(SampleRate/freq*Nup);//check index at section joint
	unsigned int N2=(int)(SampleRate/freq*(Nup+Nst));
	unsigned int N3=(int)(SampleRate/freq*(Nup+Nst+Ndn));

	if ((x>0)&&(x<=N1))//ramp up
	    {   
			ampx=((float64)x)/((float64)N1);
		    amp=Gain*ampx*sin(2*PI*freq*((float64)x)/SampleRate);
	    }
	else if ((x>N1)&&(x<=N2))//stay at max
       amp=Gain*sin(2*PI*freq*((float64)x)/SampleRate);
	else if ((x>N2)&&(x<=N3))//ramp down-degauss
	    {   
			ampx=1.0-((float64)(x-N2))/((float64)(N3-N2));
			amp=Gain*ampx*sin(2*PI*freq*((float64)x)/SampleRate);
	    }
	else 
	   {
		  //printf("sample index outranging!\n"); 
		  amp=0.0;
	   }

	return amp; 
}