#pragma once
#include <list>
#include <vector>

using namespace std;
/*
PeManger��Ҫ����

1.��ȡDosͷ
2.��ȡNtͷ
3.��ȡ�ļ�ͷ
4.��ȡ��ѡͷ
5.��ȡ����Ϣ
6.�����,�ϲ���.ɾ����. ɾ��������.
7.��ȡ���������
8.��ȡ����������
//��ȡ�ض�λ��
//��ȡ��Դ��.

*/
#include "../../publicstruct.h"

class CPeManger
{
public:
    CPeManger();
    ~CPeManger();

public:

    /*
    ��ȡPE��Ϣ�ĸ���ͷ.����ͨ���ļ���.Ҳ����ͨ���ļ����.
    */


    // OPT_dwDesiredAccess


private:
#define PEOPT_READ  0
#define PEOPT_WRITE 1
#define PEOPT_READ_WRITE 2                      //Ȩ�����
#define PEOPT_READ_WRITE_EXCUTE 3
public:
     DWORD PeDesireAccessGetDesiredAccessRead();
     DWORD PeDesireAccessGetDesiredAccessWrite();
     DWORD PeDesireAccessGetDesiredAccessReadWrite();
     DWORD PeDesireAccessGetDesiredAccessReadWriteExcute();
     BOOL  PeDesireInit(DWORD DesireAccess,DWORD&dwMapingDesireAccessType, DWORD&dwViewDesireAccessType, DWORD&dwFileDesireAccessType);

     //��ȡPEͷ��Ϣ���
public:
    PIMAGE_DOS_HEADER PeHederGetDosHeader(CBinString FileName,HANDLE hFile = INVALID_HANDLE_VALUE,DWORD DesireAccess = PEOPT_READ);
    PIMAGE_NT_HEADERS PeHederGetNtHeader(CBinString FileName, HANDLE hFile = INVALID_HANDLE_VALUE, DWORD DesireAccess = PEOPT_READ);
    PIMAGE_FILE_HEADER PeHederGetFileHeader(CBinString FileName, HANDLE hFile = INVALID_HANDLE_VALUE, DWORD DesireAccess = PEOPT_READ);
    PIMAGE_OPTIONAL_HEADER PeHederGetOptHeader(CBinString FileName, HANDLE hFile = INVALID_HANDLE_VALUE, DWORD DesireAccess = PEOPT_READ);
    vector<PIMAGE_SECTION_HEADER> * PeHederGetSectionHeader(CBinString FileName,HANDLE hFile = INVALID_HANDLE_VALUE, DWORD DesireAccess = PEOPT_READ);

    //��ȡPE�����
public:
    vector<PIMAGE_DATA_DIRECTORY> *PeDirGetDirectory(CBinString FileName, HANDLE hFile = INVALID_HANDLE_VALUE, DWORD DesireAccess = PEOPT_READ);//��ȡĿ¼�����.
    
public:
    //��������
    /*
    1.��ȡ���е����.���뵽������
    2.��ȡ�ļ�������е����� ģ������.
    3.��ȡ������������е����к�������,�Լ���Ӧ��IAT��ַ.
    */
    vector<PIMAGE_IMPORT_DESCRIPTOR> *PeImportGetImport(CBinString FileName, HANDLE hFile = INVALID_HANDLE_VALUE, DWORD DesireAccess = PEOPT_READ);
};

