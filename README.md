

# 简介
dds topic list 工具  
列出当前活跃的dds topic 以及topic_type  
效果类似ros命令  `ros2 topic list -t`

       
# 依赖环境
fastdds


# 编译
```
mkdir build && cd build
cmake ..
make -j
sudo make install
```


# 使用
```bash
dds_topc
```


PS . 默认只对Domain 0 进行 discovery