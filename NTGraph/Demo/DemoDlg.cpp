// DemoDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Demo.h"
#include "DemoDlg.h"

#include <math.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDemoDlg dialog

CDemoDlg::CDemoDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDemoDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDemoDlg)
	m_nZoom = 0;
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CDemoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDemoDlg)
	DDX_Check(pDX, IDC_STACKEDBOOLEAN, m_bStacked);
	DDX_Slider(pDX, IDC_NUMTRACESSLIDE, m_nElementsSlide);
	DDX_Slider(pDX, IDC_PTSPERTRACESLIDE, m_nPointsSlide);
	DDX_Control(pDX, IDC_NTGRAPHCTRL1, m_Graph1);
	DDX_Control(pDX, IDC_NTGRAPHCTRL2, m_Graph2);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CDemoDlg, CDialog)
	//{{AFX_MSG_MAP(CDemoDlg)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BUTTON_DEMO, OnButtonDemo)
	ON_BN_CLICKED(IDC_PROPERTIES, OnProperties)
	ON_BN_CLICKED(IDC_BUTTON_DEMOS, OnButtonDemos)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BEGIN_EVENTSINK_MAP(CDemoDlg, CDialog)
    //{{AFX_EVENTSINK_MAP(CDemoDlg)
	//}}AFX_EVENTSINK_MAP
END_EVENTSINK_MAP()


/////////////////////////////////////////////////////////////////////////////
// CDemoDlg message handlers

BOOL CDemoDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here
	m_Graph1.SetFrameStyle(2);
	m_Graph1.SetPlotAreaColor(RGB(0,0,0));
	SetPlotFace(&m_Graph2);
    
	m_Graph1.SetXLabel("");
    m_Graph1.SetYLabel("");
	m_Graph1.SetCaption("");

	m_Graph2.SetCaption("XY Plot");

	// setup the timer id=1, 50 ms
	SetTimer(1, 50, NULL);
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CDemoDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

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
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CDemoDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CDemoDlg::OnTimer(UINT nIDEvent)
{
	UpdateData(TRUE);


	 m_Graph1.ClearGraph();

	// number of points per Element >= 1
	long nPoints = __max(m_nPointsSlide, 1);

	// number of Elements >= 1
	long nElements = __max(m_nElementsSlide, 1);

	double value;
	BOOL stacked = m_bStacked;

	
	for (long elementIndex = 0; elementIndex < nElements; elementIndex++) 
	{
		m_Graph1.AddElement();
		m_Graph1.SetElementLineColor(RGB(125,255,0));
		for (long pointIndex = 0; pointIndex < nPoints; pointIndex++) 
		{
			// random double between 0 and 10
			const double maxValue = 10.0;
			value = (double)rand() / RAND_MAX * maxValue;
			if (stacked) 
				value = value / nElements + maxValue / nElements * elementIndex;

			m_Graph1.PlotXY(pointIndex, value, elementIndex+1);
		}
	}

	m_Graph1.SetRange (0,nPoints,0,10);
    UpdateData(FALSE);	

	CDialog::OnTimer(nIDEvent);
}


void CDemoDlg::OnButtonDemo() 
{

	m_Graph2.ClearGraph();
	m_Graph2.SetElement(0);
	m_Graph2.SetElementLineColor(RGB(255,0,0));
	m_Graph2.SetElementIdentify(TRUE);
	m_Graph2.SetElementPointColor(RGB(0,0,255));
	m_Graph2.SetElementPointSymbol(4);
	m_Graph2.SetElementWidth (2);
	m_Graph2.SetElementSolidPoint(TRUE);


	double x,y;
	
	for (int i=0;i<500;i++){
		
		x=(((double)i)* (40./500.))-20;
		
         // Awllayes try to avoid bad points!!!
		if(x==0) x=1e-5; 

		y=exp(-(x*x)/50.)+sin((3.15*x))/x;
		
		m_Graph2.PlotXY(x,y ,0);
	}	

	m_Graph2.SetRange(-20,20,-1,5);


}

void CDemoDlg::OnProperties() 
{
	m_Graph2.ShowProperties();
}


void CDemoDlg::SetPlotFace(CNTGraph *graph)
{
	graph->SetFrameStyle(0);
	graph->SetPlotAreaColor(RGB(255,255,255));
	graph->SetControlFrameColor(RGB(255,255,255));
	graph->SetXLabel("X Axis");
	graph->SetYLabel("Y Axis");

}


void CDemoDlg::SetScopeFace(CNTGraph *graph)
{
	graph->SetFrameStyle(1);
	graph->SetControlFrameColor(RGB(222, 215, 200));
    graph->SetPlotAreaColor(RGB(0,0,0));
}

