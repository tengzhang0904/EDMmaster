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
using namespace DATACONST;
using namespace std;

// StateMapDlg dialog

class StateMapDlg : public CDialogEx
{
	DECLARE_DYNAMIC(StateMapDlg)

public:
	StateMapDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~StateMapDlg();

// Dialog Data
	enum { IDD = IDD_STATE_MAP };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CString m_StateMapMessage;
	CNtgraphctrl m_Graph_StateMap;

	afx_msg void OnBnClickedButtonPlotmap();
};
