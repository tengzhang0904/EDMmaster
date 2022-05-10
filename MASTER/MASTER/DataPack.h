//Data management definitions
#pragma once

#ifndef _DataPack_h_included_
#define _DataPack_h_included_
#define _CRT_SECURE_NO_DEPRECATE

#include <stdio.h>
#include <math.h>
#include <iomanip>
#include <string>
#include <windows.h> 
#include <cstdlib>
#include <iostream>
#include <fstream>
#include "MASTERDlg.h"
using namespace std;
using namespace DATACONST;

#include "Excel_Link/libxl.h" //excel linking lib
#pragma comment(lib, "libxl.lib")
using namespace libxl;

//class definition of a data pack
///////////////////////////////////////////////////////////////////////////////////////////////////////////
class DataPack
{
   public:

  //Data memebers 

	   //Timing 
	   unsigned int Timing_seqid; 
	   CString Timing_loops;

	   //Coil currents
	   float Coil_Cur[8];

	   //High voltage
	   int HV_Por;
	   float HV_Vol;
	   float HV_Leak;

	   //Analog monitors
	   float Ana_Mon[AMNumShots][AMNumChls]; 

	   //PDA
	   float PDA_Signal[PDANpixel][PDANshot];

	   //Other
	   float Room_Temp;


  //Methods 
	   void init(); //initiation of data pack
	   void getPanel(CMASTERDlg *MasterDlg); //Get partial data from MASTER panel
	   bool WriteTXT(string filename); //Write DataPack from a TXT file
	   bool ReadTXT(string filename); //Read DataPack from a TXT file
	   bool WriteXLS(string filename);

	   void AddPac(DataPack *SecondPack); //Add data from a second pack
	   void DivPacByN(int NumOfPacs);//divide Pack data by N
};


//Implementation of DataPack 
///////////////////////////////////////////////////////////////////////////////////////////////////////////
void DataPack::init()//initiation of data pack
{
	   //Timing 
	   Timing_seqid=0; 
	   Timing_loops="002";

	   //Coil currents
	   for(int i=0; i<8; i++) {
	      Coil_Cur[i]=0.0;
	   }

	   //High voltage
	   HV_Por=0;
	   HV_Vol=0.0;
	   HV_Leak=0.0;

	   //Analog monitors
	   for(unsigned int j=0; j<AMNumChls;j++)
	    {
		   for (unsigned int i=0; i<AMNumShots;i++)
		     {
			    Ana_Mon[i][j]=0.0;
		     }
	    }

	   //PDA
	   for(int j=0;j<PDANpixel;j++)   
		{
			for(int k=0;k<PDANshot;k++)
			    {
				     PDA_Signal[j][k]=0.0; 
			    }
	     }

	   //Other
	   Room_Temp=23.0;
}


