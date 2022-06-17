/***************************************************************
*  This class define data and methods for photodiode arrays with PCI 6701E
*  Last modified 2012/05/29 by KZ
****************************************************************/

#ifndef _PDA_h_included_
#define _PDA_h_included_
#define _CRT_SECURE_NO_DEPRECATE

//windows and NI related headers and libraries
#include <iostream>
#include <iomanip>
#include <string>
#include <windows.h> 
#include <cstdlib>
#include <stdio.h>                    
#include <stdlib.h>                  
#include <fstream> 
#include <math.h>
#include "DAQmx/NIDAQmx.h"
#pragma comment(lib, "NIDAQmx.lib")
using namespace std;

/***************************************************************/
// Constants
/***************************************************************/
#include "CONST.h"
using namespace DATACONST;

//The sampling rate in samples per second per channel: PCI 6071E 1.25MS/s
// 50 Channels with rate 20kHz means it takes 1 microsec to read 1 pixel
#define PDA_Rate 20000   

class PDA{

  public:
	         float64 Data[PDANpixel][PDANshot];//data array to acquire
			 PDA(); //Default constructor; Initiate Data Array
			 bool WriteDataFile(const string filename); //write Data to file
			 void Acquire(unsigned short timeout); //Acquire data 
			 void GenerateTestData();//Generate fake data
			 void ReadEDMTestData(unsigned int shotID); //Read sample EDM data (LF Transition simulations) from TXT files
			 void PNswap(); //separate even and odd pixels w/ PDA_ChlID_ALTER
};

PDA::PDA() //Default constructor; Initiate Data Array
{   
	for(int j=0;j<PDANpixel;j++)   
		{
			for(int k=0;k<PDANshot;k++)
			    {
				     Data[j][k]=0;   // Initiate Data Array
			    }
	     }
}


bool PDA::WriteDataFile(const string filename)//write Data to file
{
	ofstream datalog( filename.c_str(),ios::out | ios::trunc); //delete previous log then write the new one
	datalog.precision(6); //set precision to 6 digits
	datalog.setf(ios::fixed,ios::floatfield);   // floatfield set to fixed
	for(int j=0;j<PDANpixel;j++)   
		{
			for(int k=0;k<PDANshot;k++)
			    {
				     datalog<<setw(9)<<left<<Data[j][k]<<' '; //formated output 
			    }
			datalog<<endl; //change line
	     }
   cout<<"PDA:: data written to file"<<endl<<filename<<endl;
   return true;
}

void PDA::PNswap()//separate even and odd pixels w/ PDA_ChlID_ALTER
{
	float64 swapData[PDANpixel][PDANshot];
	for(int k=0;k<PDANshot;k++)  
	{
	   for(int n=0;n<PDANpixel/2;n++) 
	   {
		   swapData[n][k]=Data[2*n][k];
		   swapData[n+25][k]=Data[2*n+1][k];
	   } 
	} 

	for(int k=0;k<PDANshot;k++)  
	{
	   for(int j=0;j<PDANpixel;j++) 
	   {
		   Data[j][k]=swapData[j][k]; 
	   } 
	} 

}


