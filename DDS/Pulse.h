/***************************************************************

*  This class gives definition and methods for a microwave pulse

*  Last modified 2012/01/11 by KZ

****************************************************************/


#ifndef _Pulse_h_included_
#define _Pulse_h_included_
#define _CRT_SECURE_NO_DEPRECATE

#include <stdio.h>
#include <math.h>
#include <iostream>
#include <iomanip>
#include <string>
#include <windows.h> 
#include "Excel_Link/libxl.h" //excel linking library
#pragma comment(lib, "libxl.lib")
using namespace std;
using namespace libxl;


class MicrowavePulse {

      private:
		unsigned int PulseNumber; //Index of the pulse
        double  CenterFrequency;    //f0, center frequency of the pulse in Hz   
        double  HFSR;    //Half-frequency sweeping range in Hz
		double  Duration; //Pulse duration
		double AmpPeak; //Pulse peak amplitude
		unsigned int PulseType; //Pulse type
		bool PulseTerminator;    //indicate whether to terminate FIFO clock at the end of a pulse  

      public:
	    void Display();
		unsigned int getPulseNumber()  {return PulseNumber;}  //Index of the pulse
		double  getCenterFrequency() {return CenterFrequency;}    //f0, center frequency of the pulse in Hz   
        double  getHFSR() {return HFSR;}    //Half-frequency sweeping range in Hz
		double  getDuration()  {return Duration;}//Pulse duration
		double getAmpPeak()  {return AmpPeak;} //Pulse peak amplitude
		unsigned int getPulseType() {return PulseType;}; //Pulse type
		bool getPulseTerminator()  {return PulseTerminator;}   //indicate whether to terminate FIFO clock at the end of a pulse  
		bool init(); //default pulse settings
		bool init(unsigned int, double, double, double, double, unsigned int, bool); //use input pulse settings
		bool ReadExcel(const char * excel, const unsigned); //get pulse parameters from an excel file
		MicrowavePulse Copy(MicrowavePulse pulse) {return pulse;}//copy a pulse to this pulse
		MicrowavePulse ReadFile(const char * excel, const unsigned); //get pulse parameters from an excel file
		void Header(); //print pulse train header
};

bool MicrowavePulse::init() //default values
{
		PulseNumber=1; //Index of the pulse
        CenterFrequency=10000000.0;    //f0, center frequency of the pulse in Hz   
        HFSR=0.0;    //Half-frequency sweeping range in Hz
		Duration=1.0; //Pulse duration
		AmpPeak=0.0; //Pulse peak amplitude
		PulseType=1; //Pulse type
		PulseTerminator=true;    //indicate whether to terminate FIFO clock at the end of a pulse  
		return true;
}

bool MicrowavePulse::init(unsigned int num, double fc, double hfsr, double tp, double pk, unsigned int ty, bool ter)
{
	 	PulseNumber=num; //Index of the pulse
        CenterFrequency=fc;    //f0, center frequency of the pulse in Hz   
        HFSR=hfsr;    //Half-frequency sweeping range in Hz
		Duration=tp; //Pulse duration
		AmpPeak=pk; //Pulse peak amplitude
		PulseType=ty; //Pulse type
		PulseTerminator=ter;    //indicate whether to terminate FIFO clock at the end of a pulse  
		return true;
}

bool MicrowavePulse::ReadExcel(const char* filename,const unsigned row)
{  
   Book* book = xlCreateBook();
   if(book->load(filename)) //open the excel book
   {
      Sheet* sheet = book->getSheet(0); //get 1st sheet fromt the excel book
      if(sheet)
      {
		    PulseNumber=row; //Index of the pulse
            CenterFrequency=sheet->readNum(row, 0);    //f0, center frequency of the pulse in Hz   
            HFSR=sheet->readNum(row, 1);    //Half-frequency sweeping range in Hz
		    Duration=sheet->readNum(row, 2); //Pulse duration
		    AmpPeak=sheet->readNum(row, 3); //Pulse peak amplitude
			PulseType=int(sheet->readNum(row, 4)); //Pulse Type

		    if (sheet->readNum(row, 5)==0) PulseTerminator=false; //indicate whether to terminate FIFO clock at the end of a pulse
            else if (sheet->readNum(row, 5)==1) PulseTerminator=true; 
      }
   }
   book->release();

   if  (CenterFrequency>0) //check whether reading is sucessful
       return true;
   else
	   cout<<"End of pulse, or unable to read pulse in the specified row"<<endl;
	   return false;
}

MicrowavePulse MicrowavePulse::ReadFile(const char* filename,const unsigned row)
{  
   static MicrowavePulse PulseR;
   PulseR.init();

	Book* book = xlCreateBook();
   if(book->load(filename)) //open the excel book
   {
      Sheet* sheet = book->getSheet(0); //get 1st sheet fromt the excel book
      if(sheet)
      {
		    PulseR.PulseNumber=row; //Index of the pulse
            PulseR.CenterFrequency=sheet->readNum(row, 0);    //f0, center frequency of the pulse in Hz   
            PulseR.HFSR=sheet->readNum(row, 1);    //Half-frequency sweeping range in Hz
		    PulseR.Duration=sheet->readNum(row, 2); //Pulse duration
		    PulseR.AmpPeak=sheet->readNum(row, 3); //Pulse peak amplitude
			PulseR.PulseType=int(sheet->readNum(row, 4)); //Pulse Type

		    if (sheet->readNum(row, 5)==0) PulseTerminator=false; //indicate whether to terminate FIFO clock at the end of a pulse
            else if (sheet->readNum(row, 5)==1) PulseTerminator=true; 

      }
   }
   book->release();

   if  (PulseR.getCenterFrequency()>0) //check whether reading is sucessful
       return PulseR;
   else
	   cout<<"End of pulse, or unable to read pulse in the specified row"<<endl;
	   PulseR.init(); PulseR.PulseNumber=row; return PulseR;
}

void MicrowavePulse::Header()
{ 
	cout<<"***************************************************************************************"<<endl;
    cout<<"Pulse       f_res[Hz]       HFSR[Hz]    Tp/ms    AMP_PK    Type                  CLKOFF"<<endl;
}

void MicrowavePulse::Display()
{
		   string PulseTypeName[5]={"", "Adiabatic Passage", "Blackman PI", "Composite PI", "Composite ARP"}, name; //store pulse names, first one blank out
		   name=PulseTypeName[this->getPulseType()];  //convert PulseType from an integer to its real name

		   string Terminatorflag; //whether to terminate FIFOCLK
		   if (this->getPulseTerminator()) Terminatorflag="YES";
		   else  Terminatorflag="NO";
           

           cout<<setfill(' ')
		   <<setw(3)<<right<<this->getPulseNumber()<<"   "
		   <<setw(14)<<right<<fixed<<setprecision(6)<<this->getCenterFrequency()<<"   "
		   <<setw(12)<<right<<fixed<<setprecision(6)<<this->getHFSR()<<"   "
		   <<setw(6)<<right<<fixed<<setprecision(3)<<this->getDuration()<<"   "
		   <<setw(6)<<right<<fixed<<setprecision(5)<<this->getAmpPeak()<<"   "
		   <<setw(20)<<right<<name.c_str()<<"   "
		   <<setw(3)<<right<<Terminatorflag.c_str()
		   <<endl;
}

/* TEST CODE
	MicrowavePulse Pulse;
	Pulse=Pulse.ReadFile("Pulse parameters.xls",3);
	Pulse.Display();
*/

#endif

