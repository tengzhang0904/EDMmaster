//global function for task interfaces on MMC main panel
#pragma once

#ifndef _TaskInterface_h_included_
#define _TaskInterface_h_included_
#define _CRT_SECURE_NO_DEPRECATE

#include <string>
#include <math.h>
#include <vector>  //std::vector
#include <thread>  //C++11 multithreading
#include "MASTERDlg.h"

#include "DataPack.h"  //Data management
#include "EDMSignalPack.h" //EDM signal data pack, single SHOT
#include "EDMScanPack.h" //EDM SCAN data pack
#include "SpinPackSWP.h" //parent class for SpinPack
#include "SpinPackSWPDev.h" //multi-location spin pack for a scan: child class

#include "DEVICE.h" //Device header files
using namespace std;
using namespace DATACONST;
using namespace DEVICEHEADER;

#include "ALGLIB/NLSF_MT.h"//Mathematical operations: integration, nonlinear fit, etc.

//misc functions
///////////////////////////////////////////////////////////////////////////////////////////////////////////
//convert a number to string
template <typename T> 
std::string to_string(T const& value) 
{
    stringstream sstr;
    sstr << value;
    return sstr.str();
}

//convert a number to string (size=3)
string intostr3(unsigned int value) 
{
    string str=to_string(value);
    while(str.size()<3)
	{
       str="0"+str;
	}
    return str;
}

//Write matrix to a txt file
bool TxtMatrix(string filename, double** mat, int rowS, int colS)
{
	ofstream datalog(filename.c_str(),ios::out | ios::trunc); //delete previous log then write the new one
	for(int j=0;j<rowS;j++)   
	{
		for(int k=0;k<colS;k++)
			 {
				     datalog<<mat[j][k]<<" ";
			 }
			datalog<<endl;
	     }
    datalog.close();
	return true;
}

//Pause the dialog for some milliseconds
void DialogPause(unsigned int ms)
{
	clock_t goal0 = ms + clock(); 
	while (goal0 > clock()); 
}


//global function for stand-alone tasks
///////////////////////////////////////////////////////////////////////////////////////////////////////////
void DoNothing(int opt) //test function
{
	//do nothing 
}

void TempgetShow(CMASTERDlg *MasterDlg) //Get and display temperature
{   
	//will be replaced by a device measurement
	MasterDlg->m_TEMPERATURE.Format("%.2f", 23.50); 
}

void Plot2D(CMASTERDlg *MasterDlg) //Plot a curve from DataPack
{
  MasterDlg->m_GRAPH_PDA.SetRange(0,50, -0.02, 1.00); //set plot range

  const unsigned int MAXCUR=8; //maximal number of curves
  unsigned long PlotColor[MAXCUR]={RGB(0,0,0),RGB(255,0,0),RGB(0,0,255),RGB(0,100,0),RGB(255,255,0),RGB(148,0,211),RGB(255,105,180),RGB(139,69,19)};
  //                                black       red          blue           green       yellow          purple           pink            brown                    

  MasterDlg->m_GRAPH_PDA.ClearGraph(); //clear old plots
  for(int j=0;j<MAXCUR;j++)
  {
	    //Set properties of jth element
	    if (j!=0)  //there is already a default one
		{
			MasterDlg->m_GRAPH_PDA.AddElement();//add an element 
		}
        MasterDlg->m_GRAPH_PDA.SetElement(j); 
        MasterDlg->m_GRAPH_PDA.SetElementWidth(2);
        MasterDlg->m_GRAPH_PDA.SetElementLineColor(PlotColor[j]);
        MasterDlg->m_GRAPH_PDA.SetElementPointColor(PlotColor[j]);
        MasterDlg->m_GRAPH_PDA.SetElementPointSymbol(j+1);
        MasterDlg->m_GRAPH_PDA.SetElementSolidPoint(TRUE);
  }

  //plot curve data
  for (int i=0;i<PDANpixel;i++)
    {   
		for(int k=0;k<PDANshot;k++)
		{
			if (k!=BKGST) //Does not plot backround
                MasterDlg->m_GRAPH_PDA.PlotY(MasterDlg->Curve[i][k],k);
		}
    }
}

