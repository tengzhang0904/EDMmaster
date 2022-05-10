//EDM Signal DataPack with EDM signal and asymmetry info, derived from DataPack class
#pragma once

#ifndef _EDMSignalPack_h_included_
#define _EDMSignalPack_h_included_
#define _CRT_SECURE_NO_DEPRECATE

#include "DataPack.h" //parent class for DataPack

//class definition of a EDMSignalPack: child class
///////////////////////////////////////////////////////////////////////////////////////////////////////////
class EDMSignalPack : public DataPack
{
  public:
	  double EDMSignal[PDANpixel][EDMinfocnt]; //EDM signal and asymmetry info for each location
	  double EDMSignalAvg[2*EDMGrpAvg][EDMinfocnt]; //EDM signal and asymmetry average info for each group 

	  EDMSignalPack(); //constructor init
	  bool AnalyzeEDM(); //Analyze PDA signal to get EDMSignal[][]
	  bool GrpAvgEDM(unsigned int IdxRange[]); //Group average of EDMSignal[][]
	  bool WriteTXT(string filename); //Write DataPack from a TXT file
	  bool ReadTXT(string filename); //Read DataPack from a TXT file
};

//constructor init
///////////////////////////////////////////////////////////////////////////////////////////////////////////
EDMSignalPack::EDMSignalPack()
{
	for(int j=0;j<PDANpixel;j++)   
		{
			for(int k=0;k<EDMinfocnt;k++)
			    {
				     EDMSignal[j][k]=0.0; 
			    }
	     }

	for(int j=0;j<2*EDMGrpAvg;j++)   
		{
			for(int k=0;k<EDMinfocnt;k++)
			    {
				     EDMSignalAvg[j][k]=0.0; 
			    }
	     }
}

//Write DataPack from a TXT file
///////////////////////////////////////////////////////////////////////////////////////////////////////////
bool EDMSignalPack::WriteTXT(string filename)
{
	DataPack::WriteTXT(filename); //write the DataPack-part of the EDM signal in the same way

	ofstream datalog(filename.c_str(),ios::out | ios::app);

	//EDMsignal
	   for(int j=0;j<PDANpixel;j++)   
		{
			for(int k=0;k<EDMinfocnt;k++)
			    {
				     datalog<<EDMSignal[j][k]<<" ";
			    }
			datalog<<endl;
	     }

	//EDMsignalAvg
	   for(int j=0;j<2*EDMGrpAvg;j++)   
		{
			for(int k=0;k<EDMinfocnt;k++)
			    {
				     datalog<<EDMSignalAvg[j][k]<<" ";
			    }
			datalog<<endl;
	     }

	datalog.close();
	return true;
}

//Read DataPack from a TXT file
///////////////////////////////////////////////////////////////////////////////////////////////////////////
bool EDMSignalPack::ReadTXT(string filename)
{
	bool successflag;
	ifstream datalog;
    datalog.open (filename.c_str(), ios::in); 
	successflag=!(datalog.fail());
    if (successflag)
    {
//////////////////////////////////////////////////////////////
	datalog>>Timing_seqid;
    string loops; datalog>>loops; Timing_loops=loops.c_str(); 

	//Coil currents
	for(int i=0; i<8; i++) {
	     datalog>>Coil_Cur[i];
	   }

	//High voltage
	datalog>>HV_Por;
	datalog>>HV_Vol;
	datalog>>HV_Leak;

	//Analog monitors
	   for(unsigned int j=0; j<AMNumChls;j++)
	    {
		   for (unsigned int i=0; i<AMNumShots;i++)
		     {
			    datalog>>Ana_Mon[i][j];
		     }
	    }

	 //PDA
	   for(int j=0;j<PDANpixel;j++)   
		{
			for(int k=0;k<PDANshot;k++)
			    {
				     datalog>>PDA_Signal[j][k];
			    }
	     }

	  //Other
	   datalog>>Room_Temp;

	  //EDMsignal
	   for(int j=0;j<PDANpixel;j++)   
		{
			for(int k=0;k<EDMinfocnt;k++)
			    {
				     datalog>>EDMSignal[j][k];
			    }
	     }

      //EDMsignalAvg
	   for(int j=0;j<2*EDMGrpAvg;j++)   
		{
			for(int k=0;k<EDMinfocnt;k++)
			    {
				     datalog>>EDMSignalAvg[j][k];
			    }
	     }
 /////////////////////////////////////////////////////////
		datalog.close();
	    return true;
	}
	else
	{
		datalog.close();
	    return false;
	}
}


