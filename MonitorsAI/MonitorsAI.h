/*********************************************************************************************
*  This header gives:
       *** definition and methods for analog monitors input
*  Last modified 2012/03/07 by KZ
**********************************************************************************************/

#ifndef _MonitorsAI_h_included_
#define _MonitorsAI_h_included_
#define _CRT_SECURE_NO_DEPRECATE

#include <stdio.h>
#include <math.h>
#include <iostream>
#include <iomanip>
#include <string>
#include <windows.h> 
#include <fstream> 
using namespace std;

#include "DAQmx/NIDAQmx.h" //NI device lib
#pragma comment(lib, "NIDAQmx.lib")

#include "Excel_Link/libxl.h" //excel linking lib
#pragma comment(lib, "libxl.lib")
using namespace libxl;

string AIChls="DevAI/ai0:15"; //specify AI channel(s) for monitors input
string AIpat="DevAI/ai30:31"; //use last 2 channels as pattern match channel

#include "CONST.h"
using namespace DATACONST;

class Amonitors{

   public:
	   float64 data[AMNumShots][AMNumChls]; //data array
	   void init(); //initialize data array
	   bool Measure(unsigned int shotid); //measurement from AI device
	   bool WriteXLS(string fielname); //write data to an excel file
};

void Amonitors::init() //initialize data array
{   
	//set default chl values
	   for(unsigned int j=0; j<AMNumChls;j++)
	    {
		   for (unsigned int i=0; i<AMNumShots;i++)
		{
			data[i][j]=0.0;
		}
	}
}

bool Amonitors::Measure(unsigned int shotid) //measurement from AI device
{   
  if(shotid>=AMNumShots) 
	{
		printf("AImonitors:: shotid %d out ranging\n",shotid);
		return false;
    }

  else{

	#define DAQmxErrChk(functionCall) if( DAQmxFailed(error=(functionCall)) ) goto Error; else
	int32       error=0;
	TaskHandle  taskHandle=0;
	int32       read;
	char        errBuff[2048]={'\0'};

	DAQmxErrChk (DAQmxCreateTask("",&taskHandle));
	DAQmxErrChk (DAQmxCreateAIVoltageChan(taskHandle,AIChls.c_str(),"",DAQmx_Val_Cfg_Default,-5.0,5.0,DAQmx_Val_Volts,NULL));
	DAQmxErrChk (DAQmxCfgSampClkTiming(taskHandle,"",1000.0,DAQmx_Val_Rising,DAQmx_Val_FiniteSamps,AMNumChls));
	DAQmxErrChk (DAQmxCfgDigEdgeStartTrig(taskHandle,"/DevAI/PFI0",DAQmx_Val_Rising));

	DAQmxErrChk (DAQmxStartTask(taskHandle));
	DAQmxErrChk (DAQmxReadAnalogF64(taskHandle,1,10.0,DAQmx_Val_GroupByChannel,this->data[shotid],AMNumChls,&read,NULL));
	printf("AImonitors:: shotid %d : acquired %d points\n",shotid,read*AMNumChls); 

  Error:
	if( DAQmxFailed(error) )
		DAQmxGetExtendedErrorInfo(errBuff,2048);
	if( taskHandle!=0 )  {
		DAQmxStopTask(taskHandle);
		DAQmxClearTask(taskHandle);
	}
	if( DAQmxFailed(error) )
		printf("DAQmx Error: %s\n",errBuff);

	return true;
  }
}

bool Amonitors::WriteXLS(string filename) //write data to an excel file
{   
   Book* book = xlCreateBook();
   Sheet* sheet = book->addSheet("Monitors");
   for(unsigned int j=0; j<AMNumChls;j++)
	 {
		 for (unsigned int i=0; i<AMNumShots;i++)
		    {   
				sheet->writeNum(j+2, i+1, this->data[i][j]);
		    }
		 sheet->writeNum(j+2, 0, j); //write channel id
	 }
   sheet->writeStr(1, 0, "Channel");
   book->save(filename.c_str());
   book->release(); 
   cout<<"MonitorsAI::data written to excel file:"<<endl;
   cout<<filename<<endl;
   return true;
}


//read last two channels (software trigger mode) for programatic pattern match 
void CheckPattern(float64 pat[]) 
{   
	#define DAQmxErrChk(functionCall) if( DAQmxFailed(error=(functionCall)) ) goto Error; else
	int32       error=0;
	TaskHandle  taskHandle=0;
	int32       read;
	char        errBuff[2048]={'\0'};

	DAQmxErrChk (DAQmxCreateTask("",&taskHandle));
	DAQmxErrChk (DAQmxCreateAIVoltageChan(taskHandle,AIpat.c_str(),"",DAQmx_Val_Cfg_Default,-10.0,10.0,DAQmx_Val_Volts,NULL));
	DAQmxErrChk (DAQmxCfgSampClkTiming(taskHandle,"",500.0,DAQmx_Val_Rising,DAQmx_Val_FiniteSamps,2));
	DAQmxErrChk (DAQmxStartTask(taskHandle));
	DAQmxErrChk (DAQmxReadAnalogF64(taskHandle,1,10.0,DAQmx_Val_GroupByChannel,pat,2,&read,NULL));

  Error:
	if( DAQmxFailed(error) )
		DAQmxGetExtendedErrorInfo(errBuff,2048);
	if( taskHandle!=0 )  {
		DAQmxStopTask(taskHandle);
		DAQmxClearTask(taskHandle);
	}
	if( DAQmxFailed(error) )
		printf("DAQmx Error: %s\n",errBuff);
}

#endif

