#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#ifdef __linux__
#include <sys/ioctl.h>
#include <sys/mman.h>
#endif

#include <iostream>

#include "framebuffer.h"
#include "fb_desc.h"

FrameBuffer::FrameBuffer(std::string fb_name)
    :fb_dev(fb_name)
    ,fb_fd(-1)
    ,fb_mem(NULL)
    ,fb_memsize(0)
    ,fb_isopen(false)
    ,fb_tmp(NULL)
{
    memset(&fb_var,0,sizeof(struct fb_var_screeninfo));
    memset(&fb_fix,0,sizeof(struct fb_fix_screeninfo));
}

FrameBuffer::~FrameBuffer()
{
    close();
}

uint32_t FrameBuffer::width() const
{
    return fb_var.xres;
}

uint32_t FrameBuffer::height() const
{
    return fb_var.yres;
}

uint32_t FrameBuffer::bitsPerPixel() const
{
    return fb_var.bits_per_pixel;
}

uint32_t FrameBuffer::bytesPerPixel() const
{
    return (fb_var.bits_per_pixel) / 8;
}

bool FrameBuffer::open()
{
    if(is_open())
        return false;
    if(fb_dev.empty())
        return false;

    fb_fd = ::open(fb_dev.c_str(),O_RDWR);
    if(fb_fd < 0)
        return false;

    if(ioctl(fb_fd,FBIOGET_VSCREENINFO,&fb_var) < 0)
    {
        goto error;
    }

    if(ioctl(fb_fd,FBIOGET_FSCREENINFO,&fb_fix) < 0)
    {
        goto error;
    }

    std::cout << fb_fix.smem_len << std::endl;
    std::cout << (fb_var.xres * fb_var.yres * fb_var.bits_per_pixel) / 8 <<std::endl;

    std::cout << fb_var.xres << fb_var.yres <<std::endl;
    std::cout << fb_var.xres_virtual << " " << fb_var.yres_virtual <<std::endl;

    /* 虚拟 smem_len = xres_virtual * yres_virtual * bits_per_pixel / 4 */
    //fb_memsize = fb_fix.smem_len;
    //fb_mem = (uint8_t *)mmap(0,fb_fix.smem_len,PROT_READ | PROT_WRITE,MAP_SHARED,fb_fd,0);
    fb_memsize = (fb_var.xres * fb_var.yres * fb_var.bits_per_pixel) / 8;
    fb_mem = (uint8_t *)mmap(0,fb_memsize,PROT_READ |PROT_WRITE,MAP_SHARED,fb_fd,0);
    if(!fb_mem)
        goto error;

    fb_tmp = new uint8_t[fb_memsize];
    if(!fb_tmp)
        goto error;

    return true;

error:
    if(fb_mem)
        munmap(fb_mem,fb_memsize);
    if(fb_fd > 0)
        ::close(fb_fd);

    return false;
}


bool FrameBuffer::close()
{
    if(fb_mem)
        munmap(fb_mem,fb_memsize);
    ::close(fb_fd);
    if(fb_tmp)
        delete []fb_tmp;

    return true;
}

void FrameBuffer::clear(uint32_t color)
{

}

void FrameBuffer::flush()
{
    if(fb_fd < 0)
        return ;
    //if(fb_mem && memcmp(fb_mem,fb_tmp,fb_fix.smem_len) != 0)
    {
        std::cout << "fjdslk" << std::endl;
        memcpy(fb_mem,fb_tmp,fb_memsize);
    }
}

void FrameBuffer::refresh()
{
    if(fb_fd > 0)
    {
        /* 立即强制设置值*/
        fb_var.activate |= FB_ACTIVATE_NOW | FB_ACTIVATE_FORCE;
        fb_var.yres_virtual = fb_var.yres * 2;
        fb_var.yoffset = fb_var.yres;
        ::ioctl(fb_fd,FBIOGET_VSCREENINFO,&fb_var);
    }
}

int32_t FrameBuffer::offset(uint32_t x, uint32_t y)
{
    if(x < fb_var.xres && y < fb_var.yres)
    {
        //return (x + fb_var.xoffset) * (fb_var.bits_per_pixel / 8) + (y + fb_var.yoffset) * fb_fix.line_length;
        return x * (fb_var.bits_per_pixel / 8) + y * fb_fix.line_length;
    }
    return -1;
}
/**
            RGB888 32bit
--------------------------------------------------------------------------------------------------------
| 0 | 0 | 0 | 0 | 0| 0 | 0 | 0 |R7|R6|R5|R4|R3|R2|R1|R0|G7|G6|G5|G4|G3|G2|G1|G0|B7|B6|B5|B4|B3|B2|B1|B0|
--------------------------------------------------------------------------------------------------------

            RGB565 16bit 全1表示一种颜色
---------------------------------------------------------------------------
| R7 | R6 | R5 | R4 | R3 | G7 | G6 | G5 | G4 | G3 | B7 | B6 | B5 | B4 | B3 |
---------------------------------------------------------------------------
*/
#define RGB888_RED      0x00ff0000
#define RGB888_GREEN    0x0000ff00
#define RGB888_BLUE     0x000000ff

