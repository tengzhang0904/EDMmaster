/***************************************************************

*  This class gives high precision time delays between two points

*  Last modified 2011/11/19 by KZ

****************************************************************/


#ifndef _Timer_h_included_
#define _Timer_h_included_
#define _CRT_SECURE_NO_DEPRECATE

#include <stdio.h>
#include <math.h>
#include <iostream>
#include <iomanip>
#include <string>
#include <windows.h> 
using namespace std;


class PrecisionTimer {

      private:
        LARGE_INTEGER frequency;        // ticks per second
        LARGE_INTEGER tStart, tEnd;      // ticks
		double elapsedTime;             // elapsed time in millisec

      public:
		void getFreuqnency(){QueryPerformanceFrequency(&frequency);}
        void Start(){QueryPerformanceCounter(&tStart);}  // start timer
		void Stop(){QueryPerformanceCounter(&tEnd);}   // stop timer
		double getTime(); //calculate delay
		void Display(); 

};

double PrecisionTimer::getTime()
		{
			elapsedTime =(tEnd.QuadPart-tStart.QuadPart)*1000.0/frequency.QuadPart;  //calculate delay in ms
			return elapsedTime;
		}

void PrecisionTimer::Display()
		{
			cout<<"Timer: "<<fixed<<setprecision(3)<<this->getTime()<<"ms"<<endl;
		}


/*
	  PrecisionTimer ReadPulse; //Initialize a precision timer
	  ReadPulse.getFreuqnency(); //Get CPU frequency
	  ReadPulse.Start(); //Start timer

	  //insert code section you want to test here

	  ReadPulse.Stop();  // Stop timer
	  ReadPulse.Display();
*/

#endif

