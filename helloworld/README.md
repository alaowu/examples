## Demo说明

* 实现用户态与内核态数据交互

## 编译

* kernel层
    
    ```
    cd helloworld/kernel
    make clean; make
    ```

* user层

    ```
    cd helloworld/user
    make clean; make
    ```

## 使用

* 将*kernel*下生成的*hello.ko*文件加载到内核中: `sudo insmod hello.ko`

* 运行*user*下的执行档`main_one`, 终端输入如下命令：
    
    'q': 退出应用
    'c': 测试close功能
    'o': 测试open功能
    'p': 测试将用户态数据传到内核态
    'g': 测试从内核态获取数据

