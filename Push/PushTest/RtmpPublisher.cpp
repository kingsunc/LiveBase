#include "stdafx.h"
#include "RtmpPublisher.h"
#include "UXTimerQueue.h"
#include <fstream>

#ifdef WIN32
#pragma comment(lib,"ws2_32.lib")
#endif

#define MAX_IMAGE_COUNT     (3)

int InitSocket()
{
	WORD version;
	WSADATA wsaData;
	version = MAKEWORD(2, 2);
	if (WSAStartup(version, &wsaData) != 0)
	{
		return 0;
	}
	return 1;
}

void CleanSocket()
{
	WSACleanup();
}

RtmpPublisher::RtmpPublisher() : m_rtmpInstance(NULL)
{
	m_pLastVideoBuff = NULL;
	m_hMainWnd = NULL;
	m_bStop = true;
}

RtmpPublisher::~RtmpPublisher()
{

}

void RtmpPublisher::SetVideoInfo(const VideoInfo &info)
{
	m_videoInfo = info;
}

void RtmpPublisher::SetAudioInfo(const AudioInfo &info)
{
	m_audioInfo = info;
}
void RtmpPublisher::SetServerInfo(const ServerInfo& info)
{
	m_serverInfo = info;
}

std::string RtmpPublisher::GetPushUrl()
{
	CStringA strUrl;
	strUrl.Format("rtmp://%s/%s/%s",
		m_serverInfo.strAddress.c_str(),
		m_serverInfo.strAppName.c_str(),
		m_serverInfo.strStreamID.c_str());
	return strUrl.GetString();
}

int RtmpPublisher::Start()
{
	if (m_bStop)
	{
		m_bStop = false;

		DWORD dwThreadID;
		CreateThread(NULL, 0, Th_Publish_Cmd, this, 0, &dwThreadID);	// 创建推送线程;
		CreateThread(NULL, 0, Th_Audio_Cmd, this, 0, &dwThreadID);		// 创建音频采集线程;
	}

	return 0;
}

int RtmpPublisher::Stop()
{

	// 此处不能释放, 去线程结束时释放;
	m_bStop = true;

	return 0;
}

bool RtmpPublisher::IsStoped()
{
	return m_bStop;
}

void RtmpPublisher::AddVideo(const CVideoBuff* pVideoBuff)
{
	if (!pVideoBuff)
	{
		return;
	}

	CVideoBuff* pTempBuff = new CVideoBuff(pVideoBuff->m_pBuff, pVideoBuff->m_iSize);
	assert(pTempBuff);

	m_lock.Lock();

	if (pTempBuff)
	{
		m_listVideoBuff.insert(m_listVideoBuff.end(), pTempBuff);

		if (m_listVideoBuff.size() > MAX_IMAGE_COUNT)
		{
			CVideoBuff* pFront = m_listVideoBuff.front();
			m_listVideoBuff.remove(pFront);
			delete pFront;
		}
	}

	m_lock.UnLock();
}

CVideoBuff* RtmpPublisher::PopVideo()
{
	m_lock.Lock();

	if (m_listVideoBuff.size() > 0)
	{
		CVideoBuff* pVideoBuff = m_listVideoBuff.front();
		if (m_pLastVideoBuff != pVideoBuff)
		{
			// 只有当上一帧画面过期时才进行释放;
			if (m_pLastVideoBuff)
			{
				delete m_pLastVideoBuff;
				m_pLastVideoBuff = NULL;
			}

			m_pLastVideoBuff = pVideoBuff;
			m_listVideoBuff.remove(pVideoBuff);
		}
	}

	m_lock.UnLock();

	return m_pLastVideoBuff;
}

void RtmpPublisher::AddAudio(const CByteArray& arrBuff)
{
	m_lock.Lock();

	m_listAudioBuff.Append(arrBuff);
	while (m_listAudioBuff.GetSize() >= 40960)
	{
		//  max 23x10 ms delay  40960=4096*10;
		m_listAudioBuff.RemoveAt(0, 20480);
	}

	m_lock.UnLock();
}

CAudioBuff* RtmpPublisher::PopAudio(int iLen)
{
	CAudioBuff* pAudioBuff = NULL;
	if ((iLen <= 0))
	{
		return pAudioBuff;
	}

	m_lock.Lock();
	if (m_listAudioBuff.GetSize() > iLen)
	{
		pAudioBuff = new CAudioBuff("", iLen);
		memcpy(pAudioBuff->m_pBuff, m_listAudioBuff.GetData(), iLen);
		m_listAudioBuff.RemoveAt(0, iLen);
	}
	m_lock.UnLock();

	return pAudioBuff;
}

