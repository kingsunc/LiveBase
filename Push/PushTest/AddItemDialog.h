#pragma once
#include "afxwin.h"


// CAddItemDialog �Ի���

class CAddItemDialog : public CDialogEx
{
	DECLARE_DYNAMIC(CAddItemDialog)

public:
	CAddItemDialog(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CAddItemDialog();

// �Ի�������
	enum { IDD = IDD_DIALOG_ADDITEM };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

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