#define RGB565_RED      0xf800
#define RGB565_GREEN    0x07e0
#define RGB565_BLUE     0x001f

FrameBuffer::Color FrameBuffer::convert(uint32_t rgb)
{
    Color c;
    c.r = (rgb >> 16) & 0xFF;
    c.g = (rgb >> 8) & 0xFF;
    c.b = (rgb >> 0) & 0xFF;

    switch(fb_var.bits_per_pixel)
    {
    case 16:
    {
        uint8_t R5 = (rgb & RGB888_RED) >> 19;
        uint8_t G6 = (rgb & RGB888_GREEN) >> 10;
        uint8_t B5 = (rgb & RGB888_BLUE) >> 3;
        uint16_t *v16 = (uint16_t *)(c.data);
        v16[0] = (R5 << 11) | (G6 <<5) | B5;
    }
        break;
    case 24:
    case 32:
        break;
        c.data[0] = c.r;
        c.data[1] = c.g;
        c.data[2] = c.b;
        break;
    default:
        break;
    }
    return c;
}

FrameBuffer::Color FrameBuffer::convert(uint8_t *pos)
{
    Color c;
    switch(fb_var.bits_per_pixel)
    {
    case 16:
        break;
    case 24:
    case 32:
        c.data[0] = pos[0];
        c.data[1] = pos[1];
        c.data[2] = pos[2];
        c.r = c.data[0];
        c.g = c.data[1];
        c.b = c.data[2];
        break;
    }
    return c;
}

void FrameBuffer::drawPoint(uint32_t x, uint32_t y, uint32_t rgb)
{
    if(fb_fd > 0)
    {
        int32_t pos = offset(x,y);
        if(pos == -1)
            return ;
        Color c = c = convert(rgb);
        memcpy(fb_tmp + pos,c.data,bytesPerPixel());
    }
}

void FrameBuffer::drawLine(uint32_t x1, uint32_t y1, uint32_t x2, uint32_t y2, uint32_t rgb)
{
    if(fb_fd > 0)
    {
        int32_t dx = x2 - x1;
        int32_t dy = y2 - y1;
        int32_t D = 2*dy - dx;

        Color c = convert(rgb);
        uint32_t Bpp = bytesPerPixel();
        //uint32_t r = rgb >> 16,g = (rgb >> 8) & 0xff,b = rgb;
        int32_t x,y;
        for(x = x1,y=y1;x < x2;++x)
        {
            uint32_t pos = offset(x,y);
            std::cout << "pos = " << pos <<std::endl;
            if(pos == -1)
            {
                std::cout << "offset = -1" << std::endl;
                continue;
            }
            memcpy(fb_tmp + pos,c.data,Bpp);

            if(D > 0)
            {
                ++y;
                D = D - 2*dx;
            }
            D = D + 2*dy;
        }
    }
}
#if 0
int main()
{
    FrameBuffer fb("/dev/fb0");
    if(fb.open() == false)
    {
        std::cout << "falied" << std::endl;
        return 0;
    }

    fb.drawLine(200,300,280,320,RGB888_GREEN);
    fb.flush();

    getchar();
}
#else
extern "C" {

#define FBDEVICE "/dev/fb0"
void draw_back(unsigned int *pfb, unsigned int width, unsigned int height, unsigned int color);

void draw_line(unsigned int *pfb, unsigned int width, unsigned int height);

int main(void)
{
    int fd = -1;
    int ret = -1;
    unsigned int *pfb = NULL;
    struct fb_fix_screeninfo finfo;
    struct fb_var_screeninfo vinfo;

    fd = open(FBDEVICE, O_RDWR);
    if (fd < 0)
    {
        perror("open");
        return -1;
    }
    printf("open %s success \n", FBDEVICE);

    /*获取fb信息*/
    ret = ioctl(fd, FBIOGET_FSCREENINFO, &finfo);
    if (ret < 0)
    {
        perror("ioctl");
        return -1;
    }

    ret = ioctl(fd, FBIOGET_VSCREENINFO, &vinfo);
    if (ret < 0)
    {
        perror("ioctl");
        return -1;
    }
    /*建立mmap映射*/
    pfb = (uint32_t*)::mmap(NULL, finfo.smem_len, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (NULL == pfb)
    {
        perror("mmap");
        return -1;
    }
    printf("pfb :0x%x \n", pfb);

    //draw_back(pfb, vinfo.xres_virtual, vinfo.yres_virtual, 0xffff0000);
    draw_line(pfb, vinfo.xres_virtual, vinfo.yres_virtual);

    getchar();
    close(fd);
    return 0;
}


void draw_back(unsigned int *pfb, unsigned int width, unsigned int height, unsigned int color)
{
    unsigned int x, y;
    for (y = 0; y < height; y++)
    {
        for (x = 0; x < width; x++)
        {
            *(pfb + y * width + x) = color;
        }
    }
}

void draw_line(unsigned int *pfb, unsigned int width, unsigned int height)
{
    unsigned int x, y;
    for (x = 50; x < width - 50; x++)
    {
        *(pfb + 50 * width + x) = 0xffffff00;
    }
    for (y = 50; y < height -50; y++)
    {
        *(pfb + y * width + 50) = 0xffffff00;
    }
}

}
#endif
