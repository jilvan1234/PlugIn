#pragma once


#include "../../../publicstruct.h"

/*
DLL ����.
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
    //�������˽ӿں���

    BOOL isFilter(CBinString DllName);

    map<string, string> GetFilterSectionNameArray() const;
    BOOL isVmpSectionName(CBinString DllName);
    

private:
     BOOL InitFilterDllArray(); //��ʼ��DLL��������
     BOOL InitFilterSectionName();
    map<CBinString,CBinString> m_PassDllArray;  //ȫ��DLL���ء�
    map <string, string> m_FilterSectionName; //�Խ����ƹ���. ����VMP
};










