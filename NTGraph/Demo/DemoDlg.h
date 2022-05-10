// DemoDlg.h : header file
//

#if !defined(AFX_DEMODLG_H__A4379986_9367_44B2_8063_467974776A21__INCLUDED_)
#define AFX_DEMODLG_H__A4379986_9367_44B2_8063_467974776A21__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "NTGraph.h"
/////////////////////////////////////////////////////////////////////////////
// CDemoDlg dialog

class CDemoDlg : public CDialog
{
// Construction
public:
	CDemoDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CDemoDlg)
	enum { IDD = IDD_DEMO_DIALOG };
	BOOL	m_bStacked;
	int		m_nElementsSlide;
	int		m_nPointsSlide;
	CNTGraph	m_Graph1;
	CNTGraph	m_Graph2;
	int		m_nZoom;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDemoDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void SetScopeFace(CNTGraph *graph);
	void SetPlotFace(CNTGraph *graph);
	void Demo1();
	void Demo2();
	void Demo3(int);
	void Demo4(int);
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CDemoDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnButtonDemo();
	afx_msg void OnProperties();
	afx_msg void OnRunDemos();
	afx_msg void OnButtonDemos();
	DECLARE_EVENTSINK_MAP()
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DEMODLG_H__A4379986_9367_44B2_8063_467974776A21__INCLUDED_)
