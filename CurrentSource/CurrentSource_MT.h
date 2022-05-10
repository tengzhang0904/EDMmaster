/* 
	CurrentSourceLibrary.cpp is a collection of functions for use in controlling up to 16 
	independent current sources.  All current sources listen on the same enable (cs) and 
	clock (SCLK)lines, but each has it own data (DIN) line which will be determined later.  
	After outputing a signal with no errors the run number, decimal number sent to the 
	current source, and the approximate output current are recorded in RecordOutput.txt.
	If an Error occurs the run number and error code are recorded in RecordOutput.txt and 
	as long as the error didn't occur in writing to the RecordOutput file the current 
	source will not update.  By the nature of the design all channels must be updated 
	simultaneously so you cannot only update a single channel.  See the ErrorCodes.txt 
	file for a list of error codes. 

	Changes in 006
	* RecordOutput now also records the channel number
	* GetSlope and GetIntercept now return different numbers based on the channel
	* The numbers are stored in files Slopes.txt and Intercepts.txt and can be edited there

	Changes in 008
	* The output is not controlled by the onboard timer it is controlled by software
	* needs a wait function which is accurate to ~0.125 ms right now Sleep is accurate to 
	  1 ms

	Changes in 009
	* Parallel data lines/common cs line (major revision)

	Changes in 010
	* Main function just takes 8 currents as parameters
	* All Text Files have been moved to a single .xls file for ease of use
	* Note this does not work because the excel stuff is a class and needs 
	  to be converted to cpp

	Changes in 012
	* Convert to c++ to use .xlsx library
	* Removed RecordOut file error since it was just going to record in the file it cant open.
	  Instead if this error occurs the main program returns -1

	Changes in 013
	* Added support for Bipolar channels

	Changes in 014
	* Added calibration matrix to determine the approriate currents only works for nonsignular matrices

	Changes in 015
	* Added Error correction, where there are 16 linear fits for the 4 MSBs.  The data is stored as the output 
	  currents at the start and end of a MSB (i.e. output currents for 0 and 4095, or 4096 and 8181).  The closet 
	  current is found by finding which endpoints the deisred output is between and then a linear approximation is
	  used.  If the desired output is between consecutive endpoints (i.e. 4095 and 4096) the closest endpoint is 
	  chosen.
	* Removed Bipolar stuff since that will be handled by the transfer function

	Changes in 016
	* Added a function which just tells the DAC to update to decimal values
	* Also now if there is a channel specific error the channel number is also recorded in the output file

	Changes in 017
	* Records the Date and time in RecordOutput.txt

	Channges on 06/11/12 Cheng Tang
	* Write data according to the sequence in the ribbon cable

	Changes on 7/16/2012 by KZ
	* Turn this into a recallable header file for Master Mind porgram

//=================================================================================================================
*/

#ifndef _CurrentSource_MT_h_included_
#define _AudioPulse_MT_h_included_
#define _CRT_SECURE_NO_DEPRECATE

#include <stdio.h>
#include "CurrentSource/CurrentSourceLibrary.h"
#include <math.h>
#include <stdlib.h>
#include <windows.h> 
#include <time.h>

#include "DAQmx/NIDAQmx.h" //NI device lib
#pragma comment(lib, "NIDAQmx.lib")

#include "Excel_Link/libxl.h" //excel linking lib
#pragma comment(lib, "libxl.lib")
using namespace libxl;

//==========================================================================================
const extern string MMCfolder; //MMC_SYSINT folder address
const string SETTINGS_FILE=MMCfolder+"CurrentSource/Settings.xls";// name of the excel file with the settings

#define  SAMPLE_NUM 40  // number of 32 bit words sent to the current source
#define  CS_CHANNEL 8  // channel of the enable pin (cs), the 2 is temporary until the new DAQ comes
#define  SCLK_CHANNEL 9 // channel of the clock pin (sclk), the 1 is temporary until the new DAQ comes
#define  CHANNEL_ROW 1 // index where the channel definitions begin in the excel file
#define  CHANNEL_COL 1
#define  TRANSFER_ROW 12 // index where the transfer function variables (i.e. endpoints) begin in the excel file
#define  TRANSFER_COL 2
#define  SEPARATION 6  // the number of columns between the transfer function variables for each channel
#define  CALIBRATION_MATRIX_ROW 1 // index where the calibration matrix begins in the excel file
#define  CALIBRATION_MATRIX_COL 6

