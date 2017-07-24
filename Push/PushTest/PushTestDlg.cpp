
// PushTestDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "time.h"
#include <list>
#include "PushTest.h"
#include "PushTestDlg.h"
#include "afxdialogex.h"
#include "AddItemDialog.h"
#include <iostream>
#include "UXTimerQueue.h"

#pragma comment(lib,"ws2_32.lib")

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define MAX_SIZE 255

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
	
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CPushTestDlg 对话框;
CPushTestDlg::CPushTestDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CPushTestDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_iVideoIndex = 0;
}

CPushTestDlg::~CPushTestDlg()
{
	if (m_pRtmpPublisher)
	{
		delete m_pRtmpPublisher;
		m_pRtmpPublisher = NULL;
	}
}

void CPushTestDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_VIDEO_INDEX,	m_edtVideoIndex);
	DDX_Control(pDX, IDC_VIDEO_WIDTH,	m_edtVideoWidth);
	DDX_Control(pDX, IDC_VIDEO_HEIGHT,	m_edtVideoHeight);
	DDX_Control(pDX, IDC_VIDEO_BPS,		m_edtVideoBps);
	DDX_Control(pDX, IDC_VIDEO_FPS,		m_edtVideoFps);
	DDX_Control(pDX, IDC_VIDEO_QUALITY,	m_ediVideoQuality);
	DDX_Control(pDX, IDC_AUDIO_ENABLE,	m_btnAudioEnable);
	DDX_Control(pDX, IDC_AUDIO_BPS,		m_edtAudioBps);
	DDX_Control(pDX, IDC_AUDIO_CHANNEL,	m_edtAudioChannel);
	DDX_Control(pDX, IDC_AUDIO_BITS,	m_edtAudioBits);
	DDX_Control(pDX, IDC_AUDIO_SAMPLE,	m_edtAudioSample);
	DDX_Control(pDX, IDC_VIDEO_INTERVAL, m_edtVideoInterval);
	DDX_Control(pDX, IDC_LIST_META,		m_listMeta);
	DDX_Control(pDX, IDC_EDIT_PUSHURL,	m_edtPushUrl);
	DDX_Control(pDX, IDC_RICHEDIT_INFO,	m_edtInfo);
}

BEGIN_MESSAGE_MAP(CPushTestDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_BUTTON_LIVE_START, &CPushTestDlg::OnBnClickedButtonLiveStart)
	ON_BN_CLICKED(IDC_BUTTON_LIVE_STOP, &CPushTestDlg::OnBnClickedButtonLiveStop)
	ON_BN_CLICKED(IDC_BUTTON_SEND_META, &CPushTestDlg::OnBnClickedButtonSendMeta)
	ON_BN_CLICKED(IDC_BUTTON_ADDITEM, &CPushTestDlg::OnBnClickedButtonAdditem)
	ON_BN_CLICKED(IDC_BUTTON_DELITEM, &CPushTestDlg::OnBnClickedButtonDelitem)
	ON_BN_CLICKED(IDC_BUTTON_OPENVIDEO, &CPushTestDlg::OnBnClickedButtonOpenvideo)
	ON_BN_CLICKED(IDC_BUTTON_CLOSEVIDEO, &CPushTestDlg::OnBnClickedButtonClosevideo)
	ON_BN_CLICKED(IDC_BUTTON_INFO_CLEAR, &CPushTestDlg::OnBnClickedButtonInfoClear)
	ON_BN_CLICKED(IDC_AUDIO_ENABLE, &CPushTestDlg::OnBnClickedAudioEnable)
	ON_COMMAND(32773, &CPushTestDlg::OnAboutMenu)
END_MESSAGE_MAP()

