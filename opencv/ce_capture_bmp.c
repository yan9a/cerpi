// file: ce_capture_bmp.c
// description: image capturing example based on capture.c from https://linuxtv.org/docs.php
// date: 2021 Sep 09
// author: Yan Naing Aye

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <fcntl.h>              /* low-level i/o */
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <linux/videodev2.h>

#define CLEAR(x) memset(&(x), 0, sizeof(x))
#define IM_W 640
#define IM_H 480
/* Bmp ---------- */

typedef struct {
        long **Image;
        long Width;
        long Height;
} IMG;

typedef struct {
        unsigned short Signature;
        unsigned long BMFileSize;
        unsigned long Reserved;
        unsigned long BMOffset;
} BMFileHeader;
#define BMFILEHEADERSIZE 14
typedef struct {                    
        unsigned long BMHeaderSize;
        long Width;
        long Height;
        unsigned short ColorPlanes;
        unsigned short BitsPerPixel;
        unsigned long Compression;
        unsigned long BMSize;
        unsigned long HorizontalPixelsPerMeter;
        unsigned long VerticalPixelsPerMeter;
        unsigned long NoOfColorsInPalette;
        unsigned long ImportantColor;
} BMHeader;
typedef struct {
unsigned char Blue;
unsigned char Green;
unsigned char Red;
unsigned char Alpha;
} BGRA;

int SaveBMPFile(char  *file_name,IMG img);
IMG AllocateImage(long Width,long Height);
void FreeImage(IMG img);

int SaveBMPFile(char  *file_name,IMG img)
{
        FILE   *fp;
        BMFileHeader bmfh;
        BMHeader bmHdr;
	long PaddedBitMapWidth;
	int i,j;
	BGRA PaletteColor;
	unsigned char *rowdata;

	//prepare bitmap header
	bmHdr.BMHeaderSize=40;
	bmHdr.Width=img.Width;
	bmHdr.Height=img.Height;
	bmHdr.ColorPlanes=1;
	bmHdr.BitsPerPixel=8;
	bmHdr.Compression=0;
	PaddedBitMapWidth=((bmHdr.Width*(long)bmHdr.BitsPerPixel/8)+3) 
		& (~((long)3));
	bmHdr.BMSize=bmHdr.Height*PaddedBitMapWidth;
	bmHdr.HorizontalPixelsPerMeter=0x0b13;
	bmHdr.VerticalPixelsPerMeter=0x0b13;
	bmHdr.NoOfColorsInPalette=0x100;
	bmHdr.ImportantColor=0;

	//prepare file header
	bmfh.Signature=0x4d42;
	bmfh.Reserved=0;
	bmfh.BMOffset=BMFILEHEADERSIZE+bmHdr.BMHeaderSize
		+bmHdr.NoOfColorsInPalette*4;
	bmfh.BMFileSize=bmfh.BMOffset+bmHdr.BMSize;

	if((fp = fopen(file_name, "wb")) == NULL)
	{
                printf("\nERROR Could not create file %s",file_name);
                return 0;
        }
	
	//write file header
	fwrite(&bmfh.Signature, 2, 1, fp);
	fwrite(&bmfh.BMFileSize, 4, 1, fp);
	fwrite(&bmfh.Reserved, 4, 1, fp);
	fwrite(&bmfh.BMOffset,4, 1, fp);

	//write bitmap header
	fwrite(&bmHdr.BMHeaderSize,4, 1, fp);
	fwrite(&bmHdr.Width,4, 1, fp);
	fwrite(&bmHdr.Height,4, 1, fp);
	fwrite(&bmHdr.ColorPlanes,2, 1, fp);
	fwrite(&bmHdr.BitsPerPixel,2, 1, fp);
	fwrite(&bmHdr.Compression,4, 1, fp);
	fwrite(&bmHdr.BMSize,4, 1, fp);
	fwrite(&bmHdr.HorizontalPixelsPerMeter,4, 1, fp);
	fwrite(&bmHdr.VerticalPixelsPerMeter,4, 1, fp);
	fwrite(&bmHdr.NoOfColorsInPalette,4, 1, fp);
	fwrite(&bmHdr.ImportantColor,4, 1, fp);

	PaletteColor.Alpha=255;
	for(i=0;i<(int)bmHdr.NoOfColorsInPalette;i++)
	{		
		PaletteColor.Blue=i;
		PaletteColor.Green=i;
		PaletteColor.Red=i;		
		fwrite(&PaletteColor,sizeof(BGRA),1,fp);
	}
	rowdata=(unsigned char*)malloc(PaddedBitMapWidth * sizeof(unsigned char));
	for(j=0;j<PaddedBitMapWidth;j++) rowdata[j]=0;

        for(i=bmHdr.Height-1; i>=0; i--)
        {                       
                for(j=0;j<bmHdr.Width;j++) rowdata[j]=(unsigned char)img.Image[i][j];		  
                fwrite(rowdata, PaddedBitMapWidth, 1, fp);
        }   
	free(rowdata);
	fclose(fp);
	return 1;
}

