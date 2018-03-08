#include "x264encode.h"
#define FPS 15
int x264_encode_init(Encoder *en)
{
    en->param = (x264_param_t *)calloc(1,sizeof(x264_param_t));
    en->pic_in = (x264_picture_t *)calloc(1,sizeof(x264_picture_t));
    en->pic_out = (x264_picture_t *)calloc(1,sizeof(x264_picture_t));
    x264_param_default_preset(en->param, "ultrafast", "zerolatency");
    en->param->i_threads = 1;
    en->param->i_width = WIDTH;
    en->param->i_height = HEIGHT;
    en->param->i_fps_num = FPS;
    en->param->i_fps_den = 1;
	
    en->param->i_keyint_max = FPS;
    en->param->b_intra_refresh = 1;
	
	en->param->rc.i_rc_method = X264_RC_CRF;
	en->param->rc.f_rf_constant = 25;
	en->param->rc.f_rf_constant_max = 35;
	
	en->param->b_repeat_headers = 1;
    en->param->b_annexb = 1;
    x264_param_apply_profile(en->param, "main");
    en->encoder = x264_encoder_open(en->param);
    x264_picture_alloc(en->pic_in, X264_CSP_I420, en->param->i_width, en->param->i_height);
    return 0;
}

void x264_xencode(Encoder *en,unsigned char *yuv_buffer)
{
    int i;
    int nnal = 0;
    static int64_t i_pts = 0;
    en->pic_in->img.plane[0] = yuv_buffer;
    en->pic_in->img.plane[1] = en->pic_in->img.plane[0] + (en->param->i_width)*(en->param->i_height);
    en->pic_in->img.plane[2] = en->pic_in->img.plane[1] + (en->param->i_width)*(en->param->i_height)/4;
 
    en->pic_in->i_pts = i_pts++;
    i = x264_encoder_encode(en->encoder, &(en->nals), &nnal, en->pic_in, en->pic_out);
    for (i = 0; i < nnal; i++) 
    {
        nal_rtp_send(en->nals[i].p_payload,en->nals[i].i_payload);
    }
    return ;
}

void x264_encode_complete(Encoder *en)
{
    if (en->pic_out)
    {
        x264_picture_clean(en->pic_out);
        free(en->pic_out);
        en->pic_out = 0;
    }
    if (en->pic_in) 
    {
        x264_picture_clean(en->pic_in);
        free(en->pic_in);
        en->pic_in = 0;
    }
    if (en->nals)
    {
        free(en->nals);
        en->nals = 0;
    }
    if (en->param) 
    {
        free(en->param);
        en->param = 0;
    }
    if (en->encoder) 
    {
        x264_encoder_close(en->encoder);
    }
    free(en);
}