#define  INPUT_OUT_OF_RANGE 1  // the DAC can only handle 16-bits (0-65535)
#define  FAILED_COMMUNICATION_WITH_DAQ 2  // The computer could not communicate with the DAQ
#define  CHANNEL_NUM_RANGE 3  // There are currently only 16 channels available for data and (0-15)
#define  SETTINGS_FILE_OPEN 4  // Theres a problem opening or reading the settings file
#define  CALIBRATION_MATRIX_SINGULAR 5 // the Calibration Matrix is singular
#define  TRANSFER_FUNCTION_FAILURE 6 // the TransferFunction function failed, most likely input is out of range

#define  DAQmxErrChk(functionCall) if( DAQmxFailed(error=(functionCall)) ) goto Error; else // from NI examples

//==========================================================================================
//==========================================================================================
// forward delclarations
int TransferFunction (double CurrentMag, int channelNum);
int ChannelNumOK (int channelNum);
int round (double a);
int ReadFromSettings(struct CurrentData *sCurrentData);
int GetNextRunNumber(FILE *pFile, char delim);
int RecordError (int ErrorNum, int channelNum = -1);
int RecordOutput (struct CurrentData *sCurrentData);
int SendStack(struct CurrentData *sCurrentData);
int CreateDataStack (struct CurrentData *sCurrentData);
int ReadCalibrationMatrix (struct BFieldData *sBFieldData);
int CalculateCurrents(struct BFieldData *sBFieldData);
int TransferFunction(int channelNum, double current);
int TransferFunctionBipolar(int channelNum, double current);
int TransferFunctionUnipolar(int channelNum, double current);
int LinearApprox(double max, double min, double current);

//==========================================================================================
//==========================================================================================
// holds the arrays containing relevant information and is passed between functions
struct CurrentData
{
	double Currents[8]; // Current requried to make BFields
	int decInputs[8];  // decimal inputs which will be sent to the DAC
	uInt16 data[SAMPLE_NUM]; // stack sent to the DAC
	int Channels[8]; // the channels where specific coils are connected
};

// holds arrays containing B-Field information
struct BFieldData
{
	double BFieldMag[8]; // size of the B-Fields that need to be produced
	double CalibrationMatrix[8][8]; // holds the calibration matrix for the magnetic field coils
	double Currents[8]; // currents calculated from the B-Field vector and the Calibration Matrix
};

//==========================================================================================
//==========================================================================================
/* Calculates the approriate currents to make the B-feilds from the calibration matrix in the
   Settings.xls file.  Then calls the UpdateAllCurrents function with the calculated currents.
   Returns 1 if there is an error that causes the DAC to not update. Returns -1 if the DAC did 
   update but there is still an error (i.e. the outputs were not recorded in RecordOutput.txt) */
int UpdateAllBFields (  double Bx, 
						double By, 
						double Bz, 
						double Bxx, 
						double Byy, 
						double Bzz, 
						double Bxy,
						double Bxz)
{
	struct BFieldData sBFieldData;

	// store the BFields in the struct
	sBFieldData.BFieldMag[0]=Bx;
	sBFieldData.BFieldMag[1]=By;
	sBFieldData.BFieldMag[2]=Bz;
	sBFieldData.BFieldMag[3]=Bxx;
	sBFieldData.BFieldMag[4]=Byy;
	sBFieldData.BFieldMag[5]=Bzz;
	sBFieldData.BFieldMag[6]=Bxy;
	sBFieldData.BFieldMag[7]=Bxz;

	// read the calibration matrix into the struct
	if (ReadCalibrationMatrix(&sBFieldData))
	{
		RecordError(SETTINGS_FILE_OPEN); // cant read settings file
		return 1;
	}

	// least squares fit to the calibration matrix A and the BfieldMag vector b
	// b=AI, where I is the current vector
	if (CalculateCurrents(&sBFieldData))
	{
		RecordError(CALIBRATION_MATRIX_SINGULAR); // the calibration matrix is singular
		return 1;
	}

	// now send the currents to the DAQ, and forward any error messages
	return UpdateAllCurrents(   sBFieldData.Currents[0],
								sBFieldData.Currents[1],
								sBFieldData.Currents[2],
								sBFieldData.Currents[3],
								sBFieldData.Currents[4],
								sBFieldData.Currents[5],
								sBFieldData.Currents[6],
								sBFieldData.Currents[7]);
}