void PDA::Acquire(unsigned short TimeOut) //Acquire PDA data
{
	#define DAQmxErrChk(functionCall) if( DAQmxFailed(error=(functionCall)) ) goto Error; else

	int32       error=0;
	TaskHandle  taskHandle=0;
	int32       NumPerChn;
	char        errBuff[2048]={'\0'};
    float64     InterleavedData[PDANpixel*PDANshot];//data array to acquire
	string      ExtCLKTrigger="/Dev3/PFI0"; //External update clock trigger
	float64     Limits[2]={-5.0,0.5};//Input range in volts: direct output from PDA AMP box is on negative side

/*
	//channelID for all 64 channels
    string      All_ChlID="Dev3/ai0:63"; 
	//channelID for one channel
	string      One_ChlID="Dev3/ai33"; 
*/
	//channelID for Integrated AI (7+7+7+4)+(7+7+7+4) channels, blank out unused channels
	string      PDA_ChlID="Dev3/ai0:6,Dev3/ai8:14,Dev3/ai16:22,Dev3/ai24:27,Dev3/ai32:38,Dev3/ai40:46,Dev3/ai48:54,Dev3/ai56:59"; 
	//channelID for Real-time AI (7+7+7+4)+(7+7+7+4) channels, blank out unused channels
	string      PDART_ChlID="Dev3/ai54:48,Dev3/ai62:56,Dev3/ai38:32,Dev3/ai46:43,Dev3/ai22:16,Dev3/ai30:24,Dev3/ai6:0,Dev3/ai14:11";
    //channelID for Integrated AI (7+7+7+4)+(7+7+7+4) channels, alternate between +Z and -Z sides
	string      PDA_ChlID_ALTER="Dev3/ai0, Dev3/ai32, Dev3/ai1, Dev3/ai34, Dev3/ai2, Dev3/ai36, Dev3/ai3, Dev3/ai37, Dev3/ai4, Dev3/ai38, Dev3/ai5, Dev3/ai40, Dev3/ai6, Dev3/ai41, "
		                        "Dev3/ai8, Dev3/ai42, Dev3/ai9, Dev3/ai43, Dev3/ai10, Dev3/ai52, Dev3/ai12, Dev3/ai45, Dev3/ai13, Dev3/ai46, Dev3/ai14, Dev3/ai48, Dev3/ai17, Dev3/ai49, "
								"Dev3/ai18, Dev3/ai50, Dev3/ai19, Dev3/ai51, Dev3/ai20, Dev3/ai53, Dev3/ai21, Dev3/ai54, Dev3/ai22, Dev3/ai57, Dev3/ai24, Dev3/ai58, Dev3/ai25, Dev3/ai59, "
								"Dev3/ai26, Dev3/ai61, Dev3/ai27, Dev3/ai62, Dev3/ai28, Dev3/ai30, Dev3/ai29, Dev3/ai56";

	string      ChannelID=PDA_ChlID_ALTER;//Channels in use

	/***************************************************************************************
	 *DAQmx Configure Code: specify channel number,input range, and triggers
	 *External update clock trigger: 64 samples per rising edge (one sample per channel)
	 *Set PDA_Rate which controls multiplexing from chl0 to chl63 and ADC conversion speed
	              to proper speed, avoid unnecessarily rapid speed
	 *Retriggerable up to PDANshot times
	****************************************************************************************/
	DAQmxErrChk (DAQmxCreateTask("",&taskHandle));
	DAQmxErrChk (DAQmxCreateAIVoltageChan(taskHandle,ChannelID.c_str(),"",DAQmx_Val_RSE,Limits[0],Limits[1],DAQmx_Val_Volts,NULL)); 
	DAQmxErrChk (DAQmxCfgSampClkTiming(taskHandle,ExtCLKTrigger.c_str(),PDA_Rate,DAQmx_Val_Rising,DAQmx_Val_FiniteSamps,PDANshot)); //External CLK

	DAQmxErrChk (DAQmxTaskControl(taskHandle,DAQmx_Val_Task_Commit));
	DAQmxErrChk (DAQmxStartTask(taskHandle));
    

	//DAQmx DAQ process:  use interleaving (GroupByScanNumber) such that 
	//                     the array lists the first sample from every channel in the task, 
	//                     then the second sample from every channel, 
	//                     up to the last sample from every channel.
    DAQmxErrChk (DAQmxReadAnalogF64(taskHandle,PDANshot,TimeOut,DAQmx_Val_GroupByScanNumber,InterleavedData,PDANpixel*PDANshot,&NumPerChn,NULL));
	cout<<"PDA::"<<PDANpixel<<"x"<<NumPerChn<<" samples taken"<<endl;
	for(int k=0;k<PDANshot;k++)  {
	   for(int j=0;j<PDANpixel;j++) {
		   Data[j][k]=(-1.0)*InterleavedData[j+k*PDANpixel]; //add inverting sign here to make signal always looks positive
	   } 
	} 
	if (ChannelID.compare(PDA_ChlID_ALTER)==0)
	{
	     this->PNswap();//separate even and odd pixels w/ PDA_ChlID_ALTER 
	}

	//avoid this channel 
	//switching +Z/-Z PDA cables confirms PDs are OK, but not electronics for this channel (Chnl 5 on card B2)
  Error: 
	if( DAQmxFailed(error) )
		DAQmxGetExtendedErrorInfo(errBuff,2048);
	if( taskHandle!=0 ) {
		DAQmxStopTask(taskHandle);
		DAQmxClearTask(taskHandle);
	}
	if( DAQmxFailed(error) )
		printf("DAQmx Error: %s\n",errBuff);
}