BOOL CPushTestDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	SetIcon(m_hIcon, TRUE);
	SetIcon(m_hIcon, FALSE);

	m_Menu.LoadMenu(IDR_MAIN_MENU);
	SetMenu(&m_Menu);

	if (!m_pRtmpPublisher)
	{
		m_pRtmpPublisher = new RtmpPublisher();
		assert(m_pRtmpPublisher);
		m_pRtmpPublisher->m_hMainWnd = m_hWnd;
	}

	m_edtVideoIndex.SetWindowText(_T("0"));
	m_edtPushUrl.SetWindowText(_T("rtmp://teacher.xescdn.com/live_bak/test"));

	LONG lStyle;
	lStyle = GetWindowLong(m_listMeta.m_hWnd, GWL_STYLE);	//获取当前窗口style;
	lStyle &= ~LVS_TYPEMASK;								// 清除显示方式位;
	lStyle |= LVS_REPORT;									// 设置style;
	SetWindowLong(m_listMeta.m_hWnd, GWL_STYLE, lStyle);	// 设置style;

	DWORD dwStyle = m_listMeta.GetExtendedStyle();
	dwStyle |= LVS_EX_FULLROWSELECT;						// 选中某行使整行高亮（只适用与report风格的listctrl）;
	dwStyle |= LVS_EX_GRIDLINES;							// 网格线（只适用与report风格的listctrl）;
	dwStyle |= LVS_EX_CHECKBOXES;							// item前生成checkbox控件;
	m_listMeta.SetExtendedStyle(dwStyle);					// 设置扩展风格;

	m_listMeta.InsertColumn(0, _T("key"), LVCFMT_LEFT, 110);	//添加列名;
	m_listMeta.InsertColumn(1, _T("value"), LVCFMT_LEFT, 110);
	m_listMeta.InsertColumn(2, _T("type"), LVCFMT_LEFT, 95);

	namedWindow("view", WINDOW_AUTOSIZE);
	HWND hWnd = (HWND)cvGetWindowHandle("view");
	::ShowWindow(hWnd, SW_SHOW);

	GetDlgItem(IDC_BUTTON_OPENVIDEO)->EnableWindow(TRUE);
	GetDlgItem(IDC_BUTTON_CLOSEVIDEO)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_LIVE_START)->EnableWindow(TRUE);
	GetDlgItem(IDC_BUTTON_LIVE_STOP)->EnableWindow(FALSE);

	CString strTemp;
	strTemp.Format(_T("%d"), m_videoInfo.width);
	m_edtVideoWidth.SetWindowText(strTemp);
	strTemp.Format(_T("%d"), m_videoInfo.height);
	m_edtVideoHeight.SetWindowText(strTemp);
	strTemp.Format(_T("%d"), m_videoInfo.bps);
	m_edtVideoBps.SetWindowText(strTemp);
	strTemp.Format(_T("%d"), m_videoInfo.fps);
	m_edtVideoFps.SetWindowText(strTemp);
	strTemp.Format(_T("%d"), m_videoInfo.quality);
	m_ediVideoQuality.SetWindowText(strTemp);
	strTemp.Format(_T("%.2f"), m_videoInfo.interval);
	m_edtVideoInterval.SetWindowText(strTemp);

	int bCheck = m_audioInfo.enableAudio ? 1 : 0;
	m_btnAudioEnable.SetCheck(bCheck);
	strTemp.Format(_T("%d"), m_audioInfo.bps);
	m_edtAudioBps.SetWindowText(strTemp);
	strTemp.Format(_T("%d"), m_audioInfo.channels);
	m_edtAudioChannel.SetWindowText(strTemp);
	strTemp.Format(_T("%d"), m_audioInfo.bits);
	m_edtAudioBits.SetWindowText(strTemp);
	strTemp.Format(_T("%d"), m_audioInfo.samples);
	m_edtAudioSample.SetWindowText(strTemp);

	OnBnClickedAudioEnable();

	return TRUE;
}

void CPushTestDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CPushTestDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