//==========================================================================================
/* Updates all currents in parallel.  Determines the appropriate current required 
   to make the magnitude B fields from the transfer function.  The channels corresponding 
   to the coils listed are stored in a .txt file. Returns 1 if there is an error that causes 
   the DAC to not update. Returns -1 if the DAC did update but there is still an error (i.e. 
   the outputs were not recorded in RecordOutput.txt) */
int UpdateAllCurrents ( double Ix, 
					    double Iy, 
					    double Iz, 
						double Ixx, 
						double Iyy, 
						double Izz, 
						double Ixy,
						double Ixz)
{
	int i=0;
	// holds all arrays so just pass this to a function that needs it
	struct CurrentData sCurrentData; 

	// transfer the inputs to the struct
	sCurrentData.Currents[0]=Ix;
	sCurrentData.Currents[1]=Iy;
	sCurrentData.Currents[2]=Iz;
	sCurrentData.Currents[3]=Ixx;
	sCurrentData.Currents[4]=Iyy;
	sCurrentData.Currents[5]=Izz;
	sCurrentData.Currents[6]=Ixy;
	sCurrentData.Currents[7]=Ixz;

	// Read settings into the struct from .xlsx file
	if(ReadFromSettings(&sCurrentData)==1)
	{
		RecordError(SETTINGS_FILE_OPEN);  // cant open/read Settings.txt
		return 1;
	}

	// initialize data stack array to 0
	for (i=0;i<SAMPLE_NUM;i++)
		sCurrentData.data[i]=0;

	// calculate appropriate decimal values to send
	for (i=0;i<8;i++)
	{		
		// return the appropriate decimal integer value to be sent to the DAC for each channel
		sCurrentData.decInputs[i] = TransferFunctionBipolar(sCurrentData.Channels[i],sCurrentData.Currents[i]);

		// check for errors
		if (sCurrentData.decInputs[i] == -1)
		{
			RecordError (TRANSFER_FUNCTION_FAILURE);
			return 1;
		}
		if (sCurrentData.decInputs[i] == -2)
		{
			RecordError (INPUT_OUT_OF_RANGE,sCurrentData.Channels[i]);
			return 1;
		}
		//printf("%d\n",sCurrentData.decInputs[i]);
	}

	// calculate data stack
	if(CreateDataStack(&sCurrentData))
		return 1; // error

	// send stack to DAC
	if(SendStack(&sCurrentData))
		return 1; // error

	// record output in file
	if(RecordOutput(&sCurrentData))
		return -1;  // error cant really record this one in output file so it returns -1
	
	return 0; // no error
}

//==========================================================================================
/* Update all the DACs with the decimal inputs listed here.  Most likely used for 
   calibration purposes.  The channels corresponding to the coils listed are stored 
   in Settings.xls file. Returns 1 if there is an error where the DAC does not update
   and -1 if there is an error where the DAC updates. */
int UpdateChannels (int Nx,
					int Ny,
					int Nz,
					int Nxx,
					int Nyy,
					int Nzz,
					int Nxy,
					int Nxz)
{
	struct CurrentData sDecimalData; 

	// transfer the inputs to the struct
	sDecimalData.decInputs[0]=Nx;
	sDecimalData.decInputs[1]=Ny;
	sDecimalData.decInputs[2]=Nz;
	sDecimalData.decInputs[3]=Nxx;
	sDecimalData.decInputs[4]=Nyy;
	sDecimalData.decInputs[5]=Nzz;
	sDecimalData.decInputs[6]=Nxy;
	sDecimalData.decInputs[7]=Nxz;

	// Read settings into the struct from .xlsx file
	if(ReadFromSettings(&sDecimalData)==1)
	{
		RecordError(SETTINGS_FILE_OPEN);  // cant open/read Settings.txt
		return 1;
	}

	// initialize data stack array to 0
	for (int i=0;i<SAMPLE_NUM;i++)
		sDecimalData.data[i]=0;

		// calculate data stack
	if(CreateDataStack(&sDecimalData))
		return 1; // error

	// send stack to DAC
	if(SendStack(&sDecimalData))
		return 1; // error

	// do a simple estimation of the output current
	for (int i=0;i<8;i++)
		sDecimalData.Currents[i]=(0.312612536E-9)*sDecimalData.decInputs[i] + (1.33595179E-9);

	// record output in file
	if(RecordOutput(&sDecimalData))
		return -1;  // error cant really record this one in output file so it returns -1
	
	return 0; // no error
}

