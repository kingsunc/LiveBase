#pragma once
#include <windows.h>
#include <string>
#include <list>
#include "encoder/rtmpclient.h"
#include "LiveLockEx.h"
#include <mmeapi.h>
#include <vector>
#include "amf.h"

#pragma comment(lib, "winmm.lib")

#define VIDEO_WIDTH			1280
#define VIDEO_HEIGHT		720
#define VIDEO_FPS			12
#define VIDEO_BPS			400000
#define VIDEO_QUALITY		32
#define VIDEO_INTERVAL		2.00

#define AUDIO_BITRATE		96000
#define AUDIO_SAMPLES		44100
#define AUDIO_CHANNELS		2
#define AUDIO_BITS			16

#define WM_APPEND_INFO		(WM_USER + 200)

class VideoInfo
{
public:
	VideoInfo()
	{
		width = VIDEO_WIDTH;
		height = VIDEO_HEIGHT;
		bps = VIDEO_BPS;
		fps = VIDEO_FPS;
		quality = VIDEO_QUALITY;
		interval = VIDEO_INTERVAL;
	}

	int				width;				// 分辨率-宽;
	int				height;				// 分辨率-高;
	int				bps;				// 码率; bits per second
	int				fps;				// 帧率;
	int				quality;			// 质量;
	double          interval;			// i帧间隔;
};

class AudioInfo
{
public:
	AudioInfo()
	{
		enableAudio = true;
		bps = AUDIO_BITRATE;
		channels = AUDIO_CHANNELS;
		bits = AUDIO_BITS;
		samples = AUDIO_SAMPLES;
	}

	bool			enableAudio;		// 是否采集音频;
	int             bps;				// 码率;
	uint16_t        channels;			// 通道数;
	uint16_t        bits;				// 采样位数;
	uint32_t        samples;			// 采样频率;
};

struct ServerInfo
{
	std::string strAddress;
	std::string strAppName;
	std::string strStreamID;

	std::string strPushUrl;	// 优先使用这个,若这个为空再拼接;
};

// 音视频数据;
class CBuffer
{
public:
	CBuffer(char* pBuff, int iSize)
	{
		m_pBuff = NULL;
		m_iSize = 0;

		if (iSize > 0)
		{
			m_iSize = iSize;
			m_pBuff = new char[m_iSize + 1];
			memcpy(m_pBuff, pBuff, iSize*sizeof(char));
		}
	}

	~CBuffer()
	{
		if (m_pBuff)
		{
			delete[]m_pBuff;
			m_pBuff = NULL;
		}
	}

	char*	m_pBuff;
	int		m_iSize;
};

typedef CBuffer CVideoBuff;
typedef CBuffer CAudioBuff;
typedef std::list<CVideoBuff*>	VideoBuffList;
typedef std::list<CAudioBuff*>	AudioBuffList;

class RtmpPublisher
{
public:
	RtmpPublisher();
	virtual ~RtmpPublisher();

	void SetVideoInfo(const VideoInfo& info);
	void SetAudioInfo(const AudioInfo& info);
	void SetServerInfo(const ServerInfo& info);
	int Start();
	int Stop();

	bool IsStoped();

	// 此函数内部为深拷贝;
	void AddVideo(const CVideoBuff* pVideoBuff);
	CVideoBuff* PopVideo();

	// 此函数内部为深拷贝;
	void AddAudio(const CByteArray& pAudioBuff);
	CAudioBuff* PopAudio(int iLen);

	void AddMetaData(std::string strKey, const AMFObject& metaData);

public:
	void OnCommand(int cmd, const char* str);

	static void CALLBACK UxTimerCallback(DWORD dwEventID, void* pThis);
	void Timer_Callback(DWORD dwEventID);

private:
	static int rtmp_helper_video_frames(rtmp_t t, char *buf, int len, void *u);
	static int rtmp_helper_audio_samples(rtmp_t t, char *buf, int len, void *u);
	static void rtmp_helper_notify(int nc, int err, rtmp_t r, char *msg, void *u);
	void CommandArrived(int cmd, const char *str);

	std::string GetPushUrl();

	static DWORD WINAPI Th_Publish_Cmd(LPVOID lpParam);

	static DWORD WINAPI Th_Audio_Cmd(LPVOID lpParam);

	void ShowWndInfo(std::string strInfo);

	void AppendMeta();

private:
	VideoBuffList	m_listVideoBuff;
	CByteArray		m_listAudioBuff;
	CLiveLockEx		m_lock;

	VideoInfo       m_videoInfo;
	AudioInfo       m_audioInfo;
	ServerInfo		m_serverInfo;

	rtmp_t          m_rtmpInstance;

	CVideoBuff*		m_pLastVideoBuff;

	bool			m_bStop;

public:
	HWND			m_hMainWnd;

	HWAVEIN			hWaveIn;			// 输入设备;  
	WAVEFORMATEX	waveform;			// 采集音频的格式，结构体;
	WAVEHDR			wHdr1;				// 采集音频时包含数据缓存的结构体;
};