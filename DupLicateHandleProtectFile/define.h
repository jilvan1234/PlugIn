#pragma once
#include <string>
using namespace std;

#ifdef UNICODE
#define STRING wstring
#else
#define  STRING string
#endif // UNICODE
