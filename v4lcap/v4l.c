#include "v4l.h"
v4l_device *vd;

static int v4l_set_picture(v4l_device *vd,int br,int hue,int col,int cont,int white,int depth,int palette)	//更改picture属性
{
    if(br)
    vd->picture.brightness=br;
    if(hue)
    vd->picture.hue=hue;
    if(col) 
    vd->picture.colour=col;
    if(cont)
    vd->picture.contrast=cont;
    if(white)
    vd->picture.whiteness=white;
    if(depth)
    vd->picture.depth=depth;
    if(palette)
    vd->picture.palette=palette;
    if(ioctl(vd->fd,VIDIOCSPICT,&vd->picture)<0)
    {
        perror("v4l_set_picture fail");
        return -1;
    }
    return 0;
}

static int v4l_mmap_init(v4l_device *vd)//内核映射方式初始化
{
    if(ioctl(vd->fd,VIDIOCGMBUF,&vd->mbuf)<0)
    {
        return -1;
    }
    if((vd->map=(unsigned char*)mmap(0,(vd->mbuf.size),PROT_READ|PROT_WRITE,MAP_SHARED,vd->fd,0))<0)
    {
        perror("v4l_mmap_init fail");
        return -1;
    }
    return 0;
}

static int v4l_set_window(v4l_device *vd,int x,int y,int width,int height)//设置截取窗口大小
{
    vd->win.x=x;
    vd->win.y=y;
    vd->win.width=width;
    vd->win.height=height;
    if(ioctl(vd->fd,VIDIOCSWIN,&vd->win)<0)
    {
        perror("v4l_set_window fail");
        return -1;
    }
    return 0;
}

static int v4l_open(char *dev,v4l_device *vd)
{
    if(!dev)
    dev=DEFAULT_DEVICE;
    if((vd->fd=open(dev,O_RDWR))<0)
    {
        perror("v4l_open fail");
        return -1;
    }
    if(ioctl(vd->fd,VIDIOCGCAP,&vd->capability)<0)	//获取设备信息的函数
    {
        return -1;
    }
    printf("video capture device name:%s\n",vd->capability.name);
    if(ioctl(vd->fd,VIDIOCGPICT,&vd->picture)<0)	//获取帧信息的函数
    {
        return -1;
    }
    printf("frames number is %d\n",vd->mbuf.frames);
    return 0;
}

int v4l_capture_init(void)
{
    int ret=0;
    vd=(v4l_device *)malloc(sizeof(v4l_device));
    memset(vd,0,sizeof(v4l_device));
    ret=v4l_open(NULL,vd);
    if(ret<0)
    {
        return ret;
    }
    ret=v4l_set_picture(vd,BRIGHTNESS,HUE,COLOUR,CONTRAST,WHITENESS,DEPTH,PALETTE);
    if(ret<0)
    {
        return ret;
    }
    ret=v4l_set_window(vd,0,0,WIDTH,HEIGHT);
    if(ret<0)
    {
        return ret;
    }
	vd->mmap.width=WIDTH;
    vd->mmap.height=HEIGHT;
    vd->mmap.format=vd->picture.palette;
    ret=v4l_mmap_init(vd);
    if(ret<0)
    {
        return ret;
    }
    return 0;
}

unsigned char * v4l_capture(void)
{
    int frame = 0;
    if(ioctl(vd->fd,VIDIOCMCAPTURE,&vd->mmap)<0)//获取视频帧
    {
        perror("v4l_grab_frame fail");
        return NULL; 
    }
    vd->mmap.frame=frame;
    if(ioctl(vd->fd,VIDIOCSYNC,&frame)<0)//等待一帧截取结束
    { 
        perror("v4l_grab_sync fail");
        return NULL;
    }
	return vd->map;
}

int v4l_close(void)
{
    munmap(vd->map,vd->mbuf.size);
    close(vd->fd);
    if(vd != NULL)
    {
        free(vd);
	    vd = NULL;
    }
    return 0;
}