void CDemoDlg::OnButtonDemos() 
{

	SetScopeFace(&m_Graph2);
	
	for (int i=0;i<30;i++) {
		Demo1();
		UpdateWindow();
		Sleep(100);
	}

	SetPlotFace(&m_Graph2);

	for (int i=0;i<30;i++) {
		Demo1();
		UpdateWindow();
		Sleep(100);
	}


	SetScopeFace(&m_Graph2);

	for (int i=0;i<30;i++) {
		Demo2();
		UpdateWindow();
		Sleep(100);
	}

	SetScopeFace(&m_Graph2);

	for (int i=0;i<30;i++) {
		Demo3(6);
		UpdateWindow();
		Sleep(100);
	}

	for (int i=0;i<30;i++) {
		Demo3(7);
		UpdateWindow();
		Sleep(100);
	}

	for (int i=0;i<30;i++) {
		Demo3(8);
		UpdateWindow();
		Sleep(100);
	}

	for (int i=0;i<30;i++) {
		Demo3(9);
		UpdateWindow();
		Sleep(100);
	}

	for (int i=0;i<30;i++) {
		Demo3(10);
		UpdateWindow();
		Sleep(100);
	}

	SetPlotFace(&m_Graph2);

	for (int i=1; i<17; i++)
		Demo4(i);

	SetPlotFace(&m_Graph2);
	m_Graph2.ClearGraph();
}




void CDemoDlg::Demo1()
{

  m_Graph2.ClearGraph();

  if(m_Graph2.GetFrameStyle()==2)
  {
	  //m_Graph2.SetElementLineColor(RGB(125,255,0));
	  m_Graph2.SetElementLineColor(RGB(255,255,0));
	  m_Graph2.SetElementLinetype(0);
	  m_Graph2.SetCaption("Have a brake on left side!");
  }
  else
  {
	  m_Graph2.SetCaption("NTGraph Demo");
	  m_Graph2.SetElementLineColor(RGB(255,0,0));
	  m_Graph2.SetElementIdentify(TRUE);
	  m_Graph2.SetElementPointColor(RGB(0,0,255));
	  m_Graph2.SetElementPointSymbol(4);
	  m_Graph2.SetElementWidth (2);
	  m_Graph2.SetElementSolidPoint(TRUE);
  }


  double data;
  for (int i=0;i<100;i++)
  {
		data=(rand()*sin(i/3.15))/10000;
        m_Graph2.PlotY(data,0);
		m_Graph2.SetRange(0,100,-3,3);
  }
  
}

void CDemoDlg::Demo2()
{

  m_Graph2.ClearGraph();

  // The Element [0] is allocated by default
  m_Graph2.SetElementLineColor(RGB(125,255,0));
 	   
  // Add the second graph element 
  m_Graph2.AddElement();
  m_Graph2.SetElementLineColor(RGB(255,255,0));

  m_Graph2.SetElementIdentify(FALSE);
	  
  m_Graph2.SetCaption(" ");

  
  m_Graph2.SetRange (0,100,0,10);

  double y;
  for (long i = 0; i < 2; i++) 
  {
 	for (long x = 0; x < 100; x++) 
	{
		y = (double)rand() / RAND_MAX * 10.0;
		y = y / 3 + 10.0 / 2 * i + 1;
		m_Graph2.PlotXY(x, y, i);
	}
  }
  
}

void CDemoDlg::Demo3(int linetype)
{

  m_Graph2.ClearGraph();

  // The Element [0] is allocated by default
  m_Graph2.SetElementLineColor(RGB(125,255,0));

  switch (linetype)
  {
  case 6: // XY Steps
	  m_Graph2.SetElementLinetype(6);
	  m_Graph2.SetCaption("Demo3: Linetype 6: XY Steps");
      break;
  case 7: // YX Steps
	  m_Graph2.SetElementLinetype(7);
	  m_Graph2.SetCaption("Linetype 7: YX Steps");
      break;
  case 8: // Bars
  	  m_Graph2.SetElementLinetype(8);
	  m_Graph2.SetCaption("Linetype 8: Bars");
      break;
  case 9: // Stick
  	  m_Graph2.SetCaption("Linetype 9: Stick");
	  m_Graph2.SetElementLinetype(9);
      break;

  case 10: // Stick
  	  m_Graph2.SetCaption("With more points...");
	  m_Graph2.SetElementLinetype(9);
     
	  double data;
	  for (int i=0;i<200;i++)
	  {
		data = (double)rand() / RAND_MAX * 10.0;
        m_Graph2.PlotY(data,0);
		m_Graph2.SetRange(0,200,0,15);
	  }
	  return;
  }

  double data;
  for (int i=0;i<30;i++)
  {
		data = (double)rand() / RAND_MAX * 10.0;
        m_Graph2.PlotY(data,0);
		m_Graph2.SetRange(0,30,0,15);
  }

  
}


