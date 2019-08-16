# PlugIn
2019-8-16
添加了文件独占 保护文件。
完成了全局共享内存遍历，以及句柄相关。
添加了插件。结束IELOCK的插件。

2019-8-13 
添加了MinHook库.并且在远程线程中使用.支持86/64
添加了Detours库.但是32位可以使用.64位无法使用正在寻找问题.
其它准备写一个自己的HOOK库进行使用.

2019-8-8
添加IAT HOOK工程,还未填写.
添加FileManger工程. 管理文件.正在编写.
添加共享内存遍历. TestCode里面则是测试共享内存.添加了ShardMemory

2019-8-6
添加远程线程卸载. 远程线程ShellCode注入.封装成类.
且跨Session 注入.

2019-7-26
DLLInject:  远程线程注入 ShellCode注入 全局钩子注入
Other:  互斥体单一运行实例
DriverManger: 进程保护. 驱动占用.
MyDuiLib: 测试的Duilib程序.(未成功)
HOOK:  还未编写.
ProcessMager: 还未编写.

2019-7-22 句柄资源服务

服务 资源 句柄占用的初始框架.

