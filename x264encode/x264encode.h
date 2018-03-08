#ifndef __X264_ENCODE_H
#define __X264_ENCODE_H
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include "x264.h"
#define WIDTH 176	
#define HEIGHT 144

typedef struct
{
    x264_param_t *param;
    x264_nal_t *nals;
    x264_t *encoder;
    x264_picture_t *pic_in;
    x264_picture_t *pic_out;
}Encoder; 

extern void nal_rtp_send(unsigned char* buf, int len);
#endif
