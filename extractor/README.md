[TOC]
## 说明
   该demo主要实现自定义avio callback函数并注册到ffmpeg, app层调用av_read_frame时会通过自定义的read_file以及seek_file实现文件数据的读取与跳转。

## 目的
   由于ffmpeg默认的avio buf size是32KB, avio_read的内部逻辑会判断当前要读的数据大小超过32KB就会直接从文件读取数据到packet buf, 没有超过32KB时会先读到avio buf中缓存起来，再从avio buf中拷贝到packet buf。对于码率较低的视频文件, 每个packet size较小, 一次读avio buf size的数据, 再进行拷贝, 会减少系统调用提高效率。通过自定义avio callback可以配置avio buf size大小, 针对不同码率的视频进行调整。

## 测试
   cd build
   camke ..
   make
   ./ffmpegextractor -i test.mp4 -c 1