//==========================================================================================
// Data Prep and Sending Functions
//==========================================================================================

/* creates the data stack which will be sent to the DAQ.  The timing for an individual chip
   is that the enable pin goes high (low on DAC due to a inverting switch) the data is sent 
   in serial MSB first, then the enable pin goes low (high) and then all the outputs are set 
   to zero.  data is read on the rising edge of the clock. */
int CreateDataStack (struct CurrentData *sCurrentData)
{
	int binData[16]; // holds the binary representation of the decInput, MSB first
	int temp[16]; // temporary array used for reversing the order of binary rep
	int trash,remain; // trash is used so that decInput isnt changed, remain is 1 or 0
	int i,j,k;
	int ctransfer[10]={10,6,4,2,0,11,7,5,3,1};
	//ctransfer[0]= 
	// DIN signals
	for (i=0;i<8;i++)
	{
		// check that the decInput is within range (0-65535)
		if ( ((*sCurrentData).decInputs[i]<=65535) && ((*sCurrentData).decInputs[i] >= 0) )
			trash = (*sCurrentData).decInputs[i]; // range ok
		else
		{
			RecordError(INPUT_OUT_OF_RANGE,(*sCurrentData).Channels[i]); // the current is too large
			return 1;  // return error
		}

		k=0;
		// convert decInput to a 16-bit binary string
		do
		{
			remain = trash % 2;
			trash = trash / 2;
			temp[k++] = remain;
		} while (trash > 0);

		// add additional zeros if all 16-bits arent used
		for (j=k; j<16; j++)
			temp[j]=0;
		// reverse the spelling
		for (j=0; j<16; j++)
			binData[j] = temp[15-j];

		// making the Data stack
		for (k=1; k<=16; k++)
		{
			// if binData[k] is high then 2^(channel) is added to the data stack
			(*sCurrentData).data[2*k]= (*sCurrentData).data[2*k] + (uInt16)(pow(2.0,ctransfer[((*sCurrentData).Channels[i])])*binData[k-1]);
			(*sCurrentData).data[2*k+1]= (*sCurrentData).data[2*k]; // repeat for SCLK high
		}
	}

	// raise CS pin (acutally lowers pin on DAC b/c of inverting operation)
	for (i=0;i<SAMPLE_NUM;i++)
		(*sCurrentData).data[i]=(*sCurrentData).data[i] + (uInt16)pow(2.0,CS_CHANNEL);
	
	// lower CS (raise .. see above)
	// this is probably unecessary but for safety sake
	(*sCurrentData).data[SAMPLE_NUM-6]=0;
	(*sCurrentData).data[SAMPLE_NUM-5]=0;
	(*sCurrentData).data[SAMPLE_NUM-4]=0;
	(*sCurrentData).data[SAMPLE_NUM-3]=0;
	(*sCurrentData).data[SAMPLE_NUM-2]=0;
	(*sCurrentData).data[SAMPLE_NUM-1]=0;

	// SCLK is on for every odd i, except the last two samples where everything is 0
	for (i=0;i<18;i++)
		(*sCurrentData).data[2*i+1]=(*sCurrentData).data[2*i+1]+(uInt16)pow(2.0,SCLK_CHANNEL);

	return 0; // no error
}

//==========================================================================================
/* This is the only function which communicates with the hardware.  The data is fed one 
   word at a time to the DAQ.  This is mostly taken from the WriteDigitalPort example.  The
   DAQ doesn't have an onboard clock or buffer so there is a sleep function which waits 1 ms.
   I would like to have it wait 125 us (or 8 kHz) but I need to find another function which 
   does us wait times */
