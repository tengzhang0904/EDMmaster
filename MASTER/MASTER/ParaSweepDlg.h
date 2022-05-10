#pragma once


// ParaSweepDlg dialog

class ParaSweepDlg : public CDialogEx
{
	DECLARE_DYNAMIC(ParaSweepDlg)

public:
	ParaSweepDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~ParaSweepDlg();

// Dialog Data
	enum { IDD = IDD_DIALOG_PARASWEEP };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	int m_SWP_File;
	CString m_SWP_Cell;
	double m_SWP_Final;
	double m_SWP_Initial;
	int m_SWP_Steps;
	int m_SWP_Type;
	afx_msg void OnBnClickedOk();
	BOOL m_DoStartScan;  //flag to determine whether starting a scan
	afx_msg void OnBnClickedParadlgCancel();
};
