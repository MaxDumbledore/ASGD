# 异步随机梯度下降算法

采用C++语言编写，基于C/S模式，端口通信， 
服务器采用异步IO实现IO多路复用，
和客户端之间保持有状态的TCP长连接。

此外对于梯度实现了加密。

使用库：
1. 网络通信库ASIO;
2. libtorch，用于实现MNIST手写数字识别网络的训练;
3. Microsoft SEAL，用于实现隐私保护

主机：

1. 打开端口，接受认证请求，进行双向认证

客户端：

1. 向服务器发送认证请求

备注：如果让神经网络的训练是单线程的那么需要在运行程序之前
加上`OMP_NUM_THREADS=1`