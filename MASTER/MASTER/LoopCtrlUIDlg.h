//Loop control user interface dialog

#pragma once


// LoopCtrlUIDlg dialog

class LoopCtrlUIDlg : public CDialogEx
{
	DECLARE_DYNAMIC(LoopCtrlUIDlg)

public:
	LoopCtrlUIDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~LoopCtrlUIDlg();

// Dialog Data
	enum { IDD = IDD_DIALOG_LOOPCTL_UI };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CString m_MAG_SCAN_header;
	double m_LarmorThreshold;
	int m_MAG_SCAN_MAX_ITERATIONS;
	CString m_EDM_SCAN_header;
	int m_EDM_SCAN_ITERATIONS;
	int m_EDM_SCAN_STEPS;
	int m_AutoMasterLoop_REPEATS;
	afx_msg void OnBnClickedButtonMasterloopConfirm();
	BOOL UpdateAndStart; //flag to determine whether updating/starting automatic EDM measurements
};
