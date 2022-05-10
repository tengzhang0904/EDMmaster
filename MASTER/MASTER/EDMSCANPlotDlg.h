//Plot EDM SCAN Analyzed data

#pragma once

#include <stdio.h>
#include <math.h>
#include <iomanip>
#include <string>
#include <windows.h> 
#include <cstdlib>
#include <iostream>
#include <fstream>
#include "ntgraphctrl.h"
#include "CONST.h"
#include <vector>  //std::vector
using namespace DATACONST;
using namespace std;

//elements of a point
struct SelectPoint
{
	double PTS[4]; //scan para, HV sign, chlA, chlB
};

// EDMSCANPlotDlg dialog

class EDMSCANPlotDlg : public CDialogEx
{
	DECLARE_DYNAMIC(EDMSCANPlotDlg)

public:
	EDMSCANPlotDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~EDMSCANPlotDlg();

// Dialog Data
	enum { IDD = IDD_DIALOG_EDMSCANPLOT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	std::vector<SelectPoint> ScanData; //vector for plotting selected channels during a SCAN
	unsigned int SCANnum; //total SCAN number

	bool ReadSCANana(string filename); //read analyzed SCAN file
	void PlotData(); //Plot ScanData on two graphs
	afx_msg void OnBnClickedButtEdmscanPlotana(); //Plot analyzed SCAN data
	CString m_REDIT_EDMScanReport; //analyzed SCAN report
	string Message;
	BOOL m_Ramsey_fringe_selected;
	BOOL m_ellipse_selected;
	CNtgraphctrl m_Ramsey_Plot;
	CNtgraphctrl m_elliptical_plot;
	afx_msg void OnEnChangeRicheditEdmscanrpt();
};