void CDemoDlg::Demo4(int symbol)
{

  m_Graph2.ClearGraph();

  // The Element [0] is allocated by default
  m_Graph2.SetElementWidth(5);

  if(symbol>8)
	  m_Graph2.SetFrameStyle(2);

	   m_Graph2.SetElementLineColor(RGB(255,0,0));
	   m_Graph2.SetElementPointColor(RGB(0,0,255));
  
 
  switch (symbol)
  {
  case 1: 
	  m_Graph2.SetElementPointSymbol(1);
	  m_Graph2.SetCaption("Demo4: Symbol 1: Empty Dot");
	  m_Graph2.SetElementSolidPoint(TRUE);
      break;
  case 2: 
	  m_Graph2.SetElementPointSymbol(2);
	  m_Graph2.SetCaption("Symbol 2: Empty Rectangle");
	  m_Graph2.SetElementSolidPoint(TRUE);
      break;
  case 3: 
  	  m_Graph2.SetElementPointSymbol(3);
	  m_Graph2.SetCaption("Symbol 3: Empty Diamond");
	  m_Graph2.SetElementSolidPoint(TRUE);
      break;
  case 4: 
  	  m_Graph2.SetElementPointSymbol(4);
	  m_Graph2.SetCaption("Symbol 4: Empty Asterisk");
	  m_Graph2.SetElementSolidPoint(TRUE);
      break;
  case 5: 
	  m_Graph2.SetElementPointSymbol(5);
	  m_Graph2.SetCaption("Symbol 5: Empty Down Triangle");
	  m_Graph2.SetElementSolidPoint(TRUE);
      break;
  case 6: 
	  m_Graph2.SetElementPointSymbol(6);
	  m_Graph2.SetCaption("Symbol 6: Empty Up Triangle");
	  m_Graph2.SetElementSolidPoint(TRUE);
      break;
  case 7: 
  	  m_Graph2.SetElementPointSymbol(7);
	  m_Graph2.SetCaption("Symbol 7: Empty Left Triangle");
	  m_Graph2.SetElementSolidPoint(TRUE);
      break;
  case 8: 
  	  m_Graph2.SetElementPointSymbol(8);
	  m_Graph2.SetCaption("Symbol 8: Empty Right Triangle");
	  m_Graph2.SetElementSolidPoint(TRUE);
      break;
  case 9: 
	  m_Graph2.SetElementPointSymbol(1);
	  m_Graph2.SetCaption("Demo4: Symbol 1: Full Dot");
	  m_Graph2.SetElementSolidPoint(TRUE);
      break;
  case 10: 
	  m_Graph2.SetElementPointSymbol(2);
	  m_Graph2.SetCaption("Symbol 2: Full Rectangle");
	  m_Graph2.SetElementSolidPoint(FALSE);
      break;
  case 11: 
  	  m_Graph2.SetElementPointSymbol(3);
	  m_Graph2.SetCaption("Symbol 3: Full Diamond");
	  m_Graph2.SetElementSolidPoint(FALSE);
      break;
  case 12: 
  	  m_Graph2.SetElementPointSymbol(4);
	  m_Graph2.SetCaption("Symbol 4:  Full Asterisk");
	  m_Graph2.SetElementSolidPoint(FALSE);
      break;
  case 13: 
	  m_Graph2.SetElementPointSymbol(5);
	  m_Graph2.SetCaption("Symbol 5:  Full Down Triangle");
	  m_Graph2.SetElementSolidPoint(FALSE);
      break;
  case 14: 
	  m_Graph2.SetElementPointSymbol(6);
	  m_Graph2.SetCaption("Symbol 6:  Full Up Triangle");
	  m_Graph2.SetElementSolidPoint(FALSE);
      break;
  case 15: 
  	  m_Graph2.SetElementPointSymbol(7);
	  m_Graph2.SetCaption("Symbol 7:  Full Left Triangle");
	  m_Graph2.SetElementSolidPoint(FALSE);
      break;
  case 16: 
  	  m_Graph2.SetElementPointSymbol(8);
	  m_Graph2.SetCaption("Symbol 8:  Full Right Triangle");
	  m_Graph2.SetElementSolidPoint(FALSE);
      break;
  }

  double data;
  m_Graph2.SetRange(0,20,-2,2);
  for (int i=0;i<20;i++)
  {
	  if(symbol>8){
			data=cos(i/3.15);
		    Sleep(10);
	  }
	  else {
			data=sin(i/3.15);
			Sleep(30);
	  }

        m_Graph2.PlotY(data,0);
		
		UpdateWindow();

  }

  m_Graph2.SetCaption("2D Graph Demo");

  
}