int SendStack(struct CurrentData *sCurrentData)
{
	int         error=0;
	uInt8		status=1; //1 is no error 0 is error
	TaskHandle	taskHandle=0;
	char        errBuff[2048]={'\0'};
	int32		written;
	float       Timeout=10.0; 
    uInt64      generated;  
	int i;
	
	//QueryPerformanceCounter(&t1);// start timer
	DAQmxErrChk (DAQmxCreateTask("",&taskHandle));
	DAQmxErrChk (DAQmxCreateDOChan(taskHandle,"Dev1/port0:1","",DAQmx_Val_ChanForAllLines)); 
	DAQmxErrChk (DAQmxStartTask(taskHandle));  


	for (i=0;i<SAMPLE_NUM;i++)
	{
		// send word
		DAQmxErrChk (DAQmxWriteDigitalU16(taskHandle,1,1,Timeout,DAQmx_Val_GroupByScanNumber,&(*sCurrentData).data[i],&written,NULL));
		
		// wait 1ms
		Sleep(1);
		// printf("%d samples written to on-board FIFO\n",written); 
	}
	//QueryPerformanceCounter(&t2);// stop timer
	// clear the task when done
	DAQmxErrChk (DAQmxClearTask(taskHandle));

	//Output timer
    //elapsedTime =(t2.QuadPart-t1.QuadPart)*1000.0/frequency.QuadPart;
	//printf("Timer: %.6f ms\n\n",elapsedTime);

Error:
	if( DAQmxFailed(error) )
		DAQmxGetExtendedErrorInfo(errBuff,2048);
	if( taskHandle!=0 ) {
		DAQmxWaitUntilTaskDone(taskHandle,Timeout);
		DAQmxGetWriteTotalSampPerChanGenerated(taskHandle,&generated); 
		DAQmxStopTask(taskHandle);
		DAQmxClearTask(taskHandle);
	}
	if( DAQmxFailed(error) )
	{
		// record error, maybe I'll make it so it records the errBuff in the if statement above
		RecordError(FAILED_COMMUNICATION_WITH_DAQ);
		return 1;
	}

	return 0; // no error
}

//==========================================================================================
// File manipulation/reading functions
//==========================================================================================

int ReadCalibrationMatrix (struct BFieldData *sBFieldData)
{
	Book* book = xlCreateBook();
	if(book->load(SETTINGS_FILE.c_str())) //open the excel book
	{
		Sheet* sheet = book->getSheet(0); //get 1st sheet fromt the excel book
		if(sheet)
		{
			// read channels from .xls file
			for (unsigned int i=0;i<8;i++) 
			{
				for (unsigned int j=0;j<8;j++)
					(*sBFieldData).CalibrationMatrix[i][j]=(double)sheet->readNum(CALIBRATION_MATRIX_ROW+i,CALIBRATION_MATRIX_COL+j);
			}
		}
		else
			return -1; // error cant open sheet
	}
	else
		return 1; // error cant open book

	book->release(); // close file
	return 0; // no error
}

//==========================================================================================
/* Returns the decInput that will make the current source output the closest current to 
   current.  Finds the two end points which sandwich the desired current and then performs 
   a linear approximation based on the end points. Returns -1 if there is an error. */
int TransferFunction(int channelNum, double current)
{
	int decInput=0;
	int i=0;
	double a,b;

	Book* book = xlCreateBook();
	if(book->load(SETTINGS_FILE.c_str())) //open the excel book
	{
		Sheet* sheet = book->getSheet(0); //get 1st sheet fromt the excel book
		if(sheet)
		{
			i = 0;
			do 
			{
				// get max and min for the range
				a=(double)sheet->readNum(TRANSFER_ROW+i,(channelNum*SEPARATION) + TRANSFER_COL); // read min
				b=(double)sheet->readNum(TRANSFER_ROW+i,(channelNum*SEPARATION) + TRANSFER_COL + 2); // read max
				//printf("%d ",i);
				i++;
			} while ( (current > b) && (i<16) ); // greater than the max

			if ( i > 16 )
				return -2;

			if ( current >= a ) // and greater than min then use this range
				// the decimal value of min (i.e. a), plus the linear approx
				decInput = (i-1)*4096 + LinearApprox(b,a,current); // i has been iterated so (i-1) is correct
			else // but less than min 
			{
				// minus 2 because need to read the previous line and i has been iterated already
				b=(double)sheet->readNum(TRANSFER_ROW+i-2,(channelNum*SEPARATION) + TRANSFER_COL + 2); // read previous max
				if ( (current-b) <= (a-current) )
					decInput = (i-1)*4096 - 1;
				else
					decInput = (i-1)*4096;					
			}
		}
		else
			return -1; // error cant open sheet
	}
	else
		return -1; // error cant open book

	book->release(); // close file
	return decInput; // no error
}
int TransferFunctionBipolar(int channelNum, double current)
{
	int decInput=0;
	int i=0;
	//double a,b;
	decInput=round(.00035/2.026*current*1*32768)+32768;
	
	return decInput; // no error
}
int TransferFunctionUnipolar(int channelNum, double current)
{
	int decInput=0;
	int i=0;
	//double a,b;
	double RLimit=.00035; //for resistor value 350 Ohm
	decInput=round(RLimit/2.026*current*1000000*65536);  
	
	return decInput; // no error
}
//==========================================================================================
/* Read all settings into the CurrentData struct from the .xls file.  Returns 1 if there is
   an error.  I took Kunyans DDS code and modified it. */
