// MagnetometryDlg.cpp : implementation file
//

#include "stdafx.h"
#include "MASTER.h"
#include "MagnetometryDlg.h"
#include "afxdialogex.h"
#include "SpinPackSWP.h" //multi-location spin pack for a scan: parent class

// MagnetometryDlg dialog

IMPLEMENT_DYNAMIC(MagnetometryDlg, CDialogEx)

MagnetometryDlg::MagnetometryDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(MagnetometryDlg::IDD, pParent) //Initializer list
	, m_BPYP(_T(""))
	, m_BPYC(_T(""))
	, m_BPYN(_T(""))
	, m_BNYP(_T(""))
	, m_BNYC(_T(""))
	, m_BNYN(_T(""))
	, m_DPDY(_T(""))
	, m_DPDZ(_T(""))
{
}

MagnetometryDlg::~MagnetometryDlg()
{
}

void MagnetometryDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_ZEROCROSPZ, m_Graph_PZCROSS);
	DDX_Control(pDX, IDC_ZEROCROSNZ, m_Graph_NZCROSS);
	DDX_Text(pDX, IDC_EDIT_BPYP, m_BPYP);
	DDX_Text(pDX, IDC_EDIT_BPYC, m_BPYC);
	DDX_Text(pDX, IDC_EDIT_BPYN, m_BPYN);
	DDX_Text(pDX, IDC_EDIT_BNYP, m_BNYP);
	DDX_Text(pDX, IDC_EDIT_bnyc, m_BNYC);
	DDX_Text(pDX, IDC_EDIT_BNYN, m_BNYN);
	DDX_Text(pDX, IDC_EDIT_DPDY, m_DPDY);
	DDX_Text(pDX, IDC_EDIT_DPDZ, m_DPDZ);
}


BEGIN_MESSAGE_MAP(MagnetometryDlg, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON_LINEARCROSS, &MagnetometryDlg::OnBnClickedButtonLinearCross)
	ON_BN_CLICKED(IDC_BUTT_DSDB, &MagnetometryDlg::OnBnClickedButtDsdb)
	ON_BN_CLICKED(IDC_BUTT_SINE, &MagnetometryDlg::OnBnClickedButtSine)
END_MESSAGE_MAP()


// MagnetometryDlg message handlers


//Linear cross method
void MagnetometryDlg::OnBnClickedButtonLinearCross()
{
	//set dialog window position tp upper left
	//SetWindowPos(NULL, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOZORDER);

	this->AnalyzeDataLinear();
	this->UpdateData(FALSE);
}

//dS/dB mthod
void MagnetometryDlg::OnBnClickedButtDsdb()
{
	//set dialog window position tp upper left
	//SetWindowPos(NULL, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOZORDER);

	this->AnalyzeDataQuad();
	this->UpdateData(FALSE);
}

//fielname for analyzed bias scan data
string MMCfolderB="C:/MMC_SYSINT/";
string SpinFile=MMCfolderB+"LAB_DAT/"+"BiasAnalyzed.txt";


