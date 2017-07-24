
// PushTestDlg.h : 头文件
//

#pragma once
#include "afxcmn.h"
#include "afxwin.h"

#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <opencv2\opencv.hpp>
#include <iostream>
#include <stdio.h>
#include "highgui.h"
#include <sstream>
#include <fstream>
#include "RtmpPublisher.h"

using namespace std;
using namespace cv;

#define LIVE_TIMER_ID	1

//#define 

// CPushTestDlg 对话框
class CPushTestDlg : public CDialogEx
{
// 构造
public:
	CPushTestDlg(CWnd* pParent = NULL);	// 标准构造函数
	~CPushTestDlg();

// 对话框数据
	enum { IDD = IDD_PUSHTEST_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持

// 实现
protected:
	HICON m_hIcon;

	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

	afx_msg void OnBnClickedButtonLiveStart();
	afx_msg void OnBnClickedButtonLiveStop();
	afx_msg void OnBnClickedButtonSendMeta();
	afx_msg void OnBnClickedButtonAdditem();
	afx_msg void OnBnClickedButtonDelitem();
	afx_msg void OnBnClickedButtonOpenvideo();
	afx_msg void OnBnClickedButtonClosevideo();
	afx_msg void OnBnClickedButtonInfoClear();
	afx_msg void OnClose();
	afx_msg void OnBnClickedAudioEnable();

	static void CALLBACK UxTimerCallback(DWORD dwEventID, void* pThis);
	void UxTimer_Callback(DWORD dwEventID);

	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);

	static DWORD WINAPI Th_OpenVideo_Cmd(LPVOID lpParam);

private:
	CEdit			m_edtVideoIndex;
	int				m_iVideoIndex;

	// 视频;
	CEdit		m_edtVideoWidth;
	CEdit		m_edtVideoHeight;
	CEdit		m_edtVideoBps;
	CEdit		m_edtVideoFps;
	CEdit		m_ediVideoQuality;
	CEdit		m_edtVideoInterval;

	// 音频;
	CButton		m_btnAudioEnable;
	CEdit		m_edtAudioBps;
	CEdit		m_edtAudioChannel;
	CEdit		m_edtAudioBits;
	CEdit		m_edtAudioSample;

	CListCtrl		m_listMeta;
	CEdit			m_edtPushUrl;
	CRichEditCtrl	m_edtInfo;

	Mat				m_matFrame;
	VideoCapture	m_capVideo;
	bool			m_bVideoOpened;

	RtmpPublisher*	m_pRtmpPublisher;

	VideoInfo		m_videoInfo;
	AudioInfo		m_audioInfo;

	CMenu			m_Menu;
public:
	afx_msg void OnAboutMenu();
};