//Analyze PDA signal to get EDMSignal[][]
///////////////////////////////////////////////////////////////////////////////////////////////////////////
/*Default m_F level mapping of PDA_Signal for testing purpose is : 
*    8th col->background, 
*    4th col->m=0, 
*    1st (7th) col-> m=-3 (m=+3)
*  Modify matrix indices ijn this function if detection sequence is changed
*/
bool EDMSignalPack::AnalyzeEDM()
{
 if ((PDANshot==8)&&(EDMinfocnt>=3))
 {
	double SignalNorm=0.0; //norm. 
	double SignalAsy=0.0; //asymmetry
	double MiscInfo=0.0; //Other info
	for(int j=0;j<PDANpixel;j++)   
		{
			SignalNorm=0.0;SignalAsy=0.0; MiscInfo=0.0; //reset
			for (unsigned int p=0; p<=PDANshot-2; p++)
			{
				SignalNorm+=PDA_Signal[j][p]; //norm. 
				SignalAsy+=-mF[p]*PDA_Signal[j][p];//asymmetry
			}
			
			//Other info (to be defined)
			MiscInfo+=abs(PDA_Signal[j][0]-PDA_Signal[j][6]);
			MiscInfo+=abs(PDA_Signal[j][1]-PDA_Signal[j][5]);
			MiscInfo+=abs(PDA_Signal[j][2]-PDA_Signal[j][4]);

			if (SignalNorm>0.0)  //Avoid DivdByZero error
			{
               EDMSignal[j][0]=PDA_Signal[j][3]/SignalNorm; //EDM signal (m=0)
			   EDMSignal[j][1]=SignalAsy/SignalNorm; //asymmetry signal
			   EDMSignal[j][2]=MiscInfo/SignalNorm ; ////Other info
			}
	    }
    return true;
  }
  else 
    return false;
}

//Group average of EDMSignal[][]
///////////////////////////////////////////////////////////////////////////////////////////////////////////
/*The input parameter must be 
* a 2 integer array 
* to specify PDA pixel range for the +/- lattice side
* used to truncate begin/end part of pixels w/o atom signals
*/
bool EDMSignalPack::GrpAvgEDM(unsigned int Idx[])
{
 if ((PDANshot==8)&&(EDMinfocnt>=3))
 {
	 double GrpSize=(double(Idx[1]-Idx[0])/double(EDMGrpAvg)); //num. of pixels for each group
	 double GrpSum[EDMinfocnt]; //signal sum for each group

	 //index range for each group, see definition in the for-loop
	 double GrpIdx[2]; int GFC[2]; 

	 ////////////////////////////////////////////////////////////////////////
	 for (unsigned int q=0; q<2*EDMGrpAvg; q++)  // loop for all groups
	 {
		 if (q<EDMGrpAvg) //+Z side
		     GrpIdx[0]=double(Idx[0])+double(q)*GrpSize;  
		 else  //-Z side, add pixel index offset
			 GrpIdx[0]=double(PDANofst+Idx[0])+double(q-EDMGrpAvg)*GrpSize;  

		 GrpIdx[1]=GrpIdx[0]+GrpSize; 
		 GFC[0]=int(floor(GrpIdx[0]));
		 GFC[1]=int(ceil(GrpIdx[1]));

		 for(int k=0;k<EDMinfocnt;k++) //loop for each signal type
		 {
			GrpSum[k]=0.0; //reset GrpSum

		    for (int p=GFC[0]; p<=GFC[1]; p++) 
			   GrpSum[k]+=EDMSignal[p][k]; //Calculate GrpSum for each group

		    EDMSignalAvg[q][k]=GrpSum[k]/double(GFC[1]-GFC[0]+1); //Calculate EDMSignalAvg
		 }
	 }
	 ////////////////////////////////////////////////////////////////////////

    return true;
 }
 else 
    return false;
}

#endif