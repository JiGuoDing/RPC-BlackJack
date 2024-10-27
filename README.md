
# ***BlakcJack-RPC使用步骤***

```shell
# 项目根目录终端运行
$ make
```

```shell
# 一个终端
$ ./src/bin/server.o

# 另一个终端（服务器地址在本地）
$ ./src/bin/client.o localhost 
```

***需要的一些库***

1. libtirpc-dev
2. rpcsvc-proto
3. rpcbind
4. libc-dev-bin