//Get partial data from MASTER panel, leaving other data unchanged
///////////////////////////////////////////////////////////////////////////////////////////////////////////
void DataPack::getPanel(CMASTERDlg *MasterDlg) 
{
	//Timing 
	Timing_seqid=MasterDlg->Sequence_id; 
	Timing_loops=MasterDlg->loopnum;

	//High voltage
	HV_Vol=(float)atof(MasterDlg->m_HVVOL);
	HV_Leak=(float)atof(MasterDlg->m_HVLEAK);

	//Analog monitors
	Ana_Mon[0][0]=(float)atof(MasterDlg->m_CAVITYP);
	Ana_Mon[0][1]=(float)atof(MasterDlg->m_CAVITYN);
	Ana_Mon[0][2]=(float)atof(MasterDlg->m_EXTBX);
	Ana_Mon[0][3]=(float)atof(MasterDlg->m_EXTBY);
	Ana_Mon[0][4]=(float)atof(MasterDlg->m_EXTBZ);
	Ana_Mon[0][5]=(float)atof(MasterDlg->m_BEAM1);
	Ana_Mon[0][6]=(float)atof(MasterDlg->m_BEAM2);
	Ana_Mon[0][7]=(float)atof(MasterDlg->m_BEAM3);

	//PDA
	for(int j=0;j<PDANpixel;j++)   
		{
			for(int k=0;k<PDANshot;k++)
			    {
				     this->PDA_Signal[j][k]=MasterDlg->Curve[j][k]; 
			    }
	     }

	//Other
	Room_Temp=(float)atof(MasterDlg->m_TEMPERATURE);
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////
bool DataPack::WriteTXT(string filename)
{
	ofstream datalog(filename.c_str(),ios::out | ios::trunc); //delete previous log then write the new one
////////////////////////////////////////////////////////////////////////////////////////
	datalog<<Timing_seqid<<endl;
    datalog<<Timing_loops<<endl; 

	for(int i=0; i<8; i++) {//Coil currents
	     datalog<<Coil_Cur[i]<<endl;
	   }

	datalog<<HV_Por<<endl;//High voltage
	datalog<<HV_Vol<<endl;
	datalog<<HV_Leak<<endl;

	//Analog monitors
	   for(unsigned int j=0; j<AMNumChls;j++)
	    {
		   for (unsigned int i=0; i<AMNumShots;i++)
		     {
			    datalog<<Ana_Mon[i][j]<<" ";
		     }
		   datalog<<endl;
	    }

	 //PDA
	   for(int j=0;j<PDANpixel;j++)   
		{
			for(int k=0;k<PDANshot;k++)
			    {
				     datalog<<PDA_Signal[j][k]<<" ";
			    }
			datalog<<endl;
	     }

	   //Other
	   datalog<<Room_Temp<<endl;
 ////////////////////////////////////////////////////////////////////////////////////////
	datalog.close();
	return true;
}

bool DataPack::ReadTXT(string filename)
{
	bool successflag;
	ifstream datalog;
    datalog.open (filename.c_str(), ios::in); 
	successflag=!(datalog.fail());
    if (successflag)
    {
////////////////////////////////////////////////////////////////////////////////////////
	datalog>>Timing_seqid;
    string loops; datalog>>loops; Timing_loops=loops.c_str(); 

	for(int i=0; i<8; i++) {//Coil currents
	     datalog>>Coil_Cur[i];
	   }

	datalog>>HV_Por;//High voltage
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
 ////////////////////////////////////////////////////////////////////////////////////////
		datalog.close();
	    return true;
	}
	else
	{
		datalog.close();
	    return false;
	}
}

bool DataPack::WriteXLS(string filename)
{
	Book* book = xlCreateBook();
    Sheet* sheet = book->addSheet("data");
	unsigned int row=1, col=0;
///////////////////////////////////////////////////////////////////////////////////////////////////////////
	sheet->writeStr(row, col, "Seq id");
    sheet->writeNum(row, col+1, Timing_seqid); row++;
	sheet->writeStr(row, col, "Loops");
    sheet->writeStr(row, col+1, Timing_loops); row++;

	sheet->writeStr(row, col, "x8 Coils");
	for(int i=0; i<8; i++) {//Coil currents
	     sheet->writeNum(row, col+1, Coil_Cur[i]); row++;
	   }

	sheet->writeStr(row, col, "HV Por");
    sheet->writeNum(row, col+1, HV_Por); row++;
	sheet->writeStr(row, col, "HV Vol");
    sheet->writeNum(row, col+1, HV_Vol); row++;
	sheet->writeStr(row, col, "HV Leak");
    sheet->writeNum(row, col+1, HV_Leak); row++;

	//Analog monitors
	sheet->writeStr(row, col, "Monitors");
	   for(unsigned int j=0; j<AMNumChls;j++)
	    {
		   for (unsigned int i=0; i<AMNumShots;i++)
		     {
			     sheet->writeNum(row+j, col+1+i, Ana_Mon[i][j]); 
		     }
	    }
	   row=row+AMNumChls;

	 //PDA
	 sheet->writeStr(row, col, "PDA");
	   for(int j=0;j<PDANpixel;j++)   
		{
			for(int k=0;k<PDANshot;k++)
			    {
				     sheet->writeNum(row+j, col+1+k, PDA_Signal[j][k]); 
			    }
	     }
	   row=row+PDANpixel;

	sheet->writeStr(row, col, "Temp");
    sheet->writeNum(row, col+1, Room_Temp); row++;
///////////////////////////////////////////////////////////////////////////////////////////////////////////
    book->save(filename.c_str());
    book->release(); 
	return true;
}


//Average (partially or fully) data from other packs
///////////////////////////////////////////////////////////////////////////////////////////////////////////
void DataPack::AddPac(DataPack *SecondPack)
{
	   //sum PDA signal
	   for(int j=0;j<PDANpixel;j++)   
		{
			for(int k=0;k<PDANshot;k++)
			    {
				     this->PDA_Signal[j][k]=this->PDA_Signal[j][k]+SecondPack->PDA_Signal[j][k]; 
			    }
	     }

	   	//sum Analog monitors
	   for(unsigned int j=0; j<AMNumChls;j++)
	    {
		   for (unsigned int i=0; i<AMNumShots;i++)
		     {
			    this->Ana_Mon[i][j]=this->Ana_Mon[i][j]+SecondPack->Ana_Mon[i][j];
		     }
	    }
}
void DataPack::DivPacByN(int NumOfPacs)//divide Pack data by N
{
	   //divN PDA signal
	   for(int j=0;j<PDANpixel;j++)   
		{
			for(int k=0;k<PDANshot;k++)
			    {
				      (this->PDA_Signal[j][k])/=(float)NumOfPacs; 
			    }
	     }
	   //divN Analog monitors
	   for(unsigned int j=0; j<AMNumChls;j++)
	    {
		   for (unsigned int i=0; i<AMNumShots;i++)
		     {
			    (this->Ana_Mon[i][j])/=(float)NumOfPacs;
		     }
	    }
}

#endif