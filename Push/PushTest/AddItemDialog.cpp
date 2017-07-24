// AddItemDialog.cpp : 实现文件
//

#include "stdafx.h"
#include "PushTest.h"
#include "AddItemDialog.h"
#include "afxdialogex.h"


// CAddItemDialog 对话框

IMPLEMENT_DYNAMIC(CAddItemDialog, CDialogEx)

CAddItemDialog::CAddItemDialog(CWnd* pParent /*=NULL*/)
	: CDialogEx(CAddItemDialog::IDD, pParent)
{

}

CAddItemDialog::~CAddItemDialog()
{
}

void CAddItemDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_KEY, m_edtKey);
	DDX_Control(pDX, IDC_EDIT_VALUE, m_edtValue);
	DDX_Control(pDX, IDC_COMBO_TYPE, m_cmbType);
}


BEGIN_MESSAGE_MAP(CAddItemDialog, CDialogEx)
	ON_BN_CLICKED(ID_BUTTON_OK, &CAddItemDialog::OnBnClickedOK)
END_MESSAGE_MAP()

void CAddItemDialog::OnBnClickedOK()
{
	m_edtKey.GetWindowText(m_strKey);
	m_edtValue.GetWindowText(m_strValue);
	m_cmbType.GetWindowText(m_strType);

	EndDialog(IDOK);
}

BOOL CAddItemDialog::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	m_cmbType.InsertString(0, _T("String"));
	m_cmbType.InsertString(1, _T("Number"));
	m_cmbType.SetCurSel(0);
	return TRUE;
}