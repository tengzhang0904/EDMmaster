//Function call to DDS

/****************************************************************************************************
*DDS v3 revisions: *  Last modified 2011/12/09 by KZ
* A truly C++ based boost on code readability and manipulability, allowing more flexibility and 
  capability on control of single pulse and pulse trains, with better exception and error handling
* see definitions of objects in file  "Pulse.h", "PulseTrain.h", "Composer.h", 'DDSDO.h", "Timer.h"
* use dynamic memory allocation to manage extra large data arrays
* use dynamic linked list to automatically expand data array size when outranging
* built in options for computation, file reading, digital output and combinations

* DDS v2.0 revisions:  (Last Modified by KZ 2010-06-08)
* (1)Increase data transfer rate by 3X using external Strobe.
* (2)Progeame in self-timing capabilities, receives start triggers from Supertime.
* (3)Compute FIFO data, combine pulses and output data all at once. Fast enough, data file unecessary.
* (4)Ease of USE: Construct a series of pulses from parameters in EXCEL; 
*                 Add/remove/change pulse parameters can be done in Excel without recompile.
* (5)Multitype frequency-chirp/amplitude-modulation: ARP_const torque, Blackman PI pulse, etc.
*
* Header file used for multithreading...
****************************************************************************************************/
#ifndef _DDS_MT_h_included_
#define _DDS_MT_h_included_
#define _CRT_SECURE_NO_DEPRECATE

#include <stdio.h>
#include <math.h>
#include <iostream>
#include <iomanip>
#include <string>
#include <windows.h> 
#include <cstdlib>
#include "DDSfunc.h" //Basic DDS conversion, arbitrary waveform control functions
#include "DAQmx/NIDAQmx.h" //Hardware control library for DAQmx
#include "Timer/Timer.h" //precision timer between two break points
#include "Pulse.h"  //define data and methods for one pulse
#include "PulseTrain.h" //define data and methods for a pulse train
#include "Composer.h" //compute data to FIFO with a pulse train
#include "DDSDO.h" //DAQmx output settings and functions
using namespace std; 

/***************************************************************/
// Frequently used or global definitions
/***************************************************************/
const extern double  FIFOCLK=2000000.0;  //PCI6534 FIFO UPDATE CLK [4.656613e-3, 20e6]Hz
const extern double  GPSCLK=10000000.0;  //10MHz GPS CLK
const extern float Timeout=-1;  //Max time waiting for trigger, in seconds

const int SAMPLE_NUMBER=200000; //initial sample number estimate (1ms pulse ~ 2000 samples)
const extern int INCRT_SIZE=20000; //step size for an expanion of array length when outranging 

const extern string MMCfolder;//MMC_SYSINT folder;

/***************************************************************/
// DDS pulse sequences for general experiments
/***************************************************************/
void DDS(unsigned int Repeat, unsigned long opt)
{   
	const string ddsfolder=MMCfolder+"DDS/";
    const string PulseParameterExcelFilename=ddsfolder+"PulseTrainParameters.xls";
    const string logfile=ddsfolder+"datalog.txt"; //default logfile name

	int Option=(int)opt; 
	string Options[4]={" ", "Option 1: compute data and write to file", 
		                    "Option 2: read calculated pulses from file  and output to DDS",
							"Option 3: compute data and output DDS"};

	PulseTrain *PulseSeries=new PulseTrain(SAMPLE_NUMBER, 0); //define and initialize a pulsetrain
	unsigned int  DataSize=0;  //Number of data sent to FIFO
	bool flag=false; //data file reading flag

	switch(Option)
	{
	  case 1: 
		  cout<<Options[1]<<endl<<logfile<<endl<<endl;
	      PulseSeries->Compute(PulseParameterExcelFilename);//compute pulsetrain data array
          PulseSeries->WriteDataFile(logfile); //write data to file (truncate file first)
		  goto EndOfProgram; //end program and skip outputing data
		  break;

      case 2: 
		  cout<<Options[2]<<endl<<logfile<<endl<<endl; 
		  flag=PulseSeries->ReadDataFile(logfile); //read data from existing .txt file
		  if (flag!=true) 
		  {  
			  cout<<"Error reading file!"<<endl;
			  goto EndOfProgram; //end program and skip outputing data
		  }
		  break;

	  case 3: 
		  cout<<Options[3]<<endl<<endl;
	      PulseSeries->Compute(PulseParameterExcelFilename);//compute pulsetrain data array
		  break;

	  default:
		  cout<<"Options or command not recognized, use the format:"<<"DDS OptionNumber"<<endl
		      <<Options[1]<<endl<<Options[2]<<endl<<Options[3]<<endl;
		  goto EndOfProgram; //end program and skip outputing data
	}

	DataSize=unsigned int(PulseSeries->length()); //total number of samples
	OutputDataRepeat(Repeat, PulseSeries->getArray(), DataSize); //output data to DDS

  EndOfProgram: 
	delete PulseSeries; //release PC-memory for PulseSeries
}


