#ifndef RTPSEND_H
#define RTPSEND_H
#include "rtpsession.h"
#include "rtpudpv4transmitter.h"
#include "rtpipv4address.h"
#include "rtpsessionparams.h"
#include "rtperrors.h"
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <string>
#define H264                    96
#define MAX_RTP_PKT_LENGTH     1350
extern "C" int rtp_params_init(void);
extern "C" void nal_rtp_send(unsigned char* buf, int len);
#endif // RTPSEND_H
