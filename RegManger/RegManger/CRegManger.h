#pragma once
#include "../../publicstruct.h"

/*

ע���������
*/
class CRegManger
{
public:
    CRegManger();
    ~CRegManger();

public:
    //Key���
public:
    //Value���.
    /*
    ����: ���ݴ���� Root Subkey ��Value��ֵ. ����value������.�Լ���Ӧ����.����CBinString����.
    */
    CBinString ReGetSpecifiedKeyValue(
        IN HKEY RootKey,
        IN CBinString SubKey,
        IN CBinString  FindValueName,
        IN DWORD FindValueNameType);

    vector<CBinString> RegGetSpecifiedKeyAllKey(
        IN HKEY RootKey,
        IN CBinString SubKey
        ); //��ȡָ��Key���������Key
};