//perform calculation and plot of data, using Linear Crossing method
void MagnetometryDlg::AnalyzeDataLinear()
{
	bool flag=false;

    SpinPack *AnalyzedSpinPack=new SpinPack(); //Spin Pack object for spin data from a field sweep 
	AnalyzedSpinPack->init();
	flag=AnalyzedSpinPack->ReadSpinPackTXT(SpinFile);

if (flag)
 {
	int AcS=AnalyzedSpinPack->ACTSTEP; //Actual size of selected array 

	//read linear fitting coefficients/Zero Crossings and original spin data
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	double LinCoefs[3][VertSubSec*2]={0.0}; 
	for (int i=0; i<3;i++)
	{
		for(int j=0; j<VertSubSec*2;j++)
		{
			LinCoefs[i][j]=AnalyzedSpinPack->LinearCoef[i][j];
		}
	}

	double Spin[SWPSTPMAX][VertSubSec*2+1]={0.0}; //array to store average spin, plus sweeped value at each step
	for (int i=0; i<AcS;i++)
	{
		for(int j=0; j<VertSubSec*2+1;j++)
		    {
			    Spin[i][j]=AnalyzedSpinPack->Spin[i][j];
		    }
	}

	//update zero crossing data on dialog
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	string Crossings[VertSubSec*2]={""};
	for (int i=0; i<VertSubSec*2; i++)
	{
		Crossings[i]=to_string(LinCoefs[2][i])+"uA";
	}
	m_BPYP=Crossings[0].c_str();
	m_BPYC=Crossings[1].c_str();
	m_BPYN=Crossings[2].c_str();
	m_BNYP=Crossings[3].c_str();
	m_BNYC=Crossings[4].c_str();
	m_BNYN=Crossings[5].c_str();

	//calculate and update gradients: calibration factors to be determined (~0.039G/cm/A)
	double BPBY=(LinCoefs[2][1]-LinCoefs[2][2])/(5*0.36); //use +Z side only, -Z side has poor resolution; prefactor means 1pixel=0.36cm
	double BPBZ=(LinCoefs[2][1]+LinCoefs[2][2])/2-(LinCoefs[2][4]+LinCoefs[2][5])/2; //use center+bottom sections on each side
	string Gpy=to_string(BPBY)+"uA/cm";
	string Gpz=to_string(BPBZ)+"uA/cm";
	m_DPDY=Gpy.c_str();
	m_DPDZ=Gpz.c_str();

	//Calculate linear fitting data using imported coefs
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	const unsigned int refS=120; //refined array 
	double  SpinLF[refS][VertSubSec*2+1]={0.0};
	double stepsize=(Spin[AcS-1][VertSubSec*2]-Spin[0][VertSubSec*2])/100.0;
	for (int i=0; i<refS; i++)
	{
		SpinLF[i][VertSubSec*2]=stepsize*double(i-10)+Spin[0][VertSubSec*2];
		for(int k=0;k<=VertSubSec*2-1;k++)
			{
				SpinLF[i][k]=LinCoefs[0][k]+LinCoefs[1][k]*SpinLF[i][VertSubSec*2];
			}
	}
	//string message="Calculated value: "+to_string(SpinLF[0][VertSubSec*2]);AfxMessageBox(message.c_str());

	//calculating plot range
	double Xaxis[2]={SpinLF[0][VertSubSec*2], SpinLF[refS-1][VertSubSec*2]};
	if (Xaxis[0]>Xaxis[1]) //swap
	{
		double temp=0.0; temp=Xaxis[0]; Xaxis[0]=Xaxis[1]; Xaxis[1]=temp;
	}

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//plot +Z side data
	unsigned long PlotColors[3]={RGB(0,100,0), RGB(0,0,255), RGB(255,0,0)}; //green, blue, red

	m_Graph_PZCROSS.ClearGraph(); //clear previous plots if any
	m_Graph_PZCROSS.SetFrameStyle(0);
	m_Graph_PZCROSS.SetXLabel("Bias coil p (uA)");
	m_Graph_PZCROSS.SetYLabel("Spin");
	m_Graph_PZCROSS.SetRange(Xaxis[0], Xaxis[1],  -3.5, 3.5);
	m_Graph_PZCROSS.SetXGridNumber(10);
	m_Graph_PZCROSS.SetYGridNumber(10); 

	for (unsigned int p=0; p<3; p++) //plot 
	{
	  m_Graph_PZCROSS.AddElement();
      m_Graph_PZCROSS.SetElement(p+1); 
      m_Graph_PZCROSS.SetElementPointColor(PlotColors[p]);
      m_Graph_PZCROSS.SetElementLinetype(5); //no solid line
	  m_Graph_PZCROSS.SetElementPointSymbol(2); //rectangles
	  m_Graph_PZCROSS.SetElementWidth(4);
	  m_Graph_PZCROSS.SetElementSolidPoint(FALSE);

	  for(int k=0;k<AcS;k++)
	  {
	     m_Graph_PZCROSS.PlotXY(Spin[k][VertSubSec*2], Spin[k][p], p+1); 
	  }
	}

	for (unsigned int p=0; p<3; p++)
	{
	  m_Graph_PZCROSS.AddElement();
      m_Graph_PZCROSS.SetElement(p+4); 
      m_Graph_PZCROSS.SetElementLineColor(PlotColors[p]);
      m_Graph_PZCROSS.SetElementLinetype(0); //solid line
	  m_Graph_PZCROSS.SetElementPointSymbol(0); //no symbols
	  m_Graph_PZCROSS.SetElementWidth(1);

	  for(int k=0;k<refS;k++)
	  {
	     m_Graph_PZCROSS.PlotXY(SpinLF[k][VertSubSec*2], SpinLF[k][p], p+4); 
	  }
	}

	//plot -Z side data
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	m_Graph_NZCROSS.ClearGraph(); //clear previous plots if any
	m_Graph_NZCROSS.SetFrameStyle(0);
	m_Graph_NZCROSS.SetXLabel("Bias coil p (uA)");
	m_Graph_NZCROSS.SetYLabel("Spin");
	m_Graph_NZCROSS.SetRange(Xaxis[0], Xaxis[1], -3.5, 3.5);
	m_Graph_NZCROSS.SetXGridNumber(10);
	m_Graph_NZCROSS.SetYGridNumber(10); 

	for (unsigned int p=0; p<3; p++) //plot 
	{
	  m_Graph_NZCROSS.AddElement();
      m_Graph_NZCROSS.SetElement(p+1); 
      m_Graph_NZCROSS.SetElementPointColor(PlotColors[p]);
      m_Graph_NZCROSS.SetElementLinetype(5); //no solid line
	  m_Graph_NZCROSS.SetElementPointSymbol(2); //rectangles
	  m_Graph_NZCROSS.SetElementWidth(4);
	  m_Graph_NZCROSS.SetElementSolidPoint(FALSE);

	  for(int k=0;k<AcS;k++)
	  {
	     m_Graph_NZCROSS.PlotXY(Spin[k][VertSubSec*2], Spin[k][p+3], p+1); 
	  }
	}

	for (unsigned int p=0; p<3; p++)
	{
	  m_Graph_NZCROSS.AddElement();
      m_Graph_NZCROSS.SetElement(p+4); 
      m_Graph_NZCROSS.SetElementLineColor(PlotColors[p]);
      m_Graph_NZCROSS.SetElementLinetype(0); //solid line
	  m_Graph_NZCROSS.SetElementPointSymbol(0); //no symbols
	  m_Graph_NZCROSS.SetElementWidth(1);

	  for(int k=0;k<refS;k++)
	  {
	     m_Graph_NZCROSS.PlotXY(SpinLF[k][VertSubSec*2], SpinLF[k][p+3], p+4); 
	  }
	}

 }//end of if(flag)
 UpdateData(false);
 delete AnalyzedSpinPack;
}