void RtmpPublisher::OnCommand(int cmd, const char* str)
{
	if (!m_rtmpInstance)
	{
		return;
	}

	CStringA strA = str;
	if (strA.Find("connecting app") > -1)
	{
		AppendMeta();
	}
	if (cmd == RTMP_NC_STREAMSTART)
	{
	}
	else if (cmd == RTMP_NC_DISCONNECTED)
	{

	}
	else if (cmd == RTMP_NC_STAT)
	{
		if (m_rtmpInstance)
		{
			rtmp_stat_t stat;
			if (rtmp_get_stat(m_rtmpInstance, &stat) == 0)
			{
				char chTemp[1024];
				sprintf(chTemp, "rtmp_get_stat: frames_unencoded: %d frames_encoded: %d frames_sent: %d samples_unencoded: %d samples_encoded: %d samples_sent: %d meta_encoded: %d meta_sent: %d lost_nodes: %d last_lost_nodes: %d bytes_sent: %d bandwidth: %d publish_count: %d stream_time: %d first_start_time: %d last_start_time: %d \n ",
					 stat.frames_unencoded,
					 stat.frames_encoded,
					 stat.frames_sent,
					 stat.samples_unencoded,
					 stat.samples_encoded,
					 stat.samples_sent,
					 stat.meta_encoded,
					 stat.meta_sent,
					 stat.lost_nodes,
					 stat.last_lost_nodes,
					 stat.bytes_sent,
					 stat.bandwidth,
					 stat.publish_count,
					 stat.stream_time,
					 stat.first_start_time,
					 stat.last_start_time);

				// 信息通知界面;
				ShowWndInfo(chTemp);

				TRACE(chTemp);
			}
		}
	}
}

void RtmpPublisher::AddMetaData(std::string strKey, const AMFObject& metaData)
{
	CStringA strInfo;
	strInfo.Append((strKey + " : { ").c_str());

	amf_data_t  *t[2];
	t[0] = amf_new_string_c(strKey.c_str());
	t[1] = amf_new_object();

	int iCount = metaData.o_num;
	AMFObjectProperty* pObjList = metaData.o_props;

	for (int iIndex = 0; iIndex < iCount; iIndex++)
	{
		switch (pObjList[iIndex].p_type)
		{
		case AMF_STRING:
			{
				CStringA strTemp;
				strTemp.Format("%s : %s,  ", pObjList[iIndex].p_name.av_val, pObjList[iIndex].p_vu.p_aval.av_val);
				strInfo.Append(strTemp);
				amf_put_prop(t[1], pObjList[iIndex].p_name.av_val, amf_new_string(pObjList[iIndex].p_vu.p_aval.av_val, pObjList[iIndex].p_vu.p_aval.av_len));
			}
			break;
		case AMF_NUMBER:
			{
				CStringA strTemp;
				strTemp.Format("%s : %f,  ", pObjList[iIndex].p_name.av_val, pObjList[iIndex].p_vu.p_number);
				strInfo.Append(strTemp);
				amf_put_prop(t[1], pObjList[iIndex].p_name.av_val, amf_new_number(pObjList[iIndex].p_vu.p_number));
			}
			break;
		default:
			break;
		}
	}
	strInfo.Append(" } \n");

	if (m_rtmpInstance)
	{
		rtmp_append_meta_info(m_rtmpInstance, t, 2);
	}

	// 信息通知界面;
	ShowWndInfo(strInfo.GetString());

	amf_free_data(t[0]);
	amf_free_data(t[1]);
}

int RtmpPublisher::rtmp_helper_video_frames(rtmp_t t, char *buf, int len, void *u)
{
	if (u == NULL)
	{
		return -1;
	}

	RtmpPublisher *pThis = static_cast<RtmpPublisher*>(u);
	assert(pThis);
	assert(len);
	assert(t);

	CVideoBuff* pVideoBuff = pThis->PopVideo();
	if (!pVideoBuff)
	{
		TRACE("pVideoBuff is null.");
		return 0;
	}

	if (len != pVideoBuff->m_iSize)
	{
		std::string strInfo = "len != pVideoBuff->m_iSize.\n";
		TRACE(strInfo.c_str());
		memset(buf, 0, len);

		// 信息通知界面;
		pThis->ShowWndInfo(strInfo);
		return 0;
	}

	memcpy(buf, pVideoBuff->m_pBuff, pVideoBuff->m_iSize);

	return pVideoBuff->m_iSize;
}

