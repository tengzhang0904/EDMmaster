/*********************************************************************************************
*  This header gives:
       *** definition and methods for Low frequency transitions (AO)
	   *** definition and methods for updating 8 digital channels
*  Last modified 2012/03/06 by KZ
**********************************************************************************************/

#ifndef _AudioPulse_h_included_
#define _AudioPulse_h_included_
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

#define PI 3.141592653589793
#define AoSampleRate 30000.0 //AO sample rate in Hz
#define EstSapNum 60000  //estimated sampel number, length > actual SampleNum

string AOChls="Dev4/ao0"; //specify AO channel(s) for the audio waveform
string DOChls="Dev4/port0"; //digital output channels on the AO card

class AudioPulse {
	
  public: 
	  float64 data[2][EstSapNum]; //to store waveform data and its T reversal
	  uInt32 SampleNum; //actual sample number of a single pulse, must be a integer multiple of 2

	  void init(); //initialize the data array
	  float64 Shaping(float64 t);//Shaping of the pulse to ensure smooth turning on/off
	  float64 lfwave(float64 t);//calculate AO voltage at point t
	  bool Calculate(void); //calculate data array and its time reversal
	  bool ReadExcel(string excel, const unsigned); //get pulse parameters from an excel file
	  bool WriteWaveformFile(string filename);//write calculated waveform data array to a txt file
	  bool AudioExe(bool option, float timeout);//flash the calculated data to AO device

  private: 
	  float64 basefreq; //base frequency in Hz
	  float64 duration; //pulse duration ms
	  float64 amplitude[3]; //ampltiudes of the 3 components in microGauss
	  float64 phase[3]; //relative phases of the 3 components and phase offset in PI
	  float64 gain;//calibration from magnetic field to voltage [V/uG]
};


void AudioPulse::init() //initilize data array
{
	SampleNum=10;
	for (int i=0;i<EstSapNum;i++)
	{
		data[0][i]=0.0;
		data[1][i]=0.0;
	}
}

bool AudioPulse::Calculate(void) //calculate data array and its time reversal
{   
	//caculate the first pulse
	SampleNum=(uInt32)ceil((duration*0.001)*AoSampleRate);
	for (unsigned int i=0;i<SampleNum;i++) 
	{
		data[0][i]=this->lfwave(((double)i)/AoSampleRate); 
	}
	if (SampleNum%2==1)  //check to ensure SampleNum is even, otherwise add another zero
	{ 
		SampleNum++; data[0][SampleNum-1]=0.0;
	}

	//calculate the time reversed pulse
	for (unsigned int i=0;i<SampleNum;i++) 
	{
		data[1][i]=data[0][(SampleNum-1)-i];
	}

    printf("AudioPulse::Computation complete, %d samples\n", SampleNum);
	return true;
}

float64 AudioPulse::lfwave(float64 t) //calculate AO voltage at point t[sec]
{
	float64 Vt=0.0; //initiation
    Vt=   amplitude[0]*sin(2*PI*1*basefreq*t+phase[2]*PI); // f
	Vt=Vt+amplitude[1]*sin(2*PI*3*basefreq*t+phase[2]*PI+phase[0]*PI); //3f
    Vt=Vt+amplitude[2]*sin(2*PI*5*basefreq*t+phase[2]*PI+phase[1]*PI);  //5f
    Vt=Vt*gain*(this->Shaping(t));  // var gain and shaping
	return Vt; 
}

float64 AudioPulse::Shaping(float64 t) //Shaping of the pulse, sin^2
{
	return pow(sin(PI*t/(duration*0.001)),2);
}

bool AudioPulse::WriteWaveformFile(string filename)//write calculated waveform data array to a txt file
{
   ofstream datalog(filename.c_str(),ios::out | ios::trunc); 
   datalog.setf(ios::fixed, ios::floatfield);
   datalog.setf(ios::showpoint);
   unsigned int i=0;
   while( i < SampleNum)
         {  
			datalog<<data[0][i]<<"  "<<data[1][i]<<endl;
            if( !datalog)   break;
            else   ++i;
         }
   if( i < SampleNum-1)
   {
      cerr << "AudioPulse::Error writing to file " << filename << endl;
      return false;
   }
   cout<<"AudioPulse:: "<<i<<" samples written to file"<<endl<< filename <<endl;
   return true;
}