//generate fake data for testing
void PDA::GenerateTestData()
{
  for (int i=0;i<(PDANpixel/2);i++)
  {
		Data[i][0]=4.0*exp(-pow((i-(PDANpixel/4.0))/6.0,2));
		Data[i+25][0]=Data[i][0];
  }
}

//Read sample EDM data (LF Transition simulations) from EDMPZ.txt and EDMNZ.txt files
void PDA::ReadEDMTestData(unsigned int shotID) 
{

 //read simulated data
	double EDMPZ[64][PDANshot], EDMNZ[64][PDANshot];
	const string PZstr="C:/MMC_SYSINT/Example DATA/EDM_SCAN Example/EDMPZ.txt";
	const string NZstr="C:/MMC_SYSINT/Example DATA/EDM_SCAN Example/EDMNZ.txt";
	//+EDM data
	bool successflag;
	ifstream datalog;
    datalog.open (PZstr.c_str(), ios::in); 
	successflag=!(datalog.fail());
    if (successflag)
    {
	   for(int j=0;j<64;j++)   
		 {
			   for(int k=0;k<PDANshot;k++)
			    {
				     datalog>>EDMPZ[j][k];
			    }
	     }
	}
	datalog.close();
	//-EDM data
	datalog.open (NZstr.c_str(), ios::in); 
	successflag=!(datalog.fail());
    if (successflag)
    {
	   for(int j=0;j<64;j++)   
		 {
			   for(int k=0;k<PDANshot;k++)
			    {
				     datalog>>EDMNZ[j][k];
			    }
	     }
	}
	datalog.close();

 //generate EDM test data for a specific SHOT
	if ((shotID>=0)&&(shotID<=63))
	{
		for(int k=0;k<PDANshot;k++)  
		 {
			 for(int j=0;j<PDANpixel;j++)
			 {
				   if (j<25) //+Z side
				   {
				       Data[j][k]=exp(-pow((double(j)-12.5)/6.0,2.0))*EDMPZ[shotID][k];
				   }
				   else //-Z side
				   {
                       Data[j][k]=exp(-pow((double(j)-12.5-25.0)/6.0,2.0))*EDMNZ[shotID][k];
				   }
				   Data[j][k]+=0.05*double(rand() % 100 -50)/100.0; //add noise
				   Data[j][k]*=0.15; //change amplitude
			 }    
	     }
	}

	/*
	string msg="EDM data: "+to_string(Data[0][0]);
	AfxMessageBox(msg.c_str());
	*/
}

#endif

/* 
Example: set different input ranges for different channles (more accurate measurement without degradation of performance)

DAQmxCreateAIVoltageChan(taskHandle, "Dev3/ai0:7", "", DAQmx_Val_RSE,  -5.0,  5.0, DAQmx_Val_Volts, NULL);
DAQmxCreateAIVoltageChan(taskHandle, "Dev3/ai8:15", "", DAQmx_Val_RSE,  0.0,  1.0, DAQmx_Val_Volts, NULL);

*/


