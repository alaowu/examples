# ARM Linux环境控制DebugLevel

## 背景

Android下有统一的属性文件, 可以通过property_get_int32类似的接口在用户层设置或者获取全局的变量。但是在Linux环境下没有全局的属性文件可以使用, 因此在用户层控制调试等级比较麻烦。

## 实现原理

Linux环境下可以设置和获取环境变量, 这是全局的可控参数, 可以利用此特性实现用户层控制调试等级。

## 测试过程

本demo利用ffmpeg av_log函数进行测试, 主要做法：

* 将av_log函数进行包装, 封成用户可控的log函数

* 通过 export ffmpeg_level=32 控制最下一层的打印等级

* 通过 export debug_level=4 控制用户层的打印等级