//DAQ with PDA and background/offset subtraction
void PDAgetShow(CMASTERDlg *MasterDlg) //PDA imaging and show
{
  PDA *PDAcq=new PDA();

  PDAcq->Acquire(30); //sec timeout

  //PDAcq->GenerateTestData();
  //const string PDA_Data=MMCfolder+"PDA/"+"PDA_Data.txt"; //PDA DATA FILENAME
  //PDAcq->WriteDataFile(PDA_Data); //save raw data

  double offsets[PDANshot]; //array to store overall offset

  //substract YAG BKG
  for (int i=0;i<PDANpixel;i++)
  {	    
	  for(int k=0;k<PDANshot;k++)
	  {  
		  if (k!=BKGST)
		  {
	          PDAcq->Data[i][k]=PDAcq->Data[i][k]-PDAcq->Data[i][BKGST]; 
		  }
	  }
  }

  //calculate overall curve offsets
  for(int k=0;k<PDANshot;k++)
  {
	  offsets[k]=0.0;
	  if (k!=BKGST)
	  {
		  for (int i=1;i<=25;i++)
		  {
			  offsets[k]+=PDAcq->Data[24+i][k];  //use the 5 points where there is no atom signal but flucuating YAG 
			  //offsets[k]+=PDAcq->Data[i+25][k];
		  }
		  offsets[k]/=25.0;
	  }
  }

  //remove overall offsets and save as final data
  for (int i=0;i<PDANpixel;i++)
  {	    
	  for(int k=0;k<PDANshot;k++)
	  {  
		  /*if (k!=BKGST)
		  {
	          PDAcq->Data[i][k]=PDAcq->Data[i][k];//-offsets[k]; 
		  }*/
	      MasterDlg->Curve[i][k]=(float)PDAcq->Data[i][k]; //save kth curve for master
	  }
  }

  Plot2D(MasterDlg);
  delete PDAcq;
}

//Read sample EDM data (LF Transition simulations)
void PDAgetShowEDM(CMASTERDlg *MasterDlg, unsigned int shotID)
{
  PDA *PDAcq=new PDA();
  PDAcq->ReadEDMTestData(shotID);
  for (int i=0;i<PDANpixel;i++)
  {	    
	  for(int k=0;k<PDANshot;k++)
	  {  
	      MasterDlg->Curve[i][k]=(float)PDAcq->Data[i][k];   //save data to MMC panel
	  }
  }
  Plot2D(MasterDlg);
  delete PDAcq;
}

//Plot a curve from DataPack, EDMSignalPack
void LoadDataToPanel(CMASTERDlg *MasterDlg, string filename) 
{
    DataPack *CurrentPack=new DataPack();
	CurrentPack->init();
	bool flag=CurrentPack->ReadTXT(filename); 
  if (flag)
  {
    //Timing 
	MasterDlg->Sequence_id=CurrentPack->Timing_seqid; 
	MasterDlg->loopnum=CurrentPack->Timing_loops;

	//High voltage
	MasterDlg->m_HVVOL.Format("%.2f", CurrentPack->HV_Vol);
	MasterDlg->m_HVLEAK.Format("%.1f", CurrentPack->HV_Leak);

	//Analog monitors
	MasterDlg->m_CAVITYP.Format("%.6f", CurrentPack->Ana_Mon[0][0]);
	MasterDlg->m_CAVITYN.Format("%.6f", CurrentPack->Ana_Mon[0][1]);
	MasterDlg->m_EXTBX.Format("%.6f", CurrentPack->Ana_Mon[0][2]);
	MasterDlg->m_EXTBY.Format("%.6f", CurrentPack->Ana_Mon[0][3]);
	MasterDlg->m_EXTBZ.Format("%.6f", CurrentPack->Ana_Mon[0][4]);
	MasterDlg->m_BEAM1.Format("%.6f", CurrentPack->Ana_Mon[0][5]);
	MasterDlg->m_BEAM2.Format("%.6f", CurrentPack->Ana_Mon[0][6]);
	MasterDlg->m_BEAM3.Format("%.6f", CurrentPack->Ana_Mon[0][7]);

	//Other
	MasterDlg->m_TEMPERATURE.Format("%.2f", CurrentPack->Room_Temp); 

    for (int i=0;i<PDANpixel;i++)
      {	 
		  for(int k=0;k<PDANshot;k++)
		  {
	          MasterDlg->Curve[i][k]=CurrentPack->PDA_Signal[i][k];
		  }
	  }
	Plot2D(MasterDlg);

  }//end of flag

	else
	{
	  string openfolder=MMCfolder+"LAB_DAT/";
	  MasterDlg->m_BULLETIN=_T("File not found.");
	  ShellExecute(NULL,NULL,openfolder.c_str(),NULL,NULL,SW_SHOWNORMAL);
	}

	delete CurrentPack;
}