int RtmpPublisher::rtmp_helper_audio_samples(rtmp_t t, char *buf, int len, void *u)
{
	if (u == NULL)
	{
		return -1;
	}

	RtmpPublisher *pThis = static_cast<RtmpPublisher*>(u);
	assert(pThis);
	try
	{
		std::string strInfo;
		// dont need audio;
		if (!pThis->m_audioInfo.enableAudio)
		{
			return 0;
		}
		assert(t);

		CAudioBuff* pAudioBuff = pThis->PopAudio(len);
		if (!pAudioBuff)
		{
			return 0;
			/*strInfo = "pThis->PopAudio() is null.\n";
			throw strInfo;*/
		}

		if (len != pAudioBuff->m_iSize)
		{
			std::string strInfo = "len != pAudioBuff->m_iSize.\n";
			throw strInfo;
		}

		memcpy(buf, pAudioBuff->m_pBuff, pAudioBuff->m_iSize);
		delete pAudioBuff;
		return len;
	}
	catch (std::string str)
	{
		TRACE(str.c_str());

		memset(buf, 0, len);
		// 信息通知界面;
		pThis->ShowWndInfo(str);

		return 0;
	}
	catch (...)
	{
	
	}

	std::string str = "unkonwn error. \n";
	TRACE(str.c_str());
	memset(buf, 0, len);
	// 信息通知界面;
	pThis->ShowWndInfo(str);

	return 0;
}

void RtmpPublisher::rtmp_helper_notify(int cmd, int err, rtmp_t r, char *msg, void *u)
{
	RtmpPublisher* pThis = static_cast<RtmpPublisher*>(u);
	CStringA str;
	str.Format("[%p]notify:[0x%04x] error[%d]:%s\n"
		, r
		, cmd
		, err
		, msg);

	pThis->CommandArrived(cmd, str);
}

void RtmpPublisher::CommandArrived(int cmd, const char *str)
{
	// 信息通知界面;
	ShowWndInfo(str);

	TRACE(str);

	OnCommand(cmd, str);
}

void RtmpPublisher::UxTimerCallback(DWORD dwEventID, void* p)
{
	RtmpPublisher* pThis = static_cast<RtmpPublisher*>(p);
	if (pThis)
	{
		pThis->Timer_Callback(dwEventID);
	}
}

void RtmpPublisher::Timer_Callback(DWORD dwEventID)
{
	// 给rtmpserver发心跳包;
	TRACE("Send heart to rtmpserver.");
}

DWORD WINAPI RtmpPublisher::Th_Publish_Cmd(LPVOID lpParam)
{
	RtmpPublisher* pThis = static_cast<RtmpPublisher*>(lpParam);
	assert(pThis);

	InitSocket();

	pThis->m_rtmpInstance = NULL;
	int rc;
	rtmp_video_t video;
	rtmp_audio_t audio;

	video.fmt_bytes = RTMP_SOURCE_TYPE_RGB24;
	video.bitrate = pThis->m_videoInfo.bps;
	video.source_cb = rtmp_helper_video_frames;
	video.fps = (double)pThis->m_videoInfo.fps;
	video.width = pThis->m_videoInfo.width;
	video.height = pThis->m_videoInfo.height;
	video.quality = pThis->m_videoInfo.quality;
	video.interval = pThis->m_videoInfo.interval;

	TRACE("rtmp stream info: fps=%llf, bitrate=%d, quality=%d", video.fps, video.bitrate, video.quality);

	audio.fmt_bytes = RTMP_SOURCE_TYPE_PCM;
	audio.bitrate = pThis->m_audioInfo.bps;
	audio.source_cb = rtmp_helper_audio_samples;
	audio.samples = pThis->m_audioInfo.samples;
	audio.channels = pThis->m_audioInfo.channels;
	audio.bits = pThis->m_audioInfo.bits;

	try
	{
		std::string strErr;
		pThis->m_rtmpInstance = rtmp_create(NULL, NULL, rtmp_helper_notify, (void *)pThis);
		if (pThis->m_rtmpInstance == NULL)
		{
			strErr = "rtmp_create failed: m_rtmpInstance is null.\n";
			throw strErr;
		}

		rc = rtmp_set_source(pThis->m_rtmpInstance, &video, &audio);
		if (rc != 0)
		{
			strErr = "rtmp_set_source failed: video or audio is not reasonable.\n";
			throw strErr;
		}

		std::string strPushUrl = pThis->m_serverInfo.strPushUrl;
		if (strPushUrl.length() <= 0)
		{
			strPushUrl = pThis->GetPushUrl();
		}
		rc = rtmp_publish_stream(pThis->m_rtmpInstance, strPushUrl.c_str());
		if (rc != 0)
		{
			strErr = "rtmp_publish_stream failed: strPushUrl is not reasonable.\n";
			throw strErr;
		}

		TRACE("Connect to server : %s\n", strPushUrl.c_str());

		while (!pThis->m_bStop)
		{
			Sleep(100);
		}
	}
	catch (std::string str)
	{
		pThis->ShowWndInfo(str);
		TRACE(str.c_str());
	}
	catch (...)
	{
	}
	
	if (pThis->m_rtmpInstance)
	{
		rtmp_destroy(pThis->m_rtmpInstance);
		pThis->m_rtmpInstance = NULL;
	}

	CleanSocket();

	return 0;
}