bool AudioPulse::ReadExcel(string filename,const unsigned row) //read para from excel file
{  
   Book* book = xlCreateBook();
   if(book->load(filename.c_str())) //open the excel book
   {
      Sheet* sheet = book->getSheet(1); //get 2nd sheet fromt the excel book
      if(sheet)
      {
            basefreq=sheet->readNum(row, 0); // base frequency in Hz 
            duration=sheet->readNum(row, 1); //pulse duration ms
			amplitude[0]=sheet->readNum(row, 2); //B1 in uG
			amplitude[1]=sheet->readNum(row, 3); //B3 in uG
			amplitude[2]=sheet->readNum(row, 4); //B5 in uG
			phase[0]=sheet->readNum(row, 5); //Phase3 in PI
			phase[1]=sheet->readNum(row, 6); //Phase5 in PI
			phase[2]=sheet->readNum(row, 7); //Phase offset in PI
			gain=sheet->readNum(row, 8); //calibration from magnetic field to voltage [V/uG]
      }
   }
   book->release();

   if  (basefreq>0) //check whether reading is sucessful
   {
	   cout<<basefreq<<"  "<<duration<<"  "<<gain<<"  "
		   <<amplitude[0]<<" "<<amplitude[1]<<" "<<amplitude[2]<<"  "
		   <<phase[0]<<" "<<phase[1]<<" "<<phase[2]<<endl;
       return true;
   }
   else
   {
	   cout<<"AudoTrans:: End of pulse, or unable to read pulse in the specified row"<<endl;
	   return false;
   }
}


bool AudioPulse::AudioExe(bool option, float timeout)//flash the calculated data to AO device
{
	#define DAQmxErrChk(functionCall) if( DAQmxFailed(error=(functionCall)) ) goto Error; else

	int         error=0;
	TaskHandle  taskHandle=0;
	char        errBuff[2048]={'\0'};
	int32   	written;

	/*********************************************/
	// DAQmx Configure Code
	/*********************************************/
	DAQmxErrChk (DAQmxCreateTask("",&taskHandle));
	DAQmxErrChk (DAQmxCreateAOVoltageChan(taskHandle,AOChls.c_str(),"",-10.0,10.0,DAQmx_Val_Volts,NULL));
	DAQmxErrChk (DAQmxCfgSampClkTiming(taskHandle,"",AoSampleRate,DAQmx_Val_Rising,DAQmx_Val_FiniteSamps,SampleNum));
	//DAQmxErrChk (DAQmxCfgDigEdgeStartTrig(taskHandle,"/Dev0/PFI0",DAQmx_Val_Rising));
	DAQmxErrChk (DAQmxTaskControl(taskHandle,DAQmx_Val_Task_Commit));

	switch(option)
	{
	  case true: //Flash 1st pulse and its time reversal  
	            for (unsigned int pt=0; pt<2; pt++)
	               {
	                   DAQmxErrChk (DAQmxWriteAnalogF64(taskHandle,SampleNum,0,10.0,DAQmx_Val_GroupByChannel,data[pt],&written,NULL)); 
	                   //printf("Audio:: %d samples written to FIFO\n", written);
	                   DAQmxErrChk (DAQmxStartTask(taskHandle));
	                   DAQmxErrChk (DAQmxWaitUntilTaskDone(taskHandle,timeout));
	                   DAQmxStopTask(taskHandle);
	               }
			   break;

	  case false: //Flash one pulse ONLY
		         DAQmxErrChk (DAQmxWriteAnalogF64(taskHandle,SampleNum,0,10.0,DAQmx_Val_GroupByChannel,data[0],&written,NULL)); 
	             printf("Audio:: %d samples written to FIFO\n", written);
	             DAQmxErrChk (DAQmxStartTask(taskHandle));
	             DAQmxErrChk (DAQmxWaitUntilTaskDone(taskHandle,timeout));
	             DAQmxStopTask(taskHandle);
		       break;
	}

  Error:
	if( DAQmxFailed(error) )
		DAQmxGetExtendedErrorInfo(errBuff,2048);
	if( taskHandle!=0 ) {
		DAQmxStopTask(taskHandle);
		DAQmxClearTask(taskHandle);
	}
	if( DAQmxFailed(error) )
		printf("Audio::DAQmx Error: %s\n",errBuff);
	return 0;
}


