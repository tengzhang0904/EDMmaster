// MASTERDlg.cpp : implementation file

#include "stdafx.h"
#include "MASTER.h"
#include "MASTERDlg.h"
#include "afxdialogex.h"
#include <string>
#include <math.h>
#include <vector>  //std::vector
#include <thread>  //C++11 multithreading

#include "ParaSweepDlg.h" //Parameter sweep dialog
#include "StateMapDlg.h" //Plot population map
#include "MagnetometryDlg.h"//Plot magnetometry: bias scan zero crossing
#include "MagnetometryGradDlg.h"//Plot magnetometry: gradient scan zero crossing
#include "EDMSCANPlotDlg.h" //Plot EDN SCAN data
#include "LoopCtrlUIDlg.h" //Loop control user interface dialog

#include "DataPack.h"  //Data management
#include "EDMSignalPack.h" //EDM signal data pack, single SHOT
#include "EDMScanPack.h" //EDM SCAN data pack
#include "HVpolarity.h" //Class for HV polarity pattern
#include "SpinPackSWPDev.h" //multi-location spin pack for a scan: child class

#include "DEVICE.h" //Device header files
#include "TaskInterface.h" //MMC task interfaces
#include "ALGLIB/NLSF_MT.h"//Mathematical operations: integration, nonlinear fit, etc.

using namespace DATACONST;
using namespace DEVICEHEADER;


///////////////////////////////////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
public:

};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)

END_MESSAGE_MAP()
///////////////////////////////////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////////////////////////////////
// CMASTERDlg dialog
const CString MMCfolderC=MMCfolder.c_str();

CMASTERDlg::CMASTERDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CMASTERDlg::IDD, pParent)
	, Sequence_id(0)
	, loopnum(_T("003"))
	, m_HVVOL(_T(""))
	, m_HVLEAK(_T(""))
	, m_CAVITYP(_T(""))
	, m_CAVITYN(_T(""))
	, m_EXTBX(_T(""))
	, m_EXTBY(_T(""))
	, m_EXTBZ(_T(""))
	, m_LASERLOCKED(TRUE)
	, m_TEMPERATURE(_T(""))
	, m_BEAM1(_T(""))
	, m_BEAM2(_T(""))
	, m_BEAM3(_T(""))
	, m_Parallel_Audio(FALSE)
	, m_Parallel_DDS(FALSE)
	, m_Parallel_HV(FALSE)
	, m_Parallel_Monitor(FALSE)
	, m_Parallel_PDA(FALSE)
	, m_Parallel_ExtField(FALSE)
	, m_Parallel_Supertime(FALSE)
	, m_Parallel_Temp(FALSE)
	, m_DATASET_FILE(_T(""))
	, m_BULLETIN(_T("Status messages may be updated here..."))
	, m_PACIDXstart(0)
	, m_PACIDXend(2)
	, m_RICHEDIT_REPORT(_T("Preliminary report will be posted here..."))
	, m_Condition_A(0)
	, m_Condition_B(1)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	SWPPARA[0]=0.0; SWPPARA[1]=1.0; SWPPARA[2]=2; SWPPARA[3]=0;
	MAG_Setting.assign(9,0.0);
}

void CMASTERDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_CBIndex(pDX, IDC_COMBOSEQUENCE, Sequence_id);
	DDV_MinMaxInt(pDX, Sequence_id, 0, 100);
	DDX_Text(pDX, IDC_EDITLOOPS, loopnum);
	DDX_Control(pDX, IDC_COMBOSEQUENCE, m_SEQNAME);
	DDX_Text(pDX, IDC_HVvol, m_HVVOL);
	DDX_Text(pDX, IDC_HVleak, m_HVLEAK);
	DDX_Text(pDX, IDC_EDIT_CAVITYP, m_CAVITYP);
	DDX_Text(pDX, IDC_EDIT_CAVITYN, m_CAVITYN);
	DDX_Text(pDX, IDC_EDIT_EXTBX, m_EXTBX);
	DDX_Text(pDX, IDC_EDIT_EXTBY, m_EXTBY);
	DDX_Text(pDX, IDC_EDIT_EXTBZ, m_EXTBZ);
	DDX_Check(pDX, IDC_CHECK_LASER, m_LASERLOCKED);
	DDX_Text(pDX, IDC_EDIT_TEMP, m_TEMPERATURE);
	DDX_Text(pDX, IDC_EDIT_BEAM1, m_BEAM1);
	DDX_Text(pDX, IDC_EDIT_BEAM2, m_BEAM2);
	DDX_Text(pDX, IDC_EDIT_BEAM3, m_BEAM3);
	DDX_Control(pDX, IDC_NTGRAPHCTRL, m_GRAPH_PDA);
	DDX_Check(pDX, IDC_CHECKAudio, m_Parallel_Audio);
	DDX_Check(pDX, IDC_CHECKDDS, m_Parallel_DDS);
	DDX_Check(pDX, IDC_CHECKHV, m_Parallel_HV);
	DDX_Check(pDX, IDC_CHECKMonitor, m_Parallel_Monitor);
	DDX_Check(pDX, IDC_CHECKPDA, m_Parallel_PDA);
	DDX_Check(pDX, IDC_CHECKPulnix, m_Parallel_ExtField);
	DDX_Check(pDX, IDC_CHECKSupertime, m_Parallel_Supertime);
	DDX_Check(pDX, IDC_CHECKTemp, m_Parallel_Temp);
	DDX_Text(pDX, IDC_EDIT_DATASETFILE, m_DATASET_FILE);
	DDX_Control(pDX, IDC_PROGRESSSWP, m_SWPPROGRESSCtrl);
	DDX_Text(pDX, IDC_EDIT_BULLETIN, m_BULLETIN);
	DDX_Text(pDX, IDC_EDIT_PACIDXstart, m_PACIDXstart);
	DDX_Text(pDX, IDC_EDIT_PACIDXend, m_PACIDXend);
	DDX_Control(pDX, IDC_RICHEDIT_ANARPT, m_RICHEDIT_RPTCTRL);
	DDX_Text(pDX, IDC_RICHEDIT_ANARPT, m_RICHEDIT_REPORT);
	DDX_Text(pDX, IDC_EDIT_CONDITION_A, m_Condition_A);
	DDX_Text(pDX, IDC_EDIT_CONDITION_B, m_Condition_B);
}