IMG AllocateImage(long Width,long Height)
{
	IMG BMImage;
	int i,j;

	BMImage.Width=Width;  
	BMImage.Height=Height;
        BMImage.Image = (long int**)malloc(BMImage.Height * sizeof(long  *));
	for(i=0; i<BMImage.Height; i++)
	{
          BMImage.Image[i] = (long int*)malloc(BMImage.Width * sizeof(long ));                                            
        } 
				
        for(i=0; i<BMImage.Height; i++)
        {                                  
                for(j=0;j<BMImage.Width;j++)
                {
                        BMImage.Image[i][j]=0;
                }
        }   
	return BMImage;
}

void FreeImage(IMG img)
{
    int i;
    for(i=0;i<img.Height;i++) free(img.Image[i]);
	free(img.Image);
}

IMG im;
char filename[] = "pic.bmp";
/* Bmp ---------- */

struct buffer {
        void   *start;
        size_t  length;
};

static char            *dev_name;
static int              fd = -1;
struct buffer          *buffers;
static unsigned int     n_buffers = 4;
static int              frame_count = 1;
unsigned char *po;

static void errno_exit(const char *s)
{
        fprintf(stderr, "%s error %d, %s\n", s, errno, strerror(errno));
        exit(EXIT_FAILURE);
}

static int xioctl(int fh, int request, void *arg)
{
        int r;

        do {
                r = ioctl(fh, request, arg);
        } while (-1 == r && EINTR == errno);

        return r;
}

static void process_raw(unsigned char* po, unsigned char* pi, int size)
{
        int i;         
        size = size/2; /* pixel count is half of YUVU size */
        /* extract y */
        for(i=0;i<size;i++){
                po[i] = pi[i*2];
        }
}

static void process_image(const void *p, int size)
{                     
        int i,j;
        unsigned char *cp = (unsigned char*)p;
        unsigned char *po = cp; 
        process_raw(po,cp,size);
        for(i=0;i<im.Height;i++)
        for(j=0;j<im.Width;j++){
                im.Image[i][j]= po[i*im.Width+j];
        }
        SaveBMPFile(filename,im);
}

static int read_frame(void)
{
        struct v4l2_buffer buf;

        CLEAR(buf);

        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_MMAP;

        if (-1 == xioctl(fd, VIDIOC_DQBUF, &buf)) {
                switch (errno) {
                case EAGAIN:
                        return 0;

                case EIO:
                        /* Could ignore EIO, see spec. */

                        /* fall through */

                default:
                        errno_exit("VIDIOC_DQBUF");
                }
        }

        assert(buf.index < n_buffers);

        process_image(buffers[buf.index].start, buf.bytesused);

        if (-1 == xioctl(fd, VIDIOC_QBUF, &buf))
                errno_exit("VIDIOC_QBUF");

        return 1;
}

static void mainloop(void)
{
        unsigned int count;

        count = frame_count;

        while (count-- > 0) {
                for (;;) {
                        fd_set fds;
                        struct timeval tv;
                        int r;

                        FD_ZERO(&fds);
                        FD_SET(fd, &fds);

                        /* Timeout. */
                        tv.tv_sec = 2;
                        tv.tv_usec = 0;

                        r = select(fd + 1, &fds, NULL, NULL, &tv);

                        if (-1 == r) {
                                if (EINTR == errno)
                                        continue;
                                errno_exit("select");
                        }

                        if (0 == r) {
                                fprintf(stderr, "select timeout\n");
                                exit(EXIT_FAILURE);
                        }

                        if (read_frame())
                                break;
                        /* EAGAIN - continue select loop. */
                }
        }
}

static void stop_capturing(void)
{
        enum v4l2_buf_type type;
                type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
                if (-1 == xioctl(fd, VIDIOC_STREAMOFF, &type))
                        errno_exit("VIDIOC_STREAMOFF");
}

static void start_capturing(void)
{
        unsigned int i;
        enum v4l2_buf_type type;

        for (i = 0; i < n_buffers; ++i) {
                struct v4l2_buffer buf;

                CLEAR(buf);
                buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
                buf.memory = V4L2_MEMORY_MMAP;
                buf.index = i;

                if (-1 == xioctl(fd, VIDIOC_QBUF, &buf))
                        errno_exit("VIDIOC_QBUF");
        }
        type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        if (-1 == xioctl(fd, VIDIOC_STREAMON, &type))
                errno_exit("VIDIOC_STREAMON");
}

static void uninit_device(void)
{
        unsigned int i;

        for (i = 0; i < n_buffers; ++i)
                if (-1 == munmap(buffers[i].start, buffers[i].length))
                        errno_exit("munmap");

        free(buffers);
}

