//Class for HV polarity pattern
#pragma once

#ifndef _HVpolarity_h_included_
#define _HVpolarity_h_included_
#define _CRT_SECURE_NO_DEPRECATE

#include <stdio.h>
#include <math.h>
#include <vector>  //std::vector
#include <windows.h> 
#include <cstdlib>
using namespace std;
using namespace DATACONST;

class HVpolarity
{
   private: 
	   std::vector<int> HVPorPat; //vector for storage of HV polarity pattern

   public: 

	   int GetHVpor(unsigned int scan_id); //return the HV polarity for a particular scan index
	   unsigned int Length(); //return the length of the HV polarity pattern
	   unsigned int OptimalScanNum(unsigned int scan_num); //return a nearest optimal number of steps during a scan, i.e. n=13 (n=9) will be changed to n=16 (n=8); 
	   void GenPorPat(unsigned int scan_num, unsigned int options); //generate HV polarity pattern based on use-specified options

};


//return the HV polarity for a particular scan index
int HVpolarity::GetHVpor(unsigned int scan_id)
{
	return HVPorPat.at(scan_id); //index start from 0
}

//return the length of the HV polarity pattern
unsigned int HVpolarity::Length()
{
	return HVPorPat.size();
}


//return a nearest optimal number (2^N) of steps during a scan
unsigned int HVpolarity::OptimalScanNum(unsigned int scan_num)
{
	if (scan_num<=2)
	   return 2;

	else
	{
       double pwr=log((double)scan_num)/log(2.0);
	   double newpwr=floor(pwr);
	   if (pwr-newpwr<=0.5)
	       return (unsigned int)pow(2,newpwr);
	   else
		   return (unsigned int)pow(2,newpwr+1);
	}
}

//generate HV polarity pattern based on use-specified options
void HVpolarity::GenPorPat(unsigned int scan_num, unsigned int options) 
{
	/****************************************************************
	*Options: 
	* 1     : alternating sign for testing purpose +-+-+-+-, ...
	* 2     : optimally reversed +--+, -++-, -++-, +--+, ...
	*****************************************************************/
	unsigned int CurrentLength=2;
	unsigned int OptScanNum=scan_num;
	unsigned int bits=1;

	switch (options)
	{
	case 1: 
		for (unsigned int i=1; i<=scan_num; i++)
		{
			if (i%2==1)
			  HVPorPat.push_back(1);
			else
              HVPorPat.push_back(-1);
		}
		break;

	case 2: 
		OptScanNum=this->OptimalScanNum(scan_num);
		bits=unsigned int(log((double)OptScanNum)/log(2.0));

		//HV polarity for 1st two SHOTs
		HVPorPat.push_back(1); HVPorPat.push_back(-1);

		//calculate HV polarity pattern
		for (unsigned int i=2; i<=bits; i++)
		{
			CurrentLength=HVPorPat.size();
			for (unsigned int j=1; j<=CurrentLength; j++)
			{
				HVPorPat.push_back((-1)*HVPorPat.at(j-1)); //P_n+1={Pn, -Pn}
			}
		}
		break;

	default:
		break;
	}
}

#endif


/*

MMC test of HV pattern class:
//////////////////////////////////////////////////////////////////////////////////////////////////
	    HVpolarity *HVPattern=new HVpolarity();
		HVPattern->GenPorPat(SWP_Steps,2); //Generate HV pattern
		string HVPatternStr="Test HV pattern: N="+to_string(HVPattern->Length())+"\n";
		for (unsigned int i=0; i<HVPattern->Length(); i++)
		{
			HVPatternStr=HVPatternStr+to_string(HVPattern->GetHVpor(i))+"\t";
			if ((i+1)%4==0)
              HVPatternStr=HVPatternStr+"\n";
		}
	    m_RICHEDIT_REPORT=HVPatternStr.c_str();
		UpdateData(FALSE);
		delete HVPattern;
///////////////////////////////////////////////////////////////////////////////////////////////////

Test HV pattern: N=32
1	-1	-1	1	
-1	1	1	-1	
-1	1	1	-1	
1	-1	-1	1	
-1	1	1	-1	
1	-1	-1	1	
1	-1	-1	1	
-1	1	1	-1	

*/