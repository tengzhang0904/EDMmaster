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

// MagnetometryDlg dialog

class MagnetometryDlg : public CDialogEx
{
	DECLARE_DYNAMIC(MagnetometryDlg)

public:
	MagnetometryDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~MagnetometryDlg();

// Dialog Data
	enum { IDD = IDD_Magnetometry };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	HICON m_hIcon; // test icon for this dialog
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
public:
	CNtgraphctrl m_Graph_PZCROSS;
	CNtgraphctrl m_Graph_NZCROSS;
	CString m_BPYP;
	CString m_BPYC;
	CString m_BPYN;
	CString m_BNYP;
	CString m_BNYC;
	CString m_BNYN;
	CString m_DPDY;
	CString m_DPDZ;
	afx_msg void OnBnClickedButtonLinearCross();
	afx_msg void AnalyzeDataLinear();
	afx_msg void OnBnClickedButtDsdb();
	afx_msg void AnalyzeDataQuad();
	afx_msg void OnBnClickedButtSine();
	afx_msg void OnEnChangeEditDpdz();
	afx_msg void OnEnChangeEditDpdy();
};
