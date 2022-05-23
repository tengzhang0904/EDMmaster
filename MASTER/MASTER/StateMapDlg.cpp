// StateMapDlg.cpp : implementation file
//

#include "stdafx.h"
#include "MASTER.h"
#include "StateMapDlg.h"
#include "afxdialogex.h"

// StateMapDlg dialog

IMPLEMENT_DYNAMIC(StateMapDlg, CDialogEx)

StateMapDlg::StateMapDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(StateMapDlg::IDD, pParent)
	, m_StateMapMessage(_T("Info regarding state poupulation..."))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDI_ICON2);// set the icon

#ifndef _WIN32_WCE
	EnableActiveAccessibility();
#endif
    
}

StateMapDlg::~StateMapDlg()
{
}

void StateMapDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_MAP_MESSAGE, m_StateMapMessage);
	DDX_Control(pDX, IDC_STATEMAPGRAPH, m_Graph_StateMap);
}


BEGIN_MESSAGE_MAP(StateMapDlg, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON_PLOTMAP, &StateMapDlg::OnBnClickedButtonPlotmap)
END_MESSAGE_MAP()

BOOL StateMapDlg::OnInitDialog()
{   
	CDialogEx::OnInitDialog();
	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	return TRUE;
}

//Plot state population map
void StateMapDlg::OnBnClickedButtonPlotmap()
{
	//set dialog window position tp upper left
	SetWindowPos(NULL, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOZORDER);

	//READ ORIGINAL DATA FROM  AVGlog.txt file
	double InteData[3][PDANshot]; //STORE ORIGINAL INTEGRATED DATA
	double PZsum=0.0, NZsum=0.0, PZSPIN=0.0, NZSPIN=0.0; //total signal and spin
	string message="";
	string MMCfolder="C:/MMC_SYSINT/";
    string avglogfile=MMCfolder+"LAB_DAT/"+"Integrated.txt";
	bool successflag;
	ifstream datalog;
    datalog.open (avglogfile.c_str(), ios::in); 
	successflag=!(datalog.fail());
    if (successflag)
    {
	    for (int i=0; i<PDANshot; i++)
		{
		   datalog>>InteData[0][i]; //index
		   datalog>>InteData[1][i]; //+Z
		   datalog>>InteData[2][i];  //-Z
		}
		datalog.close();
	}
	else
	{
		for (int i=0; i<PDANshot; i++)
		{
		   InteData[0][i]=i; //index
		   InteData[1][i]=1.0; //+Z
		   InteData[2][i]=1.0;  //-Z
		}
		datalog.close();
	}

	//compute total signal size
	for (int i=0; i<PDANshot; i++)
	{
		if (i!=BKGST)
		{
		   PZsum+=InteData[1][i];
		   NZsum+=InteData[2][i];
		}
	}

    //NTGraph init
	m_Graph_StateMap.ClearGraph(); //clear old plots
	m_Graph_StateMap.SetFrameStyle(5);
	m_Graph_StateMap.SetXLabel("mF");
	m_Graph_StateMap.SetYLabel("Nm/N0");
	m_Graph_StateMap.SetRange(-4, 4,  -0.05, 0.55);
	m_Graph_StateMap.SetXGridNumber(8);
	m_Graph_StateMap.SetYGridNumber(8);

	m_Graph_StateMap.SetElement(0); 
    m_Graph_StateMap.SetElementLineColor(RGB(255,0,0));
    m_Graph_StateMap.SetElementLinetype(6);
	m_Graph_StateMap.SetElementWidth(2);

	m_Graph_StateMap.AddElement();
	m_Graph_StateMap.SetElement(1); 
	m_Graph_StateMap.SetElementLineColor(RGB(0,0,255));
    m_Graph_StateMap.SetElementLinetype(6);
	m_Graph_StateMap.SetElementWidth(2);

	const unsigned int Nlevel=9;
	double mF[Nlevel]={4,3,2,1,0,-1,-2,-3,-4};
	double pnz[2][Nlevel];

	
	if (PDANshot>=8)
	{  
	   //normalize the signal w.r.t. total signal
	   for (int j=0;j<=PDANshot-2;j++)
	   {
		   pnz[0][j+1]=InteData[1][j]/PZsum;
		   pnz[1][j+1]=InteData[2][j]/NZsum;
		   PZSPIN+=pnz[0][j+1]*mF[j+1];
		   NZSPIN+=pnz[1][j+1]*mF[j+1];
	   }
	}
	else
	{
	   PZsum=0.0; NZsum=0.0;
	   //plot the fake test data
	   pnz[0][1]=0.05;pnz[1][1]=0.15;
	   pnz[0][2]=0.15;pnz[1][2]=0.25;
	   pnz[0][3]=0.45;pnz[1][3]=0.55;
	   pnz[0][4]=0.75;pnz[1][4]=0.25;
	   pnz[0][5]=0.45;pnz[1][5]=0.55;
	   pnz[0][6]=0.15;pnz[1][6]=0.25;
	   pnz[0][7]=0.05;pnz[1][7]=0.15;
	}
	

	for(int k=0;k<Nlevel;k++)
	{
	   mF[k]=mF[k]+0.5; //center step plot on mF index
	   pnz[0][0]=0.0;pnz[0][8]=0.0;pnz[1][0]=0.0;pnz[1][8]=0.0; //Null points
	   m_Graph_StateMap.PlotXY(mF[k], pnz[0][k], 0); //+Z
	   m_Graph_StateMap.PlotXY(mF[k], pnz[1][k], 1); //-Z
	}


	//output total signal size and avgerage spin
	message+="Total Signal\n"+to_string(PZsum)+"   "+to_string(NZsum)+"\n";
	message+="Total Spin\n"+to_string(PZSPIN)+"   "+to_string(NZSPIN)+"\n";
	m_StateMapMessage=message.c_str();
	this->UpdateData(FALSE);

}