BEGIN_MESSAGE_MAP(CMASTERDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_COMMAND(ID_USEFULLINKS_MFC, &CMASTERDlg::OnUsefullinksMfc)
	ON_COMMAND(ID_USEFULLINKS_EXCELLINK, &CMASTERDlg::OnUsefullinksExcellink)
	ON_COMMAND(ID_USEFULLINKS_WINSOCK, &CMASTERDlg::OnUsefullinksWinsock)
	ON_COMMAND(ID_USEFULLINKS_NIDAQmx, &CMASTERDlg::OnUsefullinksNidaqmx)
	ON_COMMAND(ID_USEFULLINKS_NIVISA, &CMASTERDlg::OnUsefullinksNivisa)
	ON_COMMAND(ID_HELP_SOURCECODE, &CMASTERDlg::OnHelpSourcecode)
	ON_COMMAND(ID_HELP_DOCUMENTATION, &CMASTERDlg::OnHelpDocumentation)
	ON_BN_CLICKED(IDC_BUTTONMicrowave, &CMASTERDlg::OnBnClickedButtonmicrowave)
	ON_BN_CLICKED(IDC_BUTTONAudioTrans, &CMASTERDlg::OnBnClickedButtonaudiotrans)
	ON_BN_CLICKED(IDC_BUTTONCurrentSource, &CMASTERDlg::OnBnClickedButtoncurrentsource)
	ON_BN_CLICKED(IDC_BUTTONDigital, &CMASTERDlg::OnBnClickedButtondigital)
	ON_COMMAND(ID_ATOMIC_SUPERTIMECOMMUNICATION32839, &CMASTERDlg::OnAtomicSupertimecommunication)
	ON_COMMAND(ID_MICROWAVEDDS_CALCULATEDDSDATA, &CMASTERDlg::OnMicrowaveddsCalculateddsdata)
	ON_COMMAND(ID_MICROWAVEDDS_OUTPUTDDSDATAONCE, &CMASTERDlg::OnMicrowaveddsOutputddsdataonce)
	ON_COMMAND(ID_MICROWAVEDDS_CALCULATEANDOUTPUT, &CMASTERDlg::OnMicrowaveddsCalculateandoutput)
	ON_COMMAND(ID_ATOMIC_AUDIOTRANSITIONS32850, &CMASTERDlg::OnAtomicAudiotransitions)
	ON_COMMAND(ID_ATOMIC_UPDATEASYNDIGITALLINES, &CMASTERDlg::OnAtomicUpdateasyndigitallines)
	ON_COMMAND(ID_HIGHVOLTAGE_MONITORVOLTAGE, &CMASTERDlg::OnHighvoltageMonitorvoltage)
	ON_COMMAND(ID_HIGHVOLTAGE_MONITORLEAKAGECURRENT, &CMASTERDlg::OnHighvoltageMonitorleakagecurrent)
	ON_COMMAND(ID_SINGLE_ANALOGMONITORS, &CMASTERDlg::OnSingleAnalogmonitors)
	ON_EN_CHANGE(IDC_EDITLOOPS, &CMASTERDlg::OnEnChangeEditloops)
	ON_CBN_SELCHANGE(IDC_COMBOSEQUENCE, &CMASTERDlg::OnCbnSelchangeCombosequence)
	ON_BN_CLICKED(IDC_CHECK_LASER, &CMASTERDlg::OnBnClickedCheckLaser)
	ON_COMMAND(ID_SINGLE_TEMPERATURE, &CMASTERDlg::OnSingleTemperature)
	ON_COMMAND(ID_PROTOCOLS_NTGRAPH, &CMASTERDlg::OnProtocolsNtgraph)
	ON_COMMAND(ID_SINGLE_PDAIMAGING, &CMASTERDlg::OnSinglePdaimaging)
	ON_BN_CLICKED(IDC_BUTTON_PARASWEEP, &CMASTERDlg::OnBnClickedButtonParasweep)
	ON_COMMAND(ID_PROTOCOLS_ALGLIB, &CMASTERDlg::OnProtocolsALGLIB)
	ON_BN_CLICKED(IDC_CHECKDDS, &CMASTERDlg::OnBnClickedCheckdds)
	ON_BN_CLICKED(IDC_CHECKPDA, &CMASTERDlg::OnBnClickedCheckpda)
	ON_BN_CLICKED(IDC_CHECKAudio, &CMASTERDlg::OnBnClickedCheckaudio)
	ON_BN_CLICKED(IDC_CHECKHV, &CMASTERDlg::OnBnClickedCheckhv)
	ON_BN_CLICKED(IDC_CHECKSupertime, &CMASTERDlg::OnBnClickedChecksupertime)
	ON_BN_CLICKED(IDC_CHECKMonitor, &CMASTERDlg::OnBnClickedCheckmonitor)
	ON_BN_CLICKED(IDC_CHECKTemp, &CMASTERDlg::OnBnClickedChecktemp)
	ON_BN_CLICKED(IDC_CHECKPulnix, &CMASTERDlg::OnBnClickedCheckExtField)
	ON_COMMAND(ID_COMPOSITE_PARALLELTASKING, &CMASTERDlg::OnCompositeParalleltasking)
	ON_COMMAND(ID_DATABASE_SAVE, &CMASTERDlg::OnDatabaseSave)
	ON_COMMAND(ID_DATABASE_LOADPANELDATA, &CMASTERDlg::OnDatabaseLoadpaneldata)
	ON_BN_CLICKED(IDC_BUTTON_ConvertMAT, &CMASTERDlg::OnBnClickedButtonConvertmat)
	ON_BN_CLICKED(IDC_BUTTON_CONVERTXLS, &CMASTERDlg::OnBnClickedButtonConvertxls)
	ON_COMMAND(ID_Menu_PACKAVG, &CMASTERDlg::OnMenuPackavg)
	ON_COMMAND(ID_ATOMIC_CURRENTSOURCE, &CMASTERDlg::OnAtomicCurrentsource)
	ON_COMMAND(ID_ATOMIC_HARDWAREPATTERNMATCH32873, &CMASTERDlg::OnAtomicHardwarepatternmatch)
	ON_COMMAND(ID_DATABASE_DATAPACKLISTING, &CMASTERDlg::OnDatapacklisting)
	ON_COMMAND(ID_MAGNETOMETRYBiasScan, &CMASTERDlg::OnCompositeMagnetometryBiasScan)
	ON_COMMAND(ID_MagnetometryGradScan, &CMASTERDlg::OnMenuMagnetometryGradScan)
	ON_COMMAND(ID_COMPOSITE_LOADSPINDATASWEEP, &CMASTERDlg::OnCompositeLoadspindatasweep)
	ON_COMMAND(ID_COMPOSITE_ANALYZEGRAD, &CMASTERDlg::OnCompositeAnalyzegrad)
	ON_COMMAND(ID_COMPOSITE_SPINSWEEPTRUNCATE, &CMASTERDlg::OnCompositeSpinsweeptruncate)
	ON_COMMAND(ID_ATOMIC_PAUSETIMING, &CMASTERDlg::OnAtomicPausetiming)
	ON_COMMAND(ID_Menu32872, &CMASTERDlg::TurnOn1MHzCLK)
	ON_COMMAND(ID_COMPOSITE_EDM, &CMASTERDlg::OnCompositeEdmSingleScan)
	ON_COMMAND(ID_ANALYZE_VIEWEDMSCANDATA, &CMASTERDlg::OnAnalyzeViewedmscandata)
	ON_COMMAND(ID_COMPOSITE_AUTOMATEDEDM, &CMASTERDlg::OnCompositeAutomatededm)
	ON_COMMAND(ID_COMPOSITE_AUTOMAGNETOMETRY, &CMASTERDlg::OnCompositeAutomagnetometry)
END_MESSAGE_MAP()


// CMASTERDlg message handlers

BOOL CMASTERDlg::OnInitDialog()
{   
	CDialogEx::OnInitDialog();
	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	/////////////////////////////////////////////////////////////////////////////////////////////
	//Add sequence names
	m_SEQNAME.AddString("PzMz");
	m_SEQNAME.AddString("PzMx");
	m_SEQNAME.AddString("PyMz");
	m_SEQNAME.AddString("PyMx");
	m_SEQNAME.AddString("PzMz");
	m_SEQNAME.SetCurSel(0); //default choice

	//NTGraph init
	m_GRAPH_PDA.SetFrameStyle(5);
	m_GRAPH_PDA.SetXLabel("Pixels  -Z[1,25]  +Z[25,50]");
    m_GRAPH_PDA.SetYLabel("Atom Signal (a.u.)");
	m_GRAPH_PDA.SetCaption("   ");
	m_GRAPH_PDA.AutoRange();

	//Sweep progress bar
	m_SWPPROGRESSCtrl.SetBkColor(RGB(216, 191, 216));
    m_SWPPROGRESSCtrl.SetBarColor(RGB(148, 0, 211));

	//Analysis report 
	m_RICHEDIT_RPTCTRL.SetBackgroundColor(false,RGB(238, 221, 130));

	//Initiation of data array
	for (int i=0;i<PDANpixel;i++)
		for (int j=0;j<PDANshot;j++)
	        Curve[i][j]=0.0;

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CMASTERDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CMASTERDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CMASTERDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////





///////////////////////////////////////////////////////////////////////////////////////////////////////////
//The following code lock current value of indicators 
//and will not change to default value when other indicators update 

void CMASTERDlg::OnEnChangeEditloops()
{
	UpdateData();
	loopnum=CStringA(loopnum);
}

void CMASTERDlg::OnCbnSelchangeCombosequence()
{
	UpdateData();
	Sequence_id=Sequence_id;
}

void CMASTERDlg::OnBnClickedCheckLaser()
{
	UpdateData();
	m_LASERLOCKED=m_LASERLOCKED;
}


//Parallel tasking selections

void CMASTERDlg::OnBnClickedCheckdds()
{   
	UpdateData();
	m_Parallel_DDS=m_Parallel_DDS;
}

void CMASTERDlg::OnBnClickedCheckpda()
{
	UpdateData();
	m_Parallel_PDA=m_Parallel_PDA;
}

void CMASTERDlg::OnBnClickedCheckaudio()
{
	UpdateData();
	m_Parallel_Audio=m_Parallel_Audio;
}

void CMASTERDlg::OnBnClickedCheckhv()
{
	UpdateData();
	m_Parallel_HV=m_Parallel_HV;
}

void CMASTERDlg::OnBnClickedChecksupertime()
{
	UpdateData();
	m_Parallel_Supertime=m_Parallel_Supertime;
}

void CMASTERDlg::OnBnClickedCheckmonitor()
{
    UpdateData();
	m_Parallel_Monitor=m_Parallel_Monitor;
}

void CMASTERDlg::OnBnClickedChecktemp()
{
	UpdateData();
	m_Parallel_Temp=m_Parallel_Temp;
}

void CMASTERDlg::OnBnClickedCheckExtField()
{
	UpdateData();
	m_Parallel_ExtField=m_Parallel_ExtField;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////




//Implementation for help menu
///////////////////////////////////////////////////////////////////////////////////////////////////////////
void CMASTERDlg::OnUsefullinksMfc()
{
	ShellExecute(NULL,NULL,_T("http://www.functionx.com/visualc/"),NULL,NULL,SW_SHOWNORMAL);
}

void CMASTERDlg::OnUsefullinksExcellink()
{
	ShellExecute(NULL,NULL,_T("http://www.libxl.com/"),NULL,NULL,SW_SHOWNORMAL);
}

void CMASTERDlg::OnUsefullinksWinsock()
{
	ShellExecute(NULL,NULL,_T("http://msdn.microsoft.com/en-us/library/ms740673(v=vs.85).aspx"),NULL,NULL,SW_SHOWNORMAL);
}

void CMASTERDlg::OnUsefullinksNidaqmx()
{
	ShellExecute(NULL,NULL,_T("http://www.ni.com/dataacquisition/nidaqmx.htm"),NULL,NULL,SW_SHOWNORMAL);
}

void CMASTERDlg::OnUsefullinksNivisa()
{
	ShellExecute(NULL,NULL,_T("http://www.ni.com/visa/"),NULL,NULL,SW_SHOWNORMAL);
}

void CMASTERDlg::OnProtocolsNtgraph()
{
	ShellExecute(NULL,NULL,_T("http://www.codeproject.com/Articles/3214/2D-Graph-ActiveX-Control"),NULL,NULL,SW_SHOWNORMAL);
}

void CMASTERDlg::OnProtocolsALGLIB()
{
	ShellExecute(NULL,NULL,_T("http://www.alglib.net/"),NULL,NULL,SW_SHOWNORMAL);
}

void CMASTERDlg::OnHelpSourcecode()
{
	//system("start explorer C:/Users/nounbou/Desktop/");
	ShellExecute(NULL,NULL,MMCfolderC,NULL,NULL,SW_SHOWNORMAL);
}

void CMASTERDlg::OnHelpDocumentation()
{
	ShellExecute(NULL,NULL,_T("https://amo.phys.psu.edu/WeissLabWiki/index.php/EDM:EDM_Experiment"),NULL,NULL,SW_SHOWNORMAL);
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////




//Implementation for Single tasks
///////////////////////////////////////////////////////////////////////////////////////////////////////////
void CMASTERDlg::OnBnClickedButtonmicrowave()
{
	ShellExecute(NULL,NULL,MMCfolderC+_T("/DDS/PulseTrainParameters.xls"),NULL,NULL,SW_SHOWNORMAL);
}

void CMASTERDlg::OnBnClickedButtonaudiotrans()
{
	ShellExecute(NULL,NULL,MMCfolderC+_T("/AudioTrans/AudioPulse.xls"),NULL,NULL,SW_SHOWNORMAL);
}

void CMASTERDlg::OnBnClickedButtoncurrentsource()
{
	ShellExecute(NULL,NULL,MMCfolderC+_T("/CurrentSource/Settings.xls"),NULL,NULL,SW_SHOWNORMAL);
}

void CMASTERDlg::OnBnClickedButtondigital()
{
	ShellExecute(NULL,NULL,MMCfolderC+_T("/AudioTrans/DigitalLines.xls"),NULL,NULL,SW_SHOWNORMAL);
}

void CMASTERDlg::OnAtomicSupertimecommunication()
{
	UpdateData(); //get current setting
	MMCS(Sequence_id, static_cast<LPCSTR>(CStringA(loopnum)));
	m_BULLETIN=_T("WinSock communication finished");
	UpdateData(FALSE);
}

void CMASTERDlg::OnMicrowaveddsCalculateddsdata()
{  
	  PrecisionTimer ReadPulse; //Initialize a precision timer
	  ReadPulse.getFreuqnency(); //Get CPU frequency
	  ReadPulse.Start(); //Start timer

	  DDS(1, 1);

	  ReadPulse.Stop();  // Stop timer
	  double delay=ReadPulse.getTime();
	  static char t[10];
	  sprintf (t, ", takes %.3f ms", delay);

	string str_BULLETIN="Microwave data computation complete";
	str_BULLETIN.append(t);
	m_BULLETIN=str_BULLETIN.c_str();
	UpdateData(FALSE);
}

void CMASTERDlg::OnMicrowaveddsOutputddsdataonce()
{
	DDS(1, 2);
	m_BULLETIN=_T("Microwave data output once complete");
	UpdateData(FALSE);
}

void CMASTERDlg::OnMicrowaveddsCalculateandoutput()
{
	DDS(1, 3);
	m_BULLETIN=_T("Microwave data computation and output complete");
	UpdateData(FALSE);
}

void CMASTERDlg::OnAtomicAudiotransitions()
{
	//Audio(1); //use parameters in row 2
	AudioDoppio(1);
	m_BULLETIN=_T("Audio Transitions data computation and output complete");
	UpdateData(FALSE);
}

void CMASTERDlg::OnAtomicUpdateasyndigitallines()
{
	AudioDport();
	m_BULLETIN=_T("8 asynchronous digital lines updated");
	UpdateData(FALSE);
}

void CMASTERDlg::OnHighvoltageMonitorvoltage()
{
    HVgetShow(this,1);
	UpdateData(FALSE);
}

void CMASTERDlg::OnHighvoltageMonitorleakagecurrent()
{
    HVgetShow(this,2);
	UpdateData(FALSE);
}

void CMASTERDlg::OnSingleAnalogmonitors()
{
	MonitorgetShow(this, 1);
	UpdateData(FALSE);
}

void CMASTERDlg::OnSingleTemperature()
{
    TempgetShow(this);
	UpdateData(FALSE);
}

void CMASTERDlg::OnSinglePdaimaging()
{
    PDAgetShow(this);
    UpdateWindow();
}

void CMASTERDlg::OnAtomicCurrentsource()
{
	//UpdateChannels (20000,32768,30000,40000,50000,50000,50000,50000);
	double cur[8]={0.0};
	ReadCoilCurrent(cur);
	UpdateAllCurrents(cur[0],cur[1],cur[2],cur[3],cur[4],cur[5],cur[6],cur[7]);
	//m_BULLETIN=_T("Current source output complete");
	
	string curs="Update currents (uA)\n"; //output message
	for (int i=0; i<8; i++)
		curs=curs+"Chl"+to_string(i)+"    "+to_string(cur[i])+"\n";
	m_RICHEDIT_REPORT=curs.c_str();
	UpdateData(FALSE);
}

//Hardware pattern match on last two channels of USB 9205
void CMASTERDlg::OnAtomicHardwarepatternmatch()
{
	float64 PatternLevel[2]={0.0};
	CheckPattern(PatternLevel);
    string pat="Pattern match on NI9205 ai30:31\n"; 
	for (int i=0; i<2; i++)
		pat=pat+to_string(PatternLevel[i])+"\n";
	m_RICHEDIT_REPORT=pat.c_str();
	UpdateData(FALSE);
}

//generate delayed timing pulses fro Supertime
void CMASTERDlg::OnAtomicPausetiming()
{
	float delay=float(0.05); //delay 1MHz CLK in ms
	TimingPause(delay);
    string msg="Delayed "+to_string(delay)+"ms";
	AfxMessageBox(msg.c_str());
}

//turn on 1MHz clock at keep it on if using Supertime ext CLK mode
void CMASTERDlg::TurnOn1MHzCLK()
{
	ShellExecute(NULL,NULL,MMCfolderC+_T("/Pause Timing/1MHzCLK/GenConCLK.exe"),NULL,NULL,SW_SHOWNORMAL);
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////





//Implementation parallel tasking
///////////////////////////////////////////////////////////////////////////////////////////////////////////
void CMASTERDlg::OnCompositeParalleltasking()
{  
	const int totTsk=8;
	BOOL select[totTsk]={
		m_Parallel_Audio,
	    m_Parallel_DDS,
	    m_Parallel_HV,
	    m_Parallel_Monitor,
	    m_Parallel_PDA,
	    m_Parallel_ExtField,
	    m_Parallel_Supertime,
	    m_Parallel_Temp};

//////////////////////////////////////////////////////////////////////////////////////
	//Put all selected tasks in a thread vector 
	//Note this is an C++11 implementation of multithreading, very simple and runs fine
	//except memory leak due to conflict with MFC main thread
	//a better solution would be the example in parasweep function
//////////////////////////////////////////////////////////////////////////////////////
	std::vector<std::thread> threads;
	auto pvThread = threads.begin(); 

	if (m_Parallel_Audio)
		threads.push_back(std::thread(Audio, 1));
	if (m_Parallel_DDS) 
		threads.push_back(std::thread(DDS, 1, 3));
	if (m_Parallel_HV) 
		threads.push_back(std::thread(HVgetShow, this, 3));
	if (m_Parallel_Monitor) 
		threads.push_back(std::thread(MonitorgetShow, this, 1));
	if (m_Parallel_PDA) 
		threads.push_back(std::thread(PDAgetShow, this));
	if (m_Parallel_ExtField) 
		threads.push_back(std::thread(DoNothing, 1));
	if (m_Parallel_Supertime) 
		threads.push_back(std::thread(MMCS,this->Sequence_id, static_cast<LPCSTR>(CStringA(this->loopnum))));
	if (m_Parallel_Temp) 
		threads.push_back(std::thread(TempgetShow,this));

    pvThread = threads.begin();
    while (pvThread != threads.end())
    {
         pvThread->join(); //require a C++11 or later complier platform
         pvThread++;
    }
//////////////////////////////////////////////////////////////////////////////////////

	//update data on front panel
	UpdateData(FALSE);
	UpdateWindow();

	//count selected tasks and output message
    int j=0, count=0;
	for(j=0; j<totTsk; j++) {
	   if (select[j])  count++;
	}
	char buffer[2];
	itoa (count,buffer,10);
	string prlmessage=string(buffer)+" tasks completed in parallel\n";
	m_BULLETIN=prlmessage.c_str();
	UpdateData(FALSE);

}
///////////////////////////////////////////////////////////////////////////////////////////////////////////


//Implementation of Parameter sweep (C++11 threads)
///////////////////////////////////////////////////////////////////////////////////////////////////////////
void CMASTERDlg::OnBnClickedButtonParasweep()
{
	ParaSweepDlg Dlg;
	Dlg.DoModal();

	DisableProcessWindowsGhosting(); //Disables the window ghosting feature for the calling GUI process, no "not responding" messages

 if (Dlg.m_DoStartScan)
 {

	//Data set for a parameter sweep from ParaSweepDialog
	int SWP_File=Dlg.m_SWP_File;
	CString SWP_Cell=Dlg.m_SWP_Cell;
	double SWP_Initial=Dlg.m_SWP_Initial;    this->SWPPARA[0]=float(SWP_Initial);
	double SWP_Final=Dlg.m_SWP_Final;        this->SWPPARA[1]=float(SWP_Final);
	int SWP_Steps=Dlg.m_SWP_Steps;           this->SWPPARA[2]=float(SWP_Steps);
	int SWP_Type=Dlg.m_SWP_Type;             this->SWPPARA[3]=float(SWP_Type);

	//write sweep parameter to file
	string SWPfile=MMCfolder+"LAB_DAT/"+"SWPara.txt";
	ofstream dataSWPara(SWPfile.c_str(),ios::out | ios::trunc);
	for(int PA=0;PA<4;PA++)
	{
		dataSWPara<<setw(6)<<this->SWPPARA[PA]<<endl;
	}
	dataSWPara.close();
	

	m_SWPPROGRESSCtrl.SetRange(0, SWP_Steps);

	UpdateData(); //get current setting
	string savedfile=MMCfolder+"LAB_DAT/"+(LPCTSTR)m_DATASET_FILE, newfile;
	char index[4];
	static double StepValue=0.0;  //this value must be static to avoid access violation errors

	const int totTsk=8;
	BOOL select[totTsk]={
		m_Parallel_Audio,
	    m_Parallel_DDS,
	    m_Parallel_HV,
	    m_Parallel_Monitor,
	    m_Parallel_PDA,
	    m_Parallel_ExtField,
	    m_Parallel_Supertime,
	    m_Parallel_Temp};

	DataPack CurrentPack;  
	for (int k=0; k<=SWP_Steps; k++)
	{   
		m_RICHEDIT_REPORT=_T(" ");
	    CurrentPack.init();
		StepValue=SweepValueAt(k, SWP_Initial, SWP_Final, SWP_Steps, SWP_Type);

		//parameter excel file update (func will skip if para-file not exist or unnecessary)
		SweepSetValue(SWP_File, (LPCTSTR)SWP_Cell, StepValue); 

		//update curreent source first
		this->OnAtomicCurrentsource();

		string Currentstep="Current step value\n"; //output message
		Currentstep=Currentstep+to_string(StepValue)+"\n\n";
	    m_RICHEDIT_REPORT=Currentstep.c_str()+m_RICHEDIT_REPORT;

		EnableWindow(FALSE);//Enables or disables mouse and keyboard input to the specified window, save for intensive tasks
      //////////////////////////////////////////////////////////////////////////////////////
		//Put all work needs to be done in each step
	    std::vector<std::thread> threads;
        auto pvThread = threads.begin(); 

		if (SWP_File==3) //delay timing selected
		    threads.push_back(std::thread(TimingPause, StepValue));
	    if (m_Parallel_Audio)
		    threads.push_back(std::thread(AudioDoppio, 1));
	    if (m_Parallel_DDS) 
		    threads.push_back(std::thread(DDS, 1, 2));   //choose option 2 for output .txt and 3 for calculate-output .xls
	    if (m_Parallel_HV) 
		    threads.push_back(std::thread(HVgetShow, this, 3));
	    if (m_Parallel_Monitor) 
		    threads.push_back(std::thread(MonitorgetShow, this, 1));
	    if (m_Parallel_PDA) 
		    threads.push_back(std::thread(PDAgetShow, this));
	    if (m_Parallel_ExtField) 
		    threads.push_back(std::thread(DoNothing, 1));
	    if (m_Parallel_Supertime) 
		    threads.push_back(std::thread(MMCS,Sequence_id, static_cast<LPCSTR>(CStringA(loopnum))));
	    if (m_Parallel_Temp) 
		    threads.push_back(std::thread(TempgetShow,this));

        pvThread = threads.begin();
        while (pvThread != threads.end())
         {
             pvThread->join(); //require a C++11 or later complier platform
             pvThread++;
         }
      //////////////////////////////////////////////////////////////////////////////////////
		EnableWindow(TRUE);
		
		//update data on front panel
		m_SWPPROGRESSCtrl.SetPos(k); //update progress bar
	    UpdateData(FALSE);
	    UpdateWindow();

	    CurrentPack.getPanel(this);
		itoa(k,index,10);
	    newfile=savedfile+string(index)+".txt";
	    CurrentPack.WriteTXT(newfile); 
	}//end of "for loop"

  }//end of "if: start scan" 

  else 
  {
	 string automsg="Parameter scan cancelled"; 
	 m_BULLETIN=automsg.c_str();
	 UpdateData(FALSE);
  }

}
///////////////////////////////////////////////////////////////////////////////////////////////////////////


//Analyze and List plot existing data packs
void CMASTERDlg::OnDatapacklisting()
{
	UpdateData(); //get current setting
	string savedfilePrefix=MMCfolder+"LAB_DAT/"+(LPCTSTR)m_DATASET_FILE, avgfile, avgfileXLS; 
	int IDXstart=m_PACIDXstart, IDXend=m_PACIDXend;  //datapack index

	char currentidx[4];
	bool flag=true;

	DataPack *CurrentPack=new DataPack();

	//Integrated signal
	double InteResult[2]={0.0,0.0}; //Integrated result for a particular curve
	string DataScanFile=MMCfolder+"LAB_DAT/"+"DataScan.txt";
	ofstream datalog(DataScanFile.c_str(),ios::out | ios::trunc); //delete previous log then write the new one

	unsigned int IDXrange[2]={0, 24}; //index range to integrate

	for (int k=IDXstart; k<=m_PACIDXend; k++)
	{
		CurrentPack->init();
		itoa(k,currentidx,10);
		avgfile=savedfilePrefix+string(currentidx)+".txt";
		flag=CurrentPack->ReadTXT(avgfile); 

		if (flag)
		{
			m_BULLETIN=_T("File found. Processing...");
			LoadDataToPanel(this, avgfile);
			datalog<<k<<"  "; //write PAC_index before result from 1st curve
			for (unsigned int shotID=0; shotID<PDANshot; shotID++)
			{
			   SignalInte(CurrentPack, shotID, InteResult); //Integrate over full range
			   //SignalSum(CurrentPack, shotID, InteResult,IDXrange); //Sum over a specified index range 

			   datalog<<InteResult[0]<<"  "<<InteResult[1]<<"  "; //write integrated result for this curve 
			   if(shotID==PDANshot-1) datalog<<endl; //write a line break for the last curve
			}
		}
		else
		{
			m_BULLETIN=_T("File not found. Check in folder LAB_DAT/");
		}
	    UpdateData(FALSE);
	}
	delete CurrentPack;

	datalog.close();

	if (flag)
	{
		m_BULLETIN=_T("Scan analyzed and saved as 'DataScan.txt'");
		UpdateData(FALSE);
	}
	
}


//Averaging existing data packs
void CMASTERDlg::OnMenuPackavg()
{
	UpdateData(); //get current setting
	string savedfilePrefix=MMCfolder+"LAB_DAT/"+(LPCTSTR)m_DATASET_FILE, avgfile, avgfileXLS; 
	int IDXstart=m_PACIDXstart, IDXend=m_PACIDXend;

	char currentidx[4];
	bool flag=true;
	int AvgCount=0;
	DataPack *PackAverage=new DataPack(); 
	PackAverage->init();

	DataPack *CurrentPack=new DataPack();
	for (int k=IDXstart; k<=m_PACIDXend; k++)
	{
		CurrentPack->init();
		itoa(k,currentidx,10);
		avgfile=savedfilePrefix+string(currentidx)+".txt";
		flag=CurrentPack->ReadTXT(avgfile); 

		if (flag)
		{
			m_BULLETIN=_T("File found. Processing...");
			LoadDataToPanel(this, avgfile);
			PackAverage->AddPac(CurrentPack); //add this pack
			AvgCount++;
		}
		else
		{
			m_BULLETIN=_T("File not found. Check in folder LAB_DAT/");
		}
	    UpdateData(FALSE);
	}
	delete CurrentPack;

	PackAverage->DivPacByN(AvgCount);//divid by pack num

	//insert time as filename
	time_t now = time(0);
	tm *ltm = localtime(&now);
	string timestr=to_string(ltm->tm_hour*10000+ltm->tm_min*100+ltm->tm_sec);

	avgfile=savedfilePrefix+timestr+".txt";
	PackAverage->WriteTXT(avgfile); 
	
	//dispaly integrated signal
	double InteResult[2]={0.0,0.0}; //Integrated result for a particular curve
	string message="";
           //message="Integrated signal (a.u.): \n";
	       //message+="=====id,  +Z,  -Z=====\n";
    for (unsigned int shotID=0; shotID<PDANshot; shotID++)
	{
		SignalInte(PackAverage, shotID, InteResult);
		message+=to_string(shotID)+"   "+to_string(InteResult[0])+"   "+to_string(InteResult[1])+"\n";
	}  
	m_RICHEDIT_REPORT=message.c_str(); //write report to RICHEDIT area

	if (flag)
	{
		avgfile="Averaged pack saved as "+savedfilePrefix+timestr+".txt";
		m_BULLETIN=avgfile.c_str();
		UpdateData(FALSE);

		string  avglogfile=MMCfolder+"LAB_DAT/"+"Integrated.txt";
		ofstream avglog(avglogfile.c_str(),ios::out | ios::trunc); 
		avglog<<m_RICHEDIT_REPORT;
		avglog.close();
	}

	delete PackAverage;

	//Plot renormalized state population for 7-pulse detection
	if (PDANshot==8)
	{
       StateMapDlg Dlg;
	   Dlg.DoModal();
	}

}


//Panel data save/load/convert
///////////////////////////////////////////////////////////////////////////////////////////////////////////
void CMASTERDlg::OnDatabaseSave()
{
	DataPack *CurrentPack=new DataPack();
	CurrentPack->init();
	CurrentPack->getPanel(this);

	UpdateData();
	string savedfile=(LPCTSTR)m_DATASET_FILE;
	string message="Panel data saved as ";
	if (savedfile.length()==0) 
		m_BULLETIN=_T("Filename can not be empty");
	else 
	 {
		 savedfile=MMCfolder+"LAB_DAT/"+savedfile+".txt";
		 message=message+savedfile;
	     CurrentPack->WriteTXT(savedfile); 
		 m_BULLETIN=message.c_str();
	}
	delete CurrentPack;
    UpdateData(FALSE);
}


void CMASTERDlg::OnDatabaseLoadpaneldata()
{   
	UpdateData(); //get current setting
	string savedfile=(LPCTSTR)m_DATASET_FILE; 
	if (savedfile.length()==0) 
	{
		m_BULLETIN=_T("Filename can not be empty. Choose in folder LAB_DAT/");
		UpdateData(FALSE);
	}
	else 
	 {
		 savedfile=MMCfolder+"LAB_DAT/"+savedfile+".txt";
	     LoadDataToPanel(this, savedfile);

		 DataPack *CurrentPack=new DataPack();
	     CurrentPack->init();
		 CurrentPack->getPanel(this);

		 //dispaly integrated signal
	     double InteResult[2]={0.0,0.0}; //Integrated result for a particular curve
	     string message="";
           //message="Integrated signal (a.u.): \n";
	       //message+="=====id,  +Z,  -Z=====\n";
         for (unsigned int shotID=0; shotID<PDANshot-1; shotID++)
	     {
		     SignalInte(CurrentPack, shotID, InteResult);
		     //message+=to_string(shotID)+"   "+to_string(InteResult[0])+"   "+to_string(InteResult[1])+"\n";
			 message+=to_string(InteResult[0])+"   "+to_string(InteResult[1])+" ";
	     }  
	     m_RICHEDIT_REPORT=message.c_str(); //write report to RICHEDIT area

	     string  avglogfile=MMCfolder+"LAB_DAT/"+"Integrated.txt";
	     ofstream avglog(avglogfile.c_str(),ios::out | ios::trunc); 
	     avglog<<m_RICHEDIT_REPORT;
	     avglog.close();

		 UpdateData(FALSE);
		 delete CurrentPack;

	     //Plot renormalized state population
		 if (PDANshot==8)
		 {
           StateMapDlg Dlg;
	       Dlg.DoModal();
		 }

	}

}


void CMASTERDlg::OnBnClickedButtonConvertmat()
{
	string ConvertMAT_EXE=MMCfolder+"LAB_DAT/"+"ConvertMAT.exe";
	UpdateData(); //get current setting
	string TXTfile=(LPCTSTR)m_DATASET_FILE; 
	if (TXTfile.length()==0) 
		m_BULLETIN=_T("Filename can not be empty. Choose in folder LAB_DAT/");
	else 
	 {
		 ConvertMAT_EXE=ConvertMAT_EXE+" "+TXTfile;
		 system(ConvertMAT_EXE.c_str());

		 string MATmessage="Converted file saved as "+MMCfolder+"LAB_DAT/"+TXTfile+".mat";
		 m_BULLETIN=MATmessage.c_str();
	}
	UpdateData(FALSE);
}


void CMASTERDlg::OnBnClickedButtonConvertxls()
{
	UpdateData(); //get current setting
	string savedfile=(LPCTSTR)m_DATASET_FILE, newfile; 
	if (savedfile.length()==0) 
		m_BULLETIN=_T("Filename can not be empty. Choose in folder LAB_DAT/");
	else 
	 {
		 newfile=MMCfolder+"LAB_DAT/"+savedfile+".xls";
		 savedfile=MMCfolder+"LAB_DAT/"+savedfile+".txt";
		 DataPack *pack=new DataPack();
		 pack->init();
		 bool success=pack->ReadTXT(savedfile);
		 if (!success){
	         m_BULLETIN=_T("File not found. Choose in folder LAB_DAT/");
		 }
		 else {
			 pack->WriteXLS(newfile);
			 savedfile="Converted file saved as "+newfile;
		     m_BULLETIN=savedfile.c_str();
		 }
		 delete pack;
	}
	UpdateData(FALSE);
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////



//Compute average spin (3 locations on each side) from a list of datapacks and write to a txt file
///////////////////////////////////////////////////////////////////////////////////////////////////////////
bool CMASTERDlg::ComputeAvgSpin()
{
 if (PDANshot==8) //skip this function completely if not detecting 7 sublevels
    {
		static double PopInte[7][VertSubSec*2]={0.0}; //integrated population for each measurement 
		static double AvgSpin[SWPSTPMAX][VertSubSec*2+1]={0.0}; //array to store average spin, plus sweeped value at each step
		unsigned int badshot=SWPSTPMAX; //data index for a bad shot, aka, no atom signal
	
		//index range to integrate: full range is [0,24] pixel
	    unsigned int IDXrange_BPYP[2]={4, 12}; //+Z top
	    unsigned int IDXrange_BPYC1[2]={21, 24}; //+Z center1
		unsigned int IDXrange_BPYC2[2]={0, 3}; //+Z center2
	    unsigned int IDXrange_BPYN[2]={13, 20}; //+Z bottom
	    unsigned int IDXrange_BNYP[2]={4, 12}; //-Z top
	    unsigned int IDXrange_BNYC1[2]={21, 24}; //-Z center1
		unsigned int IDXrange_BNYC2[2]={0, 3}; //-Z center2
	    unsigned int IDXrange_BNYN[2]={13, 20}; //-Z bottom

		//Compute integrated population and average spin for each measurement
		/******************************************************************************************************************/
		UpdateData(); //get current setting
	    string savedfilePrefix=MMCfolder+"LAB_DAT/"+(LPCTSTR)m_DATASET_FILE, curfile; 
	    int IDXstart=m_PACIDXstart, IDXend=m_PACIDXend; //sweep range index 
		int RSWPL=IDXend-IDXstart+1; //Real sweep array length
		char currentidx[4]; //file id number
	    bool flag=true; //file read successful?
	    for (int k=IDXstart; k<=IDXend; k++) //loop for each measurement
	    {  
		   DataPack *CurrentPack=new DataPack();
		   CurrentPack->init();
		   itoa(k,currentidx,10);
		   curfile=savedfilePrefix+string(currentidx)+".txt";
		   flag=CurrentPack->ReadTXT(curfile); 

		   if (flag) 
		   {
			    m_BULLETIN=_T("File found. Processing...");
			    LoadDataToPanel(this, curfile);
				//AfxMessageBox((curfile.c_str())); 

				//loop for each detected mF level within a given measurement
                for (unsigned int ID=0; ID<PDANshot-1; ID++) 
	            {
					double IntRet[2]={0.0,0.0}; //Integrated population for an intermediate curve
		           	SignalSum(CurrentPack, ID, IntRet, IDXrange_BPYP); PopInte[ID][0]=IntRet[0]; //-Z top
					SignalSum(CurrentPack, ID, IntRet, IDXrange_BPYC1); PopInte[ID][1]=IntRet[0]; //-Z center1
					SignalSum(CurrentPack, ID, IntRet, IDXrange_BPYC2); PopInte[ID][1]+=IntRet[0]; //-Z center2
					SignalSum(CurrentPack, ID, IntRet, IDXrange_BPYN); PopInte[ID][2]=IntRet[0]; //-Z bottom
		           	SignalSum(CurrentPack, ID, IntRet, IDXrange_BNYP); PopInte[ID][3]=IntRet[1]; //+Z top
					SignalSum(CurrentPack, ID, IntRet, IDXrange_BNYC1); PopInte[ID][4]=IntRet[1]; //+Z center1
					SignalSum(CurrentPack, ID, IntRet, IDXrange_BNYC2); PopInte[ID][4]+=IntRet[1]; //+Z center2
					SignalSum(CurrentPack, ID, IntRet, IDXrange_BNYN); PopInte[ID][5]=IntRet[1]; //+Z bottom
	            } 
		        UpdateData(FALSE);
		        delete CurrentPack;
		   }
		   else
		   {
			  m_BULLETIN=_T("DataPack file not found. Check in folder LAB_DAT/");
			  UpdateData(FALSE);
		      delete CurrentPack;
			  return false;
		   }


		   //Compute average spin
		   double AtomSignalDenom=0.0; //total population for each subsection
		   double TotalSignal=0.0; //total signal for all sections
		   TotalSignal=0.0;
		   for(int p=0; p<VertSubSec; p++) //p- section id // this is modified from VertSubSec*2 to VertSubSec
			{  
			   AtomSignalDenom=0.0; //reset the denominator [this is necessary]
			   AvgSpin[k][p]=0.0; //reset the spin [this is necessary]
			   for(int q=0; q<7; q++) //q- sublevel
			   {
				   AvgSpin[k][p]+=double(mF[q])*PopInte[q][p]; //total spin
				   AtomSignalDenom+=PopInte[q][p];
			   } 
			   AvgSpin[k][p]/=AtomSignalDenom;
			   TotalSignal+=AtomSignalDenom; //total atom signal
			}

		   //string badshotdata=to_string(k)+": "+to_string(TotalSignal); AfxMessageBox(badshotdata.c_str());
		   if (TotalSignal<1.0) //label a shot with very low atom signal as bad shot
		   {
			   badshot=k; 
		   }

		   //step value for this measurement as last element
		   	   //read saved sweep parameters
	           string SWPfile=MMCfolder+"LAB_DAT/"+"SWPara.txt";
	           ifstream dataSWPara;
			   dataSWPara.open(SWPfile.c_str(), ios::in);
			   bool successflag=!(dataSWPara.fail());
			   if (successflag) 
			   {
	               for(int PA=0;PA<4;PA++)
	               {
		               dataSWPara>>this->SWPPARA[PA];
	               }
			   }
	           dataSWPara.close();
	
		    AvgSpin[k][VertSubSec*2]=SweepValueAt(k, double(this->SWPPARA[0]), double(this->SWPPARA[1]), int(this->SWPPARA[2]), int(this->SWPPARA[3]));
	    
		}

		//local linear approximation for a bad shot
		if (badshot!=SWPSTPMAX)   //bad shot found
		{
			if (badshot==IDXstart)  //first shot
			{
				for(int p=0; p<VertSubSec*2; p++) //p- section id
				{
					//AvgSpin[badshot][p]=2.0*AvgSpin[badshot+1][p]-AvgSpin[badshot+2][p];
					AvgSpin[badshot][p]=0;
				}
			}
			else if (badshot==IDXend)  //last shot
			{
				for(int p=0; p<VertSubSec*2; p++) //p- section id
				{
					//AvgSpin[badshot][p]=2.0*AvgSpin[badshot-1][p]-AvgSpin[badshot-2][p];
					AvgSpin[badshot][p]=0;
				}
			}
			else   //intermediate shot
			{
				for(int p=0; p<VertSubSec*2; p++) //p- section id
				{
					//AvgSpin[badshot][p]=(AvgSpin[badshot-1][p]+AvgSpin[badshot+1][p])/2.0;
					AvgSpin[badshot][p]=0;
				}
			}
		}
		//string badshotid="Bad shot: "+to_string(badshot);AfxMessageBox(badshotid.c_str());
		/******************************************************************************************************************/

		//write spin data to text files
	    time_t now = time(0); tm *ltm = localtime(&now); //insert time as filename
		string timestr=to_string(ltm->tm_hour*10000+ltm->tm_min*100+ltm->tm_sec);

		//Spin file include time tag, saved for future data logging
		/*
		string SpinFile=savedfilePrefix+"Spin"+timestr+".txt"; 
		ofstream datalogS(SpinFile.c_str(),ios::out | ios::trunc);
	    for(int j=0;j<RSWPL;j++)   
	    {
		   for(int k=0;k<VertSubSec*2+1;k++)
			    {
					 datalogS<<setw(6)<<AvgSpin[j+IDXstart][k]<<" ";
			    }
		   datalogS<<endl;
	     }
		datalogS.close();
		*/

		//Spin file w/o time tag, updated each time
		string CurSpin=savedfilePrefix+"Spin"+".txt"; 
		ofstream datalog(CurSpin.c_str(),ios::out | ios::trunc);
	    for(int j=0;j<RSWPL;j++)   
	    {
		   for(int k=0;k<VertSubSec*2+1;k++)
			    {
					 datalog<<setw(6)<<AvgSpin[j+IDXstart][k]<<" ";
			    }
		   datalog<<endl;
	     }
		datalog.close(); 
		return true;
  }
  else 
	  return false;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////


//Magnetometry: bias scan
///////////////////////////////////////////////////////////////////////////////////////////////////////////
void CMASTERDlg::OnCompositeMagnetometryBiasScan()
{
	this->ComputeAvgSpin(); //Compute Average spin from a list of datapack files

	string SpinFile=MMCfolder+"LAB_DAT/"+(LPCTSTR)m_DATASET_FILE+"Spin"+".txt";//spin sweep data file name
	string AnalyzedSpinFile=MMCfolder+"LAB_DAT/"+"BiasAnalyzed"+".txt"; //Analyzed spin pack file, with fitting coefficients

	SpinPackDev *SavedSpinPack=new SpinPackDev(); //Spin Pack object for spin data from a field sweep 
	SavedSpinPack->init();
	bool flag=SavedSpinPack->ReadSpinTXT(SpinFile); //read Spin sweep data
	if (flag)
	{
		m_BULLETIN=_T("File found. Processing...");

		string message="Sweep Steps: "+to_string(SavedSpinPack->ACTSTEP)+"\n";
		m_RICHEDIT_REPORT=message.c_str(); //write report to RICHEDIT area
		UpdateData(FALSE);

		SavedSpinPack->FitLinear();
		SavedSpinPack->CalculateDSDB();
		SavedSpinPack->FitQuad();
		SavedSpinPack->FitSine(); //S(t[ms])
		SavedSpinPack->WriteSpinPackTXT(AnalyzedSpinFile);

		//Display the zero crossing fitting data and results
        MagnetometryDlg Dlg;
	    Dlg.DoModal();
	}  
	else
	{
		m_BULLETIN=_T("File not found. Check in folder LAB_DAT/");
		UpdateData(FALSE);
	}
	delete SavedSpinPack;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////


//load existing spin data from a bias field sweep 
///////////////////////////////////////////////////////////////////////////////////////////////////////////
void CMASTERDlg::OnCompositeLoadspindatasweep()
{
	UpdateData(); //get current setting
	string savedfilePrefix=MMCfolder+"LAB_DAT/"+(LPCTSTR)m_DATASET_FILE;  //enter spin file name prefix in the txet box
	string SpinFile=savedfilePrefix+"Spin.txt";//spin sweep data file name
	string AnalyzedSpinFile=MMCfolder+"LAB_DAT/"+"BiasAnalyzed"+".txt"; //Analyzed spin pack file, with fitting coefficients

	SpinPackDev *SavedSpinPack=new SpinPackDev(); //Spin Pack object for spin data from a field sweep 
	SavedSpinPack->init();
	bool flag=SavedSpinPack->ReadSpinTXT(SpinFile); //read Spin sweep data
	if (flag)
	{
		m_BULLETIN=_T("File found. Processing...");

		string message="Sweep Steps: "+to_string(SavedSpinPack->ACTSTEP)+"\n";
		m_RICHEDIT_REPORT=message.c_str(); //write report to RICHEDIT area
		UpdateData(FALSE);

		SavedSpinPack->FitLinear();
		SavedSpinPack->CalculateDSDB();
		SavedSpinPack->FitQuad();
		SavedSpinPack->FitSine(); //S(t[ms])
		SavedSpinPack->WriteSpinPackTXT(AnalyzedSpinFile);

		//Display the zero crossing fitting data and results
        MagnetometryDlg Dlg;
	    Dlg.DoModal();
	}  
	else
	{
		m_BULLETIN=_T("Spin.txt file not found. Check in folder LAB_DAT/");
		UpdateData(FALSE);
	}
	delete SavedSpinPack;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////


//Magnetometry: gradient scan zero crossings
///////////////////////////////////////////////////////////////////////////////////////////////////////////
void CMASTERDlg::OnMenuMagnetometryGradScan()
{
	this->ComputeAvgSpin(); //Compute Average spin from a list of datapack files

	string SpinFile=MMCfolder+"LAB_DAT/"+(LPCTSTR)m_DATASET_FILE+"Spin"+".txt";//spin sweep data file name
	string AnalyzedSpinFile=MMCfolder+"LAB_DAT/"+"GradAnalyzed"+".txt"; //Analyzed spin pack file, with fitting coefficients

	SpinPackDev *SavedSpinPack=new SpinPackDev(); //Spin Pack object for spin data from a field sweep 
	SavedSpinPack->init();
	bool flag=SavedSpinPack->ReadSpinTXT(SpinFile); //read Spin sweep data
	if (flag)
	{
		m_BULLETIN=_T("File found. Processing...");

		string message="Sweep Steps: "+to_string(SavedSpinPack->ACTSTEP)+"\n";
		m_RICHEDIT_REPORT=message.c_str(); //write report to RICHEDIT area
		UpdateData(FALSE);

		SavedSpinPack->CalculateGrad(); //calculate Gpy and Gpz
        SavedSpinPack->GradFit();  //linear fit to Gpy,z
		SavedSpinPack->WriteGradPackTXT(AnalyzedSpinFile);

		//Display the zero crossing fitting data and results
        MagnetometryGradDlg Dlg;
	    Dlg.DoModal();
	}  
	else
	{
		m_BULLETIN=_T("File not found. Check in folder LAB_DAT/");
		UpdateData(FALSE);
	}
	delete SavedSpinPack;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////


//load existing spin data from a gradient field sweep 
///////////////////////////////////////////////////////////////////////////////////////////////////////////
void CMASTERDlg::OnCompositeAnalyzegrad()
{
	UpdateData(); //get current setting
	string savedfilePrefix=MMCfolder+"LAB_DAT/"+(LPCTSTR)m_DATASET_FILE;  //enter spin file name prefix in the txet box
	string SpinFile=savedfilePrefix+"Spin.txt";//spin sweep data file name
	string AnalyzedGradFile=MMCfolder+"LAB_DAT/"+"GradAnalyzed"+".txt"; //Analyzed spin pack file, with fitting coefficients

	SpinPackDev *SavedSpinPack=new SpinPackDev(); //Spin Pack object for spin data from a field sweep 
	SavedSpinPack->init();
	bool flag=SavedSpinPack->ReadSpinTXT(SpinFile); //read Spin sweep data
	if (flag)
	{
		m_BULLETIN=_T("File found. Processing...");

		string message="Sweep Steps: "+to_string(SavedSpinPack->ACTSTEP)+"\n";
		m_RICHEDIT_REPORT=message.c_str(); //write report to RICHEDIT area
		UpdateData(FALSE);

		SavedSpinPack->CalculateGrad(); //calculate Gpy and Gpz
        SavedSpinPack->GradFit();  //linear fit to Gpy,z
		SavedSpinPack->WriteGradPackTXT(AnalyzedGradFile);

		//Display the zero crossing fitting data and results
        MagnetometryGradDlg Dlg;
	    Dlg.DoModal();
	}  
	else
	{
		m_BULLETIN=_T("Spin.txt file not found. Check in folder LAB_DAT/");
		UpdateData(FALSE);
	}
	delete SavedSpinPack;

}
///////////////////////////////////////////////////////////////////////////////////////////////////////////


//truncate spin file (select center part) for a sweep to get a better linear/quad fit
//index raneg entered on MasterDlg front panel
///////////////////////////////////////////////////////////////////////////////////////////////////////////
void CMASTERDlg::OnCompositeSpinsweeptruncate()
{
	UpdateData(); //get current setting
	string SpinFile=MMCfolder+"LAB_DAT/"+(LPCTSTR)m_DATASET_FILE+"Spin.txt"; 
	string TruncSpinFile=MMCfolder+"LAB_DAT/"+(LPCTSTR)m_DATASET_FILE+"+Spin.txt"; 
	int IDXstart=m_PACIDXstart, IDXend=m_PACIDXend; //data index for spin sweep truncation


	SpinPackDev *SavedSpinPack=new SpinPackDev(); //Spin Pack object for spin data from a field sweep 
	SavedSpinPack->init();
	bool flag=SavedSpinPack->ReadSpinTXT(SpinFile); //read Spin sweep data
	if (flag)
	{
		m_BULLETIN=_T("File found. Processing...");
		string message="Sweep Steps: "+to_string(SavedSpinPack->ACTSTEP)+"\n";

		if ((IDXstart>=0)&&(IDXend<SavedSpinPack->ACTSTEP)&&(IDXend>IDXstart)) //truncation index range
		{
			//passing data from the pack to the temporay matrix
			double SelSpin[SWPSTPMAX][VertSubSec*2+1]={0.0};
			for (int i=IDXstart; i<=IDXend; i++) 
			{
                for (int j=0; j<VertSubSec*2+1; j++)
				{
					SelSpin[i-IDXstart][j]=SavedSpinPack->Spin[i][j];
				}
			}

			//passing selected spin to the pack
			SavedSpinPack->init(); //reset spin pack
			SavedSpinPack->ACTSTEP=(IDXend-IDXstart)+1; //set data counst after trunc
			message+="Sweep Steps Truncated: "+to_string(SavedSpinPack->ACTSTEP)+"\n";
			for (int i=0; i<SavedSpinPack->ACTSTEP; i++) 
			{
                for (int j=0; j<VertSubSec*2+1; j++)
				{
					SavedSpinPack->Spin[i][j]=SelSpin[i][j];
				}
			}

			SavedSpinPack->WriteSpinTXT(TruncSpinFile);
			message+="Truncated file saved as:\n"+TruncSpinFile+"\n";
		}
		else
		{
			message+="Incorrect index range for truncation\n";
		}
		m_RICHEDIT_REPORT=message.c_str(); //write report to RICHEDIT area
		UpdateData(FALSE);
	}  
	else
	{
		m_BULLETIN=_T("Spin file not found. Check in folder LAB_DAT/");
		UpdateData(FALSE);
	}
	delete SavedSpinPack;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////



//Stand-alone Auto Magnetometry (loop structure w/ >=4 magnetometry scans)
///////////////////////////////////////////////////////////////////////////////////////////////////////////
void CMASTERDlg::OnCompositeAutomagnetometry()
{
	DisableProcessWindowsGhosting(); //avoid "not responding" messages on MMC panel
	string automsg=""; 

	automsg="Stand-alone Auto Magnetometry::ignore EDM related settings"; 
	m_BULLETIN=automsg.c_str();
    UpdateData(FALSE);

	//Configure Stand-alone Auto Magnetometry using master loop dialog (will ignore EDM related settings)
	LoopCtrlUIDlg MasterLoopCTRLDlg;
	MasterLoopCTRLDlg.DoModal(); //pop up dialog

 if (MasterLoopCTRLDlg.UpdateAndStart)
 {
	//Copy MAG_SCAN example data to destination folder (opt out for real experiments)
	string CopyCommand="/y \"C:\\MMC_SYSINT\\Example DATA\\MAG_SCAN Example\\*.txt\" C:\\MMC_SYSINT\\LAB_DAT";
    ShellExecute(NULL,NULL,_T("C:/Windows/System32/xcopy.exe"),CopyCommand.c_str(),NULL,SW_HIDE);

   /*////////////////////////////////////////////////////////////////////////////////////////
   *Read MAG SCAN configurations from master loop dialog (will ignore EDM related settings)
   */////////////////////////////////////////////////////////////////////////////////////////
	static unsigned int MasterLoop_Repeat=unsigned int(MasterLoopCTRLDlg.m_AutoMasterLoop_REPEATS); //Master loop number of reapeats
	string MAG_SCAN_header=(LPCTSTR)(MasterLoopCTRLDlg.m_MAG_SCAN_header); //MAG SCAN file header
	static double Target_Larmor_Period=MasterLoopCTRLDlg.m_LarmorThreshold; //Targeted Larmor procession period
	static unsigned int MAG_SCAN_MAX_ITER=unsigned int(MasterLoopCTRLDlg.m_MAG_SCAN_MAX_ITERATIONS); //Max number of iterations for a MAG SCAN loop (t,x,y,z)

   /*////////////////////////////////////////////////////////////////////////////////////
   *Perform magnetic field measuremt and set values of bias and gradient coils
   */////////////////////////////////////////////////////////////////////////////////////
	string new_MAG_SCAN_header=MAG_SCAN_header+"S_";; //temporary MAG_SCAN_header
	AutoMAG4(new_MAG_SCAN_header, Target_Larmor_Period, MAG_SCAN_MAX_ITER);

 }//end of "if: UpdateAndStart"

 else 
 {
      automsg="Stand-alone Auto Magnetometry cancelled"; 
	  m_BULLETIN=automsg.c_str();
	  UpdateData(FALSE);
 }

}



//Implementation of EDM single scan
///////////////////////////////////////////////////////////////////////////////////////////////////////////
void CMASTERDlg::OnCompositeEdmSingleScan()
{
	DisableProcessWindowsGhosting(); //Disables the window ghosting feature for the calling GUI process, no "not responding" messages

	//Pop up the “parameter sweep” dialog
	ParaSweepDlg Dlg;
	Dlg.DoModal();

if (Dlg.m_DoStartScan) //Yes, start an EDM scan
{

/*////////////////////////////////////////////////////////////////////////////////////
*set the scan parameters for Bz
*/////////////////////////////////////////////////////////////////////////////////////

	//Data set for a parameter sweep from ParaSweepDialog
	int SWP_File=Dlg.m_SWP_File;
	CString SWP_Cell=Dlg.m_SWP_Cell;
	double SWP_Initial=Dlg.m_SWP_Initial;    this->SWPPARA[0]=float(SWP_Initial);
	double SWP_Final=Dlg.m_SWP_Final;        this->SWPPARA[1]=float(SWP_Final);
	int SWP_Steps=Dlg.m_SWP_Steps;           this->SWPPARA[2]=float(SWP_Steps);
	int SWP_Type=Dlg.m_SWP_Type;             this->SWPPARA[3]=float(SWP_Type);

	//Generate an optimal HV pattern for EDM SCAN
	HVpolarity *HVPattern=new HVpolarity();
	SWP_Steps=HVPattern->OptimalScanNum(SWP_Steps); //return a nearest optimal number of steps during a scan, i.e. n=13 (n=9) will be changed to n=16 (n=8); 
	HVPattern->GenPorPat(SWP_Steps,2); //Generate HV pattern for a SCAN
	string BullStr="Updated EDM_SCAN total num. of steps : "; //Update SCAN step info
	BullStr+=to_string(SWP_Steps);
	m_BULLETIN=BullStr.c_str();
	UpdateData(FALSE);

	//write sweep parameter to file
	string SWPfile=MMCfolder+"LAB_DAT/"+"SWPara.txt";
	ofstream dataSWPara(SWPfile.c_str(),ios::out | ios::trunc);
	this->SWPPARA[2]=float(SWP_Steps); //update to the true scan number  
	for(int PA=0;PA<4;PA++)
	{
		dataSWPara<<setw(6)<<this->SWPPARA[PA]<<endl;
	}
	dataSWPara.close();
	
	//set progress bar
	m_SWPPROGRESSCtrl.SetRange(0, SWP_Steps);


/*////////////////////////////////////////////////////////////////////////////////////
*loop with multithreaded hardware control, data acquisition and pre-analysis
*/////////////////////////////////////////////////////////////////////////////////////
	UpdateData(); //get current setting from MMC panel
	string savedfile=MMCfolder+"LAB_DAT/"+(LPCTSTR)m_DATASET_FILE, newfile;
	char index[4];
	static double StepValue=0.0;  //this value must be static to avoid access violation errors

	EDMSignalPack CurrentPack;  //EDM signal pack, contains EDM and asymmetry info
	for (int k=0; k<SWP_Steps; k++)
	{   
		m_RICHEDIT_REPORT=_T(" ");
	    CurrentPack.init(); //reset Cuurent DataPack

		//parameter excel file update (func will skip if para-file not exist or unnecessary)
		StepValue=SweepValueAt(k, SWP_Initial, SWP_Final, SWP_Steps, SWP_Type);
		SweepSetValue(SWP_File, (LPCTSTR)SWP_Cell, StepValue); 

		//update curreent source first
		this->OnAtomicCurrentsource();

		string Currentstep="Current step value\n"; //output message
		Currentstep=Currentstep+to_string(StepValue)+"\n\n";
	    m_RICHEDIT_REPORT=Currentstep.c_str()+m_RICHEDIT_REPORT;

		EnableWindow(FALSE);//Enables or disables mouse and keyboard input to the specified window, save for intensive tasks

		//Put all work needs to be done in each step
	    std::vector<std::thread> threads;
        auto pvThread = threads.begin(); 

		threads.push_back(std::thread(Audio, 1));
	    //threads.push_back(std::thread(DDS_EDM, 1, 2));  //DDS pulse sequence
		threads.push_back(std::thread(HVgetShow, this, 3)); //HV leakage and volatge
		threads.push_back(std::thread(MonitorgetShow, this, 1)); //Monitors
		threads.push_back(std::thread(PDAgetShowEDM, this, k)); //PDA DAQ (background subtracted)
		threads.push_back(std::thread(TempgetShow,this));

        pvThread = threads.begin();
        while (pvThread != threads.end())
         {
             pvThread->join(); //require a C++11 or later complier platform
             pvThread++;
         }

		EnableWindow(TRUE);
		
		//update data on front panel
		m_SWPPROGRESSCtrl.SetPos(k+1); //update progress bar
	    UpdateData(FALSE);
	    UpdateWindow();

		//get data from MMC panel and pre-analyze
	    CurrentPack.getPanel(this); //get DataPack for this SHOT
		CurrentPack.HV_Por=HVPattern->GetHVpor(k); //get HV polarity for this SHOT
		CurrentPack.AnalyzeEDM(); //Calculate EDM signal for each pixel
		unsigned int GrpAvgRngSelect[2]={EDMGrpIdxRange[0], EDMGrpIdxRange[1]};
		CurrentPack.GrpAvgEDM(GrpAvgRngSelect); //Calculate averaged EDM signal for each group (a few pixels)

		//write data (single SHOT EDMSignalPack) to file
		itoa(k,index,10); newfile=savedfile+string(index)+".txt";
	    CurrentPack.WriteTXT(newfile); 

	}//end of loop for SHOTs

	//Display HV polarity pattern for this SCAN (optional)
		string HVPatternStr="Test HV pattern: N="+to_string(HVPattern->Length())+"\n";
		for (unsigned int i=0; i<HVPattern->Length(); i++)
		{
			HVPatternStr=HVPatternStr+to_string(HVPattern->GetHVpor(i))+"\t";
			if ((i+1)%4==0)
              HVPatternStr=HVPatternStr+"\n";
		}
	    m_RICHEDIT_REPORT=HVPatternStr.c_str();

	UpdateData(FALSE);
	delete HVPattern; //delete HVPattern (must have)

  }//end of "if: do start scan"

 else
 {
      string automsg="Single EDM scan cancelled"; 
	  m_BULLETIN=automsg.c_str();
	  UpdateData(FALSE);
 }

}

//Analyze and plot data for an EDM SCAN
///////////////////////////////////////////////////////////////////////////////////////////////////////////
void CMASTERDlg::OnAnalyzeViewedmscandata()
{

//Update from MMC panel: which group/pixel to analyze?
	UpdateData(); //get current setting
	int type=int(m_Condition_A), index=int(m_Condition_B); //Message for group/pixel selection

    //Read data file from an EDM SCAN
	string SHOTFilePrefix=MMCfolder+"LAB_DAT/"+(LPCTSTR)m_DATASET_FILE;//enter filename prefix for EDM single SHOT
	string SHOTFile; //filename prefix for EDM single SHOT
	string SWParaFile=SHOTFilePrefix+"SWPara.txt";; //SWP para file
	string AnalyzedEDMSCANFile=MMCfolder+"LAB_DAT/"+"EDMSCANAnalyzed"+".txt";//Analyzed EDM SCAN data filename
	
	EDMScanPack *CurSCAN=new EDMScanPack();
	CurSCAN->init();
	bool flagSWP=CurSCAN->GetPara(SWParaFile);
	bool flagSHOTFILE=true, flagSHOTcheck;
	for (unsigned int i=0; i<unsigned int(CurSCAN->ParaSWP[2]); i++)
	{
		SHOTFile=SHOTFilePrefix+to_string(i)+".txt";
		flagSHOTcheck=CurSCAN->AddShot(SHOTFile);
		flagSHOTFILE=flagSHOTFILE&&flagSHOTcheck;
	}
	CurSCAN->GetSelectChls(type, index, 0); //pull out selected points from EDMSCAN
	CurSCAN->WriteSelectedChls(AnalyzedEDMSCANFile); //write selected EDM SCAN data to a TXT file
	delete CurSCAN;

    //Plot analyzed EDM SCAN data
	if (flagSWP && flagSHOTFILE)
	{
	   EDMSCANPlotDlg EDMSCANDlg;
	   EDMSCANDlg.DoModal();
	}
	else 
	{
	   	m_BULLETIN=_T("EDM SCAN file not found. Check in folder LAB_DAT/");
		UpdateData(FALSE);
	}

}



//Auto_EDM_SCAN function for EDM master loop
///////////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CMASTERDlg::AutoEDMScan(string fileheader, double *BzRange, unsigned int EDM_SCAN_STEPS)
{
	string SCANheader=fileheader+"_"; //File header for all fiels related to this scan

/*////////////////////////////////////////////////////////////////////////////////////
*Set the scan parameters for Bz
*/////////////////////////////////////////////////////////////////////////////////////
	DisableProcessWindowsGhosting(); //avoid "not responding" messages

	//Data set for a parameter sweep from ParaSweepDialog
	int SWP_File=0; //select excel parameter file
	CString SWP_Cell=_T("D4"); //select parameter cell
	double SWP_Initial=BzRange[0]; //initial value      
	double SWP_Final=BzRange[1]; //final value          
	int SWP_Steps=EDM_SCAN_STEPS; //sweep step size
	int SWP_Type=0; //linear sweep             
	this->SWPPARA[0]=float(SWP_Initial);
	this->SWPPARA[1]=float(SWP_Final);
	this->SWPPARA[2]=float(SWP_Steps);
	this->SWPPARA[3]=float(SWP_Type);

	//EDM labview timing sequences (*.dt4 files) and loop
	string SeqPara=" ";//TCP/IP message
	unsigned int id=3; //must select a EDM sequence

	//Generate an optimal HV pattern for EDM SCAN
	HVpolarity *HVPattern=new HVpolarity();
	SWP_Steps=HVPattern->OptimalScanNum(SWP_Steps); //return a nearest optimal number of steps during a scan, i.e. n=13 (n=9) will be changed to n=16 (n=8); 
	HVPattern->GenPorPat(SWP_Steps,2); //Generate HV pattern for a SCAN
	string BullStr="Updated EDM_SCAN total num. of steps : "; //Update SCAN step info
	BullStr+=to_string(SWP_Steps);
	m_BULLETIN=BullStr.c_str();
	UpdateData(FALSE);

	//write sweep parameter to file
	string SWPfile=MMCfolder+"LAB_DAT/"+SCANheader+"SWPara.txt";
	ofstream dataSWPara(SWPfile.c_str(),ios::out | ios::trunc);
	this->SWPPARA[2]=float(SWP_Steps); //update to the true scan number  
	for(int PA=0;PA<4;PA++)
	{
		dataSWPara<<setw(6)<<this->SWPPARA[PA]<<endl;
	}
	dataSWPara.close();
	
	//set progress bar
	m_SWPPROGRESSCtrl.SetRange(0, SWP_Steps);

/*////////////////////////////////////////////////////////////////////////////////////
*loop with multithreaded hardware control, data acquisition/storage and pre-analysis
*/////////////////////////////////////////////////////////////////////////////////////

	//starting MMC-Supertime com link first
    SeqPara=" "+to_string(id)+" "+intostr3(SWP_Steps); //loop this sequence SWP_Steps times
	ShellExecute(NULL,NULL,MMCfolderC+_T("WinSock/MMCSexe"),SeqPara.c_str(),NULL,SW_HIDE);

	//Preparing a for-loop of SHOTs
	string savedfile=MMCfolder+"LAB_DAT/"+SCANheader, newfile;
	char index[4];
	static double StepValue=0.0;  //this value must be static to avoid access violation errors
	EDMSignalPack CurrentPack;  //EDM signal pack, contains EDM and asymmetry info

	//start loop for SHOTs
	for (int k=0; k<SWP_Steps; k++)
	{   
		m_RICHEDIT_REPORT=_T(" ");
	    CurrentPack.init(); //reset Cuurent DataPack

		//parameter excel file update (func will skip if para-file not exist or unnecessary)
		StepValue=SweepValueAt(k, SWP_Initial, SWP_Final, SWP_Steps, SWP_Type);
		SweepSetValue(SWP_File, (LPCTSTR)SWP_Cell, StepValue); 

		//update curreent source first
		this->OnAtomicCurrentsource();

		string Currentstep="Current step value\n"; //output message
		Currentstep=Currentstep+to_string(StepValue)+"\n\n";
	    m_RICHEDIT_REPORT=Currentstep.c_str()+m_RICHEDIT_REPORT;

		EnableWindow(FALSE);//Enables or disables mouse and keyboard input to the specified window, save for intensive tasks

		//Put all work needs to be done in each step
	    std::vector<std::thread> threads;
        auto pvThread = threads.begin(); 

		threads.push_back(std::thread(Audio, 1));
	    //threads.push_back(std::thread(DDS_EDM, 1, 2));  //DDS pulse sequence
		threads.push_back(std::thread(HVgetShow, this, 3)); //HV leakage and volatge
		threads.push_back(std::thread(MonitorgetShow, this, 1)); //Monitors
		threads.push_back(std::thread(PDAgetShowEDM, this, k)); //PDA DAQ (background subtracted)
		threads.push_back(std::thread(TempgetShow,this));

        pvThread = threads.begin();
        while (pvThread != threads.end())
         {
             pvThread->join(); //require a C++11 or later complier platform
             pvThread++;
         }

		EnableWindow(TRUE);
		
		//update data on front panel
		m_SWPPROGRESSCtrl.SetPos(k+1); //update progress bar
	    UpdateData(FALSE);
	    UpdateWindow();

		//get data from MMC panel and pre-analyze
	    CurrentPack.getPanel(this); //get DataPack for this SHOT
		CurrentPack.HV_Por=HVPattern->GetHVpor(k); //get HV polarity for this SHOT
		CurrentPack.AnalyzeEDM(); //Calculate EDM signal for each pixel
		unsigned int GrpAvgRngSelect[2]={EDMGrpIdxRange[0], EDMGrpIdxRange[1]};
		CurrentPack.GrpAvgEDM(GrpAvgRngSelect); //Calculate averaged EDM signal for each group (a few pixels)

		//write data (single SHOT EDMSignalPack) to file
		itoa(k,index,10); newfile=savedfile+string(index)+".txt";
	    CurrentPack.WriteTXT(newfile); 

	}//end of loop for SHOTs

	//Display HV polarity pattern for this SCAN (optional)
		string HVPatternStr="Test HV pattern: N="+to_string(HVPattern->Length())+"\n";
		for (unsigned int i=0; i<HVPattern->Length(); i++)
		{
			HVPatternStr=HVPatternStr+to_string(HVPattern->GetHVpor(i))+"\t";
			if ((i+1)%4==0)
              HVPatternStr=HVPatternStr+"\n";
		}
	    m_RICHEDIT_REPORT=HVPatternStr.c_str();

	UpdateData(FALSE);
	delete HVPattern; //delete HVPattern (must have)


/*////////////////////////////////////////////////////////////////////////////////////
*Analyze and display an EDM SCAN
*/////////////////////////////////////////////////////////////////////////////////////
	//Update from MMC panel: which group/pixel to analyze?
	UpdateData(); //get current setting
	int type=int(m_Condition_A), indexsel=int(m_Condition_B); //Message for group/pixel selection

    //Read data file from an EDM SCAN
	string SHOTFilePrefix=MMCfolder+"LAB_DAT/"+SCANheader;//filename prefix for EDM single SHOT
	string SHOTFile; //filename prefix for EDM single SHOT
	string SWParaFile=MMCfolder+"LAB_DAT/"+SCANheader+"SWPara.txt";; //SWP para file
	string AnalyzedEDMSCANFile=MMCfolder+"LAB_DAT/"+SCANheader+"EDMSCANAnalyzed"+".txt";//Analyzed EDM SCAN data filename
	string AnalyzedEDMSCANFileTemp=MMCfolder+"LAB_DAT/"+"EDMSCANAnalyzed"+".txt";//Analyzed EDM SCAN data filename, temporary fiel for plot
	
	EDMScanPack *CurSCAN=new EDMScanPack();
	CurSCAN->init();
	bool flagSWP=CurSCAN->GetPara(SWParaFile);
	bool flagSHOTFILE=true, flagSHOTcheck;
	for (unsigned int i=0; i<unsigned int(CurSCAN->ParaSWP[2]); i++)
	{
		SHOTFile=SHOTFilePrefix+to_string(i)+".txt";
		flagSHOTcheck=CurSCAN->AddShot(SHOTFile);
		flagSHOTFILE=flagSHOTFILE&&flagSHOTcheck;
	}
	CurSCAN->GetSelectChls(type, indexsel, 0); //pull out selected points from EDMSCAN
	CurSCAN->WriteSelectedChls(AnalyzedEDMSCANFile); //write selected EDM SCAN data to a TXT file
	CurSCAN->WriteSelectedChls(AnalyzedEDMSCANFileTemp); //write selected EDM SCAN data to a temporary TXT file for plot
	delete CurSCAN;

    //Plot analyzed EDM SCAN data
	if (flagSWP && flagSHOTFILE)
	{
	   //Initiate a modaless MFC window
	   EDMSCANPlotDlg *EDMSCANDlg=new EDMSCANPlotDlg();
	   EDMSCANDlg->Create(EDMSCANPlotDlg::IDD, this);
	   EDMSCANDlg->ShowWindow(SW_NORMAL); //show frame
	   EDMSCANDlg->UpdateWindow(); //show controls

       DialogPause(DialogWaitmseconds/5);//Pause the dialog
	   
	   //Simulate a button click
	   WPARAM WParam = MAKEWPARAM(IDC_BUTT_EDMSCAN_PLOTANA, BN_CLICKED); 
       EDMSCANDlg->SendMessage(WM_COMMAND, WParam, NULL);  
	   EDMSCANDlg->UpdateWindow(); //update window after click

	   DialogPause(DialogWaitmseconds);//Pause the dialog

	   //Destroy window
	   EDMSCANDlg->DestroyWindow(); 
	   delete EDMSCANDlg;
	}
	else 
	{
	   	m_BULLETIN=_T("EDM SCAN file not found. Check in folder LAB_DAT/");
		UpdateData(FALSE);
	}

	return true;
}


//Single magnetometry scan function for EDM master loop (Use *MAG_Setting to import previous field settings and export new field settings)
///////////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CMASTERDlg::AutoMAGScan(std::string fileheader, unsigned int MAG_SCAN_Type, double *MAG_SCAN_Range, unsigned int MAG_SCAN_STEPS, double *ScanResults)
{

   /*////////////////////////////////////////////////////////////////////////////////////
   *This function will perform a magnetometry scan (t,x,y,z)
   *Update corresponding parameters in vector<MAG_Setting> depending the scan type
   */////////////////////////////////////////////////////////////////////////////////////
	double LarmorPeriod=0.0; // Get Larmor precession period if scan time parameter
	double Bp=0.0, Gpy=0.0, Gpz=0.0; //Get bias fields and two gradients if scan bias coil

	//write fileheader to front panel for subsequent operations
	string SCANheader=fileheader+"_"; //File header for all files related to this scan
	m_DATASET_FILE=SCANheader.c_str(); //Scan file header
	m_PACIDXstart=0; m_PACIDXend=MAG_SCAN_STEPS; //Scan DataPack index range
	this->UpdateData(FALSE); this->UpdateWindow(); //Update from panel


//Perform a reconfigurable magnetometry scan: hardware control and write DataPack files
  //(will automatically select either a t-scan or x,y,z-scan)
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
	//Sweep parameters for a magnetometry scan
	double SWP_Initial=MAG_SCAN_Range[0]; //initial value      
	double SWP_Final=MAG_SCAN_Range[1]; //final value          
	int SWP_Steps=MAG_SCAN_STEPS; //sweep step size
	int SWP_Type=0; //linear sweep             
	this->SWPPARA[0]=float(SWP_Initial);
	this->SWPPARA[1]=float(SWP_Final);
	this->SWPPARA[2]=float(SWP_Steps);
	this->SWPPARA[3]=float(SWP_Type);
	//write sweep parameter to file
	string SWPfile=MMCfolder+"LAB_DAT/"+"SWPara.txt";
	string SWPfileStorage=MMCfolder+"LAB_DAT/"+SCANheader+"SWPara.txt";
	ofstream dataSWPara(SWPfile.c_str(),ios::out | ios::trunc);
	for(int PA=0;PA<4;PA++)
	{
		dataSWPara<<setw(6)<<this->SWPPARA[PA]<<endl;
	}
	dataSWPara.close();
	ofstream dataSWPara(SWPfileStorage.c_str(),ios::out | ios::trunc);
	for(int PA=0;PA<4;PA++)
	{
		dataSWPara<<setw(6)<<this->SWPPARA[PA]<<endl;
	}
	dataSWPara.close();

	//starting MMC-Supertime com link first
	unsigned int SeqID=3; //change to function of MAG_SCAN_Type for real experiements
    SeqPara=" "+to_string(SeqID)+" "+intostr3(SWP_Steps+1); //loop this sequence SWP_Steps times
	ShellExecute(NULL,NULL,MMCfolderC+_T("WinSock/MMCSexe"),SeqPara.c_str(),NULL,SW_HIDE);

	//Prepare for a magnetometry scan
	const int SWP_File_CurrentSource=0; //select excel parameter file for current source
	CString SWP_Cell[4]={_T("  "), _T("C2"), _T("C3"), _T("C4")}; //select parameter cell (will skip for t-scan)
	string savedfile=MMCfolder+"LAB_DAT/"+SCANheader, newfile;
	char index[4];
	static double StepValue=0.0;  //this value must be static to avoid access violation errors
	m_SWPPROGRESSCtrl.SetRange(0, SWP_Steps);

	DataPack CurrentPack;  
	for (int k=0; k<=SWP_Steps; k++)
	{   
		m_RICHEDIT_REPORT=_T(" ");
	    CurrentPack.init();
		StepValue=SweepValueAt(k, SWP_Initial, SWP_Final, SWP_Steps, SWP_Type);

		//For a magnetic scan (other than t-scan)
		if (MAG_SCAN_Type!=0) 
		{
		   //parameter excel file update (func will skip if para-file not exist or unnecessary)
		   SweepSetValue(SWP_File_CurrentSource, (LPCTSTR)SWP_Cell[MAG_SCAN_Type], StepValue); 
		   //update curreent source first
		   this->OnAtomicCurrentsource();
		}

		string Currentstep="Current step value\n"; //output message
		Currentstep=Currentstep+to_string(StepValue)+"\n\n";
	    m_RICHEDIT_REPORT=Currentstep.c_str()+m_RICHEDIT_REPORT;

		//Put all work needs to be done in each step
	    std::vector<std::thread> threads;
        auto pvThread = threads.begin(); 

		if (MAG_SCAN_Type==0) //for a time parameter t-scan only
		{
		    threads.push_back(std::thread(TimingPause, StepValue));
		}
		else 
		{
		    threads.push_back(std::thread(TimingPause, this->MAG_Setting.at(0)));
		}
		threads.push_back(std::thread(Audio, 1));
		threads.push_back(std::thread(DDS, 1, 2));  
		threads.push_back(std::thread(PDAgetShow, this));

        pvThread = threads.begin();
        while (pvThread != threads.end())
         {
             pvThread->join(); //require a C++11 or later
             pvThread++;
         }
		
		//update data on front panel
		m_SWPPROGRESSCtrl.SetPos(k); //update progress bar
	    UpdateData(FALSE);
	    UpdateWindow();

		//write DataPack for this shot
	    CurrentPack.getPanel(this);
		itoa(k,index,10);
	    newfile=savedfile+string(index)+".txt";
	    CurrentPack.WriteTXT(newfile); 
	}
*/
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


   /*////////////////////////////////////////////////////////////////////////////////////
   *Simulate MAG SCAN by loading exsiting DataPacks (comment out for real experiments)
   *Must copy magnetometry example data 
      from   C:\MMC_SYSINT\Example DATA\MAG_SCAN Example 
	   to    C:\MMC_SYSINT\LAB_DAT 
   */////////////////////////////////////////////////////////////////////////////////////
	m_SWPPROGRESSCtrl.SetRange(0, MAG_SCAN_STEPS+1); //set progress bar
	string magdatafile="", newfile=""; //magnetonmetry file for input and output
	DataPack CurrentPack; 
	for (int q=m_PACIDXstart; q<=m_PACIDXend; q++)
	{
		magdatafile="B_"+to_string(q);
		m_DATASET_FILE=magdatafile.c_str();
		m_SWPPROGRESSCtrl.SetPos(q+1); //update progress bar
		this->UpdateData(FALSE); this->UpdateWindow();
		this->OnDatabaseLoadpaneldata();
		DialogPause(DialogWaitmseconds/5);

		CurrentPack.init();
		CurrentPack.getPanel(this);
	    newfile=MMCfolder+"LAB_DAT/"+SCANheader+to_string(q)+".txt";
	    CurrentPack.WriteTXT(newfile); 
	}
	m_DATASET_FILE=SCANheader.c_str(); //Scan file header reset
	this->UpdateData(FALSE); this->UpdateWindow(); //set DataPack prefix back


   /*////////////////////////////////////////////////////////////////////////////////////
   *Compute Average spin from a list of DataPacks, Output Spin.txt file
   */////////////////////////////////////////////////////////////////////////////////////	
	this->ComputeAvgSpin(); //Generate Spin.txt from DataPacks, see delcariation for details

   /*////////////////////////////////////////////////////////////////////////////////////
   * Analyze spin w/ all methods, Output BiasAnalyzed.txt file
   *///////////////////////////////////////////////////////////////////////////////////// 
	SpinPackDev *SavedSpinPack=new SpinPackDev(); //Spin Pack object for spin data from a field sweep 
	SavedSpinPack->init();
	string SpinFile=MMCfolder+"LAB_DAT/"+SCANheader+"Spin"+".txt";//spin sweep data file name
	string AnalyzedSpinFile=MMCfolder+"LAB_DAT/"+"BiasAnalyzed"+".txt"; //Analyzed spin pack file for in-situ displaying
	string AnalyzedSpinFile_Storage=MMCfolder+"LAB_DAT/"+SCANheader+"BiasAnalyzed"+".txt"; //Analyzed spin pack file for data management
	bool flag=SavedSpinPack->ReadSpinTXT(SpinFile); //read Spin sweep data
	if (flag)
	{
		m_BULLETIN=_T("File found. Processing...");
		string message="Magnetometry selected scan type: "+to_string(MAG_SCAN_Type)+"\n";
		       message+="Scan type: 0=t  1=x  2=y  3=z \n\n";
		       message+="Sweep Steps: "+to_string(SavedSpinPack->ACTSTEP)+"\n";
		m_RICHEDIT_REPORT=message.c_str(); //write report to RICHEDIT area
		this->UpdateData(FALSE); this->UpdateWindow();

		//Analyze spin for this MAG_SCAN
		SavedSpinPack->FitLinear();
		SavedSpinPack->CalculateDSDB();
		SavedSpinPack->FitQuad();
		SavedSpinPack->FitSine(); //S(t[ms])
		SavedSpinPack->WriteSpinPackTXT(AnalyzedSpinFile); //for in-situ displaying
		SavedSpinPack->WriteSpinPackTXT(AnalyzedSpinFile_Storage); //for data management

	    //Plot analyzed MAG_SCAN data
		WPARAM WParam =NULL; //Parameter to simulate a button click
	    MagnetometryDlg *MAGSCANDlg=new MagnetometryDlg();
	    MAGSCANDlg->Create(MagnetometryDlg::IDD, this);
	    MAGSCANDlg->ShowWindow(SW_NORMAL); //show frame
	    MAGSCANDlg->UpdateWindow(); //show controls
		DialogPause(DialogWaitmseconds/5);//Pause the dialog

		//Click button and get parameters depending the scan type
	    if (MAG_SCAN_Type==0) //time parameter scan
		{
	        WParam = MAKEWPARAM(IDC_BUTT_SINE, BN_CLICKED); // sine fitting for period
			//Calculate average Larmor period
			LarmorPeriod=0.0;
			for(int k=0; k<VertSubSec; k++)
			{
				LarmorPeriod+=SavedSpinPack->SineCoef[1][k];
			}
			LarmorPeriod=LarmorPeriod/double(VertSubSec);
			LarmorPeriod=1000.0/LarmorPeriod; //conversion from frequency (Hz) to period (ms)
		}
	    else //Bias field scan
		{
		    WParam = MAKEWPARAM(IDC_BUTT_DSDB, BN_CLICKED);  //dSdB method for magnetic field/cuurent
			//Calculate average bias and gradients
			Bp=0.0, Gpy=0.0, Gpz=0.0;
			for (int i=0; i<VertSubSec; i++)
	        {
		        Bp+=SavedSpinPack->QuadCoef[3][i];
	        }
			Bp/=double(VertSubSec);
			//use +Z side only, -Z side has poor resolution; prefactor means 1pixel=0.36cm
			Gpy=(SavedSpinPack->QuadCoef[3][1]-SavedSpinPack->QuadCoef[3][2])/(5*0.36); 
	        //use center+bottom sections on each side
			Gpz=(SavedSpinPack->QuadCoef[3][1]+SavedSpinPack->QuadCoef[3][2])/2-(SavedSpinPack->QuadCoef[3][4]+SavedSpinPack->QuadCoef[3][5])/2;
		}

        MAGSCANDlg->SendMessage(WM_COMMAND, WParam, NULL);  
	    MAGSCANDlg->UpdateWindow(); //update window after click
		DialogPause(DialogWaitmseconds);//Pause the dialog
	    MAGSCANDlg->DestroyWindow(); //Destroy window
	    delete MAGSCANDlg;
	}  
	else
	{
		m_BULLETIN=_T("File not found. Check in folder LAB_DAT/");
		UpdateData(FALSE);
	}
	delete SavedSpinPack;

	//update array and return scan results
	ScanResults[0]=LarmorPeriod;
	ScanResults[1]=Bp;ScanResults[2]=Gpy;ScanResults[3]=Gpz;
	return flag;
}


//Magnetometry (loop structure w/ >=4 magnetometry scans) function for EDM master loop
///////////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CMASTERDlg::AutoMAG4(std::string fileheader, double LarmorPeriodThreshold, unsigned int MAX_NUM_Iterations)
{   
	//Use the global vector<MAG_Setting> for data exchange within different MAG_SCANs and with Mastermind

	BOOL flag=true; //to make sure each MAG_SCAN is OK
	double ScanResults[4]={0.0};
	unsigned int MAG_SCAN_Type=0; //Scan type (will update in the 4-setp loop)
	double MAG_SCAN_Range[2]={0.0, 1.0}; //sweep range (will update in the 4-setp loop)

	string MAGfileprefix=fileheader; //Data file header for MAG_SCAN
	string SCANTypeStr[4]={"t", "x", "y", "z"};

	//select parameter cell (change to "Cx" for real expr)
	const int SWP_File_CurrentSource=0; //select excel parameter file for current source
	CString SWP_Cell[9]={_T("  "), _T("D2"), _T("D3"), _T("D4"), _T("D5"), _T("D6"), _T("D7"), _T("D8"), _T("D9")}; 
	//same order as <MAG_Setting>      Bx        By        Bz        Gxx       Gzz       Gxy       Gxz       Gzy
	double Gpyz[3][2]={0.0}; //store Gpy and Gpz for 3 bias scans

	//Loop the magnetometry scan until either MAX_NUM_Iterations or Threshold is reached
	bool ThresholdReached=false; //Larmor threshold reached
	unsigned int id=0;  
	do
	{
		id++;

	    for (MAG_SCAN_Type=0;MAG_SCAN_Type<4;MAG_SCAN_Type++)
	    {
		   //update MAG_SCAN_Range here

		   MAGfileprefix=fileheader+to_string(id)+"_"+SCANTypeStr[MAG_SCAN_Type];
	       flag=AutoMAGScan(MAGfileprefix, MAG_SCAN_Type, MAG_SCAN_Range, MAG_SCAN_POINTS, ScanResults);
	
		   string message="Magnetometry Scan Results:\n";
		       message+="T   : "+to_string(ScanResults[0])+" ms\n";
			   message+="Bp  : "+to_string(ScanResults[1])+" uA\n";
			   message+="Gpy : "+to_string(ScanResults[2])+" uA\n";
			   message+="Gpz : "+to_string(ScanResults[3])+" uA/cm\n";
		   m_RICHEDIT_REPORT=message.c_str(); //write report to RICHEDIT area
		   this->UpdateData(FALSE); this->UpdateWindow();

		   if(MAG_SCAN_Type==0)
		   {
			   this->MAG_Setting.at(0)=ScanResults[0]; //set Larmor precession period
		   }
		   else 
		   {
			   //set Bias field values and new Larmor precession period in the vector <MAG_Setting>
			   this->MAG_Setting.at(MAG_SCAN_Type)=ScanResults[1]; //Bp
			   Gpyz[MAG_SCAN_Type-1][0]=ScanResults[2]; //Gpy
			   Gpyz[MAG_SCAN_Type-1][1]=ScanResults[3]; //Gpz
			   //New Larmor precession period (see "Offset correction" for details)
			   //this->MAG_Setting.at(0)=previous period x amplification factor
			   
			   //Update respective magnetic coil setting file (bias fields)
			   SweepSetValue(SWP_File_CurrentSource, (LPCTSTR)SWP_Cell[MAG_SCAN_Type], this->MAG_Setting.at(MAG_SCAN_Type)); 
		   }

		   DialogPause(DialogWaitmseconds/2);//Pause the dialog
	    }

		//Compute and update respective magnetic coil setting file (5 gradients)
		this->MAG_Setting.at(4)=(Gpyz[1][0]+Gpyz[2][1]);     //Gxx=const x (Gyy+Gzz)
		this->MAG_Setting.at(5)=(Gpyz[1][0]-Gpyz[2][1])/2.0; //Gzz=const x (Gyy-Gzz)/2
		this->MAG_Setting.at(6)=Gpyz[0][0];                  //Gxy
		this->MAG_Setting.at(7)=Gpyz[0][1];                  //Gxz
		this->MAG_Setting.at(8)=(Gpyz[2][0]+Gpyz[1][1])/2.0; //Gzy=const x (Gyz+Gzy)/2
		for (unsigned int i=4; i<=8; i++)
		{
		   SweepSetValue(SWP_File_CurrentSource, (LPCTSTR)SWP_Cell[i], this->MAG_Setting.at(i));
		}

		ThresholdReached=(this->MAG_Setting.at(0)>=LarmorPeriodThreshold); //Larmor threshold reached

	}while((id<MAX_NUM_Iterations)&&flag&&(!ThresholdReached));

	return flag;
}


//Automatic EDM measurements: master loop
///////////////////////////////////////////////////////////////////////////////////////////////////////////
void CMASTERDlg::OnCompositeAutomatededm()
{
	DisableProcessWindowsGhosting(); //avoid "not responding" messages on MMC panel

	//Configure automated EDM measurement master loop
	LoopCtrlUIDlg MasterLoopCTRLDlg;
	MasterLoopCTRLDlg.DoModal(); //pop up dialog


 if (MasterLoopCTRLDlg.UpdateAndStart)
 {
	//Copy MAG_SCAN example data to destination folder (opt out for real experiments)
	string CopyCommand="/y \"C:\\MMC_SYSINT\\Example DATA\\MAG_SCAN Example\\*.txt\" C:\\MMC_SYSINT\\LAB_DAT";
    ShellExecute(NULL,NULL,_T("C:/Windows/System32/xcopy.exe"),CopyCommand.c_str(),NULL,SW_HIDE);

   /*////////////////////////////////////////////////////////////////////////////////////
   *Read master loop configurations from master loop dialog
   */////////////////////////////////////////////////////////////////////////////////////
	static unsigned int MasterLoop_Repeat=unsigned int(MasterLoopCTRLDlg.m_AutoMasterLoop_REPEATS); //Master loop number of reapeats
	string MAG_SCAN_header=(LPCTSTR)(MasterLoopCTRLDlg.m_MAG_SCAN_header); //MAG SCAN file header
	static double Target_Larmor_Period=MasterLoopCTRLDlg.m_LarmorThreshold; //Targeted Larmor procession period
	static unsigned int MAG_SCAN_MAX_ITER=unsigned int(MasterLoopCTRLDlg.m_MAG_SCAN_MAX_ITERATIONS); //Max number of iterations for a MAG SCAN loop (t,x,y,z)
	string EDM_SCAN_header=(LPCTSTR)(MasterLoopCTRLDlg.m_EDM_SCAN_header); //EDM SCAN file header definition 
	static unsigned int EDM_SCAN_STEPS=unsigned int(MasterLoopCTRLDlg.m_EDM_SCAN_STEPS); //Number of steps during a single magnetometry scan (will be altered to 2^N)
	static unsigned int MAX_NUM_EDM_SCAN=unsigned int(MasterLoopCTRLDlg.m_EDM_SCAN_ITERATIONS); //Max num of EDM SCANs performed before a magnetometry measurement

   /*////////////////////////////////////////////////////////////////////////////////////
   *Executing the automated measurement: master loop
   */////////////////////////////////////////////////////////////////////////////////////
	string new_MAG_SCAN_header=""; //temporary MAG_SCAN_header
	string new_EDM_SCAN_header=""; //temporary EDM_SCAN_header
	for (unsigned int masterCNT=1; masterCNT<=MasterLoop_Repeat; masterCNT++)
	{
         //Perform magnetic field measuremt and set values of bias and gradient coils
		 new_MAG_SCAN_header=MAG_SCAN_header+to_string(masterCNT)+"_";
	     AutoMAG4(new_MAG_SCAN_header, Target_Larmor_Period, MAG_SCAN_MAX_ITER);

	     //Perform a fixed number of EDM SCANs
	     double BzRange[2]={-1.0, 1.0}; //EDM SCAN Bz sweep range
	     for (unsigned int lc=1; lc<=MAX_NUM_EDM_SCAN; lc++)
	     {
	 	     new_EDM_SCAN_header=EDM_SCAN_header+to_string(masterCNT)+"_"+to_string(lc);
	 	     this->AutoEDMScan(new_EDM_SCAN_header, BzRange, EDM_SCAN_STEPS); //a single EDM SCAN
	     }
	}// end of "for loop"

 }//end of "if: UpdateAndStart"

 else 
 {
      string automsg="Automated EDM master loop cancelled"; 
	  m_BULLETIN=automsg.c_str();
	  UpdateData(FALSE);
 }

}

