// ParaSweepDlg.cpp : implementation file
//

#include "stdafx.h"
#include "MASTER.h"
#include "ParaSweepDlg.h"
#include "afxdialogex.h"


// ParaSweepDlg dialog

IMPLEMENT_DYNAMIC(ParaSweepDlg, CDialogEx)

ParaSweepDlg::ParaSweepDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(ParaSweepDlg::IDD, pParent)
	, m_SWP_File(0)
	, m_SWP_Cell(_T("D4"))
	, m_SWP_Final(1)
	, m_SWP_Initial(0)
	, m_SWP_Steps(2)
	, m_SWP_Type(0)
{
	m_DoStartScan=FALSE;
}

ParaSweepDlg::~ParaSweepDlg()
{
}

void ParaSweepDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_CBIndex(pDX, IDC_COMBO_SWPFILE, m_SWP_File);
	DDX_Text(pDX, IDC_EDIT_SWPCELL, m_SWP_Cell);
	DDX_Text(pDX, IDC_EDIT_SWPFINAL, m_SWP_Final);
	DDX_Text(pDX, IDC_EDIT_SWPINIT, m_SWP_Initial);
	DDX_Text(pDX, IDC_EDIT_SWPSTEPS, m_SWP_Steps);
	DDX_CBIndex(pDX, IDC_COMBO_SWPTYPE, m_SWP_Type);
}


BEGIN_MESSAGE_MAP(ParaSweepDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, &ParaSweepDlg::OnBnClickedOk)
	ON_BN_CLICKED(ID_PARADLG_CANCEL, &ParaSweepDlg::OnBnClickedParadlgCancel)
END_MESSAGE_MAP()


// ParaSweepDlg message handlers


void ParaSweepDlg::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	m_DoStartScan=TRUE;
	UpdateData();
	CDialogEx::OnOK();
}


void ParaSweepDlg::OnBnClickedParadlgCancel()
{
	// TODO: Add your control notification handler code here
	m_DoStartScan=FALSE;
	CDialogEx::OnCancel();
}
