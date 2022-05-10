// MagnetometryGradDlg.cpp : implementation file
//

#include "stdafx.h"
#include "MASTER.h"
#include "MagnetometryGradDlg.h"
#include "afxdialogex.h"
#include "SpinPackSWP.h" //multi-location spin pack for a scan: parent class

// MagnetometryGradDlg dialog

IMPLEMENT_DYNAMIC(MagnetometryGradDlg, CDialogEx)

MagnetometryGradDlg::MagnetometryGradDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(MagnetometryGradDlg::IDD, pParent)
	, m_SelGradZeroCross(_T(""))
{

}

MagnetometryGradDlg::~MagnetometryGradDlg()
{
}

void MagnetometryGradDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_GRADSCAN_ZEROCROS, m_SelGradZeroCross);
	DDX_Control(pDX, IDC_NTGRAPHCTRL_GRADPLOT, m_GRADPLOT);
}


BEGIN_MESSAGE_MAP(MagnetometryGradDlg, CDialogEx)
	ON_BN_CLICKED(IDC_RADIO_GPY, &MagnetometryGradDlg::OnBnClickedRadioGpy)
	ON_BN_CLICKED(IDC_RADIO_GPZ, &MagnetometryGradDlg::OnBnClickedRadioGpz)
END_MESSAGE_MAP()


// MagnetometryGradDlg message handlers

void MagnetometryGradDlg::OnBnClickedRadioGpy()
{
	PlotSelected(0);
}


void MagnetometryGradDlg::OnBnClickedRadioGpz()
{
	PlotSelected(1);
}


//filename for analyzed gradient scan data
string MMCfolderG="C:/MMC_SYSINT/";
string GradFile=MMCfolderG+"LAB_DAT/"+"GradAnalyzed.txt";