HCURSOR CPushTestDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CPushTestDlg::OnBnClickedButtonLiveStart()
{
	{
		UpdateData(TRUE);

		CString strTemp;
		m_edtVideoWidth.GetWindowText(strTemp);
		m_videoInfo.width = _ttoi(strTemp);
		m_edtVideoHeight.GetWindowText(strTemp);
		m_videoInfo.height = _ttoi(strTemp);
		m_edtVideoBps.GetWindowText(strTemp);
		m_videoInfo.bps = _ttoi(strTemp);
		m_edtVideoFps.GetWindowText(strTemp);
		m_videoInfo.fps = _ttoi(strTemp);
		m_ediVideoQuality.GetWindowText(strTemp);
		m_videoInfo.quality = _ttoi(strTemp);
		m_edtVideoInterval.GetWindowText(strTemp);
		m_videoInfo.interval = _ttof(strTemp);

		m_audioInfo.enableAudio = m_btnAudioEnable.GetCheck() ? true : false;
		m_edtAudioBps.GetWindowText(strTemp);
		m_audioInfo.bps = _ttoi(strTemp);
		m_edtAudioChannel.GetWindowText(strTemp);
		m_audioInfo.channels = _ttoi(strTemp);
		m_edtAudioBits.GetWindowText(strTemp);
		m_audioInfo.bits = _ttoi(strTemp);
		m_edtAudioSample.GetWindowText(strTemp);
		m_audioInfo.samples = _ttoi(strTemp);
	}

	if (!m_bVideoOpened)
	{
		::MessageBox(m_hWnd, _T("推流前请打开摄像头！"), _T("提示"), MB_OK);
		return;
	}

	if (m_pRtmpPublisher)
	{
		CString strUrl;
		m_edtPushUrl.GetWindowText(strUrl);
		ServerInfo infoServer;
		infoServer.strPushUrl = CT2CA(strUrl);
		m_pRtmpPublisher->SetServerInfo(infoServer);

		m_pRtmpPublisher->SetVideoInfo(m_videoInfo);
		m_pRtmpPublisher->SetAudioInfo(m_audioInfo);
		m_pRtmpPublisher->Start();

		CUXTimer::GetInstance().SetTimer(LIVE_TIMER_ID, 1000 / VIDEO_FPS, CPushTestDlg::UxTimerCallback, this);
	}

	GetDlgItem(IDC_BUTTON_LIVE_START)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_LIVE_STOP)->EnableWindow(TRUE);
}

void CPushTestDlg::OnBnClickedButtonLiveStop()
{
	if (m_pRtmpPublisher)
	{
		m_pRtmpPublisher->Stop();

		CUXTimer::GetInstance().KillTimer(LIVE_TIMER_ID);
	}

	GetDlgItem(IDC_BUTTON_LIVE_START)->EnableWindow(TRUE);
	GetDlgItem(IDC_BUTTON_LIVE_STOP)->EnableWindow(FALSE);
}


void CPushTestDlg::OnBnClickedButtonSendMeta()
{
	int iCount = m_listMeta.GetItemCount();
	if (iCount <= 0)
	{
		return;
	}

	AMFObject obj;
	obj.o_num = iCount;
	obj.o_props = new AMFObjectProperty[obj.o_num];

	for (int iIndex = 0; iIndex < iCount; iIndex++)
	{
		TCHAR szBuf[1024];
		LVITEM lvi;
		lvi.iItem = iIndex;
		
		lvi.mask = LVIF_TEXT;
		lvi.pszText = szBuf;
		lvi.cchTextMax = 1024;

		for (int iCol = 2; iCol >= 0; iCol--)
		{
			lvi.iSubItem = iCol;
			if (m_listMeta.GetItem(&lvi))
			{
				//获取成功;
				switch (iCol)
				{
				case 0:
					{
						char* strA = new char[MAX_SIZE];
						strcpy(strA, CT2CA(lvi.pszText));
						obj.o_props[iIndex].p_name = AVC(strA);
					}
					break;
				case 1:
					{
						  if (AMF_STRING == obj.o_props[iIndex].p_type)
						  {
							  char* strA = new char[MAX_SIZE];
							  strcpy(strA, CT2CA(lvi.pszText));
							  obj.o_props[iIndex].p_vu.p_aval = AVC(strA);
						  }
						  else if (AMF_NUMBER == obj.o_props[iIndex].p_type)
						  {
							  obj.o_props[iIndex].p_vu.p_number = _wtof(lvi.pszText);
						  }
					}
					break;
				case 2:
					{
						  if (0 == wcscmp(lvi.pszText, _T("String")))
						  {
							  obj.o_props[iIndex].p_type = AMF_STRING;
						  } 
						  else if (0 == wcscmp(lvi.pszText, _T("Number")))
						  {
							  obj.o_props[iIndex].p_type = AMF_NUMBER;
						  }
					}
					break;
				default:
					break;
				}
			}
		}
	}

	m_pRtmpPublisher->AddMetaData("OnMetaEvent", obj);

	delete[]obj.o_props;
}


