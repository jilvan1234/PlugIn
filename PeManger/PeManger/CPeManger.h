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

//ͨ�����ͽṹ.���� ����������ַ��� RVA �������ֱ��RVA ������ű��RVA
typedef struct _EXPORT_TYPE_ADDRESSRVA
{
    DWORD ExpTypeRva;
}EXPORT_TYPE_ADDRESSRVA,*PEXPORT_TYPE_ADDRESSRVA;

typedef struct _EXPORT_FULLNAME
{
    TCHAR *FunctionName = NULL;
    TCHAR *UndName = NULL;      //���Ʒ�������ֵ.
}EXPORT_FULLNAME, *PEXPORT_FULLNAME;


typedef struct _IMPORT_IMPORTPE_FULLNAME
{
    TCHAR *ImportDllName;
}IMPORT_IMPORTPE_FULLNAME, *PIMPORT_IMPORTPE_FULLNAME;


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
    ����: �����ļ������е����.
    1.����1: �ļ�����
    2.����2: �ļ����
    3.����3: Ȩ��  �ļ���Ȩ��.�ڲ������ļ����õ�
    ����ֵ: �����ļ������е����.�����List�����.
    */
    vector<PIMAGE_IMPORT_DESCRIPTOR> *PeImportGetFileImportList(CBinString FileName, HANDLE hFile = INVALID_HANDLE_VALUE, DWORD DesireAccess = PEOPT_READ);
    
    //��ȡ�����������������DLL
    vector<PIMPORT_IMPORTPE_FULLNAME> *PeImportGetFullBaseName(CBinString FileName, HANDLE hFile = INVALID_HANDLE_VALUE, DWORD DesireAccess = PEOPT_READ);
    /**
    ����1: ��peImportGetImport���صĵ����List�ṹ
    ����ֵ: �ͷ��ڴ�,�ɹ�����TRUE,ʧ�ܷ���False
    */
    BOOL  PeFreeImportFileList(vector<PIMAGE_IMPORT_DESCRIPTOR> * pList); //�ͷ�ImportList���ڴ�.


    vector<PIMAGE_EXPORT_DIRECTORY> *PeExPortGetFileExPort(CBinString FileName, HANDLE hFile = INVALID_HANDLE_VALUE, DWORD DesireAccess = PEOPT_READ);

    //��ȡ�������е��������ֵ�RVA
    vector< PEXPORT_TYPE_ADDRESSRVA> *PeExPortGetFullFunctionNameRva(CBinString FileName, HANDLE hFile = INVALID_HANDLE_VALUE, DWORD DesireAccess = PEOPT_READ);

    //��ȡ�����������е��������ĵ�ַ���RVA
    vector< PEXPORT_TYPE_ADDRESSRVA> *PeExPortGetFullFunctionAddressRva(CBinString FileName, HANDLE hFile = INVALID_HANDLE_VALUE, DWORD DesireAccess = PEOPT_READ);


    //��ȡ���������к�������.
    vector<PEXPORT_FULLNAME> *PeExportGetFullFunctionName(CBinString FileName, HANDLE hFile = INVALID_HANDLE_VALUE, DWORD DesireAccess = PEOPT_READ);

public:
    DWORD RvAToFoA(DWORD RvA,CBinString FileName, HANDLE hFile = INVALID_HANDLE_VALUE, DWORD DesireAccess = PEOPT_READ);
    DWORD FoAToRva(DWORD FoA,CBinString FileName, HANDLE hFile = INVALID_HANDLE_VALUE, DWORD DesireAccess = PEOPT_READ);
public:
    //�����.


    //����һ����.
    //BOOL ExpansionSection(); 
    ////����һ����
    //BOOL AddSection();

    ////ɾ��һ����
    //BOOL DeleteSection();
    BOOL AddSection(CBinString FileName, HANDLE hFile = INVALID_HANDLE_VALUE);

};

