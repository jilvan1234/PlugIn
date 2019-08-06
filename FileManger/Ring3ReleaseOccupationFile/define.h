#pragma once
#include <windows.h>


typedef _Return_type_success_(return >= 0) LONG NTSTATUS;
#define NT_SUCCESS(Status) (((NTSTATUS)(Status)) >= 0)

#ifdef UNICODE
#define  MYISTRING wstring
#else
#define STRING string
#endif // UNICODE
