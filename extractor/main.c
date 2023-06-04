#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include "libavformat/avformat.h"
#include "libavformat/avio.h"
#include "libavformat/version.h"
#include "libavutil/mem.h"
#include "libavutil/log.h"
#include "libavutil/time.h"

#define AVIO_BUF_SIZE   32 * 1024

#define logd(fmt, arg...) av_log(NULL, AV_LOG_INFO, "[%s %d] " fmt , __FUNCTION__, __LINE__, ##arg);
#define logw(fmt, arg...) av_log(NULL, AV_LOG_WARNING, "[%s %d] " fmt , __FUNCTION__, __LINE__, ##arg);
#define loge(fmt, arg...) av_log(NULL, AV_LOG_ERROR, "[%s %d] " fmt , __FUNCTION__, __LINE__, ##arg);

typedef struct {
    int fd;
    int64_t offset;
    int64_t total_size;
    uint32_t swap_size;
} AVIOCallBack;

static int open_file(const char *url)
{
    int fd = open(url, O_RDONLY);
    if (fd < 0) {
        loge("open %s fail\n", url);
    }
    return fd;
}

static void close_file(int fd)
{
    if (fd >= 0) {
        close(fd);
    } else {
        loge("invalid file handle: %d\n", fd);
    }
}

static int read_file(void *opaque, uint8_t *buf, int buf_size)
{
    AVIOCallBack *avio = (AVIOCallBack *)opaque;
    int read_size = -1;

    if (avio->fd < 0 || avio->total_size <= 0) {
        loge("invalid fd:%d or total_size: %ld\n", avio->fd, avio->total_size);
        return AVERROR_EXTERNAL;
    }
    read_size = FFMIN(buf_size, avio->total_size - avio->offset);
    read_size = read(avio->fd, buf, read_size);
    logd("offset:%ld buf_size:%d read_size:%d\n", avio->offset, buf_size, read_size);

    return read_size;
}

static int64_t seek_file(void *opaque, int64_t offset, int whence)
{
    AVIOCallBack *avio = (AVIOCallBack *)opaque;
    int64_t postion;

    if (avio->fd < 0 || avio->total_size <= 0) {
        loge("invalid fd:%d or total_size: %ld\n", avio->fd, avio->total_size);
        return AVERROR_EXTERNAL;
    }

    switch (whence) {
        case AVSEEK_SIZE: {
            return avio->total_size;
        } break;
        case SEEK_SET: {
            postion = lseek(avio->fd, offset, SEEK_SET);
        } break;
        case SEEK_CUR: {
            postion = lseek(avio->fd, offset, SEEK_CUR);
        } break;
        case SEEK_END: {
            postion = lseek(avio->fd, offset, SEEK_END);
        } break;
        default : return AVERROR_EXTERNAL;
    }

    if (postion < 0) {
        loge("seek postion:%ld before the begin of file!\n", postion);
        return AVERROR_EXTERNAL;
    }

    if (postion > avio->total_size) {
        loge("seek postion:%ld too far and out of range %ld!\n", postion, avio->total_size);
        return AVERROR_EOF;
    }

    avio->offset = postion;

    return postion;
}

static int64_t get_file_size(const char *url)
{
    struct stat file;
    int64_t size = -1;
    if (stat(url, &file) == 0) {
        size = file.st_size;
        logd("file: %s size is: %ld\n", url, size);
        return size;
    }
    else {
        loge("file stat error: %s\n", strerror(errno));
    }
    return -1;
}

static bool read_running = false;

static void signal_event(int sig)
{
    if (sig == SIGINT) {
        read_running = false;
        logd("signal event: %d\n", sig);
    }
}

int main(int argc, char *argv[])
{
    AVFormatContext *fmt_ctx = NULL;
    AVIOContext *avio_ctx = NULL;
    AVPacket packet, *pkt = &packet;
    AVIOCallBack io_callback;
    uint8_t *avio_buf = NULL;
    const char *filename = NULL, *url = NULL;
    int ret, enable_callback = 0;
    char ch;

    while((ch = getopt(argc, argv, "i:c:"))!= -1)
    {
        switch(ch)
        {
            case 'i': {
                filename = optarg;
                logd("get filename: %s\n", filename);
            } break;
            case 'c': {
                enable_callback = atoi(optarg);
                logd("enable_callback: %d\n", enable_callback);
            } break;
            default: 
                logd("unknown input parameter: %d\n", ch);
            break;
        }
    }

    if (SIG_ERR == signal(SIGINT, signal_event)) {
        loge("register signal error\n");
        return -1;
    }

    fmt_ctx = avformat_alloc_context();
    if (!fmt_ctx) {
        loge("avformat_alloc_context error\n");
        return 0;
    }

    if (enable_callback) {
        io_callback.fd = open_file(filename);
        io_callback.offset = 0;
        io_callback.swap_size = AVIO_BUF_SIZE;
        io_callback.total_size = get_file_size(filename);

        avio_buf = (uint8_t *)av_malloc(AVIO_BUF_SIZE);
        if (!avio_buf) {
            loge("av_malloc error\n");
            goto fail;
        }

        avio_ctx = avio_alloc_context(avio_buf, AVIO_BUF_SIZE, 0, &io_callback, read_file, NULL, seek_file);
        if (!avio_ctx) {
            loge("avio_alloc_context error\n");
            goto fail;
        }

        fmt_ctx->pb = avio_ctx;
        url = NULL;
    } else {
        url = filename;
    }

    ret = avformat_open_input(&fmt_ctx, url, NULL, NULL);
    if (ret < 0) {
        loge("avformat_open_input error\n");
        goto fail;
    }

    ret = avformat_find_stream_info(fmt_ctx, NULL);
    if (ret < 0) {
        loge("avformat_open_input error\n");
        goto fail;
    }

    av_dump_format(fmt_ctx, 0, filename, 0);

    read_running = true;
    while (read_running) {
        ret = av_read_frame(fmt_ctx, pkt);
        if (ret == AVERROR_EOF) {
            logw("read end of file: %s\n", filename);
            break;
        }
        else if (ret < 0) {
            continue;
        }
        else {
            //logd("index:%d size:%d pts:%ld dts:%ld\n", pkt->stream_index, pkt->size, pkt->pts, pkt->dts);
            av_packet_unref(pkt);
        }
    }

fail:
    if (enable_callback) {
        close_file(io_callback.fd);
    }
    if (fmt_ctx) {
        avformat_close_input(&fmt_ctx);
    }

    logd("ffmpeg extractor exit\n")

    return 1;
}

