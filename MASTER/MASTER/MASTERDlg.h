
// MASTERDlg.h : header file
//

#pragma once
#include "afxwin.h"
#include "ntgraphctrl.h"

#include "CONST.h"
#include "afxcmn.h"
#include <string>
#include <vector>
using namespace DATACONST;

// CMASTERDlg dialog
class CMASTERDlg : public CDialogEx
{
// Construction
public:
	CMASTERDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_MASTER_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnEnChangeEditloops();
	afx_msg void OnCbnSelchangeCombosequence();
	afx_msg void OnUsefullinksMfc();
	afx_msg void OnUsefullinksExcellink();
	afx_msg void OnUsefullinksWinsock();
	afx_msg void OnUsefullinksNidaqmx();
	afx_msg void OnUsefullinksNivisa();
    afx_msg void OnProtocolsALGLIB();
	afx_msg void OnProtocolsNtgraph();
	afx_msg void OnHelpSourcecode();
	afx_msg void OnHelpDocumentation();
	afx_msg void OnBnClickedButtonmicrowave();
	afx_msg void OnBnClickedButtonaudiotrans();
	afx_msg void OnBnClickedButtoncurrentsource();
	afx_msg void OnBnClickedButtondigital();
	afx_msg void OnAtomicSupertimecommunication();
	int Sequence_id;
	CString loopnum;
	CComboBox m_SEQNAME;
	afx_msg void OnMicrowaveddsCalculateddsdata();
	afx_msg void OnMicrowaveddsOutputddsdataonce();
	afx_msg void OnMicrowaveddsCalculateandoutput();
	afx_msg void OnAtomicAudiotransitions();
	afx_msg void OnAtomicUpdateasyndigitallines();
	afx_msg void OnHighvoltageMonitorvoltage();
	afx_msg void OnHighvoltageMonitorleakagecurrent();
	CString m_HVVOL;
	CString m_HVLEAK;
	afx_msg void OnSingleAnalogmonitors();
	CString m_CAVITYP;
	CString m_CAVITYN;
	CString m_EXTBX;
	CString m_EXTBY;
	CString m_EXTBZ;
	CString m_BEAM1;
	CString m_BEAM2;
	CString m_BEAM3;
	BOOL m_LASERLOCKED;
	afx_msg void OnBnClickedCheckLaser();
	CString m_TEMPERATURE;
	afx_msg void OnSingleTemperature();
	CNtgraphctrl m_GRAPH_PDA;
	float Curve[PDANpixel][PDANshot];
	afx_msg void OnSinglePdaimaging();
	afx_msg void OnBnClickedButtonParasweep();
	BOOL m_Parallel_Audio;
	BOOL m_Parallel_DDS;
	BOOL m_Parallel_HV;
	BOOL m_Parallel_Monitor;
	BOOL m_Parallel_PDA;
	BOOL m_Parallel_ExtField;
	BOOL m_Parallel_Supertime;
	BOOL m_Parallel_Temp;
	afx_msg void OnBnClickedCheckdds();
	afx_msg void OnBnClickedCheckpda();
	afx_msg void OnBnClickedCheckaudio();
	afx_msg void OnBnClickedCheckhv();
	afx_msg void OnBnClickedChecksupertime();
	afx_msg void OnBnClickedCheckmonitor();
	afx_msg void OnBnClickedChecktemp();
	afx_msg void OnBnClickedCheckExtField();
	afx_msg void OnCompositeParalleltasking();
	afx_msg void OnDatabaseSave();
	afx_msg void OnDatabaseLoadpaneldata();
	CString m_DATASET_FILE;
	afx_msg void OnBnClickedButtonConvertmat();
	afx_msg void OnBnClickedButtonConvertxls();
	CProgressCtrl m_SWPPROGRESSCtrl;
	afx_msg void OnNMCustomdrawProgressswp(NMHDR *pNMHDR, LRESULT *pResult);
	CString m_BULLETIN;
	afx_msg void OnMenuPackavg();
	int m_PACIDXstart;
	int m_PACIDXend;
	CRichEditCtrl m_RICHEDIT_RPTCTRL;
	CString m_RICHEDIT_REPORT;
	afx_msg void OnAtomicCurrentsource();
	afx_msg void OnAtomicHardwarepatternmatch();
	afx_msg void OnDatapacklisting();
	afx_msg void OnCompositeMagnetometryBiasScan();
	float SWPPARA[4]; //array to character a typical parameter sweep: inital, final, steps, type
	afx_msg void OnMenuMagnetometryGradScan();
	afx_msg void OnCompositeLoadspindatasweep();
	afx_msg bool ComputeAvgSpin(); //compute average spin for a list of data packs
	afx_msg void OnCompositeAnalyzegrad();
	afx_msg void OnCompositeSpinsweeptruncate();
	afx_msg void OnAtomicPausetiming();
	afx_msg void TurnOn1MHzCLK();
	afx_msg void OnCompositeEdmSingleScan();
	afx_msg void OnAnalyzeViewedmscandata();
	double m_Condition_A;
	double m_Condition_B;

	afx_msg void OnCompositeAutomatededm(); //EDM master loop
	std::vector<double> MAG_Setting; //Magnetic coil settings for Master loop (1 time parameter + 3 bias + 5 gradients)
	BOOL AutoEDMScan(std::string fileheader, double *BzRange, unsigned int EDM_SCAN_STEPS); //Auto_EDM_SCAN function for EDM master loop
	BOOL AutoMAGScan(std::string fileheader, unsigned int MAG_SCAN_Type, double *MAG_SCAN_Range, unsigned int MAG_SCAN_STEPS, double *ScanResults); //Single magnetometry scan function for EDM master loop
	BOOL AutoMAG4(std::string fileheader, double LarmorPeriodThreshold, unsigned int MAX_NUM_Iterations); //Magnetometry (loop structure w/ >=4 magnetometry scans) function for EDM master loop
	afx_msg void OnCompositeAutomagnetometry();
	afx_msg void OnEnChangeHvvol();
	afx_msg void OnBnClickedButtonZero();
};