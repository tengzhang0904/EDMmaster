//magnetometry: gradient scan zero crossing

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
#include "afxwin.h"
using namespace DATACONST;
using namespace std;

// MagnetometryGradDlg dialog

class MagnetometryGradDlg : public CDialogEx
{
	DECLARE_DYNAMIC(MagnetometryGradDlg)

public:
	MagnetometryGradDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~MagnetometryGradDlg();

// Dialog Data
	enum { IDD = IDD_MagnetometryGradDlg };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CString m_SelGradZeroCross;
	afx_msg void OnBnClickedRadioGpy();
	afx_msg void OnBnClickedRadioGpz();
	afx_msg void PlotSelected(int selection);
	CNtgraphctrl m_GRADPLOT;
};
