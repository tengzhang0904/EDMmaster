// EDMSCANPlotDlg.cpp : implementation file
//

#include "stdafx.h"
#include "MASTER.h"
#include "EDMSCANPlotDlg.h"
#include "afxdialogex.h"



//Plot color
unsigned long PlotColor[8]={RGB(255,0,0),RGB(0,0,255),RGB(0,100,0),RGB(255,255,0),RGB(148,0,211),RGB(255,105,180),RGB(139,69,19), RGB(0,0,0),};
//                                 red          blue           green       yellow          purple           pink            brown        black                 

//fielname for analyzed bias scan data
string SelSCANFile="C:/MMC_SYSINT/LAB_DAT/EDMSCANAnalyzed.txt";



// EDMSCANPlotDlg dialog

IMPLEMENT_DYNAMIC(EDMSCANPlotDlg, CDialogEx)

EDMSCANPlotDlg::EDMSCANPlotDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(EDMSCANPlotDlg::IDD, pParent)
	, m_REDIT_EDMScanReport(_T(""))
	, m_Ramsey_fringe_selected(true)
	, m_ellipse_selected(true)
{
	Message="EDM SCAN REPORT";
	m_hIcon = AfxGetApp()->LoadIcon(IDI_ICON2);// set the icon
}

EDMSCANPlotDlg::~EDMSCANPlotDlg()
{
}

void EDMSCANPlotDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_RICHEDIT_EDMSCANRPT, m_REDIT_EDMScanReport);
	DDX_Check(pDX, IDC_Ramsey_fringe, m_Ramsey_fringe_selected);
	DDX_Check(pDX, IDC_Ellipse_plot, m_ellipse_selected);
	DDX_Control(pDX, IDC_Ramsey_Plot, m_Ramsey_Plot);
	DDX_Control(pDX, IDC_elliptical_plot, m_elliptical_plot);
}


BEGIN_MESSAGE_MAP(EDMSCANPlotDlg, CDialogEx)
	ON_BN_CLICKED(IDC_BUTT_EDMSCAN_PLOTANA, &EDMSCANPlotDlg::OnBnClickedButtEdmscanPlotana)
END_MESSAGE_MAP()

BOOL EDMSCANPlotDlg::OnInitDialog()
{   
	CDialogEx::OnInitDialog();
	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	return TRUE;
}


//read analyzed SCAN file
bool EDMSCANPlotDlg::ReadSCANana(string filename) 
{
	ScanData.clear();
	ifstream datalog;
	SelectPoint CurData;
    datalog.open (filename.c_str(), ios::in); 
	bool successflag=!(datalog.fail());
    if (successflag)
    {
	  datalog>>this->SCANnum;
	  for(unsigned int i=0;i<SCANnum;i++)   
	  {
		  for (int j=0; j<4; j++)
		  {
			datalog>>CurData.PTS[j];
		  }
			this->ScanData.push_back(CurData);
	  }
	  datalog.close();
	  return true;
	}
	else
	{
	  datalog.close();
	  return false;
	}
}

//Plot analyzed bias scan data
void EDMSCANPlotDlg::OnBnClickedButtEdmscanPlotana()
{  
	bool flag=this->ReadSCANana(SelSCANFile);
	if(flag)
	{
	   Message="EDM SCAN found: \n"+to_string(SCANnum)+" SHOTs";
	   this->PlotData(); //Plot graphs
	}
	else
	{
       Message=SelSCANFile+" NOT EXIST!";
	}
	m_REDIT_EDMScanReport=Message.c_str();
	UpdateData(false);
}

//Plot ScanData on two graphs
void EDMSCANPlotDlg::PlotData()
{
	//get plot options: Ramsey fringe vs elliptical plot
	UpdateData(); 

	const unsigned int MAXCUR=2; //maximal number of curves
	
	if(m_Ramsey_fringe_selected)
	{
		this->m_Ramsey_Plot.ClearGraph(); //clear old plots
		this->m_Ramsey_Plot.SetRange(0,SCANnum, -0.1, 1.1);
		this->m_Ramsey_Plot.SetXLabel("Scan index or Bz");
		this->m_Ramsey_Plot.SetYLabel("m=0 Signal");

		for(int j=0;j<MAXCUR;j++)
		{
			//Set properties of jth element
			if (j!=0)  //there is already a default one
			{
				this->m_Ramsey_Plot.AddElement();//add an element 
			}
			this->m_Ramsey_Plot.SetElement(j); 
			this->m_Ramsey_Plot.SetElementWidth(2);
			this->m_Ramsey_Plot.SetElementLineColor(PlotColor[j]);
			this->m_Ramsey_Plot.SetElementPointColor(PlotColor[j]);
			this->m_Ramsey_Plot.SetElementPointSymbol(j+1);
			this->m_Ramsey_Plot.SetElementSolidPoint(TRUE);
		}

	    for (unsigned int i=0;i<SCANnum;i++) 
		    for(unsigned int j=0;j<MAXCUR;j++)
                this->m_Ramsey_Plot.PlotXY(this->ScanData.at(i).PTS[0],this->ScanData.at(i).PTS[j+2],j);
	}
	
	
	if(m_ellipse_selected)
	{
		this->m_elliptical_plot.ClearGraph(); //clear old plots
		this->m_elliptical_plot.SetRange(-0.1, 1.1, -0.1, 1.1);
		this->m_elliptical_plot.SetXLabel("+dE");
		this->m_elliptical_plot.SetYLabel("-dE");

		for(int j=0;j<MAXCUR;j++)
		{
			//Set properties of jth element
			if (j!=0)  //there is already a default one
			{
				this->m_elliptical_plot.AddElement();//add an element 
			}
			this->m_elliptical_plot.SetElement(j); 
			this->m_elliptical_plot.SetElementWidth(3);
			this->m_elliptical_plot.SetElementLineColor(RGB(255,255,255));
			this->m_elliptical_plot.SetElementPointColor(PlotColor[j]);
			this->m_elliptical_plot.SetElementPointSymbol(j+1);
			this->m_elliptical_plot.SetElementSolidPoint(TRUE);
		}

	    for (unsigned int i=0;i<SCANnum;i++) 
		{
			//plotting +z vs -z
            //this->m_elliptical_plot.PlotXY(this->ScanData.at(i).PTS[2],this->ScanData.at(i).PTS[3],0);
			
			//plotting +E vs -E can be applied to high voltage switching pattern.
			
			if(this->ScanData.at(i).PTS[1]>0)
				this->m_elliptical_plot.PlotXY(this->ScanData.at(i).PTS[2],this->ScanData.at(i).PTS[3],0);
			else
				this->m_elliptical_plot.PlotXY(this->ScanData.at(i).PTS[3],this->ScanData.at(i).PTS[2],0);
			
		}
	}


	UpdateData(false);

}