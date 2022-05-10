// LoopCtrlUIDlg.cpp : implementation file
//

#include "stdafx.h"
#include "MASTER.h"
#include "LoopCtrlUIDlg.h"
#include "afxdialogex.h"


// LoopCtrlUIDlg dialog

IMPLEMENT_DYNAMIC(LoopCtrlUIDlg, CDialogEx)

LoopCtrlUIDlg::LoopCtrlUIDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(LoopCtrlUIDlg::IDD, pParent) //Initializer list
	, m_MAG_SCAN_header(_T("B"))
	, m_LarmorThreshold(1000.1)
	, m_MAG_SCAN_MAX_ITERATIONS(3)
	, m_EDM_SCAN_header(_T("D"))
	, m_EDM_SCAN_ITERATIONS(2)
	, m_EDM_SCAN_STEPS(64)
	, m_AutoMasterLoop_REPEATS(1)
{
	UpdateAndStart=FALSE; //Construct (after Initializer)
}

LoopCtrlUIDlg::~LoopCtrlUIDlg()
{
}

void LoopCtrlUIDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_MAG_SCAN_header, m_MAG_SCAN_header);
	DDX_Text(pDX, IDC_EDIT_LarmorThreshold, m_LarmorThreshold);
	DDX_Text(pDX, IDC_EDIT_MAG_SCAN_MAX_ITERATIONS, m_MAG_SCAN_MAX_ITERATIONS);
	DDX_Text(pDX, IDC_EDIT_EDM_SCAN_header, m_EDM_SCAN_header);
	DDX_Text(pDX, IDC_EDIT_EDM_SCAN_ITERATIONS, m_EDM_SCAN_ITERATIONS);
	DDX_Text(pDX, IDC_EDIT_EDM_SCAN_STEPS, m_EDM_SCAN_STEPS);
	DDX_Text(pDX, IDC_EDIT_AutoMasterLoop_REPEATS, m_AutoMasterLoop_REPEATS);
}


BEGIN_MESSAGE_MAP(LoopCtrlUIDlg, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON_MASTERLOOP_CONFIRM, &LoopCtrlUIDlg::OnBnClickedButtonMasterloopConfirm)
END_MESSAGE_MAP()


// LoopCtrlUIDlg message handlers


void LoopCtrlUIDlg::OnBnClickedButtonMasterloopConfirm()
{
	// TODO: Add your control notification handler code here
	UpdateData();
	UpdateAndStart=TRUE;
	CDialogEx::OnOK();
}
