#pragma once
#include <string>
using namespace std;

#ifdef UNICODE
#define MYISTRING wstring
#else
#define  STRING string
#endif // UNICODE
