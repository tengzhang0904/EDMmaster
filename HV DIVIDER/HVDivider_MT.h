//Funbction call to HVDivider
#ifndef _HVDivider_MT_included_
#define _HVDivider_MT_included_
#define _CRT_SECURE_NO_DEPRECATE
#pragma warning(disable: 4345)

#include "HVDivider.h"

#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <math.h>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <windows.h> 
using namespace std;

const extern string MMCfolder; //MMC_SYSINT folder

void HVDiv(int HVDoptions)
{
	const string HVDividerfolder=MMCfolder+"HV DIVIDER/";
    const string HVDividerDataFile=HVDividerfolder+"HVdata.txt";

	HVDivider *HV=new HVDivider();
    HV->init();
	HV->MeasureHV(HVDoptions);
    HV->WriteHVFile(HVDividerDataFile);

	delete HV;

}

float HVDivS() //measure once and return the data
{
	float HVdat=0.0;

	const string HVDividerfolder=MMCfolder+"HV DIVIDER/";

	HVDivider *HV=new HVDivider();
    HV->init();
	HV->MeasureHV(1);
	HVdat=HV->HVdataS;
	delete HV;
	//HVdat=(float)-42078.56; //Test number
	return HVdat;

}

#endif