#include"rtp_send.h"
#define FPS 15
#define DESTIP "192.168.1.2"
#define DESTPORT 1234
#define PORTBASE 8888

static RTPSession sess;
static void checkerror(int err);
static  int  get_next_nalu(unsigned char*,int,int *,unsigned char **);
static  int find_start_code(unsigned char*buf);

int rtp_params_init(void)
{
    int status = 0;
    RTPUDPv4TransmissionParams transparams;
    RTPSessionParams sessparams;
    uint destport = DESTPORT;
    uint portbase = PORTBASE;
    uint destip = inet_addr(DESTIP);
    if (destip == INADDR_NONE)
    {
        std::cerr << "Bad IP address specified" << std::endl;
        return -1;
    }
    destip = ntohl(destip);
    sessparams.SetOwnTimestampUnit((double)(1.0f/90000.0f));
    transparams.SetPortbase(portbase);
    printf("set port base:%d\n",portbase);
    status = sess.Create(sessparams,&transparams);
    printf("Creat session\n");
    checkerror(status);
    RTPIPv4Address addr(destip,destport);
    status = sess.AddDestination(addr);
    printf("Add to Destination\n");
    checkerror(status);
    return 0;	
}

static void rtp_send(unsigned char *buf, int len)
{
    unsigned char * sendBuffer = (unsigned char *)calloc(1000*1000,sizeof(unsigned char));
    int status = 0;
    static int timeStampInc = 90000/FPS;
    int n = len / MAX_RTP_PKT_LENGTH;
    int last = len % MAX_RTP_PKT_LENGTH;
    if(last > 0) n++;
    int timeInc ;
    char nalHead = buf[0];
    if(len < MAX_RTP_PKT_LENGTH)
    {
        status = sess.SendPacket((void*)buf,len,H264,true,timeStampInc);
    }
    else
    {
       for(int i = 0; i < n;i++)
       {
            sendBuffer[0] = (nalHead & 0x60)|28;
            sendBuffer[1] = (nalHead & 0x1f);          
            if(0 == i)
            {
                timeInc = timeStampInc;
                sendBuffer[1] |= 0x80;
                memcpy(&sendBuffer[2],&buf[i*MAX_RTP_PKT_LENGTH+1],MAX_RTP_PKT_LENGTH);
                status = sess.SendPacket((void*)sendBuffer,MAX_RTP_PKT_LENGTH+2,H264,false,timeInc);
            }

            else if(i == n - 1) //send the last subpacket
            {
               timeInc = 0;
               sendBuffer[1] |= 0x40;
               memcpy(&sendBuffer[2],&buf[i*MAX_RTP_PKT_LENGTH+1],last);
               status = sess.SendPacket((void*)sendBuffer,last +2 ,H264,true,timeInc);
            }
            else
            {
                timeInc = 0;
                memcpy(&sendBuffer[2],&buf[i*MAX_RTP_PKT_LENGTH+1],MAX_RTP_PKT_LENGTH);
                status = sess.SendPacket((void*)sendBuffer,MAX_RTP_PKT_LENGTH+2,H264,false,timeInc);

            }
            checkerror(status);
            //sess.OnPollThreadStart();
            status = sess.Poll();
            //checkerror(status);
       }
    }
    free(sendBuffer);
    sendBuffer = NULL;
}

static void checkerror(int err)
{
  if (err < 0) 
  {
      const  char* errstr = RTPGetErrorString(err).c_str();
      printf("Error:%s//n", errstr);
      exit(-1);
  }
}

void nal_rtp_send(unsigned char* buf, int len)
{
    int pos = 0;
    int offset=4;
    unsigned char *nal_find_buffer=NULL;
    while(pos < len)
    {
        int nalLen = get_next_nalu(&buf[pos],len - pos,&offset,&nal_find_buffer);
	    if(nal_find_buffer==NULL)
	    {
            printf("nal start can not find nal_find_buffer=NULL\n");
            return;
	    }
        rtp_send(nal_find_buffer+offset,nalLen-offset);   //drop the start code 00 00 00 01
        pos += nalLen;
        nal_find_buffer=NULL;
    }
}

static int get_next_nalu(unsigned char* buf,int buf_len,int *offset,unsigned char **nal_find_buffer)
{
    int startcodeflag=0;
    int pos=0;
    int n=0;
    if(buf_len<=4)
    {
        return 0;
    }
    else
    {
        while((startcodeflag=find_start_code(&buf[pos]))==0)//find start code 00 00 00 01 or 00 00 01
        {
            pos++;
        }
	if(startcodeflag==1)
        {
            *offset=3;
        }
        else if(startcodeflag==2)
        {
            *offset=4;
        }
        n=pos;
        (*nal_find_buffer)=&buf[pos];
        pos+=(*offset);
        while(pos<buf_len&&(find_start_code(&buf[pos+1]))==0)//find secode start code 
        {
            pos++;
        }
        return (pos-n+1);
    }	
}
static int find_start_code(unsigned char*buf)
{
    if((buf[0]==0)&&(buf[1]==0)&&(buf[2]==1))
    {
        return 1;
    }
    else if((buf[0] == 0)&&(buf[1] == 0)&&(buf[2] == 0)&&(buf[3] == 1))
    {
        return 2;
    }
    else
    {
        return 0;
    }
}
