#include "main.h"
int main(int argc,char *argv[])
{
    int ret=0;
    Encoder en;
    unsigned char *imagedata = NULL;
    ret=v4l_capture_init();
    if(ret<0)
    {
        printf("v4l_capture_init() error!\n");
    }
    x264_encode_init(&en);
    rtp_params_init();
    while(1)
    {
        imagedata=v4l_capture();//get a frame
        if(imagedata==NULL)
        {
            printf("v4l_capture() error!\n");
			continue;
        }
        x264_xencode(&en,imagedata);
    }
    x264_encode_complete(&en);
    v4l_close();
    return 0;
}