DWORD WINAPI RtmpPublisher::Th_Audio_Cmd(LPVOID lpParam)
{
	RtmpPublisher* pThis = static_cast<RtmpPublisher*>(lpParam);
	assert(pThis);

	memset(&pThis->waveform, 0, sizeof(WAVEFORMATEX));
	int iChannels = pThis->m_audioInfo.channels;
	int iBitsPerSample = pThis->m_audioInfo.bits;
	pThis->waveform.wFormatTag = WAVE_FORMAT_PCM;					// 声音格式为PCM;
	pThis->waveform.nSamplesPerSec = pThis->m_audioInfo.samples;	// 采样率;
	pThis->waveform.wBitsPerSample = iBitsPerSample;				// 采样比特，16bits/次;
	pThis->waveform.nChannels = iChannels;							// 采样声道数，2声道;  
	pThis->waveform.nAvgBytesPerSec = pThis->m_audioInfo.bps;		// 每秒的数据率，就是每秒能采集多少字节的数据;  
	pThis->waveform.nBlockAlign = (iChannels * iBitsPerSample) / 8;	// 一个块的大小，采样bit的bits数乘以声道数/8;
	pThis->waveform.cbSize = 0;										// 额外外信息大小, 一般为0;

	HANDLE wait = CreateEvent(NULL, 0, 0, NULL);
	// 使用waveInOpen函数开启音频采集;
	waveInOpen(&pThis->hWaveIn, WAVE_MAPPER, &pThis->waveform, (DWORD_PTR)wait, 0L, CALLBACK_EVENT);

	// 建立两个数组（这里可以建立多个数组）用来缓冲音频数据;
	DWORD bufsize = 100 * 1024;			// 每次开辟100k的缓存存储录音数据;
	while (!pThis->m_bStop)
	{
		char* pTemp = new char[bufsize];
		pThis->wHdr1.lpData = (LPSTR)pTemp;
		pThis->wHdr1.dwBufferLength = bufsize;
		pThis->wHdr1.dwBytesRecorded = 0;
		pThis->wHdr1.dwUser = 0;
		pThis->wHdr1.dwFlags = 0;
		pThis->wHdr1.dwLoops = 1;
		waveInPrepareHeader(pThis->hWaveIn, &pThis->wHdr1, sizeof(WAVEHDR));	// 准备一个波形数据块头用于录音;
		waveInAddBuffer(pThis->hWaveIn, &pThis->wHdr1, sizeof (WAVEHDR));		// 指定波形数据块为录音输入缓存;
		waveInStart(pThis->hWaveIn);	// 开始录音;
		Sleep(150);						// 等待声音录制150ms;
		waveInReset(pThis->hWaveIn);	// 停止录音;

		int iSize = pThis->wHdr1.dwBytesRecorded;
		CByteArray arrAudio;
		arrAudio.SetSize(iSize);
		memcpy(arrAudio.GetData(), pTemp, iSize);

		delete[]pTemp;
		pThis->AddAudio(arrAudio);
	}

	waveInClose(pThis->hWaveIn);
	return 0;
}

void RtmpPublisher::ShowWndInfo(std::string str)
{
	CString strInfo = CA2CT(str.c_str());
	if (m_hMainWnd)
	{
		::SendMessage(m_hMainWnd, WM_APPEND_INFO, (WPARAM)(strInfo.GetString()), NULL);
	}
}

void RtmpPublisher::AppendMeta()
{
	std::string strKey = "OnMetaData";
	AMFObject obj;
	obj.o_num = 4;
	obj.o_props = new AMFObjectProperty[obj.o_num];

	obj.o_props[0].p_name = AVC("width");
	obj.o_props[0].p_type = AMF_NUMBER;
	obj.o_props[0].p_vu.p_number = m_videoInfo.width;

	obj.o_props[1].p_name = AVC("height");
	obj.o_props[1].p_type = AMF_NUMBER;
	obj.o_props[1].p_vu.p_number = m_videoInfo.height;

	obj.o_props[2].p_name = AVC("fps");
	obj.o_props[2].p_type = AMF_NUMBER;
	obj.o_props[2].p_vu.p_number = m_videoInfo.fps;

	obj.o_props[3].p_name = AVC("bitrate");
	obj.o_props[3].p_type = AMF_NUMBER;
	obj.o_props[3].p_vu.p_number = m_videoInfo.bps;

	AddMetaData(strKey, obj);

	delete[]obj.o_props;
}