/****************************************************************************************************
*  This class generate 8-bit digital pattern once on this AO card
*****************************************************************************************************/
class MMCDigit8{

      private:
		  uInt32 Digit8; //data to write AO card digital port

      public: 
		  void init() {Digit8=0x00000000;} //default values
	      void init(unsigned int []); //values from user: binary array
		  void init(uInt32); //values from user: hex format
		  bool ReadExcel(string excel); //get digital line values from an excel file
		  void updateDigPort();//update port once
		  uInt32 getDigit8(){return Digit8;}
};

void MMCDigit8::init(unsigned int digit[8])//values from user: binary array
{   
	uInt32 data=0;
	for(unsigned int i=0; i<8; i++)
	{
		data=data+(uInt32)pow(2,i)*digit[i];
	}
	this->Digit8=data;
}

void MMCDigit8::init(uInt32 data)//values from user: hex format
{   
	if(data>=0&&data<=255) this->Digit8=data;
	else  this->Digit8=0x00000000;
}

bool MMCDigit8::ReadExcel(string filename) //read para from excel file
{  
   Book* book = xlCreateBook();
   if(book->load(filename.c_str())) //open the excel book
   {
      Sheet* sheet = book->getSheet(0); //get 1st sheet fromt the excel book
      if(sheet)
      {     
		    unsigned int digit[8];
		    for(unsigned int i=0; i<8; i++)
			{
               digit[i]=(unsigned int)sheet->readNum(i+2, 2); 
			}
			this->init(digit);
      }
   } 
   book->release();

   return true;
}


