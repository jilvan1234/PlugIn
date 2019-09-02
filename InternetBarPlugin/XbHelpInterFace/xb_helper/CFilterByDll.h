#pragma once


#include "../../../publicstruct.h"

/*
DLL 过滤.
*/
class CFilterByDll
{
public:
    CFilterByDll();
    ~CFilterByDll();

public:
    map<CBinString, CBinString>  GetPassDllArray()const;
    BOOL AddPassDllName(CBinString DllName);
public:
    //特征过滤接口函数

    BOOL isFilter(CBinString DllName);

    map<string, string> GetFilterSectionNameArray() const;
    BOOL isVmpSectionName(CBinString DllName);
    

private:
     BOOL InitFilterDllArray(); //初始化DLL拦截数组
     BOOL InitFilterSectionName();
    map<CBinString,CBinString> m_PassDllArray;  //全局DLL拦截。
    map <string, string> m_FilterSectionName; //以节名称过滤. 比如VMP
};










