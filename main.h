#ifndef __MAIN_H
#define __MAIN_H
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "x264encode/x264encode.h"
//rtp model interface
extern int rtp_params_init(void);
extern void nal_rtp_send(unsigned char* buf, int len);
//v4l capture model interface
extern int v4l_capture_init(void);
extern unsigned char * v4l_capture(void);
extern int v4l_close(void);
//x264 encode model interface
extern int x264_encode_init(Encoder *en);
extern void x264_xencode(Encoder *en,unsigned char *yuv_buffer);
extern void x264_encode_complete(Encoder *en);
//extern void close_encoder(void);
#endif