static void init_mmap(void)
{
        /* REQUEST */
        struct v4l2_requestbuffers req;

        CLEAR(req);

        /* Request buffer type and count */
        req.count = 4;
        req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        req.memory = V4L2_MEMORY_MMAP;

        if (-1 == xioctl(fd, VIDIOC_REQBUFS, &req)) {
                if (EINVAL == errno) {
                        fprintf(stderr, "%s does not support "
                                 "memory mappingn", dev_name);
                        exit(EXIT_FAILURE);
                } else {
                        errno_exit("VIDIOC_REQBUFS");
                }
        }

        /* check count */
        if (req.count < 2) {
                fprintf(stderr, "Insufficient buffer memory on %s\n",
                         dev_name);
                exit(EXIT_FAILURE);
        }

        /* ALLOCATE */
        /* allocate buffer pointers */
        buffers = calloc(req.count, sizeof(*buffers));

        if (!buffers) {
                fprintf(stderr, "Out of memory\n");
                exit(EXIT_FAILURE);
        }

        /* allocate buffers as requested */
        for (n_buffers = 0; n_buffers < req.count; ++n_buffers) {
                struct v4l2_buffer buf;

                CLEAR(buf);

                buf.type        = V4L2_BUF_TYPE_VIDEO_CAPTURE;
                buf.memory      = V4L2_MEMORY_MMAP;
                buf.index       = n_buffers;

                if (-1 == xioctl(fd, VIDIOC_QUERYBUF, &buf))
                        errno_exit("VIDIOC_QUERYBUF");

                buffers[n_buffers].length = buf.length;
                buffers[n_buffers].start =
                        mmap(NULL /* start anywhere */,
                              buf.length,
                              PROT_READ | PROT_WRITE /* required */,
                              MAP_SHARED /* recommended */,
                              fd, buf.m.offset);

                if (MAP_FAILED == buffers[n_buffers].start)
                        errno_exit("mmap");
        }
}

static void init_device(void)
{
        struct v4l2_capability cap;
        struct v4l2_cropcap cropcap;
        struct v4l2_crop crop;
        struct v4l2_format fmt;

        /* Check capabilities */
        if (-1 == xioctl(fd, VIDIOC_QUERYCAP, &cap)) {
                if (EINVAL == errno) {
                        fprintf(stderr, "%s is no V4L2 device\n",
                                 dev_name);
                        exit(EXIT_FAILURE);
                } else {
                        errno_exit("VIDIOC_QUERYCAP");
                }
        }

        if (!(cap.capabilities & V4L2_CAP_VIDEO_CAPTURE)) {
                fprintf(stderr, "%s is no video capture device\n",
                         dev_name);
                exit(EXIT_FAILURE);
        }

        if (!(cap.capabilities & V4L2_CAP_STREAMING)) {
                fprintf(stderr, "%s does not support streaming i/o\n",
                                dev_name);
                exit(EXIT_FAILURE);
        }


        /* Select video input, video standard and tune here. */
        CLEAR(cropcap);

        cropcap.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

        if (0 == xioctl(fd, VIDIOC_CROPCAP, &cropcap)) {
                crop.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
                crop.c = cropcap.defrect; /* reset to default */

                if (-1 == xioctl(fd, VIDIOC_S_CROP, &crop)) {
                        switch (errno) {
                        case EINVAL:
                                /* Cropping not supported. */
                                break;
                        default:
                                /* Errors ignored. */
                                break;
                        }
                }
        } else {
                /* Errors ignored. */
        }


        CLEAR(fmt);

        /* Set format */
        fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        fmt.fmt.pix.width       = IM_W;
        fmt.fmt.pix.height      = IM_H;
        fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;
        /* fmt.fmt.pix.field       = V4L2_FIELD_INTERLACED; */
        fmt.fmt.pix.field       = V4L2_FIELD_NONE;  
        if (-1 == xioctl(fd, VIDIOC_S_FMT, &fmt))
                errno_exit("VIDIOC_S_FMT");

        init_mmap();
}

static void close_device(void)
{
        if (-1 == close(fd)) errno_exit("close");
        fd = -1;
}

static void open_device(void)
{
        struct stat st;

        if (-1 == stat(dev_name, &st)) {
                fprintf(stderr, "Cannot identify '%s': %d, %s\n",
                         dev_name, errno, strerror(errno));
                exit(EXIT_FAILURE);
        }

        if (!S_ISCHR(st.st_mode)) {
                fprintf(stderr, "%s is no device\n", dev_name);
                exit(EXIT_FAILURE);
        }

        fd = open(dev_name, O_RDWR /* required */ | O_NONBLOCK, 0);

        if (-1 == fd) {
                fprintf(stderr, "Cannot open '%s': %d, %s\n",
                         dev_name, errno, strerror(errno));
                exit(EXIT_FAILURE);
        }
}

int main(int argc, char **argv)
{
        im = AllocateImage(IM_W,IM_H);
        /*po = (unsigned char*)malloc(IM_W * IM_H * sizeof(unsigned char));*/
        dev_name = "/dev/video0";
        open_device();
        init_device();
        start_capturing();
        mainloop();
        stop_capturing();
        uninit_device();
        close_device();
        /*free(po);*/
        FreeImage(im);
        fprintf(stderr, "\n");
        return 0;
}