int ReadFromSettings(struct CurrentData *sCurrentData)
{
	Book* book = xlCreateBook();
	if(book->load(SETTINGS_FILE.c_str())) //open the excel book
	{
		Sheet* sheet = book->getSheet(0); //get 1st sheet fromt the excel book
		if(sheet)
		{
			// read channels from .xlsx file
			for (unsigned int i=0;i<8;i++) 
			{
				// The excel book cell (1,1) is actually (0,0) here
				// read the column of channels from .xlsx file starting at book cell (2,2)
				(*sCurrentData).Channels[i]=(int)sheet->readNum(CHANNEL_ROW+i,1);
			}
		}
		else
			return -1; // error cant open sheet
	}
	else
		return 1; // error cant open book

	book->release(); // close file
	return 0; // no error
}

//==========================================================================================
/* Reads the last line of pFile and takes everything before delim character.
   Then coverts string to int + 1. 
   This is used to get the current run, if the file is empty then it will start at 1*/
int GetNextRunNumber(FILE *pFile, char delim)
{
	char line[512]; // stores the current line
	char LastRun[5]; // records the last run number in char format
	int i=0;	

	// if EOF is encountered right away then start at run 1
	if (fgets(line,512,pFile)==NULL)
		return 1;

	// read line-by-line until EOF and store last line in line[512]
	while (fgets(line,512,pFile) != NULL); 
		
	// read last line until the delimation char
	while (line[i] != delim)
	{
		LastRun[i]=line[i]; // record up to the first delim
		i++;
	}
	LastRun[i]=0;  // Null ends string

	return atoi(LastRun)+1; // if file not empty return next integer
}

//==========================================================================================
/* Records errors by number in the RecordOutput.txt File with the run number.  ChennelNum 
   default is -1, if other than -1 then record channelNum with output. Returns 1 if there is 
   an error. */
int RecordError (int ErrorNum, int channelNum)
{
	FILE *RecordFile;
	const char tab = 9;  // tab is the column deliminator in the file

	RecordFile = fopen("RecordOutput.txt","a+"); // open file
	if (RecordFile != NULL) // check file exists
	{
		// print tab delimated info and start next line
		fprintf(RecordFile,"%d%c%s%d",GetNextRunNumber(RecordFile,tab),tab,"Error Number: ",ErrorNum);

		// if channelNum is given
		if (channelNum != -1)
			fprintf(RecordFile,"%c%s%d",tab,"Channel: ",channelNum);

		// new line
		fprintf(RecordFile,"\n");
		fclose(RecordFile);
		return 0; // no error
	}
	else
		return 1; // error
}

//==========================================================================================
/* Appends the run number, the channel, the decimal input, and the approx current to the 
   output file.  Returns 1 if error. */
int RecordOutput (struct CurrentData *sCurrentData)
{
	const char tab = 9;
	FILE *RecordFile;
	int RunNum,i;
	time_t rawtime;
	struct tm * timeinfo;

	RecordFile = fopen("RecordOutput.txt","a+");

	// get the current run number
	RunNum = GetNextRunNumber(RecordFile,tab);

	time ( &rawtime );
	timeinfo = localtime ( &rawtime );

	if (RecordFile != NULL)  // if file can be opened
	{
		for (i=0;i<8;i++)
		{
			// append tab delimated info to file
			fprintf(RecordFile,"%d%c%d%c%d%c%E%",RunNum,tab,(*sCurrentData).Channels[i],tab,(*sCurrentData).decInputs[i],tab,(*sCurrentData).Currents[i]);
			if (i==0)
				fprintf(RecordFile,"%c%s", tab, asctime (timeinfo));
			else
				fprintf(RecordFile,"\n");
		}
		fclose(RecordFile);
		return 0; // no error
	}
	else
		return 1; // error
}
//==========================================================================================
// Random functions
//==========================================================================================

