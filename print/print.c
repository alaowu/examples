#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "libavutil/log.h"

typedef enum {
    LOG_NONE = 0,
    LOG_E,
    LOG_W,
    LOG_I,
    LOG_D,
    LOG_V,
} loglevel_e;

int env_get_value(const char *str, int value);

#define logd(fmt, args...) \
    do { \
        int level = env_get_value("debug_level", LOG_D); \
        printf("logd level:%d\n", level); \
        if (level >= LOG_D) { \
            printf("logd shuld enter\n"); \
            av_log(NULL, AV_LOG_DEBUG, "[%s %d] " fmt, __FUNCTION__, __LINE__, ##args); \
        } \
    } while (0)

#define logi(fmt, args...) \
    do \
        if (env_get_value("debug_level", LOG_W) >= LOG_I) { \
            av_log(NULL, AV_LOG_INFO, "[%s %d] " fmt, __FUNCTION__, __LINE__, ##args); \
        } \
    while (0)

#define loge(fmt, args...) \
    do \
        if (env_get_value("debug_level", LOG_W) >= LOG_E) { \
            av_log(NULL, AV_LOG_ERROR, "[%s %d] " fmt, __FUNCTION__, __LINE__, ##args); \
        } \
    while (0)

int env_get_value(const char *str, int value)
{
    int level = value;

    if (!str) {
        return level;
    }
    const char *env_str = getenv(str);
    if (!env_str) {
        return level;
    }
    if ((strlen(env_str) == 1 && isdigit(env_str[0])) ||
        (strlen(env_str) == 2 && isdigit(env_str[0]) && isdigit(env_str[1]))) {
        level = atoi(env_str);
    }
    printf("env_str:%s value:%d\n", env_str, level);

    return level;
}

int main(int argc, char *argv[])
{
    int log_level = env_get_value("ffmpeg_level", AV_LOG_WARNING);
    av_log_set_level(log_level);
    printf("ffmpeg level:%d\n", av_log_get_level());

    logd("logd print this!\n");
    logi("logi print this!\n");
    loge("loge print this!\n");

    return 0;
}