/***************************************************************/
// DDS pulse sequences for EDM experiments ONLY

// this is nearly identical to the general DDS() function 
// except the files that store a different sequence
// for simplicity without too much modifying existing programs 
/***************************************************************/
void DDS_EDM(unsigned int Repeat, unsigned long opt)
{   
	const string ddsfolder=MMCfolder+"DDS/";
    const string PulseParameterExcelFilename=ddsfolder+"EDM_PulseTrainParameters.xls";
    const string logfile=ddsfolder+"EDM_datalog.txt"; //default logfile name for EDM sequence

	int Option=(int)opt; 
	string Options[4]={" ", "Option 1: compute data and write to file", 
		                    "Option 2: read calculated pulses from file  and output to DDS",
							"Option 3: compute data and output DDS"};

	PulseTrain *PulseSeries=new PulseTrain(SAMPLE_NUMBER, 0); //define and initialize a pulsetrain
	unsigned int  DataSize=0;  //Number of data sent to FIFO
	bool flag=false; //data file reading flag

	switch(Option)
	{
	  case 1: 
		  cout<<Options[1]<<endl<<logfile<<endl<<endl;
	      PulseSeries->Compute(PulseParameterExcelFilename);//compute pulsetrain data array
          PulseSeries->WriteDataFile(logfile); //write data to file (truncate file first)
		  goto EndOfProgram; //end program and skip outputing data
		  break;

      case 2: 
		  cout<<Options[2]<<endl<<logfile<<endl<<endl; 
		  flag=PulseSeries->ReadDataFile(logfile); //read data from existing .txt file
		  if (flag!=true) 
		  {  
			  cout<<"Error reading file!"<<endl;
			  goto EndOfProgram; //end program and skip outputing data
		  }
		  break;

	  case 3: 
		  cout<<Options[3]<<endl<<endl;
	      PulseSeries->Compute(PulseParameterExcelFilename);//compute pulsetrain data array
		  break;

	  default:
		  cout<<"Options or command not recognized, use the format:"<<"DDS OptionNumber"<<endl
		      <<Options[1]<<endl<<Options[2]<<endl<<Options[3]<<endl;
		  goto EndOfProgram; //end program and skip outputing data
	}

	DataSize=unsigned int(PulseSeries->length()); //total number of samples
	OutputDataRepeat(Repeat, PulseSeries->getArray(), DataSize); //output data to DDS

 EndOfProgram: 
	delete PulseSeries; //release PC-memory for PulseSeries
}

#endif

/*
	PrecisionTimer ReadPulse; //Initialize a precision timer
	ReadPulse.getFreuqnency(); //Get CPU frequency
	ReadPulse.Start(); //Start timer
    //Test code
	ReadPulse.Stop();  // Stop timer
	ReadPulse.Display();
*/