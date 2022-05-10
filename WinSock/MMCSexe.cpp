/****************************************************************************************************
* To make a standalone executable for the MMC-SupertiemMaster communication link
* Compile in command prompt with current address:  "cl MMCSexe.cpp /EHsc"
****************************************************************************************************/

#include "MMCSupertime.h" //MMC-SupertiemMaster communication link interface
#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <iostream>
#include <iomanip>
#include <string>
#include <cstdlib>
using namespace std;

#define TotNumSeq 5 //total number of sequences

void main(int argc, char* argv[])
{
	//sequence file name definition and sequence selection
	string sequences[TotNumSeq]={//*.dt4 file names can not contain spaces
	                             "MicrowaveTransitionsBNew.dt4",
	                             "MOToptimizeLoadingRate.dt4",
	                             "LaunchImageBottom+Z.dt4",
								 "Default_shiftLattice+Z.dt4",
	                             "LifetimeWithOP.dt4"};
	unsigned int id; //sequence id
	string loopnum; //loop number

	if (argc==3)
	{
		id=atoi(argv[1]); //convert 1st para as sequence id
		loopnum=argv[2]; //convert 2nd para as loop number
		if ((id<TotNumSeq)&&(id>=0))
		{
		   cout<<"Loop sequence: "<<endl<<sequences[id]<<endl<<"for "<<loopnum<<" times"<<endl;
		   string SupertimeCommand="loop "+loopnum+" "+sequences[id];
	       MMCSupertime(SupertimeCommand);
		}
	}  
    else 
	{
		cout << "Too many or too few command line parameters"<<endl;
	}

/*
	//pause the program for sometime
	const unsigned int mseconds=5000;
	clock_t goal = mseconds + clock();
    while (goal > clock());
*/   
}