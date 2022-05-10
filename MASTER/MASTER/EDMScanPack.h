//EDM_SCAN_Pack class definition and methods, derived from EDMSignalPack
#pragma once

#ifndef _EDMScanPack_h_included_
#define _EDMScanPack_h_included_
#define _CRT_SECURE_NO_DEPRECATE

#include <vector>  //std::vector
#include "EDMSignalPack.h" //parent class for DataPack
#include "ALGLIB/NLSF_MT.h"//Mathematical operations: integration, nonlinear fit, etc.
#include "SelectedChannels.h" //two selected channels for plots


//EDM_SCAN_Pack class definition and methods
class EDMScanPack
{
  public:
	std::vector<EDMSignalPack> EDMSCAN; //vector for storage of HV polarity pattern
	float ParaSWP[4]; //array for sweep parameters: initial, final, Step size and scan type
	std::vector<SelectedChannels> Selected; //vector for plotting selected channels during a SCAN

	void init(); //reset EDMScanPack
	bool GetPara(string ParaFile); //get sweep parameter from "SWPara.txt" file
	bool AddShot(string filename); //add a single SHOT to the SCAN

	//pull out selected points from EDMSCAN: type=group or pixel, index=which group or pixel?
	bool GetSelectChls(unsigned int type, unsigned int index, unsigned int info); 

	void WriteSelectedChls(string filename); //write selected channel data for a SCAN to file
};

//reset EDMScanPack
void EDMScanPack::init()
{
	ParaSWP[0]=0.0; ParaSWP[1]=1.0; ParaSWP[2]=2.0; ParaSWP[3]=0.0; 
    EDMSCAN.clear(); //EDMSCAN vector will be reset as size=0
	Selected.clear(); 
}

//get sweep parameter from "SWPara.txt" file
bool EDMScanPack::GetPara(string ParaFile)
{
	bool successflag;
	ifstream datalog;
    datalog.open (ParaFile.c_str(), ios::in); 
	successflag=!(datalog.fail());
    if (successflag)
	{
		for(unsigned int i=0;i<4;i++)
			datalog>>ParaSWP[i];
	}
	datalog.close();
	return successflag;
}

//add a single SHOT to the SCAN
bool EDMScanPack::AddShot(string filename) 
{
	EDMSignalPack *CurrPac=new EDMSignalPack();
	bool flag=CurrPac->ReadTXT(filename);
	if (flag)
	{
		EDMSCAN.push_back(*CurrPac);
	}
	delete CurrPac;
	return flag;
}

//pull out selected points from EDMSCAN: type=group or pixel, index=which group or pixel?
bool EDMScanPack::GetSelectChls(unsigned int type, unsigned int index, unsigned int info)
{
	bool rangecheck=((type==0)&&(index<EDMGrpAvg))||((type==1)&&(index<(PDANpixel/2))); //check data select range
	bool EDMinfocheck=info<EDMinfocnt; //type of info, ie. EDMsignal, asymmetry, etc.
	if (rangecheck && EDMinfocheck)
	{
		unsigned int PTS=unsigned int(ParaSWP[2]); //number of steps
		SelectedChannels SelItem; //selected item
		for (unsigned int i=0; (i<PTS)&&(i<EDMSCAN.size()); i++)
		{
		   SelItem.ScanPara=double(i);  
		   SelItem.Sign=EDMSCAN.at(i).HV_Por;
		   switch(type)
		   {
		     case 0: //a group average is selected
				 SelItem.ChlA=EDMSCAN.at(i).EDMSignalAvg[index][info];
				 SelItem.ChlB=EDMSCAN.at(i).EDMSignalAvg[index+EDMGrpAvg][info];
			    break;
		     case 1: //a pixel is selected
				 SelItem.ChlA=EDMSCAN.at(i).EDMSignal[index][info];
				 SelItem.ChlB=EDMSCAN.at(i).EDMSignal[index+(PDANpixel/2)][info];
			    break; 
		     default:
				 SelItem.ChlA=0.0;
				 SelItem.ChlB=0.0;
			    break;
		   }
		   Selected.push_back(SelItem);
		}
		return true;
	}
	else 
		return false;
}

//write selected channel data for a SCAN to file
void EDMScanPack::WriteSelectedChls(string filename) 
{
	ofstream datalog(filename.c_str(),ios::out | ios::trunc);
	datalog<<Selected.size()<<endl; //write scan number first
	for(unsigned int i=0;i<Selected.size();i++)   
	{
		datalog<<Selected.at(i).ScanPara<<" ";
		datalog<<Selected.at(i).Sign<<" ";
		datalog<<Selected.at(i).ChlA<<" ";
		datalog<<Selected.at(i).ChlB<<" ";
		datalog<<endl;
	}
	datalog.close();
}

#endif