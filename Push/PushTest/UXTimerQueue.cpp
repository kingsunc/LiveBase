#include "stdafx.h"
#include "UXTimerQueue.h"


//--------------------------------------------------------------------------------------------------------
CUXTimer::CUXTimer() :m_longIdEvent(0)
{
	m_hTimerQueue = CreateTimerQueue();
	assert(m_hTimerQueue != NULL);
	InitializeCriticalSection(&m_cs_timer_queue);
}
//--------------------------------------------------------------------------------------------------------
CUXTimer:: ~CUXTimer()
{
	EnterCriticalSection(&m_cs_timer_queue);
	for (TIMERQUEUE::iterator it = m_timer_queue.begin(); it != m_timer_queue.end(); ++it)
	{
		if (!it->second->hEvent)
		{
			DeleteTimerQueueTimer(m_hTimerQueue, it->second->hEvent, INVALID_HANDLE_VALUE);
		}
		if (!it->second)
		{
			delete it->second;
			it->second = NULL;
		}
	}
	LeaveCriticalSection(&m_cs_timer_queue);
	m_timer_queue.clear();
	DeleteTimerQueue(m_hTimerQueue);
	DeleteCriticalSection(&m_cs_timer_queue);
}
//--------------------------------------------------------------------------------------------------------
void CUXTimer::SetTimer(DWORD dwIdEvent, DWORD dwTime, UXTIMERCALLBACK callBack, void* p)
{
	TIMERINFO* t = new TIMERINFO;
	t->dwEventID = dwIdEvent;
	t->callback = callBack;
	t->pEvent = p;
	t->pThis = this;
	if (!CreateTimerQueueTimer(&t->hEvent, m_hTimerQueue, (WAITORTIMERCALLBACK)TimerFunc, (void*)t, dwTime, dwTime, WT_EXECUTEINIOTHREAD))
	{
		if (!t->hEvent)
		{
			DeleteTimerQueueTimer(m_hTimerQueue, t->hEvent, INVALID_HANDLE_VALUE);
			if (t)
			{
				delete t;
				t = NULL;
			}
			return;
		}
	}
	m_timer_queue.insert(std::make_pair(dwIdEvent, t));
}
//--------------------------------------------------------------------------------------------------------
void CUXTimer::KillTimer(DWORD dwIdEvent)
{
	EnterCriticalSection(&m_cs_timer_queue);
	TIMERQUEUE::iterator it = m_timer_queue.find(dwIdEvent);
	if (it != m_timer_queue.end())
	{
		it->second->pThis = NULL;
		if (it->second->hEvent)
		{
			DeleteTimerQueueTimer(m_hTimerQueue, it->second->hEvent, INVALID_HANDLE_VALUE);
		}
		if (!it->second)
		{
			delete it->second;
			it->second = NULL;
		}
		m_timer_queue.erase(it++);
	}
	LeaveCriticalSection(&m_cs_timer_queue);

}
//--------------------------------------------------------------------------------------------------------
void  CUXTimer::TimerFunc(void* p, bool b)
{
	EnterCriticalSection(&G_UXTimerQueue.m_cs_timer_queue);
	TIMERINFO* pt = (TIMERINFO*)p;
	if (pt && pt->pThis)
	{
		TIMERQUEUE::iterator it = ((CUXTimer*)pt->pThis)->m_timer_queue.find(pt->dwEventID);
		if (it != ((CUXTimer*)pt->pThis)->m_timer_queue.end())
		{
			it->second->callback(it->second->dwEventID, it->second->pEvent);
		}
	}
	LeaveCriticalSection(&G_UXTimerQueue.m_cs_timer_queue);
}
//--------------------------------------------------------------------------------------------------------