// simple rounding fuction
int round (double a)
{
	int b;
	b = (int)floor(a);
	if (a-(float)b >= 0.5)
		return b+1;
	else
		return b;
}

//==========================================================================================
/* Check that the channel numbers are OK, Returns 0 if out of range and 1 
   if in range (0-15).  I wanted to read like a sentance which is why the output is opposite 
   the convention */
int ChannelNumOK (int channelNum)
{
	if ( !((channelNum >= 0) && (channelNum < 16)) )
	{
		RecordError(CHANNEL_NUM_RANGE,channelNum);
		return 0;  // error
	}
	else
		return 1;  // no error
}

//==========================================================================================
/* Does a gaussian elimination with partial pivoting solution to find the currents.  I should 
   just have an LUP factorization already done so that this need not be done everytime. */
int CalculateCurrents(struct BFieldData *sBFieldData)
{
	int i = 0;
	int j = 0;
	double a;
	int maxi;
	double trash[9]; // for row swapping

	// augmented matrix for the system
	double matrix[8][9];

	// copy calibration matrix to dummy var
	for (i=0;i<8;i++)
	{
		for (j=0;j<8;j++)
		{
			matrix[i][j]=(*sBFieldData).CalibrationMatrix[i][j];
		}
	}
	// augmented row
	for (i=0;i<8;i++)
		matrix[i][8]=(*sBFieldData).BFieldMag[i];

	i=0;
	j=0;
	while ((i <= 7)&&(j <= 7))
	{
		// Find max pivot in column j, starting in row i:
		maxi = i;
		for (int k=i+1; k<8; k++)
		{
			if (abs((matrix[k][j]) > abs(matrix[maxi][j])))
				maxi = k; // track the maximum pivot
		}
		if (matrix[maxi][j] != 0) // check to see if it is a row of zeros, if it is then there is no single solution
		{
			// swap rows i and maxi, but do not change the value of i
			if (i!=maxi) // check to see if swap is needed
			{
				for (int u=0;u<9;u++)
				{
					trash[u]=matrix[i][u]; // temporarily holds the row during the switch
					matrix[i][u]=matrix[maxi][u]; // move the maxi row up
					matrix[maxi][u]=trash[u]; // move row i down
				}
			}
			// now subtract the new row i from the higher rows
			for (int u = i+1;u<8;u++)
			{
				a = matrix[u][j]/matrix[i][j]; // get multiplier for row
				matrix[u][j]=0; // make sure no rounding errors
				
				 // subtract a times row i from row u
				for (int l=j+1;l<8;l++)
					matrix[u][l]=matrix[u][l]-a*matrix[i][l];
			}
			i++;
		}
		else
			return 1; // more than one solution...

		j++;
	}

	// now we need to do backwards substitution
	for (i=7;i>=0;i--)
	{
		a=0;
		for (j=i+1;j<8;j++)
			// solving the equation since we alread know the other currents
			a = a + matrix[i][j]*(*sBFieldData).Currents[j]; 
		
		// record the current
		(*sBFieldData).Currents[i] = (matrix[i][8] - a)/matrix[i][i];
		//printf("%f\n",(*sBFieldData).Currents[i]);
	}
	return 0; // no problems
}

//==========================================================================================
/* returns the integer value (0-4095) as to output as close to current as possible based on 
   the max and the min. */
int LinearApprox(double max, double min, double current)
{
	double slope;

	slope = (max-min)/4095; // there are 4095 possible values to output
	current = current - min; // cut off the intercept (i.e. min)
	return round(current/slope); // find the decimal value
}
//==========================================================================================


//==========================================================================================
// *Read coil currents from the excel book
//==========================================================================================
int ReadCoilCurrent(double curr[]) 
{
	Book* book = xlCreateBook();
	if(book->load(SETTINGS_FILE.c_str())) //open the excel book
	{
		Sheet* sheet = book->getSheet(0); //get 1st sheet fromt the excel book
		if(sheet)
		{
			// read channels from .xls file
			for (unsigned int i=0;i<8;i++) 
			{
				curr[i]=sheet->readNum(i+1,2); //first cell is (0,0)
			}
		}
		else
			return -1; // error cant open sheet
	}
	else
		return 1; // error cant open book

	book->release(); // close file
	return 0; // no error
}
//==========================================================================================
//==========================================================================================

#endif