//Functional call to MMC_Suptertime

#ifndef _MMCS_MT_included_
#define _MMCS_MT_included_
#define _CRT_SECURE_NO_DEPRECATE

#include "MMCSupertime.h"
#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <iomanip>
#include <string>
#include <cstdlib>
using namespace std;

#define TotNumSeq 5 //total number of sequences

void MMCS(unsigned int id, string loops)
{
    //sequence file name definition and sequence selection
	string sequences[TotNumSeq]={//*.dt4 file names can not contain spaces
		"Multiple_4_uW_7_levels_precession_Bz.dt4",
	    "Multiple_4_uW_7_levels_precession_Bx.dt4",
	                             "LaunchImageBottom+Z.dt4",
								 "Default_shiftLattice+Z.dt4",
	                             "LifetimeWithOP.dt4"};
	cout<<"Choose from the following sequences"<<endl<<endl
		<<"Index  "<<"Filename"<<endl;
	for(unsigned int k=0; k<TotNumSeq; k++)
	{
		cout<<setw(5)<<k<<"  "<<sequences[k]<<endl;
	}

    string SupertimeCommand="loop "+loops+" "+sequences[id];
	MMCSupertime(SupertimeCommand);  //run command 1
}

#endif