void CPushTestDlg::OnBnClickedButtonAdditem()
{
	CAddItemDialog dlg;
	if (IDOK == dlg.DoModal())
	{
		if (dlg.m_strKey.IsEmpty() || dlg.m_strType.IsEmpty())
		{
			return;
		}

		int iRow = m_listMeta.InsertItem(m_listMeta.GetItemCount(), dlg.m_strKey);
		m_listMeta.SetItemText(iRow, 1, dlg.m_strValue);
		m_listMeta.SetItemText(iRow, 2, dlg.m_strType);
		m_listMeta.SetItemState(iRow, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);
	}
}


void CPushTestDlg::OnBnClickedButtonDelitem()
{
	for (int i = m_listMeta.GetItemCount(); i >= 0; i--)
	{
		if (m_listMeta.GetCheck(i))
		{
			// 反向删除;
			m_listMeta.DeleteItem(i);
		}
	}
}

DWORD WINAPI CPushTestDlg::Th_OpenVideo_Cmd(LPVOID lpParam)
{
	CPushTestDlg* pThis = static_cast<CPushTestDlg*>(lpParam);
	assert(pThis);

	pThis->m_capVideo = pThis->m_iVideoIndex;	// 打开摄像头的索引;

	if (!pThis->m_capVideo.isOpened())
	{
		return 0;
	}
	pThis->m_bVideoOpened = true;

	while (pThis->m_bVideoOpened)
	{
		Mat tempFrame;
		pThis->m_capVideo >> tempFrame;

		if (!tempFrame.empty())
		{
			Mat dstFrame(0, 0, tempFrame.type());
			// rgb->bgr到frame
			if ((pThis->m_videoInfo.width != tempFrame.cols) ||
				(pThis->m_videoInfo.height != tempFrame.rows) )
			{
				if ((tempFrame.cols != 0) &&
					(tempFrame.rows != 0))
				{
					// 双线性插值;
					resize(tempFrame, dstFrame, Size(), pThis->m_videoInfo.width * 1.0 / tempFrame.cols, pThis->m_videoInfo.height * 1.0 / tempFrame.rows);
				}
			}
			else
			{
				dstFrame = tempFrame;
			}

			cvtColor(dstFrame, pThis->m_matFrame, COLOR_RGBA2BGR);
			imshow("view", dstFrame);
		}
		else
		{
			printf(" --(!) No captured frame -- Break!"); break;
		}

		waitKey(30);
	}

	pThis->m_capVideo.release();

	return 1;
}

void CPushTestDlg::OnBnClickedButtonOpenvideo()
{
	CString strTemp;
	m_edtVideoIndex.GetWindowText(strTemp);
	m_iVideoIndex = _ttoi(strTemp);

	{
		UpdateData(TRUE);

		CString strTemp;
		m_edtVideoWidth.GetWindowText(strTemp);
		m_videoInfo.width = _ttoi(strTemp);
		m_edtVideoHeight.GetWindowText(strTemp);
		m_videoInfo.height = _ttoi(strTemp);
		m_edtVideoBps.GetWindowText(strTemp);
		m_videoInfo.bps = _ttoi(strTemp);
		m_edtVideoFps.GetWindowText(strTemp);
		m_videoInfo.fps = _ttoi(strTemp);
		m_ediVideoQuality.GetWindowText(strTemp);
		m_videoInfo.quality = _ttoi(strTemp);

		m_audioInfo.enableAudio = m_btnAudioEnable.GetCheck() ? true : false;
		m_edtAudioBps.GetWindowText(strTemp);
		m_videoInfo.bps = _ttoi(strTemp);
		m_edtAudioChannel.GetWindowText(strTemp);
		m_audioInfo.channels = _ttoi(strTemp);
		m_edtAudioBits.GetWindowText(strTemp);
		m_audioInfo.bits = _ttoi(strTemp);
		m_edtAudioSample.GetWindowText(strTemp);
		m_audioInfo.samples = _ttoi(strTemp);
	}

	DWORD dwThreadID;
	CreateThread(NULL, 0, Th_OpenVideo_Cmd, this, 0, &dwThreadID);

	GetDlgItem(IDC_BUTTON_OPENVIDEO)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_CLOSEVIDEO)->EnableWindow(TRUE);
}