//perform calculation and plot of data, using dS/dB quad method
void MagnetometryDlg::AnalyzeDataQuad()
{
	bool flag=false;

    SpinPack *AnalyzedSpinPack=new SpinPack(); //Spin Pack object for spin data from a field sweep 
	AnalyzedSpinPack->init();
	flag=AnalyzedSpinPack->ReadSpinPackTXT(SpinFile);

if (flag)
 {
	int AcS=AnalyzedSpinPack->ACTSTEP; //Actual size of selected array 

	//read quad fitting coefficients and original dS/dB data
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	double QuadCoef[4][VertSubSec*2]={0.0}; 
	for (int i=0; i<4;i++)
	{
		for(int j=0; j<VertSubSec*2;j++)
		{
			QuadCoef[i][j]=AnalyzedSpinPack->QuadCoef[i][j];
		}
	}

	double dSdB[SWPSTPMAX-1][VertSubSec*2+1]={0.0}; //array to store average spin, plus sweeped value at each step
    for (int i=0; i<AcS-1;i++)
	{
		for(int j=0; j<VertSubSec*2+1;j++)
		{
			    dSdB[i][j]=AnalyzedSpinPack->dSdB[i][j];
		    }
	}

	//update zero crossing data on dialog
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	string Crossings[VertSubSec*2]={""};
	for (int i=0; i<VertSubSec*2; i++)
	{
		Crossings[i]=to_string(QuadCoef[3][i])+"uA";
	}
	m_BPYP=Crossings[0].c_str();
	m_BPYC=Crossings[1].c_str();
	m_BPYN=Crossings[2].c_str();
	m_BNYP=Crossings[3].c_str();
	m_BNYC=Crossings[4].c_str();
	m_BNYN=Crossings[5].c_str();

	//calculate and update gradients: calibration factors to be determined (~0.039G/cm/A)
	double BPBY=(QuadCoef[3][1]-QuadCoef[3][2])/(5*0.36); //use +Z side only, -Z side has poor resolution; prefactor means 1pixel=0.36cm
	double BPBZ=(QuadCoef[3][1]+QuadCoef[3][2])/2-(QuadCoef[3][4]+QuadCoef[3][5])/2; //use center+bottom sections on each side
	string Gpy=to_string(BPBY)+"uG/cm";
	string Gpz=to_string(BPBZ)+"uG/cm";
	m_DPDY=Gpy.c_str();
	m_DPDZ=Gpz.c_str();

	//Calculate quad fitting data using imported coefs
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	const unsigned int refS=120; //refined array 
	double  SpinLF[refS][VertSubSec*2+1]={0.0}; //fitting dS/dB curves
	double stepsize=(dSdB[AcS-2][VertSubSec*2]-dSdB[0][VertSubSec*2])/100.0;
	for (int i=0; i<refS; i++)
	{
		SpinLF[i][VertSubSec*2]=stepsize*double(i-10)+dSdB[0][VertSubSec*2];
		for(int k=0;k<=VertSubSec*2-1;k++)
			{
				SpinLF[i][k]=QuadCoef[0][k]+QuadCoef[1][k]*SpinLF[i][VertSubSec*2]+QuadCoef[2][k]*pow(SpinLF[i][VertSubSec*2],2);
			}
	}
	//string message="Calculated value: "+to_string(SpinLF[0][VertSubSec*2]);AfxMessageBox(message.c_str());

	//calculating plot range
	double Xaxis[2]={SpinLF[0][VertSubSec*2], SpinLF[refS-1][VertSubSec*2]};
	if (Xaxis[0]>Xaxis[1]) //swap
	{
		double temp=0.0; temp=Xaxis[0]; Xaxis[0]=Xaxis[1]; Xaxis[1]=temp;
	}
	double onestep=abs(dSdB[1][VertSubSec*2]-dSdB[0][VertSubSec*2]); //one step interval between nearby points
	double Yaxis[2]={-2.0/onestep,2.0/onestep}; //set one step maximal dS=2; 

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//plot +Z side data
	unsigned long PlotColors[3]={RGB(0,100,0), RGB(0,0,255), RGB(255,0,0)}; //green, blue, red

	m_Graph_PZCROSS.ClearGraph(); //clear previous plots if any
	m_Graph_PZCROSS.SetFrameStyle(0);
	m_Graph_PZCROSS.SetXLabel("Bias coil p (uA)");
	m_Graph_PZCROSS.SetYLabel("dSdB or dS/dI");
	m_Graph_PZCROSS.SetRange(Xaxis[0], Xaxis[1],  Yaxis[0], Yaxis[1]);
	m_Graph_PZCROSS.SetXGridNumber(10);
	m_Graph_PZCROSS.SetYGridNumber(10); 

	for (unsigned int p=0; p<3; p++) //plot 
	{
	  m_Graph_PZCROSS.AddElement();
      m_Graph_PZCROSS.SetElement(p+1); 
      m_Graph_PZCROSS.SetElementPointColor(PlotColors[p]);
      m_Graph_PZCROSS.SetElementLinetype(5); //no solid line
	  m_Graph_PZCROSS.SetElementPointSymbol(2); //rectangles
	  m_Graph_PZCROSS.SetElementWidth(4);
	  m_Graph_PZCROSS.SetElementSolidPoint(FALSE);

	  for(int k=0;k<AcS-1;k++)
	  {
	     m_Graph_PZCROSS.PlotXY(dSdB[k][VertSubSec*2], dSdB[k][p], p+1); 
	  }
	}

	for (unsigned int p=0; p<3; p++)
	{
	  m_Graph_PZCROSS.AddElement();
      m_Graph_PZCROSS.SetElement(p+4); 
      m_Graph_PZCROSS.SetElementLineColor(PlotColors[p]);
      m_Graph_PZCROSS.SetElementLinetype(0); //solid line
	  m_Graph_PZCROSS.SetElementPointSymbol(0); //no symbols
	  m_Graph_PZCROSS.SetElementWidth(1);

	  for(int k=0;k<refS;k++)
	  {
	     m_Graph_PZCROSS.PlotXY(SpinLF[k][VertSubSec*2], SpinLF[k][p], p+4); 
	  }
	}

	//plot -Z side data
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	m_Graph_NZCROSS.ClearGraph(); //clear previous plots if any
	m_Graph_NZCROSS.SetFrameStyle(0);
	m_Graph_NZCROSS.SetXLabel("Bias coil p (uA)");
	m_Graph_NZCROSS.SetYLabel("dS/dB or dS/dI");
	m_Graph_NZCROSS.SetRange(Xaxis[0], Xaxis[1],  Yaxis[0], Yaxis[1]);
	m_Graph_NZCROSS.SetXGridNumber(10);
	m_Graph_NZCROSS.SetYGridNumber(10); 

	for (unsigned int p=0; p<3; p++) //plot 
	{
	  m_Graph_NZCROSS.AddElement();
      m_Graph_NZCROSS.SetElement(p+1); 
      m_Graph_NZCROSS.SetElementPointColor(PlotColors[p]);
      m_Graph_NZCROSS.SetElementLinetype(5); //no solid line
	  m_Graph_NZCROSS.SetElementPointSymbol(2); //rectangles
	  m_Graph_NZCROSS.SetElementWidth(4);
	  m_Graph_NZCROSS.SetElementSolidPoint(FALSE);

	  for(int k=0;k<AcS-1;k++)
	  {
	     m_Graph_NZCROSS.PlotXY(dSdB[k][VertSubSec*2], dSdB[k][p+3], p+1); 
	  }
	}

	for (unsigned int p=0; p<3; p++)
	{
	  m_Graph_NZCROSS.AddElement();
      m_Graph_NZCROSS.SetElement(p+4); 
      m_Graph_NZCROSS.SetElementLineColor(PlotColors[p]);
      m_Graph_NZCROSS.SetElementLinetype(0); //solid line
	  m_Graph_NZCROSS.SetElementPointSymbol(0); //no symbols
	  m_Graph_NZCROSS.SetElementWidth(1);

	  for(int k=0;k<refS;k++)
	  {
	     m_Graph_NZCROSS.PlotXY(SpinLF[k][VertSubSec*2], SpinLF[k][p+3], p+4); 
	  }
	}

 }//end of if(flag)
 UpdateData(false);
 delete AnalyzedSpinPack;
}


