#pragma once
#include "../../publicstruct.h"

/*

注册表操作相关
*/
class CRegManger
{
public:
    CRegManger();
    ~CRegManger();

public:
    //Key相关
public:
    //Value相关.
    /*
    作用: 根据传入的 Root Subkey 找Value的值. 传入value的名字.以及对应类型.返回CBinString类型.
    */
    CBinString ReGetSpecifiedKeyValue(
        IN HKEY RootKey,
        IN CBinString SubKey,
        IN CBinString  FindValueName,
        IN DWORD FindValueNameType);

    vector<CBinString> RegGetSpecifiedKeyAllKey(
        IN HKEY RootKey,
        IN CBinString SubKey
        ); //获取指定Key下面的所有Key
};