void CPushTestDlg::OnBnClickedButtonClosevideo()
{
	m_bVideoOpened = false;

	GetDlgItem(IDC_BUTTON_OPENVIDEO)->EnableWindow(TRUE);
	GetDlgItem(IDC_BUTTON_CLOSEVIDEO)->EnableWindow(FALSE);
}

void CALLBACK CPushTestDlg::UxTimerCallback(DWORD dwEventID, void* pBack)
{
	CPushTestDlg* pThis = static_cast<CPushTestDlg*>(pBack);
	if (pThis)
	{
		pThis->UxTimer_Callback(dwEventID);
	}
}

//IplImage* pImage = NULL;

//{
//	string imageName("C:\\Users\\lidabao\\Desktop\\Lena.bmp"); // 图片在电脑中的绝对地址
//	Mat src_img = imread(imageName.c_str(), IMREAD_COLOR);//读入图片数据
//	if (src_img.empty())
//		return -1;
//	namedWindow("image", CV_WINDOW_AUTOSIZE | CV_WINDOW_FREERATIO);
//	imshow("image", src_img);
//	Mat dst_img1;
//	Mat dst_img2(src_img.rows*0.5, src_img.cols*2.0, src_img.type());
//	// 双线性插值
//	resize(src_img, dst_img1, Size(), 0.5, 0.5);
//	// 立方插值
//	resize(src_img, dst_img2, dst_img2.size(), INTER_CUBIC);
//	namedWindow("resize image1", CV_WINDOW_AUTOSIZE | CV_WINDOW_FREERATIO);
//	namedWindow("resize image2", CV_WINDOW_AUTOSIZE | CV_WINDOW_FREERATIO);
//	imshow("resize image1", dst_img1);
//	imshow("resize image2", dst_img2);
//	waitKey(0);
//}

void CPushTestDlg::UxTimer_Callback(DWORD dwEventID)
{
	// 定时器获取视频帧图像 放入推流队列;
	IplImage cvImage = m_matFrame;

	CVideoBuff pTempBuff(cvImage.imageData, cvImage.imageSize);
	m_pRtmpPublisher->AddVideo(&pTempBuff);
}

void CPushTestDlg::OnClose()
{
	if (!m_pRtmpPublisher->IsStoped())
	{
		::MessageBox(m_hWnd, _T("请先结束推流再关闭程序！"), _T("提示"), MB_OK);
		return;
	}

	CDialogEx::OnClose();

	// 暂停推流;
	//OnBnClickedButtonLiveStop();

	// 关闭摄像头;
	//OnBnClickedButtonClosevideo();
}

LRESULT CPushTestDlg::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_APPEND_INFO:
		{
			CString strInfo = reinterpret_cast<wchar_t*>(wParam);
			
			// 获得文字的长度;
			int nTextLength = m_edtInfo.GetWindowTextLength();
			// 将光标放在文本最末;
			m_edtInfo.SetSel(nTextLength, nTextLength);
			// 写入文本;
			m_edtInfo.ReplaceSel(strInfo);
		}
		break;
	default:
		break;
	}

	return CDialogEx::WindowProc(message, wParam, lParam);
}


void CPushTestDlg::OnBnClickedButtonInfoClear()
{
	m_edtInfo.SetWindowText(_T(""));
}

void CPushTestDlg::OnBnClickedAudioEnable()
{
	BOOL bEnable = m_btnAudioEnable.GetCheck() ? TRUE : FALSE;

	// 音频;
	GetDlgItem(IDC_STATIC_BPS)->EnableWindow(bEnable);
	m_edtAudioBps.EnableWindow(bEnable);
	GetDlgItem(IDC_STATIC_CHANNEL)->EnableWindow(bEnable);
	m_edtAudioChannel.EnableWindow(bEnable);
	GetDlgItem(IDC_STATIC_BITS)->EnableWindow(bEnable);
	m_edtAudioBits.EnableWindow(bEnable);
	GetDlgItem(IDC_STATIC_SAMPLE)->EnableWindow(bEnable);
	m_edtAudioSample.EnableWindow(bEnable);
}

void CPushTestDlg::OnAboutMenu()
{
	CAboutDlg dlg;
	dlg.DoModal();
}