void MonitorgetShow(CMASTERDlg *MasterDlg, unsigned int ids) //Analog monitors measure and display
{
	Amonitors *MonitorA=new Amonitors();
	MonitorA->init(); 
	for (unsigned int k=0;k<ids;k++)
	{
	   MonitorA->Measure(k);
	}

    //const string monitorExcelFile=MMCfolder+"MonitorsAI/"+"MonitorsAI.xls";
	//MonitorA->WriteXLS(monitorExcelFile);

	MasterDlg->m_CAVITYP.Format("%.6f", MonitorA->data[0][0]); // Top Picoammeter
	MasterDlg->m_CAVITYN.Format("%.6f", MonitorA->data[0][1]); // Bottom Picoammeter
	MasterDlg->m_EXTBX.Format("%.6f", MonitorA->data[0][2]);  // -Z monitor 
	MasterDlg->m_EXTBY.Format("%.6f", MonitorA->data[1][2]);
	MasterDlg->m_EXTBZ.Format("%.6f", MonitorA->data[2][2]);
	MasterDlg->m_BEAM1.Format("%.6f", MonitorA->data[3][2]);
	MasterDlg->m_BEAM2.Format("%.6f", MonitorA->data[4][2]);
	MasterDlg->m_BEAM3.Format("%.6f", MonitorA->data[0][4]);// +Z monitor
	delete MonitorA; 
}

void HVgetShow(CMASTERDlg *MasterDlg, int opt) //HV voltage and leakage measure and display
{   
	float HVdata, HVleak;
	switch(opt)
	{
	  case 1:
		     HVdata=HVDivS();
	         MasterDlg->m_HVVOL.Format("%.2f", HVdata);
		     break;
	  case 2:
		  	 HVleak=(float)52.8; //will be replaced by a device measurement
	         MasterDlg->m_HVLEAK.Format("%.1f", HVleak);
			 break;
	  case 3:
		     HVdata=HVDivS();
	         MasterDlg->m_HVVOL.Format("%.2f", HVdata);
		  	 HVleak=(float)30.8; //will be replaced by a device measurement
	         MasterDlg->m_HVLEAK.Format("%.1f", HVleak);
			 break;
	  default: break;
	}
}

//Calculate value at a given sweep step
double SweepValueAt(int k, double xi, double xf, int N, int type)
{
	double value=0.0;
	switch (type)
	{
	  case 0: //Linear sweep
		  value=xi+(xf-xi)*((double)k)/((double)N);
		break;
	  case 1: //Log (power law) sweep
		  if (xi!=0.0)
		  {
			  value=xi*pow(xf/xi, ((double)k)/((double)N));
		  }
		break;
	  default:
		break;
	}
	return value;
}

//Set value (write to an excel file) at a given sweep step
bool SweepSetValue(int fileID, string cell, double value)
{
  if (fileID<3)
  {
	string filename[3]={
		MMCfolder+"CurrentSource/Settings.xls",
		MMCfolder+"DDS/PulseTrainParameters.xls", 
		MMCfolder+"AudioTrans/AudioPulse.xls"};

	//compute cell location (row, col)
	int row=0, col=0;
	char colLetter=cell[0];
	string rowNum=cell.erase(0,1);
	row=atoi(rowNum.c_str())-1; //
	col=static_cast<int>(colLetter)-65; //"A"=65

    Book* book = xlCreateBook();
	book->load(filename[fileID].c_str());
    Sheet* sheet = book->getSheet(0);
    sheet->writeNum(row, col, value);
	if(fileID==1)
	{
		sheet->writeNum(row+1, col, value);// Ramsey fringe needs 2 frequency change at a time
	}
    book->save(filename[fileID].c_str());
    book->release(); 

    return true;
   }

   else //para file not found or not necessary
	return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////


#endif