void MMCDigit8::updateDigPort()//update digital port once
{
	#define DAQmxErrChk(functionCall) if( DAQmxFailed(error=(functionCall)) ) goto Error; else
	int         error=0;
	TaskHandle	taskHandle=0;
	char        errBuff[2048]={'\0'};
	int32		written;

	DAQmxErrChk (DAQmxCreateTask("",&taskHandle));
	DAQmxErrChk (DAQmxCreateDOChan(taskHandle,DOChls.c_str(),"",DAQmx_Val_ChanForAllLines));
	DAQmxErrChk (DAQmxStartTask(taskHandle));
	DAQmxErrChk (DAQmxWriteDigitalU32(taskHandle,1,1,10.0,DAQmx_Val_GroupByChannel,&Digit8,&written,NULL));
	cout<<"MMCDigit8::update->0x"<<setw(2)<<setfill('0')<<hex<<Digit8<<endl;

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



/****************************************************************************************************
*  This class Generates two low frequency oscillating magnetic waaveforms
*****************************************************************************************************/

class MagPulse {
	
public:
	float64 data[EstSapNum]; //array to store waveform data
	uInt32 SampleNumB; //actual sample number of a single pulse

	void init(); //initialize the data array
	bool Calculate(void); //calculate data array
	bool WriteFile(string filename); //write calculated waveform data array to a txt file
	float64 Field(float64 t); //generates oscillating magentic field function
	float64 Shape(float64 t); //shpaes the field in the form of a blackman pulse
	bool ExcelRead(string excel, const unsigned); //gets pulse paramters from excel file
	bool AudioDoppioExe(float timeout);//flash the calculated data to AO device

private:
	float64 centerfreq; //center frequency in Hz
	float64 delta; //shift in frequency
	float64 amplitude; //amplitude of waveform
	float64 duration; //pulse duration in ms

};

void MagPulse::init() //initilize data array 
{
	for (int i=0;i<EstSapNum;i++)
	{
		data[i]=0.0;
	}
}

bool MagPulse::Calculate(void) //calculate data array
{
	//caculate the first pulse
	SampleNumB = (uInt32)ceil((duration*0.001)*AoSampleRate);
	for (unsigned int i = 0; i<SampleNumB; i++)
	{
		data[i] = this->Field(((double)i) / AoSampleRate);
	}
	if (SampleNumB%2==1)  //check to ensure SampleNum is even, otherwise add another zero
	{ 
		SampleNumB++; data[SampleNumB-1]=0.0;
	}
	return true;
}

float64 MagPulse::Field(float64 t) //generates osc mag field pulse function
{
	float64 freq_A; 
	freq_A = centerfreq + (delta/2);
	float64 freq_B;
	freq_B = centerfreq - (delta/2);
	return amplitude*(cos(2*PI*t*(freq_A))+cos(2*PI*t*(freq_B)))*(this->Shape(t)); // the amplitude needs to be calibrated
	
}

float64 MagPulse::Shape(float64 t) //shapes to blackman pulse
{
	return 0.42 - 0.5*cos(2*PI*(t/(duration*0.001) )) + 0.08*cos(4*PI*(t/(duration*0.001) ));
}

bool MagPulse::ExcelRead(string filename, const unsigned row) //read parameters from excel file
{
	Book* book = xlCreateBook();
	if (book->load(filename.c_str())) //open the excel book
	{
		Sheet* sheet = book->getSheet(0); //get 1nd sheet from the excel book
		if (sheet)
		{
			centerfreq = sheet->readNum(row, 0); //center frequency in Hz 
			delta = sheet->readNum(row, 1); //shift in frequency 
			amplitude = sheet->readNum(row, 2); //amplitude in 
			duration = sheet->readNum(row, 3); //duration in ms
		}
	}
	book->release();

	if (centerfreq>0 && delta>0 ) //check whether reading is sucessful
	{
		cout << centerfreq << "  " << delta << "  " << amplitude << "  "
			<< duration << endl;
		return true;
	}
	else
	{
		cout << "MagTrans:: End of pulse, or unable to read pulse in the specified row" << endl;
		return false;
	}
}

bool MagPulse::WriteFile(string filename) //write calculated waveform data array to a txt file
{
	ofstream datalog(filename.c_str(), ios::out | ios::trunc);
	datalog.setf(ios::fixed, ios::floatfield);
	datalog.setf(ios::showpoint);
	unsigned int i = 0;
	while (i < SampleNumB)
	{
		datalog << data[i]  << endl;
		if (!datalog)   break;
		else   ++i;
	}
	if (i < SampleNumB - 1)
	{
		cerr << "AudioPulse::Error writing to file " << filename << endl;
		return false;
	}
	cout << "AudioPulse:: " << i << " samples written to file" << endl << filename << endl;
	return true;
}

bool MagPulse::AudioDoppioExe(float timeout)//flash the calculated data to AO device
{
	#define DAQmxErrChk(functionCall) if( DAQmxFailed(error=(functionCall)) ) goto Error; else

	int         error=0;
	TaskHandle  taskHandle=0;
	char        errBuff[2048]={'\0'};
	int32   	written;

	/*********************************************/
	// DAQmx Configure Code
	/*********************************************/
	DAQmxErrChk (DAQmxCreateTask("",&taskHandle));
	DAQmxErrChk (DAQmxCreateAOVoltageChan(taskHandle,AOChls.c_str(),"",-10.0,10.0,DAQmx_Val_Volts,NULL));
	DAQmxErrChk (DAQmxCfgSampClkTiming(taskHandle,"",AoSampleRate,DAQmx_Val_Rising,DAQmx_Val_FiniteSamps,SampleNumB));
	DAQmxErrChk (DAQmxCfgDigEdgeStartTrig(taskHandle,"/Dev4/PFI1",DAQmx_Val_Rising));
	DAQmxErrChk (DAQmxTaskControl(taskHandle,DAQmx_Val_Task_Commit));


    //Flash one pulse ONLY
	DAQmxErrChk (DAQmxWriteAnalogF64(taskHandle,SampleNumB,0,10.0,DAQmx_Val_GroupByChannel,data,&written,NULL)); 
	printf("Audio:: %d samples written to FIFO\n", written);
	DAQmxErrChk (DAQmxStartTask(taskHandle));
	DAQmxErrChk (DAQmxWaitUntilTaskDone(taskHandle,timeout));
	DAQmxStopTask(taskHandle);


  Error:
	if( DAQmxFailed(error) )
		DAQmxGetExtendedErrorInfo(errBuff,2048);
	if( taskHandle!=0 ) {
		DAQmxStopTask(taskHandle);
		DAQmxClearTask(taskHandle);
	}
	if( DAQmxFailed(error) )
		printf("Audio::DAQmx Error: %s\n",errBuff);
	return 0;
}

#endif