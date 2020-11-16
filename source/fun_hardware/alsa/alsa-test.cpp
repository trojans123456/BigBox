/**
 *
 * 1.打开设备
 * 2.分配内存,设置参数
 * 3.填充默认参数
 * 4.设置参数 PCM
 *  4.1 通道数
 *  4.2 采样率,指定时间和文件大小
 *  4.3 帧数,每次读取的数据长度
 *  4.4 数据格式
 *  4.5 设备访问类型
 * 5.读取写入数据
 *
*/

#include <stdio.h>
#include <iostream>
#include <alsa-lib/asoundlib.h>

#include <fstream>

using namespace std;

int main(int argc,char *argv[])
{
    // 设备名称，这里采用默认，还可以选取"hw:0,0","plughw:0,0"等
    const char *device = "default";
    // 设备句柄
    // 以下均定义两个，根据前缀区分，c->capture,p->playback,没有前缀的表示参数相同
    snd_pcm_t *chandle;
    snd_pcm_t *phandle;
    // 硬件参数
    snd_pcm_hw_params_t *cparams;
    snd_pcm_hw_params_t *pparams;
    // 数据访问类型，读写方式：内存映射或者读写，数据
    snd_pcm_access_t access_type = SND_PCM_ACCESS_RW_INTERLEAVED;
    // 格式，
    snd_pcm_format_t format = SND_PCM_FORMAT_S16_LE;
    // 码率，采样率,8000Hz,44100Hz
    unsigned int rate = 44100;
    // 通道数
    unsigned int channels = 2;
    // 帧数，这里取32
    snd_pcm_uframes_t frames = 32;
    // 以下为可选参数
    unsigned int bytes_per_frame;
    // 软件重采样
    unsigned int soft_resample;

    /** 打开设备*/
    int err;
    if ((err = snd_pcm_open(&chandle, device, SND_PCM_STREAM_CAPTURE, 0)) < 0)
    {
        printf("Capture device open failed.\n");
    }
    if ((err = snd_pcm_open(&phandle, device, SND_PCM_STREAM_PLAYBACK, 0)) < 0)
    {
        printf("Playback device open failed.\n");
    }

    /** 设置参数 */
    // 先计算每帧数据的大小
    bytes_per_frame = snd_pcm_format_width(format) / 8 * 2;
    // 计算需要分配的缓存空间的大小
    buffer_size = frames * bytes_per_frame;

    // 为参数分配空间
    snd_pcm_hw_params_alloca(&params);
    // 填充参数空间
    snd_pcm_hw_params_any(handle, params);
    // 设置数据访问方式
    snd_pcm_hw_params_set_access(handle, params, access_type);
    // 设置格式
    snd_pcm_hw_params_set_format(handle, params, format);
    // 设置通道
    snd_pcm_hw_params_set_channels(handle, params, channels);
    // 设置采样率
    snd_pcm_hw_params_set_rate_near(handle, params, &rate, 0);

    // 可选项，不改不影响
    // 设置缓存大小
    buffer_size = period_size * 2;
    snd_pcm_hw_params_set_buffer_size_near(handle, params, &buffer_size);
    // 设置段大小，period与OSS中的segment类似
    period_size = buffer_size / 2;
    snd_pcm_hw_params_set_period_size_near(handle, params, &period_size, 0);

    //设置参数
    snd_pcm_hw_params(handle, params);

    /**** 读写数据 */
    // 分配缓存空间，大小上面通过buffer_size计算出了
    char *buffer = (char *)malloc(buffer_size);
    // 读写数据
    snd_pcm_readi(chandle, buffer, frames);
    snd_pcm_writei(phandle, buffer, frames);

    /** 循环播放 */
    while(1)
    {
        snd_pcm_readi(chandle, buffer, frames);
        snd_pcm_writei(phandle, buffer, frames);
    }

    //捕获一定时间的音频数据到文件流
    ofstream output("test.pcm", ios::trunc);

    int loop_sec;
    int frames_readed;
    loop_sec = 10;
    unsigned long loop_limit;
    // 计算循环大小
    loop_limit = loop_sec * rate;

    for (size_t i = 0; i < loop_limit; )
    {
        // 这里还需要判断一下返回值是否为负
        frames_readed = snd_pcm_readi(chandle, buffer, frames);
        output.write(buffer, buffer_size);
        i += frames_readed;
    }
    /** 关闭设备*/
    snd_pcm_close(chandle);
    snd_pcm_close(phandle);
    free(buffer);
}
