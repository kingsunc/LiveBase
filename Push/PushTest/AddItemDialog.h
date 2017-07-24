#pragma once
#include "afxwin.h"


// CAddItemDialog 对话框

class CAddItemDialog : public CDialogEx
{
	DECLARE_DYNAMIC(CAddItemDialog)

public:
	CAddItemDialog(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CAddItemDialog();

// 对话框数据
	enum { IDD = IDD_DIALOG_ADDITEM };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
	afx_msg void OnBnClickedOK();

public:
	CEdit m_edtKey;
	CEdit m_edtValue;
	CComboBox m_cmbType;

	CString m_strKey;
	CString m_strValue;
	CString m_strType;
};
