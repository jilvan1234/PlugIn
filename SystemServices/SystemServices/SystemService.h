#pragma once
#include <Windows.h>

/*
函数类型: 回调函数
函数作用: 控制服务状态
参数1: 传入的控制代码.比如传入0x3 SERVICE_CONTROL_CONTINUE 表示通知暂停的服务应该恢复
参数2: 已发生的时间的类型.
参数3: 传入的其它的设备信息. 个数取决于参数1根参数2的值.
参数4: 用户自定义的数据.通过 RegisterServiceCtrlHandleEx喘息 .共享服务的时候, lpContext可以帮助识别服务.
返回值: 返回值取决于收到的控制代码.
*/
DWORD WINAPI ServiceHandle(DWORD    dwControl, DWORD    dwEventType, LPVOID   lpEventData, LPVOID   lpContext);

/*
函数类型: 回调函数.由SCM服务控制器调用
函数作用: 服务的入口点
参数1:
参数2:
返回值:
*/
VOID WINAPI SerMain(DWORD   dwNumServicesArgs, LPWSTR* lpServiceArgVectors);