#ifndef __RTMP_CLIENT_H__
#define __RTMP_CLIENT_H__

#include <stdint.h>
#include <stdio.h>

#include "rtmp_amf.h"

#ifdef __cplusplus
extern "C" {
#endif

#define RTMP_ERR_OK           0x0000

#define RTMP_NC_SYS_ERROR     0xfff0
#define RTMP_NC_LIB_H264      0xff01
#define RTMP_NC_LIB_AAC       0xff02
#define RTMP_NC_LIB_RTMP      0xff03
#define RTMP_NC_CONNFAILED    0xff04
#define RTMP_NC_AVENCODED     0xff05
#define RTMP_NC_AVSEND        0xff06

#define RTMP_NC_MEM_ALLOC     0xfffe

#define RTMP_NC_STAT          0x0001

#define RTMP_NC_DEBUG         0x0100
#define RTMP_NC_CONNECTED     0x0001
#define RTMP_NC_DISCONNECTED  0x0002
#define RTMP_NC_STREAMSTART   0x0003
#define RTMP_NC_STREAMEND     0x0004
#define RTMP_NC_DESTROYED     0x0005
#define RTMP_NC_EXIT_NORMAL   0x0006
#define RTMP_NC_LOSTTOOMUCH   0x0007
#define RTMP_NC_STREAM_RETRY  0x0008
#define RTMP_NC_STREAM_TERM   0x0009

typedef void* rtmp_t;
typedef int (*rtmp_samples_cb)(rtmp_t t,char *buf,int len,void *u);

#define RTMP_SOURCE_TYPE_PCM       2
#define RTMP_SOURCE_TYPE_RGB24     3
#define RTMP_SOURCE_TYPE_RGB32     4

struct rtmp_stat_s {
    uint32_t       frames_unencoded;
    uint32_t       frames_encoded;
    uint32_t       frames_sent;
/*  uint32_t       frames_cached; */

    uint32_t       samples_unencoded;
    uint32_t       samples_encoded;
    uint32_t       samples_sent;
/*  uint32_t       samples_cached; */

    uint32_t       meta_encoded;
    uint32_t       meta_sent;

    uint32_t       lost_nodes;
    uint32_t       last_lost_nodes;

    uint32_t       bytes_sent;          /* bytes */
    uint32_t       bandwidth;           /* kbps */

    uint32_t       publish_count;       /*retry counts*/

    uint32_t       stream_time;         /*msec*/
    uint32_t       first_start_time;
    uint32_t       last_start_time;
};

struct rtmp_video_info{
    int             fmt_bytes;  /*RGB24/RGB32 only*/
    int             bitrate;
    int             quality;
    uint16_t        width;
    uint16_t        height;
    double          fps;
    rtmp_samples_cb source_cb;
    double          interval;   /*interval*/
};

struct rtmp_audio_info{
    int             fmt_bytes;  /*PCM only*/
    int             bitrate;
    uint16_t        channels;
    uint16_t        bits;
    uint32_t        samples;
    rtmp_samples_cb source_cb;
    double          interval;   /*interval*/
};

typedef struct rtmp_audio_info rtmp_audio_t;
typedef struct rtmp_video_info rtmp_video_t;
typedef struct rtmp_stat_s rtmp_stat_t;

typedef void* (*mem_alloc_rt)(unsigned size,void *u);
typedef void  (*mem_free_rt)(void *p,void *u);
typedef void  (*rtmp_error_rt)(int nc,int err,rtmp_t r,char *msg,void *u);

/*
   if a or f is null,this malloc() and free() willed used respectively
 */
rtmp_t rtmp_create(mem_alloc_rt a,mem_free_rt f,rtmp_error_rt e,void *u);
void rtmp_destroy(rtmp_t rtmp);

int rtmp_set_source(rtmp_t rtmp,rtmp_video_t *,rtmp_audio_t *);

int rtmp_set_url(rtmp_t rtmp, const char *url);
char* rtmp_get_url(rtmp_t rtmp, char *url, int n);

int rtmp_get_stat(rtmp_t rtmp, rtmp_stat_t *stat);

int rtmp_append_meta_info(rtmp_t r,amf_data_t* t[],int num);

int rtmp_publish_stream(rtmp_t rtmp,const char *url);
int rtmp_play_stream(rtmp_t rtmp,const char *url);

void* rtmp_get_userdata(rtmp_t t);
void* rtmp_set_userdata(rtmp_t t,void *u);

#ifdef __cplusplus
}
#endif

#endif

