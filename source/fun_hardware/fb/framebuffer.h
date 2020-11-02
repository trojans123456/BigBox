#ifndef __FRAMEBUFFER_H
#define __FRAMEBUFFER_H

#include <stdint.h>
#ifdef __linux__
#include <linux/fb.h>
#endif

#include <string>

using std::string;

class FrameBuffer
{
public:
    FrameBuffer(string fb_name = "");
    ~FrameBuffer();

    uint32_t bitsPerPixel() const;
    uint32_t bytesPerPixel() const;

    bool open();
    bool is_open()  {return fb_isopen;}


    bool close();
    void clear(uint32_t color = 0x0);
    void flush();

    void drawPoint(uint32_t x,uint32_t y,uint32_t rgb);
    void drawLine(uint32_t x1,uint32_t y1,uint32_t x2,uint32_t y2,uint32_t rgb);

    uint32_t width() const;
    uint32_t height() const;
    void refresh();

private:
    /* 求x,y对应的像素点*/
    int32_t offset(uint32_t x,uint32_t y);
    struct Color
    {
        uint8_t r;
        uint8_t g;
        uint8_t b;
        uint8_t data[3];/* 24bit rgb888 rgb565 */
    };
    Color convert(uint32_t rgb);
    Color convert(uint8_t *pos);
private:
    string fb_dev;
    int fb_fd;
    uint8_t *fb_mem;
    uint32_t fb_memsize;
    bool fb_isopen;
    uint8_t *fb_tmp;
    struct fb_var_screeninfo fb_var;
    struct fb_fix_screeninfo fb_fix;
};

/**
如果在linux下测试。需进入无图形界面模式 ctrl + alt + f1~6
*/

#endif
