#ifndef _V4L_H_
#define _V4L_H_
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h> 		//stdio.h and stdlib.h are needed by perror function
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h> 		//O_RDWR
#include <unistd.h>
#include <sys/mman.h> 		//unistd.h and sys/mman.h are needed by mmap function
#include <stdbool.h>		//false and true
#include <sys/ioctl.h>
#include <linux/videodev.h>	//v4l API
#include "param.h"

typedef struct _v4l_struct
{
    int fd;			//保存打开视频文件的设备描述符
    struct video_window win;
    struct video_capability capability;
    struct video_picture picture;
    struct video_mmap mmap;
    struct video_mbuf mbuf;
    unsigned char *map;		//用于指向图像数据的指针
}v4l_device;

#define DEFAULT_DEVICE "/dev/video0"	//摄像头设备名字

#endif
