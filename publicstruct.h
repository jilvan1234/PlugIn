#pragma once
#include <windows.h>
#include <string>
using namespace std;

#ifdef UNICODE
#define CBinString wstring
#else
#define CBinString string
#endif

#ifdef _WIN64

#define UBinSize DWORD64
#else
#define UBinSize DWORD

#endif
