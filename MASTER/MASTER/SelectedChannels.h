//Define two selected channels for plots
#pragma once

#ifndef _SelectedChannels_h_included_
#define _SelectedChannels_h_included_
#define _CRT_SECURE_NO_DEPRECATE

//////////////////////////////////////////////////////////
//Define two selected channels for plots, 
//i.e. a particular pixel/group of (+Z,-Z) for a SHOT 
class SelectedChannels
{
  public :
	double ScanPara, Sign, ChlA, ChlB; //scan parameter and two selected data channels
	SelectedChannels(); //constructor init
};

SelectedChannels::SelectedChannels()
{
	ScanPara=0.0;
	Sign=0.0;
	ChlA=0.0;
	ChlB=0.0;
}
//////////////////////////////////////////////////////////

#endif