//Displot analyzed spin precession data using S(t) sine fit method
void MagnetometryDlg::OnBnClickedButtSine()
{
	bool flag=false;

    SpinPack *AnalyzedSpinPack=new SpinPack(); //Spin Pack object for spin data from a field sweep 
	AnalyzedSpinPack->init();
	flag=AnalyzedSpinPack->ReadSpinPackTXT(SpinFile);

if (flag)
 {
	int AcS=AnalyzedSpinPack->ACTSTEP; //Actual size of selected array 

	//read linear fitting coefficients/Zero Crossings and original spin data
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	double Coefs[4][VertSubSec*2]={0.0}; 
	for (int i=0; i<4;i++)
	{
		for(int j=0; j<VertSubSec*2;j++)
		{
			Coefs[i][j]=AnalyzedSpinPack->SineCoef[i][j];
		}
	}

	double Spin[SWPSTPMAX][VertSubSec*2+1]={0.0}; //array to store average spin, plus sweeped value at each step
	for (int i=0; i<AcS;i++)
	{
		for(int j=0; j<VertSubSec*2+1;j++)
		    {
			    Spin[i][j]=AnalyzedSpinPack->Spin[i][j];
		    }
	}

	//update zero crossing data on dialog
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	string Crossings[VertSubSec*2]={""};
	for (int i=0; i<VertSubSec*2; i++)
	{
		Crossings[i]=to_string(Coefs[3][i])+"uG";
	}
	m_BPYP=Crossings[0].c_str();
	m_BPYC=Crossings[1].c_str();
	m_BPYN=Crossings[2].c_str();
	m_BNYP=Crossings[3].c_str();
	m_BNYC=Crossings[4].c_str();
	m_BNYN=Crossings[5].c_str();

	//calculate and update gradients: calibration factors to be determined (~0.039G/cm/A)
	double BPBY=(Coefs[3][1]-Coefs[3][2])/(5*0.36); //use +Z side only, -Z side has poor resolution; prefactor means 1pixel=0.36cm
	double BPBZ=(Coefs[3][1]+Coefs[3][2])/2-(Coefs[3][4]+Coefs[3][5])/2; //use center+bottom sections on each side
	string Gpy=to_string(BPBY)+"uG/cm";
	string Gpz=to_string(BPBZ)+"uG/cm";
	m_DPDY=Gpy.c_str();
	m_DPDZ=Gpz.c_str();

	//Calculate linear fitting data using imported coefs
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	const unsigned int refS=120; //refined array 
	double  SpinLF[refS][VertSubSec*2+1]={0.0};
	double stepsize=(Spin[AcS-1][VertSubSec*2]-Spin[0][VertSubSec*2])/100.0;
	for (int i=0; i<refS; i++)
	{
		SpinLF[i][VertSubSec*2]=stepsize*double(i-10)+Spin[0][VertSubSec*2];
		for(int k=0;k<=VertSubSec*2-1;k++)
			{
				SpinLF[i][k]=Coefs[0][k]*(sin(TPI*Coefs[1][k]*SpinLF[i][VertSubSec*2]*0.001+Coefs[2][k])-sin(Coefs[2][k]));
			}
	}
	//string message="Calculated value: "+to_string(SpinLF[0][VertSubSec*2]);AfxMessageBox(message.c_str());

	//calculating plot range
	double Xaxis[2]={SpinLF[0][VertSubSec*2], SpinLF[refS-1][VertSubSec*2]};
	if (Xaxis[0]>Xaxis[1]) //swap
	{
		double temp=0.0; temp=Xaxis[0]; Xaxis[0]=Xaxis[1]; Xaxis[1]=temp;
	}

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//plot +Z side data
	unsigned long PlotColors[3]={RGB(0,100,0), RGB(0,0,255), RGB(255,0,0)}; //green, blue, red

	m_Graph_PZCROSS.ClearGraph(); //clear previous plots if any
	m_Graph_PZCROSS.SetFrameStyle(0);
	m_Graph_PZCROSS.SetXLabel("Time (ms)");
	m_Graph_PZCROSS.SetYLabel("Spin");
	m_Graph_PZCROSS.SetRange(Xaxis[0], Xaxis[1],  -3.5, 3.5);
	m_Graph_PZCROSS.SetXGridNumber(10);
	m_Graph_PZCROSS.SetYGridNumber(10); 

	for (unsigned int p=0; p<3; p++) //plot 
	{
	  m_Graph_PZCROSS.AddElement();
      m_Graph_PZCROSS.SetElement(p+1); 
      m_Graph_PZCROSS.SetElementPointColor(PlotColors[p]);
      m_Graph_PZCROSS.SetElementLinetype(5); //no solid line
	  m_Graph_PZCROSS.SetElementPointSymbol(2); //rectangles
	  m_Graph_PZCROSS.SetElementWidth(4);
	  m_Graph_PZCROSS.SetElementSolidPoint(FALSE);

	  for(int k=0;k<AcS;k++)
	  {
	     m_Graph_PZCROSS.PlotXY(Spin[k][VertSubSec*2], Spin[k][p], p+1); 
	  }
	}

	for (unsigned int p=0; p<3; p++)
	{
	  m_Graph_PZCROSS.AddElement();
      m_Graph_PZCROSS.SetElement(p+4); 
      m_Graph_PZCROSS.SetElementLineColor(PlotColors[p]);
      m_Graph_PZCROSS.SetElementLinetype(0); //solid line
	  m_Graph_PZCROSS.SetElementPointSymbol(0); //no symbols
	  m_Graph_PZCROSS.SetElementWidth(1);

	  for(int k=0;k<refS;k++)
	  {
	     m_Graph_PZCROSS.PlotXY(SpinLF[k][VertSubSec*2], SpinLF[k][p], p+4); 
	  }
	}

	//plot -Z side data
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	m_Graph_NZCROSS.ClearGraph(); //clear previous plots if any
	m_Graph_NZCROSS.SetFrameStyle(0);
	m_Graph_NZCROSS.SetXLabel("Time (ms)");
	m_Graph_NZCROSS.SetYLabel("Spin");
	m_Graph_NZCROSS.SetRange(Xaxis[0], Xaxis[1], -3.5, 3.5);
	m_Graph_NZCROSS.SetXGridNumber(10);
	m_Graph_NZCROSS.SetYGridNumber(10); 

	for (unsigned int p=0; p<3; p++) //plot 
	{
	  m_Graph_NZCROSS.AddElement();
      m_Graph_NZCROSS.SetElement(p+1); 
      m_Graph_NZCROSS.SetElementPointColor(PlotColors[p]);
      m_Graph_NZCROSS.SetElementLinetype(5); //no solid line
	  m_Graph_NZCROSS.SetElementPointSymbol(2); //rectangles
	  m_Graph_NZCROSS.SetElementWidth(4);
	  m_Graph_NZCROSS.SetElementSolidPoint(FALSE);

	  for(int k=0;k<AcS;k++)
	  {
	     m_Graph_NZCROSS.PlotXY(Spin[k][VertSubSec*2], Spin[k][p+3], p+1); 
	  }
	}

	for (unsigned int p=0; p<3; p++)
	{
	  m_Graph_NZCROSS.AddElement();
      m_Graph_NZCROSS.SetElement(p+4); 
      m_Graph_NZCROSS.SetElementLineColor(PlotColors[p]);
      m_Graph_NZCROSS.SetElementLinetype(0); //solid line
	  m_Graph_NZCROSS.SetElementPointSymbol(0); //no symbols
	  m_Graph_NZCROSS.SetElementWidth(1);

	  for(int k=0;k<refS;k++)
	  {
	     m_Graph_NZCROSS.PlotXY(SpinLF[k][VertSubSec*2], SpinLF[k][p+3], p+4); 
	  }
	}

 }//end of if(flag)

 UpdateData(false);
 delete AnalyzedSpinPack;
	
}