void MagnetometryGradDlg::PlotSelected(int selection)
{
	SetWindowPos(NULL, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOZORDER);//set dialog window position tp upper left

	string Message=""; //display fitting result


	bool flag=false;

    SpinPack *AnalyzedGradPack=new SpinPack(); //Spin Pack object for spin data from a field sweep 
	AnalyzedGradPack->init();
	flag=AnalyzedGradPack->ReadGradPackTXT(GradFile);

 if (flag)
 {
	int AcS=AnalyzedGradPack->ACTSTEP; //Actual size of selected array 

	//read linear fitting coefficients/Zero Crossings, and original Gradient data
	double LinearCoef[3][2]={0.0}; //linear fitting coefficients for gradients zero crossing
	for (int i=0; i<3;i++)
	{
		for(int j=0; j<2;j++)
		{
			LinearCoef[i][j]=AnalyzedGradPack->GradLinCoef[i][j];
		}
	}
	double Gradient[SWPSTPMAX][3]={0.0}; //array to store 2 gradients, plus sweeped value at each step
	for (int i=0; i<AcS;i++)
	{
		for(int j=0; j<3;j++)
		{
			Gradient[i][j]=AnalyzedGradPack->Gradient[i][j];;
		}
	}

	//Calculate linear fitting data using imported coefs
	const unsigned int refS=120; //refined array 
	double  GradLF[refS][3]={0.0};
	double stepsize=(Gradient[AcS-1][2]-Gradient[0][2])/100.0;
	for (int i=0; i<refS; i++)
	{
		GradLF[i][2]=stepsize*double(i-10)+Gradient[0][2];
		for(int k=0;k<=1;k++)
			{
				GradLF[i][k]=LinearCoef[0][k]+LinearCoef[1][k]*GradLF[i][2];
			}
	}
	//string message="Calculated value: "+to_string(GradLF[60][2]);AfxMessageBox(message.c_str());

	//calculating plot range
	double Xaxis[2]={GradLF[0][2], GradLF[refS-1][2]};
	double Yaxis[2]={GradLF[0][0], GradLF[refS-1][0]};
	double YZaxis[2]={GradLF[0][1], GradLF[refS-1][1]};
	if (Xaxis[0]>Xaxis[1]) //swap
	{
		double temp=0.0; temp=Xaxis[0]; Xaxis[0]=Xaxis[1]; Xaxis[1]=temp;
	}
	if (Yaxis[0]>Yaxis[1]) //swap
	{
		double temp=0.0; temp=Yaxis[0]; Yaxis[0]=Yaxis[1]; Yaxis[1]=temp;
	}
	if (YZaxis[0]>YZaxis[1]) //swap
	{
		double temp=0.0; temp=YZaxis[0]; YZaxis[0]=YZaxis[1]; YZaxis[1]=temp;
	}

	//plot and display all data
	if (selection==0)
	     {
		     Message+="Gpy ZR: "+to_string(LinearCoef[2][0]);

			 m_GRADPLOT.ClearGraph(); //clear previous plots if any
			 m_GRADPLOT.SetFrameStyle(0);
	         m_GRADPLOT.SetXLabel("Gradient Coil (uA)");
	         m_GRADPLOT.SetYLabel("Selected Gradient");
	         m_GRADPLOT.SetXGridNumber(10);
	         m_GRADPLOT.SetYGridNumber(10); 
	         m_GRADPLOT.SetRange(floor(Xaxis[0]), ceil(Xaxis[1]), floor(Yaxis[0]), ceil(Yaxis[1]));

			 m_GRADPLOT.AddElement();
             m_GRADPLOT.SetElement(1); 
             m_GRADPLOT.SetElementPointColor(RGB(255,0,0));
             m_GRADPLOT.SetElementLinetype(5); //no solid line
	         m_GRADPLOT.SetElementPointSymbol(2); //rectangles
	         m_GRADPLOT.SetElementWidth(4);
	         m_GRADPLOT.SetElementSolidPoint(FALSE);
	         for(int k=0;k<AcS;k++)
	         {
	             m_GRADPLOT.PlotXY(Gradient[k][2], Gradient[k][0], 1); 
	         }

			 m_GRADPLOT.AddElement();
             m_GRADPLOT.SetElement(2); 
             m_GRADPLOT.SetElementLineColor(RGB(0,0,255));
             m_GRADPLOT.SetElementLinetype(0); //solid line
	         m_GRADPLOT.SetElementPointSymbol(0); //no symbols
	         m_GRADPLOT.SetElementWidth(1);
	         for(int k=0;k<refS;k++)
	         {
	            m_GRADPLOT.PlotXY(GradLF[k][2], GradLF[k][0], 2); 
	         }
	     }//end of selection=0

	  else if (selection==1)
	     {
		     Message+="Gpz ZR: "+to_string(LinearCoef[2][1]);

			 m_GRADPLOT.ClearGraph(); //clear previous plots if any
			 m_GRADPLOT.SetFrameStyle(0);
	         m_GRADPLOT.SetXLabel("Gradient Coil (uA)");
	         m_GRADPLOT.SetYLabel("Selected Gradient");
	         m_GRADPLOT.SetXGridNumber(10);
	         m_GRADPLOT.SetYGridNumber(10); 
	         m_GRADPLOT.SetRange(floor(Xaxis[0]), ceil(Xaxis[1]), floor(YZaxis[0]), ceil(YZaxis[1]));

			 m_GRADPLOT.AddElement();
             m_GRADPLOT.SetElement(1); 
             m_GRADPLOT.SetElementPointColor(RGB(255,0,0));
             m_GRADPLOT.SetElementLinetype(5); //no solid line
	         m_GRADPLOT.SetElementPointSymbol(2); //rectangles
	         m_GRADPLOT.SetElementWidth(4);
	         m_GRADPLOT.SetElementSolidPoint(FALSE);
	         for(int k=0;k<AcS;k++)
	         {
	             m_GRADPLOT.PlotXY(Gradient[k][2], Gradient[k][1], 1); 
	         }

			 m_GRADPLOT.AddElement();
             m_GRADPLOT.SetElement(2); 
             m_GRADPLOT.SetElementLineColor(RGB(0,0,255));
             m_GRADPLOT.SetElementLinetype(0); //solid line
	         m_GRADPLOT.SetElementPointSymbol(0); //no symbols
	         m_GRADPLOT.SetElementWidth(1);
	         for(int k=0;k<refS;k++)
	         {
	            m_GRADPLOT.PlotXY(GradLF[k][2], GradLF[k][1], 2); 
	         }
	     } //end of selection=1
 } //end of if(flag)

 else 
 {
     Message="GradAnalyzed.txt file not found";
 }

 m_SelGradZeroCross=Message.c_str();
 this->UpdateData(FALSE);

 delete AnalyzedGradPack;

}